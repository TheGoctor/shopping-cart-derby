///////////////////////////////////////////////////////////////////////////////
//  File			:	"CInventoryManager.h"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/17/11
//
//	Last Changed	:	04/17/11
//
//  Purpose			:	A manager for inventory components
///////////////////////////////////////////////////////////////////////////////

// Includes
#include "CInventoryManager.h"
#include "CSpawningManager.h"
#include "..\\Global Managers\\Event Manager\\CEventManager.h"
#include "..\\Global Managers\\Event Manager\\EventStructs.h"
#include "..\\..\\CObject.h"
using namespace EventStructs;

void CInventoryManager::Init(void)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	

	pEM->RegisterEvent("PlayerRammed",
		(IComponent*)GetInstance(), PlayerRammedCallback);
}

int CInventoryManager::CreateInventoryComp(lua_State* pLua)
{
	// Create the Obj
	CObject* pObj = (CObject*)lua_topointer(pLua, -1);
	
	// Create Component
	CInventoryComponent* pSpriteComp = CreateInventoryComp(pObj);

	// End LUA
	lua_pop(pLua, 1);
	return 0;
}

CInventoryComponent* CInventoryManager::CreateInventoryComp(CObject* pParent)
{
	static int nPlayer = 0; // player this belongs to
	// Create Component
	CInventoryComponent *pComp = MMNEW(CInventoryComponent(pParent, nPlayer));	

	nPlayer++;

	// Insert Component
	GetInstance()->m_cInvComps.push_back(pComp);

	// Add to Parent
	pParent->AddComponent(pComp);

	// Return Component
	return pComp;
}

// Singleton Access
CInventoryManager* CInventoryManager::GetInstance(void)
{
	static CInventoryManager cInventoryManager;
	return &cInventoryManager;
}

// Callbacks
void CInventoryManager::GoalItemCollectedCallback(IEvent* e, IComponent* comp)
{
	// Get the Values from the Event
	/*TGoalItemEvent* pcObjEvent = static_cast<TGoalItemEvent*>(e->GetData());
	EGoalItemType eType = CSpawningManager::GetInstance()->GetGoalItemType(pcObjEvent->m_pcGoalItem);
	CObject* pCollector = pcObjEvent->m_pcCollector;


	// Components
	list<CInventoryComponent*, CAllocator<CInventoryComponent*>>
		::iterator cIter = GetInstance()->m_cInvComps.begin();

	while(cIter != GetInstance()->m_cInvComps.end())
	{
		CInventoryComponent* pIC = *cIter;
		if(pCollector == pIC->GetParent())
		{
			pIC->GoalItemCollected(eType);
		}

		cIter++;
	}/**/
}

void CInventoryManager::PlayerRammedCallback(IEvent* e, IComponent* comp)
{
	// This should be moved to the inventorycomponent instead of the manager, no?
	TRamEvent* pEvent = (TRamEvent*)e->GetData();

	list<CInventoryComponent*, CAllocator<CInventoryComponent*>>
		::iterator iter = GetInstance()->m_cInvComps.begin();

	while(iter != GetInstance()->m_cInvComps.end())
	{
		if((*iter)->GetParent() == pEvent->m_pcRammee)
		{
			(*iter)->Rammed(pEvent->m_pcRammer, pEvent->m_pcRammee);
		}
		iter++;
	}
}