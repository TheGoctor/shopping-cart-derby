#include "memory_buffer_pool.h"

namespace scd {
namespace pmr {

memory_buffer_pool::memory_buffer_pool() {
  m_pchPool = NULL;
  m_ptEndPoolFooter = NULL;
  m_ptFreeHead = NULL;
  m_nTotalPoolSize = 0;

  m_nMemoryAvailable = 0;
  m_nNumPtrs = 0;
}

memory_buffer_pool::~memory_buffer_pool() {
  if (m_cLeakList.empty() == false) {
    OutputDebugStringA("***** MEMORY LEAKS DETECTED *****\n");
    char szNumLeaks[64];
    sprintf_s(szNumLeaks, "Number of Leaks: %d\n", m_cLeakList.size());
    OutputDebugStringA(szNumLeaks);
    char szTotalLeak[64];
    sprintf_s(szTotalLeak, "Total Memory Leaked: %d bytes\n",
              m_nTotalPoolSize - m_nMemoryAvailable);
    OutputDebugStringA(szTotalLeak);

    // map<unsigned int, TLeakDetector>::iterator pIter;
    // pIter = m_cLeakList.begin();
    // while(pIter != m_cLeakList.end())
    int nTotal = m_cLeakList.size() * (sizeof(Header) + sizeof(TFooter));
    while (m_cLeakList.empty() == false) {
      map<unsigned int, TLeakDetector>::iterator pIter = m_cLeakList.begin();
      char szLeak[256];
      sprintf_s(szLeak, "Leak at %d in file %s on line %d for %d bytes.\n",
                pIter->first, pIter->second.szFile, pIter->second.nLine,
                pIter->second.nSize);
      OutputDebugStringA(szLeak);
      nTotal += pIter->second.nSize;
      m_cLeakList.erase(pIter);
      // pIter++;
    }
    char szTotalSize[256];
    sprintf_s(
        szTotalSize,
        "Actual Amount of Memory Leaked: %d bytes\nDifference: %d bytes\n",
        nTotal, (m_nTotalPoolSize - m_nMemoryAvailable) - nTotal);
    OutputDebugStringA(szTotalSize);
  }

  free(m_pchPool);
}

////////////////////////////////////////////////////////////////////////////////
// Init()	:	Initializes the Heap Object
//
// Ins		:	nPoolSizeInBytes	-	The total pool size of
// the heap in bytes
//
// Outs		:	void
//
// Returns	:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
void memory_buffer_pool::Init(unsigned int nPoolSizeInBytes) {
  m_nTotalPoolSize = (nPoolSizeInBytes + (BYTE_ALIGN - 1)) & ~(BYTE_ALIGN - 1);

  m_pchPool = (char *)malloc(m_nTotalPoolSize);
  memset(m_pchPool, 0, m_nTotalPoolSize);

  FindFreeMemory = &memory_buffer_pool::FirstAvailable;

  Header *ptHead = (Header *)m_pchPool;
  ptHead->m_size = m_nTotalPoolSize - (sizeof(Header) + sizeof(TFooter));
  ptHead->m_ptPrev = ptHead;
  ptHead->m_ptNext = ptHead;

  TFooter *ptFoot = (TFooter *)(m_pchPool + ptHead->m_size + sizeof(Header));
  ptFoot->m_nSize = ptHead->m_size;

  m_ptFreeHead = ptHead;
  m_ptEndPoolFooter = ptFoot;
  m_nMemoryAvailable = m_ptFreeHead->m_size;
}

////////////////////////////////////////////////////////////////////////////////
// FirstAvailable()	:	Finds the first available chunk of memory large
// enough
//						to hold the size passed in
//
// Ins				:	nAllocSize	-	The amount of
// memory to allocate
//
// Outs				:	void
//
// Returns			:	char*	-	A pointer to the memory
// allocated
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
memory_buffer_pool::Header *memory_buffer_pool::FirstAvailable(unsigned int nAllocSize) {
  if (m_ptFreeHead == NULL || m_ptFreeHead->m_size == 0)
    return NULL;

  unsigned int nAvgMemSize =
      m_nNumPtrs > 0 ? (m_nTotalPoolSize - m_nMemoryAvailable) / (m_nNumPtrs)
                     : 0;

  if (nAllocSize < nAvgMemSize) {
    Header *ptIter = m_ptFreeHead->m_ptNext;
    do {
      if (ptIter->m_size >= nAllocSize)
        return ptIter;
      else
        ptIter = ptIter->m_ptNext;

    } while (ptIter != m_ptFreeHead->m_ptNext);
  } else {
    Header *ptIter = m_ptFreeHead->m_ptPrev;
    do {
      if (ptIter->m_size >= nAllocSize)
        return ptIter;
      else
        ptIter = ptIter->m_ptPrev;

    } while (ptIter != m_ptFreeHead->m_ptPrev);
  }

  // No More Memory
  throw;
  // return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Allocate()	:	Allocates a section of memory using the Allocator class
//
// Ins			:	nAllocSize	-	The amount of memory to
// allocate
//
// Outs			:	void
//
// Returns		:	char*	-	A pointer to the memory
// allocated
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
char *memory_buffer_pool::Allocate(unsigned int nAllocSize, char *szFile,
                                   unsigned int nLine) {
  // Byte Align
  nAllocSize = (nAllocSize + (BYTE_ALIGN - 1)) & ~(BYTE_ALIGN - 1);
  Header *ptHead = FirstAvailable(nAllocSize);

  if (ptHead == NULL) // Free Memory was not found
  {
    return NULL;
  }

  TFooter *ptFoot =
      (TFooter *)((char *)ptHead + ptHead->m_size + sizeof(Header));

  // Remove the whole block in case we can't split
  m_nMemoryAvailable -= ptHead->m_size;
  ++m_nNumPtrs;

  // See if there's enough leftover memory for another chunk
  if (ptHead->m_size - nAllocSize > (sizeof(Header) + sizeof(TFooter))) {
    // Split the block into smaller blocks
    unsigned int nBlockSize = ptHead->m_size;

    ptHead->m_size = nAllocSize;

    TFooter *ptNewFoot = ptFoot;

    ptFoot = (TFooter *)((char *)ptHead + ptHead->m_size + sizeof(Header));
    ptFoot->m_nSize = ptHead->m_size;

    Header *ptNewHead = (Header *)((char *)ptFoot + sizeof(TFooter));
    ptNewHead->m_size =
        nBlockSize - nAllocSize - (sizeof(Header) + sizeof(TFooter));
    ptNewHead->m_ptNext = ptHead->m_ptNext;
    ptNewHead->m_ptPrev = ptHead;

    ptHead->m_ptNext->m_ptPrev = ptNewHead;
    ptHead->m_ptNext = ptNewHead;

    if (ptHead->m_ptPrev == ptHead) {
      ptHead->m_ptPrev = ptNewHead;
    }

    ptNewFoot->m_nSize = ptNewHead->m_size;

    // add back the memory from the split block
    m_nMemoryAvailable +=
        nBlockSize - (sizeof(Header) + sizeof(TFooter) + nAllocSize);
  }

  if (ptHead == m_ptFreeHead) // the memory found was the head of the list
  {
    if (ptHead->m_ptNext != ptHead) // the memory found is not the only
    {                               // chunk in the list
      m_ptFreeHead = ptHead->m_ptNext;
    } else {
      m_ptFreeHead = NULL; // There is no more free memory
    }
  }

  if (m_ptFreeHead != NULL) {
    ptHead->m_ptPrev->m_ptNext = ptHead->m_ptNext;
    ptHead->m_ptNext->m_ptPrev = ptHead->m_ptPrev;
  }

  ptHead->m_ptPrev = ptHead->m_ptNext = ptHead;

  ptHead->m_size |= (1 << 31);

  ptFoot->m_nSize |= (1 << 31);

  char *pMemPtr = (char *)ptHead + sizeof(Header);

  // Leak Detection
  TLeakDetector tLeak;
  tLeak.szFile = szFile;
  tLeak.nLine = nLine;
  tLeak.nSize = nAllocSize;
  m_cLeakList[(unsigned int)pMemPtr] = tLeak;

  return pMemPtr;
}

////////////////////////////////////////////////////////////////////////////////
// DeAllocate()	:	Deallocates a section of memory using the Allocator
// class
//
// Ins			:	pchData	-	A pointer to the block of memory
// to deallocate
//
// Outs			:	void
//
// Returns		:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
void memory_buffer_pool::DeAllocate(char *pchData) {
  // Get the header and footer for the data block and
  // Turn off the high bit to label it as free memory
  Header *ptHead = (Header *)(pchData - sizeof(Header));
  if ((ptHead->m_size & (1 << 31)) == 0) {
    return;
  } else {
    ptHead->m_size &= ~(1 << 31);
  }

  TFooter *ptFoot = (TFooter *)(pchData + ptHead->m_size);
  ptFoot->m_nSize &= ~(1 << 31);

  // Clear the old memory
  memset(pchData, 0, ptHead->m_size);

  m_nMemoryAvailable += ptHead->m_size;
  --m_nNumPtrs;

  // Leak Detection
  m_cLeakList.erase((unsigned int)pchData);

  // See if you can merge right
  Header *ptNextHead = (Header *)((char *)ptFoot + sizeof(TFooter));
  if (((char *)ptNextHead < (char *)m_ptEndPoolFooter) &&
      (ptNextHead->m_size & (1 << 31)) == 0) {
    // Next block is free
    TFooter *ptNextFoot =
        (TFooter *)((char *)ptNextHead + sizeof(Header) + ptNextHead->m_size);

    // Merge Right
    ptHead->m_size += ptNextHead->m_size + sizeof(Header) + sizeof(TFooter);
    ptNextFoot->m_nSize = ptHead->m_size;

    ptFoot->m_nSize = 0;

    // Move foot pointer
    ptFoot = ptNextFoot;

    m_nMemoryAvailable += sizeof(Header) + sizeof(TFooter);

    m_ptFreeHead = ptNextHead->m_ptNext;

    if (ptNextHead == m_ptFreeHead) {
      m_ptFreeHead = NULL;
    } else {
      ptNextHead->m_ptNext->m_ptPrev = ptNextHead->m_ptPrev;
      ptNextHead->m_ptPrev->m_ptNext = ptNextHead->m_ptNext;
    }
  }

  // See if you can merge left
  TFooter *ptPrevFoot = (TFooter *)((char *)ptHead - sizeof(TFooter));

  if (((char *)ptPrevFoot > m_pchPool) &&
      (ptPrevFoot->m_nSize & (1 << 31)) == 0) {
    // Previous block is free
    Header *ptPrevHead =
        (Header *)((char *)ptPrevFoot - ptPrevFoot->m_nSize - sizeof(Header));

    // Merge Left
    ptPrevHead->m_size += ptHead->m_size + sizeof(Header) + sizeof(TFooter);
    ptFoot->m_nSize = ptPrevHead->m_size;

    // Clear old Memory
    ptPrevFoot->m_nSize = 0;
    ptHead->m_ptNext = ptHead->m_ptPrev = NULL;
    ptHead->m_size = 0;

    // Reset head Pointer
    ptHead = ptPrevHead;

    m_nMemoryAvailable += sizeof(Header) + sizeof(TFooter);

    return;
  }

  // Block did not merge left, reinsert into list
  if (m_ptFreeHead != NULL) {
    // Set the head's prev (left)
    ptHead->m_ptPrev = m_ptFreeHead->m_ptPrev;
    ptHead->m_ptPrev->m_ptNext = ptHead;

    // Set the head's next (right)
    ptHead->m_ptNext = m_ptFreeHead;
    ptHead->m_ptNext->m_ptPrev = ptHead;
  } else {
    m_ptFreeHead = ptHead;
  }
}
} // namespace pmr
} // namespace scd
