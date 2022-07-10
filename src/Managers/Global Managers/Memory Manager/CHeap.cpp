#include <windows.h>
#define LEAN_AND_MEAN

#include <malloc.h>
#include <assert.h>
#include "CHeap.h"

#define NULL 0
#define BYTE_ALIGN 8

#if MM_LEAK_DETECTION
// This version of the functions use Leak Detection

CHeap::CHeap()
{
	m_pchPool = NULL;
	m_ptEndPoolFooter = NULL;
	m_ptFreeHead = NULL;
	m_nTotalPoolSize = 0;

	m_nMemoryAvailable = 0;
	m_nNumPtrs = 0;
}

CHeap::~CHeap()
{	
	if(m_cLeakList.empty() == false)
	{
		OutputDebugStringA("***** MEMORY LEAKS DETECTED *****\n");
		char szNumLeaks[64];
		sprintf_s(szNumLeaks, "Number of Leaks: %d\n", m_cLeakList.size());
		OutputDebugStringA(szNumLeaks);
		char szTotalLeak[64];
		sprintf_s(szTotalLeak, "Total Memory Leaked: %d bytes\n", m_nTotalPoolSize-m_nMemoryAvailable);
		OutputDebugStringA(szTotalLeak);

		//map<unsigned int, TLeakDetector>::iterator pIter;
		//pIter = m_cLeakList.begin();
		//while(pIter != m_cLeakList.end())
		int nTotal = m_cLeakList.size() * (sizeof(THeader)+sizeof(TFooter));
		while(m_cLeakList.empty() == false)
		{
			map<unsigned int, TLeakDetector>::iterator pIter = m_cLeakList.begin();
			char szLeak[256];
			sprintf_s(szLeak, "Leak at %d in file %s on line %d for %d bytes.\n",
				pIter->first, pIter->second.szFile, pIter->second.nLine, pIter->second.nSize);
			OutputDebugStringA(szLeak);
			nTotal += pIter->second.nSize;
			m_cLeakList.erase(pIter);
			//pIter++;
		}
		char szTotalSize[256];
		sprintf_s(szTotalSize, "Actual Amount of Memory Leaked: %d bytes\nDifference: %d bytes\n",
			nTotal, (m_nTotalPoolSize-m_nMemoryAvailable) - nTotal);
		OutputDebugStringA(szTotalSize);
	}

	free(m_pchPool);
}

////////////////////////////////////////////////////////////////////////////////
// Init()	:	Initializes the Heap Object
//
// Ins		:	nPoolSizeInBytes	-	The total pool size of the heap in bytes
//
// Outs		:	void
//
// Returns	:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
void CHeap::Init(unsigned int nPoolSizeInBytes)
{
	m_nTotalPoolSize = (nPoolSizeInBytes+(BYTE_ALIGN-1)) & ~(BYTE_ALIGN-1);

	m_pchPool = (char*)malloc(m_nTotalPoolSize);
	memset(m_pchPool, 0, m_nTotalPoolSize);

	FindFreeMemory = &CHeap::FirstAvailable;

	THeader *ptHead = (THeader*)m_pchPool;
	ptHead->m_nSize = m_nTotalPoolSize - (sizeof(THeader) + sizeof(TFooter));
	ptHead->m_ptPrev = ptHead;
	ptHead->m_ptNext = ptHead;

	TFooter *ptFoot = (TFooter*)(m_pchPool + ptHead->m_nSize + sizeof(THeader));
	ptFoot->m_nSize = ptHead->m_nSize;

	m_ptFreeHead = ptHead;
	m_ptEndPoolFooter = ptFoot;
	m_nMemoryAvailable = m_ptFreeHead->m_nSize;
}

////////////////////////////////////////////////////////////////////////////////
// FirstAvailable()	:	Finds the first available chunk of memory large enough
//						to hold the size passed in
//
// Ins				:	nAllocSize	-	The amount of memory to allocate
//
// Outs				:	void
//
// Returns			:	char*	-	A pointer to the memory allocated
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
CHeap::THeader * CHeap::FirstAvailable(unsigned int nAllocSize)
{
	if(m_ptFreeHead == NULL || m_ptFreeHead->m_nSize == 0)
		return NULL;

	unsigned int nAvgMemSize = m_nNumPtrs > 0 ? (m_nTotalPoolSize - m_nMemoryAvailable) / (m_nNumPtrs) : 0;

	if(nAllocSize < nAvgMemSize)
	{
		THeader* ptIter = m_ptFreeHead->m_ptNext;
		do
		{
			if(ptIter->m_nSize >= nAllocSize)
				return ptIter;
			else
				ptIter = ptIter->m_ptNext;

		} while(ptIter != m_ptFreeHead->m_ptNext);
	}
	else
	{
		THeader* ptIter = m_ptFreeHead->m_ptPrev;
		do
		{
			if(ptIter->m_nSize >= nAllocSize)
				return ptIter;
			else
				ptIter = ptIter->m_ptPrev;

		} while(ptIter != m_ptFreeHead->m_ptPrev);
	}

	// No More Memory
	throw;
	//return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Allocate()	:	Allocates a section of memory using the Allocator class
//
// Ins			:	nAllocSize	-	The amount of memory to allocate
//
// Outs			:	void
//
// Returns		:	char*	-	A pointer to the memory allocated
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
char * CHeap::Allocate(unsigned int nAllocSize, char* szFile, unsigned int nLine)
{
	// Byte Align
	nAllocSize = (nAllocSize+(BYTE_ALIGN-1)) & ~(BYTE_ALIGN-1);
	THeader *ptHead = FirstAvailable(nAllocSize);

	if(ptHead == NULL) // Free Memory was not found
	{
		return NULL;
	}

	TFooter *ptFoot = (TFooter*)((char*)ptHead + ptHead->m_nSize + 
		sizeof(THeader));

	// Remove the whole block in case we can't split
	m_nMemoryAvailable -= ptHead->m_nSize;
	++m_nNumPtrs;

	// See if there's enough leftover memory for another chunk
	if(ptHead->m_nSize - nAllocSize > (sizeof(THeader) + sizeof(TFooter)))
	{
		// Split the block into smaller blocks
		unsigned int nBlockSize = ptHead->m_nSize;

		ptHead->m_nSize = nAllocSize;

		TFooter *ptNewFoot = ptFoot;

		ptFoot = (TFooter*)((char*)ptHead + ptHead->m_nSize + sizeof(THeader));
		ptFoot->m_nSize = ptHead->m_nSize;

		THeader* ptNewHead = (THeader*)((char*)ptFoot + sizeof(TFooter));
		ptNewHead->m_nSize = nBlockSize - nAllocSize - (sizeof(THeader) + 
			sizeof(TFooter));
		ptNewHead->m_ptNext = ptHead->m_ptNext;
		ptNewHead->m_ptPrev = ptHead;

		ptHead->m_ptNext->m_ptPrev = ptNewHead;
		ptHead->m_ptNext = ptNewHead;

		if(ptHead->m_ptPrev == ptHead)
		{
			ptHead->m_ptPrev = ptNewHead;
		}

		ptNewFoot->m_nSize = ptNewHead->m_nSize;

		// add back the memory from the split block
		m_nMemoryAvailable += nBlockSize - (sizeof(THeader) + sizeof(TFooter) + nAllocSize);
	}

	if(ptHead == m_ptFreeHead) // the memory found was the head of the list
	{
		if(ptHead->m_ptNext != ptHead)	// the memory found is not the only
		{								// chunk in the list
			m_ptFreeHead = ptHead->m_ptNext;
		}
		else
		{
			m_ptFreeHead = NULL; // There is no more free memory
		}
	}

	if(m_ptFreeHead != NULL)
	{
		ptHead->m_ptPrev->m_ptNext = ptHead->m_ptNext;
		ptHead->m_ptNext->m_ptPrev = ptHead->m_ptPrev;
	}

	ptHead->m_ptPrev = ptHead->m_ptNext = ptHead;
	
	ptHead->m_nSize |= (1<<31);

	ptFoot->m_nSize |= (1<<31);

	char* pMemPtr = (char*)ptHead + sizeof(THeader);

	// Leak Detection
	TLeakDetector tLeak;
	tLeak.szFile = szFile;
	tLeak.nLine = nLine;
	tLeak.nSize = nAllocSize;
	m_cLeakList[(unsigned int)pMemPtr] = tLeak;

	return pMemPtr;
}
	
////////////////////////////////////////////////////////////////////////////////
// DeAllocate()	:	Deallocates a section of memory using the Allocator class
//
// Ins			:	pchData	-	A pointer to the block of memory to deallocate
//
// Outs			:	void
//
// Returns		:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
void CHeap::DeAllocate(char * pchData)
{
	// Get the header and footer for the data block and
	// Turn off the high bit to label it as free memory
	THeader * ptHead = (THeader*)(pchData - sizeof(THeader));
	if((ptHead->m_nSize & (1<<31)) == 0)
	{
		return;
	}
	else
	{
		ptHead->m_nSize &= ~(1<<31);
	}

	TFooter * ptFoot = (TFooter*)(pchData + ptHead->m_nSize);
	ptFoot->m_nSize &= ~(1<<31);

	// Clear the old memory
	memset(pchData, 0, ptHead->m_nSize);

	m_nMemoryAvailable += ptHead->m_nSize;
	--m_nNumPtrs;

	// Leak Detection
	m_cLeakList.erase((unsigned int)pchData);

	// See if you can merge right
	THeader * ptNextHead = (THeader*)((char*)ptFoot + sizeof(TFooter));
	if(((char*)ptNextHead < (char*)m_ptEndPoolFooter) && (ptNextHead->m_nSize & (1<<31)) == 0)
	{
		// Next block is free
		TFooter * ptNextFoot = (TFooter*)((char*)ptNextHead + sizeof(THeader) + ptNextHead->m_nSize);

		// Merge Right
		ptHead->m_nSize += ptNextHead->m_nSize + sizeof(THeader) + sizeof(TFooter);
		ptNextFoot->m_nSize = ptHead->m_nSize;

		ptFoot->m_nSize = 0;

		// Move foot pointer
		ptFoot = ptNextFoot;

		m_nMemoryAvailable += sizeof(THeader) + sizeof(TFooter);

		m_ptFreeHead = ptNextHead->m_ptNext;

		if(ptNextHead == m_ptFreeHead)
		{
			m_ptFreeHead = NULL;
		}
		else
		{
			ptNextHead->m_ptNext->m_ptPrev = ptNextHead->m_ptPrev;
			ptNextHead->m_ptPrev->m_ptNext = ptNextHead->m_ptNext;
		}
	}

	// See if you can merge left
	TFooter * ptPrevFoot = (TFooter*)((char*)ptHead - sizeof(TFooter));

	if(((char*)ptPrevFoot > m_pchPool) && (ptPrevFoot->m_nSize & (1<<31)) == 0)
	{
		// Previous block is free
		THeader * ptPrevHead = (THeader*)((char*)ptPrevFoot - 
			ptPrevFoot->m_nSize - sizeof(THeader));

		// Merge Left
		ptPrevHead->m_nSize += ptHead->m_nSize + sizeof(THeader) + sizeof(TFooter);
		ptFoot->m_nSize = ptPrevHead->m_nSize;

		// Clear old Memory
		ptPrevFoot->m_nSize = 0;
		ptHead->m_ptNext = ptHead->m_ptPrev = NULL;
		ptHead->m_nSize = 0;

		// Reset head Pointer
		ptHead = ptPrevHead;

		m_nMemoryAvailable += sizeof(THeader) + sizeof(TFooter);

		return;
	}

	// Block did not merge left, reinsert into list
	if(m_ptFreeHead != NULL)
	{
		// Set the head's prev (left)
		ptHead->m_ptPrev = m_ptFreeHead->m_ptPrev;
		ptHead->m_ptPrev->m_ptNext = ptHead;

		// Set the head's next (right)
		ptHead->m_ptNext = m_ptFreeHead;
		ptHead->m_ptNext->m_ptPrev = ptHead;
	}
	else
	{
		m_ptFreeHead = ptHead;
	}
}

#else
// This version of the functions do not use leak detection

CHeap::CHeap()
{
	m_pchPool = NULL;
	m_ptEndPoolFooter = NULL;
	m_ptFreeHead = NULL;
	m_nTotalPoolSize = 0;

	m_nMemoryAvailable = 0;
	m_nNumPtrs = 0;
}

CHeap::~CHeap()
{
	free(m_pchPool);
}

////////////////////////////////////////////////////////////////////////////////
// Init()	:	Initializes the Heap Object
//
// Ins		:	nPoolSizeInBytes	-	The total pool size of the heap in bytes
//
// Outs		:	void
//
// Returns	:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
void CHeap::Init(unsigned int nPoolSizeInBytes)
{
	m_nTotalPoolSize = (nPoolSizeInBytes+(BYTE_ALIGN-1)) & ~(BYTE_ALIGN-1);

	m_pchPool = (char*)malloc(m_nTotalPoolSize);
	memset(m_pchPool, 0, m_nTotalPoolSize);

	FindFreeMemory = &CHeap::FirstAvailable;

	THeader *ptHead = (THeader*)m_pchPool;
	ptHead->m_nSize = m_nTotalPoolSize - (sizeof(THeader) + sizeof(TFooter));
	ptHead->m_ptPrev = ptHead;
	ptHead->m_ptNext = ptHead;

	TFooter *ptFoot = (TFooter*)(m_pchPool + ptHead->m_nSize + sizeof(THeader));
	ptFoot->m_nSize = ptHead->m_nSize;

	m_ptFreeHead = ptHead;
	m_ptEndPoolFooter = ptFoot;
	m_nMemoryAvailable = m_ptFreeHead->m_nSize;
}

////////////////////////////////////////////////////////////////////////////////
// FirstAvailable()	:	Finds the first available chunk of memory large enough
//						to hold the size passed in
//
// Ins				:	nAllocSize	-	The amount of memory to allocate
//
// Outs				:	void
//
// Returns			:	char*	-	A pointer to the memory allocated
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
CHeap::THeader * CHeap::FirstAvailable(unsigned int nAllocSize)
{
	if(m_ptFreeHead == NULL || m_ptFreeHead->m_nSize == 0)
		return NULL;

	// Attempting to separate the small and large pointers to help avoid
	// memory fragmentation
	unsigned int nAvgMemSize = m_nNumPtrs > 0 ? (m_nTotalPoolSize - m_nMemoryAvailable) / (m_nNumPtrs) : 0;

	if(nAllocSize < nAvgMemSize)
	{
		THeader* ptIter = m_ptFreeHead->m_ptNext;
		do
		{
			if(ptIter->m_nSize >= nAllocSize)
				return ptIter;
			else
				ptIter = ptIter->m_ptNext;

		} while(ptIter != m_ptFreeHead->m_ptNext);
	}
	else
	{
		THeader* ptIter = m_ptFreeHead->m_ptPrev;
		do
		{
			if(ptIter->m_nSize >= nAllocSize)
				return ptIter;
			else
				ptIter = ptIter->m_ptPrev;

		} while(ptIter != m_ptFreeHead->m_ptPrev);
	}

	// No More Memory
	throw;
	//return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Allocate()	:	Allocates a section of memory using the Allocator class
//
// Ins			:	nAllocSize	-	The amount of memory to allocate
//
// Outs			:	void
//
// Returns		:	char*	-	A pointer to the memory allocated
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
char * CHeap::Allocate(unsigned int nAllocSize)
{
	// Byte Align
	if(nAllocSize == 0)
		return NULL;

	nAllocSize = (nAllocSize+(BYTE_ALIGN-1)) & ~(BYTE_ALIGN-1);
	THeader *ptHead = FirstAvailable(nAllocSize);

	if(ptHead == NULL) // Free Memory was not found
	{
		return NULL;
	}

	TFooter *ptFoot = (TFooter*)((char*)ptHead + ptHead->m_nSize + 
		sizeof(THeader));

	// Remove the whole block in case we can't split
	m_nMemoryAvailable -= ptHead->m_nSize;
	++m_nNumPtrs;

	// See if there's enough leftover memory for another chunk
	if(ptHead->m_nSize - nAllocSize > (sizeof(THeader) + sizeof(TFooter)))
	{
		// Split the block into smaller blocks
		unsigned int nBlockSize = ptHead->m_nSize;

		ptHead->m_nSize = nAllocSize;

		TFooter *ptNewFoot = ptFoot;

		ptFoot = (TFooter*)((char*)ptHead + ptHead->m_nSize + sizeof(THeader));
		ptFoot->m_nSize = ptHead->m_nSize;

		THeader* ptNewHead = (THeader*)((char*)ptFoot + sizeof(TFooter));
		ptNewHead->m_nSize = nBlockSize - nAllocSize - (sizeof(THeader) + 
			sizeof(TFooter));
		ptNewHead->m_ptNext = ptHead->m_ptNext;
		ptNewHead->m_ptPrev = ptHead;

		ptHead->m_ptNext->m_ptPrev = ptNewHead;
		ptHead->m_ptNext = ptNewHead;

		if(ptHead->m_ptPrev == ptHead)
		{
			ptHead->m_ptPrev = ptNewHead;
		}

		ptNewFoot->m_nSize = ptNewHead->m_nSize;

		// add back the memory from the split block
		m_nMemoryAvailable += nBlockSize - (sizeof(THeader) + sizeof(TFooter) + nAllocSize);
	}

	if(ptHead == m_ptFreeHead) // the memory found was the head of the list
	{
		if(ptHead->m_ptNext != ptHead)	// the memory found is not the only
		{								// chunk in the list
			m_ptFreeHead = ptHead->m_ptNext;
		}
		else
		{
			m_ptFreeHead = NULL; // There is no more free memory
		}
	}

	if(m_ptFreeHead != NULL)
	{
		ptHead->m_ptPrev->m_ptNext = ptHead->m_ptNext;
		ptHead->m_ptNext->m_ptPrev = ptHead->m_ptPrev;
	}

	ptHead->m_ptPrev = ptHead->m_ptNext = ptHead;
	
	ptHead->m_nSize |= (1<<31);

	ptFoot->m_nSize |= (1<<31);

	return (char*)ptHead + sizeof(THeader);
}

////////////////////////////////////////////////////////////////////////////////
// DeAllocate()	:	Deallocates a section of memory using the Allocator class
//
// Ins			:	pchData	-	A pointer to the block of memory to deallocate
//
// Outs			:	void
//
// Returns		:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
void CHeap::DeAllocate(char * pchData)
{
	// Get the header and footer for the data block and
	// Turn off the high bit to label it as free memory
	THeader * ptHead = (THeader*)(pchData - sizeof(THeader));
	if((ptHead->m_nSize & (1<<31)) == 0)
	{
		return;
	}
	else
	{
		ptHead->m_nSize &= ~(1<<31);
	}

	TFooter * ptFoot = (TFooter*)(pchData + ptHead->m_nSize);
	ptFoot->m_nSize &= ~(1<<31);

	// Clear the old memory
	memset(pchData, 0, ptHead->m_nSize);

	m_nMemoryAvailable += ptHead->m_nSize;
	--m_nNumPtrs;

	// See if you can merge right
	THeader * ptNextHead = (THeader*)((char*)ptFoot + sizeof(TFooter));
	if(((char*)ptNextHead < (char*)m_ptEndPoolFooter) && (ptNextHead->m_nSize & (1<<31)) == 0)
	{
		// Next block is free
		TFooter * ptNextFoot = (TFooter*)((char*)ptNextHead + sizeof(THeader) + ptNextHead->m_nSize);

		// Merge Right
		ptHead->m_nSize += ptNextHead->m_nSize + sizeof(THeader) + sizeof(TFooter);
		ptNextFoot->m_nSize = ptHead->m_nSize;

		ptFoot->m_nSize = 0;

		// Move foot pointer
		ptFoot = ptNextFoot;

		m_nMemoryAvailable += sizeof(THeader) + sizeof(TFooter);

		m_ptFreeHead = ptNextHead->m_ptNext;

		if(ptNextHead == m_ptFreeHead)
		{
			m_ptFreeHead = NULL;
		}
		else
		{
			ptNextHead->m_ptNext->m_ptPrev = ptNextHead->m_ptPrev;
			ptNextHead->m_ptPrev->m_ptNext = ptNextHead->m_ptNext;
		}
	}

	// See if you can merge left
	TFooter * ptPrevFoot = (TFooter*)((char*)ptHead - sizeof(TFooter));

	if(((char*)ptPrevFoot > m_pchPool) && (ptPrevFoot->m_nSize & (1<<31)) == 0)
	{
		// Previous block is free
		THeader * ptPrevHead = (THeader*)((char*)ptPrevFoot - 
			ptPrevFoot->m_nSize - sizeof(THeader));

		// Merge Left
		ptPrevHead->m_nSize += ptHead->m_nSize + sizeof(THeader) + sizeof(TFooter);
		ptFoot->m_nSize = ptPrevHead->m_nSize;

		// Clear old Memory
		ptPrevFoot->m_nSize = 0;
		ptHead->m_ptNext = ptHead->m_ptPrev = NULL;
		ptHead->m_nSize = 0;

		// Reset head Pointer
		ptHead = ptPrevHead;

		m_nMemoryAvailable += sizeof(THeader) + sizeof(TFooter);

		return;
	}

	// Block did not merge left, reinsert into list
	if(m_ptFreeHead != NULL)
	{
		// Set the head's prev (left)
		ptHead->m_ptPrev = m_ptFreeHead->m_ptPrev;
		ptHead->m_ptPrev->m_ptNext = ptHead;

		// Set the head's next (right)
		ptHead->m_ptNext = m_ptFreeHead;
		ptHead->m_ptNext->m_ptPrev = ptHead;
	}
	else
	{
		m_ptFreeHead = ptHead;
	}
}

#endif