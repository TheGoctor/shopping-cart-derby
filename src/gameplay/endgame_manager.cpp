////////////////////////////////////////////////////////////////////////////////
//	File			:	endgame_manager.cpp
//	Date			:	6/17/11
//	Mod. Date		:	6/17/11
//	Mod. Initials	:	JL, HN, MS
//	Author			:
//	Purpose			:	Encapsulates the Endgame Senarios
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "endgame_manager.h"

#include "audio/wwise_audio_manager.h"
#include "components/rendering/sprite.h"
#include "core/object.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "gameplay/unlockable_manager.h"
#include "rendering/hud_manager.h"
#include "rendering/texture_manager.h"

/// Defines
#define FADE_ALPHA_RATE (0.5f)
#define LOSE_FADE_TIME (3.0f)

// Constructor
endgame_manager::endgame_manager()
  : m_bExitingGameplay(false)
  , m_bLoseFade(false)
  , m_fFadeScreenAlpha(0.0f)
  , m_pFadeScreenComp(NULL)
  , m_pWinnerObject(NULL)
  , m_pLoserObject1(NULL)
  , m_pLoserObject2(NULL)
  , m_pLoserObject3(NULL)
  , m_pPlayerObject(NULL)
  , m_pWinLoseComp(NULL)
  , m_pMainMenuComp(NULL)
  , m_fLoseFadeTimer(0.0f)
  , m_bVictoryPlayed(false)
  , m_bHumanWon(false)
  , m_fWinLoseAlpha(0.0f)
  , m_bSoundPlayed(false)
  , m_fCameraMoveTimer(0.0f)
  , m_bCameraMoved(false)
  , m_fCameraTime(0.0f) {
  m_bExitingGameplay = false;
  m_fFadeScreenAlpha = 0.0f;
  m_fCameraMoveTimer = 0.0f;
  // Register for Events
  CEventManager* pEM = CEventManager::GetInstance();
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;

  pEM->RegisterEvent(szEventName, (IComponent*)this, GameplayUpdateCallback);

  szEventName = "InitObjects";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, (IComponent*)this, GameplayInitCallback);

  szEventName = "InitObjects";
  szEventName += WIN_STATE;
  pEM->RegisterEvent(szEventName, (IComponent*)this, WinInitCallback);

  szEventName = "Update";
  szEventName += WIN_STATE;
  pEM->RegisterEvent(szEventName, (IComponent*)this, WinUpdateCallback);

  szEventName = "GameWon";
  pEM->RegisterEvent(szEventName, (IComponent*)this, WinGameCallback);

  szEventName = "ShutdownObjects";
  szEventName += WIN_STATE;
  pEM->RegisterEvent(szEventName, (IComponent*)this, WinStateExitCallback);

  szEventName = "AcceptWinState";
  pEM->RegisterEvent(szEventName, (IComponent*)this, EnterPressedCallback);

  // Create object
  CObjectManager* pOM     = CObjectManager::GetInstance();
  CObject* pFadeScreenObj = pOM->CreateObject("EndFadeScreen");

  m_pPlayerObject = pOM->CreateObject("InvisObject");

  CTextureManager* pTM = CTextureManager::GetInstance();
  /// Setup sprite data
  TSpriteData tSpriteData;
  tSpriteData.m_nTexture
    = pTM->LoadTexture("Resource\\HUD\\T_Black_Screen_D.png", 0);
  tSpriteData.m_fScaleX      = 1.0f;
  tSpriteData.m_fScaleY      = 1.0f;
  tSpriteData.m_nZ           = 130;
  tSpriteData.m_fRot         = 0.0f;
  tSpriteData.m_fRotCenterX  = 0.0f;
  tSpriteData.m_fRotCenterY  = 0.0f;
  tSpriteData.m_nX           = 0;
  tSpriteData.m_nY           = 0;
  tSpriteData.m_dwColor      = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  tSpriteData.m_tRect.bottom = 0;
  tSpriteData.m_tRect.left   = 0;
  tSpriteData.m_tRect.right  = 0;
  tSpriteData.m_tRect.top    = 0;

  m_pFadeScreenComp = pTM->CreateSpriteComp(pFadeScreenObj, tSpriteData, false);

  CObject* pWinTextObj = pOM->CreateObject("WinText");

  tSpriteData.m_nTexture
    = pTM->LoadTexture("Resource\\HUD\\T_Endings_Words_D.png", 0);
  tSpriteData.m_fScaleX      = 1.0f;
  tSpriteData.m_fScaleY      = 1.0f;
  tSpriteData.m_fRotCenterX  = 0.0f;
  tSpriteData.m_fRotCenterY  = 0.0f;
  tSpriteData.m_fRot         = 0.0f;
  tSpriteData.m_nX           = 250;
  tSpriteData.m_nY           = 30;
  tSpriteData.m_nZ           = 131;
  tSpriteData.m_dwColor      = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  tSpriteData.m_tRect.top    = 0;
  tSpriteData.m_tRect.bottom = 145;
  tSpriteData.m_tRect.left   = 0;
  tSpriteData.m_tRect.right  = 500;

  m_pWinLoseComp = pTM->CreateSpriteComp(pWinTextObj, tSpriteData, false);

  CObject* pMainMenuText = pOM->CreateObject("MainMenuText");

  tSpriteData.m_fScaleX      = 1.25f;
  tSpriteData.m_fScaleY      = 1.25f;
  tSpriteData.m_tRect.top    = 190;
  tSpriteData.m_tRect.left   = 10;
  tSpriteData.m_tRect.right  = 300;
  tSpriteData.m_tRect.bottom = 265;
  tSpriteData.m_nX           = 350;
  tSpriteData.m_nY           = 650;

  m_pMainMenuComp = pTM->CreateSpriteComp(pMainMenuText, tSpriteData, false);

  CObject* pFinishFlagPicObj = pOM->CreateObject("FinishFlagPic");

  tSpriteData.m_nTexture
    = pTM->LoadTexture("Resource\\HUD\\T_Finish_Sign_D.png", 0);
  tSpriteData.m_fScaleX      = 1.0f;
  tSpriteData.m_fScaleY      = 1.0f;
  tSpriteData.m_tRect.top    = 0;
  tSpriteData.m_tRect.left   = 0;
  tSpriteData.m_tRect.right  = 0;
  tSpriteData.m_tRect.bottom = 0;
  tSpriteData.m_nX           = 150; // set in update
  tSpriteData.m_nY           = 150;

  m_pFinishFlag = pTM->CreateSpriteComp(pFinishFlagPicObj, tSpriteData, false);
  if (!CUnlockableManager::GetPlayerNotified()) {
    CBitmapFont font(-1, 13, 15, 33);
    font.LoadFont("Resource\\BitmapFont.png", "Resource\\BitmapFont_Width.bin");
    float fScale = 5.0f;
    m_nFontX     = -100;
    m_nFontY     = 740;
    scd::vector4 fontColor(1.0f, 1.0f, 1.0f, 255.0f);
    std::string szUnlockMsg = "New Shoppers Unlocked!";
    CObject* pFontObj       = CObjectManager::CreateObject("UnlockMsgObj");
    m_pFont                 = CTextureManager::CreateBitmapFontComp(pFontObj,
                                                    szUnlockMsg,
                                                    font,
                                                    m_nFontX,
                                                    m_nFontY,
                                                    fScale,
                                                    fontColor,
                                                    false);
    //		m_pFont->DrawWord();
    m_pFont->SetX(m_nFontX);
    m_pFont->SetY(m_nFontY);
  }
}

// Helper funcs
void endgame_manager::ResetSprites(void) {
  // Update the Fade Screen Sprite
  TSpriteData pSData = m_pFadeScreenComp->GetSpriteData();
  pSData.m_dwColor   = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  m_pFadeScreenComp->SetSpriteData(pSData);

  // Update the Win/Lose Sprite
  pSData           = m_pWinLoseComp->GetSpriteData();
  pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  m_pWinLoseComp->SetSpriteData(pSData);

  // Update the Main Menu Sprite
  pSData           = m_pMainMenuComp->GetSpriteData();
  pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  m_pMainMenuComp->SetSpriteData(pSData);

  pSData           = m_pFinishFlag->GetSpriteData();
  pSData.m_fScaleX = pSData.m_fScaleY = 0.0f;
  m_pFinishFlag->SetSpriteData(pSData);
}

void endgame_manager::on_gameplay_update() {
  // Get the Data from the Event
  TUpdateStateEvent* tData = (TUpdateStateEvent*)pEvent->GetData();

  // Check to see if Gameplay is exiting
  if (m_bExitingGameplay) {
    // Update Time
    float fDT = tData->m_fDeltaTime;

    // Increase Fade
    pEGM->m_fFadeScreenAlpha += fDT * FADE_ALPHA_RATE;
    if (pEGM->m_fFadeScreenAlpha > 1.0f) {
      // Change States
      SendStateEvent(
        "PushState", (IComponent*)GetInstance(), WIN_STATE, PRIORITY_NORMAL);

      // Reset Values
      pEGM->m_bExitingGameplay = false;
      pEGM->m_fFadeScreenAlpha = 1.0f;
    }

    // Update the Sprite
    TSpriteData pSData = pEGM->m_pFadeScreenComp->GetSpriteData();
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, pEGM->m_fFadeScreenAlpha);
    pEGM->m_pFadeScreenComp->SetSpriteData(pSData);

    // Do stuff for the finish flag showing up and stuff
    if (pEGM->m_bHumanWon
        && pEGM->m_fFadeScreenAlpha
             <= .25f) // only do for half the fade time. this makes it go faster
                      // because normally we'd just sin(x*.5) but that makes it
                      // too slow
    {
      TSpriteData pSData1 = pEGM->m_pFinishFlag->GetSpriteData();
      pSData1.m_fScaleX   = sin(pEGM->m_fFadeScreenAlpha * 3.1415f * 2.0f);
      pSData1.m_fScaleY   = pSData.m_fScaleX;
      pSData1.m_dwColor   = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);

      pSData1.m_nX
        = 325
          - (int)(sin(pEGM->m_fFadeScreenAlpha * 3.1415)
                  * 110.0f); // sin()*VAL where VAL = range at which it moves

      pEGM->m_pFinishFlag->SetSpriteData(pSData1);
    }
  }
}

void endgame_manager::GameplayInitCallback(IEvent*, IComponent*) {
  // Get Singletons
  endgame_manager* pEGM = GetInstance();

  // reset values so when game restarts, this stuff works fine
  pEGM->m_bExitingGameplay = false;
  pEGM->m_fFadeScreenAlpha = 0.0f;
  pEGM->m_fWinLoseAlpha    = 0.0f;
  pEGM->m_pWinnerObject    = NULL;
  pEGM->m_pLoserObject1    = NULL;
  pEGM->m_pLoserObject2    = NULL;
  pEGM->m_fCameraMoveTimer = 0.0f;
  pEGM->m_pLoserObject3    = NULL;
  pEGM->m_bLoseFade        = false;
  pEGM->m_bCameraMoved     = false;
  pEGM->m_fLoseFadeTimer   = 0.0f;
  pEGM->m_fCameraTime      = 0.0f;
  pEGM->ResetSprites();

  // set all the components to invisible false
  pEGM->m_pFadeScreenComp->SetActive(false);
  pEGM->m_pMainMenuComp->SetActive(false);
  pEGM->m_pWinLoseComp->SetActive(false);
  pEGM->m_pFinishFlag->SetActive(false);
}

void endgame_manager::WinInitCallback(IEvent*, IComponent*) {
  endgame_manager* pEGM = GetInstance();

  CWwiseSoundManager::PlayTheSound(GAMEPLAY_MUSIC_STOP, GLOBAL_ID);
  // Position Player by their Rank

  // Set up a matrix for the players (So they face the right way)
  scd::transform mMat;
  D3DXMatrixRotationAxis(
    &mMat, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), 3.1415 * 0.0f + 3.1415f);

  // set the position of the people in win state
  mMat._41 = 3.5f;
  mMat._43 = 59.0f;

  // winner player
  if (pEGM->m_pWinnerObject) // if not null
  {
    // winner is in center
    // D3DXMatrixIdentity(&pEGM->m_pWinnerObject->GetTransform()->GetLocalMatrix());
    // // dont identity
    pEGM->m_pWinnerObject->GetTransform()->GetLocalMatrix() = mMat;
  }
  // loser players
  if (pEGM->m_pLoserObject2) // if not null
  {
    // loser1 is in right back
    mMat._41 -= 4.0f;
    mMat._43 += 2.0f;
    pEGM->m_pLoserObject2->GetTransform()->GetLocalMatrix() = mMat;
  }
  if (pEGM->m_pLoserObject1) // if not null
  {
    mMat._41 += 5.5f;
    pEGM->m_pLoserObject1->GetTransform()->GetLocalMatrix() = mMat;
  }
  if (pEGM->m_pLoserObject3) // if not null
  {
    // left back (when facing checkout)
    mMat._41 += 3.0f;
    pEGM->m_pLoserObject3->GetTransform()->GetLocalMatrix() = mMat;
  }

  // Set appropriate sprite data
  if (pEGM->m_bHumanWon) {
    // Player has beaten the game

    // Update the Sprite
    TSpriteData pSData    = pEGM->m_pWinLoseComp->GetSpriteData();
    pSData.m_tRect.left   = 0;
    pSData.m_tRect.top    = 0;
    pSData.m_tRect.right  = 500;
    pSData.m_tRect.bottom = 145;
    pEGM->m_pWinLoseComp->SetSpriteData(pSData);
    if (!CUnlockableManager::GetPlayerNotified()) {
      CBitmapFont font(-1, 13, 15, 33);
      font.LoadFont("Resource\\BitmapFont.png",
                    "Resource\\BitmapFont_Width.bin");
      float fScale = 2.0f;
      //		pEGM->m_nFontX+=100.0f*fDT;
      scd::vector4 fontColor(RAND_FLOAT(0.0f, 1.0f),
                             RAND_FLOAT(0.0f, 1.0f),
                             RAND_FLOAT(0.0f, 1.0f),
                             255.0f);
      std::string szUnlockMsg = "New Shoppers Unlocked!";

      pEGM->m_pFont = CTextureManager::CreateBitmapFontComp(
        CObjectManager::GetObjectByName("UnlockMsgObj"),
        szUnlockMsg,
        font,
        m_nFontX,
        m_nFontY,
        fScale,
        fontColor,
        true);

      pEGM->m_pFont->SetX(pEGM->m_nFontX);
      pEGM->m_pFont->SetY(pEGM->m_nFontY);
      pEGM->m_pFont->SetColor(fontColor);
      //		pEGM->m_pFont->DrawWord();
    }
    // unlock the characters
    CUnlockableManager::SetCharactersUnlocked(true);

  } else {
    // Player Sux

    // Update the Sprite
    TSpriteData pSData    = pEGM->m_pWinLoseComp->GetSpriteData();
    pSData.m_tRect.left   = 0;
    pSData.m_tRect.top    = 280;
    pSData.m_tRect.right  = 500;
    pSData.m_tRect.bottom = 450;

    pEGM->m_pWinLoseComp->SetSpriteData(pSData);
  }

  // Turn Sprites On
  pEGM->m_pWinLoseComp->SetActive(true);
  pEGM->m_pMainMenuComp->SetActive(true);

  // turn the finish flag off
  pEGM->m_pFinishFlag->SetActive(false);

  // Set the camera to point to winner
  if (pEGM->m_bHumanWon) {
    pEGM->m_pPlayerObject->GetTransform()->GetLocalMatrix()
      = pEGM->m_pWinnerObject->GetTransform()->GetLocalMatrix();
    SendObjectEvent(
      "AttachToCameraWin", (IComponent*)GetInstance(), pEGM->m_pPlayerObject);
    // TODO: Do confetti here
    SendIEvent("HumanWinEffect", (IComponent*)pEGM, NULL, PRIORITY_NORMAL);
  } else {
    pEGM->m_pPlayerObject->GetTransform()->GetLocalMatrix()
      = pEGM->m_pLoserObject1->GetTransform()->GetLocalMatrix();
    SendObjectEvent(
      "AttachToCameraLose", (IComponent*)GetInstance(), pEGM->m_pPlayerObject);
  }

  CWwiseSoundManager::PlayMusic(MENU_MUSIC_PLAY, GLOBAL_ID);
  //
}

void endgame_manager::WinUpdateCallback(IEvent* pEvent, IComponent*) {
  // Get Singletons
  endgame_manager* pEGM = GetInstance();

  // Get Data from Event
  TUpdateStateEvent* tData = (TUpdateStateEvent*)pEvent->GetData();
  float fDT                = tData->m_fDeltaTime;
  if (!CUnlockableManager::GetPlayerNotified()) {
    CBitmapFont font(-1, 13, 15, 33);
    font.LoadFont("Resource\\BitmapFont.png", "Resource\\BitmapFont_Width.bin");
    //		float fScale = 2.0f;
    pEGM->m_nFontX += (int)(80 * fDT);
    if (pEGM->m_nFontX >= 1024)
      pEGM->m_nFontX = -480;
    pEGM->m_nFontY = 740;
    scd::vector4 fontColor(RAND_FLOAT(0.0f, 1.0f),
                           RAND_FLOAT(0.0f, 1.0f),
                           RAND_FLOAT(0.0f, 1.0f),
                           255.0f);
    std::string szUnlockMsg = "New Shoppers Unlocked!";
    //
    //		pEGM->m_pFont =
    // CTextureManager::CreateBitmapFontComp(CObjectManager::GetObjectByName("UnlockMsgObj"),
    // szUnlockMsg, 			font, m_nFontX,
    // m_fFontY, fScale, fontColor, true);
    //
    //		pEGM->m_pFont = font;
    pEGM->m_pFont->SetWord(szUnlockMsg);
    pEGM->m_pFont->SetX(pEGM->m_nFontX);
    pEGM->m_pFont->SetY(pEGM->m_nFontY);
    pEGM->m_pFont->SetColor(fontColor);
    pEGM->m_pFont->SetIsActive(true);
    pEGM->m_pFont->DrawWord();
  }
  //	else
  //	{
  //		pEGM->m_pFont->SetActive(false);
  //	}

  // Decrease Fade
  if (pEGM->m_bLoseFade == false && pEGM->m_fLoseFadeTimer == 0.0f) {
    pEGM->m_fFadeScreenAlpha -= fDT * FADE_ALPHA_RATE;
    if (pEGM->m_fFadeScreenAlpha < 0.0f) {
      pEGM->m_fFadeScreenAlpha = 0.0f;

      if (pEGM->m_bHumanWon == false) {
        // pEGM->m_bLoseFade = true;
        pEGM->m_fLoseFadeTimer = LOSE_FADE_TIME;
      }
    }

    // Update the Fade Screen Sprite
    TSpriteData pSData = pEGM->m_pFadeScreenComp->GetSpriteData();
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, pEGM->m_fFadeScreenAlpha);
    pEGM->m_pFadeScreenComp->SetSpriteData(pSData);
  }

  // Decrease Lose Fade Time
  if (pEGM->m_fLoseFadeTimer > 0.0f) {
    pEGM->m_fLoseFadeTimer -= fDT;
    if (pEGM->m_fLoseFadeTimer <= 0.0f) {
      // pEGM->m_fLoseFadeTimer = 0.0f;
      pEGM->m_bLoseFade = true;
    }
  }

  // Increase Win/Lose Fade
  if (pEGM->m_fFadeScreenAlpha == 0.0f) {
    pEGM->m_fWinLoseAlpha += fDT * FADE_ALPHA_RATE;
    if (pEGM->m_fWinLoseAlpha > 1.0f) {
      pEGM->m_fWinLoseAlpha = 1.0f;
    }

    // Update the Win/Lose Sprite
    TSpriteData pSData = pEGM->m_pWinLoseComp->GetSpriteData();
    pSData.m_dwColor   = scd::vector4(1.0f, 1.0f, 1.0f, pEGM->m_fWinLoseAlpha);
    pEGM->m_pWinLoseComp->SetSpriteData(pSData);

    // Update the Main Menu Sprite
    pSData           = pEGM->m_pMainMenuComp->GetSpriteData();
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, pEGM->m_fWinLoseAlpha);
    pEGM->m_pMainMenuComp->SetSpriteData(pSData);
  }

  // Lose Fade
  if (pEGM->m_bLoseFade == true) {
    pEGM->m_fFadeScreenAlpha += fDT * FADE_ALPHA_RATE;
    if (pEGM->m_fFadeScreenAlpha > 0.5f) {
      pEGM->m_fFadeScreenAlpha = 0.5f;
      // pEGM->m_bLoseFade = false;
    }

    // Update the Fade Screen Sprite
    TSpriteData pSData = pEGM->m_pFadeScreenComp->GetSpriteData();
    // pSData.m_nZ = 110;
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, pEGM->m_fFadeScreenAlpha);
    pEGM->m_pFadeScreenComp->SetSpriteData(pSData);
  }

  if (!pEGM->m_bHumanWon) {
    //////////////////On Lose, the camera starts at the player and slowly moves
    /// back
    D3DXVECTOR3 vStart(
      pEGM->m_pLoserObject1->GetTransform()->GetLocalMatrix()._41,
      pEGM->m_pLoserObject1->GetTransform()->GetLocalMatrix()._42,
      pEGM->m_pLoserObject1->GetTransform()->GetLocalMatrix()._43);

    D3DXVECTOR3 vEnd(vStart.x - 2.0f, vStart.y + 1.0f, vStart.z - 10.0f);

    D3DXVECTOR3 vMove = vEnd - vStart;

    D3DXVec3Normalize(&vMove, &vMove);

    D3DXVec3Scale(&vMove, &vMove, fDT * 1.5f);

    if (pEGM->m_fCameraMoveTimer < 4.0f) {
      pEGM->m_pPlayerObject->GetTransform()->TranslateFrame(vMove);
      pEGM->m_fCameraMoveTimer += fDT;
    }
  } else {
    D3DXVECTOR3 vStart(
      pEGM->m_pWinnerObject->GetTransform()->GetLocalMatrix()._41,
      pEGM->m_pWinnerObject->GetTransform()->GetLocalMatrix()._42,
      pEGM->m_pWinnerObject->GetTransform()->GetLocalMatrix()._43);

    D3DXVECTOR3 vEnd(vStart.x, vStart.y, vStart.z - 6.0f);

    D3DXVECTOR3 vMove = vEnd - vStart;

    D3DXVec3Normalize(&vMove, &vMove);

    D3DXVec3Scale(&vMove, &vMove, fDT * 1.5f);
    if (pEGM->m_fCameraMoveTimer < 2.5f) {
      pEGM->m_pPlayerObject->GetTransform()->TranslateFrame(vMove);
      pEGM->m_fCameraMoveTimer += fDT;
    }
  }
  if (!pEGM->m_bVictoryPlayed && pEGM->m_bHumanWon) {
    switch (CHUDManager::GetPlayer1Char()) {
    case BIKER_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(BULLDOG_VICTORY, GLOBAL_ID);
      break;
    }
    case BANDITOS_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(BANDITOS_VICTORY, GLOBAL_ID);
      break;
    }
    case LARPERS_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(LARPER_VICTORY, GLOBAL_ID);
      break;
    }
    case SCIENTIST_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(SCIENTIST_VICTORY, GLOBAL_ID);
      break;
    }
    case SASHA_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(SASHA_WIN, GLOBAL_ID);
      break;
    }
    case CRYGAME_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(CRYGAME_VICTORY, GLOBAL_ID);
      break;
    }
    case STORYTIME_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(STORYTIME_VICTORY, GLOBAL_ID);
      break;
    }
    }

    pEGM->m_bVictoryPlayed = true;
  }
  if (!pEGM->m_bVictoryPlayed && !pEGM->m_bHumanWon) {
    switch (CHUDManager::GetPlayer1Char()) {
    case BIKER_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(BULLDOG_LOSS, GLOBAL_ID);
      break;
    }
    case BANDITOS_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(BANDITOS_LOSS, GLOBAL_ID);
      break;
    }
    case LARPERS_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(LARPER_LOSS, GLOBAL_ID);
      break;
    }
    case SCIENTIST_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(SCIENTIST_LOSS, GLOBAL_ID);
      break;
    }
    case SASHA_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(SASHA_LOSE, GLOBAL_ID);
      break;
    }
    case CRYGAME_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(CRYGAME_LOSS, GLOBAL_ID);
      break;
    }
    case STORYTIME_CHARACTER: {
      CWwiseSoundManager::PlayTheSound(STORYTIME_LOSS, GLOBAL_ID);
      break;
    }
    }
    pEGM->m_bVictoryPlayed = true;
  }
  //////////////////////////////////////////////////////////////////////////////////
  // Tell the animations to animate, sending win anim to win obj and lose anim
  // to lose obj
  SendInputEvent("TigerBlood", (IComponent*)pEGM, pEGM->m_pWinnerObject, 0.0f);
  SendInputEvent("GoodDaySir", (IComponent*)pEGM, pEGM->m_pLoserObject1, 0.0f);
  SendInputEvent("GoodDaySir", (IComponent*)pEGM, pEGM->m_pLoserObject2, 0.0f);
  SendInputEvent("GoodDaySir", (IComponent*)pEGM, pEGM->m_pLoserObject3, 0.0f);
}

void endgame_manager::WinGameCallback(IEvent* pEvent, IComponent*) {
  endgame_manager* pEGM = GetInstance();

  if (pEGM->m_bGameWon)
    return;                // somebody already won this round
  pEGM->m_bGameWon = true; // somebody just won this round

  TInputEvent* tData = (TInputEvent*)pEvent->GetData();
  // Get the player who won from the event
  // store that as the winning player
  pEGM->m_pWinnerObject = tData->m_pPlayer;
  // Send the input event passing the player object who is winning

  // set the losing players to the rest of the players
  CObject* pThePlayers[4];
  pThePlayers[0] = CObjectManager::GetObjectByName("Player0");
  pThePlayers[1] = CObjectManager::GetObjectByName("Player1");
  pThePlayers[2] = CObjectManager::GetObjectByName("Player2");
  pThePlayers[3] = CObjectManager::GetObjectByName("Player3");

  // set our pointers to the proper objects
  if (pEGM->m_pWinnerObject == pThePlayers[0]) {
    pEGM->m_pLoserObject1 = pThePlayers[1];
    pEGM->m_pLoserObject2 = pThePlayers[2];
    pEGM->m_pLoserObject3 = pThePlayers[3];
    SendIEvent(
      "ItemUnlocked", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);
  } else if (pEGM->m_pWinnerObject == pThePlayers[1]) {
    pEGM->m_pLoserObject1 = pThePlayers[0];
    pEGM->m_pLoserObject2 = pThePlayers[2];
    pEGM->m_pLoserObject3 = pThePlayers[3];
    SendIEvent(
      "ItemUnlocked", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);
  } else if (pEGM->m_pWinnerObject == pThePlayers[2]) {
    pEGM->m_pLoserObject1 = pThePlayers[0];
    pEGM->m_pLoserObject2 = pThePlayers[1];
    pEGM->m_pLoserObject3 = pThePlayers[3];
    SendIEvent(
      "ItemUnlocked", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);
  } else {
    pEGM->m_pLoserObject1 = pThePlayers[0];
    pEGM->m_pLoserObject2 = pThePlayers[1];
    pEGM->m_pLoserObject3 = pThePlayers[2];
  }
  pEGM->m_pPlayerObject->GetTransform()->GetLocalMatrix()
    = pThePlayers[0]->GetTransform()->GetLocalMatrix();
  m_bExitingGameplay = true;

  // Make the fade thingy show
  pEGM->m_pFadeScreenComp->SetActive(true);

  pEGM->m_bHumanWon = (pThePlayers[0] == pEGM->m_pWinnerObject);

  if (pEGM->m_bHumanWon) {
    pEGM->m_pFinishFlag->SetActive(true);
  }

  /*if (!pEGM->m_bSoundPlayed && pEGM->m_bHumanWon)
  {
  CWwiseSoundManager::PlayTheSound(GAMEPLAY_WIN, GLOBAL_ID);
  pEGM->m_bSoundPlayed = true;
  }
  if (!pEGM->m_bSoundPlayed && !pEGM->m_bHumanWon)
  {
  CWwiseSoundManager::PlayTheSound(GAMEPLAY_LOSE, GLOBAL_ID);
  pEGM->m_bSoundPlayed = true;
  }*/

  SendIEvent("WonGame", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);
}

void endgame_manager::WinStateExitCallback(IEvent*, IComponent*) {
  // Get Singletons
  endgame_manager* pEGM = endgame_manager::GetInstance();
  if (!CUnlockableManager::GetPlayerNotified()) {
    pEGM->m_pFont->SetX(1000);
    pEGM->m_pFont->SetY(1000);
    pEGM->m_pFont->SetActive(false);
    CUnlockableManager::SetPlayerNotified(true);
  }

  // Turn off the Sprites
  pEGM->m_pFadeScreenComp->SetActive(false);
  pEGM->m_pMainMenuComp->SetActive(false);
  pEGM->m_pWinLoseComp->SetActive(false);
  pEGM->m_bSoundPlayed   = false;
  pEGM->m_bVictoryPlayed = false;
  pEGM->m_bGameWon       = false;
  CWwiseSoundManager::PlayTheSound(STOP_ALL_EXCEPT, GLOBAL_ID);
}

void endgame_manager::EnterPressedCallback(IEvent*, IComponent*) {
  // Get Singletons
  endgame_manager* pEGM = endgame_manager::GetInstance();

  // if we've hit enter and we're in the state that this listens for, change
  // states to menu state
  SendStateEvent(
    "StateChange", (IComponent*)pEGM, MAIN_MENU_STATE, PRIORITY_IMMEDIATE);
}
