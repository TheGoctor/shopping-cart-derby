/*******************************************************************************
 * Filename:  		CLevelManager.h
 * Date:      		04/09/2011
 * Mod. Date: 		04/12/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This manager will be used to
 *					spawn items on the level and
 *					cycle through them.
 ******************************************************************************/

#ifndef _CLEVELMANAGER_H_
#define _CLEVELMANAGER_H_

#include <D3dx9math.h>
#include <list>
#include <map>
#include <string>
using std::list;
using std::map;
using std::string;

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Global Managers\Memory Manager\CAllocator.h"
#include "..\Global Managers\Event Manager\CGoalItemEvent.h"

class CEventManager;
class CGoalItems;
class IEvent;
class IComponent;
class CObject;
class CRenderComponent;

class CLevelManager
{
private:

	struct CNode
	{
		char tName[40];
		D3DXMATRIX tLocalMatrix;
		D3DXMATRIX tWorldMatrix;

	};

	list<CGoalItems*, CAllocator<CGoalItems*>> m_cGoalItemsMasterList;
	list<CGoalItems*, CAllocator<CGoalItems*>> m_cGoalItemsPool;
	list<CGoalItems*, CAllocator<CGoalItems*>> m_cSpawnedGoalItems;
	list<CGoalItems*, CAllocator<CGoalItems*>> m_cDespawningGoalItems;

	list<CRenderComponent*, CAllocator<CRenderComponent*>> m_cLevelRenderComps;

	list<D3DXVECTOR3> m_cHeldItemLocations;

	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>> m_cGoalItems;

	string m_szGoalItem;
	const float m_fWaveTime;
	float m_fSpawnTimer;
	unsigned m_nCounter;

	
	int	m_objectcount;

	CNode* m_levelNodes;

	// Constructor
	CLevelManager();

	// Destructor
	~CLevelManager();

	void SpawnGoalItem();
	void BuildLevel(char* szNodeMap);
	bool AddCollision(char* szMeshName);

public:
	
	static CLevelManager* GetInstance()
	{
		static CLevelManager cLevelManager;
		return &cLevelManager;
	}

	void Init();
	static void Shutdown(IEvent*, IComponent*);
	static void Update(IEvent*, IComponent*);
	static void Despawned(IEvent*, IComponent*);
	static void GoalItemCollected(IEvent*, IComponent*);

	// Accessors
	EGoalItemType GetGoalItemType(CObject* pcObject);

	// Mutators

	int CreateGoalItemComponent(lua_State* pLua);
	CGoalItems* CreateGoalItemComponent(CObject* pObj, EGoalItemType);

	void UpdateTimer(const float fDT);
	void DespawnGoalItem(CGoalItems* pcGoalItem);
	void SetDespawnTimer(const float fDespawnTime);
};

#endif