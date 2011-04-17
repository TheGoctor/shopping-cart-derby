////////////////////////////////////////////////////////////////////////////////
//	File			:	CStateEvent.h
//	Date			:	4/5/11
//	Mod. Date		:	4/5/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	The event that handles state transitions.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CSTATEEVENT_H_
#define _CSTATEEVENT_H_

#include "IEvent.h"
#include "../State Manager/CStateManager.h"

class CStateEvent : public IEvent
{
	EGameState m_eFromState;
	EGameState m_eToState;

public:
	// Constructor
	CStateEvent(unsigned int nEventID, IComponent* pcSender, EGameState 
		eFromState, EGameState eToState) : IEvent(nEventID, pcSender)
	{
		m_eFromState = eFromState;
		m_eToState = eToState;
	}

	inline EGameState GetFromState()
	{
		return m_eFromState;
	}

	inline EGameState GetToState()
	{
		return m_eToState;
	}
};

#endif