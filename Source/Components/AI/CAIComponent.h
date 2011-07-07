/***********************************************
 * Filename:  		CAIComponent.cpp
 * Date:      		04/04/2011
 * Mod. Date: 		06/10/2011
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
#define MAXDIST 100000

// TODO : SET CORRECT VALUE FOR MINIMAP_RADIUS
#define MINIMAP_RADIUS 3000000

#include <list>
#include <map>
#include <D3dx9math.h>
using std::list;
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

class CPickupItemComponent;
class CPickupItemManager;
class CEventManager;
class CObject;
class IEvent;
class DXMesh;
class CRenderComponent;

class CAIComponent : public IComponent
{
private:

	enum EStates { NOT_INIT = -1, COLLECTIVE = 0, AGGRESSIVE, DEFENSIVE };

	// For rendering triangles in the nav mesh
	DXMesh* m_pMesh;
	CRenderComponent* m_pRendComp;

	void StartDraw();
	void EndDraw();
	void DrawTriangle(D3DXVECTOR3 vVert0, D3DXVECTOR3 vVert1, D3DXVECTOR3 vVert2, D3DXCOLOR cColor);
	void DrawTriangle(TTri* tTri, D3DXCOLOR cColor);
	void DrawTriangle(TTriangle* tTri, D3DXCOLOR cColor);
	void DrawLine(D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, D3DXCOLOR cColor);


	// Pathplanning stuff
public:

	int m_nCurrentTriangle, m_nTargetTriangle;

public:

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
	TRay m_tTargetPlayerLine;
	EStates m_eCurrentState;
	float m_pfWeight[3];
	float m_fTimer;
	CObject* m_pObject;
	CObject* m_pTargetPlayer;
	bool m_bTargetItem;
	D3DXVECTOR3 m_vGoalPosition;
	float m_fSpeed;
	float m_fRammingSpeed;
	float m_fwaaaaAaaaAAAAAattttt;

	void Use(EHeldItemType);

	void (CAIComponent::*m_pfUpdateState)();

	void Update();
	void UpdateCollective();
	void UpdateAggressive();
	void UpdateDefensive();
	//static CVertex* PathPlan(CVertex*, CVertex*);

	/*****************************************************************
	* TGoalItems	Holds the item's id and weight. Weight will
	*				determine how likely the AI is to go for it.
	*				The higher the weight, the more likely the AI
	*				will go for the item.
	*****************************************************************/
	struct TGoalItem
	{
		D3DXVECTOR3 m_cPos;
		EGoalItemType m_nGoalItem;
		float m_pfWeight;
	};

	struct THeldItem
	{
		D3DXVECTOR3 m_cPos;
		EHeldItemType m_nHeldItem;
		int m_nID;
		bool m_bPowerup;
	};

	CObject* GetOpponent(int);
	D3DXVECTOR3 GetOpponentPos(int);
	bool FindNearbyHeldItem(bool bPowerup, THeldItem&);
	CObject* GetClosestOpponent();
	bool IsPowerup(int);
	float GetDistance(const D3DXVECTOR3 &cPos1, const D3DXVECTOR3 &cPos2) const;
	void UseHeldItem();

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
			list<EGoalItemType, CAllocator<EGoalItemType>> m_cGoalItems;
			
			unsigned int m_nAggression;

			// The opponent's object's ID
			int m_nPlayer;
			int m_nItem;
			char m_chAmtGoalItems;
			bool m_bTarget;
			bool m_bHasItem;
			float m_fCurrentSpeed;
		};

		TPlayer m_cOpponents[3];
		int m_nOpponentID;

		list<TGoalItem, CAllocator<TGoalItem>> m_cGoalItemsInLevel;
		list<THeldItem, CAllocator<THeldItem>> m_cHeldItemsInLevel;
		list<EGoalItemType, CAllocator<EGoalItemType>> m_cNeededGoalItems;
		list<EGoalItemType, CAllocator<EGoalItemType>> m_cDroppedItems;
		EHeldItemType m_peMyHeldItems[2];

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
	* EvaluateGoalItemWeights()	Ranks the currently spawned Goal Items in
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
	void EvaluateGoalItemWeights();

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

	void EvaluateHeldItemWeights();

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
	void SetupOpponents(CObject*);

	static void ItemDropped(IEvent*, IComponent*);
	static void GoalItemInit(IEvent*, IComponent*);
	static void InitCB(IEvent*, IComponent*);
	static void Shutdown(IEvent*, IComponent*);
	static void UpdateSpeeds(IEvent*, IComponent*);
	static void HeldItemSpawned(IEvent*, IComponent*);
	static void HeldItemCollected(IEvent*, IComponent*);
	static void SetRamSpeed(IEvent* pEvent, IComponent* pComp);
	static void PickupItemCollected(IEvent*, IComponent*);

	int nAnimation;
	bool bDrawTris;
	bool bLogical;
	float fTurnSpeed;
	float fThreshold[2];
	TTri* tNextTri;
	TTri* m_pMiniGoalTri;
	static void PlayerCreated(IEvent*, IComponent*);
	static void ToggleTris(IEvent*, IComponent*);
	static void ToggleAI(IEvent*, IComponent*);
	static void ToggleLogic(IEvent*, IComponent*);
	static void ShowTurnSpeed(IEvent*, IComponent*);

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


	// To render triangles fo the navmesh
	CRenderComponent* CreateMeshTriangle();


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
	//void SteerTowardTarget();

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
	//void AvoidObstacles();

	void SetControllerNumber(int nNum)
	{
		m_nControllerNumber = nNum;
	}

	static void PauseUpdateCallback(IEvent*, IComponent* pComp);

};

#endif