#include "CEventManager.h"
#include <assert.h>
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
	static CEventManager cEventManager;
	return &cEventManager;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
EventID CEventManager::RegisterEvent(string szEventName, IComponent* pcListener,
									 void(*pfCallback)(IEvent*, IComponent*))
{
	// Convert string to int
	EventID nEventID = CIDGen::GetInstance()->GetID(szEventName);

	// Register the Listener
	RegisterEvent(nEventID, pcListener, pfCallback);

	return nEventID;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::RegisterEvent(EventID nEventID, IComponent* pcListener,
								  void(*pfCallback)(IEvent*, IComponent*))
{
	// Make sure someone is actually listening
	assert(pcListener != NULL);

	// Make sure we don't re-register the same listener for the same event
	pair<EventIter, EventIter> range;
	range = m_cListeners.equal_range(nEventID);

	for(EventIter cMapIter = range.first; cMapIter != range.second;
		cMapIter++)
	{
		if(cMapIter->second->m_pcListener == pcListener &&
			cMapIter->second->m_pfCallback == pfCallback)
			return;	// The Listener is already registered
	}

	// Make Listener Object
	TListener* ptListener = MMNEWEVENT(TListener);
	ptListener->m_pcListener = pcListener;
	ptListener->m_pfCallback = pfCallback;

	// use generated int to pair with listener into map
	// and add listener to map
	m_cListeners.insert(make_pair(nEventID, ptListener));
}

void CEventManager::UnregisterEvents()
{
	while(m_cUnregisterList.empty() == false)
	{
		TUnregister& pEvent = m_cUnregisterList.front();
		if(pEvent.nEventID == 0)
		{
			ActuallyUnregisterEventAll(pEvent.pListener);
		}
		else
		{
			ActuallyUnregisterEvent(pEvent.nEventID, pEvent.pListener);
			pEvent.pListener->SetIsActive(true);
		}

		m_cUnregisterList.pop_front();
	}
}

void CEventManager::ActuallyUnregisterEvent(EventID nEventID, IComponent* pcListener)
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

void CEventManager::ActuallyUnregisterEventAll(IComponent* pcListener)
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
void CEventManager::UnregisterEvent(EventID nEventID, IComponent* pcListener)
{
	TUnregister pEvent;
	pEvent.nEventID = nEventID;
	pEvent.pListener = pcListener;
	pcListener->SetIsActive(false);
	m_cUnregisterList.push_back(pEvent);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::UnregisterEventAll(IComponent* pcListener)
{
	TUnregister pEvent;
	pEvent.nEventID = 0;
	pEvent.pListener = pcListener;
	pcListener->SetIsActive(false);
	m_cUnregisterList.push_back(pEvent);
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
			//if(cMapIter->second->m_pcListener->GetIsActive())
			{
				cMapIter->second->m_pfCallback(pEvent, 
					cMapIter->second->m_pcListener);
			}
		}
		MMDELEVENT(pEvent);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::ClearEvents()
{
	set<IEvent*, cmp, CEventAllocator<IEvent*>>::iterator cIter;
	cIter = m_cEventList.begin();
	while(cIter != m_cEventList.end())
	{
		// Delete because the Event Manager is responsible for cleaning dynamic
		// memory.
		MMDEL(*cIter);
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
	while(!m_cEventList.empty())
	{
		IEvent* pEvent = *m_cEventList.begin();

		// Remove from the list now to avoid problems with sorting new events
		m_cEventList.erase(m_cEventList.begin());

		// Make range of listeners for the current event
		pair<EventIter, EventIter> range;
		range = m_cListeners.equal_range(pEvent->GetEventID());


		for(EventIter cMapIter = range.first; cMapIter != range.second; 
			cMapIter++)
		{
			// Call the callback function of each listener listening for this
			// event
			//if(cMapIter->second->m_pcListener->GetIsActive())
			{
				cMapIter->second->m_pfCallback(pEvent, 
					cMapIter->second->m_pcListener);
			}
		}

		// clean up the event's memory
		MMDELEVENT(pEvent);

		// Unregister any events
		UnregisterEvents();
	}
}	

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CEventManager::Shutdown()
{
	ClearEvents();
	EventIter pIter = m_cListeners.begin();
	while(pIter != m_cListeners.end())
	{
		MMDELEVENT(pIter->second);
		++pIter;
	}
	m_cListeners.clear();
}