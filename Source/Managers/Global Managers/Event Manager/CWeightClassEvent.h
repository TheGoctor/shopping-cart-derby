////////////////////////////////////////////////////////////////////////////////
//	File			:	CWeightClasEvent.h
//	Date			:	4/9/11
//	Mod. Date		:	4/9/11
//	Mod. Initials	:	MS
//	Author			:	Mac Smith
//	Purpose			:	The event that sends WeightClass commands.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CWEIGHTCLASSEVENT_H_
#define _CWEIGHTCLASSEVENT_H_

#include "IEvent.h"
#include "..\..\Component Managers\CSteeringManager.h"

class IComponent;

class CWeightClassEvent : public IEvent
{
private:
	ECartWeight	m_eWeightClass;

public:
	CWeightClassEvent(unsigned int nEventID, IComponent* pcSender,
		ECartWeight eWeightClass)
		: IEvent(nEventID, pcSender)
	{
		m_eWeightClass = eWeightClass;
	}

	inline ECartWeight GetWeightClass()
	{
		return m_eWeightClass;
	}
};

#endif