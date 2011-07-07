/*******************************************************************************
 * Filename:  		CAIManager.h
 * Date:      		04/04/2011
 * Mod. Date: 		06/04/2011
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
#include <set>
using std::set;

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Global Managers\Memory Manager\CAllocator.h"
#include "CCollisionManager.h"

class CAIComponent;
class CEventManager;
class CObject;
class IEvent;
class IComponent;

struct TTri
{
	map<int, TTri*, less<int>,
		CAllocator<pair<int, TTri*>>> m_cConnections;
	TLine tEdges[3];
	D3DXVECTOR3 tPos;
	int m_nIndice[3];

	bool IsConnected(const TTri& rhs) const
	{
		D3DXVECTOR3 a, b, c, x, y, z;
		int connections = 0;

		a = tEdges[0].cLineStart;
		b = tEdges[0].cLineEnd;
		c = tEdges[1].cLineEnd;
		x = rhs.tEdges[0].cLineStart;
		y = rhs.tEdges[0].cLineEnd;
		z = rhs.tEdges[1].cLineEnd;

		if( (a == x) ||
			(a == y) ||
			(a == z))
		{
			++connections;
		}
		if( (b == x) ||
			(b == y) ||
			(b == z))
		{
			++connections;
		}
		if( (c == x) ||
			(c == y) ||
			(c == z))
		{
			++connections;
		}
		if(connections > 1)
		{
			return true;
		}

		return false;
	}

	~TTri()
	{
		m_cConnections.clear();
	}
};

struct TPlannerNode
{
	TPlannerNode*	m_pParent;
	TTri*		m_pTriangle;

	float m_fGivenCost;
	float m_fHeuristicCost;
	float m_fFinalCost;

	TPlannerNode() : m_pParent(NULL), m_pTriangle(NULL), m_fGivenCost(0), 
		m_fHeuristicCost(0), m_fFinalCost(0)
	{}

	TPlannerNode(TPlannerNode* pParent, TTri* tTri, float fGivenCost, float fHeuristicCost) : 
	m_pParent(pParent), m_pTriangle(tTri), m_fGivenCost(fGivenCost), 
		m_fHeuristicCost(fHeuristicCost), m_fFinalCost(0)
	{
		m_fFinalCost = m_fHeuristicCost + m_fGivenCost;
	}
};

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

	// Pathfinding stuffs
	struct TPlannerNodeCmp
	{
		bool operator()(const TPlannerNode* lhs, const TPlannerNode* rhs)
		{
			if(lhs->m_fFinalCost == rhs->m_fFinalCost)
			{
				return lhs < rhs;
			}
			return lhs->m_fFinalCost < rhs->m_fFinalCost;
		}
	};

	set<TPlannerNode*, TPlannerNodeCmp, CAllocator<TPlannerNode*>>
		m_cOpen;
	map<TTri*, TPlannerNode*, less<TTri*>,
		CAllocator<pair<TTri*, TPlannerNode*>>>		m_cCreated;
	char*											m_nLookup;
	map<int, TTri*, less<int>,
		CAllocator<pair<int, TTri*>>>				m_cTris;

	CObject*										m_pNavMesh;

	bool m_bShowInfo;

	void LoadNavMesh(string szFileName);
	bool IsConnected(const TTri& lhs, const TTri& rhs) const;
	void CreateConnections(string szFileName);
	void CreateLookUpTable(string szFileName);
	int Pathplan(const int, const int);
	bool LinesEqual(const TLine& lhs, TLine& rhs);

	// x = Current, y = Target
	inline unsigned int LookUpCellAlgorithm(int x, int y)
	{
		return y * m_cTris.size() + x;
	}

	D3DXVECTOR3 ClosestPtPointTriangle(D3DXVECTOR3 p, D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c);
	D3DXVECTOR3 ClosestPtPointTriangle(D3DXVECTOR3 pt, TTriangle tTri)
	{
		return ClosestPtPointTriangle(pt, tTri.tVert0, tTri.tVert1, tTri.tVert2);
	}
	// /Pathfinding stuffs

	list<CAIComponent*, CAllocator<CAIComponent*>> m_cAwaitingDeletion;

protected:
	D3DXVECTOR3 cDepartmentPos[9];

	list<EGoalItemType, CAllocator<EGoalItemType>> m_cInitedGoalItems;

	// Will call allocate to create
	// the amount of AI agents needed
	map<unsigned int, CAIComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAIComponent*>>> m_cAIComponents;

	map<unsigned int, CObject*, less<unsigned int>,
		CAllocator<pair<unsigned int, CObject*>>> m_cSpawnableGoalItems;

	list<CObject*, CAllocator<CObject*>> m_cPlayers;

	inline float GetDistance(const D3DXVECTOR3 &cPos1, const D3DXVECTOR3 &cPos2) const
	{
		float x = cPos2.x - cPos1.x;
		float z = cPos2.z - cPos1.z;
		return ( (x * x) + (z * z) );
	}

public:
	void Init();

	static int CreateAIComponent(lua_State* pLua);
	static CAIComponent* CreateAIComponent(CObject* pObj);

	static void GoalItemInit(IEvent*, IComponent*);
	static void RemoveAIComponent(IEvent*, IComponent*);
	static void DeleteAIComponent(IEvent*, IComponent*);
	static void ToggleInfo(IEvent*, IComponent*);
	static void LoadLevel(IEvent*, IComponent*);

	string m_szInfo;
	string& GetAIInfo();
	static int SetGoalTri(lua_State* pLua);

	/***************
	*  Accessors
	***************/
	static CAIManager* GetInstance();

	/***************
	* Mutators
	***************/
	static void SetupOpponents(IEvent* piEvent, IComponent* piComponent);

	/*****************************************************************
	***************************** Events *****************************
	*****************************************************************/

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
	//static void GoalItemCollected(IEvent*, IComponent*);

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
	//static void GoalItemSpawned(IEvent*, IComponent*);

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
	//static void GoalItemDespawned(IEvent*, IComponent*);

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