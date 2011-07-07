/*******************************************************************************
 * Filename:  		CSpawningManager.h
 * Date:      		05/04/2011
 * Mod. Date: 		05/16/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This manager will be used to
 *					spawn items on the level and
 *					cycle through them.
 ******************************************************************************/

#ifndef _CSPAWNINGMANAGER_H_
#define _CSPAWNINGMANAGER_H_

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
#include "..\..\Components\Level\CHeldItemComponent.h"
#include "..\..\Components\Level\CGoalItemComponent.h"


class CEventManager;
class IEvent;
class IComponent;
class CObject;
class CRenderComponent;
class CDepartment;


struct TSpawnLocation
{
	D3DXVECTOR3 m_tPosition;
	CObject* m_cEndCap;
	D3DXMATRIX m_tRotation;
	bool m_bUsed;
};

class CSpawningManager
{
private:
	friend class CGoalItems;
	friend class CHeldItemComponent;


	struct TNode
	{
		char szName[40];
		D3DXMATRIX tLocalMatrix;
		D3DXMATRIX tWorldMatrix;
	};

	// Each Goal Item that is in the level
	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>> m_cGoalItems;

	// Each Held Item that is in the level
	map<unsigned int, CHeldItemComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CHeldItemComponent*>>> m_cHeldItems;

	// List of each department in the level
	map<EDepartment, CDepartment*, less<unsigned int>,
		CAllocator<pair<EDepartment, CDepartment*>>> m_cDepartments;

	// List of all the locations that Held Items can spawn
	list<TSpawnLocation, CAllocator<TSpawnLocation>> m_cHeldItemLocations;

	const unsigned m_nMaxHeldItems;
	const float m_fGoalItemWaveTime;
	const float m_fHeldItemWaveTime;
	int m_nTotalSpawnedHeldItems;
	float m_fGoalItemSpawnTimer;
	float m_fHeldItemSpawnTimer;
	unsigned m_nCounter;
	
	//CNode* m_levelNodes;

	// Constructor
	CSpawningManager();

	/////////////////////
	// Trilogy of Evil //
	/////////////////////
	~CSpawningManager();
	CSpawningManager(const CSpawningManager&) :
		m_fGoalItemWaveTime(0), m_fHeldItemWaveTime(0), m_nMaxHeldItems(0) {}
	CSpawningManager& operator=(const CSpawningManager&) {}

	void SpawnGoalItem();
	void SpawnHeldItem();
	void InitGoalItems();
	void InitHeldItems();
	void LoadGoalItemLocations();
	void LoadHeldItemLocations();
	void DespawnHeldItem(const CHeldItemComponent&);

public:
	
	static CSpawningManager* GetInstance()
	{
		static CSpawningManager cSpawningManager;
		return &cSpawningManager;
	}
	string m_szGoalItemNames[MAX_GOAL_ITEMS];
	unsigned m_nGoalItems[MAX_GOAL_ITEMS];
	unsigned m_nGoalItemRenderContexts[MAX_GOAL_ITEMS];

	unsigned int m_nHeldItemMeshIDs[MAX_HELD_ITEMS];
	unsigned int m_nHeldItemRenderCompIDs[MAX_HELD_ITEMS];

	void Init();
	static void Shutdown(IEvent*, IComponent*);
	static void Update(IEvent*, IComponent*);
	static void Despawned(IEvent*, IComponent*);
	static void GoalItemCollected(IEvent*, IComponent*);
	static void GoalItemCollision(IEvent*, IComponent*);
	static void DestroyObject(IEvent*, IComponent*);

	static void HandleEnableObjects(IEvent*, IComponent*);
	static void HandleDisableObjects(IEvent*, IComponent*);
	static void HandleInitObjects(IEvent*, IComponent*);

	int GetGoalItemMeshID(int nItemID);

	// Accessors
	EGoalItemType GetGoalItemType(CObject* pcObject);
	EHeldItemType GetHeldItemType(CObject* pcObject);

	// Mutators

	int CreateGoalItemComponent(lua_State* pLua);
	CGoalItems* CreateGoalItemComponent(CObject* pObj, EGoalItemType);
	int CreateHeldItemComponent(lua_State* pLua);
	CHeldItemComponent* CreateHeldItemComponent(CObject* pObj);
	int CreateDepartmentComponent(lua_State* pLua);
	CDepartment* CreateDepartmentComponent(CObject* pObj, EDepartment eType);

	void UpdateTimer(const float fDT);
	//void DespawnGoalItem(CGoalItems* pcGoalItem);
};

#endif // _CSPAWNINGMANAGER_H_