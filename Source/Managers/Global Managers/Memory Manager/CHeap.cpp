#include <malloc.h>
#include <assert.h>
#include "CHeap.h"

#define NULL 0
#define BYTE_ALIGN 8

CHeap::CHeap()
{
	m_pchPool = NULL;
	m_ptEndPoolFooter = NULL;
	m_ptFreeHead = NULL;
	m_nTotalPoolSize = 0;
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

	// TODO: align malloc call to 4 bytes but not 8 bytes
	// so end of first header is 8 byte-aligned
	m_pchPool = (char*)malloc(m_nTotalPoolSize);

	FindFreeMemory = &CHeap::FirstAvailable;

	THeader *ptHead = (THeader*)m_pchPool;
	ptHead->m_nSize = m_nTotalPoolSize - (sizeof(THeader) + sizeof(TFooter));
	ptHead->m_ptPrev = ptHead;
	ptHead->m_ptNext = ptHead;

	TFooter *ptFoot = (TFooter*)(m_pchPool + ptHead->m_nSize + sizeof(THeader));
	ptFoot->m_nSize = ptHead->m_nSize;

	m_ptFreeHead = ptHead;
	m_ptEndPoolFooter = ptFoot;
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
	nAllocSize = (nAllocSize+(BYTE_ALIGN-1)) & ~(BYTE_ALIGN-1);
	THeader *ptHead = FirstAvailable(nAllocSize);

	if(ptHead == NULL) // Free Memory was not found
	{
		return NULL;
	}

	TFooter *ptFoot = (TFooter*)((char*)ptHead + ptHead->m_nSize + 
		sizeof(THeader));

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
	if(m_ptFreeHead == NULL)
	{
		return NULL;
	}

	if(m_ptFreeHead->m_nSize >= nAllocSize)
	{
		return m_ptFreeHead;
	}
	else
	{
		THeader* ptIter = m_ptFreeHead;
		do
		{
			if(ptIter->m_nSize >= nAllocSize)
				return ptIter;
			else
				ptIter = ptIter->m_ptNext;

		} while(ptIter != m_ptFreeHead);

		return NULL;
	}
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
	}

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

		if(ptNextHead->m_ptPrev != ptHead)
		{
			ptHead->m_ptPrev = ptNextHead->m_ptPrev;
		}
		if(ptNextHead->m_ptNext != ptHead)
		{
			ptHead->m_ptNext = ptNextHead->m_ptNext;
		}

		if(ptHead->m_ptPrev == ptNextHead)
		{
			ptHead->m_ptPrev = ptHead;
		}
		if(ptHead->m_ptNext == ptNextHead)
		{
			ptHead->m_ptNext = ptHead;
		}

		if(ptHead->m_ptPrev != ptHead)
		{
			ptHead->m_ptPrev->m_ptNext = ptHead;
		}
		if(ptHead->m_ptNext != ptHead)
		{
			ptHead->m_ptNext->m_ptPrev = ptHead;
		}


		if(ptNextHead == m_ptFreeHead)
		{
			m_ptFreeHead = ptHead;
		}

		// Clear old memory
		ptNextHead->m_ptPrev = ptNextHead->m_ptNext = NULL;
		ptNextHead->m_nSize = 0;
	}

	if(m_ptFreeHead != NULL)
	{
		// Set the head's prev (left)
		if(ptHead->m_ptPrev == ptHead)
		{
			ptHead->m_ptPrev = m_ptFreeHead->m_ptPrev;
			ptHead->m_ptPrev->m_ptNext = ptHead;
		}

		// Set the head's next (right)
		if(ptHead->m_ptNext == ptHead)
		{
			ptHead->m_ptNext = m_ptFreeHead;
			ptHead->m_ptNext->m_ptPrev = ptHead;
		}
	}
	else
	{
		m_ptFreeHead = ptHead;
	}

	pchData = NULL;
}