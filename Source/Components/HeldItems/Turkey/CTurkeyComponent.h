// FILE HEADER
#ifndef _CTurkeyCOMPONENT_H_
#define _CTurkeyCOMPONENT_H_

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\..\..\IComponent.h"
#include "..\..\..\CObject.h"

class IEvent;
class CObject;
class CRenderComponent;
class CCollideable;


class CTurkeyComponent : public IComponent
{
private:
	CObject*			m_pObject; // parent of component
	bool				m_bSpawned;
	
	D3DXVECTOR3			m_vMoveDirection;

	float				m_fDuration;
	float				m_fTimeLeft;
	float				m_fStunDuration;
	int 				TURKEY_ID;

	float				m_fSpeed;
	CCollideable*		m_pCollidableComponent;


public:


	CTurkeyComponent(CObject* pObj);

	static CTurkeyComponent* CreateTurkeyComponent(CObject* pObj, D3DXVECTOR3 vDirectionToMove);

	void FirstInit();

	void ReInit();

	void SetPosition(D3DXVECTOR3 vPos)
	{
		m_pObject->GetTransform()->GetLocalMatrix()._41 = vPos.x;
		m_pObject->GetTransform()->GetLocalMatrix()._42 = vPos.y;
		m_pObject->GetTransform()->GetLocalMatrix()._43 = vPos.z;
	}
	
	void SetDirection(D3DXVECTOR3 vDir)
	{
		m_vMoveDirection = vDir;
	}
	
	bool Spawned()
	{
		return m_bSpawned;
	}

	void SetCollidableComponent(CCollideable* pCol)
	{
		m_pCollidableComponent = pCol;
	}

	void Despawn();

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
	static void PlayerCollision(IEvent* cEvent, IComponent* cCenter);
	static void EnvironmentCollision(IEvent* cEvent, IComponent* cCenter);
	static void PauseUpdateCallback(IEvent*, IComponent* pComp);
};

#endif