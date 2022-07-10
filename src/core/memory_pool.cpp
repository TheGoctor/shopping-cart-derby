#include "MemoryPool.h"

#include <iostream>
#include <memory>

#include <cstdlib>

namespace {
constexpr size_t byte_alignment = 8;
constexpr size_t ByteAlign(size_t size) {
  return (size + (byte_alignment - 1)) & ~(byte_alignment - 1);
}
constexpr size_t NodeSize
  = sizeof(scd::MemoryPool::Header) + sizeof(scd::MemoryPool::Footer);
constexpr size_t FreeMask = (1 << (sizeof(size_t) * 8 - 1));
} // anonymous namespace

scd::MemoryPool::MemoryPool(size_t pool_size) {
  _pool = std::calloc(1, pool_size);
}

scd::MemoryPool::~MemoryPool() {
  if (_ptr_list.empty() == false) {
    std::cerr << "***** MEMORY LEAKS DETECTED *****\n"
              << "Number of leaks: " << _ptr_list.size() << "\n"
              << "Total memory leaked: " << GetMemUsed() << " bytes\n";

    size_t total = _ptr_list.size() * NodeSize;
    for (auto& leak : _ptr_list) {
      std::cerr << "Leaked " << leak.second.size << " bytes at " << leak.first
                << " (" << leak.second.file << ":" << leak.second.line << ")\n";

      total += leak.second.size;
    }

    std::cerr << "Actual memory leaked: " << total << " bytes\n"
              << "Difference: " << GetMemUsed() - total << " bytes.\n";
  }

  std::free(_pool);
}

bool scd::MemoryPool::Init(size_t pool_size) {
  _total_size = ByteAlign(pool_size);

  _pool = calloc(1, _total_size);

  if (nullptr == _pool)
    return false;

  FindFreeMemory = &scd::MemoryPool::FirstAvailable;

  _available_memory = _total_size - NodeSize;

  _free_head       = (Header*)_pool;
  _free_head->size = _available_memory;
  _free_head->prev = _free_head;
  _free_head->next = _free_head;

  _footer       = (Footer*)((char*)_pool + _available_memory + sizeof(Header));
  _footer->size = _available_memory;

  return true;
}

scd::MemoryPool::Header*
  scd::MemoryPool::FirstAvailable(size_t alloc_size) const {
  if (_free_head == nullptr || _free_head->size == 0)
    return nullptr;

  size_t avg_alloc_size
    = _ptr_count > 0 ? (_total_size - _available_memory) / (_ptr_count) : 0;

  if (alloc_size < avg_alloc_size) {
    Header* iter = _free_head->next;
    do {
      if (iter->size >= alloc_size)
        return iter;
      else
        iter = iter->next;

    } while (iter != _free_head->next);
  } else {
    Header* iter = _free_head->prev;
    do {
      if (iter->size >= alloc_size)
        return iter;
      else
        iter = iter->prev;

    } while (iter != _free_head->prev);
  }

  // No More Memory
  return nullptr;
}

bool scd::MemoryPool::IsFree(Header* header) const {
  return (header->size & FreeMask) == 0;
}

void* scd::MemoryPool::Allocate(size_t size, char* file, unsigned int line) {
  size         = ByteAlign(size);
  Header* head = FirstAvailable(size);

  if (head == nullptr) // Free Memory was not found
  {
    return nullptr;
  }

  Footer* foot = (Footer*)((char*)head + head->size + sizeof(Header));

  // Remove the whole block in case we can't split
  _available_memory -= head->size;
  ++_ptr_count;

  // See if there's enough leftover memory for another chunk
  if (head->size - size > NodeSize) {
    // Split the block into smaller blocks
    size_t block_size = head->size;

    head->size = size;

    Footer* new_foot = foot;

    foot       = (Footer*)((char*)head + head->size + sizeof(Header));
    foot->size = head->size;

    Header* new_head = (Header*)((char*)foot + sizeof(Footer));
    new_head->size   = block_size - size - NodeSize;
    new_head->next   = head->next;
    new_head->prev   = head;

    head->next->prev = new_head;
    head->next       = new_head;

    if (head->prev == head) {
      head->prev = new_head;
    }

    new_foot->size = new_head->size;

    // add back the memory from the split block
    _available_memory += block_size - (NodeSize + size);
  }

  if (head == _free_head) {
    // the memory found was the head of the list
    if (head->next != head) {
      // the memory found is not the only chunk in the list
      _free_head = head->next;
    } else {
      // There is no more free memory
      _free_head = nullptr;
    }
  }

  if (_free_head != nullptr) {
    head->prev->next = head->next;
    head->next->prev = head->prev;
  }

  head->prev = head->next = head;

  head->size |= (1 << 31);

  foot->size |= (1 << 31);

  char* ptr = (char*)head + sizeof(Header);

  // Leak Detection
  Allocation tLeak;
  tLeak.file             = file;
  tLeak.line             = line;
  tLeak.size             = size;
  _ptr_list[(size_t)ptr] = tLeak;

  return ptr;
}

void scd::MemoryPool::Deallocate(void* ptr) {
  // Get the header and footer for the data block and
  // Turn off the high bit to label it as free memory
  Header* head = (Header*)((char*)ptr - sizeof(Header));
  if (IsFree(head)) {
    return;
  } else {
    head->size &= ~(1 << 31);
  }

  Footer* foot = (Footer*)((char*)ptr + head->size);
  foot->size &= ~(1 << 31);

  // Clear the old memory
  memset(ptr, 0, head->size);

  _available_memory += head->size;
  --_ptr_count;

  // Leak Detection
  _ptr_list.erase((size_t)ptr);

  // See if you can merge right
  Header* next_head = (Header*)((char*)foot + sizeof(Footer));
  if (((char*)next_head < (char*)_footer) && IsFree(next_head)) {
    // Next block is free
    Footer* next_foot
      = (Footer*)((char*)next_head + sizeof(Header) + next_head->size);

    // Merge Right
    head->size += next_head->size + NodeSize;
    next_foot->size = head->size;

    foot->size = 0;

    // Move foot pointer
    foot = next_foot;

    _available_memory += NodeSize;

    _free_head = next_head->next;

    if (next_head == _free_head) {
      _free_head = nullptr;
    } else {
      next_head->next->prev = next_head->prev;
      next_head->prev->next = next_head->next;
    }
  }

  // See if you can merge left
  Footer* prev_foot = (Footer*)((char*)head - sizeof(Footer));

  if (((char*)prev_foot > _pool) && (prev_foot->size & (1 << 31)) == 0) {
    // Previous block is free
    Header* prev_head
      = (Header*)((char*)prev_foot - prev_foot->size - sizeof(Header));

    // Merge Left
    prev_head->size += head->size + NodeSize;
    foot->size = prev_head->size;

    // Clear old Memory
    prev_foot->size = 0;
    head->next = head->prev = nullptr;
    head->size              = 0;

    // Reset head Pointer
    head = prev_head;

    _available_memory += NodeSize;

    return;
  }

  // Block did not merge left, reinsert into list
  if (_free_head != nullptr) {
    // Set the head's prev (left)
    head->prev       = _free_head->prev;
    head->prev->next = head;

    // Set the head's next (right)
    head->next       = _free_head;
    head->next->prev = head;
  } else {
    _free_head = head;
  }
}
