#include "CTurkeyComponent.h"

#include "..\..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

#include "..\..\..\CObject.h"
#include "..\..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"


#include "..\..\..\Managers\Component Managers\CHeldItemManager.h"
#include "..\..\..\Managers\Component Managers\CCollisionManager.h"



CTurkeyComponent::CTurkeyComponent(CObject* pObj) : m_pObject(pObj)
{
}



CTurkeyComponent* CTurkeyComponent::CreateTurkeyComponent(CObject* pObj, D3DXVECTOR3 vDirection)
{
	CTurkeyComponent* comp = MMNEW(CTurkeyComponent(pObj));
	

	comp->m_vMoveDirection = vDirection;
	D3DXVec3Normalize(&comp->m_vMoveDirection, &comp->m_vMoveDirection);

	comp->FirstInit();
	
	pObj->AddComponent(comp);	

	return comp;
}

void CTurkeyComponent::FirstInit()
{
	m_fDuration = 30.0f;
	m_fTimeLeft = m_fDuration;
	m_fSpeed = 20.0f;
	m_fStunDuration = 2.0f;
	TURKEY_ID = 0;
	// TODO: subscribe to events
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);
	CEventManager::GetInstance()->RegisterEvent("HeldItemInWorldCollision", this, EnvironmentCollision);
	CEventManager::GetInstance()->RegisterEvent("HeldItemInWorldPlayerCollision", this, PlayerCollision);


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


	SendObjectEvent("TurkeyCreated", this, m_pObject);

}

void CTurkeyComponent::ReInit()
{
	m_fTimeLeft = m_fDuration;
	m_bSpawned = true;
	TURKEY_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	TSphere tsphere;
	tsphere.cPosition = D3DXVECTOR3(0,5,0);
	tsphere.fRadius = .5f;				// TODO: Is this the radius we want?
	m_pCollidableComponent->SetBVType(BSPHERE);
	m_pCollidableComponent->SetSphere(tsphere);

	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_TURKEY_USE, TURKEY_ID);
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_TURKEY_TRAVEL, TURKEY_ID);

	// let effect stuff know we're fired
	SendObjectEvent("TurkeyEffect", this, m_pObject);

}

void CTurkeyComponent::Despawn()
{
	m_bSpawned = false;
	m_pObject->GetTransform()->GetLocalMatrix()._41 = 300.0f;
	m_pObject->GetTransform()->GetLocalMatrix()._42 = 300.0f;
	m_pObject->GetTransform()->GetLocalMatrix()._43 = 300.0f;

	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_TURKEY_TRAVEL_STOP, TURKEY_ID);
	CWwiseSoundManager::GetInstance()->UnregisterObject(TURKEY_ID);
	// dont remove because it crashes the collidable manager
	//CCollisionManager::GetInstance()->RemoveNonStatic(m_pCollidableComponent, m_pObject->GetID());

	// instead, just set the sphere's radius to 0
	TSphere tsphere;
	tsphere.cPosition = m_pObject->GetTransform()->GetWorldPosition();
	tsphere.fRadius = 0.0f;				// TODO: Is this the radius we want?
	m_pCollidableComponent->SetBVType(BSPHERE);
	m_pCollidableComponent->SetSphere(tsphere);

	SendObjectEvent("TurkeyDespawned", this, m_pObject);
}


///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CTurkeyComponent::Update(IEvent* cEvent, IComponent* cCenter)
{
	CTurkeyComponent* comp = (CTurkeyComponent*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDt = eEvent->m_fDeltaTime;

	// if w'ere not spawned
	if(!comp->m_bSpawned)
	{
		// no need to update
		return;
	}

	comp->m_fTimeLeft -= fDt;

	if(comp->m_fTimeLeft <= 0.0f)
	{
		// destroy me sire!
		comp->Despawn();
		
		return;
	}
	

	// TODO: Play effect stuff here (frost, etc)


	// translate the object
	comp->m_pObject->GetTransform()->TranslateFrame(comp->m_vMoveDirection*fDt*comp->m_fSpeed);

	
	comp->m_pObject->GetTransform()->RotateFrame(D3DXVECTOR3(0.0f, 1.0f, 0.0f), fDt*5.0f);

	CWwiseSoundManager::GetInstance()->SetObjectPosition(comp->TURKEY_ID, 
		comp->m_pObject->GetTransform()->GetWorldPosition(), 0.25f);
	// tell it to render
	SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_IMMEDIATE);

}

// Collision on world
void CTurkeyComponent::EnvironmentCollision(IEvent* cEvent, IComponent* cCenter)
{
	CTurkeyComponent* pComp = (CTurkeyComponent*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

	// collider is the item, collided is the world geometry
	if(pComp->m_pObject == tEvent->m_pcCollider && pComp->m_bSpawned)
	{
		// bounce around normal
//		if(D3DXVec3LengthSq(&tEvent->m_vNormal) == 0.0f)
//		{
//			// generate a random direction 200 range -100 to 100 
//			pComp->m_vMoveDirection.x = static_cast<float>((rand()%200 - 100));
//			pComp->m_vMoveDirection.z = static_cast<float>((rand()%200 - 100));
//			D3DXVec3Normalize(&pComp->m_vMoveDirection, &pComp->m_vMoveDirection);
//		}
//		else
//		{
//			pComp->m_vMoveDirection = tEvent->m_vNormal;
//		}
		//fresh start, bounce based on the normal yo!
		//current direction, normal of collision, direction after reflection
		D3DXVECTOR3 MoveDir, ColNormal, ReflectDir; 
		MoveDir.x = pComp->m_pObject->GetTransform()->GetLocalMatrix()._31;
		MoveDir.y = pComp->m_pObject->GetTransform()->GetLocalMatrix()._32;
		MoveDir.z = pComp->m_pObject->GetTransform()->GetLocalMatrix()._33;
		D3DXVec3Normalize(&MoveDir, &MoveDir);
		ColNormal = tEvent->m_vNormal;
		D3DXVec3Normalize(&ColNormal, &ColNormal);

		if(D3DXVec3Length(&ColNormal) == 0.0f)
		{
			//crap crap crap, bad normal!
			return;//aye, there be no bounce happening here i guess
		}
		//now that that's out of the way, lets do fancy stuff
//		ReflectDir = MoveDir - (D3DXVec3Dot(&MoveDir, &ColNormal) * ColNormal * 2.0f);
//		ReflectDir.y = 0.0f;
//		D3DXVec3Normalize(&ReflectDir, &ReflectDir);
//		pComp->m_vMoveDirection = ReflectDir;
		// play sound
		if(ColNormal.x > 0.0f || ColNormal.x <= 0.0f && ColNormal.z == 0.0f)
		{
			pComp->m_vMoveDirection.x *= -1.0f;
		}
		else if(ColNormal.z > 0.0f || ColNormal.z < 0.0f && ColNormal.x == 0.0f)
		{
			pComp->m_vMoveDirection.z *= -1.0f;
		}
		else
		{
			pComp->m_vMoveDirection.x *= -1.0f;
			pComp->m_vMoveDirection.z *= -1.0f;
		}

		CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_TURKEY_EN_IMPACT, pComp->TURKEY_ID); // HACK: not global id
	}

}

// collision on player
void CTurkeyComponent::PlayerCollision(IEvent* cEvent, IComponent* cCenter)
{
	CTurkeyComponent* comp = (CTurkeyComponent*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

	// collider is cart, collided is item
	if(comp->m_pObject == tEvent->m_pCollidedWith && comp->m_bSpawned)
	{
		// send stun effect event to player
		SendStatusEffectEvent("Stun", comp, tEvent->m_pcCollider, comp->m_fStunDuration);
		// play sound

		CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_TURKEY_TRAVEL_STOP, comp->TURKEY_ID); // HACK: not global id
		CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_TURKEY_IMPACT, comp->TURKEY_ID);
		CWwiseSoundManager::GetInstance()->PlayTheSound(STATUS_STUN, comp->TURKEY_ID);
		// destroy me
		comp->Despawn();
		
		// tell who we rammed to drop an item
		SendRamEvent("PlayerRammed", comp, comp->m_pObject, tEvent->m_pcCollider);


	}
}


void CTurkeyComponent::PauseUpdateCallback(IEvent*, IComponent* pComp)
{
	// Get the Effect Comp
	CTurkeyComponent* comp = (CTurkeyComponent*)pComp;
	if(comp->m_bSpawned)
	{
		SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_UPDATE);
	}
}


