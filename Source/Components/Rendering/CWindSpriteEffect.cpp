////////////////////////////////////////////////////////////////////////////////
//	File			:	CWindSpriteEffect.cpp
//	Date			:	6/29/11
//	Mod. Date		:	6/29/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Wind Boost Effect
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CWindSpriteEffect.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\Texture Managers\CTextureManager.h"
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"

// Defines
#define BOOST_WIND_ANIM_SWITCHTIME (0.001f)
#define BOOST_WIND_POS_X		   (-150)
#define BOOST_WIND_POS_Y		   (-100)
#define BOOST_WIND_SCALE_X		   (5.25f)
#define BOOST_WIND_SCALE_Y		   (4.0f)
#define BOOST_WIND_NUM_FRAMES      (16)


// Constructor
CWindSpriteEffect::CWindSpriteEffect(void) : m_pWindScreenSprite(NULL),
											 m_fAnimTimer(0.0f),
											 m_nAnimCell(0),
											 m_fDuration(0.0f),
											 m_bActive(false)
{
}

// Helper Funcs
void CWindSpriteEffect::CreateWindSprite(void)
{
	// Screen Sprite
	int nTexID = CTextureManager::GetInstance()->LoadTexture("Resource\\Textures\\Boost Sprites\\FFP_2D_wind_FIN.png");
	TSpriteData tData;
	tData.m_nTexture = nTexID;
	tData.m_nX = BOOST_WIND_POS_X;
	tData.m_nY = BOOST_WIND_POS_Y;
	tData.m_nZ = 0;
	tData.m_fScaleX = BOOST_WIND_SCALE_X;
	tData.m_fScaleY = BOOST_WIND_SCALE_Y;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 1024;
	tData.m_tRect.bottom = 768;
	string szObjName = "WindScreenSprite";
	CObject* pObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	m_pWindScreenSprite = CTextureManager::GetInstance()->CreateSpriteComp(pObj, tData, false);
}

// Update
void CWindSpriteEffect::Update(float fDT)
{
	// We are Boosting
	if(m_fDuration > 0.0f)
	{
		// Decrease Duration
		m_fDuration -= fDT;

		// Update Wind
		UpdateWind(fDT);

		// Done
		if(m_fDuration < 0.0f)
		{
			m_pWindScreenSprite->SetActive(false);
			m_fDuration = 0.0f;
			m_fAnimTimer = 0.0f;
			m_nAnimCell = 0;
		}
		else // Keep Goin
		{
			m_pWindScreenSprite->SetActive(true);
		}
	}
	else // Just Stopped
	{
		m_bActive = false;
		m_pWindScreenSprite->SetActive(false);
	}
}

// Update Wind
void CWindSpriteEffect::UpdateWind(float fDT)
{
	// Increase Anim Timer
	m_fAnimTimer += fDT;

	// If we are past the Switch Time Change Cell
	if(m_fAnimTimer > (BOOST_WIND_ANIM_SWITCHTIME * ((float)m_nAnimCell + 1)))
	{
		// Go to next frame
		m_nAnimCell++;

		// Have we reached the end?
		if(m_nAnimCell > BOOST_WIND_NUM_FRAMES)
		{
			m_nAnimCell = 0;
			m_fAnimTimer = 0.0f;
		}
	}

	// Get Sprite Data
	TSpriteData tData = m_pWindScreenSprite->GetSpriteData();

	// Update Rect
	tData.m_tRect = CellAlgo(m_nAnimCell, 4, 256, 256);

	// Update Sprite Data
	m_pWindScreenSprite->SetSpriteData(tData);
}

// Cell Algo
RECT CWindSpriteEffect::CellAlgo(int nID, int nNumCols,
								int nCellWidth, int nCellHeight)
{
	RECT rCell;
	
	// pick out the proper letter from the image
	rCell.left = (nID % nNumCols) * nCellWidth;
	rCell.top = (nID / nNumCols) * nCellHeight;
	rCell.right = rCell.left + nCellWidth;
	rCell.bottom = rCell.top + nCellHeight;

	return rCell;
}

// Set Sprites Active
void CWindSpriteEffect::SetSpritesActive(bool bActive)
{
	m_pWindScreenSprite->SetActive(bActive);
}