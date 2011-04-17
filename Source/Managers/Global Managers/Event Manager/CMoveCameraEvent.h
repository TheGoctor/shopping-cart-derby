////////////////////////////////////////////////////////////////////////////////
//	File			:	CMoveCameraEvent.h
//	Date			:	4/12/11
//	Mod. Date		:	4/12/11
//	Mod. Initials	:	MS
//	Author			:	Mac Smith
//	Purpose			:	The event that sends MoveCamera commands.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CMOVECAMERAEVENT_H_
#define _CMOVECAMERAEVENT_H_


#include <D3dx9math.h>

#include "IEvent.h"


class IComponent;

class CMoveCameraEvent : public IEvent
{
private:
	D3DXVECTOR3		m_vObjPos;
	float			m_fObjSpeed;
	float			m_fTurnRate;

public:
	CMoveCameraEvent(unsigned int nEventID, IComponent* pcSender, D3DXVECTOR3 vObjPos,
		float fObjSpeed, float fTurnRate) : IEvent(nEventID, pcSender), 
		m_vObjPos(vObjPos), m_fObjSpeed(fObjSpeed), m_fTurnRate(fTurnRate)
	{

	}

	inline D3DXVECTOR3 GetPosition()
	{
		return m_vObjPos;
	}

	inline float GetSpeed()
	{
		return m_fObjSpeed;
	}
	
	inline float GetTurnRate()
	{
		return m_fTurnRate;
	}
};

#endif // _CMOVECAMERAEVENT_H_