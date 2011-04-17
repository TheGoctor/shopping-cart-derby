////////////////////////////////////////////////////////////////////////////////
//	File			:	MemoryManager.h
//	Date			:	3/29/11
//	Mod. Date		:	3/29/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Manages Heap objects and keeps track of memory.
////////////////////////////////////////////////////////////////////////////////

#include "CMemoryManager.h"

CMemoryManager::CMemoryManager() : m_cGeneralHeap(), m_cEventHeap()
{
	//m_pInstance = NULL;
}

CMemoryManager::~CMemoryManager()
{
}

CMemoryManager* CMemoryManager::GetInstance()
{
	//if(m_pInstance)
	//	return m_pInstance;

	//m_pInstance = (CMemoryManager*) m_cGeneralHeap.Allocate(sizeof(CMemoryManager));

	static CMemoryManager m_cInstance;
	return &m_cInstance;
}

void CMemoryManager::Initialize()
{
	m_cGeneralHeap.Init(1048576);	// 1MB
	m_cEventHeap.Init(1048576);	// 1MB
}

void CMemoryManager::Initialize(unsigned int nGeneralHeapSize, unsigned int nEventHeapSize)
{
	m_cGeneralHeap.Init(nGeneralHeapSize);
	m_cEventHeap.Init(nEventHeapSize);
}

void CMemoryManager::Shutdown()
{
}

char* CMemoryManager::Allocate(unsigned int nAllocSize, EHeapID eHeapIndx)
{
	switch(eHeapIndx)
	{
	case HEAPID_GENERAL:
		{
			return m_cGeneralHeap.Allocate(nAllocSize);
		}
		break;
	case HEAPID_EVENT:
		{
			return m_cEventHeap.Allocate(nAllocSize);
		}
		break;
	default:
		{
			// There was an error
			return NULL;
		}
		break;
	};
}

void CMemoryManager::Deallocate(char *pchData, EHeapID eHeapIndx)
{
	switch(eHeapIndx)
	{
	case HEAPID_GENERAL:
		{
			m_cGeneralHeap.DeAllocate(pchData);
		}
		break;
	case HEAPID_EVENT:
		{
			m_cEventHeap.DeAllocate(pchData);
		}
		break;
	default:
		{
			// There was an error
		}
		break;
	};
}