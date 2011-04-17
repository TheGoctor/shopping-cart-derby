////////////////////////////////////////////////////////////////////////////////
//	File			:	CRamEvent.h
//	Date			:	4/12/11
//	Mod. Date		:	4/12/11
//	Mod. Initials	:	RN
//	Author			:	Ray Nieves
//	Purpose			:	The event is sent to the Level Manager to notify them
//						of a goal item being collected
////////////////////////////////////////////////////////////////////////////////

#ifndef _CRAMEVENT_H_
#define _CRAMEVENT_H_

#include "IEvent.h"
#include "..\..\..\CObject.h"

class CRamEvent : public IEvent
{
	CObject* m_pcRammer;
	CObject* m_pcGuyWhoWasRammed;

public:
	// Constructor
	CRamEvent(unsigned int nEventID, IComponent* pcSender,
		CObject* pcRammer, CObject* pcWasRammed) : IEvent(nEventID, pcSender)
	{
		m_pcRammer = pcRammer;
		m_pcGuyWhoWasRammed = pcWasRammed;
	}

	CObject* GetRammer(void)
	{
		return m_pcRammer;
	}
	CObject* GetWhoWasRammed(void)
	{
		return m_pcGuyWhoWasRammed;
	}
};

#endif // _CGOALITEMEVENT_H_