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
#include "CAllocator.h"
#include "..\..\..\Enums.h"

// To make Memory Init more readable
#define MB *1024 KB
#define KB *1024

#if MM_LEAK_DETECTION
// The version of the class that uses Leak Detection

// Memory Macros for Explicit Heaps
#define MMNEWEX(Type, ID) new(CMemoryManager::GetInstance()->Allocate(sizeof(Type), ID, __FILE__, __LINE__)) Type
#define MMDELEX(Ptr, ID) MyDelete(Ptr, ID)

// Memory Macros for General Heap
#define MMNEW(Type) MMNEWEX(Type, HEAPID_GENERAL)
#define MMDEL(Ptr) MyDelete(Ptr, HEAPID_GENERAL)

// Memory Macros for Event Heap
#define MMNEWEVENT(Type) MMNEWEX(Type, HEAPID_EVENT)
#define MMDELEVENT(Ptr) MyDelete(Ptr, HEAPID_EVENT)

// Memory Macros for Arrays in an Explicit Heap
#define MMNEWARRAYEX(Type, Count, ID) NewArray<Type>(Count, ID, __FILE__, __LINE__)
#define MMDELARRAYEX(Ptr, ID) DelArray(Ptr, ID)

template<typename TYPE>
void MyDelete(TYPE* pPtr, EHeapID nHeap)
{
	if(pPtr)
	{
		pPtr->~TYPE();
		CMemoryManager::GetInstance()->Deallocate((char*)pPtr, nHeap);
	}
}

template<typename TYPE>
TYPE* NewArray(unsigned int nCount, EHeapID nHeap, char* szFile, int nLine)
{
	int nTypeSize = sizeof(TYPE);

	// Allocate memory with added space for typesize and count
	int* pNewMem = (int*)CMemoryManager::GetInstance()->Allocate(nTypeSize * 
		nCount + sizeof(nTypeSize) + sizeof(nCount), nHeap, szFile, nLine);

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

template<typename TYPE>
void DelArray(TYPE* pPtr, EHeapID nHeap)
{
	char* pIter = (char*)pPtr;

	// Get the size of the orignal type
	int nTypeSize = *(pIter - 8);	// 8 for 2 ints

	// Get the number of elements
	int nCount = *(pIter - 4);		// 4 for 1 int

	// Loop through the elements and call MMDEL on them
	for(int n = 0; n < nCount; n++)
	{
		((TYPE*)pIter)->~TYPE();
		pIter += nTypeSize;
	}

	pIter = (char*)pPtr;
	// Delete the whole array
	MMDELEX(pIter - 8, nHeap); // -8 to get beginning of array
}

class CMemoryManager
{
	// list of heaps
	CHeap	m_cGeneralHeap;
	CHeap	m_cEventHeap;
	CHeap	m_cRenderHeap;

	//// For Singleton Access
	//CMemoryManager* m_pInstance;

	// Constructor
	CMemoryManager();

	// Trilogy of Evil
	~CMemoryManager();
	CMemoryManager(const CMemoryManager&);
	CMemoryManager& operator=(const CMemoryManager&);

public:
	static CMemoryManager* GetInstance();

	unsigned int GetGHeapMemUsed() { return m_cGeneralHeap.GetMemUsed(); }
	unsigned int GetGHeapMemSize() { return m_cGeneralHeap.GetPoolSize(); }
	unsigned int GetEHeapMemUsed() { return m_cEventHeap.GetMemUsed(); }
	unsigned int GetEHeapMemSize() { return m_cEventHeap.GetPoolSize(); }
	unsigned int GetRHeapMemUsed() { return m_cRenderHeap.GetMemUsed(); }
	unsigned int GetRHeapMemSize() { return m_cRenderHeap.GetPoolSize(); }

	void Initialize(unsigned int nGeneralHeapSize, unsigned int nEventHeapSize, 
		unsigned int nRenderHeapSize);
	void Shutdown();

	char* Allocate(unsigned int nAllocSize, EHeapID eHeapIndx, char* szFile, int nLine);
	void Deallocate(char* pchData, EHeapID eHeapIndx);
};

#else
// Version of the class that does not use Memory Leak Detection

// Memory Macros for Explicit Heaps
#define MMNEWEX(Type, ID) new(CMemoryManager::GetInstance()->Allocate(sizeof(Type), ID)) Type
#define MMDELEX(Ptr, ID) MyDelete(Ptr, ID)

// Memory Macros for General Heap
#define MMNEW(Type) MMNEWEX(Type, HEAPID_GENERAL)
#define MMDEL(Ptr) MyDelete(Ptr, HEAPID_GENERAL)

// Memory Macros for Event Heap
#define MMNEWEVENT(Type) MMNEWEX(Type, HEAPID_EVENT)
#define MMDELEVENT(Ptr) MyDelete(Ptr, HEAPID_EVENT)

// Memory Macros for Arrays in an Explicit Heap
#define MMNEWARRAYEX(Type, Count, ID) NewArray<Type>(sizeof(Type), Count, ID)
#define MMDELARRAYEX(Ptr, ID) DelArray(Ptr, ID)

template<typename TYPE>
void MyDelete(TYPE* pPtr, EHeapID nHeap)
{
	if(pPtr)
	{
		pPtr->~TYPE();
		CMemoryManager::GetInstance()->Deallocate((char*)pPtr, nHeap);
	}
}

template<typename TYPE>
TYPE* NewArray(unsigned int nTypeSize, unsigned int nCount, EHeapID nHeap)
{
	// Allocate memory with added space for typesize and count
	int* pNewMem = (int*)CMemoryManager::GetInstance()->Allocate(nTypeSize * 
		nCount + sizeof(nTypeSize) + sizeof(nCount), nHeap);

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

template<typename TYPE>
void DelArray(TYPE* pPtr, EHeapID nHeap)
{
	char* pIter = (char*)pPtr;

	// Get the size of the orignal type
	int nTypeSize = *(pIter - 8);	// 8 for 2 ints

	// Get the number of elements
	int nCount = *(pIter - 4);		// 4 for 1 int

	// Loop through the elements and call MMDEL on them
	for(int n = 0; n < nCount; n++)
	{
		((TYPE*)pIter)->~TYPE();
		pIter += nTypeSize;
	}

	pIter = (char*)pPtr;
	// Delete the whole array
	MMDELEX(pIter - 8, nHeap); // -8 to get beginning of array
}

class CMemoryManager
{
	// list of heaps
	CHeap	m_cGeneralHeap;
	CHeap	m_cEventHeap;
	CHeap	m_cRenderHeap;

	//// For Singleton Access
	//CMemoryManager* m_pInstance;

	// Constructor
	CMemoryManager();

	// Trilogy of Evil
	~CMemoryManager();
	CMemoryManager(const CMemoryManager&);
	CMemoryManager& operator=(const CMemoryManager&);

public:
	static CMemoryManager* GetInstance();
	
	unsigned int GetGHeapMemUsed() { return m_cGeneralHeap.GetMemUsed(); }
	unsigned int GetGHeapMemSize() { return m_cGeneralHeap.GetPoolSize(); }
	unsigned int GetEHeapMemUsed() { return m_cEventHeap.GetMemUsed(); }
	unsigned int GetEHeapMemSize() { return m_cEventHeap.GetPoolSize(); }
	unsigned int GetRHeapMemUsed() { return m_cRenderHeap.GetMemUsed(); }
	unsigned int GetRHeapMemSize() { return m_cRenderHeap.GetPoolSize(); }

	void Initialize(unsigned int nGeneralHeapSize, unsigned int nEventHeapSize,
		unsigned int nRenderHeapSize);
	void Shutdown();

	char* Allocate(unsigned int nAllocSize, EHeapID eHeapIndx);

	void Deallocate(char* pchData, EHeapID eHeapIndx);
};

#endif	// MM_LEAK_DETECTION

#endif	// _CMEMORYMANAGER_H_