///////////////////////////////////////////////////////////////////////////////
// BLIND EFFECT
///////////////////////////////////////////////////////////////////////////////

// Includes
#include "CBlindSpriteEffect.h"
#include "Texture Managers\CTextureManager.h"
#include "..\\Object Manager\\CObjectManager.h"
#include "..\\Event Manager\\CEventManager.h"

// Defines
//#define NULL (0)
#define BLIND_ANIM_SWITCH_TIME (0.25f)
#define BLIND_END_TIME		   (7.0f)
#define BLIND_DROP_RATE		   (20.0f)
#define BLIND_ALPHA_RATE	   (0.35f)

// Constructor
CBlindSpriteEffect::CBlindSpriteEffect(void) : m_pCreamSpriteComp(NULL),
											   m_pSplatSpriteComp(NULL),
											   m_pPinkStarsSpriteComp(NULL),
											   m_pSplaterSpriteComp(NULL),
											   m_fCounter(0.0f),
											   m_fCreamAlpha(1.0f),
											   m_fSplatAlpha(1.0f),
											   m_fStarsAlpha(1.0f),
											   m_nEffectStage(0),
											   m_bActive(false)
{
	// Get Singletons
	CEventManager* pEM = CEventManager::GetInstance();

	// Init Gameplay
	string szEventName = "InitObject";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, GameplayInitCallback);
}

///////////////////////////////////////////////////////////////////////////////
// TEMPLATES
///////////////////////////////////////////////////////////////////////////////

// Cream
TSpriteData CBlindSpriteEffect::GetCreamInitData(void)
{
	CTextureManager* pTM = CTextureManager::GetInstance();
	int texID = pTM->LoadTexture("Resource\\Textures\\Pie Sprites\\FFP_2D_Splat_FIN.png");
	TSpriteData tData;
	tData.m_nTexture = texID;
	tData.m_nX = 100;
	tData.m_nY = 0;
	tData.m_nZ = 101;
	tData.m_fScaleX = 1.5f;
	tData.m_fScaleY = 1.5f;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 512;
	tData.m_tRect.bottom = 512;
	return tData;
}

// Splat
TSpriteData CBlindSpriteEffect::GetSplatInitData(void)
{
	CTextureManager* pTM = CTextureManager::GetInstance();
	int texID = pTM->LoadTexture("Resource\\Textures\\Pie Sprites\\FFP_2D_SplatWord_FIN.png");
	TSpriteData tData;
	tData.m_nTexture = texID;
	tData.m_nX = 100;
	tData.m_nY = 0;
	tData.m_nZ = 102;
	tData.m_fScaleX = 1.5f;
	tData.m_fScaleY = 1.5f;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 512;
	tData.m_tRect.bottom = 512;
	return tData;
}

// Pink Stars
TSpriteData CBlindSpriteEffect::GetPinkStarsInitData(void)
{
	CTextureManager* pTM = CTextureManager::GetInstance();
	int texID = pTM->LoadTexture("Resource\\Textures\\Pie Sprites\\FFP_2D_LittleSplat_FIN.png");
	TSpriteData tData;
	tData.m_nTexture = texID;
	tData.m_nX = 100;
	tData.m_nY = 0;
	tData.m_nZ = 103;
	tData.m_fScaleX = 1.5f;
	tData.m_fScaleY = 1.5f;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 512;
	tData.m_tRect.bottom = 512;
	return tData;
}

// Splater
TSpriteData CBlindSpriteEffect::GetSplaterInitData(void)
{
	CTextureManager* pTM = CTextureManager::GetInstance();
	int texID = pTM->LoadTexture("Resource\\Textures\\Pie Sprites\\FFP_2D_Splater_FIN.png");
	TSpriteData tData;
	tData.m_nTexture = texID;
	tData.m_nX = 100;
	tData.m_nY = 0;
	tData.m_nZ = 100;
	tData.m_fScaleX = 1.5f;
	tData.m_fScaleY = 1.5f;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 512;
	tData.m_tRect.bottom = 512;
	return tData;
}


// Create SpriteComps
void CBlindSpriteEffect::CreateSpriteComps(void)
{
	// Keep a counter of how many we made
	static int nCount = 0;

	// Create Objects
	string szObjName = "BlindCreamSprite";
	szObjName += (char)nCount;
	CObject* pCreamObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	m_pCreamSpriteComp = CTextureManager::GetInstance()->CreateSpriteComp(pCreamObj, GetCreamInitData(), false);

	szObjName = "BlindSplatSprite";
	szObjName += (char)nCount;
	CObject* pSplatObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	m_pSplatSpriteComp = CTextureManager::GetInstance()->CreateSpriteComp(pSplatObj, GetSplatInitData(), false);

	szObjName = "BlindSplater";
	szObjName += (char)nCount;
	CObject* pSplaterObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	m_pSplaterSpriteComp = CTextureManager::GetInstance()->CreateSpriteComp(pSplaterObj, GetSplaterInitData(), false);

	szObjName = "BlindPinkStarsSprite";
	szObjName += (char)nCount;
	CObject* pStarsObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	m_pPinkStarsSpriteComp = CTextureManager::GetInstance()->CreateSpriteComp(pStarsObj, GetPinkStarsInitData(), false);
}

// Reset Sprites
void CBlindSpriteEffect::ResetSprites(void)
{
	// Reset Values
	m_fCounter = 0.0f;
	m_nEffectStage = 0;
	m_fCreamAlpha = 1.0f;
	m_fSplatAlpha = 1.0f;
	m_fStarsAlpha = 1.0f;

	// If we havent created the components yet create them
	if(m_pCreamSpriteComp == NULL)
	{
		CreateSpriteComps();
		return;
	}

	// Set Sprite Data
	m_pCreamSpriteComp->SetSpriteData(GetCreamInitData());
	m_pSplatSpriteComp->SetSpriteData(GetSplatInitData());
	m_pPinkStarsSpriteComp->SetSpriteData(GetPinkStarsInitData());
	m_pSplaterSpriteComp->SetSpriteData(GetSplaterInitData());

	// Set Actives Off
	SetSpritesActive(false);
}

// Set Sprites Active
void CBlindSpriteEffect::SetSpritesActive(bool bActive)
{
	// Turn Off Whole Effect
	/*if(bActive == false)
	{*/
		if(m_pCreamSpriteComp)
			m_pCreamSpriteComp->SetActive(bActive);
		if(m_pSplatSpriteComp)
			m_pSplatSpriteComp->SetActive(bActive);
		if(m_pPinkStarsSpriteComp)
			m_pPinkStarsSpriteComp->SetActive(bActive);
		if(m_pSplaterSpriteComp)
			m_pSplaterSpriteComp->SetActive(bActive);
	//}
	//else // Turn On Sprites for Current Stage
	//{
	//	switch(m_nEffectStage)
	//	{
	//	case 0:
	//		if(m_pCreamSpriteComp)
	//			m_pCreamSpriteComp->SetActive(bActive);
	//	break;
	//	case 1:
	//		if(m_pCreamSpriteComp)
	//			m_pCreamSpriteComp->SetActive(bActive);
	//		if(m_pSplatSpriteComp)
	//			m_pSplatSpriteComp->SetActive(bActive);
	//	break;
	//	case 2:
	//		if(m_pCreamSpriteComp)
	//			m_pCreamSpriteComp->SetActive(bActive);
	//		if(m_pSplatSpriteComp)
	//			m_pSplatSpriteComp->SetActive(bActive);
	//		if(m_pPinkStarsSpriteComp)
	//			m_pPinkStarsSpriteComp->SetActive(bActive);
	//	break;
	//	}
	//}
}

///////////////////////////////////////////////////////////////////////////////
// UPDATES
///////////////////////////////////////////////////////////////////////////////

// Effect
void CBlindSpriteEffect::Update(float fDT)
{
	// Increment Counters
	m_fCounter += fDT;

	// Shutoff all Sprites
	SetSpritesActive(false);

	// Update Current Stage
	switch(m_nEffectStage)
	{
	case 0: UpdateStage0(fDT);
	break;
	case 1: UpdateStage1(fDT);
	break;
	case 2: UpdateStage2(fDT);
	break;
	}
}

// Stage 0
void CBlindSpriteEffect::UpdateStage0(float fDT)
{
	// Should we go to the next stage?
	if(m_fCounter > BLIND_ANIM_SWITCH_TIME * (float)m_nEffectStage)
	{
		// Increment Stage
		++m_nEffectStage;
	}

	// Fade out
	m_fSplatAlpha -= BLIND_ALPHA_RATE * fDT;
	if(m_fSplatAlpha < 0.0f)
		m_fSplatAlpha = 0.0f;

	// Update Sprites
	UpdateCream(fDT);
	UpdateSplat(fDT);
	UpdateStars(fDT);

	// Turn Sprites On
	m_pCreamSpriteComp->SetActive(true);
	m_pSplatSpriteComp->SetActive(true);
	m_pPinkStarsSpriteComp->SetActive(true);
}

// Stage 1
void CBlindSpriteEffect::UpdateStage1(float fDT)
{
	// Should we go to the next stage?
	if(m_fCounter > BLIND_ANIM_SWITCH_TIME * (float)m_nEffectStage + 1)
	{
		// Increment Stage
		++m_nEffectStage;
	}

	// Fade out
	m_fSplatAlpha -= BLIND_ALPHA_RATE * fDT;
	if(m_fSplatAlpha < 0.0f)
		m_fSplatAlpha = 0.0f;
	m_fStarsAlpha -= BLIND_ALPHA_RATE * fDT;
	if(m_fStarsAlpha < 0.0f)
		m_fStarsAlpha = 0.0f;

	// Update Cream Sprite
	UpdateCream(fDT);
	UpdateSplat(fDT);
	UpdateStars(fDT);

	// Turn Cream and Splat On
	m_pCreamSpriteComp->SetActive(true);
	m_pSplatSpriteComp->SetActive(true);
	m_pPinkStarsSpriteComp->SetActive(true);
	m_pSplaterSpriteComp->SetActive(true);
}

// Stage 2
void CBlindSpriteEffect::UpdateStage2(float fDT)
{
	// Should we go to the next stage?
	if(m_fCounter > BLIND_END_TIME)
	{
		// Fade out
		m_fCreamAlpha -= BLIND_ALPHA_RATE * fDT;

		if(m_fCreamAlpha < 0.0f)
		{
			// End and Reset Effect
			m_bActive = false;
			ResetSprites();
			return;
		}
	}

	// Fade out
	m_fSplatAlpha -= BLIND_ALPHA_RATE * fDT;
	if(m_fSplatAlpha < 0.0f)
		m_fSplatAlpha = 0.0f;
	m_fStarsAlpha -= BLIND_ALPHA_RATE * fDT;
	if(m_fStarsAlpha < 0.0f)
		m_fStarsAlpha = 0.0f;

	// Update Cream Sprite
	UpdateCream(fDT);
	UpdateSplat(fDT);
	UpdateStars(fDT);

	// Turn Sprites On
	m_pCreamSpriteComp->SetActive(true);
	m_pSplatSpriteComp->SetActive(true);
	m_pPinkStarsSpriteComp->SetActive(true);
	m_pSplaterSpriteComp->SetActive(true);
}

// Cream
void CBlindSpriteEffect::UpdateCream(float fDT)
{
	// Get Current Data
	TSpriteData tData = m_pCreamSpriteComp->GetSpriteData();
	tData.m_nY += (int)(BLIND_DROP_RATE * fDT);
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, m_fCreamAlpha);
	m_pCreamSpriteComp->SetSpriteData(tData);

	tData = m_pSplaterSpriteComp->GetSpriteData();
	tData.m_nY += (int)(BLIND_DROP_RATE * fDT);
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, m_fCreamAlpha);
	m_pSplaterSpriteComp->SetSpriteData(tData);
}

// Splat
void CBlindSpriteEffect::UpdateSplat(float fDT)
{
	// Get Current Data
	TSpriteData tData = m_pSplatSpriteComp->GetSpriteData();
	tData.m_nY += (int)(BLIND_DROP_RATE * fDT);
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, m_fSplatAlpha);
	m_pSplatSpriteComp->SetSpriteData(tData);
}

// Pink Stars
void CBlindSpriteEffect::UpdateStars(float fDT)
{
	// Get Current Data
	TSpriteData tData = m_pPinkStarsSpriteComp->GetSpriteData();
	tData.m_nY += (int)(BLIND_DROP_RATE * fDT);
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, m_fStarsAlpha);
	m_pPinkStarsSpriteComp->SetSpriteData(tData);
}

// Gameplay Init
void CBlindSpriteEffect::GameplayInitCallback(IEvent* pEvent, IComponent* pComp)
{
	// 
	CBlindSpriteEffect* pBSE = (CBlindSpriteEffect*)pComp;
	pBSE->ResetSprites();
	pBSE->m_bActive = false;
}