/***********************************************
 * Filename:  		CAIComponent.cpp
 * Date:      		04/04/2011
 * Mod. Date: 		07/26/2011
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

#define MAXDIST 100000

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
	bool m_bShutdown;

	/*****************************************************************
	* StartDraw()	Sets up anything needed to begin drawing traingles
	*
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      06/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void StartDraw();

	/*****************************************************************
	* EndDraw()	Passes all buffers to be drawn
	*
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      06/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void EndDraw();

	/*****************************************************************
	* DrawTriangle()	Passes information for the triangle to be
	*					drawn and a color to be drawn in
	*
	* Ins:				D3DXVECTOR3 / TTriangle* / TTri*
	*					D3DXCOLOR cColor
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      06/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void DrawTriangle(D3DXVECTOR3 vVert0, D3DXVECTOR3 vVert1, D3DXVECTOR3 vVert2, D3DXCOLOR cColor);
	void DrawTriangle(TTriangle* tTri, D3DXCOLOR cColor);
	void DrawTriangle(TTri* tTri, D3DXCOLOR cColor);
	
	/*****************************************************************
	* DrawTriangle()	Passes information for a line to be
	*					drawn and a color to be drawn in
	*
	* Ins:				D3DXVECTOR3 vStart, vEnd
	*					D3DXCOLOR cColor
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      06/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void DrawLine(D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, D3DXCOLOR cColor);

	// Pathplanning stuff
	int m_nCurrentTriangle, m_nTargetTriangle;

	// Steering Information
	D3DXVECTOR3		m_vTargetPosition;

	int				m_nControllerNumber;
	//

	// Needed Logic
	TRay m_tTargetPlayerLine;
	float m_pfWeight[3];
	float m_fTimer;
	CObject* m_pTargetPlayer;
	bool m_bTargetItem;
	bool m_bRandomItems;
	
	float m_fSpeed;
	float m_fRammingSpeed;
	float m_fAcceleration;
	float m_fBlindTimer;
	float m_fJamTimer;
	float m_fUseItemTimer;
	float m_fTurkeyToPlayer;

	/*****************************************************************
	* Use()				Uses the passed in type if it has one
	*
	* Ins:				EHeldItemType
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	bool Use(EHeldItemType);

	/*****************************************************************
	* GetFirstPlace()	Returns the ID of who is in first place
	*
	* Ins:				
	*
	* Outs:			
	*
	* Returns:			int
	*
	* Mod. Date:		      07/18/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static int GetFirstPlace();

	/*****************************************************************
	* GetMyPlace()	Returns the numerial place that the agent is
	*				currently in
	*
	* Ins:				
	*
	* Outs:			
	*
	* Returns:			int
	*
	* Mod. Date:		      07/18/11
	* Mod. Initials:	      JS
	*****************************************************************/
	int GetMyPlace();

	/*****************************************************************
	* UpdateCollective()	Updates agent logic with a collective bias
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
	void UpdateCollective();

	/*****************************************************************
	* UpdateAggressive()	Updates agent logic with a aggressive bias
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
	void UpdateAggressive();

	/*****************************************************************
	* UpdateDefensive()	Updates agent logic with a defensive bias
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
	void UpdateDefensive();

	/*****************************************************************
	* RemoveGoalItemFromLevel()	Removes the past in Goal Item type
	*							from the GoalItemsInLevel map
	*
	* Ins:				EGoalItemType
	*
	* Outs:			
	*
	* Returns:			
	*
	* Mod. Date:		      07/21/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void RemoveGoalItemFromLevel(EGoalItemType eGoalItemType);

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

	/*****************************************************************
	* TKnowledge	Holds all knowledge that the agent will have
	*
	* TPlayer		Has a char for amount of goal item the the
	*				opponent has collected and a list for
	*				goal items that it has SEEN the opponent collect
	*****************************************************************/
	struct TPlayer
	{
		map<unsigned int, EGoalItemType, less<unsigned int>,
			CAllocator<pair<unsigned int, EGoalItemType>>> m_cGoalItems;

		unsigned int m_nAggression;

		// The opponent's object's ID
		int m_nPlayer;
		int m_nItem;
		char m_chAmtGoalItems;
		bool m_bTarget;
		bool m_bHasItem;
		float m_fCurrentSpeed;
	};

	/*****************************************************************
	* GetOpponent()	Returns a pointer to the object who owns the
	*				passed in ID
	*
	* Ins:				int nID
	*
	* Outs:			
	*
	* Returns:			
	*
	* Mod. Date:		      05/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	CObject* GetOpponent(int);

	/*****************************************************************
	* GetOpponentPos()	Returns the position of the object who
	*					owns the passed in ID
	*
	* Ins:				int nID
	*
	* Outs:			
	*
	* Returns:			D3DXVECTOR3 - Position of opponent
	*
	* Mod. Date:		      05/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	D3DXVECTOR3 GetOpponentPos(int);

	/*****************************************************************
	* FindNearbyHeldItem()	Finds a nearby held item of either a
	*						power up or attack item
	*
	* Ins:				bool bPowerup - true if you want to find
	*									a power up, false if you
	*									want an attack item
	*
	* Outs:				THeldItem& - A THeldItem that contains all
	*								information needed about found
	*								held item
	*
	* Returns:			True - item found
	*					False - item not found
	*
	* Mod. Date:		      05/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	bool FindNearbyHeldItem(bool bPowerup, THeldItem&);

	/*****************************************************************
	* GetClosestOpponent()	Finds the closest opponent to the agent
	*						and returns a pointer to their object
	*
	* Ins:				int nID
	*
	* Outs:				
	*
	* Returns:			CObject* of found player
	*
	* Mod. Date:		      05/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	CObject* GetClosestOpponent();

	/*****************************************************************
	* IsPowerup()	Determines if the type of a item is a power up
	*				or attack item
	*
	* Ins:				int nID
	*
	* Outs:				
	*
	* Returns:			True - Powerup
	*					False - Attack item
	*
	* Mod. Date:		      05/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	bool IsPowerup(int);

	/*****************************************************************
	* GetDistance()	returns the squared distance between two points
	*
	* Ins:				const D3DXVECTOR3& cPos1
	*					const D3DXVECTOR3& cPos2
	*
	* Outs:				
	*
	* Returns:			float - Squared distance between two points
	*
	* Mod. Date:		      05/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	float GetDistance(const D3DXVECTOR3 &cPos1, const D3DXVECTOR3 &cPos2) const;

	/*****************************************************************
	* UseHeldItem()	Goes through logic and uses any Held Items that
	*				the agent deems apporiate
	*
	* Ins:			
	*
	* Outs:				
	*
	* Returns:		
	*
	* Mod. Date:		      05/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void UseHeldItem();

	/*****************************************************************
	* PickRandomHeldItem()	Makes the agent use a psudeo-random 
	*						Held Item
	*
	* Ins:			
	*
	* Outs:				
	*
	* Returns:		
	*
	* Mod. Date:		      07/20/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void PickRandomHeldItem();

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
	* Mod. Date:		      04/16/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void EvaluateGoalItemWeights();

	/*****************************************************************
	* EvaluateOpponentWeights()	Ranks each opponent based either on
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
	* Mod. Date:		      04/16/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void EvaluateOpponentWeights();

	/*****************************************************************
	* EvaluateHeldItemWeights()	Ranks each Held Item based on distance
	*							and state of the agent
	*
	* Ins:			
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/26/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void EvaluateHeldItemWeights();

public:

	struct TKnowledge
	{
		TPlayer m_cOpponents[3];
		int m_nOpponentID;

		map<unsigned int, TGoalItem, less<unsigned int>, 
			CAllocator<pair<unsigned int, TGoalItem>>> m_cGoalItemsInLevel;
		map<unsigned int, THeldItem, less<unsigned int>,
			CAllocator<pair<unsigned int, THeldItem>>> m_cHeldItemsInLevel;
		map<unsigned int, EGoalItemType, less<unsigned int>,
			CAllocator<pair<unsigned int, EGoalItemType>>> m_cNeededGoalItems;
		map<unsigned int, EGoalItemType, less<unsigned int>,
			CAllocator<pair<unsigned int, EGoalItemType>>> m_cDroppedItems;

		EHeldItemType m_peMyHeldItems[2];

		unsigned int m_nDistanceFromGoal;

	}m_tKnowledge;

	D3DXVECTOR3 m_vGoalPosition;
	CObject* m_pObject;
	EStates m_eCurrentState;
	int nAnimation;
	bool bDrawTris;
	bool bLogical;
	float fTurnSpeed;
	float fThreshold[2];
	TTri* tNextTri;
	TTri* m_pMiniGoalTri;

	/*****************************************************************
	* SetUseRandomItems()	Sets a bool on weather the agent should
	*						recieve random held items or not
	*
	* Ins:				EGoalItemType
	*
	* Outs:			
	*
	* Returns:			
	*
	* Mod. Date:		      07/25/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void SetUseRandomItems(bool bRandomItems)
	{
		m_bRandomItems = bRandomItems;
	}

	/*****************************************************************
	* CAIComponent()	Creates an instace of CAIComponent
	*
	* Ins:				CObject* pObj - Parent of the component
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	CAIComponent(CObject* pObj);

	/*****************************************************************
	* ~CAIComponent()	Destroys the instace of CAIComponent
	*
	* Ins:				
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
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
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void Init();

	// Events

	/*****************************************************************
	* SetupOpponents()	Adds the agent's opponents to its list of
	*					opponents
	*	
	* Ins:				CObject* - Pointer to the object of the
	*								opponent
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void SetupOpponents(CObject*);

	/*****************************************************************
	* ItemDropped()	Is called when a Goal Item is knocked out of a 
	*				players cart
	*	
	* Ins:				IEvent*		-	TGoalItemCollectedEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ItemDropped(IEvent*, IComponent*);

	/*****************************************************************
	* GoalItemInit()	Is called when a Goal Item is created from
	*					CSpawningManager
	*	
	* Ins:				IEvent*		-	TGoalItemEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemInit(IEvent*, IComponent*);

	/*****************************************************************
	* InitCB()	Reinits the AI after multiple playthroughs
	*	
	* Ins:				IEvent*		-	
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void InitCB(IEvent*, IComponent*);

	/*****************************************************************
	* Shutdown()	Called when the gameplay state is exited and
	*				the agent should shutdown
	*	
	* Ins:				IEvent*		-	
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Shutdown(IEvent*, IComponent*);

	/*****************************************************************
	* UpdateSpeeds()	Called every frame updating values for each
	*					players' speed
	*	
	* Ins:				IEvent*		-	TSpeedEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void UpdateSpeeds(IEvent*, IComponent*);

	/*****************************************************************
	* HeldItemSpawned()	Called when a new Held Item is spawned
	*	
	* Ins:				IEvent*		-	THeldItemSpawned*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void HeldItemSpawned(IEvent*, IComponent*);

	/*****************************************************************
	* HeldItemCollected()	Called when a Held Item is collected by
	*						a player
	*	
	* Ins:				IEvent*		-	THeldItemCollected*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void HeldItemCollected(IEvent*, IComponent*);

	/*****************************************************************
	* SetRamSpeed()	Called at the beginning of the game to set the
	*				ram speed of the agent
	*	
	* Ins:				IEvent*		-	TSpeedEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void SetRamSpeed(IEvent* pEvent, IComponent* pComp);

	/*****************************************************************
	* PickupItemCollected()	Called when a pick up item (knocked out
	*						Goal Item) is collected
	*	
	* Ins:				IEvent*		-	TPickupItemCollectedEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void PickupItemCollected(IEvent*, IComponent*);

	/*****************************************************************
	* ItemStealing()	Called when a Held Item is stolen through the
	*					use of donuts
	*	
	* Ins:				IEvent*		-	TImpactEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ItemStealing(IEvent*, IComponent*);

	/*****************************************************************
	* PlayerCreated()	Called when a new player is created
	*	
	* Ins:				IEvent*		-	TTwoIntEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void PlayerCreated(IEvent*, IComponent*);

	/*****************************************************************
	* ToggleTris()	Event sent through Lua to toggle the rendering
	*				of the nav mesh
	*	
	* Ins:				IEvent*		-	
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ToggleTris(IEvent*, IComponent*);

	/*****************************************************************
	* ToggleAI()	Event sent through Lua to toggle certain aspects
	*				of the AI
	*	
	* Ins:				IEvent*		-	
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ToggleAI(IEvent*, IComponent*);

	/*****************************************************************
	* ToggleLogic()	Event sent through Lua to toggle if the agent runs
	*				logic
	*	
	* Ins:				IEvent*		-	
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ToggleLogic(IEvent*, IComponent*);

	/*****************************************************************
	* ShowTurnSpeed()	Called when the game starts to set all the
	*					turn speeds of the players
	*	
	* Ins:				IEvent*		-	TSpeedEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ShowTurnSpeed(IEvent*, IComponent*);

	/*****************************************************************
	* ShowAcceleration()	Called when the game starts to set all the
	*					acceleration of the players
	*	
	* Ins:				IEvent*		-	TSpeedEvent*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ShowAcceleration(IEvent*, IComponent*);

	/*****************************************************************
	* Blind()	Called when a player is hit by a pie
	*	
	* Ins:				IEvent*		-	pStatusEffect*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Blind(IEvent*, IComponent*);

	/*****************************************************************
	* Jammed()	Called when a player is hit by jam
	*	
	* Ins:				IEvent*		-	pStatusEffect*
	*					IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Jammed(IEvent*, IComponent*);

	/*****************************************************************
	* GoalItemCollected()	An event that tells the agent that a
	*						Goal Item has been collected
	*
	* Ins:					IEvent*		-	TGoalItemCollectedEvent*
	*						IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/08/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemCollected(IEvent*, IComponent*);

	/*****************************************************************
	* GoalItemSpawned()	An event that tells the agent that a
	*					Goal Item has spawned
	*
	* Ins:					IEvent*		-	TGoalItemEvent*
	*						IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/08/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemSpawned(IEvent*, IComponent*);

	/*****************************************************************
	* GoalItemSpawned()	An event that tells the agent that a
	*					Goal Item has spawned
	*
	* Ins:					IEvent*		-	TGoalItemEvent*
	*						IComponent*	-	CAIComponent
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/08/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void GoalItemDespawned(IEvent*, IComponent*);

	/*****************************************************************
	* Update()	Update calls any function that needs calling
	*
	* Ins:		IEvent*		-	TUpdateStateEvent*
	*			IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/16/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Update(IEvent*, IComponent*);

	/*****************************************************************
	* PauseUpdateCallback()	Called when the gameplay state is paused
	*						to render the game world
	*
	* Ins:		IEvent*		-
	*			IComponent*	-	CAIComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      07/13/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void PauseUpdateCallback(IEvent*, IComponent* pComp);

	/*****************************************************************
	* CreateMeshTriangle()	Creates an instance of an object to render
	*						a triangle of the nav mesh
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:				CRenderComponent* - component of the triangle
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	CRenderComponent* CreateMeshTriangle();

	/*****************************************************************
	* SetControllerNumber()	Sets the controller number of the agent
	*
	* Ins:					int nNum
	*
	* Outs:			
	*
	* Returns:	
	*
	* Mod. Date:		      06/07/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void SetControllerNumber(int nNum)
	{
		m_nControllerNumber = nNum;
	}
};

#endif