/*******************************************************************************
 * Filename:  		CDepartment.h
 * Date:      		05/01/2011
 * Mod. Date: 		05/09/2011
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
#include "CGoalItemComponent.h"
#include "..\..\Managers\Global Managers\Memory Manager\CAllocator.h"

class IEvent;

struct TGoalItemSpawnLocation
{
	D3DXVECTOR3 m_cPos;
	CGoalItems* m_cGoalItem;
	bool m_bSpawned;
};

class CDepartment : public IComponent
{
	CObject* m_pParent;
	EDepartment m_eType;
	bool m_bUsed;

	D3DXVECTOR3 GetSpawnPosition(EDepartment eDepartment);

public:

	map<int, TGoalItemSpawnLocation, less<unsigned int>,
		CAllocator<pair<int, TGoalItemSpawnLocation>>> m_cGoalItemSpawnLocations;

	CDepartment(CObject*);
	~CDepartment();
	
	inline CObject* GetParent()
	{
		return m_pParent;
	}

	inline bool IsSpawned()
	{
		return m_bUsed;
	}

	inline void Deactivate()
	{
		// Setting this to true makes it so
		// that the Spawning Manager ignores it
		// while picking a Department to spawn
		// Goal Items in
		m_bUsed = true;
	}

	void Init(EDepartment eDepartment);
	void AcknowledgeDepartments(int);
	void Despawn();
	void SpawnGoalItem();

	static void Spawn(IEvent*, IComponent*);
};

#endif // _CDEPARTMENT_H_