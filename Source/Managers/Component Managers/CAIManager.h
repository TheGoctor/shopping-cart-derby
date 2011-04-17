/*******************************************************************************
 * Filename:  		CAIManager.h
 * Date:      		04/04/2011
 * Mod. Date: 		04/14/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This is the manager for the
				CAIComponent components. Will hold a pointer
				to each of the CAIComponent objects and
				any static information they need.
 ******************************************************************************/

#ifndef _CAIMANAGER_H_
#define _CAIMANAGER_H_

#include <map>
#include <D3dx9math.h>
#include <list>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Global Managers\Memory Manager\CAllocator.h"

class CAIComponent;
class CEventManager;
class CObject;
class IEvent;
class IComponent;
struct TNode;
struct TVertex;
struct TEdge;

class CAIManager
{
private:

	struct TNode
	{
		TNode* m_pcParent;
		TVertex* m_pcVertex;
	};
	struct TVertex
	{
		D3DXVECTOR3 m_cLocation;
		std::list<TEdge*> m_pcConnections;
	};
	struct TEdge
	{
		double m_Cost;
		TVertex* m_pcEndpoint;
	};

	//struct TNode
	//{
	//	std::list<TNode*> m_tNeighbors;
	//	D3DXVECTOR3 m_cLocation;
	//	double m_Cost;
	//	double m_Heuristic;
	//	double m_FinalCost;
	//	TNode* m_tParent;
	//};

	D3DXVECTOR3 cDepartmentPos[9];

	/////////////////
	// Constructor //
	/////////////////
	CAIManager();

	/////////////////////
	// Trilogy of Evil //
	/////////////////////
	~CAIManager() {}
	CAIManager(const CAIManager&) {}
	CAIManager& operator=(const CAIManager&) {}

public:

	// Will call allocate to create
	// the amount of AI agents needed
	std::map<unsigned int, CAIComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAIComponent*>>> m_cAIComponents;

	std::list<CObject*,	CAllocator<CObject*>> m_cPlayers;

	enum EDepartments { };

	static CAIManager* GetInstance()
	{
		static CAIManager AIManager;

		return &AIManager;
	}

	void Init();

	static int CreateAIComponent(lua_State* pLua);

	static CAIComponent* CreateAIComponent(CObject* pObj);

	/***************
	*  Accessors
	***************/

	/***************
	* Mutators
	***************/

	/*****************************************************************
	***************************** Events *****************************
	*****************************************************************/
	static void GoalItemInit(IEvent*, IComponent*);

	static void PlayerCreated(IEvent*, IComponent*);

	/*****************************************************************
	* GoalItemCollected()	An event that tells the agent that a
	*						Goal Item has been collected
	*
	* Ins:					cPlayer - Player who collected the item
	*						cGoalItem - The Goal Item collected
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/08/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemCollected(IEvent*, IComponent*);

	/*****************************************************************
	* PlayerAttacked()	An event that tells the agent that a
	*					player is being aggressive
	*
	* Ins:					cAggressor - Player who was aggressive
	*						cVictim - Play who was attacked
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/08/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	static void PlayerAttacked(IEvent*, IComponent*);

	/*****************************************************************
	* GoalItemSpawned()	An event that tells the agent that a
	*					Goal Item has spawned
	*
	* Ins:					cGoalItem - The item that spawned
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/08/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemSpawned(IEvent*, IComponent*);

	/*****************************************************************
	* GoalItemSpawned()	An event that tells the agent that a
	*					Goal Item has spawned
	*
	* Ins:					cGoalItem - The item that despawned
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/08/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemDespawned(IEvent*, IComponent*);

	/*****************************************************************
	* Update()	Update calls any function that needs calling
	*
	* Ins:		fDT - delta time
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/04/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Update(IEvent*, IComponent*);

	/*****************************************************************
	* Shutdown()	Shuts down the component
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/04/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Shutdown(IEvent*, IComponent*);
};

#endif	// _CAIMANAGER_H_