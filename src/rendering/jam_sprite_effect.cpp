///////////////////////////////////////////////////////////////////////////////
// JAM EFFECT
///////////////////////////////////////////////////////////////////////////////

// Includes
#include "CJamSpriteEffect.h"
#include "Texture Managers\CTextureManager.h"
#include "..\\Object Manager\\CObjectManager.h"
#include "..\\..\\..\\Components\\Rendering\\CSpriteComponent.h"

// Jam
#define JAM_EXPLODING_CELL_SIZE (256)
#define JAM_JAR_X_POS (200)
#define JAM_JAR_Y_POS (860)
#define JAM_SPLAT_X_POS (400)
#define JAM_SPLAT_Y_POS (300)
#define JAM_EXPLODING_MOVE_Y (450.0f)
#define JAM_EXPLODING_NUM_FRAMES (6)
#define JAM_ROCKET_NUM_FRAMES (15)
#define JAM_EXPLODING_NUM_COLS (4)
#define JAM_EXPLODING_SWITCHTIME (0.0f) //
#define JAM_ROCKET_SWITCHTIME (0.001f)
#define JAM_SPLAT_TIME (0.3f) // .4
#define JAM_SPLAT_SCALE_RATE (1.8f)
#define JAM_SPLAT_MOVE_X_RATE (1000.0f)
#define JAM_SPLAT_MOVE_Y_RATE (600.0f)
#define JAM_SCREEN_ALPHA_RATE (0.5f)
#define JAM_SCREEN_TIME (8.0f)

#define JAM_SCREEN_SCALE_X_RATE (1.8f) // 1
#define JAM_SCREEN_SCALE_Y_RATE (1.8f) // 1
#define JAM_SCREEN_MOVE_X_RATE (720.0f) // 400
#define JAM_SCREEN_MOVE_Y_RATE (720.0f) // 400


RECT CJamSpriteEffect::CellAlgo(int nID, int nNumCols,
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

CJamSpriteEffect::CJamSpriteEffect(void) : pJamScreenSprite(NULL),
							   fJamCounter(0.0f), fJamCounter2(0.0f),
							   nNumExplodingCurrentFrame(0), nEffectStage(0),
							   bActive(false), fAlpha(1.0f), nNumRocketCurrentFrame(0)
{
}

// Set On/Off
void CJamSpriteEffect::SetSpritesActive(bool active)
{
	if(active == false)
	{
		if(pJamScreenSprite)
			pJamScreenSprite->SetActive(active);
	}
	else
	{
		switch(nEffectStage)
		{
		case 2:
			if(pJamScreenSprite)
				pJamScreenSprite->SetActive(active);
		break;
		}
	}
}

// Create SpriteComps
void CJamSpriteEffect::CreateSpriteComps(void)
{
	static int nCount = 0;

	// Screen Sprite
	int texID = CTextureManager::GetInstance()->LoadTexture("Resource\\Textures\\Jam Sprites\\FFP_2D_JamScreen_FIN.png");
	TSpriteData tData;
	tData.m_nTexture = texID;
	tData.m_nX = -80;
	tData.m_nY = -100;
	tData.m_nZ = 110;
	tData.m_fScaleX = 1.25f;
	tData.m_fScaleY = 1.25f;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 1024;
	tData.m_tRect.bottom = 768;
	string szObjName = "JamScreenSprite";
	szObjName += (char)nCount;
	CObject* pObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	pJamScreenSprite = CTextureManager::GetInstance()->CreateSpriteComp(pObj, tData, false);

	// Increment Counter
	nCount++;
}

// Reset Sprites
void CJamSpriteEffect::ResetSprites(void)
{
	fJamCounter = 0.0f;
	fJamCounter2 = 0.0f;
	fAlpha = 1.0f;
	nEffectStage = 0;
	nNumExplodingCurrentFrame = 0;
	nNumRocketCurrentFrame = 0;

	if(pJamScreenSprite == NULL)
	{
		CreateSpriteComps();
		return;
	}

	CTextureManager* pTM = CTextureManager::GetInstance();

	// Screen Sprite
	int texID = pTM->LoadTexture("Resource\\Textures\\Jam Sprites\\FFP_2D_JamScreen_FIN.png");
	TSpriteData tData;
	tData.m_nTexture = texID;
	tData.m_nX = -80;
	tData.m_nY = -100;
	tData.m_nZ = 110;
	tData.m_fScaleX = 1.25f;
	tData.m_fScaleY = 1.25f;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 1024;
	tData.m_tRect.bottom = 768;
	pJamScreenSprite->SetSpriteData(tData);

	pJamScreenSprite->SetIsActive(false);
}

// Update
void CJamSpriteEffect::Update(float fDT)
{
	fJamCounter  += fDT;
	fJamCounter2 += fDT;

	switch(nEffectStage)
	{
	case 0: UpdateStage0(fDT);
	break;
	case 1: UpdateStage1(fDT);
	break;
	case 2: UpdateStage2(fDT);
	}
}

// Stage 0
void CJamSpriteEffect::UpdateStage0(float fDT)
{
	// Check if we should move to the next frame
	if(fJamCounter2 > (JAM_ROCKET_SWITCHTIME * (float)nNumRocketCurrentFrame + 1))
	{
		// Go to next frame
		nNumRocketCurrentFrame++;

		// Have we reached the end?
		if(nNumRocketCurrentFrame > JAM_ROCKET_NUM_FRAMES)
		{
			nNumRocketCurrentFrame = 0;
			fJamCounter2 = 0.0f;
		}
	}

	// Check if we should move to the next frame
	if(fJamCounter > (JAM_EXPLODING_SWITCHTIME * (float)nNumExplodingCurrentFrame + 1))
	{
		// Go to next frame
		nNumExplodingCurrentFrame++;

		// Have we reached the end?
		if(nNumExplodingCurrentFrame > JAM_EXPLODING_NUM_FRAMES)
		{
			nNumExplodingCurrentFrame = 0;
			fJamCounter  = 0.0f;
			fJamCounter2 = 0.0f;
			nEffectStage++;

			return;
		}
	}
}

// Stage 1
void CJamSpriteEffect::UpdateStage1(float fDT)
{
	// Check if we should move to the next stage
	if(fJamCounter > JAM_SPLAT_TIME)
	{
		fJamCounter = 0.0f;
		fJamCounter2 = 0.0f;

		nEffectStage++;
	}
}

// Stage 2
void CJamSpriteEffect::UpdateStage2(float fDT)
{
	TSpriteData tData = pJamScreenSprite->GetSpriteData();
	
	if(fJamCounter > 0.0f && fJamCounter < 0.15f)
	{
		// Scale In
		tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, fAlpha);

		tData.m_nX += int(fDT * JAM_SCREEN_MOVE_X_RATE);
		tData.m_nY += int(fDT * JAM_SCREEN_MOVE_Y_RATE);
		if(tData.m_nX > 0)
			tData.m_nX = 0;
		if(tData.m_nY > 0)
			tData.m_nY = 0;

		tData.m_fScaleX -= fDT * JAM_SCREEN_SCALE_X_RATE;
		tData.m_fScaleY -= fDT * JAM_SCREEN_SCALE_Y_RATE;
		if(tData.m_fScaleX < 1.0f)
			tData.m_fScaleX = 1.0f;
		if(tData.m_fScaleY < 1.0f)
			tData.m_fScaleY = 1.0f;
	}
	else if(fJamCounter > 0.15f && fJamCounter < 0.22f) // .15 / .25
	{
		// Scale In
		tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, fAlpha);
		tData.m_nX -= int(fDT * (JAM_SCREEN_MOVE_X_RATE / 2));
		tData.m_nY -= int(fDT * (JAM_SCREEN_MOVE_Y_RATE / 2));
		if(tData.m_nX > 0)
			tData.m_nX = 0;
		if(tData.m_nY > 0)
			tData.m_nY = 0;

		tData.m_fScaleX += fDT * (JAM_SCREEN_SCALE_X_RATE / 2);
		tData.m_fScaleY += fDT * (JAM_SCREEN_SCALE_Y_RATE / 2);
		if(tData.m_fScaleX < 1.0f)
			tData.m_fScaleX = 1.0f;
		if(tData.m_fScaleY < 1.0f)
			tData.m_fScaleY = 1.0f;
	}
	else if(fJamCounter > 0.25f)
	{
		// Scale In
		tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, fAlpha);
		tData.m_nX += int(fDT * JAM_SCREEN_MOVE_X_RATE);
		tData.m_nY += int(fDT * JAM_SCREEN_MOVE_Y_RATE);
		if(tData.m_nX > 0)
			tData.m_nX = 0;
		if(tData.m_nY > 0)
			tData.m_nY = 0;

		tData.m_fScaleX -= fDT * (JAM_SCREEN_SCALE_X_RATE);
		tData.m_fScaleY -= fDT * (JAM_SCREEN_SCALE_Y_RATE);
		if(tData.m_fScaleX < 1.0f)
			tData.m_fScaleX = 1.0f;
		if(tData.m_fScaleY < 1.0f)
			tData.m_fScaleY = 1.0f;
	}
	
	pJamScreenSprite->SetSpriteData(tData);


	// Check if we should end the effect
	if(fJamCounter > JAM_SCREEN_TIME)
	{
		fAlpha -= JAM_SCREEN_ALPHA_RATE * fDT;
		
		if(fAlpha <= 0.0f)
		{
			pJamScreenSprite->SetActive(false);
			bActive = false;
			ResetSprites();
		}
		else
		{
			// Update Sprite Data
			TSpriteData tData = pJamScreenSprite->GetSpriteData();
			tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, fAlpha);

			pJamScreenSprite->SetSpriteData(tData);

			// Set Sprites
			pJamScreenSprite->SetActive(true);
		}
	}
	else
	{
		pJamScreenSprite->SetActive(true);
	}
}