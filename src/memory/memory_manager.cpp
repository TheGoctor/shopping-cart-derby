////////////////////////////////////////////////////////////////////////////////
//	File			:	MemoryManager.h
//	Date			:	3/29/11
//	Mod. Date		:	3/29/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Manages Heap objects and keeps track of memory.
////////////////////////////////////////////////////////////////////////////////

#include "memory_manager.h"

void memory_manager::memory_manager(heap::size_type general_heap_size,
                                    heap::size_type event_heap_size,
                                    heap::size_type render_heap_size)
  : _general_heap(general_heap_size)
  , _event_heap(event_heap_size)
  , _render_heap(render_heap_size) {}

void memory_manager::deallocate(void* data, heap_id heap_id) {
  if (data == nullptr)
    return;

  switch (eHeapIndx) {
  case HEAPID_GENERAL: {
    m_cGeneralHeap.DeAllocate(pchData);
  } break;
  case HEAPID_EVENT: {
    m_cEventHeap.DeAllocate(pchData);
  } break;
  case HEAPID_RENDER: {
    m_cRenderHeap.DeAllocate(pchData);
  } break;
  default: {
    // There was an error
  } break;
  };
}

void* memory_manager::allocate(heap::size_type size, heap_id heap_id,
                               const char* file, int line) {
  switch (heap_id) {
  case HEAPID_GENERAL: {
    return m_cGeneralHeap.Allocate(nAllocSize, szFile, nLine);
  } break;
  case HEAPID_EVENT: {
    return m_cEventHeap.Allocate(nAllocSize, szFile, nLine);
  } break;
  case HEAPID_RENDER: {
    return m_cRenderHeap.Allocate(nAllocSize, szFile, nLine);
  } break;
  default: {
    // There was an error
    return nullptr;
  } break;
  };
}

void* memory_manager::allocate(heap::size_type size, heap_id heap_id) {
  switch (heap_id) {
  case HEAPID_GENERAL: {
    return m_cGeneralHeap.Allocate(nAllocSize);
  } break;
  case HEAPID_EVENT: {
    return m_cEventHeap.Allocate(nAllocSize);
  } break;
  case HEAPID_RENDER: {
    return m_cRenderHeap.Allocate(nAllocSize);
  } break;
  default: {
    // There was an error
    return nullptr;
  } break;
  };
}
