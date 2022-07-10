#include "PeanutButter.h"
#include "Managers\Global Managers\Event Manager\CEventManager.h"
#include "Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;
#include "Managers\Global Managers\Object Manager\scd::objectManager.h"
#include "Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "Managers\Component Managers\CHeldItemManager.h"	



CPeanutButter* CPeanutButter::CreatePeanutButterComponent(scd::object *pObj)
{
	CPeanutButter* pComp = MMNEW(CPeanutButter(pObj));
	pComp->PEANUT_BUTTER_ID = -1;
	pComp->FirstInit();
	pObj->AddComponent(pComp);
	return pComp;
}
void CPeanutButter::FirstInit(void)
{
	m_fEffectDuration = 5.0f;
	m_fSoundCooldown = 0.0f;
	m_fDuration = 15.0f; // lifetime of pb
	m_fTimeRemaining = m_fDuration;
	m_bIsTossedForward = false;
	PEANUT_BUTTER_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	// register for events
	CEventManager::GetInstance()->RegisterEvent("HeldItemInWorldPlayerCollision", this, PlayerCollision);
	CEventManager::GetInstance()->RegisterEvent("HeldItemInWorldCollision", this, ItemCollision);
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);
	
	szEventName = "Update";
	szEventName += PAUSE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);	
	szEventName = "Update";
	szEventName += CONSOLE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);

	
	szEventName = "Update";
	szEventName += PAUSE_OPTIONS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);

	szEventName = "Update";
	szEventName += PAUSE_KEYBINDS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);

	szEventName = "Update";
	szEventName += QUIT_CONFIRMATION_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);
	szEventName = "Update";
	szEventName += IN_GAME_HOW_TO_PLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);
	szEventName = "Update";
	szEventName += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);

}
void CPeanutButter::ReInit()
{
	PEANUT_BUTTER_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	CWwiseSoundManager::GetInstance()->SetObjectPosition(PEANUT_BUTTER_ID, m_pParent->GetTransform()->GetWorldPosition(), 0.3f);
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_PEANUTBUTTER_USE, PEANUT_BUTTER_ID);
	SetIsSpawned(true);
	SetTimeRemaining(GetDuration());

}
//call backs
void CPeanutButter::Update(IEvent* cEvent, scd::base_component* cCenter)
{
	CPeanutButter* pComp = (CPeanutButter*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDT = eEvent->m_fDeltaTime;
	
	// if were not spawned, dont do antyhing
	if(!pComp->GetIsSpawned())
	{
		return;
	}
	pComp->SetTimeRemaining(pComp->GetTimeRemaining()-fDT);
	pComp->m_fSoundCooldown -= fDT; // update our cooldown for sound playing
	
	SendRenderEvent("AddToSet", pComp, pComp->m_pParent, PRIORITY_IMMEDIATE);

	CWwiseSoundManager::GetInstance()->SetObjectPosition(pComp->PEANUT_BUTTER_ID, pComp->GetParent()->GetTransform()->GetWorldPosition(), 0.25f);
	if(pComp->GetTimeRemaining()<=0.0f)
	{
		pComp->Despawn();
		return;
	}
}
void CPeanutButter::PlayerCollision(IEvent* cEvent, scd::base_component* cCenter)
{
	CPeanutButter* pComp = (CPeanutButter*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();
	
	//CWwiseSoundManager::GetInstance()->PlayTheSound(soundIDENUM, ObjID)

	// if we're the ones that collided and we're spawned
	if(pComp->GetParent() == tEvent->m_pCollidedWith && pComp->GetIsSpawned())
	{
		SendStatusEffectEvent("Slow", pComp, tEvent->m_pcCollider, pComp->m_fEffectDuration);

		if(pComp->m_fSoundCooldown <= 0.0f)
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(STATUS_SLOWED, pComp->PEANUT_BUTTER_ID);
			pComp->m_fSoundCooldown = pComp->m_fEffectDuration; // reset once it plays
		}
	}

	// dont despawn until our timer's out NEVERMIND LOL NEVERMIND THAT NEVERMIND LOL
	//pComp->Despawn();
}
void CPeanutButter::ItemCollision(IEvent* cEvent, scd::base_component* cCenter)
{
	CPeanutButter* pComp = (CPeanutButter*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();


}

void CPeanutButter::Despawn()
{
	CWwiseSoundManager::GetInstance()->UnregisterObject(PEANUT_BUTTER_ID);
		m_fTimeRemaining = 0.0f;
		m_bIsSpawned = false;
		m_pParent->GetTransform()->GetLocalMatrix()._41 = 300.0f;
		m_pParent->GetTransform()->GetLocalMatrix()._42 = 300.0f;
		m_pParent->GetTransform()->GetLocalMatrix()._43 = 300.0f;
	
}


void CPeanutButter::PauseUpdateCallback(IEvent*, scd::base_component* pComp)
{
	// Get the Effect Comp
	CPeanutButter* comp = (CPeanutButter*)pComp;
	if(comp->m_bIsSpawned)
	{
		SendRenderEvent("AddToSet", comp, comp->m_pParent, PRIORITY_UPDATE);
	}
}


