////////////////////////////////////////////////////////////////////////////////
//	File			:	CUpdateState.h
//	Date			:	4/5/11
//	Mod. Date		:	4/5/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	The event is sent to the State Manager to begin updating
//						game objects.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CUPDATESTATEEVENT_H_
#define _CUPDATESTATEEVENT_H_

#include "IEvent.h"

class CUpdateStateEvent : public IEvent
{
	float m_fDeltaTime;

public:
	// Constructor
	CUpdateStateEvent(unsigned int nEventID, IComponent* pcSender, 
		float fDeltaTime) : IEvent(nEventID, pcSender), m_fDeltaTime(fDeltaTime)
	{
	}

	inline float GetDeltaTime()
	{
		return m_fDeltaTime;
	}
};

#endif