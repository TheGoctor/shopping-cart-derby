////////////////////////////////////////////////////////////////////////////////
//	File			:	CInputEvent.h
//	Date			:	4/8/11
//	Mod. Date		:	4/8/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	The event that sends input commands.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CINPUTEVENT_H_
#define _CINPUTEVENT_H_

#include "IEvent.h"

class IComponent;

class CInputEvent : public IEvent
{
private:
	int		m_nPlayer;
	float	m_fAmount;

public:
	CInputEvent(unsigned int nEventID, IComponent* pcSender, int nPlayer, 
		float fAmount) : IEvent(nEventID, pcSender)
	{
		m_nPlayer = nPlayer;
		m_fAmount = fAmount;
	}

	inline int GetPlayer()
	{
		return m_nPlayer;
	}

	inline float GetAmount()
	{
		return m_fAmount;
	}
};

#endif