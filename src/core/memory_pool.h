/** @file MemoryPool.h
 *  @brief Allocates a pool of memory for use with groups of objects.
 *
 *  @author Mac Reichelt
 *  @copyright (c) 2016 Mac Reichelt
 */
#ifndef __scd_memory_pool_h__
#define __scd_memory_pool_h__

#include <memory_resource>
#include <unordered_map>

namespace scd {

using memory_pool = std::pmr::unsynchronized_pool_resource;

class MemoryPool {
public:
  MemoryPool(size_t pool_size);
  ~MemoryPool();

  size_t bytes_used() const { return _total_size - _available_memory; }
  size_t size() const { return _total_size; }

  /** Initializes the MemoryPool by allocating its memory.
   *
   * @param pool_size The size of the pool in bytes.
   *
   * @return Whether the pool allocation was successful.
   */
  bool init(size_t pool_size);

  /** Allocates a block of memory from the memory pool.
   *
   * @param size The size of the block to allocate in bytes.
   * @param file The name of the file from which this method was called.
   * @param line The line number from which this method was called.
   *
   * @return A pointer to the allocated block or `nullptr` if no block is
   * available.
   */
  void* allocate(size_t size, char* file, unsigned int line);

  /** Reclaims the memory allocated at the pointer for reuse in the pool.
   *
   * @param data The pointer to reclaim. This pointer must have been initially
   * returned from MemoryPool::Allocate.
   */
  void deallocate(void* data);

  struct header {
    // How much memory is free INSIDE the block : excludes header and footer
    // size
    // High order bit of 1 == used memory block
    // High order bit of 0 == free memory block
    size_t size;
    // Pointer to the previous free header in the Cyclic Doubly Linked List
    header* prev;
    // Pointer to the next free header in the Cyclic Doubly Linked List
    header* next;
  };

  struct footer {
    // How much memory is free INSIDE the block : excludes header and footer
    // size

    // High order bit of 1 == used memory block
    // High order bit of 0 == free memory block
    size_t size;
  };

  typedef header* (MemoryPool::*AllocationMethod)(size_t alloc_size) const;

private:
  // Entire Memory Pool
  void* _pool = nullptr;
  // The Last footer
  footer* _footer = nullptr;
  // The Entry Point to the free list (Cyclic Doubly-Linked List)
  header* _free_head = nullptr;
  // How much memory was allocated for this pool.
  size_t _total_size = 0;
  // Function pointer for the allocation behavior
  AllocationMethod FindFreeMemory;

  // Allocation Behaviors
  header* FirstAvailable(size_t alloc_size) const;

  bool IsFree(header* header) const;

  // For Debugging Purposes
  size_t _available_memory = 0;
  size_t _ptr_count = 0;

  // Struct used in the detection of memory leaks within the manager
  struct allocation {
    char* file;        // File where the memory was allocated
    unsigned int line; // Line the memory was allocated on
    size_t size;       // The memory of allocated
  };

  // List of all the Memory currently allocated
  std::unordered_map<size_t, allocation> _ptr_list;
};

} // namespace scd

#endif // __scd_memory_pool_h__
