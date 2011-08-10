#ifndef CREAMPIE_H_
#define CREAMPIE_H_

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

class IEvent;
class CRenderComponent;

#define BLIND_TIME (3.0f)

class CPie : public IComponent
{
private:
	CObject* m_pParent;
	bool m_bIsSpawned;
	D3DXVECTOR3 m_vDirection;
	float m_fDuration;
	float m_fTimeLeft;
	float m_fSpeed;
	int	CREAM_PIE_ID;
	
public:
	CPie(CObject* pObj) : m_pParent(pObj)
	{
	}
	static CPie* CreatePieComponent(CObject* pObj, D3DXVECTOR3 vDir);
	void FirstInit(void);
	//call backs
	static void Update(IEvent* cEvent, IComponent* cCenter);
	static void PlayerCollision(IEvent* cEvent, IComponent* cCenter);
	static void EnvironmentCollision(IEvent* cEvent, IComponent* cCenter);
	static void ItemCollision(IEvent* cEvent, IComponent* cCenter);
	void ReInit();
	void Despawn();
	

	//accessors
	
	CObject* GetParent() 
	{ 
		return m_pParent; 
	}
	bool GetIsSpawned()
	{
		return m_bIsSpawned; 
	}
	D3DXVECTOR3 GetDirection()
	{
		return m_vDirection; 
	}
	float GetDuration()
	{ 
		return m_fDuration; 
	}
	float GetTimeLeft()
	{
		return m_fTimeLeft;
	}
	//mutators
	
	void SetParent(CObject* pParent) 
	{ 
		m_pParent = pParent; 
	}
	void SetIsSpawned(bool bIsSpawned) 
	{ 
		m_bIsSpawned = bIsSpawned; 
	}
	void SetPosition(D3DXVECTOR3 vPos)
	{
		m_pParent->GetTransform()->GetLocalMatrix()._41 = vPos.x;
		m_pParent->GetTransform()->GetLocalMatrix()._42 = 0.5;
		m_pParent->GetTransform()->GetLocalMatrix()._43 = vPos.z;
	}
	void SetDirection(D3DXVECTOR3 vDir)
	{
		m_vDirection = vDir;
	}
	void SetDuration(float fDuration) 
	{ 
		m_fDuration = fDuration; 
	}
	void SetTimeLeft(float fRemaining)
	{
		m_fTimeLeft = fRemaining;
	}
	

	static void PauseUpdateCallback(IEvent*, IComponent* pComp);
	
};

#endif