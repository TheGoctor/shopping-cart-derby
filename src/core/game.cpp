#include "Game.h"

#include "Settings.h"

#include <Magnum\GL\DefaultFramebuffer.h>

#include <iostream>

// scd::game::game(int argc, char** argv) {
//  settings _settings;
//
//  _settings.load_cli(argc, argv);
//
//  // TODO: other init code
//}

scd::game::game(const Arguments& arguments)
  : Magnum::Platform::Application{arguments} {
  // Rand
  srand(timeGetTime()); // GetTickCount());

  // Init Managers
  CMemoryManager::GetInstance()->Initialize(50 MB, 5 MB, 20 MB, 75 MB);
  CInputManager::GetInstance()->Initialize(hWnd, hInstance);
  CStateManager::GetInstance()->Init();

  m_pRenderer = Renderer::GetInstance();
  m_pRenderer->Init(hWnd, nScreenWidth, nScreenHeight, bIsWindowed);
  m_pRenderer->LoadModels();

  m_pSound = CWwiseSoundManager::GetInstance();
  m_pSound->InitSoundManager();
  m_pSound->RegisterObject(GLOBAL_ID);
  m_pSound->RegisterObject(BIKER_ID);
  m_pSound->RegisterObject(LARPER_ID);
  m_pSound->RegisterObject(SASHA_ID);
  m_pSound->RegisterObject(BANDITOS_ID);
  m_pSound->RegisterObject(SCIENTIST_ID);
  m_pSound->RegisterObject(CRYGAME_ID);
  m_pSound->RegisterObject(STORYTIME_ID);
  m_pSound->RegisterObject(ITEM_ID);
  m_pEM = CEventManager::GetInstance();

  CCollisionManager::GetInstance()->Init();
  ModelManager::GetInstance()->Init();

  CAIManager::GetInstance()->Init();
  // Place this where you want to initialize Lua
  CConsoleManager::GetInstance().Initialize();

  scd::objectManager::GetInstance();
  CMovementManager::GetInstance();

  // Inventory
  CInventoryManager::GetInstance()->Init();

  // HUD
  CIntroManager::GetInstance()->Init();
  CCreditManager::GetInstance()->Init();
  // CCharacterSelectManager::GetInstance()->Init();
  CHUDManager::GetInstance()->Init();
  CLevelManager::GetInstance()->Init();

  // level manager just made a whoooole bunch of boxes, I'm gonna fix this, HAS
  // TO BE AFTER LEVEL MANAGER INIT!!!!!!!!! -Raymoney
  CCollisionManager::GetInstance()->CombineLinedBoxes();

  CStartOfRaceManager::GetInstance(); // create the mgr
  CSpawningManager::GetInstance()->Init();
  CPickupItemManager::GetInstance()->Init();
  CHeldItemManager::GetInstance()->Init();

  CAnimateManager::GetInstance()->Init();

  CUnlockableManager::GetInstance()->Init();
  CEndgameManager::GetInstance()->Init();

  // set the screen parameters
  SetScreenWidth(nScreenWidth);
  SetScreenHeight(nScreenHeight);
  SetIsWindowed(bIsWindowed);

  // Initialize Effect Manager
  CEffectManager::GetInstance()->Init();

  // init keybinds manager
  CKeybindsManager::GetInstance()->Init();

  // Register for Events
  m_pEM->RegisterEvent(
    "ShutdownGame", (scd::base_component*)GetInstance(), ShutdownCallback);

  _prev_time = _timer.now()
}

scd::game::~game() {
  // Unregister Events

  m_pEM->ClearEvents();
  SendIEvent("Shutdown", (scd::base_component*)GetInstance(), NULL, PRIORITY_IMMEDIATE);

  // Shutdown Managers
  if (CInputManager::GetInstance())
    CInputManager::GetInstance()->Shutdown();

  m_pSound->GetInstance()->ShutdownManager();
  if (m_pEM)
    m_pEM->Shutdown();
  m_pEM = NULL;

  m_bShutdown = false;
}

int scd::game::main() {
  // Time
  m_pSound->Update();
  auto start_time = _timer.now();
  _delta_time     = (float)(start_time - _prev_time) / 1000.0f;
  _prev_time      = start_time;

  ++_frame_count;
  _frame_time += _delta_time;

  // Cap the dt
  if (_delta_time > .2f) {
    _delta_time = .2f;
  }

  if (_frame_time >= 1) {
    _fps         = _frame_count;
    _frame_time  = 0;
    _frame_count = 0;
    std::cout << "FPS: " << _fps << std::endl;
  }

  // Input
  SendIEvent("GetInput", (scd::component*)GetInstance(), NULL, PRIORITY_INPUT);

  // Check for Lost Device
  Direct3DManager::GetInstance()->CheckForLostDevice();

  // Update
  SendUpdateEvent("UpdateState", (scd::base_component*)GetInstance(), m_fDT);

  // Collision Test
  SendIEvent("Collision", (scd::base_component*)GetInstance(), NULL, PRIORITY_COLLISION);

  // Render
  SendIEvent("Render", (scd::base_component*)GetInstance(), NULL, PRIORITY_RENDER);

  // Process Events
  m_pEM->ProcessEvents();

  return 0;
}

void scd::game::drawEvent() {
  Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

  // TODO: Add your drawing code here

  swapBuffers();
}
