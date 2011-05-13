// FILE HEADER
#ifndef _CPICKUPITEMOMPONENT_H_
#define _CPICKUPITEMCOMPONENT_H_

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\..\IComponent.h"

class IEvent;
class CObject;

class CPickupItemComponent : public IComponent
{
private:
	CObject*			m_pObject; // parent of component
	
	float				m_fTimeLeft;
	float				m_fLifetime;
	float				m_fFlyOutSpeed;
	float				m_fTimeToFly;

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

	bool IsReadyToCollide()
	{
		return m_fTimeLeft < m_fLifetime - m_fTimeToFly;
	}

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
	static void GoalItemCollectionCallback(IEvent* cEvent, IComponent* cCenter);
};

#endif