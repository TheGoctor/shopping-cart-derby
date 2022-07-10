#include "CreamPie.h"
#include "Managers\Global Managers\Event Manager\CEventManager.h"
#include "Managers\Global Managers\Event Manager\EventStructs.h"
 using namespace EventStructs;
#include "scd::object.h"
#include "Managers\Global Managers\Object Manager\scd::objectManager.h"
#include "Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "Managers\Component Managers\CHeldItemManager.h"
#include "Managers\Component Managers\CCollisionManager.h"
#include "Managers\Global Managers\Rendering Managers\CEffectManager.h"


 CPie* CPie::CreatePieComponent(scd::object* pObj, scd::vector3 vDir)
 {
	CPie* pComp = MMNEW(CPie(pObj));
	pComp->m_vDirection = vDir;
	pComp->CREAM_PIE_ID = -1;
	D3DXVec3Normalize(&pComp->m_vDirection, &pComp->m_vDirection);
	pComp->FirstInit();
	pObj->AddComponent(pComp);
	
	return pComp;
 }
 void CPie::FirstInit(void)
 {
	m_fDuration = 10.0f;
	m_fTimeLeft = m_fDuration;
	m_fSpeed = 28.0f;
	CREAM_PIE_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	std::string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);
	CEventManager::GetInstance()->RegisterEvent("HeldItemInWorldCollision", this, EnvironmentCollision);
	CEventManager::GetInstance()->RegisterEvent("HeldItemInWorldPlayerCollision", this, PlayerCollision);
	CEventManager::GetInstance()->RegisterEvent("PieDestroyedByItem", this, ItemCollision);

	
	szEvent = "Update";
	szEvent += PAUSE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);
	szEvent = "Update";
	szEvent += CONSOLE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);

	szEvent = "Update";
	szEvent += PAUSE_OPTIONS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);
	szEvent = "Update";
	szEvent += PAUSE_KEYBINDS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);
	szEvent = "Update";
	szEvent += QUIT_CONFIRMATION_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);

	szEvent = "Update";
	szEvent += IN_GAME_HOW_TO_PLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);
	szEvent = "Update";
	szEvent += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);


	SendObjectEvent("PieCreated", this, m_pParent, PRIORITY_IMMEDIATE);

 }
 //call backs
  void CPie::Update(IEvent* cEvent, scd::base_component* cCenter)
 {
	CPie* pComp = (CPie*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDt = eEvent->m_fDeltaTime;
	
	if(!pComp->GetIsSpawned())
	{
		return;
	}
	scd::vector3 posit = pComp->m_pParent->GetTransform()->GetWorldPosition();
	posit.y = 0.5f;
	pComp->SetPosition(posit);
	pComp->m_fTimeLeft-=fDt;

	if(pComp->m_fTimeLeft <=0.0f)
	{
		pComp->Despawn();
		return;
	}
	
	pComp->m_pParent->GetTransform()->TranslateFrame(pComp->m_vDirection*fDt*pComp->m_fSpeed);

	pComp->m_pParent->GetTransform()->RotateFrame(scd::vector3(0.0f, 1.0f, 0.0f), fDt*7.0f);
	
	CWwiseSoundManager::GetInstance()->SetObjectPosition(pComp->CREAM_PIE_ID, 
		pComp->m_pParent->GetTransform()->GetWorldPosition(), 0.30f);
	
	SendRenderEvent("AddToSet", pComp, pComp->m_pParent, PRIORITY_IMMEDIATE);
	

  }

  void CPie::PlayerCollision(IEvent* cEvent, scd::base_component* cCenter)
 {
	CPie* pComp = (CPie*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

	// for player collision, the collider is the player
	if(pComp->m_pParent == tEvent->m_pCollidedWith && pComp->m_bIsSpawned)
	{
		SendStatusEffectEvent("Blind", pComp, tEvent->m_pcCollider, BLIND_TIME);
		pComp->Despawn();
	}
 }
  void CPie::EnvironmentCollision(IEvent* cEvent, scd::base_component* cCenter)
 {
	CPie* pComp = (CPie*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();
	//hit a wall, it's a pie, it does not get to go on!
	//pComp->Despawn();

	// parent == collider was causing it to always think it collides
	// for environment collisions, the collider is the item
	if(pComp->m_pParent == tEvent->m_pcCollider && pComp->m_bIsSpawned)
	{	
		scd::vector3 MoveDir, ColNormal, ReflectDir; 
		
		MoveDir.x = pComp->GetParent()->GetTransform()->GetLocalMatrix()._31;
		MoveDir.y = pComp->GetParent()->GetTransform()->GetLocalMatrix()._32;
		MoveDir.z = pComp->GetParent()->GetTransform()->GetLocalMatrix()._33;
		D3DXVec3Normalize(&MoveDir, &MoveDir);
		ColNormal = tEvent->m_vNormal;
		D3DXVec3Normalize(&ColNormal, &ColNormal);

//		if(D3DXVec3Length(&ColNormal) == 0.0f)
//		{
//			//crap crap crap, bad normal!
//			return;//aye, there be no bounce happening here i guess
//		}
		//now that that's out of the way, lets do fancy stuff
//		ReflectDir = MoveDir - (D3DXVec3Dot(&MoveDir, &ColNormal) * ColNormal * 2.0f);
//		ReflectDir.y = 0.0f;
//		D3DXVec3Normalize(&ReflectDir, &ReflectDir);
//		pComp->m_vDirection = ReflectDir;
		// play sound
		if(ColNormal.x > 0.0f || ColNormal.x <= 0.0f && ColNormal.z == 0.0f)
		{
			pComp->m_vDirection.x *= -1.0f;
		}
		else if(ColNormal.z > 0.0f || ColNormal.z < 0.0f && ColNormal.x == 0.0f)
		{
			pComp->m_vDirection.z *= -1.0f;
		}
		else
		{
			pComp->m_vDirection.x *= -1.0f;
			pComp->m_vDirection.z *= -1.0f;
		}

		//pComp->Despawn(); // dont despawn anymore
		CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_CREAMPIE_USE, pComp->CREAM_PIE_ID);
	}
 }
  void CPie::ReInit()
 {
	 m_fTimeLeft = m_fDuration;
	 m_bIsSpawned = true;
	CREAM_PIE_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	CWwiseSoundManager::GetInstance()->SetObjectPosition(CREAM_PIE_ID, 
		m_pParent->GetTransform()->GetWorldPosition(), 0.3f);
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_CREAMPIE_USE, CREAM_PIE_ID);

	SendObjectEvent("PieEffect", this, m_pParent, PRIORITY_IMMEDIATE);

}
 void CPie::Despawn()
 {
	m_bIsSpawned = false;
	m_pParent->GetTransform()->GetLocalMatrix()._41 = 300.0f;
	m_pParent->GetTransform()->GetLocalMatrix()._42 = 300.0f;
	m_pParent->GetTransform()->GetLocalMatrix()._43 = 300.0f;
	CWwiseSoundManager::GetInstance()->UnregisterObject(CREAM_PIE_ID);

	SendObjectEvent("PieDespawned", this, m_pParent);

 }



 

 void CPie::PauseUpdateCallback(IEvent*, scd::base_component* pComp)
{
	// Get the Effect Comp
	CPie* comp = (CPie*)pComp;
	
	if(comp->m_bIsSpawned)
	{
		SendRenderEvent("AddToSet", comp, comp->m_pParent, PRIORITY_UPDATE);
	}
}

 void CPie::ItemCollision(IEvent* cEvent, scd::base_component* cCenter)
{
	CPie* pComp = (CPie*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();
	if(pComp->m_pParent == tEvent->m_pcCollider && pComp->GetIsSpawned())
	{	
		//effect stuff
		scd::vector3 p1, p2, trans;
		p2 = pComp->GetParent()->GetTransform()->GetWorldPosition();
		p1 = CCollisionManager::GetInstance()->GetPieDestroyObject()->GetTransform()->GetWorldPosition();
		trans = p2 - p1;//vector to the turkeys position
		CCollisionManager::GetInstance()->GetPieDestroyObject()->GetTransform()->TranslateFrame(trans);
		CEffectComponent* pEC = CEffectManager::GetInstance()->CreatePieHitComponent(CCollisionManager::GetInstance()->GetPieDestroyObject());
		pEC->SetDeadTimer(EC_TYPE_PIE_HIT, 0.0f);
		pEC->SwitchOnOffEmitters(EC_TYPE_PIE_HIT, true);

		pComp->Despawn();
	}
}

