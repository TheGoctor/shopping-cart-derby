/*******************************************************************************
 * Filename:  		CAIManager.h
 * Date:      		04/04/2011
 * Mod. Date: 		07/26/2011
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
	string m_szInfo;

	/*****************************************************************
	* LoadNavMesh()	Loads a nav mesh
	*	
	* Ins:				string szFileName
	*
	* Outs:			
	*
	* Returns:			
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void LoadNavMesh(string szFileName);

	/*****************************************************************
	* IsConnected()	Checks to see if two TTri's are connected
	*	
	* Ins:				const TTri& rhs
	*
	* Outs:			
	*
	* Returns:			True - Connected
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	bool IsConnected(const TTri& lhs, const TTri& rhs) const;

	/*****************************************************************
	* CreateConnections()	Preforms logic to either read in connections
	*						or create the connections file
	*	
	* Ins:				string szFileName
	*
	* Outs:			
	*
	* Returns:			
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void CreateConnections(string szFileName);

	/*****************************************************************
	* CreateLookUpTable()	Preforms logic to either read in a look up
	*						table or create the file
	*	
	* Ins:				const TTri& rhs
	*
	* Outs:				string szFileName
	*
	* Returns:			
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void CreateLookUpTable(string szFileName);

	/*****************************************************************
	* Pathplan()	Preforms A* pathfinding from triangle 1 to 
	*				triangle 2
	*	
	* Ins:				const int Tri1, const int Tri2
	*
	* Outs:			
	*
	* Returns:			int - Edge to go through from tri 1
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	int Pathplan(const int, const int);

	/*****************************************************************
	* LinesEqual()	Checks to see if two lines are the same line
	*	
	* Ins:			const TLine&, const TLine&
	*
	* Outs:			
	*
	* Returns:		True - Same Line
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	bool LinesEqual(const TLine& lhs, TLine& rhs);

	/*****************************************************************
	* LookUpCellAlgorithm()	Takes in two ints for a 2D array and
	*						returns the next triangle to go to.
	*	
	* Ins:			const int, const int
	*
	* Outs:			
	*
	* Returns:		unsigned int - index of next triangle
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	inline unsigned int LookUpCellAlgorithm(const int x, const int y)
	{
		// x = Current, y = Target
		return y * m_cTris.size() + x;
	}
	// /Pathfinding stuffs

protected:
	D3DXVECTOR3 cDepartmentPos[9];

	list<EGoalItemType, CAllocator<EGoalItemType>> m_cInitedGoalItems;

	// Will call allocate to create
	// the amount of AI agents needed
	map<unsigned int, CAIComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAIComponent*>>> m_cAIComponents;

	map<unsigned int, CObject*, less<unsigned int>,
		CAllocator<pair<unsigned int, CObject*>>> m_cSpawnableGoalItems;

	map<unsigned int, CObject*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CObject*>>> m_cPlayers;

	/*****************************************************************
	* GetDistance()	Returns SQUARED distance between two points
	*	
	* Ins:			const D3DXVECTOR3&, const D3DXVECTOR3&
	*
	* Outs:			
	*
	* Returns:		float - distance
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	inline float GetDistance(const D3DXVECTOR3 &cPos1, const D3DXVECTOR3 &cPos2) const
	{
		float x = cPos2.x - cPos1.x;
		float z = cPos2.z - cPos1.z;
		return ( (x * x) + (z * z) );
	}

public:
	/*****************************************************************
	* Init()	Registers for events and loads any needed file
	*	
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/04/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void Init();

	/*****************************************************************
	* GetAIInfo()	Recieves each agent and their state
	*	
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      07/13/11
	* Mod. Initials:	      JS
	*****************************************************************/
	string& GetAIInfo();

	/*****************************************************************
	* CreateAIComponent()	Creates an AI component
	*	
	* Ins:					CObject* - Parent, bool UseRandomItems
	*
	* Outs:			
	*
	* Returns:				CAIComponent*
	*
	* Mod. Date:		      04/04/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static CAIComponent* CreateAIComponent(CObject* pObj, bool);
	static int CreateAIComponent(lua_State* pLua);

	/*****************************************************************
	* GoalItemInit()	Saves the inited goal item into a list
	*	
	* Ins:				IEvent* -
	*					IComponent* - TGoalItemEvent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      06/27/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemInit(IEvent*, IComponent*);

	/*****************************************************************
	* ToggleInfo()	Toggles if the szInfo string returns actual
	*				info or a null string
	*	
	* Ins:			IEvent* -
	*				IComponent* - 
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      07/12/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ToggleInfo(IEvent*, IComponent*);

	/*****************************************************************
	* LoadLevel()	Loads the NavMesh, creates connections between
	*				triangles, and then generates the look
	*				up table
	*	
	* Ins:			IEvent* -
	*				IComponent* - TStringEvent
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/04/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void LoadLevel(IEvent*, IComponent*);

	/*****************************************************************
	* SetGoalTri()	Sets the traingle the specified agent is
	*				to go to
	*	
	* Ins:			pLua - string: SetAIGoalTri(#agent, #tri)
	*
	* Outs:			
	*
	* Returns:		int - random int because this is a lua function
	*
	* Mod. Date:		      06/14/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static int SetGoalTri(lua_State* pLua);

	/*****************************************************************
	* Update()	Update calls any function that needs calling
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/04/11
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
	* Mod. Date:		      04/04/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Shutdown(IEvent*, IComponent*);

	/*****************************************************************
	* SetupOpponents()	Calls SetupOpponents() on each agent
	*					so they know who their opponents are (ID wise)
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/01/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void SetupOpponents(IEvent* piEvent, IComponent* piComponent);

	/***************
	*  Accessors
	***************/
	static CAIManager* GetInstance();
	
};

#endif	// _CAIMANAGER_H_