/**
 * @file heap.h
 *
 * @author Mac Reichelt
 *
 * @brief Chunks of memory that can be allocated and used as dynamic memory
 */

#pragma once

#include <map>

namespace scd {
class heap {
public:
  using size_type = std::size_t;

  struct header {
    // How much memory is free INSIDE the block : excludes header and footer
    // size High order bit of 1 == used memory block High order bit of 0 == free
    // memory block
    size_type _size;
    // Pointer to the previous free header in the Cyclic Doubly Linked List
    header* _prev;
    // Pointer to the next free header in the Cyclic Doubly Linked List
    header* _next;
  };

  struct footer {
    // How much memory is free INSIDE the block : excludes header and footer
    // size High order bit of 1 == used memory block High order bit of 0 == free
    // memory block
    size_type _size;
  };

  typedef header* (heap::*allocation_method)(size_type size);


private:
  // Entire Memory Pool
  void* _pool = nullptr;

  // The Last footer
  footer* _last_footer = nullptr;

  // The Entry Point to the free list (Cyclic Doubly-Linked List)
  header* _free_head = nullptr;

  // How much memory was allocated for this pool.
  size_type _total_size = 0;

  // Function pointer for the allocation behavior
  allocation_method _find_free_memory;

  // Allocation Behaviors
  header* first_available(size_type size);

  // For Debugging Purposes
  size_type _bytes_available = 0;
  size_type _pointer_count = 0;

  // Struct used in the detection of memory leaks within the manager
  struct leak_detector {
    size_type _size; // The memory of allocated
    size_type _line; // Line the memory was allocated on
    const char* _file; // File where the memory was allocated
  };

  // List of all the Memory currently allocated
  std::map<void*, leak_detector> _leaks;

public:
  /**
   * Initializes the Heap Object
   *
   * @param[in] size The total pool size of the heap in bytes
   */
  heap(size_type size);

  // Trilogy of Evil
  ~heap();
  heap(const heap&) = delete;
  heap& operator=(const heap&) = delete;

  // Accessors
  size_type bytes_allocated() const { return _total_size - _bytes_available; }
  size_type total_size() const { return _total_size; }

  /**
   * Allocates a section of memory using the Allocator class
   *
   * @param[in] nAllocSize The amount of memory to allocate
   *
   * @return A pointer to the memory allocated
   */
  void* allocate(size_type size);

  /**
   * Allocates a section of memory using the Allocator class
   *
   * @param[in] size The number of bytes to allocate.
   * @param[in] file The file making the allocation.
   * @param[in] line The line of the file where the allocation call was made.
   *
   * @return A pointer to the memory allocated.
   */
  void* allocate(size_type size, const char* file, size_type line);

  /**
   * Adds the memory allocated at the passed in pointer back to the pool of free
   * memory.
   *
   * @param[in] data The pointer to deallocate.
   */
  void deallocate(void* data);
};

} // namespace scd
