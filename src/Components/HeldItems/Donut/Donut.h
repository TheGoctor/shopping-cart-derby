#ifndef CDONUT_H_
#define CDONUT_H_

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
#define DONUTTIME 15.0f
class IEvent;
class CRenderComponent;

class CDonut : public IComponent
{
private:
	float m_fTimeRemaining;
	float m_fDuration;
	CObject* m_pParent;
	CObject* m_pAttachedObject;
	bool m_bIsSpawned;
	int DONUT_ID;
public:
	CDonut(CObject* pObj) : m_pParent(pObj)
	{
	}
	static CDonut* CreateDonutComponent(CObject* pObj);
	void FirstInit(void);
	//call backs
	static void Update(IEvent* cEvent, IComponent* cCenter);
	static void PlayerCollision(IEvent* cEvent, IComponent* cCenter);
	void ReInit();
	void Despawn();

	//accessors
	float GetTimeRemaining() 
	{ 
		return m_fTimeRemaining; 
	}
	float GetDuration() 
	{
		return m_fDuration; 
	}
	CObject* GetParent() 
	{
		return m_pParent; 
	}
	bool GetIsSpawned()
	{
		return m_bIsSpawned;
	}
	
	//mutators
	void SetTimeRemaining(float fRemaining)
	{ 
		m_fTimeRemaining = fRemaining; 
	}
	void SetDuration(float fDuration)
	{
		m_fDuration = fDuration; 
	}
	void SetParent(CObject* pParent) 
	{ 
		m_pParent = pParent; 
	}
	void SetIsSpawned(bool bSpawned)
	{
		m_bIsSpawned = bSpawned;
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

};


#endif