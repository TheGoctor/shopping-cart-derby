///////////////////////////////////////////////////////////////////////////////
//  File			:	"CSpriteComponent.h"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/11/11
//
//	Last Changed	:	04/11/11
//
//  Purpose			:	A component that allows a game object to be represented
//						as a sprite
///////////////////////////////////////////////////////////////////////////////
#ifndef _CSPRITECOMPONENT_H_
#define _CSPRITECOMPONENT_H_

#include <windows.h>
#define LEAN_AND_MEAN

#include"..\\..\\IComponent.h"

// Foward Declares
class CTextureManager;
class CObject;

// Structs
struct TSpriteData
{
	int		 m_nTexture;	// The Texture ID of the Sprite
	int		 m_nX;			// The X Position of the Sprite in Screen Space
	int		 m_nY;			// The Y Position of the Sprite in Screen Space
	int		 m_nZ;			// The Z Position of the Sprite in Screen Space
	float	 m_fScaleX;		// The Scale of the Sprite in the X
	float	 m_fScaleY;		// The Scale of the Sprite in the X
	float	 m_fRotCenterX; // The the Center point for Sprite in the X
	float	 m_fRotCenterY; // The the Center point for Sprite in the Y
	float	 m_fRot;		// The the amount of rotation applied to the sprite
	DWORD	 m_dwColor;		// The color to render the Sprite in
	RECT	 m_tRect;		// The rectangle section of the Sprite to Draw
};

class CSpriteComponent : public IComponent
{
	CTextureManager*	m_pTM;		   // A Pointer to the Texture Manager
	CObject*			m_pcParent;	   // The Game Object this Comp us associated with
	TSpriteData			m_tSpriteData; // The data of the Sprite
	bool				m_bActive;	   // A bool stating if the Sprite is Rendering

public:

	// Constructor
	CSpriteComponent(CObject* pParent, TSpriteData tSpriteData);

	// Draw Sprite
	void DrawSprite(void);

	// Accessors
	inline CObject* GetParent()
	{
		return m_pcParent;
	}

	inline TSpriteData GetSpriteData()
	{
		return m_tSpriteData;
	}

	inline bool IsActive(void)
	{
		return m_bActive;
	}

	// Mutators
	inline void SetSpriteData(TSpriteData tSpriteData)
	{
		m_tSpriteData = tSpriteData;
	}

	inline void SetActive(bool bActive)
	{
		m_bActive = bActive;
	}

	// Sort
	bool operator <(CSpriteComponent& sc)
	{
		return this->m_tSpriteData.m_nZ < sc.m_tSpriteData.m_nZ;
	}
};

#endif // _CSPRITECOMPONENT_H_