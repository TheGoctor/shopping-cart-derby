///////////////////////////////////////////////////////////////////////////////
//  File			:	"CSpriteComponent.cpp"
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
// Includes
#include "CSpriteComponent.h"
#include"..\..\Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"

// Constructor
CSpriteComponent::CSpriteComponent(CObject* pParent, TSpriteData tSpriteData)
	: m_pcParent(pParent), m_tSpriteData(tSpriteData), m_pTM(NULL),
	  m_bActive(false)
{
	m_pTM = CTextureManager::GetInstance();
}

// Draw
void CSpriteComponent::DrawSprite(void)
{
	// Check for NULL
	if(m_pTM)
	{
		// Check for bad Rect
		RECT* pRect = &m_tSpriteData.m_tRect;
		if(pRect->bottom == pRect->top || pRect->left == pRect->right)
			pRect = NULL;

		// Draw the Sprite
		m_pTM->Draw(m_tSpriteData.m_nTexture, m_tSpriteData.m_nX,
			m_tSpriteData.m_nY, m_tSpriteData.m_fScaleX,
			m_tSpriteData.m_fScaleY, pRect,
			m_tSpriteData.m_fRotCenterX, m_tSpriteData.m_fRotCenterY,
			m_tSpriteData.m_fRot, m_tSpriteData.m_dwColor);
	}
}