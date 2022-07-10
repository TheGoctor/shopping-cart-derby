///////////////////////////////////////////////////////////////////////////////
//  File			:	"CInventoryManager.h"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/17/11
//
//	Last Changed	:	05/17/11
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
}

int CInventoryManager::CreateInventoryComp(lua_State* pLua)
{
	// Create the Obj
	CObject* pObj = (CObject*)lua_topointer(pLua, -1);
	
	// Create Component
	/*CInventoryComponent* pSpriteComp = */CreateInventoryComp(pObj);

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



CInventoryComponent* CInventoryManager::GetInventoryComponent(CObject* pObj)
{
	list<CInventoryComponent*, CAllocator<CInventoryComponent*>>::iterator iter;
	iter = m_cInvComps.begin();

	while(iter != m_cInvComps.end())
	{
		if((*iter)->GetParent() == pObj)
		{
			return (*iter);
		}
		iter++;
	}

	// didn't find it
	return NULL;
}