#ifndef CBANANA_H_
#define CBANANA_H_

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


#define SLOW_TIME (2.3f)

class CRenderComponent;
class CCollideable;
class IEvent;
class CRenderComponent;

class CBanana : public IComponent
{
private:
	CObject* m_pObject;
	bool m_bIsSpawned;
	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vDirection;
	CCollideable* m_pAreaOfEffect;
	int BANANA_ID;
public:
	CBanana(CObject* pObj) : m_pObject(pObj)
	{
	}
	static CBanana* CreateBananaComponent(CObject* pObj);
	void FirstInit();
	void ReInit();
	static void Update(IEvent* cEvent, IComponent* cCenter);
	static void PlayerCollision(IEvent* cEvent, IComponent* cCenter);
	void Despawn();
	
	//accessors
	bool GetIsSpawned(void)
	{
		return m_bIsSpawned;
	}
	D3DXVECTOR3 GetPosition(void)
	{
		return m_vPosition;
	}
	D3DXVECTOR3 GetDirection(void)
	{
		return m_vDirection;
	}
	CCollideable* GetAreaOfEffect(void)
	{
		return m_pAreaOfEffect;
	}
	//mutators
	void SetIsSpawned(bool bSpawned)
	{
		m_bIsSpawned = bSpawned;
	}
	void SetPosition(D3DXVECTOR3 vPos)
	{
		m_pObject->GetTransform()->GetLocalMatrix()._41 = vPos.x;
		m_pObject->GetTransform()->GetLocalMatrix()._42 = vPos.y;
		m_pObject->GetTransform()->GetLocalMatrix()._43 = vPos.z;
		m_vPosition = vPos;
	}
	void SetDirection(D3DXVECTOR3 vDir)
	{
		m_vDirection = vDir;
	}
	void SetAreaOfEffect(CCollideable* pAOE)
	{
		m_pAreaOfEffect = pAOE;
	}
	
	static void PauseUpdateCallback(IEvent*, IComponent* pComp);
};
#endif