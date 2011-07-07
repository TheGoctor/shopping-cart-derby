////////////////////////////////////////////////////////////////////////////////
//	File			:	CBlindSpriteEffect.h
//	Date			:	6/4/11
//	Mod. Date		:	6/4/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Pie Blind Effect
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CBLINDSPRITEEFFECT_H_
#define _CBLINDSPRITEEFFECT_H_

// Includes
#include "..\\..\\..\\Components\\Rendering\\CSpriteComponent.h"

// Foward Declares
class CSpriteComponent;
class IEvent;

// Blind Sprite Effect
class CBlindSpriteEffect : IComponent
{
private:
	CSpriteComponent* m_pCreamSpriteComp;
	CSpriteComponent* m_pSplatSpriteComp;
	CSpriteComponent* m_pPinkStarsSpriteComp;
	float			  m_fCounter;
	float			  m_fCreamAlpha;
	float			  m_fSplatAlpha;
	float			  m_fStarsAlpha;
	int				  m_nEffectStage;
	bool			  m_bActive;

	// Create SpriteComps
	void CreateSpriteComps(void);

	// Templates
	TSpriteData GetCreamInitData(void);
	TSpriteData GetSplatInitData(void);
	TSpriteData GetPinkStarsInitData(void);

	// Updates
	void UpdateStage0(float fDT);
	void UpdateStage1(float fDT);
	void UpdateStage2(float fDT);
	void UpdateCream(float fDT);
	void UpdateSplat(float fDT);
	void UpdateStars(float fDT);

public:

	// Constructor
	CBlindSpriteEffect(void);

	// Reset Sprites
	void ResetSprites(void);

	// Set On/Off
	void SetOn(bool bOn) { m_bActive = bOn; }
	void SetSpritesActive(bool bActive);

	// Update
	void Update(float fDT);

	// Callbacks
	static void GameplayInitCallback(IEvent* pEvent, IComponent* pComp);


	// Accessors
	bool IsActive(void) { return m_bActive; }

	// Mutators
	void SetStage(int nStage) { m_nEffectStage = nStage; }
	void SetCounter(float fTime) { m_fCounter = fTime; }
};

#endif // _CBLINDSPRITEEFFECT_H_