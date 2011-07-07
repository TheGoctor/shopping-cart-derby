/*******************************************************************************
 * Filename:  		CHeldItemComponent.h
 * Date:      		05/01/2011
 * Mod. Date: 		05/09/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This will represent the Held Item in the world.
					When created, Init should be called and a EHeldItemType
					should be given.
 ******************************************************************************/

#ifndef _CHELDITEMCOMPONENT_H_
#define _CHELDITEMCOMPONENT_H_

#include "..\..\IComponent.h"
#include "..\..\Enums.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"

class CObject;
class IEvent;
class IComponent;
struct TSpawnLocation;
class CRenderComponent;

class CHeldItemComponent : public IComponent
{
	CObject* m_pParent;
	TSpawnLocation* m_tSpawnLocation;
	EHeldItemType m_eType;
	float m_fSpawnTimer;
	bool m_bSpawned;

	CRenderComponent* m_pRenderComponent;
	
public:
	// Constructor
	CHeldItemComponent(CObject* pObj);
	CHeldItemComponent();

	// Destructor
	~CHeldItemComponent();

	void Init();
	void Spawn();
	void Despawn();

	static void Update(IEvent*, IComponent*);

	inline CObject* GetParent(void)
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

	void SetRenderComponent(CRenderComponent* pComp)
	{
		m_pRenderComponent = pComp;
	}

	////// CALLBACKS //////////////
	static void HeldItemCollected(IEvent*, IComponent*);
	static void HeldItemCollision(IEvent*, IComponent*);
	static void PauseUpdateCallback(IEvent*, IComponent* pComp);
	
};

#endif // _CHELDITEMCOMPONENT_H_