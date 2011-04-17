////////////////////////////////////////////////////////////////////////////////
//	File			:	CGoalItemEvent.h
//	Date			:	4/12/11
//	Mod. Date		:	4/14/11
//	Mod. Initials	:	JS
//	Author			:	Jesse Stanciu
//	Purpose			:	The event is sent to the AI and the HUD to notify them
//						of an object spawning or despawning
////////////////////////////////////////////////////////////////////////////////

#ifndef _CGOALITEMEVENT_H_
#define _CGOALITEMEVENT_H_

#include "IEvent.h"

class CGoalItems;

enum EGoalItemType { NO_ITEM = -1, GORILLA_MILK, CAPTAIN_FLAKEY, AGENT_ORANGE, BAKON, KILLER_TOMATO_SOUP, SAD_APPLE,
	FRENCHY_BREAD, SAMMY_SANDWICH, MAX_ITEMS};

class CGoalItemEvent : public IEvent
{
	CGoalItems* m_pcGoalItem;
	EGoalItemType m_eGoalItemType;

public:
	// Constructor
	CGoalItemEvent(unsigned int nEventID, IComponent* pcSender, 
		CGoalItems* pcGoalItem, EGoalItemType eGoalItemType = NO_ITEM) : IEvent(nEventID, pcSender)
	{
		m_pcGoalItem = pcGoalItem;
		m_eGoalItemType = eGoalItemType;
	}

	inline CGoalItems* GetGoalItem()
	{
		return m_pcGoalItem;
	}

	inline EGoalItemType GetGoalItemType()
	{
		return m_eGoalItemType;
	}
};

#endif // _CGOALITEMEVENT_H_