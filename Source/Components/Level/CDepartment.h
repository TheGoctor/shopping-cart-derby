/*******************************************************************************
 * Filename:  		CDepartment.h
 * Date:      		05/01/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		Departments will hold a set amount of Goal Items.
					These Goal Items will be assoicated to the Department
					in terms of type (i.e. milk in the dairy department).

					A Department will list for a SpawnGoalItem event and will
					spawn a random Goal Item within itself.
 ******************************************************************************/
#ifndef _CDEPARTMENT_H_
#define _CDEPARTMENT_H_

#include <D3dx9math.h>
#include <map>
using std::map;

#include "..\..\IComponent.h"
#include "..\..\Managers\Global Managers\Memory Manager\CAllocator.h"

class IEvent;
class CGoalItems;
class CObject;

struct TGoalItemSpawnLocation
{
	D3DXVECTOR3 m_cPos;
	CGoalItems* m_cGoalItem;
};

class CDepartment : public IComponent
{
	CObject* m_pParent;
	EDepartment m_eType;
	bool m_bUsed;

public:

	map<int, TGoalItemSpawnLocation, less<unsigned int>,
		CAllocator<pair<int, TGoalItemSpawnLocation>>> m_cGoalItemSpawnLocations;

	/*****************************************************************
	* CDepartment()	Creates an instance of Department
	*
	* Ins:			CObject* - Parent
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	CDepartment(CObject*);

	/*****************************************************************
	~CDepartment()	Destroys the instance
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
	~CDepartment();

	/*****************************************************************
	* Init()	Sets up any data needed for the department.
	*
	* Ins:		EDepartment - The Department type (Dairy/Deli)
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void Init(EDepartment eDepartment);

	/*****************************************************************
	* AcknowledgeDepartments()	Tells the Department that it will be
	*							used this game. Creates the Goal
	*							Items and pushes them into its map
	*							of Goal Item locations. Sends the
	*							GoalItemInit event.
	*
	* Ins:						nID - EGoalItemType to spawn
	*
	* Outs:			
	*
	* Returns:		
	*
	* Mod. Date:		      05/06/11
	* Mod. Initials:	      JS
	*****************************************************************/
	void AcknowledgeDepartments(int);

	/*****************************************************************
	* Despawn()	Tells the Department to despawn any Goal Item spawned
	*			in it and then set its used bool to false.
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
	* SpawnGoalItem()	Picks a psudeo random location to spawn a 
	*					Goal Item. Then places the Goal Item at
	*					that location.
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
	void SpawnGoalItem();

	/*****************************************************************
	* Deactivate()	Opposite of AcknowledgeDepartment. Sets the
	*				Department to not be used this game.
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
	inline void Deactivate()
	{
		// Setting this to true makes it so
		// that the Spawning Manager ignores it
		// while picking a Department to spawn
		// Goal Items in
		m_bUsed = true;
	}
	
	/****************************************************************
	* Accessors
	****************************************************************/
	inline CObject* GetParent()
	{
		return m_pParent;
	}

	inline bool IsSpawned()
	{
		return m_bUsed;
	}
};

#endif // _CDEPARTMENT_H_