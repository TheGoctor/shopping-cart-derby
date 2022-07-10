#include "steal_effect.h"

#include "core/object_manager.h"
#include "rendering/texture_manager.h"

// Defines
#define STEAL_EFFECT_END_TIME (3.0f)
#define STEAL_EFFECT_SCALE_SWITCH (0.33f)
#define STEAL_EFFECT_BLAST_SCALE_RATE (0.05f)
#define STEAL_EFFECT_ITEM_SCALE_RATE (0.1f)
#define STEAL_EFFECT_BLAST_MOVE_RATE (10.0f)
#define STEAL_EFFECT_BLAST_TOT_RATE (3.14f)

namespace scd {

// Constructor
CStealEffect::CStealEffect(void)
    : m_pItemComp(NULL)
    , m_pBlastComp(NULL)
    , m_fScaleCounter(0.0f)
    , m_fCounter(0.0f)
    , m_bActive(false)
    , m_bScaleUp(true) {}

// Templates
TSpriteData CStealEffect::GetBlastInitData(void) {
  CTextureManager* pTM = CTextureManager::GetInstance();
  int texID = pTM->LoadTexture(
      "Resource\\Textures\\Donut Steal Sprites\\FFP_2D_Shine2_FIN.png");
  TSpriteData tData;
  tData.m_nTexture = texID;
  tData.m_nX = 0;
  tData.m_nY = 0;
  tData.m_nZ = 1;
  tData.m_fScaleX = 0.3f;
  tData.m_fScaleY = 0.3f;
  tData.m_fRot = 0.0f;
  tData.m_fRotCenterX = 256.0f;
  tData.m_fRotCenterY = 256.0f;
  tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
  tData.m_tRect.top = 0;
  tData.m_tRect.left = 0;
  tData.m_tRect.right = 512;
  tData.m_tRect.bottom = 512;
  return tData;
}

// Create SpriteComps
void CStealEffect::CreateSpriteComps(void) {
  // Keep a counter of how many we made
  static int nCount = 0;

  // Create Objects
  string szObjName = "StealBlast";
  szObjName += (char)nCount;
  CObject* pBlastObj = CObjectManager::GetInstance()->CreateObject(szObjName);
  m_pBlastComp = CTextureManager::GetInstance()->CreateSpriteComp(
      pBlastObj, GetBlastInitData(), false);
}

// Reset Sprites
void CStealEffect::ResetSprites(void) {
  if (m_pItemComp) {
    m_pItemComp->SetSpriteData(m_tOriginalItemData);
  }

  m_pItemComp = NULL;
  m_fCounter = 0.0f;
  m_fScaleCounter = 0.0f;

  // If we havent created it yet create it
  if (m_pBlastComp == NULL) {
    CreateSpriteComps();
    return;
  }

  // Set Sprite Data
  m_pBlastComp->SetSpriteData(GetBlastInitData());

  // Set Actives Off
  SetSpritesActive(false);
}

// Set On/Off
void CStealEffect::SetSpritesActive(bool active) {
  /*if(m_pItemComp)
          m_pItemComp->SetActive(active);*/
  if (m_pBlastComp)
    m_pBlastComp->SetActive(active);
}

// Update
void CStealEffect::Update(float fDT) {
  // Inc Timers
  m_fCounter += fDT;
  m_fScaleCounter += fDT;

  // Flip Scaling
  if (m_fScaleCounter > STEAL_EFFECT_SCALE_SWITCH) {
    m_fScaleCounter = 0.0f;
    m_bScaleUp = !m_bScaleUp;
  }

  // End Effect?
  if (m_fCounter > STEAL_EFFECT_END_TIME) {
    m_bActive = false;
    ResetSprites();
    return;
  }

  // Update Sprites
  UpdateItemComp(fDT);
  UpdateBlastComp(fDT);

  m_pBlastComp->SetActive(true);
}

// Item
void CStealEffect::UpdateItemComp(float fDT) {
  if (m_pItemComp == NULL)
    return;

  // Get Current Data
  TSpriteData tData = m_pItemComp->GetSpriteData();

  if (m_bScaleUp) {
    tData.m_fScaleX -= (STEAL_EFFECT_ITEM_SCALE_RATE * fDT);
    tData.m_fScaleY += (STEAL_EFFECT_ITEM_SCALE_RATE * fDT);
  } else {
    tData.m_fScaleX += (STEAL_EFFECT_ITEM_SCALE_RATE * fDT);
    tData.m_fScaleY -= (STEAL_EFFECT_ITEM_SCALE_RATE * fDT);
  }

  // Set Sprite Data
  m_pItemComp->SetSpriteData(tData);
}

// Blast
void CStealEffect::UpdateBlastComp(float fDT) {
  // Get Current Data
  TSpriteData tData = m_pBlastComp->GetSpriteData();

  if (m_pItemComp) {
    // TSpriteData tDataItem = m_pItemComp->GetSpriteData();
    tData.m_nX = m_tOriginalItemData.m_nX - 15;
    tData.m_nY = m_tOriginalItemData.m_nY - 25;
  }

  if (m_bScaleUp) {
    tData.m_fScaleX -= (STEAL_EFFECT_BLAST_SCALE_RATE * fDT);
    tData.m_fScaleY += (STEAL_EFFECT_BLAST_SCALE_RATE * fDT);
    tData.m_nX -= (int)(STEAL_EFFECT_BLAST_MOVE_RATE * fDT);
    tData.m_nY += (int)(STEAL_EFFECT_BLAST_MOVE_RATE * fDT);
  } else {
    tData.m_fScaleX += (STEAL_EFFECT_BLAST_SCALE_RATE * fDT);
    tData.m_fScaleY -= (STEAL_EFFECT_BLAST_SCALE_RATE * fDT);
    tData.m_nX += (int)(STEAL_EFFECT_BLAST_MOVE_RATE * fDT);
    tData.m_nY -= (int)(STEAL_EFFECT_BLAST_MOVE_RATE * fDT);
  }

  tData.m_fRot += STEAL_EFFECT_BLAST_TOT_RATE * fDT;

  // Set Sprite Data
  m_pBlastComp->SetSpriteData(tData);
}

} // namespace scd
