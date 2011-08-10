#include "Jam.h"
#include "..\..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;
#include "..\..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\..\Managers\Component Managers\CHeldItemManager.h"

CRayJam* CRayJam::CreateRayJamComponent(CObject* pObj)
{
	CRayJam* pComp = MMNEW(CRayJam(pObj));
	pComp->JAM_ID = -1;
	pComp->FirstInit();
	pObj->AddComponent(pComp);
	return pComp;
}
void CRayJam::FirstInit()
{
	m_fDuration = 10.0f;
	JAM_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	std::string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);
	//CEventManager::GetInstance()->RegisterEvent("UseJam", this, ReInit);
	//register for events
}
//call backs
void CRayJam::Update(IEvent* cEvent, IComponent* cCenter)
{
	CRayJam* pComp = (CRayJam*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDt = eEvent->m_fDeltaTime;
	if(!pComp->GetIsSpawned())
	{
		return;
	}
	pComp->SetTimeLeft(pComp->GetTimeLeft()-fDt);
	CWwiseSoundManager::GetInstance()->SetObjectPosition(pComp->JAM_ID, pComp->GetParent()->GetTransform()->GetWorldPosition(), 0.35f);
	if(pComp->GetTimeLeft() <= 0.0f)
	{
		pComp->Despawn();
		return;
	}
}
void CRayJam::ReInit(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CRayJam* pComp = (CRayJam*)cCenter;
	//TStatusEffectEvent* eEvent = (TStatusEffectEvent*)cEvent->GetData();
	//m_fTimeLeft = m_fDuration;
	pComp->JAM_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	CWwiseSoundManager::GetInstance()->SetObjectPosition(pComp->JAM_ID, pComp->m_pParent->GetTransform()->GetWorldPosition(), 0.35f);
	//SendStatusEffectEvent("JamEffect",pComp, eEvent->m_pObject, pComp->m_fDuration);
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_JAM_USE, pComp->JAM_ID);
	//pComp->SetIsSpawned(true);
}
void CRayJam::Despawn()
{
	CWwiseSoundManager::GetInstance()->UnregisterObject(JAM_ID);
	m_bIsSpawned = false;
	m_pParent->GetTransform()->GetLocalMatrix()._41 = 300.0f;
	m_pParent->GetTransform()->GetLocalMatrix()._42 = 300.0f;
	m_pParent->GetTransform()->GetLocalMatrix()._43 = 300.0f;
}
