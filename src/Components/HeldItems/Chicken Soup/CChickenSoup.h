#ifndef CCHICKENSOUP_H_
#define CCHICKENSOUP_H_

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\..\..\IComponent.h"
#include "..\..\..\CObject.h"
#include "..\..\Collision\CCollideable.h"
#include "..\..\..\Managers\Component Managers\CCollisionManager.h"
#define SOUPTIME 5.0f
class IEvent;
class CRenderComponent;
class CChickenSoupComponent : public IComponent
{
private:
	float m_fTimeRemaining;
	float m_fDuration;
	CObject* m_pParent;
	bool m_bIsSpawned;
	CObject* m_pAttachedObject;
	D3DXVECTOR3 m_vPosition;		//world space position
	int CHICKENSOUP_ID;

public:
	CChickenSoupComponent(CObject* pObj) : m_pParent(pObj)
	{
	}
	static CChickenSoupComponent* CreateChickenSoupComponent(CObject* pObj);
	void FirstInit(void);
	//call backs
	static void Update(IEvent* cEvent, IComponent* cCenter);
	static void PlayerCollision(IEvent* cEvent, IComponent* cCenter);
	static void ItemCollision(IEvent* cEvent, IComponent* cCenter);
	void ReInit();
	void Despawn();
	
	float GetTimeRemaining(void)
	{
		return m_fTimeRemaining;
	}
	float GetDuration(void)
	{
		return m_fDuration;
	}
	CObject* GetParent(void)
	{
		return m_pParent;
	}
	bool GetIsSpawned(void)
	{
		return m_bIsSpawned;
	}
	//mutators
	void SetTimeRemaining(float fRemaining)
	{
		m_fTimeRemaining = fRemaining;
	}
	void SetPosition(D3DXVECTOR3 vPos)
	{
		m_pParent->GetTransform()->GetLocalMatrix()._41 = vPos.x;
		m_pParent->GetTransform()->GetLocalMatrix()._42 = vPos.y;
		m_pParent->GetTransform()->GetLocalMatrix()._43 = vPos.z;
		
	}

	void SetAttachedObject(CObject* pObj)
	{
		m_pAttachedObject = pObj;
	}

	//Warning, only call once
	void SetDuration(float fDuration)
	{
		static bool bHasBeenCalled = false;	//only call this function once, you have been warned
		if(bHasBeenCalled)
		{
			m_fDuration = fDuration;
			bHasBeenCalled = true;	//don't do it again, duration is not to be changed
		}
	}

	void SetParent(CObject* pParent)
	{	//this set function technically attaches the component to a player/ai object
		m_pParent = pParent;
	}
	void SetIsSpawned(bool bIsSpawned)
	{
		m_bIsSpawned = bIsSpawned;
	}

};



#endif