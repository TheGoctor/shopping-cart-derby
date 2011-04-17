////////////////////////////////////////////////////////////////////////////////
//	File			:	Heap.h
//	Date			:	3/29/11
//	Mod. Date		:	3/29/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Chunks of memory that can be allocated and used as 
//						dynamic memory
////////////////////////////////////////////////////////////////////////////////

#ifndef _CHEAP_H_
#define _CHEAP_H_

class CHeap
{
public:
	struct THeader
	{
		// How much memory is free INSIDE the block : excludes header and footer size
		// High order bit of 1 == used memory block
		// High order bit of 0 == free memory block
		unsigned int m_nSize;
		// Pointer to the previous free header in the Cyclic Doubly Linked List
		THeader * m_ptPrev;
		// Pointer to the next free header in the Cyclic Doubly Linked List
		THeader * m_ptNext;
	};
	struct TFooter
	{
		// How much memory is free INSIDE the block : excludes header and footer size
		// High order bit of 1 == used memory block
		// High order bit of 0 == free memory block
		unsigned int m_nSize;
	};
private:
	// Entire Memory Pool
	char * m_pchPool;
	// The Last footer
	TFooter * m_ptEndPoolFooter;
	// The Entry Point to the free list (Cyclic Doubly-Linked List)
	THeader * m_ptFreeHead;
	// How much memory was allocated for this pool.
	unsigned int m_nTotalPoolSize;
	// Function pointer for the allocation behavior
	THeader *(CHeap::*FindFreeMemory)(unsigned int nAllocSize);
	// Allocation Behaviors
	THeader * FirstAvailable(unsigned int nAllocSize);
	
public:
	CHeap();
	~CHeap();
	CHeap(const CHeap &);
	CHeap & operator=(const CHeap &);

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
	void Init(unsigned int nPoolSizeInBytes);

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
	char * Allocate(unsigned int nAllocSize);
	
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
	void DeAllocate(char * pchData);
};

#endif