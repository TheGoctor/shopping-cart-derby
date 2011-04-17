#include "CEventManager.h"
#include "IEvent.h"
#include "../../../IComponent.h"
#include "../../../CObject.h"
#include "CIDGen.h"

CEventManager::CEventManager(void)
{
}

CEventManager::~CEventManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CEventManager* CEventManager::GetInstance()
{
	static CEventManager m_cInstance;
	return &m_cInstance;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
EventID CEventManager::RegisterEvent(string szEventName, IComponent* pcListener,
									 void(*pfCallback)(IEvent*, IComponent*))
{
	//if(pcListener == NULL)
	//	return 0;	// Error

	// make listener obj
	TListener* ptListener = (TListener*)CMemoryManager::GetInstance()->
		Allocate(sizeof(TListener), HEAPID_EVENT);
	ptListener->m_pcListener = pcListener;
	ptListener->m_pfCallback = pfCallback;

	// Convert string to int
	EventID nEventID = CIDGen::GetInstance()->GetID(szEventName);

	// use generated int to pair with listener into map
	// and add listener to map
	m_cListeners.insert(make_pair(nEventID, ptListener));

	return nEventID;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::RegisterEvent(EventID nEventID)
{
}

bool CEventManager::AlreadyRegistered(EventID nEventID, IComponent* pListener)
{
	pair<EventIter, EventIter> range;

	range = m_cListeners.equal_range(nEventID);

	for(EventIter iter = range.first; iter != range.second; iter++)
	{
		if((*iter).second->m_pcListener == pListener)
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::UnregisterEvent(EventID nEventID, IComponent* pcListener)
{
	pair<EventIter, EventIter> range;

	range = m_cListeners.equal_range(nEventID);

	for(EventIter iter = range.first; iter != range.second; iter++)
	{
		if((*iter).second->m_pcListener == pcListener)
		{
			iter = m_cListeners.erase(iter);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::UnregisterEventAll(IComponent* pcListener)
{
	EventIter iter = m_cListeners.begin();
	
	while(iter != m_cListeners.end())
	{
		if((*iter).second->m_pcListener == pcListener)
			iter = m_cListeners.erase(iter);
		else
			iter++;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::PostEvent(IEvent* pEvent, unsigned int nPriority)
{
	if(nPriority != PRIORITY_IMMEDIATE)
	{
		pEvent->SetPriority(nPriority);
		m_cEventList.insert(pEvent);
	}
	else
	{
		pair<EventIter, EventIter> range;
		range = m_cListeners.equal_range(pEvent->GetEventID());

		for(EventIter cMapIter = range.first; cMapIter != range.second;
			cMapIter++)
		{
			(*cMapIter).second->m_pfCallback(pEvent, 
				pEvent->GetSender());
		}
		CMemoryManager::GetInstance()->Deallocate((char*)pEvent, HEAPID_EVENT);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::ClearEvents()
{
	set<IEvent*, less<IEvent*>, CEventAllocator<IEvent*>>::iterator cIter;
	cIter = m_cEventList.begin();
	while(cIter != m_cEventList.end())
	{
		// Delete because the Event Manager is responsible for cleaning dynamic
		// memory.
		CMemoryManager::GetInstance()->Deallocate((char*)(*cIter), HEAPID_EVENT);
		cIter++;
	}
	
	// Just in case we missed something
	m_cEventList.clear();
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::ProcessEvents()
{
	set<IEvent*, less<IEvent*>, CEventAllocator<IEvent*>>::iterator cSetIter;
	set<IEvent*, less<IEvent*>, CEventAllocator<IEvent*>>::iterator cPrevIter;
	cSetIter = m_cEventList.begin();
	while(!m_cEventList.empty())
	{
		pair<EventIter, EventIter> range;

		range = m_cListeners.equal_range((*cSetIter)->GetEventID());

		for(EventIter cMapIter = range.first; cMapIter != range.second;
			cMapIter++)
		{
			cMapIter->second->m_pfCallback((*cSetIter), 
				cMapIter->second->m_pcListener);
		}
		CMemoryManager::GetInstance()->Deallocate((char*)(*cSetIter),
			HEAPID_EVENT);

		cPrevIter = cSetIter;
		cSetIter++;
		m_cEventList.erase(cPrevIter);
	}
}	

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::Shutdown()
{
	ClearEvents();
	m_cListeners.clear();
}