////////////////////////////////////////////////////////////////////////////////
//	File			:	CHUDEffectsComp.cpp
//	Date			:	7/27/11
//	Mod. Date		:	7/27/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the HUD Item Effect
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CHUDEffectsComp.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\Texture Managers\\CHUDManager.h"
#include "..\\..\\Managers\\Component Managers\\CSpawningManager.h"
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"

// Defines
#define HUDE_GIC_DURATION	     (0.5f)
#define HUDE_GIC_SLI_GROW_RATE   (2.5f) // 1.0
#define HUDE_GIC_SHINE_GROW_RATE (0.65f) // 0.25
#define HUDE_GIC_SHINE_ROT_RATE  (3.14f)

#define HUDE_PID_DURATION	     (1.0f)

#define HUDE_PIC_DURATION	     (0.5f)
#define HUDE_PIC_SLI_GROW_RATE   (2.5f) // 1.0
#define HUDE_PIC_SHINE_GROW_RATE (0.65f) // 0.25
#define HUDE_PIC_SHINE_ROT_RATE  (3.14f)

#define HUDE_HIS_DURATION	     (1.0f)

#define HUDE_HIC_DURATION	     (1.0f)
#define HUDE_HIC_NUM_LOOPS		 (10.0f)

#define HUDE_HIU_DURATION	     (0.25f)
#define HUDE_HIU_INV_SHRINK_RATE (3.0f)
#define HUDE_HIU_INV_MOVE_RATE	 (200.0f)

// Initalize
void CHUDEffectsComp::Init(void)
{
	// Get Singletons
	static CEventManager* pEM = CEventManager::GetInstance();

	// Register for Events
	
	// Update Gameplay
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	// Shutdown Gameplay
	szEventName = "ShutdownObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, ShutdownGameplayCallback);

	// Shutdown 
	szEventName = "Shutdown";
	pEM->RegisterEvent(szEventName, this, ShutdownCallback);

	// Goal Item Collected
	pEM->RegisterEvent("GoalItemCollected",	this, GoalItemCollectedCallback);

	// Pickup Item Dropped
	pEM->RegisterEvent("ItemDropped", this, PickupItemDroppedCallback);

	// Pickup Item Collected
	pEM->RegisterEvent("PickupItemCollected", this, PickupItemCollectedCallback);

	// Held Item Stolen
	pEM->RegisterEvent("HeldItemStolenEffect", this, HeldItemStolenCallback);

	// Held Item Collected
	pEM->RegisterEvent("HeldItemCollectedEffect", this, HeldItemCollectedCallback);

	// Held Item Used
	pEM->RegisterEvent("HeldItemUsedEffect", this, HeldItemUsedCallback);

	// Reset
	szEventName = "EnableObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, EnableCallback);
	szEventName = "DisableObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, DisableCallback);
}

// Helper Funcs
bool CHUDEffectsComp::CheckForDupe(EGoalItemType eGIType)
{
	// Check for an Effect on this Icon
	HUDEffectList::iterator vfxIter = m_cActiveEffects.begin();
	while(vfxIter != m_cActiveEffects.end())
	{
		THUDEffect* ptEffect = *vfxIter;

		if(ptEffect->m_eGIType == eGIType)
			return true;

		vfxIter++;
	}

	return false;
}
RECT THUDEffect::CellAlgo(int nID, int nNumCols,
						  int nCellWidth, int nCellHeight)
{
	RECT rCell;
	
	// pick out the proper frame from the image
	rCell.left = (nID % nNumCols) * nCellWidth;
	rCell.top = (nID / nNumCols) * nCellHeight;
	rCell.right = rCell.left + nCellWidth;
	rCell.bottom = rCell.top + nCellHeight;

	return rCell;
}

THUDEffect* CHUDEffectsComp::GetHUDEffect(EGoalItemType eGIType, EHUDEffectType eHUDEType)
{
	THUDEffect* ptEffect = NULL;

	// Check if we are already modifiling that item
	if(CheckForDupe(eGIType))
		return ptEffect;

	// Can we Recycle
	if(m_cDeadEffects.size() > 0)
	{
		ptEffect = m_cDeadEffects.front();
		m_cDeadEffects.pop_front();
		ptEffect->Reset(eGIType, eHUDEType);
	}
	else
	{
		ptEffect = MMNEW(THUDEffect);
		ptEffect->Init(eGIType, eHUDEType);
	}

	// Set as Active
	m_cActiveEffects.push_back(ptEffect);

	return ptEffect;
}
THUDEffect* CHUDEffectsComp::GetInvHUDEffect(CSpriteComponent* pSpriteComp, EHUDEffectType eHUDEType)
{
	THUDEffect* ptEffect = NULL;

	// Can we Recycle
	if(m_cDeadEffects.size() > 0)
	{
		ptEffect = m_cDeadEffects.front();
		m_cDeadEffects.pop_front();
		ptEffect->ResetInv(pSpriteComp, eHUDEType);
	}
	else
	{
		ptEffect = MMNEW(THUDEffect);
		ptEffect->InitInv(pSpriteComp, eHUDEType);
	}

	// Set as Active
	m_cActiveEffects.push_back(ptEffect);

	return ptEffect;
}

TSpriteData THUDEffect::GetInitSpriteData(EGoalItemType eGIType, EHUDEffectType eHUDEType)
{
	// Get Singletons
	static CHUDManager* pHUDMan = CHUDManager::GetInstance();
	static CTextureManager* pTM = CTextureManager::GetInstance();

	// Get Sprite Data of the Shopping List Item
	m_pSLISpriteComp = pHUDMan->GetListIconComponent(eGIType);
	TSpriteData pData = m_pSLISpriteComp->GetSpriteData();
	m_tOriginalSLIData = pData;
	m_fScale = pData.m_fScaleX;

	switch(eHUDEType)
	{
	case HUDE_GOAL_COLLECT:
		pData.m_nTexture = pTM->LoadTexture("Resource\\Textures\\Particle Images\\FFP_2D_Shine_FIN.png");
		pData.m_nX -= 18;
		pData.m_nY -= 20;
		pData.m_fScaleX = 0.17f;
		pData.m_fScaleY = 0.17f;
		pData.m_nZ = 99;
		pData.m_fRotCenterX = 256.0f;
		pData.m_fRotCenterY = 256.0f;
		pData.m_tRect.bottom = 0;
		pData.m_tRect.left   = 0;
		pData.m_tRect.right  = 0;
		pData.m_tRect.top	 = 0;
		break;
	case HUDE_PICKUP_DROP:
		pData.m_nTexture = pTM->LoadTexture("Resource\\Textures\\Particle Images\\FFP_2D_BurstSheetWhite_FIN.png");
		pData.m_nX -= 24;
		pData.m_nY -= 20;
		pData.m_fScaleX = 0.8f;
		pData.m_fScaleY = 0.8f;
		pData.m_nZ = 100;
		pData.m_tRect = CellAlgo(0, 4, 128, 128);
		break;
	case HUDE_PICKUP_COLLECT:
		pData.m_nTexture = pTM->LoadTexture("Resource\\Textures\\Donut Steal Sprites\\FFP_2D_Shine2_FIN.png");
		pData.m_nX -= 18;
		pData.m_nY -= 20;
		pData.m_fScaleX = 0.17f;
		pData.m_fScaleY = 0.17f;
		pData.m_nZ = 99;
		pData.m_fRotCenterX = 256.0f;
		pData.m_fRotCenterY = 256.0f;
		pData.m_tRect.bottom = 0;
		pData.m_tRect.left   = 0;
		pData.m_tRect.right  = 0;
		pData.m_tRect.top	 = 0;
		break;
	};

	return pData;
}
TSpriteData THUDEffect::GetInitInvSpriteData(CSpriteComponent* pSpriteComp, EHUDEffectType eHUDEType)
{
	// Get Singletons
	static CTextureManager* pTM = CTextureManager::GetInstance();

	TSpriteData pData = pSpriteComp->GetSpriteData();

	switch(eHUDEType)
	{
	case HUDE_HELD_STOLEN:
		pData.m_nTexture = pTM->LoadTexture("Resource\\Textures\\Particle Images\\FFP_2D_BurstSheetWhite_FIN.png");
		pData.m_nX -= 35;
		pData.m_nY -= 30;
		pData.m_fScaleX = 1.5f;
		pData.m_fScaleY = 1.5f;
		pData.m_nZ = 100;
		pData.m_tRect = CellAlgo(0, 4, 128, 128);
		break;
	case HUDE_HELD_COLLECT:
		pData.m_nTexture = pTM->LoadTexture("Resource\\Textures\\Particle Images\\FFP_2D_Sparkles_FIN.png");
		pData.m_nX += 20;
		//pData.m_nY -= 30;
		pData.m_fScaleX = 1.5f;
		pData.m_fScaleY = 1.5f;
		pData.m_nZ = 100;
		pData.m_tRect = CellAlgo(0, 4, 64, 64);
		m_bGrowing = true;
		break;
	};

	return pData;
}

// Initalize Effect
void THUDEffect::Init(EGoalItemType eGIType, EHUDEffectType eHUDEType)
{
	// Get Singletons
	static CObjectManager*  pOM = CObjectManager::GetInstance();
	static CTextureManager* pTM = CTextureManager::GetInstance();

	// Keep a counter of how many we made
	static int snCount = 0;

	// Create Objects
	string szObjName = "HUDEffect";
	szObjName += (char)snCount;
	CObject* pHUDEffectObj = pOM->CreateObject(szObjName);
	m_pEffectSpriteComp = pTM->CreateSpriteComp(pHUDEffectObj,
		GetInitSpriteData(eGIType, eHUDEType), true);

	// Set On and Type
	m_bActive = true;
	m_eType = eHUDEType;
	m_eGIType = eGIType;
}
void THUDEffect::InitInv(CSpriteComponent* pSpriteComp, EHUDEffectType eHUDEType)
{
	// Get Singletons
	static CObjectManager*  pOM = CObjectManager::GetInstance();
	static CTextureManager* pTM = CTextureManager::GetInstance();

	// Keep a counter of how many we made
	static int snCount = 0;

	// Create Objects
	string szObjName = "HUDStolenEffect";
	szObjName += (char)snCount;
	CObject* pHUDEffectObj = pOM->CreateObject(szObjName);
	m_pEffectSpriteComp = pTM->CreateSpriteComp(pHUDEffectObj,
		GetInitInvSpriteData(pSpriteComp, eHUDEType), true);

	// Set On and Type
	m_bActive = true;
	m_eType = eHUDEType;
	m_eGIType = NO_ITEM;
	m_pSLISpriteComp = NULL;
}


// Reset
void THUDEffect::Reset(EGoalItemType eGIType, EHUDEffectType eHUDEType)
{
	// Reset Sprite Data
	TSpriteData pData = GetInitSpriteData(eGIType, eHUDEType);
	m_pEffectSpriteComp->SetSpriteData(pData);
	m_pEffectSpriteComp->SetActive(true);

	// Set On and Type
	m_bActive = true;
	m_eType = eHUDEType;
	m_eGIType = eGIType;
}
void THUDEffect::ResetInv(CSpriteComponent* pSpriteComp, EHUDEffectType eHUDEType)
{
	// Reset Sprite Data
	TSpriteData pData = GetInitInvSpriteData(pSpriteComp, eHUDEType);
	m_pEffectSpriteComp->SetSpriteData(pData);
	m_pEffectSpriteComp->SetActive(true);

	// Set On and Type
	m_bActive = true;
	m_eType = eHUDEType;
	m_eGIType = NO_ITEM;
	m_pSLISpriteComp = NULL;
}

// Update Gameplay
void CHUDEffectsComp::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Update Component
	pHUDE->Update(fDT);
}
void CHUDEffectsComp::Update(float fDT)
{
	// Get Singletons
	static CHUDManager* pHUD = CHUDManager::GetInstance();

	// Update all Active Effects
	HUDEffectList::iterator vfxIter = m_cActiveEffects.begin();
	while(vfxIter != m_cActiveEffects.end())
	{
		// Get Effect
		THUDEffect* ptEffect = (*vfxIter);
		ptEffect->Update(fDT);

		// Check for Dead
		if(ptEffect->m_bActive == false)
		{
			m_cDeadEffects.push_back(ptEffect);
			vfxIter = m_cActiveEffects.erase(vfxIter);
		}
		else
		{
			// Render it
			vfxIter++;
		}
	}

	// Update HUD
	pHUD->SetUpdateSpriteData(true);
}
// Update Effects
void THUDEffect::Update(float fDT)
{
	// Decrease Duration
	m_fDuration -= fDT;

	// Update Effect
	switch(m_eType)
	{
	case HUDE_GOAL_COLLECT:
		UpdateGICEffect(fDT);
		break;
	case HUDE_PICKUP_DROP:
		UpdatePIDEffect(fDT);
		break;
	case HUDE_PICKUP_COLLECT:
		UpdatePICEffect(fDT);
		break;
	case HUDE_HELD_STOLEN:
		UpdatePIDEffect(fDT);
		break;
	case HUDE_HELD_COLLECT:
		UpdateHICEffect(fDT);
		break;
	case HUDE_HELD_USE:
		UpdateHIUEffect(fDT);
		break;
	};

	// Kill Effct
	if(m_fDuration <= 0.0f)
	{
		m_nAnimFrame = 0;
		m_fDuration = 0.0f;
		m_bActive = false;
		m_bGrowing = false;
		m_pEffectSpriteComp->SetActive(false);

		if(m_pSLISpriteComp)
			m_pSLISpriteComp->SetSpriteData(m_tOriginalSLIData);
	}
}
void THUDEffect::UpdateGICEffect(float fDT)
{
	// Get Current Sprite Data
	TSpriteData pEffectData = m_pEffectSpriteComp->GetSpriteData();
	TSpriteData pSLIData = m_pSLISpriteComp->GetSpriteData();

	// Growing
	if(m_bGrowing)
	{
		m_fScale += HUDE_GIC_SLI_GROW_RATE * fDT;

		pEffectData.m_fScaleX += HUDE_GIC_SHINE_GROW_RATE * fDT;
		pEffectData.m_fScaleY += HUDE_GIC_SHINE_GROW_RATE * fDT;
		pEffectData.m_fRot += HUDE_GIC_SHINE_ROT_RATE * fDT;
		
		pSLIData.m_fScaleX = m_fScale;
		pSLIData.m_fScaleY = m_fScale;
	}
	else // Shrinking
	{
		m_fScale -= HUDE_GIC_SLI_GROW_RATE * fDT;

		pEffectData.m_fScaleX -= HUDE_GIC_SHINE_GROW_RATE * fDT;
		pEffectData.m_fScaleY -= HUDE_GIC_SHINE_GROW_RATE * fDT;
		pEffectData.m_fRot -= HUDE_GIC_SHINE_ROT_RATE * fDT;
			
		pSLIData.m_fScaleX = m_fScale;
		pSLIData.m_fScaleY = m_fScale;
	}

	// Switch
	if(m_fDuration < HUDE_GIC_DURATION / 2 && m_bGrowing)
	{
		m_bGrowing = false;
	}

	// Position
	pSLIData.m_nZ = 100;
	pSLIData.m_nX = pSLIData.m_nX;
	pSLIData.m_nY = pSLIData.m_nY;

	// Set Sprite Data
	m_pEffectSpriteComp->SetSpriteData(pEffectData);
	m_pSLISpriteComp->SetSpriteData(pSLIData);
}
void THUDEffect::UpdatePIDEffect(float fDT)
{
	// Get Current Sprite Data
	TSpriteData pEffectData = m_pEffectSpriteComp->GetSpriteData();

	// Increament Animation Counter
	m_fAnimCounter += fDT;

	// Get to the next Frame
	if(m_fAnimCounter > HUDE_PID_DURATION / 16)
	{
		++m_nAnimFrame;

		m_fAnimCounter = 0.0f;
	}

	// Set the RECT and Sprite Data
	pEffectData.m_tRect = CellAlgo(m_nAnimFrame, 4, 128, 128);
	m_pEffectSpriteComp->SetSpriteData(pEffectData);
}
void THUDEffect::UpdatePICEffect(float fDT)
{
	// Get Current Sprite Data
	TSpriteData pEffectData = m_pEffectSpriteComp->GetSpriteData();
	TSpriteData pSLIData = m_pSLISpriteComp->GetSpriteData();

	// Growing
	if(m_bGrowing)
	{
		m_fScale += HUDE_PIC_SLI_GROW_RATE * fDT;

		pEffectData.m_fScaleX += HUDE_PIC_SHINE_GROW_RATE * fDT;
		pEffectData.m_fScaleY += HUDE_PIC_SHINE_GROW_RATE * fDT;
		pEffectData.m_fRot += HUDE_PIC_SHINE_ROT_RATE * fDT;

		pSLIData.m_fScaleX = m_fScale;
		pSLIData.m_fScaleY = m_fScale;
	}
	else // Shrinking
	{
		m_fScale -= HUDE_PIC_SLI_GROW_RATE * fDT;

		pEffectData.m_fScaleX -= HUDE_PIC_SHINE_GROW_RATE * fDT;
		pEffectData.m_fScaleY -= HUDE_PIC_SHINE_GROW_RATE * fDT;
		pEffectData.m_fRot -= HUDE_PIC_SHINE_ROT_RATE * fDT;

		pSLIData.m_fScaleX = m_fScale;
		pSLIData.m_fScaleY = m_fScale;
	}

	// Switch
	if(m_fDuration < HUDE_PIC_DURATION / 2 && m_bGrowing)
	{
		m_bGrowing = false;
	}

	// Position
	pSLIData.m_nZ = 100;

	// Set Sprite Data
	m_pEffectSpriteComp->SetSpriteData(pEffectData);
	m_pSLISpriteComp->SetSpriteData(pSLIData);
}
void THUDEffect::UpdateHICEffect(float fDT)
{
	// Get Current Sprite Data
	TSpriteData pEffectData = m_pEffectSpriteComp->GetSpriteData();

	// Increament Animation Counter
	m_fAnimCounter += fDT;

	// Get to the next Frame
	if(m_bGrowing)
	{
		if(m_fAnimCounter > HUDE_HIC_DURATION / (8 * HUDE_HIC_NUM_LOOPS * 2))
		{
			++m_nAnimFrame;

			if(m_nAnimFrame == 5)
			{
				m_nAnimFrame = 4;
				m_bGrowing = false;
			}

			m_fAnimCounter = 0.0f;
		}
	}
	else // Reverse
	{
		if(m_fAnimCounter > HUDE_HIC_DURATION / (8 * HUDE_HIC_NUM_LOOPS * 2))
		{
			--m_nAnimFrame;

			if(m_nAnimFrame == -1)
			{
				m_nAnimFrame = 0;
				m_bGrowing = true;
			}

			m_fAnimCounter = 0.0f;
		}
	}

	// Set the RECT and Sprite Data
	pEffectData.m_tRect = CellAlgo(m_nAnimFrame, 4, 64, 64);
	m_pEffectSpriteComp->SetSpriteData(pEffectData);
}
void THUDEffect::UpdateHIUEffect(float fDT)
{
	// Get Current Sprite Data
	TSpriteData pEffectData = m_pEffectSpriteComp->GetSpriteData();

	// Move and Scale
	pEffectData.m_nX += int(HUDE_HIU_INV_MOVE_RATE * fDT);
	pEffectData.m_nY += int(HUDE_HIU_INV_MOVE_RATE * fDT);
	pEffectData.m_fScaleX -= HUDE_HIU_INV_SHRINK_RATE * fDT;
	pEffectData.m_fScaleY -= HUDE_HIU_INV_SHRINK_RATE * fDT;

	// Set Sprite Data
	m_pEffectSpriteComp->SetSpriteData(pEffectData);
}

// Goal Item Collected
void CHUDEffectsComp::GoalItemCollectedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;
	TGoalItemCollectedEvent* pData = (TGoalItemCollectedEvent*)pEvent->GetData();

	if(pHUDE->m_pParentObj == pData->m_pcCollector)
	{
		pHUDE->GoalItemCollected(pData);
	}
}
void CHUDEffectsComp::GoalItemCollected(TGoalItemCollectedEvent* ptGICEvent)
{
	// Get Singletons
	static CSpawningManager* pSM = CSpawningManager::GetInstance();

	// Find the Type
	EGoalItemType eGIType = pSM->GetGoalItemType(ptGICEvent->m_pcGoalItem);

	// Get an Effect
	THUDEffect* pEffect = GetHUDEffect(eGIType, HUDE_GOAL_COLLECT);

	if(pEffect == NULL)
		return;

	// Set Init Values
	pEffect->m_bGrowing = true;
	pEffect->m_fDuration = HUDE_GIC_DURATION;
}

// Pickup Item Dropped
void CHUDEffectsComp::PickupItemDroppedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;
	TGoalItemCollectedEvent* pData = (TGoalItemCollectedEvent*)pEvent->GetData();

	// Player Check
	if(pHUDE->m_pParentObj == pData->m_pcCollector)
	{
		pHUDE->PickupItemDropped(pData);
	}
}
void CHUDEffectsComp::PickupItemDropped(TGoalItemCollectedEvent* ptGICEvent)
{
	// Get Singletons
	static CSpawningManager* pSM = CSpawningManager::GetInstance();

	// Find the Type
	EGoalItemType eGIType = (EGoalItemType)((int)ptGICEvent->m_pcGoalItem);

	// Get an Effect
	THUDEffect* pEffect = GetHUDEffect(eGIType, HUDE_PICKUP_DROP);

	if(pEffect == NULL)
		return;

	// Set Init Data
	pEffect->m_fDuration = HUDE_PID_DURATION;
}

// Pickup Item Collected
void CHUDEffectsComp::PickupItemCollectedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;
	TPickupItemCollectedEvent* pData = (TPickupItemCollectedEvent*)pEvent->GetData();

	if(pHUDE->m_pParentObj == pData->m_pcCollector)
	{
		pHUDE->PickupItemCollected(pData);
	}
}
void CHUDEffectsComp::PickupItemCollected(TPickupItemCollectedEvent* ptPICEvent)
{
	// Get an Effect
	THUDEffect* pEffect = GetHUDEffect(ptPICEvent->m_eItemType, HUDE_PICKUP_COLLECT);

	if(pEffect == NULL)
		return;

	// Set Init Data
	pEffect->m_bGrowing = true;
	pEffect->m_fDuration = HUDE_PIC_DURATION;
}

// Held Item Stolen
void CHUDEffectsComp::HeldItemStolenCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;
	CSpriteComponent* pData = (CSpriteComponent*)pEvent->GetSender();

	pHUDE->HeldItemStolen(pData);
}
void CHUDEffectsComp::HeldItemStolen(CSpriteComponent* pSpriteComp)
{
	// Get an Effect
	THUDEffect* pEffect = GetInvHUDEffect(pSpriteComp, HUDE_HELD_STOLEN);

	if(pEffect == NULL)
		return;

	// Set Init Data
	pEffect->m_fDuration = HUDE_HIS_DURATION;
}

// Held Item Collected
void CHUDEffectsComp::HeldItemCollectedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;
	CSpriteComponent* pData = (CSpriteComponent*)pEvent->GetSender();

	// Get an Effect
	THUDEffect* pEffect = pHUDE->GetInvHUDEffect(pData, HUDE_HELD_COLLECT);

	if(pEffect == NULL)
		return;

	pEffect->m_fDuration = HUDE_HIC_DURATION;
}

// Held Item Used
void CHUDEffectsComp::HeldItemUsedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;
	CSpriteComponent* pData = (CSpriteComponent*)pEvent->GetSender();

	// Turn off Sprite
	pData->SetActive(false);

	// Get an Effect
	THUDEffect* pEffect = pHUDE->GetInvHUDEffect(pData, HUDE_HELD_USE);

	if(pEffect == NULL)
		return;

	pEffect->m_fDuration = HUDE_HIU_DURATION;
}

// Disable
void CHUDEffectsComp::DisableCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;

	// Turn off all Active Effects
	HUDEffectList::iterator vfxIter = pHUDE->m_cActiveEffects.begin();
	while(vfxIter != pHUDE->m_cActiveEffects.end())
	{
		THUDEffect* ptEffect = (*vfxIter);

		ptEffect->m_pEffectSpriteComp->SetActive(false);

		vfxIter++;
	}
}

// Enable
void CHUDEffectsComp::EnableCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;

	// Turn on all Active Effects
	HUDEffectList::iterator vfxIter = pHUDE->m_cActiveEffects.begin();
	while(vfxIter != pHUDE->m_cActiveEffects.end())
	{
		THUDEffect* ptEffect = (*vfxIter);

		ptEffect->m_pEffectSpriteComp->SetActive(true);

		vfxIter++;
	}
}

// Shutdown
void CHUDEffectsComp::ShutdownCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;

	// Unregister
	CEventManager::GetInstance()->UnregisterEventAll(pComp);

	// Clean Active
	HUDEffectList::iterator vfxIter = pHUDE->m_cActiveEffects.begin();
	while(vfxIter != pHUDE->m_cActiveEffects.end())
	{
		THUDEffect* ptEffect = (*vfxIter);

		MMDEL(ptEffect);

		vfxIter = pHUDE->m_cActiveEffects.erase(vfxIter);
	}

	// Clean Active
	vfxIter = pHUDE->m_cDeadEffects.begin();
	while(vfxIter != pHUDE->m_cDeadEffects.end())
	{
		THUDEffect* ptEffect = (*vfxIter);

		MMDEL(ptEffect);

		vfxIter = pHUDE->m_cDeadEffects.erase(vfxIter);
	}
}
void CHUDEffectsComp::ShutdownGameplayCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CHUDEffectsComp* pHUDE = (CHUDEffectsComp*)pComp;

	// Clean Active
	HUDEffectList::iterator vfxIter = pHUDE->m_cActiveEffects.begin();
	while(vfxIter != pHUDE->m_cActiveEffects.end())
	{
		THUDEffect* ptEffect = (*vfxIter);

		pHUDE->m_cDeadEffects.push_back(ptEffect);
		vfxIter = pHUDE->m_cActiveEffects.erase(vfxIter);
	}
}