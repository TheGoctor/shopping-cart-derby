#ifndef PEANUT_BUTTER_H_
#define PEANUT_BUTTER_H_

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
#define SLOWTIME (2.0f)
#define SLOWRATE (0.3f)

class IEvent;
class CRenderComponent;

class CPeanutButter : public IComponent
{
private:
	float m_fDuration;
	float m_fTimeRemaining;
	float m_fSlowRate;		//something to mult the speed by to gain the slowed effect
	bool m_bIsTossedForward;
	CObject *m_pParent;
	bool m_bIsSpawned;
	D3DXVECTOR3 m_vPosition;
	CCollideable *m_pcAreaOfEffect;
	float	m_fEffectDuration;
	float	m_fSoundCooldown;
	int PEANUT_BUTTER_ID;
public:
	CPeanutButter(CObject *pObj):m_pParent(pObj)
	{	
	}
	static CPeanutButter* CreatePeanutButterComponent(CObject *pObj);
	void FirstInit(void);
	//call backs
	static void Update(IEvent* cEvent, IComponent* cCenter);
	static void PlayerCollision(IEvent* cEvent, IComponent* cCenter);
	static void ItemCollision(IEvent* cEvent, IComponent* cCenter);

	
	void ReInit();
	void Despawn();
	//bananaccessors
	CCollideable * GetAOE(void)
	{
		return m_pcAreaOfEffect;
	}
	float GetTimeRemaining(void)
	{
		return m_fTimeRemaining;
	}
	float GetDuration(void)
	{
		return m_fDuration;
	}
	bool GetIsTossedForward(void)
	{
		return m_bIsTossedForward;
	}
	D3DXVECTOR3 GetPosition(void)
	{
		return m_vPosition;
	}
	CObject* GetParent(void)
	{
		return m_pParent;
	}
	bool GetIsSpawned(void)
	{
		return m_bIsSpawned;
	}
	//mutananas
	void SetTimeRemaining(float fTime)
	{
		m_fTimeRemaining = fTime;
	}
	void SetPosition(D3DXVECTOR3 vPos)
	{
		m_pParent->GetTransform()->GetLocalMatrix()._41 = vPos.x;
		m_pParent->GetTransform()->GetLocalMatrix()._42 = vPos.y;
		m_pParent->GetTransform()->GetLocalMatrix()._43 = vPos.z;
	}
	void SetDuration(float fDuration)
	{
		m_fDuration = fDuration;
	}
	void SetAOE(CCollideable* pAOE)
	{
		m_pcAreaOfEffect = pAOE;
	}
	void SetParent(CObject* pParent)
	{
		m_pParent = pParent;
	}
	void SetIsSpawned(bool bIsSpawned)
	{
		m_bIsSpawned = bIsSpawned;
	}
	void SetIsTossedForward(bool bForward)
	{
		m_bIsTossedForward = bForward;
	}


	static void PauseUpdateCallback(IEvent*, IComponent* pComp);

};

#endif