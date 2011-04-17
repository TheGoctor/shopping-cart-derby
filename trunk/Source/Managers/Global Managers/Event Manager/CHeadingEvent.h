////////////////////////////////////////////////////////////////////////////////
//	File			:	CHeadingEvent.h
//	Date			:	4/9/11
//	Mod. Date		:	4/9/11
//	Mod. Initials	:	MS
//	Author			:	Mac Smith
//	Purpose			:	The event that sends Heading commands.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CHEADINGEVENT_H_
#define _CHEADINGEVENT_H_

#include <D3dx9math.h>


#include "IEvent.h"

class IComponent;

class CHeadingEvent : public IEvent
{
private:
	D3DXVECTOR3	m_vHeading;

public:
	CHeadingEvent(unsigned int nEventID, IComponent* pcSender, D3DXVECTOR3 vHeading) : IEvent(nEventID, pcSender)
	{
		m_vHeading = vHeading;
	}

	inline D3DXVECTOR3& GetHeading()
	{
		return m_vHeading;
	}
};

#endif