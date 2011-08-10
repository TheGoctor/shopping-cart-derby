////////////////////////////////////////////////////////////////////////////////
//	File			:	CJamSpriteEffect.h
//	Date			:	6/4/11
//	Mod. Date		:	6/4/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Jam Radar Effect
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CJAMSPRITEEFFECT_H_
#define _CJAMSPRITEEFFECT_H_

// Includes
#include <windows.h>
#define LEAN_AND_MEAN

// Foward Declares
class CSpriteComponent;

// Structures
class CJamSpriteEffect
{
private:
	CSpriteComponent* pJamScreenSprite;

	float fJamCounter;
	float fJamCounter2;
	float fAlpha;
	int nNumExplodingCurrentFrame;
	int nNumRocketCurrentFrame;
	int nEffectStage;
	bool bActive;

	// Create SpriteComps
	void CreateSpriteComps(void);

	void UpdateStage0(float fDT);
	void UpdateStage1(float fDT);
	void UpdateStage2(float fDT);

	// Cell Algo
	RECT CellAlgo(int nID, int nNumCols,
						   int nCellWidth, int nCellHeight);

public:
	// Constructor
	CJamSpriteEffect(void);

	// Reset Sprites
	void ResetSprites(void);

	// Set On/Off
	void SetOn(bool bOn) { bActive = bOn; fJamCounter = 0.0f; fAlpha = 1.0f; nEffectStage = 0; }
	void SetSpritesActive(bool active);

	// Update
	void Update(float fDT);

	// Is Active
	bool IsActive(void) { return bActive; }

	// Mutators
	void SetStage(int nStage) { nEffectStage = nStage; }
	void SetCounter(float fTime) { fJamCounter = fTime; }
};

#endif // _CJAMSPRITEEFFECT_H_