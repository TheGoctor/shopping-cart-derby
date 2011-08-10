////////////////////////////////////////////////////////////////////////////////
//	File			:	CEffectManager.cpp
//	Date			:	5/19/11
//	Mod. Date		:	07/26/11
//	Mod. Initials	:	HN
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Loading and Spawning of Effects
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
// Includes
#include <fstream>
#include "CEffectManager.h"
#include "..\\..\\..\\Components\\Level\\CGoalItemComponent.h"
#include "..\\..\\..\\Components\\Level\\CHeldItemComponent.h"
#include "Texture Managers\CTextureManager.h"
#include "..\Memory Manager\CMemoryManager.h"
#include "..\Event Manager\CIDGen.h"
#include "..\Sound Manager\CWwiseSoundManager.h"
#include "..\\..\\..\\Components\\Rendering\\CSlipVFXComp.h"
#include "..\\..\\..\\Components\\Rendering\\CJamUseEffectComp.h"
#include "..\\..\\..\\Components\\Rendering\\CJamHitEffectComp.h"
#include "Texture Managers\CHUDManager.h"
#include "dxutil.h"

// Defines
#define POPUP_COOLDOWN (1.0f)
#define SKID_LIFESPAN  (0.3f)
//#define JAM_HIT_POPUP_TIMER (1.0f)

// Don't use for arrays and only use when fin is your fstream
#define READ(var) (fin.read((char*)&var, sizeof(var)))

// Singleton Functions
CEffectManager::CEffectManager() : m_bFirstInit(true) 
{
	pOM = CObjectManager::GetInstance();
	pRenMan = Renderer::GetInstance();
}
CEffectManager::CEffectManager(CEffectManager& /*ref*/) 
{

}
CEffectManager& CEffectManager::operator=(CEffectManager& /*ref*/) 
{
	return *this; 
}

// Get Instance
CEffectManager* CEffectManager::GetInstance(void) 
{
	static CEffectManager effectManInst;
	return &effectManInst;
}

// Load Emitters
void CEffectManager::LoadEmitters(void)
{
	CEffectManager* pPEM = GetInstance();
	pPEM->LoadEmitter("Icon_Front.bin");
	pPEM->LoadEmitter("Icon_Front_End.bin");
	pPEM->LoadEmitter("Glow_Grow.bin");
	pPEM->LoadEmitter("Glow_Grow_End.bin");
	pPEM->LoadEmitter("Glow_Shrink.bin");
	pPEM->LoadEmitter("Glow_Shrink_End.bin");
	pPEM->LoadEmitter("Shine_Backdrop.bin");
	pPEM->LoadEmitter("Shine_Backdrop_End.bin");
	pPEM->LoadEmitter("Star_Spread.bin");
	pPEM->LoadEmitter("Star_Spread_End.bin");

	// Movement
	pPEM->LoadEmitter("Smoke_Puff.bin");

	// Boost
	pPEM->LoadEmitter("Cloud_Gray.bin");
	pPEM->LoadEmitter("Glow_Blue.bin");
	pPEM->LoadEmitter("Lighting_Big.bin");

	// Turkey
	pPEM->LoadEmitter("Blue_Puff.bin");
	pPEM->LoadEmitter("Snowflake_Trail.bin");
	pPEM->LoadEmitter("Snowflake_Trail_2.bin");
	pPEM->LoadEmitter("Snowflake_Burst.bin");
	pPEM->LoadEmitter("Snowflake_Burst2.bin");
	pPEM->LoadEmitter("Snowflake_Burst3.bin");
	pPEM->LoadEmitter("Krack_Small.bin");
	pPEM->LoadEmitter("Krack_Mid.bin");
	pPEM->LoadEmitter("Krack_Big.bin");
	pPEM->LoadEmitter("Krack_Popup.bin");

	// Chicken Soup
	pPEM->LoadEmitter("Bubble_Trail.bin");
	pPEM->LoadEmitter("Sparkle_Soup_Trail.bin");

	// Banana
	pPEM->LoadEmitter("Banana_Splat.bin");
	pPEM->LoadEmitter("Drip_Left.bin");
	pPEM->LoadEmitter("Drip_Right.bin");
	pPEM->LoadEmitter("Shloop_Popup.bin");

	// Peanut Butter
	pPEM->LoadEmitter("Squish_Popup.bin");
	pPEM->LoadEmitter("Peanut_Mud.bin");

	// Crash
	pPEM->LoadEmitter("Crash_Popup.bin");
	pPEM->LoadEmitter("Crash_Star_Small.bin");
	pPEM->LoadEmitter("Crash_Star_Mid.bin");
	pPEM->LoadEmitter("Crash_Star_Big.bin");
	pPEM->LoadEmitter("Bolt_Burst.bin");
	pPEM->LoadEmitter("Spark_Burst.bin");

	// Jam
	pPEM->LoadEmitter("Jam_Trail.bin");
	pPEM->LoadEmitter("Jam_Use_Trail.bin");
	pPEM->LoadEmitter("Jam_Hit_Meteor_Down.bin");
	pPEM->LoadEmitter("Jam_Hit_Splat.bin");
	pPEM->LoadEmitter("Splurch_Popup.bin");
	pPEM->LoadEmitter("Jam_Hit_Burst.bin");

	// Donut
	pPEM->LoadEmitter("Donut_Puff.bin");

	// Pie
	pPEM->LoadEmitter("Splat_Popup.bin");
	pPEM->LoadEmitter("Splat_Burst.bin");
	pPEM->LoadEmitter("Pie_Trail.bin");
	pPEM->LoadEmitter("Pie_Trail2.bin");

	// Victory
	pPEM->LoadEmitter("Firework_Trail.bin");
	pPEM->LoadEmitter("Firework_Burst.bin");
	pPEM->LoadEmitter("Con_Strip.bin");
	pPEM->LoadEmitter("Con_Star.bin");

	// Caution
	pPEM->LoadEmitter("Lose_Haz.bin");
}

// Init
void CEffectManager::Init(void)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	IComponent* pInstance = (IComponent*)GetInstance();

	if(m_bFirstInit)
		LoadEmitters();

	// Initalize Objects
	SetupJamSprite();
	SetupBlindSprite();
	SetupStealSprite();
	InitObjects();

	// Goal Item Icons
	m_nGoalItemIconFrames[0] = 0;//RC_ITEM_EFFECT_GORILLA_MILK;
	m_nGoalItemIconFrames[1] = 5;//RC_ITEM_EFFECT_CAPTAIN_FLAKEY;
	m_nGoalItemIconFrames[2] = 3;//RC_ITEM_EFFECT_PIZZA;
	m_nGoalItemIconFrames[3] = 7;//RC_ITEM_EFFECT_CAKE;
	m_nGoalItemIconFrames[4] = 6;//RC_ITEM_EFFECT_CANNED_BEARS;
	m_nGoalItemIconFrames[5] = 4;//RC_ITEM_EFFECT_CARROTS;
	m_nGoalItemIconFrames[6] = 2;//RC_ITEM_EFFECT_SAMMY_SANDWICH;
	m_nGoalItemIconFrames[7] = 1;//RC_ITEM_EFFECT_SQUID;
	m_nGoalItemIconFrames[8] = 8;//RC_ITEM_EFFECT_AGENT_ORANGE;

	// Goal Item Init
	pEM->RegisterEvent("GoalItemInit", pInstance, GoalItemInitCallback);

	// Goal Item Spawned
	pEM->RegisterEvent("GoalItemSpawned", pInstance, GoalItemSpawnedCallback);

	// Goal Item Despawned
	pEM->RegisterEvent("GoalItemDespawned",	pInstance, GoalItemDespawnedCallback);

	// Goal Item Collected
	pEM->RegisterEvent("GoalItemCollected",	pInstance, GoalItemCollectedCallback);

	// Pickup Item Spawned
	pEM->RegisterEvent("PickupItemSpawned", pInstance, PickupItemSpawnedCallback);

	// Pickup Item Dropped
	pEM->RegisterEvent("PickupItemDropped", pInstance, PickupItemDroppedCallback);

	// Pickup Item Despawn
	pEM->RegisterEvent("PickupItemDespawned", pInstance, PickupItemDespawnCallback);

	// Pickup Item Collected
	pEM->RegisterEvent("PickupItemCollected", pInstance, PickupItemCollectedCallback);

	// Cart Collision
	pEM->RegisterEvent("CartCollision", pInstance, CartCollisionCallback);

	// Boost
	pEM->RegisterEvent("UseBoost", pInstance, BoostCallback);

	// Jam
	pEM->RegisterEvent("UseJam", pInstance, JamCallback);

	// Donut
	//pEM->RegisterEvent("UseDonut", pInstance, DonutCallback);
	//pEM->RegisterEvent("DonutDespawn", pInstance, DonutCallback);
	pEM->RegisterEvent("StealHeldItemEffect", pInstance, StealCallback);

	// Turkey Stun
	pEM->RegisterEvent("StunEffect", pInstance, StunCallback);
	pEM->RegisterEvent("TurkeyCreated", pInstance, TurkeySpawnCallback);
	pEM->RegisterEvent("TurkeyEffect", pInstance, TurkeyFireCallback);
	pEM->RegisterEvent("TurkeyDespawned", pInstance, TurkeyDespawnCallback);

	// Pie
	pEM->RegisterEvent("PieCreated", pInstance, PieSpawnCallback);
	pEM->RegisterEvent("PieEffect", pInstance, PieFireCallback);
	pEM->RegisterEvent("PieDespawned", pInstance, PieDespawnCallback);

	// Slip
	pEM->RegisterEvent("SlipEffect", pInstance, SlipCallback);

	// Blind
	pEM->RegisterEvent("BlindEffect", pInstance, BlindCallback);

	// Slow
	pEM->RegisterEvent("SlowEffect", pInstance, SlowCallback);

	// Inv
	pEM->RegisterEvent("Invulnerable", pInstance, InvulnerableCallback);

	// Has 8
	pEM->RegisterEvent("Has8", pInstance, HaveEightCallback);
	pEM->RegisterEvent("ItemDropped", pInstance, LostItemCallback);

	// Update
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, pInstance, UpdateCallback);


	// Initalize Objects
	szEventName = "InitObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, pInstance, InitObjectsCallback);

	szEventName = "InitObjects";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, pInstance, InitObjectsCallback);

	// Shutdown Objects
	szEventName = "ShutdownObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, pInstance, ShutdownObjectsCallback);

	// Destroy Obj
	pEM->RegisterEvent("DestroyObject", pInstance, DestroyObjCallback);

	// Reset
	szEventName = "EnableObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, pInstance, EnableCallback);
	szEventName = "DisableObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, pInstance, DisableCallback);

	// Shutdown
	pEM->RegisterEvent("Shutdown", pInstance, ShutdownCallback);
}

// Initalize Objects
void CEffectManager::InitObjectsCallback(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	// Call Handler
	GetInstance()->InitObjects();
}
void CEffectManager::InitObjects(void)
{
	if(m_bFirstInit)
	{
		SetupPlayerEmitters();
		m_bFirstInit = false;
	}
	m_cJamEffect.SetOn(false);
	m_cBlindEffect.SetOn(false);
	m_cStealEffect.SetOn(false);
	m_cBoostEffect.SetOn(false);
	m_cJamEffect.ResetSprites();
	m_cBlindEffect.ResetSprites();
	m_cStealEffect.ResetSprites();
	m_cBoostEffect.CreateWindSprite();
	m_cJamEffect.SetSpritesActive(false);
	m_cBlindEffect.SetSpritesActive(false);
	m_cStealEffect.SetSpritesActive(false);

	// Comps
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		compIter->second->SwitchOnOffEmitters(EC_TYPE_CAUTION, false);
		//compIter->second->SetDeadTimer(EC_TYPE_CART_COLLISION_CRASH, 0.0f);
		compIter++;
	}
}

// Shutdown Objects
void CEffectManager::ShutdownObjectsCallback(IEvent*, IComponent*)
{
	// Call Handler
	GetInstance()->ShutdownObjects();
}
void CEffectManager::ShutdownObjects(void)
{
	// Clean Comps
	//EffectCompMap::iterator compIter;
	//compIter = m_cEmitterComps.begin();
	//while(compIter != m_cEmitterComps.end())
	//{
	//	MMDEL(compIter->second);
	//	compIter++;
	//}
	//m_cEmitterComps.clear();

	//CObjectManager* pOM = CObjectManager::GetInstance();

	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		compIter->second->SetDeadTimer(EC_TYPE_BOOST, 3.0f);
		compIter->second->SetDeadTimer(EC_TYPE_CART_COLLISION_CRASH, 2.0f);
		compIter->second->SetDeadTimer(EC_TYPE_GOAL_ITEM_COLLECTION, 3.0f);
		compIter->second->SetDeadTimer(EC_TYPE_TURKEY_STUN, 4.0f);
		compIter->second->SetDeadTimer(EC_TYPE_BANANA_SLIP, 3.0f);
		compIter->second->SetDeadTimer(EC_TYPE_PEANUT_SQUISH, 4.0f);
		compIter->second->SetDeadTimer(EC_TYPE_PIE_HIT, 4.0f);
		compIter->second->SetDeadTimer(EC_TYPE_COL_BURST, 3.0f);

		compIter++;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Helper Funcs
//////////////////////////////////////////////////////////////////////////////
void CEffectManager::CreateCartCollisionComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "CartCollisionGreenSmall";
	szEffectObjName += (char*)uID;
	CObject* pColEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "CartCollisionGreenMid";
	szEffectObjName += (char*)uID;
	CObject* pColEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "CartCollisionGreenBig";
	szEffectObjName += (char*)uID;
	CObject* pColEffect2 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "CartCollisionCrashPopup";
	szEffectObjName += (char*)uID;
	CObject* pColEffect3 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Crash_Star_Small.bin", pColEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Crash_Star_Mid.bin", pColEffect1);
	CParticleEmitter* pPE2 = GetCloneEmitter("Crash_Star_Big.bin", pColEffect2);
	CParticleEmitter* pPE3 = GetCloneEmitter("Crash_Popup.bin", pColEffect3);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pColEffect0,
		(DXMesh*)pPE0, RC_COLLISION_EFFECT_GREEN_SMALL, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pColEffect1,
		(DXMesh*)pPE1, RC_COLLISION_EFFECT_GREEN_MID, RF_PARTICLE);
	CRenderComponent* pRenComp2 = Renderer::GetInstance()->CreateRenderComp(pColEffect2,
		(DXMesh*)pPE2, RC_COLLISION_EFFECT_GREEN_BIG, RF_PARTICLE);
	CRenderComponent* pRenComp3 = Renderer::GetInstance()->CreateRenderComp(pColEffect3,
		(DXMesh*)pPE3, RC_COLLISION_EFFECT_CRASH_POPUP, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC0 = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_SMALL);
	pEC0->AddEmitter(EC_TYPE_CART_COLLISION_CRASH, pPE1, pRenComp1,
		pPlayerObj->GetTransform(), PE_TYPE_CRASH_MID);
	pEC0->AddEmitter(EC_TYPE_CART_COLLISION_CRASH, pPE2, pRenComp2,
		pPlayerObj->GetTransform(), PE_TYPE_CRASH_BIG);
	pEC0->AddEmitter(EC_TYPE_CART_COLLISION_CRASH, pPE3, pRenComp3,
		pPlayerObj->GetTransform(), PE_TYPE_CRASH_POPUP);
	pEC0->SetOnTarget(EC_TYPE_CART_COLLISION_CRASH, true);
}

void CEffectManager::CreateBoostComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "BoostGlow";
	szEffectObjName += (char*)uID;
	CObject* pBoost0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "BoostLightning";
	szEffectObjName += (char*)uID;
	CObject* pBoost1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "BoostCloud";
	szEffectObjName += (char*)uID;
	CObject* pBoost2 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	
	// Get Emitter
	CParticleEmitter* pPEB0 = GetCloneEmitter("Cloud_Gray.bin", pBoost0);
	CParticleEmitter* pPEB1 = GetCloneEmitter("Glow_Blue.bin", pBoost1);
	CParticleEmitter* pPEB2 = GetCloneEmitter("Lighting_Big.bin", pBoost2);
		
	// Create Render Comp
	CRenderComponent* pRenCompB0 = Renderer::GetInstance()->CreateRenderComp(pBoost0,
		(DXMesh*)pPEB0, RC_CLOUD_GRAY, RF_PARTICLE);
	CRenderComponent* pRenCompB1 = Renderer::GetInstance()->CreateRenderComp(pBoost1,
		(DXMesh*)pPEB1, RC_BOOST_GLOW_BLUE, RF_PARTICLE);
	CRenderComponent* pRenCompB2 = Renderer::GetInstance()->CreateRenderComp(pBoost2,
		(DXMesh*)pPEB2, RC_BOOST_LIGHTNING, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPEB0, pRenCompB0,
		EC_TYPE_BOOST, PE_TYPE_BOOST_CLOUD);
	pEC->AddEmitter(EC_TYPE_BOOST, pPEB1, pRenCompB1, pPlayerObj->GetTransform(),
		PE_TYPE_BOOST_GLOW);
	pEC->AddEmitter(EC_TYPE_BOOST, pPEB2, pRenCompB2, pPlayerObj->GetTransform(),
		PE_TYPE_BOOST_LIGHTNING);
	pEC->SetOnTarget(EC_TYPE_BOOST, true);
}
CEffectComponent* CEffectManager::CreateStunComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "TurkeyKrackSmall";
	szEffectObjName += (char*)uID;
	CObject* pStunEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "TurkeyKrackMid";
	szEffectObjName += (char*)uID;
	CObject* pStunEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "TurkeyKrackBig";
	szEffectObjName += (char*)uID;
	CObject* pStunEffect2 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "TurkeyKrackPopup";
	szEffectObjName += (char*)uID;
	CObject* pStunEffect3 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Burst
	szEffectObjName = "TurkeyBurst";
	szEffectObjName += (char*)uID;
	CObject* pStunEffect4 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "TurkeyBurst2";
	szEffectObjName += (char*)uID;
	CObject* pStunEffect5 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "TurkeyPuff";
	szEffectObjName += (char*)uID;
	CObject* pStunEffect6 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "TurkeyBurst3";
	szEffectObjName += (char*)uID;
	CObject* pStunEffect7 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);


	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Krack_Small.bin", pStunEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Krack_Mid.bin", pStunEffect1);
	CParticleEmitter* pPE2 = GetCloneEmitter("Krack_Big.bin", pStunEffect2);
	CParticleEmitter* pPE3 = GetCloneEmitter("Krack_Popup.bin", pStunEffect3);
	CParticleEmitter* pPE4 = GetCloneEmitter("Snowflake_Burst.bin", pStunEffect4);
	CParticleEmitter* pPE5 = GetCloneEmitter("Snowflake_Burst2.bin", pStunEffect5);
	CParticleEmitter* pPE6 = GetCloneEmitter("Blue_Puff.bin", pStunEffect6);
	CParticleEmitter* pPE7 = GetCloneEmitter("Snowflake_Burst3.bin", pStunEffect7);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pStunEffect0,
		(DXMesh*)pPE0, RC_TURKEY_KRACK_SMALL, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pStunEffect1,
		(DXMesh*)pPE1, RC_SNOWFLAKE_B_EFFECT, RF_PARTICLE);
	CRenderComponent* pRenComp2 = Renderer::GetInstance()->CreateRenderComp(pStunEffect2,
		(DXMesh*)pPE2, RC_TURKEY_KRACK_BIG, RF_PARTICLE);
	CRenderComponent* pRenComp3 = Renderer::GetInstance()->CreateRenderComp(pStunEffect3,
		(DXMesh*)pPE3, RC_TURKEY_KRACK, RF_PARTICLE);

	CRenderComponent* pRenComp4 = Renderer::GetInstance()->CreateRenderComp(pStunEffect4,
		(DXMesh*)pPE4, RC_SNOWFLAKE_EFFECT, RF_PARTICLE);
	CRenderComponent* pRenComp5 = Renderer::GetInstance()->CreateRenderComp(pStunEffect5,
		(DXMesh*)pPE5, RC_SNOWFLAKE_B_EFFECT, RF_PARTICLE);
	CRenderComponent* pRenComp6 = Renderer::GetInstance()->CreateRenderComp(pStunEffect6,
		(DXMesh*)pPE6, RC_BLUE_PUFF, RF_PARTICLE);
	CRenderComponent* pRenComp7 = Renderer::GetInstance()->CreateRenderComp(pStunEffect7,
		(DXMesh*)pPE7, RC_SNOWFLAKE_B_EFFECT, RF_PARTICLE);


	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_TURKEY_STUN, PE_TYPE_KRACK_SMALL);
	pEC->AddEmitter(EC_TYPE_TURKEY_STUN, pPE1, pRenComp1, pPlayerObj->GetTransform(),
		PE_TYPE_KRACK_MID);
	pEC->AddEmitter(EC_TYPE_TURKEY_STUN, pPE2, pRenComp2, pPlayerObj->GetTransform(),
		PE_TYPE_KRACK_BIG);
	pEC->AddEmitter(EC_TYPE_TURKEY_STUN, pPE3, pRenComp3, pPlayerObj->GetTransform(),
		PE_TYPE_KRACK_POPUP);

	pEC->AddEmitter(EC_TYPE_TURKEY_STUN, pPE4, pRenComp4, pPlayerObj->GetTransform(),
		PE_TYPE_TURKEY_SNOWFLAKE_BURST_A);
	pEC->AddEmitter(EC_TYPE_TURKEY_STUN, pPE5, pRenComp5, pPlayerObj->GetTransform(),
		PE_TYPE_TURKEY_SNOWFLAKE_BURST_B);
	pEC->AddEmitter(EC_TYPE_TURKEY_STUN, pPE6, pRenComp6, pPlayerObj->GetTransform(),
		PE_TYPE_TURKEY_PUFF);
	pEC->AddEmitter(EC_TYPE_TURKEY_STUN, pPE7, pRenComp7, pPlayerObj->GetTransform(),
		PE_TYPE_TURKEY_SNOWFLAKE_BURST_C);

	pEC->SetOnTarget(EC_TYPE_TURKEY_STUN, true);
	return pEC;
}

CEffectComponent* CEffectManager::CreateBoltsAndSparksEffect(D3DXVECTOR3 vColPt)
{
	static int snCount = 0;

	// Check for an inactive comp
	CEffectComponent* pComp = NULL;
	ObjEffectCompMap::iterator compIter = m_cBoltsAndSparks.begin();
	while(compIter != m_cBoltsAndSparks.end())
	{
		CEffectComponent* pCurrComp = compIter->second;
		if(pCurrComp->GetIsActiveEmitter(EC_TYPE_COL_BURST) == false)
		{
			pComp = pCurrComp;

			// We found one
			D3DXMATRIX& mWorldMat = compIter->first->GetTransform()->GetLocalMatrix();

			mWorldMat._41 = vColPt.x;
			mWorldMat._42 = 1.0f;
			mWorldMat._43 = vColPt.z;

			pCurrComp->SetDeadTimer(EC_TYPE_COL_BURST, 0.0f);
			pCurrComp->SwitchOnOffEmitters(EC_TYPE_COL_BURST, true);

			return pComp;

			//compIter = m_cBoltsAndSparks.erase(compIter);
		}
		//else
			compIter++;
	}

	// Create a New one
	//CObjectManager* pOM = CObjectManager::GetInstance();
	//Renderer* pRenMan = Renderer::GetInstance();

	// Create Effect Objs
	string szEffectObjName = "BoltBurstEffect";
	szEffectObjName += (char)snCount;
	CObject* pBNSEffect0 = pOM->CreateObject(szEffectObjName,
		vColPt.x, 1.0f, vColPt.z);
	szEffectObjName = "ColSparkEffect";
	szEffectObjName += (char)snCount;
	CObject* pBNSEffect1 = pOM->CreateObject(szEffectObjName, 0.0f, 0.0f, 0.0f,
		0.0f, pBNSEffect0);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Bolt_Burst.bin", pBNSEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Spark_Burst.bin", pBNSEffect1);

	// Create Render Comps
	CRenderComponent* pRenComp0 = pRenMan->CreateRenderComp(pBNSEffect0,
		(DXMesh*)pPE0, RC_BOLT, RF_PARTICLE);
	CRenderComponent* pRenComp1 = pRenMan->CreateRenderComp(pBNSEffect1,
		(DXMesh*)pPE1, RC_FIREWORK_ORANGE, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pBNSEffect0, pPE0, pRenComp0,
		EC_TYPE_COL_BURST, PE_TYPE_BOLT_BURST);
	pEC->AddEmitter(EC_TYPE_COL_BURST, pPE1, pRenComp1,
		pBNSEffect0->GetTransform(), PE_TYPE_SPARK_BURST);

	pEC->SetOnTarget(EC_TYPE_COL_BURST, true);

	m_cBoltsAndSparks.insert(make_pair(pBNSEffect0, pEC));

	++snCount;

	return pEC;
}

CEffectComponent* CEffectManager::CreateSlipComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "BananaFloorSplat";
	szEffectObjName += (char*)uID;
	CObject* pSlipEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "ShloopPopup";
	szEffectObjName += (char*)uID;
	CObject* pSlipEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Banana_Splat.bin", pSlipEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Shloop_Popup.bin", pSlipEffect1);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pSlipEffect0,
		(DXMesh*)pPE0, RC_FLOOR_SPLAT, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pSlipEffect1,
		(DXMesh*)pPE1, RC_SHLOOP_POPUP, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_BANANA_SLIP, PE_TYPE_BANANA_FLOOR_SPLAT);
	pEC->AddEmitter(EC_TYPE_BANANA_SLIP, pPE1, pRenComp1,
		pPlayerObj->GetTransform(), PE_TYPE_BANANA_SHLOOP_POPUP);
	pEC->SetOnTarget(EC_TYPE_BANANA_SLIP, true);
	return pEC;
}

CEffectComponent* CEffectManager::CreateSlipDripComp(CObject* pParentObj, bool bLeft)
{
	unsigned int uID = pParentObj->GetID();

	// set appropriate values
	string szMod;
	string szFileName;
	ERenderContext eRC;
	if(bLeft)
	{
		szMod = "Left";
		szFileName = "Drip_Left.bin";
		eRC = RC_DRIP_LEFT;
	}
	else
	{
		szMod = "Right";
		szFileName = "Drip_Right.bin";
		eRC = RC_DRIP_RIGHT;
	}

	// Create Objs
	string szEffectObjName = "Drip";
	szEffectObjName = szMod;
	szEffectObjName += (char*)uID;
	CObject* pDripEffect = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitters
	CParticleEmitter* pPE = GetCloneEmitter(szFileName, pDripEffect);

	// Render Comps
	CRenderComponent* pRenComp = Renderer::GetInstance()->CreateRenderComp(pDripEffect,
		(DXMesh*)pPE, eRC, RF_PARTICLE);

	// Effect Comp
	CEffectComponent* pEC = CreateEffectComp(pParentObj, pPE, pRenComp,
		EC_TYPE_BANANA_SLIP, PE_TYPE_BANANA_DRIP);
	pEC->SetOnTarget(EC_TYPE_BANANA_SLIP, true);

	return pEC;
}

// Mud
CEffectComponent* CEffectManager::CreateSlowMudComp(CObject* pParentObj)
{
	// Get Singletons
	//CObjectManager* pOM = CObjectManager::GetInstance();
	//Renderer* pRenMan = Renderer::GetInstance();

	// Setup Effects
	string szEffectObjName = "PBMud";
	szEffectObjName += (char*)pParentObj->GetID();
	CObject* pMudEffect = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitters
	CParticleEmitter* pPE1 = GetCloneEmitter("Peanut_Mud.bin", pMudEffect);

	// Create Render Comp
	CRenderComponent* pRenComp1 = pRenMan->CreateRenderComp(pMudEffect,
		(DXMesh*)pPE1, RC_CART_MUD, RF_PARTICLE);

	CEffectComponent* pEC = CreateEffectComp(pParentObj, pPE1, pRenComp1, EC_TYPE_CART_MUD,
		PE_TYPE_CART_MUD);

	// Create Effect Comps
	return pEC;
}

void CEffectManager::CreateSlowComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "SquishPopup";
	szEffectObjName += (char*)uID;
	CObject* pSlowEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE1 = GetCloneEmitter("Squish_Popup.bin", pSlowEffect1);

	// Create Render Comp
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pSlowEffect1,
		(DXMesh*)pPE1, RC_SQUISH_POPUP, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE1, pRenComp1,
		EC_TYPE_PEANUT_SQUISH, PE_TYPE_PEANUT_SQUISH_POPUP);
	pEC->SetOnTarget(EC_TYPE_PEANUT_SQUISH, true);
}

CEffectComponent* CEffectManager::CreateDonutUseComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "DonutPuff";
	szEffectObjName += (char*)uID;
	CObject* pDonutEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Donut_Puff.bin", pDonutEffect0);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pDonutEffect0,
		(DXMesh*)pPE0, RC_DONUT_EFFECT_PUFF, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_DONUT_USE, PE_TYPE_DONUT_PUFF);
	pEC->SetOnTarget(EC_TYPE_DONUT_USE, true);

	return pEC;
}

// Pie Hit
CEffectComponent* CEffectManager::CreatePieHitComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "PieSplat";
	szEffectObjName += (char*)uID;
	CObject* pPieEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "PieBurst";
	szEffectObjName += (char*)uID;
	CObject* pPieEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Splat_Popup.bin", pPieEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Splat_Burst.bin", pPieEffect1);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pPieEffect0,
		(DXMesh*)pPE0, RC_SPLAT_POPUP, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pPieEffect1,
		(DXMesh*)pPE1, RC_SPLAT_BURST, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_PIE_HIT, PE_TYPE_PIE_SPLAT_POPUP);
	pEC->AddEmitter(EC_TYPE_PIE_HIT, pPE1, pRenComp1, pPlayerObj->GetTransform(),
		PE_TYPE_PIE_SPLAT_POPUP);
	pEC->SetOnTarget(EC_TYPE_PIE_HIT, true);
	return pEC;
}

// Caution
void CEffectManager::CreateCautionEffectComponent(CObject* pParentObj)
{
	// Setup Effect Object
	unsigned int uID = pParentObj->GetID();
	string szEffectObjName = "Caution";
	szEffectObjName += (char*)uID;
	CObject* pCautionEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Lose_Haz.bin", pCautionEffect0);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pCautionEffect0,
		(DXMesh*)pPE0, RC_CAUTION, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pParentObj, pPE0, pRenComp0,
		EC_TYPE_CAUTION, PE_TYPE_CAUTION_SIGN);
	pEC->SetOnTarget(EC_TYPE_CAUTION, true);
}

// Firework
CEffectComponent* CEffectManager::CreateFireworkEffectComponent(CObject* pParentObj)
{
	// Get Singletons
	//CObjectManager* pOM = CObjectManager::GetInstance();
	CEventManager*  pEM = CEventManager::GetInstance();

	// Setup Effect Object
	unsigned int uID = pParentObj->GetID();
	string szEffectObjName = "FireworkTrail";
	szEffectObjName += (char*)uID;
	CObject* pFWEffect0 = pOM->CreateObject(szEffectObjName);
	szEffectObjName = "ForeworkBurst";
	szEffectObjName += (char*)uID;
	CObject* pFWEffect1 = pOM->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Firework_Trail.bin", pFWEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Firework_Burst.bin", pFWEffect1);

	// Create Render Comp
	ERenderContext eRenderConxtext;
	switch(rand() % 4)
	{
	case 0:
		eRenderConxtext = RC_FIREWORK_BLUE;		
		break;
	case 1:
		eRenderConxtext = RC_FIREWORK_GREEN;		
		break;
	case 2:
		eRenderConxtext = RC_FIREWORK_BLACK;		
		break;
	case 3:
		eRenderConxtext = RC_FIREWORK_ORANGE;		
		break;
	default:
		eRenderConxtext = RC_FIREWORK_BLUE;
		break;
	}

	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pFWEffect0,
		(DXMesh*)pPE0, eRenderConxtext, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pFWEffect1,
		(DXMesh*)pPE1, eRenderConxtext, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC0 = CreateEffectComp(pParentObj, pPE0, pRenComp0,
		EC_TYPE_FIREWORK_TRAIL, PE_TYPE_FIRE_WORK_TRAIL);

	CEffectComponent* pEC1 = CreateEffectComp(pParentObj, pPE1, pRenComp1,
		EC_TYPE_FIREWORK_BURST,	PE_TYPE_FIRE_WORK_BURST);
	pEC1->SetOnTarget(EC_TYPE_FIREWORK_BURST, true);

	// Register for Win Update
	string szEventName = "Update";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, pEC0, CEffectComponent::UpdateCallback);
	pEM->RegisterEvent(szEventName, pEC1, CEffectComponent::UpdateCallback);

	return pEC0;
}

// Confetti
CEffectComponent* CEffectManager::CreateConfettiEffectComponent(CObject* pParentObj)
{
	// Get Singeltons
	//CObjectManager* pOM = CObjectManager::GetInstance();
	CEventManager*  pEM = CEventManager::GetInstance();

	// Setup Effect Object
	unsigned int uID = pParentObj->GetID();
	string szEffectObjName = "ConfettiStrips";
	szEffectObjName += (char*)uID;
	CObject* pConEffect0 = pOM->CreateObject(szEffectObjName);
	szEffectObjName = "ConfettiStars";
	szEffectObjName += (char*)uID;
	CObject* pConEffect1 = pOM->CreateObject(szEffectObjName);

	// Get Emitters
	CParticleEmitter* pPE0 = GetCloneEmitter("Con_Strip.bin", pConEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Con_Star.bin", pConEffect1);

	// Create Ren Comps
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pConEffect0,
		(DXMesh*)pPE0, RC_CONFETTI_STRIP, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pConEffect1,
		(DXMesh*)pPE1, RC_CONFETTI_STAR, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC0 = CreateEffectComp(pParentObj, pPE0, pRenComp0,
		EC_TYPE_CONFETTI, PE_TYPE_CONFETTI_STRIP);
	pEC0->AddEmitter(EC_TYPE_CONFETTI, pPE1, pRenComp1, pParentObj->GetTransform(),
		PE_TYPE_CONFETTI_STAR);

	// Register for Win Update
	string szEventName = "Update";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, pEC0, CEffectComponent::UpdateCallback);

	return pEC0;
}

// Invulnerable
CEffectComponent* CEffectManager::CreateInvulnerableEffectComponent(CObject* pParent)
{
	// Setup Effect Object
	unsigned int uID = pParent->GetID();

	// Get Singeltons
	//CObjectManager* pOM = CObjectManager::GetInstance();

	// Create Objects
	string szEffectObjName = "BubbleTrail";
	szEffectObjName += (char*)uID;
	CObject* pChikenEffect0 = pOM->CreateObject(szEffectObjName);
	szEffectObjName = "BubbleSparkle";
	szEffectObjName += (char*)uID;
	CObject* pChikenEffect1 = pOM->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Bubble_Trail.bin", pChikenEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Sparkle_Soup_Trail.bin", pChikenEffect1);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pChikenEffect0,
		(DXMesh*)pPE0, RC_BROTH, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pChikenEffect1,
		(DXMesh*)pPE1, RC_SOUP_SPARKLE, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pParent, pPE0, pRenComp0,
		EC_TYPE_CHICKEN_INV, PE_TYPE_BUBBLE_TRAIL);
	pEC->AddEmitter(EC_TYPE_CHICKEN_INV, pPE1, pRenComp1, pParent->GetTransform(),
		PE_TYPE_SOUP_SPARKLE);

	return pEC;
}

void CEffectManager::SetupPlayerEmitters(void)
{
	// Get Player Objects
	CObject* pPlayer0 = CObjectManager::GetInstance()->GetObjectByName("Player0");
	CObject* pPlayer1 = CObjectManager::GetInstance()->GetObjectByName("Player1");
	CObject* pPlayer2 = CObjectManager::GetInstance()->GetObjectByName("Player2");
	CObject* pPlayer3 = CObjectManager::GetInstance()->GetObjectByName("Player3");
	
	if(pPlayer0)
	{
		CreateCartCollisionComponent(pPlayer0);
		CreateBoostComponent(pPlayer0);
		CreateStunComponent(pPlayer0);
		CreateSlipComponent(pPlayer0);
		CreateSlowComponent(pPlayer0);
		CreatePieHitComponent(pPlayer0);
		CJamUseEffectComp::CreateJamUseVFXComponent(pPlayer0);
		CJamHitEffectComp::CreateJamHitVFXComponent(pPlayer0);
		CSlipVFXComp::CreateSlipVFXComponent(pPlayer0);

		CHUDEffectsComp::CreateHUDEffectsComp(pPlayer0);
		CVictoryVFXComp::CreateVictoryVFXComponent(pPlayer0);
	}
	if(pPlayer1)
	{
		CreateBoostComponent(pPlayer1);
		CreateStunComponent(pPlayer1);
		CreateSlipComponent(pPlayer1);
		CreateSlowComponent(pPlayer1);
		CreatePieHitComponent(pPlayer1);
		CreateCautionEffectComponent(pPlayer1);
		CJamUseEffectComp::CreateJamUseVFXComponent(pPlayer1);
		CJamHitEffectComp::CreateJamHitVFXComponent(pPlayer1);
		CSlipVFXComp::CreateSlipVFXComponent(pPlayer1);
	}
	if(pPlayer2)
	{
		CreateBoostComponent(pPlayer2);
		CreateStunComponent(pPlayer2);
		CreateSlipComponent(pPlayer2);
		CreateSlowComponent(pPlayer2);
		CreatePieHitComponent(pPlayer2);
		CreateCautionEffectComponent(pPlayer2);
		CJamUseEffectComp::CreateJamUseVFXComponent(pPlayer2);
		CJamHitEffectComp::CreateJamHitVFXComponent(pPlayer2);
		CSlipVFXComp::CreateSlipVFXComponent(pPlayer2);
	}
	if(pPlayer3)
	{
		CreateBoostComponent(pPlayer3);
		CreateStunComponent(pPlayer3);
		CreateSlipComponent(pPlayer3);
		CreateSlowComponent(pPlayer3);
		CreatePieHitComponent(pPlayer3);
		CreateCautionEffectComponent(pPlayer3);
		CJamUseEffectComp::CreateJamUseVFXComponent(pPlayer3);
		CJamHitEffectComp::CreateJamHitVFXComponent(pPlayer3);
		CSlipVFXComp::CreateSlipVFXComponent(pPlayer3);
	}
}

CEffectComponent* CEffectManager::CreateDustEffectComponent(CObject* pParentObj)
{
	// Get Singletons
	//CObjectManager* pOM = CObjectManager::GetInstance();
	//Renderer* pRenMan = Renderer::GetInstance();

	// Setup Effects
	string szEffectObjName = "CartDust";
	szEffectObjName += (char*)pParentObj->GetID();

	CObject* pDustEffect = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitters
	CParticleEmitter* pPE1 = GetCloneEmitter("Smoke_Puff.bin", pDustEffect);

	// Create Render Comp
	CRenderComponent* pRenComp1 = pRenMan->CreateRenderComp(pDustEffect,
		(DXMesh*)pPE1, RC_CART_SMOKE_EFFECT, RF_PARTICLE);

	// Create Effect Comps
	return CreateEffectComp(pParentObj, pPE1, pRenComp1, EC_TYPE_CART_SMOKE,
		PE_TYPE_CART_DUST);
}

// Setup Radar Jam
void CEffectManager::SetupJamSprite(void)
{
	m_cJamEffect.ResetSprites();
}

// Setup Blind
void CEffectManager::SetupBlindSprite(void)
{
	m_cBlindEffect.ResetSprites();
}

// Setup Steal
void CEffectManager::SetupStealSprite(void)
{
	m_cStealEffect.ResetSprites();
}

// Create Goal Item Effect
void CEffectManager::CreateGoalItemEffectComponent(CObject* pItemObj,
												   EGoalItemType eType)
{
	// Create Effect Objs

		// Glow Grow
		unsigned int uID = pItemObj->GetID();
		string szEffectObjName = "GoalGlowGrow";
		szEffectObjName += (char*)uID;
		CObject* pEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
		szEffectObjName = "GoalGlowGrowEnd";
		szEffectObjName += (char*)uID;
		CObject* pEffectEnd0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

		// Glow Shrink
		szEffectObjName = "GoalGlowShrink";
		szEffectObjName += (char*)uID;
		CObject* pEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
		szEffectObjName = "GoalGlowShrinkEnd";
		szEffectObjName += (char*)uID;
		CObject* pEffectEnd1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

		// Stars
		szEffectObjName = "GoalStar";
		szEffectObjName += (char*)uID;
		CObject* pEffect2 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
		szEffectObjName = "GoalStarEnd";
		szEffectObjName += (char*)uID;
		CObject* pEffectEnd2 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

		// Shine
		szEffectObjName = "GoalShine";
		szEffectObjName += (char*)uID;
		CObject* pEffect3 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
		szEffectObjName = "GoalShineEnd";
		szEffectObjName += (char*)uID;
		CObject* pEffectEnd3 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

		// Icon
		szEffectObjName = "GoalIcon";
		szEffectObjName += (char*)uID;
		CObject* pEffect4 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
		szEffectObjName = "GoalIconEnd";
		szEffectObjName += (char*)uID;
		CObject* pEffectEnd4 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Glow_Grow.bin", pEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Glow_Shrink.bin", pEffect1);
	CParticleEmitter* pPE2 = GetCloneEmitter("Star_Spread.bin", pEffect2);
	CParticleEmitter* pPE3 = GetCloneEmitter("Shine_Backdrop.bin", pEffect3);
	CParticleEmitter* pPE4 = GetCloneEmitter("Icon_Front.bin", pEffect4);

	// End
	CParticleEmitter* pPEEnd0 = GetCloneEmitter("Glow_Grow_End.bin", pEffectEnd0);
	CParticleEmitter* pPEEnd1 = GetCloneEmitter("Glow_Shrink_End.bin", pEffectEnd1);
	CParticleEmitter* pPEEnd2 = GetCloneEmitter("Star_Spread_End.bin", pEffectEnd2);
	CParticleEmitter* pPEEnd3 = GetCloneEmitter("Shine_Backdrop_End.bin", pEffectEnd3);
	CParticleEmitter* pPEEnd4 = GetCloneEmitter("Icon_Front_End.bin", pEffectEnd4);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pEffect0,
		(DXMesh*)pPE0, RC_ITEM_EFFECT_GLOW, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pEffect1,
		(DXMesh*)pPE1, RC_ITEM_EFFECT_GLOW, RF_PARTICLE);
	CRenderComponent* pRenComp2 = Renderer::GetInstance()->CreateRenderComp(pEffect2,
		(DXMesh*)pPE2, RC_ITEM_EFFECT_STAR, RF_PARTICLE);
	CRenderComponent* pRenComp3 = Renderer::GetInstance()->CreateRenderComp(pEffect3,
		(DXMesh*)pPE3, RC_ITEM_EFFECT_SHINE, RF_PARTICLE);

	// End
	CRenderComponent* pRenCompEnd0 = Renderer::GetInstance()->CreateRenderComp(pEffectEnd0,
		(DXMesh*)pPEEnd0, RC_ITEM_EFFECT_GLOW, RF_PARTICLE);
	CRenderComponent* pRenCompEnd1 = Renderer::GetInstance()->CreateRenderComp(pEffectEnd1,
		(DXMesh*)pPEEnd1, RC_ITEM_EFFECT_GLOW, RF_PARTICLE);
	CRenderComponent* pRenCompEnd2 = Renderer::GetInstance()->CreateRenderComp(pEffectEnd2,
		(DXMesh*)pPEEnd2, RC_ITEM_EFFECT_STAR, RF_PARTICLE);
	CRenderComponent* pRenCompEnd3 = Renderer::GetInstance()->CreateRenderComp(pEffectEnd3,
		(DXMesh*)pPEEnd3, RC_ITEM_EFFECT_SHINE, RF_PARTICLE);

	// Find Icon
	int nIconFrame = m_nGoalItemIconFrames[eType];
	CRenderComponent* pRenComp4 = Renderer::GetInstance()->CreateRenderComp(pEffect4,
		(DXMesh*)pPE4, RC_GOAL_ICON_EFFECT_SHEET, RF_PARTICLE);
	CRenderComponent* pRenCompEnd4 = Renderer::GetInstance()->CreateRenderComp(pEffectEnd4,
		(DXMesh*)pPEEnd4, RC_GOAL_ICON_EFFECT_SHEET, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pItemObj, pPE0, pRenComp0,
		EC_TYPE_GOAL_ITEM, PE_TYPE_GOAL_ITEM_GLOW_GROW);
	pEC->AddEmitter(EC_TYPE_GOAL_ITEM, pPE1, pRenComp1,
		pItemObj->GetTransform(), PE_TYPE_GOAL_ITEM_GLOW_SHRINK);
	pEC->AddEmitter(EC_TYPE_GOAL_ITEM, pPE2, pRenComp2,
		pItemObj->GetTransform(), PE_TYPE_GOAL_ITEM_STARS);
	pEC->AddEmitter(EC_TYPE_GOAL_ITEM, pPE3, pRenComp3,
		pItemObj->GetTransform(), PE_TYPE_GOAL_ITEM_SHINE);
	
	pEC->AddEmitter(EC_TYPE_GOAL_ITEM, pPE4, pRenComp4,
		pItemObj->GetTransform(), PE_TYPE_GOAL_ITEM_ICON);
	pEC->SetFrame(EC_TYPE_GOAL_ITEM, PE_TYPE_GOAL_ITEM_ICON, nIconFrame);
	pEC->SetOnTarget(EC_TYPE_GOAL_ITEM, true);

	// End
	CEffectComponent* pECEnd = CreateEffectComp(pItemObj, pPEEnd0, pRenCompEnd0,
		EC_TYPE_GOAL_ITEM_COLLECTION, PE_TYPE_GOAL_ITEM_GLOW_GROW);
	pECEnd->AddEmitter(EC_TYPE_GOAL_ITEM_COLLECTION, pPEEnd1, pRenCompEnd1,
		pItemObj->GetTransform(), PE_TYPE_GOAL_ITEM_GLOW_SHRINK);
	pECEnd->AddEmitter(EC_TYPE_GOAL_ITEM_COLLECTION, pPEEnd2, pRenCompEnd2,
		pItemObj->GetTransform(), PE_TYPE_GOAL_ITEM_STARS);
	pECEnd->AddEmitter(EC_TYPE_GOAL_ITEM_COLLECTION, pPEEnd3, pRenCompEnd3,
		pItemObj->GetTransform(), PE_TYPE_GOAL_ITEM_SHINE);

	pECEnd->AddEmitter(EC_TYPE_GOAL_ITEM_COLLECTION, pPEEnd4, pRenCompEnd4,
		pItemObj->GetTransform(), PE_TYPE_GOAL_ITEM_ICON);
	pECEnd->SetFrame(EC_TYPE_GOAL_ITEM_COLLECTION, PE_TYPE_GOAL_ITEM_ICON, nIconFrame);
	pECEnd->SetOnTarget(EC_TYPE_GOAL_ITEM_COLLECTION, true);
}

// Turkey
void CEffectManager::CreateTurkeyEffectComponent(CObject* pTurkeyObj)
{
	// Setup Effect Object
	unsigned int uID = pTurkeyObj->GetID();
	string szEffectObjName = "TurkeyTrail";
	szEffectObjName += (char*)uID;
	CObject* pTurkeyEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "TurkeyTrail2";
	szEffectObjName += (char*)uID;
	CObject* pTurkeyEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Snowflake_Trail.bin", pTurkeyEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Snowflake_Trail_2.bin", pTurkeyEffect1);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pTurkeyEffect0,
		(DXMesh*)pPE0, RC_SNOWFLAKE_EFFECT, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pTurkeyEffect1,
		(DXMesh*)pPE1, RC_SNOWFLAKE_B_EFFECT, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pTurkeyObj, pPE0, pRenComp0,
		EC_TYPE_TURKEY_TRAIL, PE_TYPE_TURKEY_SNOWFLAKE_TRAIL_A);
	pEC->AddEmitter(EC_TYPE_TURKEY_TRAIL, pPE1, pRenComp1, pTurkeyObj->GetTransform(),
		PE_TYPE_TURKEY_SNOWFLAKE_TRAIL_B);
}

// Pie Trail
void CEffectManager::CreatePieTrailEffectComponent(CObject* pPieObj)
{
	// Setup Effect Object
	unsigned int uID = pPieObj->GetID();
	string szEffectObjName = "PieTrail";
	szEffectObjName += (char*)uID;
	CObject* pPieEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "PieTrail2";
	szEffectObjName += (char*)uID;
	CObject* pPieEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Pie_Trail.bin", pPieEffect0);
	CParticleEmitter* pPE1 = GetCloneEmitter("Pie_Trail2.bin", pPieEffect1);

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pPieEffect0,
		(DXMesh*)pPE0, RC_SPLAT_BURST, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pPieEffect1,
		(DXMesh*)pPE1, RC_SPLAT_BURST, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPieObj, pPE0, pRenComp0,
		EC_TYPE_PIE_TRAIL, PE_TYPE_PIE_TRAIL_A);
	pEC->AddEmitter(EC_TYPE_PIE_TRAIL, pPE1, pRenComp1, pPieObj->GetTransform(),
		PE_TYPE_PIE_TRAIL_B);
}

// Load Emitter
void CEffectManager::LoadEmitter(char* szFileName)
{
	// Get the Full Path
	string szFullPath = "Resource\\Effect Files\\";
	szFullPath += szFileName;

	// Open a FileStream
	fstream fin(szFullPath.c_str(), ios_base::in | ios_base::binary);

	if(fin.is_open())
	{
		// Create a new Emitter
		CParticleEmitter* pNewEmitter = MMNEW(CParticleEmitter);

		// Get Texture Length
		int nTexNameLen;
		fin.read((char*)&nTexNameLen, sizeof(nTexNameLen));

		// Get Texture Name
		char szTextureName[64];
		fin.read(szTextureName, nTexNameLen);

		// Get Textue Name
		szTextureName[nTexNameLen] = '\0';
		string szTexFilePath = szTextureName;
		pNewEmitter->SetTextureName(szTexFilePath);

		// Get Max Particles
		unsigned int uMaxParticles;
		READ(uMaxParticles);
		pNewEmitter->SetMaxParticles(uMaxParticles);

		// Get Min LifeTime
		float fMinLifeTime;
		READ(fMinLifeTime);
		pNewEmitter->SetMinLifeTime(fMinLifeTime);

		// Get Max LifeTime
		float fMaxLifeTime;
		READ(fMaxLifeTime);
		pNewEmitter->SetMaxLifeTime(fMaxLifeTime);

		// Get Continuous
		bool bContinuous;
		READ(bContinuous);
		pNewEmitter->SetContinuous(bContinuous);

		// Get Billboard
		bool bBillboard;
		READ(bBillboard);
		pNewEmitter->SetBillboard(bBillboard);

		// Get Spawn Area Type
		int nSpawnAreaType;
		READ(nSpawnAreaType);
		pNewEmitter->SetSpawnAreaType((EParticleEmitterSpawnAreaType)nSpawnAreaType);

		// Get Spawn Position
		D3DXVECTOR3 vSpawnPos;
		READ(vSpawnPos.x);
		READ(vSpawnPos.y);
		READ(vSpawnPos.z);
		pNewEmitter->SetSpawnPosition(vSpawnPos);

		// Get Spawn Rectangle Min
		D3DXVECTOR3 vSpawnRectMin;
		READ(vSpawnRectMin.x);
		READ(vSpawnRectMin.y);
		READ(vSpawnRectMin.z);
		pNewEmitter->SetSpawnRectMin(vSpawnRectMin);

		// Get Spawn Rectangle Max
		D3DXVECTOR3 vSpawnRectMax;
		READ(vSpawnRectMax.x);
		READ(vSpawnRectMax.y);
		READ(vSpawnRectMax.z);
		pNewEmitter->SetSpawnRectMax(vSpawnRectMax);

		// Get Spawn Ellipse Radius
		D3DXVECTOR3 vSpawnEllipseRadius;
		READ(vSpawnEllipseRadius.x);
		READ(vSpawnEllipseRadius.y);
		READ(vSpawnEllipseRadius.z);
		pNewEmitter->SetSpawnEllipseRadius(vSpawnEllipseRadius);

		// Get Spawn Ellipse Hole
		D3DXVECTOR3 vSpawnEllipseHole;
		READ(vSpawnEllipseHole.x);
		READ(vSpawnEllipseHole.y);
		READ(vSpawnEllipseHole.z);
		pNewEmitter->SetSpawnEllipseHole(vSpawnEllipseHole);

		// Get Initial Scale
		D3DXVECTOR2 vScaleInit;
		READ(vScaleInit.x);
		READ(vScaleInit.y);
		pNewEmitter->SetInitialScale(vScaleInit);

		// Get Scale Rate
		D3DXVECTOR2 vScaleRate;
		READ(vScaleRate.x);
		READ(vScaleRate.y);
		pNewEmitter->SetScaleRate(vScaleRate);

		// Get Initial Velocity
		D3DXVECTOR3 vVelInit;
		READ(vVelInit.x);
		READ(vVelInit.y);
		READ(vVelInit.z);
		pNewEmitter->SetInitialVelocity(vVelInit);

		// Get Velocity Rate
		D3DXVECTOR3 vVelRate;
		READ(vVelRate.x);
		READ(vVelRate.y);
		READ(vVelRate.z);
		pNewEmitter->SetVelocityRate(vVelRate);

		// Get Initial Acceleration
		D3DXVECTOR3 vAccInit;
		READ(vAccInit.x);
		READ(vAccInit.y);
		READ(vAccInit.z);
		pNewEmitter->SetInitialAcceleration(vAccInit);

		// Get Acceleration Rate
		D3DXVECTOR3 vAccRate;
		READ(vAccRate.x);
		READ(vAccRate.y);
		READ(vAccRate.z);
		pNewEmitter->SetAccelerationRate(vAccRate);

		// Get Rotation Local
		D3DXVECTOR3 vRotLocInit;
		READ(vRotLocInit.x);
		READ(vRotLocInit.y);
		READ(vRotLocInit.z);
		pNewEmitter->SetInitialRotationLocal(vRotLocInit);

		// Get Rotation Local Rate
		D3DXVECTOR3 vRotLocRate;
		READ(vRotLocRate.x);
		READ(vRotLocRate.y);
		READ(vRotLocRate.z);
		pNewEmitter->SetRotationLocalRate(vRotLocRate);

		// Get Initial Rotation World
		D3DXVECTOR3 vRotWorldInit;
		READ(vRotWorldInit.x);
		READ(vRotWorldInit.y);
		READ(vRotWorldInit.z);
		pNewEmitter->SetInitialRotationWorld(vRotWorldInit);

		// Get Rotation World Rate
		D3DXVECTOR3 vRotWorldRate;
		READ(vRotWorldRate.x);
		READ(vRotWorldRate.y);
		READ(vRotWorldRate.z);
		pNewEmitter->SetRotationWorldRate(vRotWorldRate);

		// Get Apply Gravity
		bool bApplyGravity;
		READ(bApplyGravity);
		pNewEmitter->SetApplyGravity(bApplyGravity);

		// Get Gravity Position
		D3DXVECTOR3 vGravPos;
		READ(vGravPos.x);
		READ(vGravPos.y);
		READ(vGravPos.z);
		pNewEmitter->SetGravityPosition(vGravPos);

		// Get Initial Gravity
		float fGravInit;
		READ(fGravInit);
		pNewEmitter->SetInitialGravity(fGravInit);

		// Get Gravity Rate
		float fGravRate;
		READ(fGravRate);
		pNewEmitter->SetGravityRate(fGravRate);

		// Get Random Color
		bool bRandomColor;
		READ(bRandomColor);
		pNewEmitter->SetRandomColor(bRandomColor);

		// Get Initial Color
		BYTE R, G, B, A;
		READ(R);
		READ(G);
		READ(B);
		READ(A);
		pNewEmitter->SetInitialAlpha(A);
		pNewEmitter->SetInitialRed(R);
		pNewEmitter->SetInitialGreen(G);
		pNewEmitter->SetInitialBlue(B);

		// Get Color Rate
		float fR, fG, fB, fA;
		READ(fR);
		READ(fG);
		READ(fB);
		READ(fA);
		pNewEmitter->SetAlphaRate(fA);
		pNewEmitter->SetRedRate(fR);
		pNewEmitter->SetGreenRate(fG);
		pNewEmitter->SetBlueRate(fB);

		// Get Source Blend
		int nSrcBlend;
		READ(nSrcBlend);
		pNewEmitter->SetSourceBlend(nSrcBlend);

		// Get Destination Blend
		int nDestBlend;
		READ(nDestBlend);
		pNewEmitter->SetDestinationBlend(nDestBlend);

		// Get Animation Frames
		int nAnimFrame;
		READ(nAnimFrame);
		pNewEmitter->SetNumFrames(nAnimFrame);

		// Get Number of Rows
		int nAnimRows;
		READ(nAnimRows);
		pNewEmitter->SetNumRows(nAnimRows);

		// Get Number of Cols
		int nAnimCols;
		READ(nAnimCols);
		pNewEmitter->SetNumCols(nAnimCols);

		// Get Animation Switch Time
		float fAnimSwitchTime;
		READ(fAnimSwitchTime);
		pNewEmitter->SetAnimationSwitchTime(fAnimSwitchTime);

		// Mesh Name
		char cChar = 'a';
		int count = 0;
		while(cChar != '\0')
		{
			pNewEmitter->m_szName[count] = szFileName[count];
			cChar = szFileName[count];
			++count;
		}

		// Insert into Map
		CIDGen* pIDGen = CIDGen::GetInstance();
		m_cEmitterTemplates.insert(make_pair(pIDGen->GetID(szFileName), pNewEmitter));

		// Close and Clear the FileStream
		SAFE_CLOSE(fin);
	}
}

// Get Emitter
CParticleEmitter* CEffectManager::GetCloneEmitter(string szEmitterName, CObject* pParentObj)
{
	// Find the Original
	unsigned int uID = CIDGen::GetInstance()->GetID(szEmitterName);
	CParticleEmitter* pOriginalEmitter = m_cEmitterTemplates[uID];

	// Clone Emitter
	CParticleEmitter* pCloneEmitter = MMNEW(CParticleEmitter);
	*pCloneEmitter = *pOriginalEmitter;
	
	// Store Clone
	m_cClonedEffect.insert(make_pair(pParentObj->GetID(), pCloneEmitter));

	// Return Clone
	return pCloneEmitter;
}

// Factory
CEffectComponent* CEffectManager::CreateEffectComp(CObject* pParent, CParticleEmitter* pParticleEffect, CRenderComponent* pRenderComp, 
												   EEmitterCompType eType, EParticleEmitterType eEmitterType)
{
	// Look for a Already ecisting Component
	EffectCompMap::iterator compIter = GetInstance()->m_cEmitterComps.find(pParent->GetID());
	CEffectComponent* comp;

	// Create Component
	if(compIter == GetInstance()->m_cEmitterComps.end())
	{
		comp = MMNEW(CEffectComponent(pParent));

		// Attach to Obj
		pParent->AddComponent(comp);

		// Add the first Effect to the Component
		comp->AddEmitter(eType, pParticleEffect, pRenderComp,
			pParent->GetTransform(), eEmitterType);

		// Insert into Map
		GetInstance()->m_cEmitterComps.insert(make_pair((unsigned int)pParent->GetID(), comp));
	}
	else
	{
		comp = compIter->second;

		// Add the first Effect to the Component
		comp->AddEmitter(eType, pParticleEffect, pRenderComp,
			pParent->GetTransform(), eEmitterType);
	}
	
	return comp;
}

// Goal Item Init
void CEffectManager::GoalItemInitCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->GoalItemInit((TGoalItemEvent*)pEvent->GetData());
}
void CEffectManager::GoalItemInit(TGoalItemEvent* pcObjEvent)
{
	// Get the Object 
	CObject* pObj = pcObjEvent->m_pcGoalItem->GetParent();
	CreateGoalItemEffectComponent(pObj, pcObjEvent->m_eGoalItemType);
}

// Goal Item Spawned
void CEffectManager::GoalItemSpawnedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->GoalItemSpawned((TGoalItemEvent*)pEvent->GetData());
}
void CEffectManager::GoalItemSpawned(TGoalItemEvent* pcObjEvent)
{
	// Find Component
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.find(pcObjEvent->m_pcGoalItem->GetParent()->GetID());
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM, true);
	}
}

// Goal Item Despawned
void CEffectManager::GoalItemDespawnedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->GoalItemDespawned((TGoalItemEvent*)pEvent->GetData());
}
void CEffectManager::GoalItemDespawned(TGoalItemEvent* pcObjEvent)
{
	// Find Component
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.find(pcObjEvent->m_pcGoalItem->GetParent()->GetID());
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM, false);
	}
}

// Goal Item Collected
void CEffectManager::GoalItemCollectedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->GoalItemCollected((TGoalItemCollectedEvent*)pEvent->GetData());
}
void CEffectManager::GoalItemCollected(TGoalItemCollectedEvent* pcObjEvent)
{
	// Find Component
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.find(pcObjEvent->m_pcGoalItem->GetID());
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM, false);
		pEC->SetParentFrame(EC_TYPE_GOAL_ITEM_COLLECTION, pcObjEvent->m_pcCollector->GetTransform());
		pEC->SetDeadTimer(EC_TYPE_GOAL_ITEM_COLLECTION, 0.0f);
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM_COLLECTION, true);
	}
}

// Pickup Item Spawned
void CEffectManager::PickupItemSpawnedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->PickupItemSpawned((TPickupItemEvent*)pEvent->GetData());
}
void CEffectManager::PickupItemSpawned(TPickupItemEvent* pcObjEvent)
{
	// Get the Object 
	CObject* pObj = pcObjEvent->m_pcPickupItem;
	CreateGoalItemEffectComponent(pObj, pcObjEvent->m_eItemType);
}

// Pickup Item Dropped
void CEffectManager::PickupItemDroppedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->PickupItemDropped((TPickupItemEvent*)pEvent->GetData());
}
void CEffectManager::PickupItemDropped(TPickupItemEvent* pcObjEvent)
{
	// Get ID
	unsigned int uID = pcObjEvent->m_pcPickupItem->GetID();
	EGoalItemType eType = pcObjEvent->m_eItemType;

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SetFrame(EC_TYPE_GOAL_ITEM, PE_TYPE_GOAL_ITEM_ICON, m_nGoalItemIconFrames[eType]);
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM, true);
	}
}

// Pickup Item Despawn
void CEffectManager::PickupItemDespawnCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->PickupItemDespawn((TObjectEvent*)pEvent->GetData());
}
void CEffectManager::PickupItemDespawn(TObjectEvent* pcObjEvent)
{
	// Get ID
	unsigned int uID = pcObjEvent->m_pcObj->GetID();

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM, false);
	}
}

// Pickup Item Collected
void CEffectManager::PickupItemCollectedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->PickupItemCollected((TPickupItemCollectedEvent*)pEvent->GetData());
}

void CEffectManager::PickupItemCollected(TPickupItemCollectedEvent* pcEvent)
{
	// Find Component
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.find(pcEvent->m_pcPickupItem->GetID());
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM, false);

		// Colletion Effect
		EGoalItemType eType = pcEvent->m_eItemType;
		pEC->SetParentFrame(EC_TYPE_GOAL_ITEM_COLLECTION, pcEvent->m_pcCollector->GetTransform());
		pEC->SetFrame(EC_TYPE_GOAL_ITEM_COLLECTION, PE_TYPE_GOAL_ITEM_ICON, m_nGoalItemIconFrames[eType]);

		pEC->SetDeadTimer(EC_TYPE_GOAL_ITEM_COLLECTION, 0.0f);
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM_COLLECTION, true);
	}
}

// Has 8
void CEffectManager::HaveEightCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->HaveEight((TObjectEvent*)pEvent->GetData());
}
void CEffectManager::HaveEight(TObjectEvent* pcObjEvent)
{
	// Get ID
	unsigned int uID = pcObjEvent->m_pcObj->GetID();

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_CAUTION, true);
	}
}

// Lost Item
void CEffectManager::LostItemCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->LostItem((TGoalItemCollectedEvent*)pEvent->GetData());
}
void CEffectManager::LostItem(TGoalItemCollectedEvent* pcEvent)
{
	// Get ID
	unsigned int uID = pcEvent->m_pcCollector->GetID();

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_CAUTION, false);
	}
}

// Inv
void CEffectManager::InvulnerableCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Invulnerable((TStatusEffectEvent*)pEvent->GetData());
}
void CEffectManager::Invulnerable(TStatusEffectEvent* pcEvent)
{
	// Get ID
	unsigned int uID = pcEvent->m_pObject->GetID();

	// Pie
	unsigned int nPlayerID = CObjectManager::GetInstance()->GetObjectByName("Player0")->GetID();
	if(uID == nPlayerID)
	{
		if(m_cBlindEffect.IsActive())
		{
			m_cBlindEffect.SetCounter(7.0f);
			m_cBlindEffect.SetStage(2);
		}

		if(m_cJamEffect.IsActive())
		{
			m_cJamEffect.SetCounter(8.0f);
			m_cJamEffect.SetStage(2);
		}
	}

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;

		pEC->SwitchOnOffEmitters(EC_TYPE_PEANUT_SQUISH, false);
	}
}

// Cart Collision
void CEffectManager::CartCollisionCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->CartCollision((TImpactEvent*)pEvent->GetData());
}
void CEffectManager::CartCollision(TImpactEvent* pcObjEvent)
{
	/*if(fCollisionTimer > 0.0f)
		return;*/

	// Comps
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;

		if(pcObjEvent->m_pcCollider	   == pEC->GetParent() ||
			pcObjEvent->m_pCollidedWith == pEC->GetParent())
		{
			if(pEC->GetCooldown(EC_TYPE_CART_COLLISION_CRASH) > 0.0f)
				return;

			switch(rand() % 4)
			{
			case 0: // Crash
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_SMALL, RC_COLLISION_EFFECT_GREEN_SMALL);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_MID, RC_COLLISION_EFFECT_GREEN_MID);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_BIG, RC_COLLISION_EFFECT_GREEN_BIG);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_POPUP, RC_COLLISION_EFFECT_CRASH_POPUP);
				break;
			case 1: // Pow
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_SMALL, RC_COLLISION_EFFECT_RED_SMALL);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_MID, RC_COLLISION_EFFECT_RED_MID);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_BIG, RC_COLLISION_EFFECT_RED_BIG);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_POPUP, RC_COLLISION_EFFECT_POW_POPUP);
				break;
			case 2: // Smash
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_SMALL, RC_COLLISION_EFFECT_BLUE_SMALL);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_MID, RC_COLLISION_EFFECT_BLUE_MID);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_BIG, RC_COLLISION_EFFECT_BLUE_BIG);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_POPUP, RC_COLLISION_EFFECT_SMASH_POPUP);
				break;
			case 3: // Crunch
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_SMALL, RC_COLLISION_EFFECT_YELLOW_SMALL);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_MID, RC_COLLISION_EFFECT_YELLOW_MID);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_BIG, RC_COLLISION_EFFECT_YELLOW_BIG);
				pEC->ChangeContext(EC_TYPE_CART_COLLISION_CRASH, PE_TYPE_CRASH_POPUP, RC_COLLISION_EFFECT_CRUNCH_POPUP);
				break;
			}

			pEC->SwitchOnOffEmitters(EC_TYPE_CART_COLLISION_CRASH, true);
			pEC->SetDeadTimer(EC_TYPE_CART_COLLISION_CRASH, 0.0f);
			pEC->SetCooldown(EC_TYPE_CART_COLLISION_CRASH, POPUP_COOLDOWN);
		}

		compIter++;
	}
	
	//Bolts and Sparks
	CEffectComponent* pECB = CreateBoltsAndSparksEffect(pcObjEvent->m_vColPt);
	pECB->SetDeadTimer(EC_TYPE_COL_BURST, 0.0f);
	pECB->SwitchOnOffEmitters(EC_TYPE_COL_BURST, true);
}

// Steal
void CEffectManager::StealCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Steal(pEvent);
}
void CEffectManager::Steal(IEvent* pEvent)
{
	// Set the Item Sprite Appropriatlly
	m_cStealEffect.SetItemSpriteComp((CSpriteComponent*)pEvent->GetSender());
	m_cStealEffect.SetOn(true);
}

// Boost
void CEffectManager::BoostCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Boost((TStatusEffectEvent*)pEvent->GetData());
}
void CEffectManager::Boost(TStatusEffectEvent* pcObjEvent)
{
	// If its the player
	unsigned int nPlayerID = CObjectManager::GetInstance()->GetObjectByName("Player0")->GetID();
	if(pcObjEvent->m_pObject->GetID() == nPlayerID)
	{
		m_cBoostEffect.SetOn(true);
		m_cBoostEffect.SetDuration(pcObjEvent->m_fDuration);
	}

	// Find the Component Associated with that Obj
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		if(pcObjEvent->m_pObject == pEC->GetParent())
		{
			pEC->SetDeadTimer(EC_TYPE_BOOST, 0.0f);
			pEC->SwitchOnOffEmitters(EC_TYPE_BOOST, true);
		}
		compIter++;
	}
}

// Stun
void CEffectManager::StunCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Stun((TStatusEffectEvent*)pEvent->GetData());
}
void CEffectManager::Stun(TStatusEffectEvent* pcObjEvent)
{
	// Find the Component Associated with that Obj
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		if(pcObjEvent->m_pObject == pEC->GetParent())
		{
			pEC->SetDeadTimer(EC_TYPE_TURKEY_STUN, 0.0f);
			pEC->SwitchOnOffEmitters(EC_TYPE_TURKEY_STUN, true);
			pEC->SetCooldown(EC_TYPE_TURKEY_STUN, POPUP_COOLDOWN);
		}
		compIter++;
	}
}

// Turkey Spawn
void CEffectManager::TurkeySpawnCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->TurkeySpawn((TObjectEvent*)pEvent->GetData());
}
void CEffectManager::TurkeySpawn(TObjectEvent* pcObjEvent)
{
	// Get the Object 
	CObject* pObj = pcObjEvent->m_pcObj;
	CreateTurkeyEffectComponent(pObj);	
}

// Pie Spawn
void CEffectManager::PieSpawnCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->PieSpawn((TObjectEvent*)pEvent->GetData());
}
void CEffectManager::PieSpawn(TObjectEvent* pcObjEvent)
{
	// Get the Object 
	CObject* pObj = pcObjEvent->m_pcObj;
	CreatePieTrailEffectComponent(pObj);	
}

// Turkey Fire
void CEffectManager::TurkeyFireCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->TurkeyFire((TObjectEvent*)pEvent->GetData());
}
void CEffectManager::TurkeyFire(TObjectEvent* pcObjEvent)
{
	// Get ID
	unsigned int uID = pcObjEvent->m_pcObj->GetID();

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SetEffectTimer(EC_TYPE_TURKEY_TRAIL, 0.1f);
		//pEC->SwitchOnOffEmitters(EC_TYPE_TURKEY_TRAIL, true);
	}
}

// Pie Fire
void CEffectManager::PieFireCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->PieFire((TObjectEvent*)pEvent->GetData());
}
void CEffectManager::PieFire(TObjectEvent* pcObjEvent)
{
	// Get ID
	unsigned int uID = pcObjEvent->m_pcObj->GetID();

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SetEffectTimer(EC_TYPE_PIE_TRAIL, 0.1f);
		//pEC->SwitchOnOffEmitters(EC_TYPE_PIE_TRAIL, true);
	}
}

// Turkey Despawn
void CEffectManager::TurkeyDespawnCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->TurkeyDespawn((TObjectEvent*)pEvent->GetData());
}
void CEffectManager::TurkeyDespawn(TObjectEvent* pcObjEvent)
{
	// Get ID
	unsigned int uID = pcObjEvent->m_pcObj->GetID();

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_TURKEY_TRAIL, false);
	}
}

// PIe Despawn
void CEffectManager::PieDespawnCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->PieDespawn((TObjectEvent*)pEvent->GetData());
}
void CEffectManager::PieDespawn(TObjectEvent* pcObjEvent)
{
	// Get ID
	unsigned int uID = pcObjEvent->m_pcObj->GetID();

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);
	if(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		pEC->SwitchOnOffEmitters(EC_TYPE_PIE_TRAIL, false);
	}
}


// Slip
void CEffectManager::SlipCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Slip((TStatusEffectEvent*)pEvent->GetData());
}
void CEffectManager::Slip(TStatusEffectEvent* pcObjEvent)
{
	// Find the Component Associated with that Obj
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		if(pcObjEvent->m_pObject == pEC->GetParent())
		{
			pEC->SetDeadTimer(EC_TYPE_BANANA_SLIP, 0.0f);
			pEC->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);
		}
		compIter++;
	}
}

// Slow
void CEffectManager::SlowCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Slow((TStatusEffectEvent*)pEvent->GetData());
}
void CEffectManager::Slow(TStatusEffectEvent* pcObjEvent)
{
	// Find the Component Associated with that Obj
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		if(pcObjEvent->m_pObject == pEC->GetParent())
		{
			if(pEC->GetCooldown(EC_TYPE_PEANUT_SQUISH) > 0.0f)
				return;

			pEC->SetDeadTimer(EC_TYPE_PEANUT_SQUISH, 0.0f);
			pEC->SwitchOnOffEmitters(EC_TYPE_PEANUT_SQUISH, true);
			pEC->SetCooldown(EC_TYPE_PEANUT_SQUISH, 3.0f);
		}
		compIter++;
	}
}

// Radar Jam
void CEffectManager::JamCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Jam((TStatusEffectEvent*)pEvent->GetData());
}
void CEffectManager::Jam(TStatusEffectEvent* pcObjEvent)
{	
	// Get Player Obj
	unsigned int nPlayerID = CIDGen::GetInstance()->GetID("Player0");
	if(pcObjEvent->m_pObject->GetID() != nPlayerID)
	{
		// Put jam effect on screen for player
		if(CMovementManager::GetInstance()->PlayerIsInvincible(
			CObjectManager::GetInstance()->GetObjectByName("Player0")) == false)
		{
			m_cJamEffect.SetOn(true);
		}
	}
}

// Blind
void CEffectManager::BlindCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Blind((TStatusEffectEvent*)pEvent->GetData());
}
void CEffectManager::Blind(TStatusEffectEvent* pcObjEvent)
{
	unsigned int nPlayerID = CObjectManager::GetInstance()->GetObjectByName("Player0")->GetID();
	if(pcObjEvent->m_pObject->GetID() == nPlayerID)
	{
		m_cBlindEffect.SetOn(true);
	}


	// Pie Hit
	// Find the Component Associated with that Obj
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		if(pcObjEvent->m_pObject == pEC->GetParent())
		{
			pEC->SetDeadTimer(EC_TYPE_PIE_HIT, 0.0f);
			pEC->SwitchOnOffEmitters(EC_TYPE_PIE_HIT, true);
		}
		compIter++;
	}
}

// Update
void CEffectManager::UpdateCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;
	GetInstance()->Update(fDT);
}
void CEffectManager::Update(float fDT)
{
	// Jam
	if(m_cJamEffect.IsActive())
		m_cJamEffect.Update(fDT);

	if(m_cBlindEffect.IsActive())
		m_cBlindEffect.Update(fDT);

	if(m_cStealEffect.IsActive())
		m_cStealEffect.Update(fDT);

	if(m_cBoostEffect.IsActive())
		m_cBoostEffect.Update(fDT);
}

// Shutdown
void CEffectManager::ShutdownCallback(IEvent*, IComponent*)
{
	GetInstance()->Shutdown();
}
void CEffectManager::Shutdown(void)
{
	// Emitters
	EmitterMap::iterator emitterIter;
	emitterIter = m_cEmitterTemplates.begin();
	while(emitterIter != m_cEmitterTemplates.end())
	{
		MMDEL(emitterIter->second);
		emitterIter++;
	}

	// Comps
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		//compIter->second->Shutdown();
		MMDEL(compIter->second);
		compIter++;
	}

	// Clones
	emitterIter = m_cClonedEffect.begin();
	while(emitterIter != m_cClonedEffect.end())
	{
		MMDEL(emitterIter->second);
		emitterIter++;
	}
}

// Reset
void CEffectManager::DisableCallback(IEvent*, IComponent*)
{
	GetInstance()->m_cJamEffect.SetSpritesActive(false);
	GetInstance()->m_cBlindEffect.SetSpritesActive(false);
	GetInstance()->m_cStealEffect.SetSpritesActive(false);
	GetInstance()->m_cBoostEffect.SetSpritesActive(false);
}
void CEffectManager::EnableCallback(IEvent*, IComponent*)
{
	GetInstance()->Enable();
}
void CEffectManager::Enable(void)
{
	if(m_cJamEffect.IsActive())
	{
		m_cJamEffect.SetSpritesActive(true);
	}

	if(m_cBlindEffect.IsActive())
	{
		m_cBlindEffect.SetSpritesActive(true);
	}

	if(m_cStealEffect.IsActive())
	{
		m_cStealEffect.SetSpritesActive(true);
	}

	if(m_cBoostEffect.IsActive())
	{
		m_cBoostEffect.SetSpritesActive(true);
	}
}

// Destroy Obj
void CEffectManager::DestroyObjCallback(IEvent* pEvent, IComponent*)
{
	CEffectManager* pEM = CEffectManager::GetInstance();

	// Get Obj
	TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();

	EffectCompMap::iterator compIter = pEM->m_cEmitterComps.find(pData->m_pcObj->GetID());
	if(compIter != pEM->m_cEmitterComps.end())
	{
		/*ObjMap::iterator pIter = pEM->m_cInvObjs.find(compIter->first);
		if(pIter != pEM->m_cInvObjs.end())
		{
			CObjectManager::GetInstance()->DestroyObject(pIter->second);
			CEventManager::GetInstance()->UnregisterEventAll(compIter->second);
			pEM->m_cInvObjs.erase(pIter);
		}*/
		pEM->m_cEmitterComps.erase(compIter);
	}

	// Look for Clone
	EmitterMap::iterator emitterIter = pEM->m_cClonedEffect.find(pData->m_pcObj->GetID());
	if(emitterIter != pEM->m_cClonedEffect.end())
	{
		MMDEL(emitterIter->second);
		pEM->m_cClonedEffect.erase(emitterIter);
	}

}

#undef READ