////////////////////////////////////////////////////////////////////////////////
//	File			:	IEvent.h
//	Date			:	3/31/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Encapsulates the data for an event such as, the event ID
//						and the parameter. CEventManager will mainly use this
//						class but classes that include CEventManager will also
//						be able to use it.
////////////////////////////////////////////////////////////////////////////////

#ifndef _IEVENT_H_
#define _IEVENT_H_
#define NULL 0

class IComponent;

class IEvent
{
private:
	unsigned int	m_nEventID;		// The ID used to identify this event
	IComponent*		m_pcSender;		// The component sending the event
	unsigned int	m_nPriority;	// When the event gets processed
	void*			m_pData;		// Pointer to a structure containing additional data

	friend class CEventManager;

protected:
	// Mutator
	void SetPriority(unsigned int nPriority)
	{
		m_nPriority = nPriority;
	}

public:
	// Constructor
	IEvent(unsigned int nEventID, IComponent* pcSender, void* pData = NULL)
	{
		m_nEventID = nEventID;
		m_pcSender = pcSender;
		m_pData = pData;
	}

	// Destructor
	~IEvent()
	{
		MMDELEVENT(m_pData);
	}

	// Accessors
	unsigned int GetEventID()
	{
		return m_nEventID;
	}

	IComponent* GetSender()
	{
		return m_pcSender;
	}

	void* GetData()
	{
		return m_pData;
	}
};

#endif