////////////////////////////////////////////////////////////////////////////////
//	File			:	CGoalItemCollected.h
//	Date			:	4/12/11
//	Mod. Date		:	4/12/11
//	Mod. Initials	:	RN
//	Author			:	Ray Nieves
//	Purpose			:	The event is sent to the Level Manager to notify them
//						of a goal item being collected
////////////////////////////////////////////////////////////////////////////////

#ifndef _CGOALITEMCOLLECTED_H_
#define _CGOALITEMCOLLECTED_H_

#include "IEvent.h"
//#include "..\..\Components\Level\CGoalItemComponent.h"
#include "..\..\..\CObject.h"

class CGoalItemCollectedEvent : public IEvent
{
	CObject* m_pcGoalItem;
	CObject* m_pcCollector;

public:
	// Constructor
	CGoalItemCollectedEvent(unsigned int nEventID, IComponent* pcSender,
		CObject* pcGoalItem, CObject* pcCollector) : IEvent(nEventID, pcSender)
	{
		m_pcGoalItem = pcGoalItem;
		m_pcCollector = pcCollector;
	}

	inline CObject* GetGoalItem()
	{
		return m_pcGoalItem;
	}

	inline CObject* GetCollector()
	{
		return m_pcCollector;
	}

	inline int GetCollectorType()
	{
		if( 0 == _stricmp((char*)m_pcCollector->GetID(), "Player") )
		{
			return 0;
		}

		if( 0 == _stricmp((char*)m_pcCollector->GetID(), "Agent1") )
		{
			return 1;
		}

		if( 0 == _stricmp((char*)m_pcCollector->GetID(), "Agent2") )
		{
			return 2;
		}

		if( 0 == _stricmp((char*)m_pcCollector->GetID(), "Agent3") )
		{
			return 3;
		}

		return -1;
	}
};

#endif // _CGOALITEMEVENT_H_