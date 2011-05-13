/***********************************************
 * Filename:  		CAIComponent.cpp
 * Date:      		04/04/2011
 * Mod. Date: 		05/09/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This is the AI's main class
			which is responsable for changing
			the AI's state based on weights.
					The AI will have three main
			states: Defensive, Collective, and 
			Aggressive. Each state will dertermine
			how the AI behaves in each situation.
 ************************************************/

#ifndef _CAICOMPONENT_H_
#define _CAICOMPONENT_H_

// TODO : SET CORRECT VALUE FOR MAXDIST
// SQUARE MAXDIST
#define MAXDIST 10000

// TODO : SET CORRECT VALUE FOR MINIMAP_RADIUS
#define MINIMAP_RADIUS 300000

#include <list>
#include <set>
#include <map>
#include <D3dx9math.h>
using std::list;
using std::set;
using std::map;

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\..\IComponent.h"
#include "..\..\Managers\Global Managers\Memory Manager\CAllocator.h"
#include "..\..\Managers\Component Managers\CAIManager.h"

class CEventManager;
class CObject;
class IEvent;

class CAIComponent : public IComponent
{
private:

	enum EStates { NOT_INIT = -1, COLLECTIVE = 0, AGGRESSIVE, DEFENSIVE };

	// Pathplanning stuff
	struct TNodeCmp
	{
		bool operator()(const CVertex* lhs, const CVertex* rhs)
		{
			//if(lhs->m_nPriority == rhs->m_nPriority)
			//{
			//	return lhs < rhs;
			//}
			// Greater than operator
			return lhs->m_dGivenCost > rhs->m_dGivenCost;
		}
	};

	set<CVertex*, TNodeCmp, CAllocator<CVertex*>> m_cOpen;
	map<CVertex*, CVertex*, less<CEdge*>, CAllocator<
		pair<CVertex*, CVertex*>>> m_cCreated;
public:
	bool m_bPathFound;

	list<D3DXVECTOR3, CAllocator<D3DXVECTOR3>> m_cPath;
	CVertex* GetClosestWaypoint(D3DXVECTOR3& cPos);
	//

	// Steering Information
	D3DXVECTOR3		m_vTargetPosition;

	int				m_nFramesToAvoidObstacle;
	D3DXVECTOR3		m_vAvoidanceTargetPosition;

	float			m_fReactionDistance; // coefficient for the collision prediction
	float			m_fSideReactionDistance; // coeff for side feelers
	D3DXVECTOR3		m_pvReactionFeelers[3]; // normalized vector for the direction
											// of the feelers. [0] is forward,
										// [1] is left, [2] is right
	int				m_nControllerNumber;
	int				m_nFramesWeAreOnThisTarget;
	int				m_nFramesToAccel;
	//

	// Needed Logic
public:
	EStates m_eCurrentState;
//private:
	float m_pfWeight[3];
	float m_fTimer;
	CObject* m_pObject;
	D3DXVECTOR3 m_cTargetItemPos;
	D3DXVECTOR3 m_cTargetPlayerPos;

	void (CAIComponent::*m_pfUpdateState)();

	void Update();
	void UpdateCollective();
	void UpdateAggressive();
	void UpdateDefensive();
	void PathPlan(const float fTimeSlice);

	D3DXVECTOR3 GetOpponentPos(int);
	CObject* GetClosestOpponent();

	float GetDistance(const D3DXVECTOR3 &cPos1, const D3DXVECTOR3 &cPos2) const;

	/*****************************************************************
	* TGoalItems	Holds the item's id and weight. Weight will
	*				determine how likely the AI is to go for it.
	*				The higher the weight, the more likely the AI
	*				will go for the item.
	*****************************************************************/
	struct TGoalItem
	{
		float m_pfWeight;
		D3DXVECTOR3 m_cPos;
		unsigned m_nGoalItem;
	};

	/*****************************************************************
	* TKnowledge	Holds all knowledge that the agent will have
	*
	* TPlayer		Has a char for amount of goal item the the
	*				opponent has collected and a list for
	*				goal items that it has SEEN the opponent collect
	*****************************************************************/
	struct TKnowledge
	{
		//•	Opponents positions and actions if they are near each other
		//	o	In line-of-sight
		//	o	In mini-map range
		struct TPlayer
		{
			list<unsigned int, CAllocator<unsigned int>> m_cGoalItems;
			
			unsigned int m_nAggression;
			float m_fWeight;
			int m_nPlayer;
			int m_nItem;
			char m_chAmtGoalItems;
			bool m_bTarget;
			bool m_bHasItem;
		};

		TPlayer m_cOpponents[3];
		int m_nOpponentID;

		list<TGoalItem, CAllocator<TGoalItem>> m_cGoalItemsInLevel;
		list<unsigned int, CAllocator<unsigned int>> m_cNeededGoalItems;
		char m_pchMyHeldItems[2];

		unsigned int m_nDistanceFromGoal;

	}m_tKnowledge;

	/*****************************************************************
	* WithinRadar()	Returns true if the passed in vec3 is within the
	*				scope of the radar. Does not sqrt
	*
	* Ins:			cPos - Position
	*
	* Outs:			
	*
	* Returns:		True if within range, false if not
	*
	* Mod. Date:		      04/04/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	bool WithinRadar(const D3DXVECTOR3& cPos) const;

	/*****************************************************************
	* EvaluateStateWeights()	Adds up all the weight components and 
	*							changes states if necessary
	*
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/16/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void EvaluateStateWeights();

	/*****************************************************************
	* EvaluateItemWeights()	Ranks the currently spawned Goal Items in
	*						a weight based system to determine which
	*						to go for
	*
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/16/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	void EvaluateItemWeights();

	/*****************************************************************
	* EvaluateOpponentWeights()	Ranks the each opponent based either on
	*							collection weights, aggressive weights,
	*							or defensive weights, depending on
	*							the agent's current state
	*
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/16/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	void EvaluateOpponentWeights();

public:

	CAIComponent(CObject* pObj);
	~CAIComponent();

	/*****************************************************************
	* Init()	Inits the component
	*
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	void Init();

	// Events
	static void SetupOpponents(IEvent*, IComponent*);

	static void ItemDropped(IEvent*, IComponent*);

	static void GoalItemInit(IEvent*, IComponent*);

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
	* Mod. Date:		      04/16/2011
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Update(IEvent*, IComponent*);




	///////////////////////////////////////////
	// 
	//		Navigation
	//
	///////////////////////////////////////////

	/*****************************************************************
	* SteerTowardTarget()	Tells the steering component to steer towards
	*						its target.
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/12/2011
	* Mod. Initials:	      MS
	*****************************************************************/
	void SteerTowardTarget();

	/*****************************************************************
	* AvoidObstacles()	
	*						its target.
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/12/2011
	* Mod. Initials:	      MS
	*****************************************************************/
	void AvoidObstacles();

	void SetControllerNumber(int nNum)
	{
		m_nControllerNumber = nNum;
	}
};

#endif