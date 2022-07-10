////////////////////////////////////////////////////////////////////////////////
//	File			:	CWindSpriteEffect.h
//	Date			:	6/29/11
//	Mod. Date		:	6/29/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Wind Boost Effect
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CWINDSPRITEEFFECT_H_
#define _CWINDSPRITEEFFECT_H_

// Includes
#include <windows.h>
#define LEAN_AND_MEAN

// Foward Declares
class CSpriteComponent;

// Wind Sprite Effect
class CWindSpriteEffect
{
private:

	// Wind
	CSpriteComponent* m_pWindScreenSprite;

	// Anim Timer
	float m_fAnimTimer;
	int m_nAnimCell;

	// Duration
	float m_fDuration;

	// Active Flags
	bool m_bActive;

	// Update Wind
	void UpdateWind(float fDT);

	// Cell Algo
	RECT CellAlgo(int nID, int nNumCols, int nCellWidth, int nCellHeight);

public:

	// Constructor
	CWindSpriteEffect(void);

	// Update
	void Update(float fDT);

	// Accessor
	bool IsActive(void) { return m_bActive; }

	// Mutators
	void SetOn(bool bOn) { m_bActive = bOn; }
	void SetSpritesActive(bool bActive);
	void SetDuration(float fDuration) { m_fDuration = fDuration; }

	// Helper Funcs
	void CreateWindSprite(void);
};

#endif // _CWINDSPRITEEFFECT_H_