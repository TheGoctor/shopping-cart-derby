/*******************************************************************************
 * Filename:  		CHeldItemComponent.h
 * Date:      		05/01/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This will represent the Held Item in the world.
					When created, Init should be called and a EHeldItemType
					should be given.
 ******************************************************************************/
#ifndef _CHELDITEMCOMPONENT_H_
#define _CHELDITEMCOMPONENT_H_

#include "scd::base_component.h"

class scd::object;
class IEvent;
class scd::base_component;
struct TSpawnLocation;
class CRenderComponent;
enum EHeldItemType;

class CHeldItemComponent : public scd::base_component
{
	scd::object* m_pParent;
	TSpawnLocation* m_tSpawnLocation;
	EHeldItemType m_eType;
	float m_fSpawnTimer;
	bool m_bSpawned;

	CRenderComponent* m_pRenderComponent;
	
public:
	/*****************************************************************
	* CHeldItemComponent()	Creates an instance of CHeldItemComponent
	*
	* Ins:				scd::object* - Parent of the component
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	CHeldItemComponent(scd::object* pObj);
	CHeldItemComponent();

	/*****************************************************************
	* ~CHeldItemComponent()	Destroys an instance of CHeldItemComponent
	*
	* Ins:				
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	~CHeldItemComponent();

	/*****************************************************************
	* Init()	Resets all the data for the component
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void Init();

	/*****************************************************************
	* Spawn()	Sets the object to spawned, gives it a psuedo random
	*			type and places it at an end cap
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void Spawn();

	/*****************************************************************
	* Despawn()	Moves the item away and sets the respawn timer
	*
	* Ins:		
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void Despawn();

	/*****************************************************************
	* Update()	Update call
	*	
	* Ins:				IEvent*		-	TUpdateStateEvent*
	*					scd::base_component*	-	CHeldItemComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void Update(IEvent*, scd::base_component*);

	/*****************************************************************
	* HeldItemCollected()	Called when a Held Item was collected.
	*						Calls despawn on the Held Item.
	*	
	* Ins:				IEvent*		-	THeldItemCollected*
	*					scd::base_component*	-	CHeldItemComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void HeldItemCollected(IEvent*, scd::base_component*);

	/*****************************************************************
	* HeldItemCollision()	Called when a Held Item was collided with by
	*						a player. Sends out the HeldItemCollected event
	*						and the BlueLightSpecialDespawned event
	*	
	* Ins:				IEvent*		-	THeldItemCollected*
	*					scd::base_component*	-	CHeldItemComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void HeldItemCollision(IEvent*, scd::base_component*);

	/*****************************************************************
	* PauseUpdateCallback()	Allows us to render in pause state
	*	
	* Ins:				IEvent*		-	
	*					scd::base_component*	-	CHeldItemComponent*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void PauseUpdateCallback(IEvent*, scd::base_component*);

	/****************************************************************
	* Accessors
	****************************************************************/
	inline scd::object* GetParent(void)
	{
		return m_pParent;
	}

	inline EHeldItemType GetType()
	{
		return m_eType;
	}

	inline bool IsSpawned()
	{
		return m_bSpawned;
	}

	/****************************************************************
	* Mutators
	****************************************************************/
	void SetRenderComponent(CRenderComponent* pComp)
	{
		m_pRenderComponent = pComp;
	}	
};

#endif // _CHELDITEMCOMPONENT_H_