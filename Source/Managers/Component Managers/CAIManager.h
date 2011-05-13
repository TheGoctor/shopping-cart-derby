/*******************************************************************************
 * Filename:  		CAIManager.h
 * Date:      		04/04/2011
 * Mod. Date: 		05/12/2011
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
class CVertex;
class CEdge;

// Pathfinding classes
class CVertex
{
	friend class CAIManager;
	friend class CAIComponent;

protected:

	CObject* m_pcObject;
	CVertex* m_pcParent;
	D3DXVECTOR3 m_cLocation;
	double m_dGivenCost;
	std::list<CEdge*, CAllocator<CEdge*>> m_pcConnections;

	CVertex(CObject* pObj) : m_pcObject(pObj), m_pcParent(NULL), m_dGivenCost(0)
	{}
};
class CEdge
{
	friend class CAIManager;
	friend class CAIComponent;

protected:

	double m_dFinalCost;
	CVertex* m_pcEndpoint;

	CEdge() : m_dFinalCost(0), m_pcEndpoint(NULL)
	{}
};
//

class CAIManager
{
	friend class CAIComponent;

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

	struct TNode
	{
		char szName[40];
		D3DXMATRIX tLocalMatrix;
		D3DXMATRIX tWorldMatrix;
	};

	void LoadAIWaypoints();
	void ConnectWaypoints();

protected:
	D3DXVECTOR3 cDepartmentPos[9];

	std::list<CVertex*, CAllocator<CVertex*>> m_cWaypointNodes;

	// Will call allocate to create
	// the amount of AI agents needed
	std::map<unsigned int, CAIComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAIComponent*>>> m_cAIComponents;

	std::list<CObject*,	CAllocator<CObject*>> m_cPlayers;

public:
	void Init();

	static int CreateAIComponent(lua_State* pLua);

	static CAIComponent* CreateAIComponent(CObject* pObj);

	

	/***************
	*  Accessors
	***************/
	static CAIManager* GetInstance();

	/***************
	* Mutators
	***************/

	/*****************************************************************
	***************************** Events *****************************
	*****************************************************************/
	//static void GoalItemInit(IEvent*, IComponent*);

	//static void PlayerCreated(IEvent*, IComponent*);

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
	//static void PlayerAttacked(IEvent*, IComponent*);

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