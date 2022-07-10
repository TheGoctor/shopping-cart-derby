////////////////////////////////////////////////////////////////////////////////
//	File			:	MemoryManager.h
//	Date			:	3/29/11
//	Mod. Date		:	3/29/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Manages Heap objects and keeps track of memory.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CMEMORYMANAGER_H_
#define _CMEMORYMANAGER_H_

#include "CHeap.h"
#include "scd::allocator.h"
#include "..\..\..\Enums.h"

// Macros To make Memory Init more readable
#define MB *1024 KB
#define KB *1024

#if MM_LEAK_DETECTION
// Code that uses Leak Detection

	// Memory Macros for Explicit Heaps
	#define MMNEWEX(Type, ID) new(CMemoryManager::GetInstance()->Allocate(sizeof(Type), ID, __FILE__, __LINE__)) Type
	// Memory Macros for Arrays in an Explicit Heap
	#define MMNEWARRAYEX(Type, Count, ID) NewArray<Type>(Count, ID, __FILE__, __LINE__)

#else
// Code that doesn't use Leak Detection

	// Memory Macros for Explicit Heaps
	#define MMNEWEX(Type, ID) new(CMemoryManager::GetInstance()->Allocate(sizeof(Type), ID)) Type
	// Memory Macros for Arrays in an Explicit Heap
	#define MMNEWARRAYEX(Type, Count, ID) NewArray<Type>(Count, ID)

#endif

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
template<typename TYPE>
void MyDelete(TYPE* pPtr, EHeapID nHeap)
{
	if(pPtr)
	{
		pPtr->~TYPE();
		CMemoryManager::GetInstance()->Deallocate((char*)pPtr, nHeap);
	}
}

// Function for allocating new arrays w/ and w/o leak detection
#if MM_LEAK_DETECTION

template<typename TYPE>
TYPE* NewArray(unsigned int nCount, EHeapID nHeap, char* szFile, int nLine)
{
	unsigned int nTypeSize = sizeof(TYPE);

	// Allocate memory with added space for typesize and count
	unsigned int* pNewMem = (unsigned int*)CMemoryManager::GetInstance()->Allocate(nTypeSize * 
		nCount + sizeof(nTypeSize) + sizeof(nCount), nHeap, szFile, nLine);

#else

template<typename TYPE>
TYPE* NewArray(unsigned int nCount, EHeapID nHeap)
{
	unsigned int nTypeSize = sizeof(TYPE);

	// Allocate memory with added space for typesize and count
	unsigned int* pNewMem = (unsigned int*)CMemoryManager::GetInstance()->Allocate(nTypeSize * 
		nCount + sizeof(nTypeSize) + sizeof(nCount), nHeap);

#endif

	// Save the size of the data type
	pNewMem[0] = nTypeSize;

	// Save the number of elements
	pNewMem[1] = nCount;

	// Loop and call placement new on each index
	char* pIter = (char*)&(pNewMem[2]);
	for(unsigned int n = 0; n < nCount; n++)
	{
		new (pIter) TYPE();
		pIter += nTypeSize;
	}

	// Return the pointer to the actual memory after the size data
	return (TYPE*)&(pNewMem[2]);
}

// Function for deleting arrays
template<typename TYPE>
void DelArray(TYPE* pPtr, EHeapID nHeap)
{
	if(pPtr)
	{
		char* pIter = (char*)pPtr;

		// Get the size of the orignal type
		unsigned int nTypeSize = *((unsigned int*)(pIter - sizeof(unsigned int)*2));

		// Get the number of elements
		unsigned int nCount = *((unsigned int*)(pIter - sizeof(unsigned int)));

		// Loop through the elements and call MMDEL on them
		for(unsigned int n = 0; n < nCount; n++)
		{
			((TYPE*)pIter)->~TYPE();
			pIter += nTypeSize;
		}

		pIter = (char*)pPtr;
		// Delete the whole array
		MMDELEX(pIter - 8, nHeap); // -8 to get beginning of array
	}
}

class CMemoryManager
{
	// list of heaps
	CHeap	m_cGeneralHeap;	// Used for Overall Uses
	CHeap	m_cEventHeap;	// Used specifically for Events and the ID Gen
	CHeap	m_cRenderHeap;	// Used primarily for rendering containers
	CHeap	m_cDynArrayHeap;	// Used exclusively for CDynArray

	// Constructor
	CMemoryManager();

	// Trilogy of Evil
	~CMemoryManager();
	CMemoryManager(const CMemoryManager&);
	CMemoryManager& operator=(const CMemoryManager&);

public:
	static CMemoryManager* GetInstance();

	// Accessors
	unsigned int GetGHeapMemUsed() { return m_cGeneralHeap.GetMemUsed(); }
	unsigned int GetGHeapMemSize() { return m_cGeneralHeap.GetPoolSize(); }
	unsigned int GetEHeapMemUsed() { return m_cEventHeap.GetMemUsed(); }
	unsigned int GetEHeapMemSize() { return m_cEventHeap.GetPoolSize(); }
	unsigned int GetRHeapMemUsed() { return m_cRenderHeap.GetMemUsed(); }
	unsigned int GetRHeapMemSize() { return m_cRenderHeap.GetPoolSize(); }
	unsigned int GetDHeapMemUsed() { return m_cDynArrayHeap.GetMemUsed(); }
	unsigned int GetDHeapMemSize() { return m_cDynArrayHeap.GetPoolSize(); }

	////////////////////////////////////////////////////////////////////////////////
	// Function: “Initialize”
	//
	// Return:  void
	//
	// Parameters:	unsigned int nGeneralHeapSize	-	The size of the General Heap
	//				unsigned int nEventHeapSize		-	The size of the Event Heap
	//				unsigned int nRenderHeapSize	-	The size of the Render Heap
	//				unsigned int nDynArrayHeapSize - The size of the DynArray Heap
	//
	// Purpose:  This function initializes the memory manager by setting the heap
	//		sizes for each of the heaps.
	////////////////////////////////////////////////////////////////////////////////
	void Initialize(unsigned int nGeneralHeapSize, unsigned int nEventHeapSize, 
		unsigned int nRenderHeapSize, unsigned int nDynArrayHeapSize);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “Allocate”
	//
	// Return:  char*	-	The pointer to the memory allocated
	//
	// Parameters:	unsigned int	-	The amount of memory to allocate
	//				EHeapID			-	The index of the Heap to allocate to
	//
	// Purpose:  This function allocates memory of the specified size in the 
	//		specified heap. Returns NULL if not enough memory is available.
	////////////////////////////////////////////////////////////////////////////////
#if MM_LEAK_DETECTION
	char* Allocate(unsigned int nAllocSize, EHeapID eHeapIndx, char* szFile, int nLine);
#else
	char* Allocate(unsigned int nAllocSize, EHeapID eHeapIndx);
#endif

	////////////////////////////////////////////////////////////////////////////////
	// Function: “Deallocate”
	//
	// Return:  void
	//
	// Parameters:	char* pchData		-	The pointer to deallocate
	//				EHeapID eHeapIndx	-	the index of the heap to deallocate from
	//
	// Purpose:  This function deallocates the memory at the passed in pointer in
	//		the specified heap.
	////////////////////////////////////////////////////////////////////////////////
	void Deallocate(char* pchData, EHeapID eHeapIndx);
};

#endif	// _CMEMORYMANAGER_H_