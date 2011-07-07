////////////////////////////////////////////////////////////////////////////////
//	File			:	CStealEffect.h
//	Date			:	6/4/11
//	Mod. Date		:	6/4/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Donut Steal Effect
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CSTEALEFFECT_H_
#define _CSTEALEFFECT_H_

// Includes
#include "..\\..\\..\\Components\\Rendering\\CSpriteComponent.h"

// Steal Sprite Effect
class CStealEffect
{
private:
	CSpriteComponent* m_pItemComp;
	CSpriteComponent* m_pBlastComp;
	float			  m_fScaleCounter;
	float			  m_fCounter;
	bool			  m_bScaleUp;
	bool			  m_bActive;
	TSpriteData		  m_tOriginalItemData;

	// Create SpriteComps
	void CreateSpriteComps(void);

	// Templates
	TSpriteData GetBlastInitData(void);

	// Updates
	void UpdateItemComp(float fDT);
	void UpdateBlastComp(float fDT);

public:

	// Constructor
	CStealEffect(void);

	// Reset Sprites
	void ResetSprites(void);

	// Update
	void Update(float fDT);

	// Accessors
	bool IsActive(void) { return m_bActive; }

	// Set On/Off
	void SetOn(bool bOn) { m_bActive = bOn; }
	void SetSpritesActive(bool active);

	// Mutators
	void SetItemSpriteComp(CSpriteComponent* pComp)
	{ 
		m_pItemComp = pComp;

		// Store the original sprite data
		if(pComp)
		{
			m_tOriginalItemData = pComp->GetSpriteData();
			TSpriteData tData = m_pBlastComp->GetSpriteData();
			tData.m_nX = m_tOriginalItemData.m_nX - 15;
			tData.m_nY = m_tOriginalItemData.m_nY - 25;
			m_pBlastComp->SetSpriteData(tData);
		}
	}


};

#endif // _CEFFECTMANAGER_H_