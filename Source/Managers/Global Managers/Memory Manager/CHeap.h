////////////////////////////////////////////////////////////////////////////////
//	File			:	Heap.h
//	Date			:	3/29/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Chunks of memory that can be allocated and used as 
//						dynamic memory
////////////////////////////////////////////////////////////////////////////////

#ifndef _CHEAP_H_
#define _CHEAP_H_

#if _DEBUG
	#define MM_LEAK_DETECTION 1
#else
	#define MM_LEAK_DETECTION 0
#endif

#if MM_LEAK_DETECTION
// This version of the class uses Leak Detection

#include <map>
using std::map;

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

	// For Debugging Purposes
	int m_nMemoryAvailable;
	int m_nNumPtrs;

	// Struct used in the detection of memory leaks within the manager
	struct TLeakDetector
	{
		char* szFile;			// File where the memory was allocated
		unsigned int nLine;		// Line the memory was allocated on
		unsigned int nSize;		// The memory of allocated
	};

	// List of all the Memory currently allocated
	map<unsigned int, TLeakDetector> m_cLeakList;
	
public:
	// Constructor
	CHeap();

	// Trilogy of Evil
	~CHeap();
	CHeap(const CHeap &);
	CHeap & operator=(const CHeap &);

	// Accessors
	unsigned int GetMemUsed() { return m_nTotalPoolSize - m_nMemoryAvailable; }
	unsigned int GetPoolSize() { return m_nTotalPoolSize; }

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
	char * Allocate(unsigned int nAllocSize, char* szFile, unsigned int nLine);
	
////////////////////////////////////////////////////////////////////////////////
// DeAllocate()		:	Adds the memory allocated at the passed in pointer back to
//						to the pool of free memory
//
// Ins				:	char* pchData	-	The data to be deallocated
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void DeAllocate(char * pchData);
};

#else
// This version of the class does not use leak detection

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

	// For Debugging Purposes
	int m_nMemoryAvailable;
	int m_nNumPtrs;
	
public:
	// Constructor
	CHeap();

	// Trilogy of Evil
	~CHeap();
	CHeap(const CHeap &);
	CHeap & operator=(const CHeap &);
	
	// Accessors
	unsigned int GetMemUsed() { return m_nTotalPoolSize - m_nMemoryAvailable; }
	unsigned int GetPoolSize() { return m_nTotalPoolSize; }

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
// DeAllocate()		:	Adds the memory allocated at the passed in pointer back to
//						to the pool of free memory
//
// Ins				:	char* pchData	-	The data to be deallocated
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void DeAllocate(char * pchData);
};

#endif	// MM_LEAK_DETECTION
#endif	// _CHEAP_H_