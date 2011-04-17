// FILE HEADER
#ifndef _CGOALITEMS_H_
#define _CGOALITEMS_H_

#include "..\..\IComponent.h"
#include "..\..\Managers\Global Managers\Event Manager\CGoalItemEvent.h"

class CObject;

class CGoalItems : public IComponent
{
	
public:

	EGoalItemType m_eType;
	CObject* m_pObject;
	bool m_bSpawned;
	bool m_bDespawning;
	float m_fDespawnTimer;

	// Constructor
	CGoalItems(CObject* pObj);

	// Destructor
	~CGoalItems();

	void Init(EGoalItemType);

	void Update(const float fDespawnTime);
	void SetDespawnTimer(const float fDT);

	inline CObject* GetParent(void)
	{
		return m_pObject;
	}
};

#endif