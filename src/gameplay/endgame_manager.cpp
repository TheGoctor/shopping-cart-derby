#include "endgame_manager.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/rendering/sprite.h"
#include "core/object.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "gameplay/unlockable_manager.h"
#include "rendering/hud_manager.h"
#include "rendering/texture_manager.h"

#include <functional>

namespace scd {
// Constructor
endgame_manager::endgame_manager() {
  // Register for Events
  event_manager* pEM;

  pEM->register_event("UpdateGameplay", std::bind(&on_gameplay_update, this));

  szEventName = "InitObjects";
  szEventName += GAMEPLAY_STATE;
  pEM->register_event(szEventName, std::bind(&on_gameplay_init, this));

  szEventName = "InitObjects";
  szEventName += WIN_STATE;
  pEM->register_event(szEventName, std::bind(&on_win_init, this));

  szEventName = "Update";
  szEventName += WIN_STATE;
  pEM->register_event(szEventName, std::bind(&on_win_update, this));

  pEM->register_event("GameWon", std::bind(&on_game_win, this));

  szEventName = "ShutdownObjects";
  szEventName += WIN_STATE;
  pEM->register_event(szEventName, std::bind(&on_win_state_exit, this));

  pEM->register_event("AcceptWinState", std::bind(&on_enter_pressed, this));

  // Create object
  object_manager* pOM;

  _player = pOM->create("InvisObject");

  texture_manager* pTM;
  /// Setup sprite data
  sprite_data sprite{};
  sprite.texture_id = pTM->load_texture("Resource/HUD/T_Black_Screen_D.png", 0);
  sprite.scale = {1.0f, 1.0f};
  sprite.x = 0;
  sprite.y = 0;
  sprite.z = 130;
  sprite.rotation_angle = 0.0f;
  sprite.rotation_center = {0.0f, 0.0f};
  sprite.color = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  sprite.rectangle = {};

  auto fade_screen_obj = pOM->create("EndFadeScreen");
  _fade_screen = pTM->create_sprite(*fade_screen_obj, sprite, false);


  sprite = {};

  sprite.m_nTexture =
      pTM->LoadTexture("Resource\\HUD\\T_Endings_Words_D.png", 0);
  sprite.m_fScaleX = 1.0f;
  sprite.m_fScaleY = 1.0f;
  sprite.m_fRotCenterX = 0.0f;
  sprite.m_fRotCenterY = 0.0f;
  sprite.m_fRot = 0.0f;
  sprite.m_nX = 250;
  sprite.m_nY = 30;
  sprite.m_nZ = 131;
  sprite.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  sprite.m_tRect.top = 0;
  sprite.m_tRect.bottom = 145;
  sprite.m_tRect.left = 0;
  sprite.m_tRect.right = 500;

  auto win_text_obj = pOM->create("WinText");
  m_pWinLoseComp = pTM->CreateSpriteComp(pWinTextObj, tSpriteData, false);

  CObject* pMainMenuText = pOM->CreateObject("MainMenuText");

  tSpriteData.m_fScaleX = 1.25f;
  tSpriteData.m_fScaleY = 1.25f;
  tSpriteData.m_tRect.top = 190;
  tSpriteData.m_tRect.left = 10;
  tSpriteData.m_tRect.right = 300;
  tSpriteData.m_tRect.bottom = 265;
  tSpriteData.m_nX = 350;
  tSpriteData.m_nY = 650;

  m_pMainMenuComp = pTM->CreateSpriteComp(pMainMenuText, tSpriteData, false);

  CObject* pFinishFlagPicObj = pOM->CreateObject("FinishFlagPic");

  tSpriteData.m_nTexture =
      pTM->LoadTexture("Resource\\HUD\\T_Finish_Sign_D.png", 0);
  tSpriteData.m_fScaleX = 1.0f;
  tSpriteData.m_fScaleY = 1.0f;
  tSpriteData.m_tRect.top = 0;
  tSpriteData.m_tRect.left = 0;
  tSpriteData.m_tRect.right = 0;
  tSpriteData.m_tRect.bottom = 0;
  tSpriteData.m_nX = 150; // set in update
  tSpriteData.m_nY = 150;

  m_pFinishFlag = pTM->CreateSpriteComp(pFinishFlagPicObj, tSpriteData, false);

  if (!CUnlockableManager::GetPlayerNotified()) {
    CBitmapFont font(-1, 13, 15, 33);
    font.LoadFont("Resource\\BitmapFont.png", "Resource\\BitmapFont_Width.bin");
    float fScale = 5.0f;
    m_nFontX = -100;
    m_nFontY = 740;
    scd::vector4 fontColor(1.0f, 1.0f, 1.0f, 255.0f);
    std::string szUnlockMsg = "New Shoppers Unlocked!";
    CObject* pFontObj = CObjectManager::CreateObject("UnlockMsgObj");
    m_pFont = CTextureManager::CreateBitmapFontComp(
        pFontObj,
        szUnlockMsg,
        font,
        m_nFontX,
        m_nFontY,
        fScale,
        fontColor,
        false);
    // m_pFont->DrawWord();
    m_pFont->SetX(m_nFontX);
    m_pFont->SetY(m_nFontY);
  }
}

// Helper funcs
void endgame_manager::ResetSprites(void) {
  // Update the Fade Screen Sprite
  sprite_data pSData = m_pFadeScreenComp->GetSpriteData();
  pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  m_pFadeScreenComp->SetSpriteData(pSData);

  // Update the Win/Lose Sprite
  pSData = m_pWinLoseComp->GetSpriteData();
  pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  m_pWinLoseComp->SetSpriteData(pSData);

  // Update the Main Menu Sprite
  pSData = m_pMainMenuComp->GetSpriteData();
  pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 0.0f);
  m_pMainMenuComp->SetSpriteData(pSData);

  pSData = m_pFinishFlag->GetSpriteData();
  pSData.m_fScaleX = pSData.m_fScaleY = 0.0f;
  m_pFinishFlag->SetSpriteData(pSData);
}

void endgame_manager::on_gameplay_update(float dt) {
  // Check to see if Gameplay is exiting
  if (m_bExitingGameplay) {
    // Increase Fade
    _fFadeScreenAlpha += dt * FADE_ALPHA_RATE;
    if (_fFadeScreenAlpha > 1.0f) {
      // Change States
      SendStateEvent(
          "PushState", (IComponent*)GetInstance(), WIN_STATE, PRIORITY_NORMAL);

      // Reset Values
      _bExitingGameplay = false;
      _fFadeScreenAlpha = 1.0f;
    }

    // Update the Sprite
    sprite_data pSData = _pFadeScreenComp->GetSpriteData();
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, _fFadeScreenAlpha);
    _pFadeScreenComp->SetSpriteData(pSData);

    // Do stuff for the finish flag showing up and stuff
    if (_bHumanWon
        && _fFadeScreenAlpha
               <= .25f) // only do for half the fade time. this makes it go
                        // faster because normally we'd just sin(x*.5) but that
                        // makes it too slow
    {
      sprite_data pSData1 = _pFinishFlag->GetSpriteData();
      pSData1.m_fScaleX = sin(_fFadeScreenAlpha * 3.1415f * 2.0f);
      pSData1.m_fScaleY = pSData.m_fScaleX;
      pSData1.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);

      pSData1.m_nX =
          325 - (int)(sin(_fFadeScreenAlpha * 3.1415) * 110.0f); // sin()*VAL
                                                                 // where VAL
                                                                 // = range
                                                                 // at which
                                                                 // it moves

      _pFinishFlag->SetSpriteData(pSData1);
    }
  }
}

void endgame_manager::on_gameplay_init() {
  // reset values so when game restarts, this stuff works fine
  _bExitingGameplay = false;
  _fFadeScreenAlpha = 0.0f;
  _fWinLoseAlpha = 0.0f;
  _pWinnerObject = NULL;
  _pLoserObject1 = NULL;
  _pLoserObject2 = NULL;
  _fCameraMoveTimer = 0.0f;
  _pLoserObject3 = NULL;
  _bLoseFade = false;
  _bCameraMoved = false;
  _fLoseFadeTimer = 0.0f;
  _fCameraTime = 0.0f;
  ResetSprites();

  // set all the components to invisible false
  _pFadeScreenComp->SetActive(false);
  _pMainMenuComp->SetActive(false);
  _pWinLoseComp->SetActive(false);
  _pFinishFlag->SetActive(false);
}

void endgame_manager::on_win_init() {
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
    TSpriteData pSData = pEGM->m_pWinLoseComp->GetSpriteData();
    pSData.m_tRect.left = 0;
    pSData.m_tRect.top = 0;
    pSData.m_tRect.right = 500;
    pSData.m_tRect.bottom = 145;
    pEGM->m_pWinLoseComp->SetSpriteData(pSData);
    if (!CUnlockableManager::GetPlayerNotified()) {
      CBitmapFont font(-1, 13, 15, 33);
      font.LoadFont(
          "Resource\\BitmapFont.png", "Resource\\BitmapFont_Width.bin");
      float fScale = 2.0f;
      //		pEGM->m_nFontX+=100.0f*fDT;
      scd::vector4 fontColor(
          RAND_FLOAT(0.0f, 1.0f),
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
    TSpriteData pSData = pEGM->m_pWinLoseComp->GetSpriteData();
    pSData.m_tRect.left = 0;
    pSData.m_tRect.top = 280;
    pSData.m_tRect.right = 500;
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
    pEGM->m_pPlayerObject->GetTransform()->GetLocalMatrix() =
        pEGM->m_pWinnerObject->GetTransform()->GetLocalMatrix();
    SendObjectEvent(
        "AttachToCameraWin", (IComponent*)GetInstance(), pEGM->m_pPlayerObject);
    // TODO: Do confetti here
    SendIEvent("HumanWinEffect", (IComponent*)pEGM, NULL, PRIORITY_NORMAL);
  } else {
    pEGM->m_pPlayerObject->GetTransform()->GetLocalMatrix() =
        pEGM->m_pLoserObject1->GetTransform()->GetLocalMatrix();
    SendObjectEvent(
        "AttachToCameraLose",
        (IComponent*)GetInstance(),
        pEGM->m_pPlayerObject);
  }

  CWwiseSoundManager::PlayMusic(MENU_MUSIC_PLAY, GLOBAL_ID);
  //
}

void endgame_manager::on_win_update(float dt) {
  if (!CUnlockableManager::GetPlayerNotified()) {
    bitmap_font font(-1, 13, 15, 33);
    font.load_font("Resource/BitmapFont.png", "Resource/BitmapFont_Width.bin");
    _font_position[0] += 80 * dt;

    if (_font_position[0] >= 1024)
      _font_position[0] = -480;

    _font_position[1] = 740;

    scd::vector4 font_color(
        RAND_FLOAT(0.0f, 1.0f),
        RAND_FLOAT(0.0f, 1.0f),
        RAND_FLOAT(0.0f, 1.0f),
        255.0f);

    std::string unlock_message = "New Shoppers Unlocked!";

    _font->SetWord(unlock_message);
    _font->SetX(_font_position[0]);
    _font->SetY(_font_position[1]);
    _font->SetColor(font_color);
    _font->SetIsActive(true);
    _font->DrawWord();
  }

  // Decrease Fade
  if (_bLoseFade == false && _fLoseFadeTimer == 0.0f) {
    _fFadeScreenAlpha -= fDT * FADE_ALPHA_RATE;
    if (_fFadeScreenAlpha < 0.0f) {
      _fFadeScreenAlpha = 0.0f;

      if (_bHumanWon == false) {
        _fLoseFadeTimer = LOSE_FADE_TIME;
      }
    }

    // Update the Fade Screen Sprite
    sprite_data pSData = _pFadeScreenComp->GetSpriteData();
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, _fFadeScreenAlpha);
    _pFadeScreenComp->SetSpriteData(pSData);
  }

  // Decrease Lose Fade Time
  if (_fLoseFadeTimer > 0.0f) {
    _fLoseFadeTimer -= dt;
    if (_fLoseFadeTimer <= 0.0f) {
      _bLoseFade = true;
    }
  }

  // Increase Win/Lose Fade
  if (_fFadeScreenAlpha == 0.0f) {
    _fWinLoseAlpha += fDT * FADE_ALPHA_RATE;
    if (_fWinLoseAlpha > 1.0f) {
      _fWinLoseAlpha = 1.0f;
    }

    // Update the Win/Lose Sprite
    TSpriteData pSData = _pWinLoseComp->GetSpriteData();
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, _fWinLoseAlpha);
    _pWinLoseComp->SetSpriteData(pSData);

    // Update the Main Menu Sprite
    pSData = _pMainMenuComp->GetSpriteData();
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, _fWinLoseAlpha);
    _pMainMenuComp->SetSpriteData(pSData);
  }

  // Lose Fade
  if (_bLoseFade == true) {
    _fFadeScreenAlpha += fDT * FADE_ALPHA_RATE;
    if (_fFadeScreenAlpha > 0.5f) {
      _fFadeScreenAlpha = 0.5f;
    }

    // Update the Fade Screen Sprite
    TSpriteData pSData = _pFadeScreenComp->GetSpriteData();
    // pSData.m_nZ = 110;
    pSData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, _fFadeScreenAlpha);
    _pFadeScreenComp->SetSpriteData(pSData);
  }

  if (!_bHumanWon) {
    //////////////////On Lose, the camera starts at the player and slowly moves
    /// back
    D3DXVECTOR3 vStart(
        _pLoserObject1->GetTransform()->GetLocalMatrix()._41,
        _pLoserObject1->GetTransform()->GetLocalMatrix()._42,
        _pLoserObject1->GetTransform()->GetLocalMatrix()._43);

    D3DXVECTOR3 vEnd(vStart.x - 2.0f, vStart.y + 1.0f, vStart.z - 10.0f);

    D3DXVECTOR3 vMove = vEnd - vStart;

    D3DXVec3Normalize(&vMove, &vMove);

    D3DXVec3Scale(&vMove, &vMove, fDT * 1.5f);

    if (_fCameraMoveTimer < 4.0f) {
      _pPlayerObject->GetTransform()->TranslateFrame(vMove);
      _fCameraMoveTimer += fDT;
    }
  } else {
    D3DXVECTOR3 vStart(
        _pWinnerObject->GetTransform()->GetLocalMatrix()._41,
        _pWinnerObject->GetTransform()->GetLocalMatrix()._42,
        _pWinnerObject->GetTransform()->GetLocalMatrix()._43);

    D3DXVECTOR3 vEnd(vStart.x, vStart.y, vStart.z - 6.0f);

    D3DXVECTOR3 vMove = vEnd - vStart;

    D3DXVec3Normalize(&vMove, &vMove);

    D3DXVec3Scale(&vMove, &vMove, fDT * 1.5f);
    if (_fCameraMoveTimer < 2.5f) {
      _pPlayerObject->GetTransform()->TranslateFrame(vMove);
      _fCameraMoveTimer += fDT;
    }
  }
  if (!_bVictoryPlayed && _bHumanWon) {
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

    _bVictoryPlayed = true;
  }
  if (!_bVictoryPlayed && !_bHumanWon) {
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
    _bVictoryPlayed = true;
  }
  //////////////////////////////////////////////////////////////////////////////////
  // Tell the animations to animate, sending win anim to win obj and lose anim
  // to lose obj
  SendInputEvent("TigerBlood", _pWinnerObject, 0.0f);
  SendInputEvent("GoodDaySir", _pLoserObject1, 0.0f);
  SendInputEvent("GoodDaySir", _pLoserObject2, 0.0f);
  SendInputEvent("GoodDaySir", _pLoserObject3, 0.0f);
}

void endgame_manager::on_game_win(IEvent* pEvent) {
  if (_GameWon)
    return;         // somebody already won this round
  _bGameWon = true; // somebody just won this round

  TInputEvent* tData = (TInputEvent*)pEvent->GetData();
  // Get the player who won from the event
  // store that as the winning player
  _pWinnerObject = tData->m_pPlayer;
  // Send the input event passing the player object who is winning

  // set the losing players to the rest of the players
  object* pThePlayers[4];
  pThePlayers[0] = CObjectManager::GetObjectByName("Player0");
  pThePlayers[1] = CObjectManager::GetObjectByName("Player1");
  pThePlayers[2] = CObjectManager::GetObjectByName("Player2");
  pThePlayers[3] = CObjectManager::GetObjectByName("Player3");

  // set our pointers to the proper objects
  if (_pWinnerObject == pThePlayers[0]) {
    _pLoserObject1 = pThePlayers[1];
    _pLoserObject2 = pThePlayers[2];
    _pLoserObject3 = pThePlayers[3];
    SendIEvent("ItemUnlocked", PRIORITY_NORMAL);
  } else if (_pWinnerObject == pThePlayers[1]) {
    _pLoserObject1 = pThePlayers[0];
    _pLoserObject2 = pThePlayers[2];
    _pLoserObject3 = pThePlayers[3];
    SendIEvent("ItemUnlocked", PRIORITY_NORMAL);
  } else if (_pWinnerObject == pThePlayers[2]) {
    _pLoserObject1 = pThePlayers[0];
    _pLoserObject2 = pThePlayers[1];
    _pLoserObject3 = pThePlayers[3];
    SendIEvent("ItemUnlocked", PRIORITY_NORMAL);
  } else {
    _pLoserObject1 = pThePlayers[0];
    _pLoserObject2 = pThePlayers[1];
    _pLoserObject3 = pThePlayers[2];
  }
  _pPlayerObject->GetTransform()->GetLocalMatrix() =
      pThePlayers[0]->GetTransform()->GetLocalMatrix();
  m_bExitingGameplay = true;

  // Make the fade thingy show
  _pFadeScreenComp->SetActive(true);

  _bHumanWon = (pThePlayers[0] == _pWinnerObject);

  if (_bHumanWon) {
    _pFinishFlag->SetActive(true);
  }

  SendIEvent("WonGame", (IComponent*)GetInstance(), nullptr, PRIORITY_NORMAL);
}

void endgame_manager::on_win_state_exit() {
  if (!CUnlockableManager::GetPlayerNotified()) {
    _font->SetX(1000);
    _font->SetY(1000);
    _font->SetActive(false);
    CUnlockableManager::SetPlayerNotified(true);
  }

  // Turn off the Sprites
  _FadeScreenComp->SetActive(false);
  _MainMenuComp->SetActive(false);
  _WinLoseComp->SetActive(false);
  _SoundPlayed = false;
  _VictoryPlayed = false;
  _GameWon = false;
  CWwiseSoundManager::PlayTheSound(STOP_ALL_EXCEPT, GLOBAL_ID);
}

void endgame_manager::EnterPressedCallback() {
  // if we've hit enter and we're in the state that this listens for, change
  // states to menu state
  SendStateEvent(
      "StateChange", (IComponent*)pEGM, MAIN_MENU_STATE, PRIORITY_IMMEDIATE);
}
} // namespace scd
