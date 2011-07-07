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
#define JAM_EXPLODING_SWITCHTIME (0.2f)
#define JAM_ROCKET_SWITCHTIME (0.001f)
#define JAM_SPLAT_TIME (0.4f)
#define JAM_SPLAT_SCALE_RATE (1.8f)
#define JAM_SPLAT_MOVE_X_RATE (1000.0f)
#define JAM_SPLAT_MOVE_Y_RATE (600.0f)
#define JAM_SCREEN_ALPHA_RATE (0.5f)
#define JAM_SCREEN_TIME (8.0f)

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

CJamSpriteEffect::CJamSpriteEffect(void) : pJamScreenSprite(NULL), pJamExplodingSprite(NULL),
							   pJamSplatSprite(NULL), pJamRocketSprite(NULL),
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
		//bActive = active;
		if(pJamExplodingSprite)
			pJamExplodingSprite->SetActive(active);
		if(pJamRocketSprite)
			pJamRocketSprite->SetActive(active);
		if(pJamSplatSprite)
			pJamSplatSprite->SetActive(active);
		if(pJamScreenSprite)
			pJamScreenSprite->SetActive(active);
	}
	else
	{
		switch(nEffectStage)
		{
		case 0:
			if(pJamExplodingSprite)
				pJamExplodingSprite->SetActive(active);
			if(pJamRocketSprite)
				pJamRocketSprite->SetActive(active);
		break;
		case 1:
			if(pJamSplatSprite)
				pJamSplatSprite->SetActive(active);
		break;
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
	tData.m_nX = 0;
	tData.m_nY = 0;
	tData.m_nZ = 110;
	tData.m_fScaleX = 1.0f;
	tData.m_fScaleY = 1.0f;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 1024;
	tData.m_tRect.bottom = 768;
	string szObjName = "JamScreenSprite";
	szObjName += (char)nCount;
	CObject* pObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	pJamScreenSprite = CTextureManager::GetInstance()->CreateSpriteComp(pObj, tData, false);

	// Jar Exploding Sprite
	int texID1 = CTextureManager::GetInstance()->LoadTexture("Resource\\Textures\\Jam Sprites\\FFP_2D_JamJarExpand_FIN.png");
	TSpriteData tData1;
	tData1.m_nTexture = texID1;
	tData1.m_nX = JAM_JAR_X_POS;
	tData1.m_nY = JAM_JAR_Y_POS;
	tData1.m_nZ = 110;
	tData1.m_fScaleX = 2.0f;
	tData1.m_fScaleY = 2.0f;
	tData1.m_fRot = 0.0f;
	tData1.m_fRotCenterX = 0.0f;
	tData1.m_fRotCenterY = 0.0f;
	tData1.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData1.m_tRect.top  = 0;
	tData1.m_tRect.left = 0;
	tData1.m_tRect.right = JAM_EXPLODING_CELL_SIZE;
	tData1.m_tRect.bottom = JAM_EXPLODING_CELL_SIZE;
	szObjName = "JamExplodingSprite";
	szObjName += (char)nCount;
	CObject* pObj1 = CObjectManager::GetInstance()->CreateObject(szObjName);
	pJamExplodingSprite = CTextureManager::GetInstance()->CreateSpriteComp(pObj1, tData1, false);

	// Jar Splat Sprite
	int texID2 = CTextureManager::GetInstance()->LoadTexture("Resource\\Textures\\Jam Sprites\\FFP_2D_JamExplosion_FIN.png");
	TSpriteData tData2;
	tData2.m_nTexture = texID2;
	tData2.m_nX = JAM_SPLAT_X_POS;
	tData2.m_nY = JAM_SPLAT_Y_POS;
	tData2.m_nZ = 110;
	tData2.m_fScaleX = 0.25f;
	tData2.m_fScaleY = 0.25f;
	tData2.m_fRot = 0.0f;
	tData2.m_fRotCenterX = 0.0f;
	tData2.m_fRotCenterY = 0.0f;
	tData2.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData2.m_tRect.top  = 0;
	tData2.m_tRect.left = 0;
	tData2.m_tRect.right = 1024;
	tData2.m_tRect.bottom = 768;
	szObjName = "JamSplatSprite";
	szObjName += (char)nCount;
	CObject* pObj2 = CObjectManager::GetInstance()->CreateObject(szObjName);
	pJamSplatSprite = CTextureManager::GetInstance()->CreateSpriteComp(pObj2, tData2, false);

	// Jar Rocket Sprite
	int texID3 = CTextureManager::GetInstance()->LoadTexture("Resource\\Textures\\Jam Sprites\\FFP_2D_BlastSheet2_FIN.png");
	TSpriteData tData3;
	tData3.m_nTexture = texID3;
	tData3.m_nX = JAM_JAR_X_POS + 40;
	tData3.m_nY = JAM_JAR_Y_POS + 60;
	tData3.m_nZ = 109;
	tData3.m_fScaleX = 2.0f;
	tData3.m_fScaleY = 2.0f;
	tData3.m_fRot = 0.0f;
	tData3.m_fRotCenterX = 0.0f;
	tData3.m_fRotCenterY = 0.0f;
	tData3.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData3.m_tRect.top  = 0;
	tData3.m_tRect.left = 0;
	tData3.m_tRect.right = JAM_EXPLODING_CELL_SIZE;
	tData3.m_tRect.bottom = JAM_EXPLODING_CELL_SIZE;
	szObjName = "JamRocketSprite";
	szObjName += (char)nCount;
	CObject* pObj3 = CObjectManager::GetInstance()->CreateObject(szObjName);
	pJamRocketSprite = CTextureManager::GetInstance()->CreateSpriteComp(pObj3, tData3, false);

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
	tData.m_nX = 0;
	tData.m_nY = 0;
	tData.m_nZ = 110;
	tData.m_fScaleX = 1.0f;
	tData.m_fScaleY = 1.0f;
	tData.m_fRot = 0.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect.top  = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 1024;
	tData.m_tRect.bottom = 768;
	pJamScreenSprite->SetSpriteData(tData);

	// Jar Exploding Sprite
	int texID1 = pTM->LoadTexture("Resource\\Textures\\Jam Sprites\\FFP_2D_JamJarExpand_FIN.png");
	TSpriteData tData1;
	tData1.m_nTexture = texID1;
	tData1.m_nX = JAM_JAR_X_POS;
	tData1.m_nY = JAM_JAR_Y_POS;
	tData1.m_nZ = 110;
	tData1.m_fScaleX = 2.0f;
	tData1.m_fScaleY = 2.0f;
	tData1.m_fRot = 0.0f;
	tData1.m_fRotCenterX = 0.0f;
	tData1.m_fRotCenterY = 0.0f;
	tData1.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData1.m_tRect.top  = 0;
	tData1.m_tRect.left = 0;
	tData1.m_tRect.right = JAM_EXPLODING_CELL_SIZE;
	tData1.m_tRect.bottom = JAM_EXPLODING_CELL_SIZE;
	pJamExplodingSprite->SetSpriteData(tData1);

	// Jar Splat Sprite
	int texID2 = pTM->LoadTexture("Resource\\Textures\\Jam Sprites\\FFP_2D_JamExplosion_FIN.png");
	TSpriteData tData2;
	tData2.m_nTexture = texID2;
	tData2.m_nX = JAM_SPLAT_X_POS;
	tData2.m_nY = JAM_SPLAT_Y_POS;
	tData2.m_nZ = 110;
	tData2.m_fScaleX = 0.25f;
	tData2.m_fScaleY = 0.25f;
	tData2.m_fRot = 0.0f;
	tData2.m_fRotCenterX = 0.0f;
	tData2.m_fRotCenterY = 0.0f;
	tData2.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData2.m_tRect.top  = 0;
	tData2.m_tRect.left = 0;
	tData2.m_tRect.right = 1024;
	tData2.m_tRect.bottom = 768;
	pJamSplatSprite->SetSpriteData(tData2);

	// Jar Rocket Sprite
	int texID3 = pTM->LoadTexture("Resource\\Textures\\Jam Sprites\\FFP_2D_BlastSheet2_FIN.png");
	TSpriteData tData3;
	tData3.m_nTexture = texID3;
	tData3.m_nX = JAM_JAR_X_POS + 40;
	tData3.m_nY = JAM_JAR_Y_POS + 60;
	tData3.m_nZ = 109;
	tData3.m_fScaleX = 2.0f;
	tData3.m_fScaleY = 2.0f;
	tData3.m_fRot = 0.0f;
	tData3.m_fRotCenterX = 0.0f;
	tData3.m_fRotCenterY = 0.0f;
	tData3.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tData3.m_tRect.top  = 0;
	tData3.m_tRect.left = 0;
	tData3.m_tRect.right = JAM_EXPLODING_CELL_SIZE;
	tData3.m_tRect.bottom = JAM_EXPLODING_CELL_SIZE;
	pJamRocketSprite->SetSpriteData(tData3);

	pJamExplodingSprite->SetIsActive(true);
	pJamRocketSprite->SetIsActive(true);
	pJamSplatSprite->SetIsActive(false);
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
	// Update Rocket
	TSpriteData tDataR = pJamRocketSprite->GetSpriteData();

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
			pJamRocketSprite->SetActive(false);
		}
		else
		{
			pJamRocketSprite->SetActive(true);
		}
	}

	tDataR.m_nY -= (int)(JAM_EXPLODING_MOVE_Y * fDT);
	tDataR.m_tRect = CellAlgo(nNumRocketCurrentFrame, 
		JAM_EXPLODING_NUM_COLS, JAM_EXPLODING_CELL_SIZE, JAM_EXPLODING_CELL_SIZE);
	pJamRocketSprite->SetSpriteData(tDataR);

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

			// Switch Sprites
			pJamExplodingSprite->SetActive(false);
			pJamRocketSprite->SetActive(false);
			pJamSplatSprite->SetActive(true);
			pJamScreenSprite->SetActive(false);
			return;
		}
		else
		{
			pJamExplodingSprite->SetActive(true);
			//pJamRocketSprite->SetActive(true);
			pJamSplatSprite->SetActive(false);
			pJamScreenSprite->SetActive(false);
		}
	}

	// Have we reached the end?
	if(nNumExplodingCurrentFrame > JAM_EXPLODING_NUM_FRAMES - 1)
	{
		// Update Sprite Data
		TSpriteData tData = pJamSplatSprite->GetSpriteData();
		tData.m_nX -= (int)(JAM_SPLAT_MOVE_X_RATE * fDT);
		tData.m_nY -= (int)(JAM_SPLAT_MOVE_Y_RATE * fDT);
		tData.m_fScaleX += JAM_SPLAT_SCALE_RATE * fDT;
		tData.m_fScaleY += JAM_SPLAT_SCALE_RATE * fDT;
		pJamSplatSprite->SetSpriteData(tData);
		pJamSplatSprite->SetActive(true);
	}

	// Update Sprite Data
	TSpriteData tData = pJamExplodingSprite->GetSpriteData();
	tData.m_nY -= (int)(JAM_EXPLODING_MOVE_Y * fDT);
	tData.m_tRect = CellAlgo(nNumExplodingCurrentFrame, 
		JAM_EXPLODING_NUM_COLS, JAM_EXPLODING_CELL_SIZE, JAM_EXPLODING_CELL_SIZE);
	pJamExplodingSprite->SetSpriteData(tData);
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

		// Set Sprites
		pJamExplodingSprite->SetActive(false);
		pJamRocketSprite->SetActive(false);
		pJamSplatSprite->SetActive(false);
		pJamScreenSprite->SetActive(true);
		//return;
	}
	else
	{
		// Update Sprite Data
		TSpriteData tData = pJamSplatSprite->GetSpriteData();
		tData.m_nX -= (int)(JAM_SPLAT_MOVE_X_RATE * fDT);
		tData.m_nY -= (int)(JAM_SPLAT_MOVE_Y_RATE * fDT);
		tData.m_fScaleX += JAM_SPLAT_SCALE_RATE * fDT;
		tData.m_fScaleY += JAM_SPLAT_SCALE_RATE * fDT;
		pJamSplatSprite->SetSpriteData(tData);

		// Set Sprites
		pJamExplodingSprite->SetActive(false);
		pJamRocketSprite->SetActive(false);
		pJamSplatSprite->SetActive(true);
		pJamScreenSprite->SetActive(false);
	}
}

// Stage 2
void CJamSpriteEffect::UpdateStage2(float fDT)
{
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
			tData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
			pJamScreenSprite->SetSpriteData(tData);

			// Set Sprites
			pJamExplodingSprite->SetActive(false);
			pJamRocketSprite->SetActive(false);
			pJamSplatSprite->SetActive(false);
			pJamScreenSprite->SetActive(true);
		}
	}
	else
	{
		// Set Sprites
		pJamExplodingSprite->SetActive(false);
		pJamRocketSprite->SetActive(false);
		pJamSplatSprite->SetActive(false);
		pJamScreenSprite->SetActive(true);
	}
}