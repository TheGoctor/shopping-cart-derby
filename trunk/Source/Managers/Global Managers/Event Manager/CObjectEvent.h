////////////////////////////////////////////////////////////////////////////////
//	File			:	CObjectEvent.h
//	Date			:	4/9/11
//	Mod. Date		:	4/14/11
//	Mod. Initials	:	JS
//	Author			:	Jesse Stanciu
//	Purpose			:	The event is sent to the AI and the HUD to notify them
//						of an object spawning or despawning
////////////////////////////////////////////////////////////////////////////////

#ifndef _COBJECTEVENT_H_
#define _COBJECTEVENT_H_

#include "IEvent.h"

class CObject;

class CObjectEvent : public IEvent
{
	CObject* m_pcObj;

public:
	// Constructor
	CObjectEvent(unsigned int nEventID, IComponent* pcSender, 
		CObject* pcObj) : IEvent(nEventID, pcSender)
	{
		m_pcObj = pcObj;
	}

	inline CObject* GetObject()
	{
		return m_pcObj;
	}
};

#endif