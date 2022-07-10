////////////////////////////////////////////////////////////////////////////////
//	File			:	MemoryManager.h
//	Date			:	3/29/11
//	Mod. Date		:	3/29/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Manages Heap objects and keeps track of memory.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "enums.h"
#include "memory/allocator.h"
#include "memory/heap.h"

// Macros To make Memory Init more readable
#define MiB *1024 KB
#define KiB *1024

// Memory Macros for Explicit Heaps
#define MMNEWEX(Type, ID)                                                      \
  new (CMemoryManager::GetInstance()->Allocate(                                \
    sizeof(Type), ID, __FILE__, __LINE__)) Type
// Memory Macros for Arrays in an Explicit Heap
#define MMNEWARRAYEX(Type, Count, ID)                                          \
  NewArray<Type>(Count, ID, __FILE__, __LINE__)

// Delete macros for explicit heaps
#define MMDELEX(Ptr, ID) MyDelete(Ptr, ID)
#define MMDELARRAYEX(Ptr, ID) DelArray(Ptr, ID)

// Memory Macros for General Heap
#define MMNEW(Type) MMNEWEX(Type, HEAPID_GENERAL)
#define MMDEL(Ptr) MyDelete(Ptr, HEAPID_GENERAL)

// Memory Macros for Event Heap
#define MMNEWEVENT(Type) MMNEWEX(Type, HEAPID_EVENT)
#define MMDELEVENT(Ptr) MyDelete(Ptr, HEAPID_EVENT)

// Function for deleting pointers
template <typename TYPE>
void MyDelete(TYPE* pPtr, EHeapID nHeap) {
  if (pPtr) {
    pPtr->~TYPE();
    CMemoryManager::GetInstance()->Deallocate((char*)pPtr, nHeap);
  }
}

template <typename TYPE>
TYPE* NewArray(unsigned int nCount, EHeapID nHeap, char* szFile, int nLine) {
  unsigned int nTypeSize = sizeof(TYPE);

  // Allocate memory with added space for typesize and count
  unsigned int* pNewMem
    = (unsigned int*)CMemoryManager::GetInstance()->Allocate(
      nTypeSize * nCount + sizeof(nTypeSize) + sizeof(nCount),
      nHeap,
      szFile,
      nLine);

  // Save the size of the data type
  pNewMem[0] = nTypeSize;

  // Save the number of elements
  pNewMem[1] = nCount;

  // Loop and call placement new on each index
  char* pIter = (char*)&(pNewMem[2]);
  for (unsigned int n = 0; n < nCount; n++) {
    new (pIter) TYPE();
    pIter += nTypeSize;
  }

  // Return the pointer to the actual memory after the size data
  return (TYPE*)&(pNewMem[2]);
}

// Function for deleting arrays
template <typename TYPE>
void DelArray(TYPE* pPtr, EHeapID nHeap) {
  if (pPtr) {
    char* pIter = (char*)pPtr;

    // Get the size of the orignal type
    unsigned int nTypeSize
      = *((unsigned int*)(pIter - sizeof(unsigned int) * 2));

    // Get the number of elements
    unsigned int nCount = *((unsigned int*)(pIter - sizeof(unsigned int)));

    // Loop through the elements and call MMDEL on them
    for (unsigned int n = 0; n < nCount; n++) {
      ((TYPE*)pIter)->~TYPE();
      pIter += nTypeSize;
    }

    pIter = (char*)pPtr;
    // Delete the whole array
    MMDELEX(pIter - 8, nHeap); // -8 to get beginning of array
  }
}

class memory_manager {
  // list of heaps
  heap m_cGeneralHeap; // Used for Overall Uses
  heap m_cEventHeap;   // Used specifically for Events and the ID Gen
  heap m_cRenderHeap;  // Used primarily for rendering containers

  /**
   * Initializes the memory manager by setting the heap sizes for each of the
   * heaps.
   *
   * @param[in] general_heap_size The size of the General Heap
   * @param[in] event_heap_size The size of the Event Heap
   * @param[in] render_heap_size The size of the Render Heap
   */
  memory_manager(heap::size_type general_heap_size,
                 heap::size_type event_heap_size,
                 heap::size_type render_heap_size);

  // Trilogy of Evil
  ~memory_manager();
  memory_manager(const memory_manager&);
  memory_manager& operator=(const memory_manager&);

public:
  // Accessors
  unsigned int GetGHeapMemUsed() { return m_cGeneralHeap.GetMemUsed(); }
  unsigned int GetGHeapMemSize() { return m_cGeneralHeap.GetPoolSize(); }
  unsigned int GetEHeapMemUsed() { return m_cEventHeap.GetMemUsed(); }
  unsigned int GetEHeapMemSize() { return m_cEventHeap.GetPoolSize(); }
  unsigned int GetRHeapMemUsed() { return m_cRenderHeap.GetMemUsed(); }
  unsigned int GetRHeapMemSize() { return m_cRenderHeap.GetPoolSize(); }
  unsigned int GetDHeapMemUsed() { return m_cDynArrayHeap.GetMemUsed(); }
  unsigned int GetDHeapMemSize() { return m_cDynArrayHeap.GetPoolSize(); }

  /**
   * This function allocates memory of the specified size in the specified heap.
   * Returns nullptr if not enough memory is available.
   *
   * @param[in] size The amount of memory to allocate
   * @param[in] heap_id The id of the Heap to allocate to
   *
   * @return The pointer to the memory allocated
   */
  void* allocate(heap::size_type size, heap_type heap_id);
  void* allocate(heap::size_type size, heap_type heap_id, const char* file,
                 int line);

  /**
   * Deallocates the memory at the passed in pointer in the specified heap.
   *
   * @param[in] data The pointer to deallocate
   * @param[in] heap_id The index of the heap to deallocate from
   */
  void deallocate(void* data, heap_id heap_id);
};
