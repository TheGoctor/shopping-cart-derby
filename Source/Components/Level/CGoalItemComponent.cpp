#include "CGoalItemComponent.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CGoalItemEvent.h"

CGoalItems::CGoalItems(CObject* pObj) : m_pObject(pObj)
{
}

CGoalItems::~CGoalItems()
{
}

void CGoalItems::Init(EGoalItemType eType)
{
	m_eType = eType;
	//m_fDespawnTimer = 5.0f;
	m_bDespawning = false;
	m_bSpawned = false;
}

void CGoalItems::SetDespawnTimer(const float fDespawnTime)
{
	m_fDespawnTimer = fDespawnTime;
	m_bDespawning = true;
}

void CGoalItems::Update(const float fDT)
{
	if(m_bDespawning == false)
		return;

	m_fDespawnTimer -= fDT;

	if(m_fDespawnTimer <= 0.0f)
	{
		CGoalItemEvent* pcGoalItem = MMNEWEVENT(CGoalItemEvent) CGoalItemEvent(CIDGen::GetInstance()->
			GetID("Despawned"), NULL, this);

		CEventManager::GetInstance()->PostEvent(pcGoalItem, PRIORITY_NORMAL);
	}
}