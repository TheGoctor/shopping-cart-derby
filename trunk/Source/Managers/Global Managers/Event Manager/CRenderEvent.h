// FILE HEADER
#ifndef _CRENDEREVENT_H_
#define _CRENDEREVENT_H_

#include "IEvent.h"

class CObject;

class CRenderEvent : public IEvent
{
	CObject* m_pParent;
public:
	CRenderEvent(unsigned int nEventID, IComponent* pcSender, CObject* pParent) 
		: IEvent(nEventID, pcSender)
	{
		m_pParent = pParent;
	}

	inline CObject* GetParent()
	{
		return m_pParent;
	}
};

#endif