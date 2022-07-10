#include "heap.h"

#include <memory>

#include <cassert>

namespace {
constexpr scd::heap::size_type byte_alignment = 8;

constexpr scd::heap::size_type align(scd::heap::size_type size) {
  return (size + (byte_alignment - 1)) & ~(byte_alignment - 1);
}

constexpr bool is_free(scd::heap::size_type size_value) {
  return size_value & (1 << (sizeof(size_value) * 8 - 1));
}

constexpr void mark_free(scd::heap::size_type& size_value) {
  size_value &= ~(1 << (sizeof(size_value) * 8 - 1));
}

constexpr void mark_used(scd::heap::size_type& size_value) {
  size_value |= (1 << (sizeof(size_value) * 8 - 1));
}
} // namespace

namespace scd {

heap::heap(size_type size) {
  _total_size = align(size);

  _pool = std::malloc(_total_size);
  std::memset(_pool, 0, _total_size);

  _find_free_memory = &heap::first_available;

  header* head = reinterpret_cast<header*>(_pool);
  head->_size  = _total_size - (sizeof(header) + sizeof(footer));
  head->_prev  = head;
  head->_next  = head;

  footer* foot
    = reinterpret_cast<footer*>(_pool + _total_size - sizeof(footer));
  foot->_size = head->_size;

  _free_head       = head;
  _last_footer     = foot;
  _bytes_available = head->_size;
}

heap::~heap() {
  if (!_leaks.empty()) {
    OutputDebugStringA("***** MEMORY LEAKS DETECTED *****\n");
    char szNumLeaks[64];
    sprintf_s(szNumLeaks, "Number of Leaks: %d\n", _leaks.size());
    OutputDebugStringA(szNumLeaks);
    char szTotalLeak[64];
    sprintf_s(szTotalLeak,
              "Total Memory Leaked: %d bytes\n",
              _total_size - _bytes_available);
    OutputDebugStringA(szTotalLeak);

    int total = _leaks.size() * (sizeof(header) + sizeof(footer));

    while (_leaks.empty() == false) {
      auto iter = _leaks.begin();
      char szLeak[256];
      sprintf_s(szLeak,
                "Leak at %d in file %s on line %d for %d bytes.\n",
                iter->first,
                iter->second._file,
                iter->second._line,
                iter->second._size);
      OutputDebugStringA(szLeak);
      total += iter->second._size;
      _leaks.erase(iter);
      // pIter++;
    }

    char szTotalSize[256];
    sprintf_s(
      szTotalSize,
      "Actual Amount of Memory Leaked: %d bytes\nDifference: %d bytes\n",
      total,
      (_total_size - _bytes_available) - total);
    OutputDebugStringA(szTotalSize);
  }

  std::free(_pool);
}

heap::header* heap::first_available(size_type size) {
  if (_free_head == nullptr || _free_head->_size == 0)
    return nullptr;

  size_type average_block_size
    = _pointer_count > 0 ? (_total_size - _bytes_available) / (_pointer_count)
                         : 0;

  if (size < average_block_size) {
    header* iter = _free_head->_next;
    do {
      if (iter->_size >= size)
        return iter;
      else
        iter = iter->_next;

    } while (iter != _free_head->_next);
  } else {
    header* iter = _free_head->_prev;
    do {
      if (iter->_size >= size)
        return iter;
      else
        iter = iter->_prev;

    } while (iter != _free_head->_prev);
  }

  // No more memory
  return nullptr;
}

void* heap::allocate(size_type size, const char* file, size_type line) {
  size         = align(size);
  header* head = first_available(size);

  if (head == nullptr) // Free Memory was not found
  {
    return nullptr;
  }

  footer* foot = reinterpret_cast<footer*>(reinterpret_cast<std::byte*>(head)
                                           + head->_size + sizeof(header));

  // Remove the whole block in case we can't split
  _bytes_available -= head->_size;
  ++_pointer_count;

  // See if there's enough leftover memory for another chunk
  if (head->_size - size > (sizeof(header) + sizeof(footer))) {
    // Split the block into smaller blocks
    size_type block_size = head->_size;

    head->_size = size;

    footer* new_foot = foot;

    foot        = reinterpret_cast<footer*>(reinterpret_cast<std::byte*>(head)
                                     + head->_size + sizeof(header));
    foot->_size = head->_size;

    header* new_head = reinterpret_cast<header*>(
      reinterpret_cast<std::byte*>(foot) + sizeof(footer));
    new_head->_size = block_size - size - (sizeof(header) + sizeof(footer));
    new_head->_next = head->_next;
    new_head->_prev = head;

    head->_next->_prev = new_head;
    head->_next        = new_head;

    if (head->_prev == head) {
      head->_prev = new_head;
    }

    new_foot->_size = new_head->_size;

    // add back the memory from the split block
    _bytes_available += block_size - (sizeof(header) + sizeof(footer) + size);
  }

  if (head == _free_head) // the memory found was the head of the list
  {
    if (head->_next != head) // the memory found is not the only
    {                        // chunk in the list
      _free_head = head->_next;
    } else {
      _free_head = nullptr; // There is no more free memory
    }
  }

  if (_free_head != nullptr) {
    head->_prev->_next = head->_next;
    head->_next->_prev = head->_prev;
  }

  head->_prev = head->_next = head;

  mark_used(head->_size);
  mark_used(foot->_size);

  void* data = reinterpret_cast<std::byte*>(head) + sizeof(header);

  // Leak Detection
  leak_detector leak;
  leak._file = file;
  leak._line = line;
  leak._size = size;
  _leaks.emplace(data, leak);

  return data;
}

void heap::deallocate(void* data) {
  // Get the header and footer for the data block and
  // Turn off the high bit to label it as free memory
  header* head = (header*)(data - sizeof(header));
  if (is_free(head->_size)) {
    return;
  } else {
    mark_free(head->_size);
  }

  footer* foot = (footer*)(data + head->_size);
  mark_free(foot->_size);

  // Clear the old memory
  memset(data, 0, head->_size);

  _bytes_available += head->_size;
  --_pointer_count;

  // Leak Detection
  _leaks.erase(data);

  // See if you can merge right
  header* next_head = (header*)((char*)foot + sizeof(footer));
  if (((char*)next_head < (char*)_last_footer)
      && is_free(next_head->_size)) {
    // Next block is free
    footer* next_foot
      = (footer*)((char*)next_head + sizeof(header) + next_head->_size);

    // Merge Right
    head->_size += next_head->_size + sizeof(header) + sizeof(footer);
    next_foot->_size = head->_size;

    foot->_size = 0;

    // Move foot pointer
    foot = next_foot;

    _bytes_available += sizeof(header) + sizeof(footer);

    _free_head = next_head->_next;

    if (next_head == _free_head) {
      _free_head = nullptr;
    } else {
      next_head->_next->_prev = next_head->_prev;
      next_head->_prev->_next = next_head->_next;
    }
  }

  // See if you can merge left
  footer* prev_foot = (footer*)((char*)head - sizeof(footer));

  if (((char*)prev_foot > _pool) && is_free(prev_foot->_size)) {
    // Previous block is free
    header* prev_head
      = (header*)((char*)prev_foot - prev_foot->_size - sizeof(header));

    // Merge Left
    prev_head->_size += head->_size + sizeof(header) + sizeof(footer);
    foot->_size = prev_head->_size;

    // Clear old Memory
    prev_foot->_size = 0;
    head->_next = head->_prev = nullptr;
    head->_size               = 0;

    // Reset head Pointer
    head = prev_head;

    _bytes_available += sizeof(header) + sizeof(footer);

    return;
  }

  // Block did not merge left, reinsert into list
  if (_free_head != nullptr) {
    // Set the head's prev (left)
    head->_prev        = _free_head->_prev;
    head->_prev->_next = head;

    // Set the head's next (right)
    head->_next        = _free_head;
    head->_next->_prev = head;
  } else {
    _free_head = head;
  }
}

} // namespace scd
