/*******************************************************************************
 * Filename:  		CGoalItemComponent.h
 * Date:      		05/01/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This will represent the Goal Item in the world.
					When created, Init should be called and a EGoalItemType
					should be given. Goal Items will register for an update
					event and will despawn after a set period of time. This
					time is set in the init function.
 ******************************************************************************/
#ifndef _CGOALITEMS_H_
#define _CGOALITEMS_H_

#include "scd::base_component.h"

class scd::object;
class IEvent;
class scd::base_component;
enum EGoalItemType;

class CGoalItems : public scd::base_component
{
	EGoalItemType m_eType;
	scd::object* m_pObject;
	bool m_bSpawned;
	bool m_bDespawning;
	bool m_bCollidable;
	float m_fDespawnTimer;
	bool m_bBlink;

public:
	/*****************************************************************
	* CGoalItems()	Creates an instance of a Goal Item component
	*
	* Ins:			scd::object* - Parent of this component
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/01/11
	* Mod. Initials:	      JS
	*****************************************************************/
	CGoalItems(scd::object* pObj);
	CGoalItems(){}

	/*****************************************************************
	* ~CGoalItems()	Destroys an instance of a Goal Item component
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
	~CGoalItems();

	/*****************************************************************
	* Init()	Resets all the information for the Goal Item and
	*			sets it to the specified type
	*
	* Ins:		EGoalItemType
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/01/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void Init(const EGoalItemType);

	/*****************************************************************
	* Spawn()	Sends our the Spawned event for Goal Items and makes
	*			the Goal Item collidable
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
	void Spawn();

	/*****************************************************************
	* Despawn()		Sets the collidable bool to false
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
	void Despawn();

	/*****************************************************************
	* Update()	The update call
	*	
	* Ins:				IEvent*		-	TUpdateStateEvent*
	*					scd::base_component*	-	CGoalItems*
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
	* IteShutdownGoalItemmDropped()	Game is shutting down. Delete all
	*								memory and set things to NULL
	*	
	* Ins:				IEvent*		-	
	*					scd::base_component*	-	CGoalItems*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void ShutdownGoalItem(IEvent*, scd::base_component*);

	/*****************************************************************
	* PauseUpdateCallback()	Allows us to render during the pause state
	*	
	* Ins:				IEvent*		-	
	*					scd::base_component*	-	CGoalItems*
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      04/11/11
	* Mod. Initials:	      JS
	*****************************************************************/
	static void PauseUpdateCallback(IEvent*, scd::base_component* pComp);

	/****************************************************************
	* Accessors
	****************************************************************/
	inline EGoalItemType GetType() const
	{
		return m_eType;
	}

	inline scd::object* GetParent() const
	{
		return m_pObject;
	}

	inline bool GetIsCollidable() const
	{
		return m_bCollidable;
	}

	inline bool GetIsSpawned() const
	{
		return m_bSpawned;
	}
};
#endif