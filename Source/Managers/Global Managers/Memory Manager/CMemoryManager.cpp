////////////////////////////////////////////////////////////////////////////////
//	File			:	MemoryManager.h
//	Date			:	3/29/11
//	Mod. Date		:	3/29/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Manages Heap objects and keeps track of memory.
////////////////////////////////////////////////////////////////////////////////

#include "CMemoryManager.h"

#if MM_LEAK_DETECTION
// This version of the functions use Leak Detection

CMemoryManager::CMemoryManager() : m_cGeneralHeap(), m_cEventHeap(), m_cRenderHeap()
{
}

CMemoryManager::~CMemoryManager()
{
}

CMemoryManager* CMemoryManager::GetInstance()
{
	static CMemoryManager cMemoryManager;
	return &cMemoryManager;
}

void CMemoryManager::Initialize(unsigned int nGeneralHeapSize, unsigned int nEventHeapSize, 
	unsigned int nRenderHeapSize)
{
	m_cGeneralHeap.Init(nGeneralHeapSize);
	m_cEventHeap.Init(nEventHeapSize);
	m_cRenderHeap.Init(nRenderHeapSize);
}

void CMemoryManager::Shutdown()
{
}

char* CMemoryManager::Allocate(unsigned int nAllocSize, EHeapID eHeapIndx, char* szFile, int nLine)
{
	switch(eHeapIndx)
	{
	case HEAPID_GENERAL:
		{
			return m_cGeneralHeap.Allocate(nAllocSize, szFile, nLine);
		}
		break;
	case HEAPID_EVENT:
		{
			return m_cEventHeap.Allocate(nAllocSize, szFile, nLine);
		}
		break;
	case HEAPID_RENDER:
		{
			return m_cRenderHeap.Allocate(nAllocSize, szFile, nLine);
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
	if(pchData == NULL)
		return;

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
	case HEAPID_RENDER:
		{
			m_cRenderHeap.DeAllocate(pchData);
		}
		break;
	default:
		{
			// There was an error
		}
		break;
	};
}

#else
// This version of the functions do not use leak detection

CMemoryManager::CMemoryManager() : m_cGeneralHeap(), m_cEventHeap(), m_cRenderHeap()
{
}

CMemoryManager::~CMemoryManager()
{
}

CMemoryManager* CMemoryManager::GetInstance()
{
	static CMemoryManager cMemoryManager;
	return &cMemoryManager;
}

void CMemoryManager::Initialize(unsigned int nGeneralHeapSize, unsigned int nEventHeapSize,
	unsigned int nRenderHeapSize)
{
	m_cGeneralHeap.Init(nGeneralHeapSize);
	m_cEventHeap.Init(nEventHeapSize);
	m_cRenderHeap.Init(nRenderHeapSize);
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
	case HEAPID_RENDER:
		{
			return m_cRenderHeap.Allocate(nAllocSize);
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
	if(pchData == NULL)
		return;

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
	case HEAPID_RENDER:
		{
			m_cRenderHeap.DeAllocate(pchData);
		}
		break;
	default:
		{
			// There was an error
		}
		break;
	};
}
#endif