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

enum EHeapID{HEAPID_GENERAL, HEAPID_EVENT};

#define MMNEW(Type) new(CMemoryManager::GetInstance()->Allocate(sizeof(Type), HEAPID_GENERAL))
#define MMDEL(Type, Ptr) (Ptr)->~Type(); CMemoryManager::GetInstance()->Deallocate((char*)(Ptr), HEAPID_GENERAL)

#define MMNEWEVENT(Type) new(CMemoryManager::GetInstance()->Allocate(sizeof(Type), HEAPID_EVENT))
#define MMDELEVENT(Type, Ptr) (Ptr)->~Type(); CMemoryManager::GetInstance()->Deallocate((char*)(Ptr), HEAPID_EVENT)

#define MMNEWEX(Type, ID) new(CMemoryManager::GetInstance()->Allocate(sizeof(Type), ID))
#define MMDELEX(Type, Ptr, ID) (Ptr)->Type(); CMemoryManager::GetInstance()->Deallocate((char*)(Ptr), ID)

class CMemoryManager
{
	// list of heaps
	CHeap	m_cGeneralHeap;
	CHeap	m_cEventHeap;

	//// For Singleton Access
	//static CMemoryManager* m_pInstance;

	// Constructor
	CMemoryManager();

	// Trilogy of Evil
	~CMemoryManager();
	CMemoryManager(const CMemoryManager&);
	CMemoryManager& operator=(const CMemoryManager&);

public:

	static CMemoryManager* GetInstance();

	void Initialize();
	void Initialize(unsigned int nGeneralHeapSize, unsigned int nEventHeapSize);
	void Shutdown();

	char* Allocate(unsigned int nAllocSize, EHeapID eHeapIndx);
	void Deallocate(char* pchData, EHeapID eHeapIndx);

};

#endif