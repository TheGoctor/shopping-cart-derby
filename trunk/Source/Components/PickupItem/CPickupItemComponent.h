// FILE HEADER
#ifndef _CPICKUPITEMCOMPONENT_H_
#define _CPICKUPITEMCOMPONENT_H_

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\..\IComponent.h"
#include "..\..\Enums.h"

class IEvent;
class CObject;
class CRenderComponent;

class CPickupItemComponent : public IComponent
{
private:
	CObject*			m_pObject; // parent of component
	
	float				m_fTimeLeft;
	float				m_fLifetime;
	float				m_fFlyOutSpeed;
	float				m_fTimeToFly;
	float				m_fMaxArcHeight;

	//int					m_nItemType;
	EGoalItemType		m_nItemType;

	bool				m_bRendering;
	bool				m_bSpawned;

	CRenderComponent*	m_pRenderComp;

	D3DXVECTOR3			m_vDirectionToMove;

public:
	///////////////////////////////////////////////////////////////////////////
	//	Name:			CPickupItemComponent
	//	Parameters:		none
	//	Return:			void
	//	Description:	Subscribes to relevant events.
	///////////////////////////////////////////////////////////////////////////
	CPickupItemComponent(CObject* pObj);

	static int CreatePickupItemComponent(lua_State* pLua);
	static CPickupItemComponent* CreatePickupItemComponent(CObject* pObj, D3DXVECTOR3 vDirectionToMove);
	
	void Init();

	static void DestroyPickupItem(CPickupItemComponent* toDestroy);

	inline bool IsReadyToCollide()
	{
		return (m_fTimeLeft < m_fLifetime - m_fTimeToFly) && (m_fTimeLeft > 0.0f);
	}

	inline float GetArcYValue()
	{
		return 1.0f + sin( ((m_fLifetime-m_fTimeLeft)/m_fTimeToFly) * 3.1415f) * m_fMaxArcHeight;
	}

	inline CObject* GetObject()
	{
		return m_pObject;
	}

	inline EGoalItemType GetType()
	{
		return m_nItemType;
	}

	inline bool Spawned()
	{
		return m_bSpawned;
	}

	friend class CPickupItemManager;

	void EventInit();

	void SetMesh(unsigned int nMeshID);


	///////////////////////////
	//
	//	Callback Functions
	//
	///////////////////////////
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			Update
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CUpdateEvent
	//											//Holds the frame's delta time
	//	Return:			void
	//	Description:	Calls all of the functions that need to 
	//					 be called every frame. This function is 
	//					 called by the event system. 
	///////////////////////////////////////////////////////////////////////////
	static void Update(IEvent* cEvent, IComponent* cCenter);
	static void PickupItemCollisionCallback(IEvent* cEvent, IComponent* cCenter);


	static void PauseUpdateCallback(IEvent*, IComponent* pComp);

};

#endif