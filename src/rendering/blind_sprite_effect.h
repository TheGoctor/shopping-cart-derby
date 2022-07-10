////////////////////////////////////////////////////////////////////////////////
//	File			:	CBlindSpriteEffect.h
//	Date			:	6/4/11
//	Mod. Date		:	6/4/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Pie Blind Effect
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "components/rendering/sprite.h"
#include "core/base_component.h"

namespace scd {

// Blind Sprite Effect
class blind_sprite_effect : scd::base_component {
private:
  CSpriteComponent* m_pCreamSpriteComp;
  CSpriteComponent* m_pSplatSpriteComp;
  CSpriteComponent* m_pPinkStarsSpriteComp;
  CSpriteComponent* m_pSplaterSpriteComp;

  float m_fCounter;
  float m_fCreamAlpha;
  float m_fSplatAlpha;
  float m_fStarsAlpha;
  int m_nEffectStage;
  bool m_bActive;

  // Create SpriteComps
  void CreateSpriteComps(void);

  // Templates
  TSpriteData GetCreamInitData(void);
  TSpriteData GetSplatInitData(void);
  TSpriteData GetPinkStarsInitData(void);
  TSpriteData GetSplaterInitData(void);

  // Updates
  void UpdateStage0(float fDT);
  void UpdateStage1(float fDT);
  void UpdateStage2(float fDT);
  void UpdateCream(float fDT);
  void UpdateSplat(float fDT);
  void UpdateStars(float fDT);

public:
  // Constructor
  blind_sprite_effect(void);

  // Reset Sprites
  void ResetSprites(void);

  // Set On/Off
  void SetOn(bool bOn) {
    m_bActive      = bOn;
    m_fCounter     = 0.0f;
    m_nEffectStage = 0;
    m_fCreamAlpha  = 1.0f;
    m_fSplatAlpha  = 1.0f;
    m_fStarsAlpha  = 1.0f;
  }
  void SetSpritesActive(bool bActive);

  // Update
  void Update(float fDT);

  // Callbacks
  static void GameplayInitCallback(event* pEvent, base_component* pComp);

  // Mutators
  void SetStage(int nStage) { m_nEffectStage = nStage; }
  void SetCounter(float fTime) { m_fCounter = fTime; }
};

} // namespace scd
