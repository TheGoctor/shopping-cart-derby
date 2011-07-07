////////////////////////////////////////////////////////////////////////////////
//	File			:	CEffectManager.cpp
//	Date			:	5/19/11
//	Mod. Date		:	5/19/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Loading and Spawning of Effects
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
// Includes
#include <fstream>
#include "CEffectManager.h"
#include "Renderer.h"
#include "..\\Object Manager\\CObjectManager.h"
#include "..\\..\\..\\Components\\Level\\CGoalItemComponent.h"
#include "..\\..\\..\\Components\\Level\\CHeldItemComponent.h"
#include "Texture Managers\CTextureManager.h"
#include "..\Memory Manager\CMemoryManager.h"
#include "..\Event Manager\CIDGen.h"
#include "..\Sound Manager\CWwiseSoundManager.h"
#include "..\\..\\..\\Components\\Rendering\\CSlipVFXComp.h"
#include "dxutil.h"

// Defines
#define POPUP_COOLDOWN (1.0f)
#define SKID_LIFESPAN  (0.3f)
#define JAM_HIT_POPUP_TIMER (1.0f)

// Don't use for arrays and only use when fin is your fstream
#define READ(var) (fin.read((char*)&var, sizeof(var)))

// Singleton Functions
CEffectManager::CEffectManager() : m_bFirstInit(true), m_pDustLeft(NULL), m_pDustRight(NULL), m_pSciGlowS(NULL), m_pSciGlowG(NULL) {}
CEffectManager::CEffectManager(CEffectManager& /*ref*/) {}
CEffectManager& CEffectManager::operator=(CEffectManager& /*ref*/) { return *this; }

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
	pPEM->LoadEmitter("Blue_Coin.bin");
	pPEM->LoadEmitter("Blue_Coin_End.bin");
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
	pPEM->LoadEmitter("Skid_Marks.bin");

	// Boost
	pPEM->LoadEmitter("Cloud_Gray.bin");
	pPEM->LoadEmitter("Glow_Blue.bin");
	pPEM->LoadEmitter("Lighting_Big.bin");

	// Held Items
	pPEM->LoadEmitter("Held_Item.bin");

	// Turkey
	pPEM->LoadEmitter("Snowflake_Trail.bin");
	pPEM->LoadEmitter("Snowflake_Trail_2.bin");
	pPEM->LoadEmitter("Snowflake_Burst.bin");
	pPEM->LoadEmitter("Snowflake_Burst2.bin");
	pPEM->LoadEmitter("Krack_Small.bin");
	pPEM->LoadEmitter("Krack_Mid.bin");
	pPEM->LoadEmitter("Krack_Big.bin");
	pPEM->LoadEmitter("Krack_Popup.bin");

	// Chicken Soup
	pPEM->LoadEmitter("Bubble_Trail.bin");

	// Banana
	pPEM->LoadEmitter("Banana_Splat.bin");
	pPEM->LoadEmitter("Drip_Left.bin");
	pPEM->LoadEmitter("Drip_Right.bin");
	pPEM->LoadEmitter("Shloop_Popup.bin");

	// Peanut Butter
	//pPEM->LoadEmitter("Peanut_Splash.bin");
	pPEM->LoadEmitter("Squish_Popup.bin");

	// Crash
	pPEM->LoadEmitter("Crash_Popup.bin");
	pPEM->LoadEmitter("Green_Star_Small.bin");
	pPEM->LoadEmitter("Green_Star_Mid.bin");
	pPEM->LoadEmitter("Green_Star_Big.bin");

	// Jam
	pPEM->LoadEmitter("Jam_Rocket.bin");
	pPEM->LoadEmitter("Jam_Jar.bin");
	pPEM->LoadEmitter("Jam_Rocket_Down.bin");
	pPEM->LoadEmitter("Jam_Jar_Down.bin");
	pPEM->LoadEmitter("Jam_Hit_Splat.bin");
	pPEM->LoadEmitter("Splurch_Popup.bin");

	// Donut
	pPEM->LoadEmitter("Donut_Puff.bin");

	// Pie
	pPEM->LoadEmitter("Splat_Popup.bin");

	// Victory
	pPEM->LoadEmitter("Firework_Trail.bin");
	pPEM->LoadEmitter("Firework_Burst.bin");
	pPEM->LoadEmitter("Con_Strip.bin");
	pPEM->LoadEmitter("Con_Star.bin");

	// Caution
	pPEM->LoadEmitter("Lose_Haz.bin");

	// Scientists
	pPEM->LoadEmitter("Blue_SGlow_Shrink.bin");
	pPEM->LoadEmitter("Blue_SGlow_Grow.bin");
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
	m_nGoalItemIconRenderContexts[0] = RC_ITEM_EFFECT_GORILLA_MILK;
	m_nGoalItemIconRenderContexts[1] = RC_ITEM_EFFECT_CAPTAIN_FLAKEY;
	m_nGoalItemIconRenderContexts[2] = RC_ITEM_EFFECT_PIZZA;
	m_nGoalItemIconRenderContexts[3] = RC_ITEM_EFFECT_CAKE;
	m_nGoalItemIconRenderContexts[4] = RC_ITEM_EFFECT_CANNED_BEARS;
	m_nGoalItemIconRenderContexts[5] = RC_ITEM_EFFECT_CARROTS;
	m_nGoalItemIconRenderContexts[6] = RC_ITEM_EFFECT_SAMMY_SANDWICH;
	m_nGoalItemIconRenderContexts[7] = RC_ITEM_EFFECT_SQUID;
	m_nGoalItemIconRenderContexts[8] = RC_ITEM_EFFECT_AGENT_ORANGE;

	// Goal Item Init
	pEM->RegisterEvent("GoalItemInit", pInstance, GoalItemInitCallback);

	// Goal Item Spawned
	pEM->RegisterEvent("GoalItemSpawned", pInstance, GoalItemSpawnedCallback);

	// Goal Item Despawned
	pEM->RegisterEvent("GoalItemDespawned",	pInstance, GoalItemDespawnedCallback);

	// Goal Item Collected
	pEM->RegisterEvent("GoalItemCollected",	pInstance, GoalItemCollectedCallback);

	// Helpd Item Spawned
	//pEM->RegisterEvent("HeldItemSpawned", pInstance, HeldItemSpawnedCallback);

	// Helpd Item Collected
	//pEM->RegisterEvent("HeldItemCollected", pInstance, HeldItemCollectedCallback);

	// Pickup Item Spawned
	pEM->RegisterEvent("PickupItemSpawned", pInstance, PickupItemSpawnedCallback);

	// Pickup Item Dropped
	pEM->RegisterEvent("PickupItemDropped", pInstance, PickupItemDroppedCallback);

	// Pickup Item Despawn
	pEM->RegisterEvent("PickupItemDespawned", pInstance, PickupItemDespawnCallback);

	// Pickup Item Collected
	pEM->RegisterEvent("PickupItemCollected", pInstance, PickupItemCollectedCallback);

	// Cart Collision
	pEM->RegisterEvent("CartCollisionEffect", pInstance, CartCollisionCallback);

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
	//szEventName = "ShutdownObjects";
	//szEventName += GAMEPLAY_STATE;
	//pEM->RegisterEvent(szEventName, pInstance, ShutdownObjectsCallback);

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
	//SetupStunEmitters();
	//SetupInvulnerableEmitters();
	//SetupSlipEmitters();
	//SetupSlowEmitters();
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

	CObjectManager* pOM = CObjectManager::GetInstance();
	pOM->DestroyObject(m_pDustLeft);
	pOM->DestroyObject(m_pDustRight);
	pOM->DestroyObject(m_pSciGlowS);
	pOM->DestroyObject(m_pSciGlowG);

	m_pDustLeft  = NULL;
	m_pDustRight = NULL;
	m_pSciGlowS  = NULL;
	m_pSciGlowG  = NULL;
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

	// Pow
	/*szEffectObjName = "CartCollisionRedSmall";
	szEffectObjName += (char*)uID;
	CObject* pColPowEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "CartCollisionRedMid";
	szEffectObjName += (char*)uID;
	CObject* pColPowEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "CartCollisionRedBig";
	szEffectObjName += (char*)uID;
	CObject* pColPowEffect2 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "CartCollisionPowPopup";
	szEffectObjName += (char*)uID;
	CObject* pColPowEffect3 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);*/

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Green_Star_Small.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Green_Star_Mid.bin");
	CParticleEmitter* pPE2 = GetCloneEmitter("Green_Star_Big.bin");
	CParticleEmitter* pPE3 = GetCloneEmitter("Crash_Popup.bin");

	// Pow
	/*CParticleEmitter* pPERed0 = GetCloneEmitter("Pow_Small.bin");
	CParticleEmitter* pPERed1 = GetCloneEmitter("Pow_Mid.bin");
	CParticleEmitter* pPERed2 = GetCloneEmitter("Pow_Big.bin");
	CParticleEmitter* pPERed3 = GetCloneEmitter("Pow_Popup.bin");*/

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pColEffect0,
		(DXMesh*)pPE0, RC_COLLISION_EFFECT_GREEN_SMALL, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pColEffect1,
		(DXMesh*)pPE1, RC_COLLISION_EFFECT_GREEN_MID, RF_PARTICLE);
	CRenderComponent* pRenComp2 = Renderer::GetInstance()->CreateRenderComp(pColEffect2,
		(DXMesh*)pPE2, RC_COLLISION_EFFECT_GREEN_BIG, RF_PARTICLE);
	CRenderComponent* pRenComp3 = Renderer::GetInstance()->CreateRenderComp(pColEffect3,
		(DXMesh*)pPE3, RC_COLLISION_EFFECT_CRASH_POPUP, RF_PARTICLE);

	// Pow
	/*CRenderComponent* pRenCompRed0 = Renderer::GetInstance()->CreateRenderComp(pColPowEffect0,
		(DXMesh*)pPERed0, RC_COLLISION_EFFECT_RED_SMALL, RF_PARTICLE);
	CRenderComponent* pRenCompRed1 = Renderer::GetInstance()->CreateRenderComp(pColPowEffect1,
		(DXMesh*)pPERed1, RC_COLLISION_EFFECT_RED_MID, RF_PARTICLE);
	CRenderComponent* pRenCompRed2 = Renderer::GetInstance()->CreateRenderComp(pColPowEffect2,
		(DXMesh*)pPERed2, RC_COLLISION_EFFECT_RED_BIG, RF_PARTICLE);
	CRenderComponent* pRenCompRed3 = Renderer::GetInstance()->CreateRenderComp(pColPowEffect3,
		(DXMesh*)pPERed3, RC_COLLISION_EFFECT_POW_POPUP, RF_PARTICLE);*/

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

	// Pow
	/*CEffectComponent* pEC1 = CreateEffectComp(pPlayerObj, pPERed0, pRenCompRed0,
		EC_TYPE_CART_COLLISION_POW, PE_TYPE_POW_RED_SMALL);
	pEC1->AddEmitter(EC_TYPE_CART_COLLISION_POW, pPERed1, pRenCompRed1,
		pPlayerObj->GetTransform(), PE_TYPE_POW_RED_MID);
	pEC1->AddEmitter(EC_TYPE_CART_COLLISION_POW, pPERed2, pRenCompRed2,
		pPlayerObj->GetTransform(), PE_TYPE_POW_RED_BIG);
	pEC1->AddEmitter(EC_TYPE_CART_COLLISION_POW, pPERed3, pRenCompRed3,
		pPlayerObj->GetTransform(), PE_TYPE_POW_POPUP);
	pEC1->SetOnTarget(EC_TYPE_CART_COLLISION_POW, true);*/
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
	CParticleEmitter* pPEB0 = GetCloneEmitter("Cloud_Gray.bin");
	CParticleEmitter* pPEB1 = GetCloneEmitter("Glow_Blue.bin");
	CParticleEmitter* pPEB2 = GetCloneEmitter("Lighting_Big.bin");
		
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
void CEffectManager::CreateStunComponent(CObject* pPlayerObj)
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

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Krack_Small.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Krack_Mid.bin");
	CParticleEmitter* pPE2 = GetCloneEmitter("Krack_Big.bin");
	CParticleEmitter* pPE3 = GetCloneEmitter("Krack_Popup.bin");
	CParticleEmitter* pPE4 = GetCloneEmitter("Snowflake_Burst.bin");
	CParticleEmitter* pPE5 = GetCloneEmitter("Snowflake_Burst2.bin");

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

	pEC->SetOnTarget(EC_TYPE_TURKEY_STUN, true);
}

void CEffectManager::CreateSlipComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "BananaFloorSplat";
	szEffectObjName += (char*)uID;
	CObject* pSlipEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "ShloopPopup";
	szEffectObjName += (char*)uID;
	CObject* pSlipEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

//	szEffectObjName = "DripLeft";
//	szEffectObjName += (char*)uID;
//	CObject* pSlipEffect2 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
////	CObjectManager::GetInstance()->BindObjects(pPlayerObj, pSlipEffect2);
//
//	szEffectObjName = "DripRight";
//	szEffectObjName += (char*)uID;
//	CObject* pSlipEffect3 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
//	CObjectManager::GetInstance()->BindObjects(pPlayerObj, pSlipEffect2);


	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Banana_Splat.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Shloop_Popup.bin");
	//CParticleEmitter* pPE2 = GetCloneEmitter("Drip_Left.bin");
	//CParticleEmitter* pPE3 = GetCloneEmitter("Drip_Right.bin");

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pSlipEffect0,
		(DXMesh*)pPE0, RC_FLOOR_SPLAT, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pSlipEffect1,
		(DXMesh*)pPE1, RC_SHLOOP_POPUP, RF_PARTICLE);
	//CRenderComponent* pRenComp2 = Renderer::GetInstance()->CreateRenderComp(pSlipEffect2,
	//	(DXMesh*)pPE2, RC_DRIP_LEFT, RF_PARTICLE);
	//CRenderComponent* pRenComp3 = Renderer::GetInstance()->CreateRenderComp(pSlipEffect3,
	//	(DXMesh*)pPE3, RC_DRIP_RIGHT, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_BANANA_SLIP, PE_TYPE_BANANA_FLOOR_SPLAT);
	pEC->AddEmitter(EC_TYPE_BANANA_SLIP, pPE1, pRenComp1,
		pPlayerObj->GetTransform(), PE_TYPE_BANANA_SHLOOP_POPUP);
	/*pEC->AddEmitter(EC_TYPE_BANANA_SLIP, pPE2, pRenComp2,
		pPlayerObj->GetTransform(), PE_TYPE_BANANA_DRIP_LEFT);
	pEC->AddEmitter(EC_TYPE_BANANA_SLIP, pPE3, pRenComp3,
		pPlayerObj->GetTransform(), PE_TYPE_BANANA_DRIP_RIGHT);*/
	pEC->SetOnTarget(EC_TYPE_BANANA_SLIP, true);
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
	CParticleEmitter* pPE = GetCloneEmitter(szFileName);

	// Render Comps
	CRenderComponent* pRenComp = Renderer::GetInstance()->CreateRenderComp(pDripEffect,
		(DXMesh*)pPE, eRC, RF_PARTICLE);

	// Effect Comp
	CEffectComponent* pEC = CreateEffectComp(pParentObj, pPE, pRenComp,
		EC_TYPE_BANANA_SLIP, PE_TYPE_BANANA_DRIP_LEFT);
	pEC->SetOnTarget(EC_TYPE_BANANA_SLIP, true);

	return pEC;
}

void CEffectManager::CreateSlowComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	//string szEffectObjName = "PeanutSplashEffect";
	//szEffectObjName += (char*)uID;
	//CObject* pSlowEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	string szEffectObjName = "SquishPopup";
	szEffectObjName += (char*)uID;
	CObject* pSlowEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	//CParticleEmitter* pPE0 = GetCloneEmitter("Peanut_Splash.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Squish_Popup.bin");

	// Create Render Comp
	/*CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pSlowEffect0,
		(DXMesh*)pPE0, RC_PEANUT_EFFECT_SPLASH, RF_PARTICLE);*/
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pSlowEffect1,
		(DXMesh*)pPE1, RC_SQUISH_POPUP, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE1, pRenComp1,
		EC_TYPE_PEANUT_SQUISH, PE_TYPE_PEANUT_SQUISH_POPUP);
	
	/*CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_PEANUT_SQUISH, PE_TYPE_PEANUT_SPLASH);*/
	/*pEC->AddEmitter(EC_TYPE_PEANUT_SQUISH, pPE1, pRenComp1, pPlayerObj->GetTransform(),
		PE_TYPE_PEANUT_SQUISH_POPUP);*/
	pEC->SetOnTarget(EC_TYPE_PEANUT_SQUISH, true);
}

void CEffectManager::CreateJamUseComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "JamRocket";
	szEffectObjName += (char*)uID;
	CObject* pJamEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "JamJar";
	szEffectObjName += (char*)uID;
	CObject* pJamEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Jam_Rocket.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Jam_Jar.bin");

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pJamEffect0,
		(DXMesh*)pPE0, RC_JAM_USE_EFFECT_ROCKET, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pJamEffect1,
		(DXMesh*)pPE1, RC_JAM_USE_EFFECT_JAR, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_JAM_USE, PE_TYPE_JAM_ROCKET);
	pEC->AddEmitter(EC_TYPE_JAM_USE, pPE1, pRenComp1, pPlayerObj->GetTransform(),
		PE_TYPE_JAM_JAR);
	pEC->SetOnTarget(EC_TYPE_JAM_USE, true);
}

void CEffectManager::CreateJamHitComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "JamRocketDown";
	szEffectObjName += (char*)uID;
	CObject* pJamEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "JamJarDown";
	szEffectObjName += (char*)uID;
	CObject* pJamEffect1 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "JamHitBlob";
	szEffectObjName += (char*)uID;
	CObject* pJamEffect2 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);
	szEffectObjName = "JamSplurchPopup";
	szEffectObjName += (char*)uID;
	CObject* pJamEffect3 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Jam_Rocket_Down.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Jam_Jar_Down.bin");
	CParticleEmitter* pPE2 = GetCloneEmitter("Jam_Hit_Splat.bin");
	CParticleEmitter* pPE3 = GetCloneEmitter("Splurch_Popup.bin");

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pJamEffect0,
		(DXMesh*)pPE0, RC_JAM_USE_EFFECT_ROCKET, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pJamEffect1,
		(DXMesh*)pPE1, RC_JAM_USE_EFFECT_JAR, RF_PARTICLE);
	CRenderComponent* pRenComp2 = Renderer::GetInstance()->CreateRenderComp(pJamEffect2,
		(DXMesh*)pPE2, RC_JAM_HIT_EFFECT_BLOB, RF_PARTICLE);
	CRenderComponent* pRenComp3 = Renderer::GetInstance()->CreateRenderComp(pJamEffect3,
		(DXMesh*)pPE3, RC_JAM_HIT_EFFECT_SPLURCH_POPUP, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC0 = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_JAM_HIT_FALL, PE_TYPE_JAM_ROCKET_DOWN);
	pEC0->AddEmitter(EC_TYPE_JAM_HIT_FALL, pPE1, pRenComp1, pPlayerObj->GetTransform(),
		PE_TYPE_JAM_JAR_DOWN);
	pEC0->SetOnTarget(EC_TYPE_JAM_HIT_FALL, true);

	CEffectComponent* pEC1 = CreateEffectComp(pPlayerObj, pPE2, pRenComp2, EC_TYPE_JAM_HIT_POPUP,
		PE_TYPE_JAM_HIT_BLOB);
	pEC1->AddEmitter(EC_TYPE_JAM_HIT_POPUP, pPE3, pRenComp3, pPlayerObj->GetTransform(),
		PE_TYPE_JAM_HIT_POPUP);
	pEC1->SetOnTarget(EC_TYPE_JAM_HIT_POPUP, true);
}

CEffectComponent* CEffectManager::CreateDonutUseComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "DonutPuff";
	szEffectObjName += (char*)uID;
	CObject* pDonutEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Donut_Puff.bin");

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
void CEffectManager::CreatePieHitComponent(CObject* pPlayerObj)
{
	// Setup Effect Object
	unsigned int uID = pPlayerObj->GetID();
	string szEffectObjName = "PieHit";
	szEffectObjName += (char*)uID;
	CObject* pPieEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Splat_Popup.bin");

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pPieEffect0,
		(DXMesh*)pPE0, RC_SPLAT_POPUP, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pPlayerObj, pPE0, pRenComp0,
		EC_TYPE_PIE_HIT, PE_TYPE_PIE_SPLAT_POPUP);
	pEC->SetOnTarget(EC_TYPE_PIE_HIT, true);
}

// Caution
void CEffectManager::CreateCautionEffectComponent(CObject* pParentObj)
{
	// Setup Effect Object
	unsigned int uID = pParentObj->GetID();
	string szEffectObjName = "Caution";
	szEffectObjName += (char*)uID;
	CObject* pEffect0 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Lose_Haz.bin");

	// Create Render Comp
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pEffect0,
		(DXMesh*)pPE0, RC_CAUTION, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC = CreateEffectComp(pParentObj, pPE0, pRenComp0,
		EC_TYPE_CAUTION, PE_TYPE_CAUTION_SIGN);
	pEC->SetOnTarget(EC_TYPE_CAUTION, true);
	//pEC->SwitchOnOffEmitters(EC_TYPE_CAUTION, true);
}

// Firework
CEffectComponent* CEffectManager::CreateFireworkEffectComponent(CObject* pParentObj)
{
	// Get Singletons
	CObjectManager* pOM = CObjectManager::GetInstance();
	CEventManager*  pEM = CEventManager::GetInstance();

	// Setup Effect Object
	unsigned int uID = pParentObj->GetID();
	string szEffectObjName = "FireworkTrail";
	szEffectObjName += (char*)uID;
	CObject* pEffect0 = pOM->CreateObject(szEffectObjName);
	szEffectObjName = "ForeworkBurst";
	szEffectObjName += (char*)uID;
	CObject* pEffect1 = pOM->CreateObject(szEffectObjName);

	// Get Emitter
	CParticleEmitter* pPE0 = GetCloneEmitter("Firework_Trail.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Firework_Burst.bin");

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

	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pEffect0,
		(DXMesh*)pPE0, eRenderConxtext, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pEffect1,
		(DXMesh*)pPE1, eRenderConxtext, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC0 = CreateEffectComp(pParentObj, pPE0, pRenComp0,
		EC_TYPE_FIREWORK_TRAIL, PE_TYPE_FIRE_WORK_TRAIL);
	//pEC0->SetOnTarget(EC_TYPE_FIREWORK_TRAIL, true);

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
	CObjectManager* pOM = CObjectManager::GetInstance();
	CEventManager*  pEM = CEventManager::GetInstance();

	// Setup Effect Object
	unsigned int uID = pParentObj->GetID();
	string szEffectObjName = "ConfettiStrips";
	szEffectObjName += (char*)uID;
	CObject* pEffect0 = pOM->CreateObject(szEffectObjName);
	szEffectObjName = "ConfettiStars";
	szEffectObjName += (char*)uID;
	CObject* pEffect1 = pOM->CreateObject(szEffectObjName);

	// Get Emitters
	CParticleEmitter* pPE0 = GetCloneEmitter("Con_Strip.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Con_Star.bin");

	// Create Ren Comps
	CRenderComponent* pRenComp0 = Renderer::GetInstance()->CreateRenderComp(pEffect0,
		(DXMesh*)pPE0, RC_CONFETTI_STRIP, RF_PARTICLE);
	CRenderComponent* pRenComp1 = Renderer::GetInstance()->CreateRenderComp(pEffect1,
		(DXMesh*)pPE1, RC_CONFETTI_STAR, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC0 = CreateEffectComp(pParentObj, pPE0, pRenComp0,
		EC_TYPE_CONFETTI, PE_TYPE_CONFETTI_STRIP);
	pEC0->AddEmitter(EC_TYPE_CONFETTI, pPE1, pRenComp1, pParentObj->GetTransform(),
		PE_TYPE_CONFETTI_STAR);
	//pEC1->SetOnTarget(EC_TYPE_CONFETTI, true);

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

	// Check the InvMap
	ObjMap::iterator objIter = m_cInvObjs.find(uID);
	if(objIter != m_cInvObjs.end())
	{
		return m_cEmitterComps.find(uID)->second;
	}
	else
	{
		string szEffectObjName = "BubbleTrail";
		szEffectObjName += (char*)uID;

		CObject* pChikenEffect3 = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

		// Get Emitter
		CParticleEmitter* pPE3 = GetCloneEmitter("Bubble_Trail.bin");

		// Create Render Comp
		CRenderComponent* pRenComp3 = Renderer::GetInstance()->CreateRenderComp(pChikenEffect3,
			(DXMesh*)pPE3, RC_BROTH, RF_PARTICLE);

		// Create Effect Comps
		CEffectComponent* pEC = CreateEffectComp(pParent, pPE3, pRenComp3,
			EC_TYPE_CHICKEN_INV, PE_TYPE_BUBBLE_TRAIL);

		// Add the Effect Obj to the InvMap
		m_cInvObjs.insert(make_pair(uID, pChikenEffect3));

		return pEC;
	}
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
		//CreateCartSkidsComponent(pPlayer0);
		CreateBoostComponent(pPlayer0);
		CreateStunComponent(pPlayer0);
		CreateSlipComponent(pPlayer0);
		CreateSlowComponent(pPlayer0);
		CreateJamUseComponent(pPlayer0);
		CreateJamHitComponent(pPlayer0);
		//CreateDonutUseComponent(pPlayer0);
		CreatePieHitComponent(pPlayer0);

		pPlayer0->AddComponent(CVictoryVFXComp::CreateVictoryVFXComponent(pPlayer0));
		CSlipVFXComp::CreateSlipVFXComponent(pPlayer0);
	}
	if(pPlayer1)
	{
		//CreateCartCollisionComponent(pPlayer1);
		//CreateCartSkidsComponent(pPlayer1);
		CreateBoostComponent(pPlayer1);
		CreateStunComponent(pPlayer1);
		CreateSlipComponent(pPlayer1);
		CreateSlowComponent(pPlayer1);
		CreateJamUseComponent(pPlayer1);
		CreateJamHitComponent(pPlayer1);
		//CreateDonutUseComponent(pPlayer1);
		CreatePieHitComponent(pPlayer1);
		CreateCautionEffectComponent(pPlayer1);
		CSlipVFXComp::CreateSlipVFXComponent(pPlayer1);
	}
	if(pPlayer2)
	{
		//CreateCartCollisionComponent(pPlayer2);
		//CreateCartSkidsComponent(pPlayer2);
		CreateBoostComponent(pPlayer2);
		CreateStunComponent(pPlayer2);
		CreateSlipComponent(pPlayer2);
		CreateSlowComponent(pPlayer2);
		CreateJamUseComponent(pPlayer2);
		CreateJamHitComponent(pPlayer2);
		//CreateDonutUseComponent(pPlayer2);
		CreatePieHitComponent(pPlayer2);
		CreateCautionEffectComponent(pPlayer2);
		CSlipVFXComp::CreateSlipVFXComponent(pPlayer2);
	}
	if(pPlayer3)
	{
		//CreateCartCollisionComponent(pPlayer3);
		//CreateCartSkidsComponent(pPlayer3);
		CreateBoostComponent(pPlayer3);
		CreateStunComponent(pPlayer3);
		CreateSlipComponent(pPlayer3);
		CreateSlowComponent(pPlayer3);
		CreateJamUseComponent(pPlayer3);
		CreateJamHitComponent(pPlayer3);
		//CreateDonutUseComponent(pPlayer3);
		CreatePieHitComponent(pPlayer3);
		CreateCautionEffectComponent(pPlayer3);
		CSlipVFXComp::CreateSlipVFXComponent(pPlayer3);
	}
}

CEffectComponent* CEffectManager::CreateDustEffectComponent(CObject* pParentObj)
{
	// Get Singletons
	CObjectManager* pOM = CObjectManager::GetInstance();
	Renderer* pRenMan = Renderer::GetInstance();

	// Setup Effects

	// Get Emitters
	CParticleEmitter* pPE1 = GetCloneEmitter("Smoke_Puff.bin");

	// Create Render Comp
	CRenderComponent* pRenComp1 = pRenMan->CreateRenderComp(pParentObj,
		(DXMesh*)pPE1, RC_CART_SMOKE_EFFECT, RF_PARTICLE);

	// Create Effect Comps
	return CreateEffectComp(pParentObj, pPE1, pRenComp1, EC_TYPE_CART_SMOKE,
		PE_TYPE_CART_DUST);
}

// Scientist
CEffectComponent* CEffectManager::CreateScientistEffectComponent(CObject* pParentObj)
{
	// Get Singletons
	CObjectManager* pOM = CObjectManager::GetInstance();
	Renderer* pRenMan = Renderer::GetInstance();

	// Setup Effects

	// Scientists Effect
	string szEffectObjName = "CartGlowShrink";
	szEffectObjName += (char*)pParentObj->GetID();
	CObject* pEffect0 = pOM->CreateObject(szEffectObjName);
	szEffectObjName = "CartGlowGrow";
	szEffectObjName += (char*)pParentObj->GetID();
	CObject* pEffect1 = pOM->CreateObject(szEffectObjName);

	//m_pSciGlowS = pEffect0;
	//m_pSciGlowG = pEffect1;

	// Get Emitters
	CParticleEmitter* pPE0 = GetCloneEmitter("Blue_SGlow_Shrink.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Blue_SGlow_Grow.bin");

	//pPE1->SetSpawnPosition(pPE1->GetSpawnPosition() + D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	pPE0->SetOn(true);
	pPE1->SetOn(true);

	// Create Render Comp
	CRenderComponent* pRenComp0 = pRenMan->CreateRenderComp(pEffect0,
		(DXMesh*)pPE0, RC_SCI_GLOW, RF_PARTICLE);
	CRenderComponent* pRenComp1 = pRenMan->CreateRenderComp(pEffect1,
		(DXMesh*)pPE1, RC_SCI_GLOW, RF_PARTICLE);

	// Create Effect Comps
	CEffectComponent* pEC0 = CreateEffectComp(pParentObj, pPE0, pRenComp0,
		EC_TYPE_SCIENTIST_GLOW, PE_TYPE_SCI_GLOW_SHRINK);
	pEC0->AddEmitter(EC_TYPE_SCIENTIST_GLOW, pPE1, pRenComp1, pParentObj->GetTransform(), PE_TYPE_SCI_GLOW_GROW);
	pEC0->SetOnTarget(EC_TYPE_SCIENTIST_GLOW, true);

	return pEC0;
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
	CParticleEmitter* pPE0 = GetCloneEmitter("Glow_Grow.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Glow_Shrink.bin");
	CParticleEmitter* pPE2 = GetCloneEmitter("Star_Spread.bin");
	CParticleEmitter* pPE3 = GetCloneEmitter("Shine_Backdrop.bin");
	CParticleEmitter* pPE4 = GetCloneEmitter("Icon_Front.bin");

	// End
	CParticleEmitter* pPEEnd0 = GetCloneEmitter("Glow_Grow_End.bin");
	CParticleEmitter* pPEEnd1 = GetCloneEmitter("Glow_Shrink_End.bin");
	CParticleEmitter* pPEEnd2 = GetCloneEmitter("Star_Spread_End.bin");
	CParticleEmitter* pPEEnd3 = GetCloneEmitter("Shine_Backdrop_End.bin");
	CParticleEmitter* pPEEnd4 = GetCloneEmitter("Icon_Front_End.bin");

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
	ERenderContext eIconContext = (ERenderContext)m_nGoalItemIconRenderContexts[eType];
	CRenderComponent* pRenComp4 = Renderer::GetInstance()->CreateRenderComp(pEffect4,
		(DXMesh*)pPE4, eIconContext, RF_PARTICLE);
	CRenderComponent* pRenCompEnd4 = Renderer::GetInstance()->CreateRenderComp(pEffectEnd4,
		(DXMesh*)pPEEnd4, eIconContext, RF_PARTICLE);

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
	CParticleEmitter* pPE0 = GetCloneEmitter("Snowflake_Trail.bin");
	CParticleEmitter* pPE1 = GetCloneEmitter("Snowflake_Trail_2.bin");

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
	//pEC->SetOnTarget(EC_TYPE_TURKEY_TRAIL, true);
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
		//CTextureManager* pTM = CTextureManager::GetInstance();
		szTextureName[nTexNameLen] = '\0';
		string szTexFilePath = szTextureName;//"Resource\\Textures\\Particle Images\\";
		//szTexFilePath += szTextureName;
		//int  pTM->LoadTexture(szTexFilePath.c_str());
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
CParticleEmitter* CEffectManager::GetCloneEmitter(string szEmitterName)
{
	// Find the Original
	unsigned int uID = CIDGen::GetInstance()->GetID(szEmitterName);
	CParticleEmitter* pOriginalEmitter = m_cEmitterTemplates[uID];

	// Clone Emitter
	CParticleEmitter* pCloneEmitter = MMNEW(CParticleEmitter);
	*pCloneEmitter = *pOriginalEmitter;
	
	// Store Clone
	m_cClonedEffect.push_back(pCloneEmitter);

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
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM_COLLECTION, true);
	}
}

// Held Item Spawned
void CEffectManager::HeldItemSpawnedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->HeldItemSpawned((THeldItemSpawned*)pEvent->GetData());
}
void CEffectManager::HeldItemSpawned(THeldItemSpawned* pEvent)
{
	CObject* pcObj = pEvent->m_pHeldItem->GetParent();

	// Get ID
	unsigned int uID = pcObj->GetID();

	// Look for a match
	EffectCompMap::iterator compIter = m_cEmitterComps.find(uID);

	if(compIter == m_cEmitterComps.end())
	{
		// Collection
		string szEffectObjName = "HeldItem";
		szEffectObjName += (char*)uID;
		CObject* pEffect = CObjectManager::GetInstance()->CreateObject(szEffectObjName);

		// Emitter
		CParticleEmitter* pPE = GetCloneEmitter("Held_Item.bin");
		pPE->SetOn(true);

		// Create Render Comp
		CRenderComponent* pRenComp = Renderer::GetInstance()->CreateRenderComp(pEffect,
			(DXMesh*)pPE, RC_HELD_ITEM_COLLECTION_EFFECT, RF_PARTICLE);

		// Create Effect Comps
		/*CEffectComponent* pEC  =*/ CreateEffectComp(pcObj, pPE,
			pRenComp, EC_TYPE_HELD_ITEM, PE_TYPE_SALE_POPUP);
		//pEC->SetOnTarget(true);
	}
}

// Held Item Collected
void CEffectManager::HeldItemCollectedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->HeldItemCollected((THeldItemCollected*)pEvent->GetData());
}
void CEffectManager::HeldItemCollected(THeldItemCollected* /*pcObjEvent*/)
{
	
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
		pEC->ChangeContext(EC_TYPE_GOAL_ITEM, PE_TYPE_GOAL_ITEM_ICON, m_nGoalItemIconRenderContexts[eType]);
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM, true);
	}
}

// Pickup Item Despawn
void CEffectManager::PickupItemDespawnCallback(IEvent* pEvent, IComponent* pComp)
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
void CEffectManager::PickupItemCollectedCallback(IEvent* pEvent, IComponent* pComp)
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
		pEC->ChangeContext(EC_TYPE_GOAL_ITEM_COLLECTION, PE_TYPE_GOAL_ITEM_ICON, m_nGoalItemIconRenderContexts[eType]);
		
		pEC->SwitchOnOffEmitters(EC_TYPE_GOAL_ITEM_COLLECTION, true);
	}
}

// Has 8
void CEffectManager::HaveEightCallback(IEvent* pEvent, IComponent* pComp)
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
void CEffectManager::LostItemCallback(IEvent* pEvent, IComponent* pComp)
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
void CEffectManager::InvulnerableCallback(IEvent* pEvent, IComponent* pComp)
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
			pEC->SetCooldown(EC_TYPE_CART_COLLISION_CRASH, POPUP_COOLDOWN);
		}

		compIter++;
	}
}

// Donut
//void CEffectManager::DonutCallback(IEvent* pEvent, IComponent* /*pComp*/)
//{
//	// Get the Values from the Event
//	GetInstance()->Donut((TStatusEffectEvent*)pEvent->GetData());
//}

//void CEffectManager::Donut(TStatusEffectEvent* pcObjEvent)
//{
//	// Comps
//	//EffectCompMap::iterator compIter;
//	//compIter = m_cEmitterComps.begin();
//	//while(compIter != m_cEmitterComps.end())
//	//{
//	//	CEffectComponent* pEC = compIter->second;
//	//	if(pcObjEvent->m_pObject == pEC->GetParent())
//	//	{
//	//		pEC->SwitchOnOffEmitters(EC_TYPE_DONUT_USE, true);
//	//		pEC->SetLifespan(EC_TYPE_DONUT_USE, DONUT_PUFF_LIFESPAN);
//	//	}
//	//	compIter++;
//	//}
//}

// Steal
void CEffectManager::StealCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->Steal(pEvent);
}
void CEffectManager::Steal(IEvent* pEvent)
{
	// If its the player
	//unsigned int nPlayerID = CObjectManager::GetInstance()->GetObjectByName("Player0")->GetID();
	//if(pcObjEvent->m_pObject->GetID() == nPlayerID)
	//{
	//	m_cBoostEffect.SetOn(true);
	//}

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
			pEC->SwitchOnOffEmitters(EC_TYPE_BOOST, true);
		}
		compIter++;
	}

	// OLD WAY
	//pBoostEmitter->SwitchOnOffEmitters(EC_TYPE_BOOST, true);
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
			pEC->SwitchOnOffEmitters(EC_TYPE_TURKEY_STUN, true);
			pEC->SetCooldown(EC_TYPE_TURKEY_STUN, POPUP_COOLDOWN);
		}
		compIter++;
	}
}

// Turkey
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
		pEC->SwitchOnOffEmitters(EC_TYPE_TURKEY_TRAIL, true);
	}
}

// Turkey Despawn
void CEffectManager::TurkeyDespawnCallback(IEvent* pEvent, IComponent* pComp)
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
			pEC->SwitchOnOffEmitters(EC_TYPE_PEANUT_SQUISH, true);
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
	//CTextureManager* pTM = CTextureManager::GetInstance();
	//CSpriteComponent* pJamSprite = pTM->
	
	// Get Player Obj
	unsigned int nPlayerID = CIDGen::GetInstance()->GetID("Player0");
	unsigned int nPlayer1 = CIDGen::GetInstance()->GetID("Player1");
	unsigned int nPlayer2 = CIDGen::GetInstance()->GetID("Player2");
	unsigned int nPlayer3 = CIDGen::GetInstance()->GetID("Player3");
	if(pcObjEvent->m_pObject->GetID() != nPlayerID)
	{
		// Put jam effect on screen for player
		if(CMovementManager::GetInstance()->PlayerIsInvincible(
			CObjectManager::GetInstance()->GetObjectByName("Player0")) == false)
		{
			m_cJamEffect.SetOn(true);
			CWwiseSoundManager::GetInstance()->PlayTheSound(STATUS_RADARJAM, BIKER_ID);
		}
	}

	// Find the Component Associated with that Obj
	EffectCompMap::iterator compIter;
	unsigned iterID;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		CEffectComponent* pEC = compIter->second;
		if(pcObjEvent->m_pObject == pEC->GetParent())
		{
			// Show jam firing from user
			pEC->SwitchOnOffEmitters(EC_TYPE_JAM_USE, true);
		}
		else if( pEC->GetParent()->GetID() != nPlayerID)
		{
			// Show jam flying down towards opponents
			iterID = pEC->GetParent()->GetID();
			if( (iterID == nPlayer1) || (iterID == nPlayer2) || (iterID == nPlayer3) )
			{
				if(CMovementManager::GetInstance()->PlayerIsInvincible(pEC->GetParent()) == false)
				{
					pEC->SwitchOnOffEmitters(EC_TYPE_JAM_HIT_FALL, true);
					pEC->SetEffectTimer(EC_TYPE_JAM_HIT_POPUP, JAM_HIT_POPUP_TIMER);
				}
			}
		}

		compIter++;
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
	/*if(fSkidTimer > 0.0f)
	{
		fSkidTimer -= fDT;

		if(fSkidTimer <= 0.0f)
		{
			pSkidEmitter->SwitchOnOffEmitters(false);
			pDustEmitter->SwitchOnOffEmitters(true);
		}
	}*/

	// Jam
	//m_cJamEffect.bActive = true;
	//m_cJamEffect.SetOn(true);
	if(m_cJamEffect.IsActive())
		m_cJamEffect.Update(fDT);

	/*if(m_cJamEffect.fJamCounter > 0.0f)
	{
		m_cJamEffect.fJamCounter -= fDT;

		if(m_cJamEffect.fJamCounter <= 0.0f)
		{
			m_cJamEffect.pJamScreenSprite->SetActive(false);
		}
	}*/

	if(m_cBlindEffect.IsActive())
		m_cBlindEffect.Update(fDT);

	if(m_cStealEffect.IsActive())
		m_cStealEffect.Update(fDT);

	if(m_cBoostEffect.IsActive())
		m_cBoostEffect.Update(fDT);

	/*if(fBlindCounter > 0.0f)
	{
		fBlindCounter -= fDT;

		TSpriteData tData = pBlindSprite->GetSpriteData();
		tData.m_nY += 4.0f;
		pBlindSprite->SetSpriteData(tData);

		if(fBlindCounter <= 0.0f)
		{
			pBlindSprite->SetActive(false);
			tData.m_nY = 0;
			pBlindSprite->SetSpriteData(tData);
		}
	}*/
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

	// Clones
	list<CParticleEmitter*, CAllocator<CParticleEmitter*>>::iterator cloneIter;
	cloneIter = m_cClonedEffect.begin();
	while(cloneIter != m_cClonedEffect.end())
	{
		MMDEL(*cloneIter);
		cloneIter++;
	}

	// Comps
	EffectCompMap::iterator compIter;
	compIter = m_cEmitterComps.begin();
	while(compIter != m_cEmitterComps.end())
	{
		MMDEL(compIter->second);
		compIter++;
	}
}

// Reset
void CEffectManager::DisableCallback(IEvent*, IComponent*)
{
	//GetInstance()->Shutdown();
	//GetInstance()->InitObjects();
	GetInstance()->m_cJamEffect.SetSpritesActive(false);
	GetInstance()->m_cBlindEffect.SetSpritesActive(false);
	GetInstance()->m_cStealEffect.SetSpritesActive(false);
	GetInstance()->m_cBoostEffect.SetSpritesActive(false);
}
void CEffectManager::EnableCallback(IEvent*, IComponent*)
{
	//GetInstance()->Shutdown();
	//GetInstance()->InitObjects();
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
		ObjMap::iterator pIter = pEM->m_cInvObjs.find(compIter->first);
		if(pIter != pEM->m_cInvObjs.end())
		{
			CObjectManager::GetInstance()->DestroyObject(pIter->second);
			CEventManager::GetInstance()->UnregisterEventAll(compIter->second);
			pEM->m_cInvObjs.erase(pIter);
		}
		pEM->m_cEmitterComps.erase(compIter);
	}

}

#undef READ