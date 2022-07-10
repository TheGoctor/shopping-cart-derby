/*******************************************************************************
 * Filename:  		CSpawningManager.h
 * Date:      		05/04/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This manager will be used to
 *					spawn items on the level and
 *					cycle through them.
 ******************************************************************************/
#pragma once

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "core/containers.h"
#include "memory/allocator.h"


namespace scd {

struct spawn_location
{
	std::shared_ptr<scd::object> endcap;
	scd::transform pose;
	bool is_used;
};

class spawning_manager
{
public:
	/**
	* @brief Registers for all needed events and retrieves models and textures
	* needed for items. Also acknowledges any departments we will use this game.
	*/
	void init();

	/*****************************************************************
	* UpdateTimer()	Updates spawn timer. Calls SpawnGoalItem().
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void UpdateTimer(const float fDT);

	/*****************************************************************
	* CreateGoalItemComponent()	Creates a GoalItemComponent and stores
	*							it in the map of GoalItems.
	*
	* Ins:						Lua* / scd::object* - Parent, EGoalItemType
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	int CreateGoalItemComponent(lua_State* pLua);
	CGoalItems* CreateGoalItemComponent(scd::object* pObj, EGoalItemType);

	/*****************************************************************
	* CreateHeldItemComponent()	Creates a HeldItemComponent and stores
	*							it in the map of HeldItems.
	*
	* Ins:						Lua* / scd::object* - Parent
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	int CreateHeldItemComponent(lua_State* pLua);
	CHeldItemComponent* CreateHeldItemComponent(scd::object* pObj);

	/*****************************************************************
	* CreateDepartmentComponent()	Creates a Department and stores
	*							it in the map of Departments.
	*
	* Ins:						Lua* / scd::object* - Parent, EDepartment
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	int CreateDepartmentComponent(lua_State* pLua);
	CDepartment* CreateDepartmentComponent(scd::object* pObj, EDepartment eType);

	/*****************************************************************
	* Shutdown()	Deletes all memory and clears all containers.
	*
	* Ins:				IEvent*		-
	*					scd::base_component*	-
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Shutdown(IEvent*, scd::base_component*);

	/*****************************************************************
	* Update()	Update call
	*
	* Ins:				IEvent*		-	TUpdateStateEvent*
	*					scd::base_component*	-
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Update(IEvent*, scd::base_component*);

	/*****************************************************************
	* Despawned()	Is called from GoalItems. Then tells the department
	*				to despawn itself.
	*
	* Ins:				IEvent*		-	TGoalItemEvent*
	*					scd::base_component*	-
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Despawned(IEvent*, scd::base_component*);

	/*****************************************************************
	* GoalItemCollected()	Called when a GoalItem is collected. Tells
	*						the Department to despawn.
	*
	*
	* Ins:				IEvent*		-	TGoalItemCollectedEvent*
	*					scd::base_component*	-
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemCollected(IEvent*, scd::base_component*);

	/*****************************************************************
	* GoalItemCollision()	Called when a Goal Item is collided with.
	*						Sends "GoalItemCollected" event if the
	*						Goal Item isn't collidable.
	*
	* Ins:				IEvent*		-	TGoalItemCollectedEvent*
	*					scd::base_component*	-
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemCollision(IEvent*, scd::base_component*);

	/*****************************************************************
	* DestroyObject()	Destroys the passed in Goal Item.
	*
	* Ins:				IEvent*		-	TObjectEvent*
	*					scd::base_component*	-
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void DestroyObject(IEvent*, scd::base_component*);

	/*****************************************************************
	* HandleInitObjects()	Sets the init goal item wave spawner.
	*
	* Ins:				IEvent*		-
	*					scd::base_component*	-
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void HandleInitObjects(IEvent*, scd::base_component*);

	/*****************************************************************
	* HandleStartUpdate()	Sets the m_bGameHasntStartedYet to false.
	*
	* Ins:				IEvent*		-
	*					scd::base_component*	-
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void HandleStartUpdate(IEvent*, scd::base_component*);

	/////////////////////////////////
	// Accessors
	/////////////////////////////////
	inline static CSpawningManager* GetInstance()
	{
		static CSpawningManager cSpawningManager;
		return &cSpawningManager;
	}
	EGoalItemType GetGoalItemType(scd::object* pcObject);
	EHeldItemType GetHeldItemType(scd::object* pcObject);
	bool IsSpawned(EGoalItemType eType);
	int GetGoalItemMeshID(int nItemID);

	/////////////////////////////////
	// Mutators
	/////////////////////////////////
	inline void AddHeldItemSpawnLocation(TSpawnLocation tSpawnLocation)
	{
		m_cHeldItemLocations.push_back(tSpawnLocation);
	}

	inline string GetGoalItemName(int nIndex)
	{
		return m_szGoalItemNames[nIndex];
	}

	inline unsigned GetGoalItem(int nIndex)
	{
		return m_nGoalItems[nIndex];
	}

	inline unsigned GetGoalItemRenderContext(int nIndex)
	{
		return m_nGoalItemRenderContexts[nIndex];
	}

	inline unsigned GetHeldItemMeshID(int nIndex)
	{
		return m_nHeldItemMeshIDs[nIndex];
	}

	inline unsigned GetHeldItemRenderCompID(int nIndex)
	{
		return m_nHeldItemRenderCompIDs[nIndex];
	}

private:
	friend class CGoalItems;
	friend class CHeldItemComponent;

	struct TNode
	{
		char szName[40];
		scd::transform tLocalMatrix;
		scd::transform tWorldMatrix;
	};

	// Each Goal Item that is in the level
	map<unsigned int, CGoalItems*, less<unsigned int>,
		scd::allocator<pair<unsigned int, CGoalItems*>>> m_cGoalItems;

	// Each Held Item that is in the level
	map<unsigned int, CHeldItemComponent*, less<unsigned int>,
		scd::allocator<pair<unsigned int, CHeldItemComponent*>>> m_cHeldItems;

	// List of each department in the level
	map<EDepartment, CDepartment*, less<unsigned int>,
		scd::allocator<pair<EDepartment, CDepartment*>>> m_cDepartments;

	// List of all the locations that Held Items can spawn
	list<TSpawnLocation, scd::allocator<TSpawnLocation>> m_cHeldItemLocations;

	string m_szGoalItemNames[MAX_GOAL_ITEMS];
	int m_nTotalSpawnedHeldItems;
	const unsigned m_nMaxHeldItems;
	unsigned m_nGoalItems[MAX_GOAL_ITEMS];
	unsigned m_nGoalItemRenderContexts[MAX_GOAL_ITEMS];
	unsigned m_nHeldItemMeshIDs[MAX_HELD_ITEMS];
	unsigned m_nHeldItemRenderCompIDs[MAX_HELD_ITEMS];
	unsigned m_nCounter;
	const float m_fGoalItemWaveTime;
	float m_fGoalItemSpawnTimer;
	bool m_bGameHasntStartedYet;

	// Constructor
	CSpawningManager();

	/////////////////////
	// Trilogy of Evil //
	/////////////////////
	~CSpawningManager();
	CSpawningManager(const CSpawningManager&) :
		m_fGoalItemWaveTime(0), m_nMaxHeldItems(0) {}
	CSpawningManager& operator=(const CSpawningManager&) {}

	/*****************************************************************
	* SpawnGoalItem()	Picks a psudeo random department to spawn a
	*					goal item in and tells it to spawn.
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void SpawnGoalItem();

	/*****************************************************************
	* InitGoalItems()	Creates all the Departments
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void InitGoalItems();

	/*****************************************************************
	* InitHeldItems()	Creates all the held items
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void InitHeldItems();

	/*****************************************************************
	* LoadGoalItemLocations()	Opens GoalItems.nm and reads in all the
	*							spawn points for goal items
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void LoadGoalItemLocations();

	/*****************************************************************
	* LoadHeldItemLocations()	Gets data from LevelManager.cpp and
	*							saves all the positions of endcaps
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/02/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void LoadHeldItemLocations();
};

} // namespace scd
