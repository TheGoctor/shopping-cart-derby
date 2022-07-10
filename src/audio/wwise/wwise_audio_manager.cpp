#if 0
////////////////////////////////////////////////////////
//  File			:	"CWwiseSoundManager.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	3/27/2011
//
//	Last Changed	:	7/22/2001
//
//	Changed By		:	HN
//
//  Purpose			:	Wrapper class for Wwise API
//
////////////////////////////////////////////////////////
#include "wwise_audio_manager.h"

#include "audio/wwise/includes/akfilepackagelowlevelioblocking.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "rendering/hud_manager.h"

#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>

#ifdef _DEBUG
#include <AK/Comm/AkCommunication.h>
#endif

// Plugins
#include "audio/wwise/Plugin/AkAudioInputSourceFactory.h"
#include "audio/wwise/Plugin/AkDelayFXFactory.h"
#include "audio/wwise/Plugin/AkParametricEQFXFactory.h"
#include "audio/wwise/Plugin/AkPeakLimiterFXFactory.h"
#include "audio/wwise/Plugin/AkRoomVerbFXFactory.h"
#include "audio/wwise/Plugin/AkSoundSeedImpactFXFactory.h"
#include "audio/wwise/Plugin/AkVorbisFactory.h"

#include <iostream>

#define SOUND_VOLUME_DEFAULT 50.0f
#define MUSIC_VOLUME_DEFAULT 50.0f
#define DX_VOLUME_DEFAULT 75.0f
////////////////////////////////////////////////////////////////////////////////
// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined
////////////////////////////////////////////////////////////////////////////////
namespace AK {
#ifdef WIN32
void* AllocHook(size_t in_size) { return malloc(in_size); }
void FreeHook(void* in_ptr) { free(in_ptr); }
// Note: VirtualAllocHook() may be used by I/O pools of the default
// implementation of the Stream Manager, to allow "true" unbuffered I/O (using
// FILE_FLAG_NO_BUFFERING
// - refer to the Windows SDK documentation for more details). This is NOT
// mandatory; you may implement it with a simple malloc().
void* VirtualAllocHook(
    void* in_pMemAddress,
    size_t in_size,
    DWORD in_dwAllocationType,
    DWORD in_dwProtect) {
  return VirtualAlloc(
      in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
}
void VirtualFreeHook(
    void* in_pMemAddress,
    size_t in_size,
    DWORD in_dwFreeType) {
  VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
}
#endif
} // namespace AK

// Constructor
CWwiseSoundManager::CWwiseSoundManager()
    : m_pObjectID(100) {}
// Deconstuctor
CWwiseSoundManager::~CWwiseSoundManager() {}
////////////////////////////////////////////////////////////////////////////////
// Returns a single instance of the manager
//////////////////////////////////////////////////////////////////////////////////
CWwiseSoundManager* CWwiseSoundManager::GetInstance() {
  static CWwiseSoundManager cWwiseSoundManager;
  return &cWwiseSoundManager;
}
////////////////////////////////////////////////////////////////////////////////
// Properly initializes the manager and loads in the BNK files need to play
// sound sets the default volumes for sound and music
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::InitSoundManager() {
  InitWise();
  InitBanks();
  LoadBank(INIT);
  LoadBank(SOUNDBANK);
  RegisterForEvents();

  m_fDefSound = SOUND_VOLUME_DEFAULT;
  m_fDefMusic = MUSIC_VOLUME_DEFAULT;
  m_fDefDXSound = DX_VOLUME_DEFAULT;

  m_bGameplayEntered = false;

  SetSoundVolume(m_fDefSound);
  SetMusicVolume(m_fDefMusic);
  SetDXVolume(m_fDefDXSound);
}
////////////////////////////////////////////////////////////////////////////////
// Calls RenderAudio to go through the sound events that needed to be played,
// must be called in update every frame
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::Update() { AK::SoundEngine::RenderAudio(); }
////////////////////////////////////////////////////////////////////////////////
// Calls functions to shutdown the manager correctly
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::ShutdownManager() {
  StopAll();
  UnloadAll();
  ShutdownWise();
}
////////////////////////////////////////////////////////////////////////////////
// Plays the sound associated with the object
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::PlayTheSound(SoundID soundid, int object) {
  AkGameObjectID obj = object;
  AkUniqueID eventID = soundid;
  AK::SoundEngine::PostEvent(eventID, obj);
}
void CWwiseSoundManager::PlayMusic(SoundID soundid, int object) {
  AkGameObjectID obj = object;
  AkUniqueID eventID = soundid;
  AK::SoundEngine::PostEvent(eventID, obj);
}
////////////////////////////////////////////////////////////////////////////////
// Pauses all the sounds
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::PauseAll() {
  AK::SoundEngine::PostEvent(AK::EVENTS::PAUSE_ALL, 0);
}
////////////////////////////////////////////////////////////////////////////////
// Reumses the sound
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::ResumeAll() {
  AK::SoundEngine::PostEvent(AK::EVENTS::RESUME_ALL, 0);
}
////////////////////////////////////////////////////////////////////////////////
// Stops all the sounds in the game
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::StopAll() {
  AK::SoundEngine::PostEvent(AK::EVENTS::STOP_ALL, 1);
}
////////////////////////////////////////////////////////////////////////////////
// Sets the scale of the object
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::SetScale(int object, float scale) {
  AkGameObjectID obj = object;
  AK::SoundEngine::SetAttenuationScalingFactor(obj, scale);
}
////////////////////////////////////////////////////////////////////////////////
// Sets the Music volume
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::SetMusicVolume(float volume) {
  m_fMusicVolume = volume;
  if (m_fMusicVolume > 100.0f) {
    m_fMusicVolume = 100.0f;
  } else if (m_fMusicVolume < 0.0f) {
    m_fMusicVolume = 0.0f;
  }
  AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::MX_VOLUME, m_fMusicVolume);
}

////////////////////////////////////////////////////////////////////////////////
// Sets the SFX volume
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::SetSoundVolume(float volume) {
  m_fSoundVolume = volume;

  if (m_fSoundVolume > 100.0f) {
    m_fSoundVolume = 100.0f;
  } else if (m_fSoundVolume < 0.0f) {
    m_fSoundVolume = 0.0f;
  }
  AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::FX_VOLUME, m_fSoundVolume);
}
////////////////////////////////////////////////////////////////////////////////
// Sets the DX Volume
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::SetDXVolume(float volume) {
  m_fDXvolume = volume;
  AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::DX_VOLUME, m_fDXvolume);
}
////////////////////////////////////////////////////////////////////////////////
// Sets the RPM value
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::SetRPMValueForSound(float rpm, int object) {
  m_fRPM = rpm;
  AkGameObjectID obj = object;
  AK::SoundEngine::SetRTPCValue(
      AK::GAME_PARAMETERS::SPEED, (AkRtpcValue)m_fRPM, obj);
}

////////////////////////////////////////////////////////////////////////////////
// Sets the listers position, if the listener moves then call this in the
// listeners update
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::SetListenerPosition(
    D3DXVECTOR3 position,
    D3DXVECTOR3 forwardvec,
    float scale,
    int listener) {
  AkListenerPosition pos;
  pos.Position.X = position.x;
  pos.Position.Y = position.y;
  pos.Position.Z = position.z;

  pos.OrientationFront.X = forwardvec.x;
  pos.OrientationFront.Y = forwardvec.y;
  pos.OrientationFront.Z = forwardvec.z;

  pos.OrientationTop.X = 0.0f;
  pos.OrientationTop.Y = 1.0f;
  pos.OrientationTop.Z = 0.0f;

  AK::SoundEngine::SetListenerPosition(pos, listener);
  AK::SoundEngine::SetListenerScalingFactor(0, scale);
}
////////////////////////////////////////////////////////////////////////////////
// Sets an objects position, needs to be called and objects update
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::SetObjectPosition(
    int object,
    D3DXVECTOR3 position,
    float scale) {
  AkSoundPosition pos;
  pos.Position.X = position.x;
  pos.Position.Y = position.y;
  pos.Position.Z = position.z;

  AkGameObjectID obj = object;
  AK::SoundEngine::SetPosition(obj, pos);
  AK::SoundEngine::SetAttenuationScalingFactor(obj, scale);
}

////////////////////////////////////////////////////////////////////////////////
// Registers a game object and all sounds the object needs to use
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::RegisterObject(int object) {
  AkGameObjectID obj = object;
  AK::SoundEngine::RegisterGameObj(obj);
}

int CWwiseSoundManager::RegisterHeldObject() {
  int object = m_pObjectID;
  AK::SoundEngine::RegisterGameObj(m_pObjectID);
  m_pObjectID++;
  return object;
}

////////////////////////////////////////////////////////////////////////////////
// Unregister an object
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::UnregisterObject(int object) {
  AkGameObjectID obj = object;
  AK::SoundEngine::UnregisterGameObj(obj);
}
////////////////////////////////////////////////////////////////////////////////
// Properly start up Wwise
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::InitWise() {
  //////////////////////////////////////////////////////////////////////////
  // The following code was taken from the Wwise documentation
  //////////////////////////////////////////////////////////////////////////
  AkMemSettings memSettings;
  memSettings.uMaxNumPools = 200;

  if (AK::MemoryMgr::Init(&memSettings) != AK_Success) {
    assert(!"Could not create the memory manager.");
  }

  // Create and initialize an instance of the default streaming manager. Note
  // that you can override the default streaming manager with your own. Refer
  // to the SDK documentation for more information.

  AkStreamMgrSettings stmSettings;
  AK::StreamMgr::GetDefaultSettings(stmSettings);

  // Customize the Stream Manager settings here.

  if (!AK::StreamMgr::Create(stmSettings)) {
    assert(!"Could not create the Streaming Manager");
  }

  //
  // Create a streaming device with blocking low-level I/O handshaking.
  // Note that you can override the default low-level I/O module with your own.
  // Refer to the SDK documentation for more information.
  //
  AkDeviceSettings deviceSettings;
  AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

  // Customize the streaming device settings here.

  // CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
  // in the Stream Manager, and registers itself as the File Location Resolver.
  if (g_lowLevelIO.Init(deviceSettings) != AK_Success) {
    assert(!"Could not create the streaming device and Low-Level I/O system");
  }

  // Create the Sound Engine
  // Using default initialization parameters

  AkInitSettings initSettings;
  AkPlatformInitSettings platformInitSettings;
  AK::SoundEngine::GetDefaultInitSettings(initSettings);
  AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

  if (AK::SoundEngine::Init(&initSettings, &platformInitSettings)
      != AK_Success) {
    assert(!"Could not initialize the Sound Engine.");
  }

  // Initialize the music engine
  // Using default initialization parameters

  AkMusicSettings musicInit;
  AK::MusicEngine::GetDefaultInitSettings(musicInit);

  if (AK::MusicEngine::Init(&musicInit) != AK_Success) {
    assert(!"Could not initialize the Music Engine.");
  }

  AK::SoundEngine::SetListenerPipeline(0, true, false);

  //////////////////////////////////////////////////////
  // Connect to the Wwise API for debugging
#ifdef _DEBUG

  AkCommSettings commSettings;
  AK::Comm::GetDefaultInitSettings(commSettings);
  if (AK::Comm::Init(commSettings) != AK_Success) {
    assert(!"Could not initialize communication.");
    // return false;
  }
#endif

  // Regestering the codecs and plugins
  AK::SoundEngine::RegisterPlugin(
      AkPluginTypeEffect,
      AKCOMPANYID_AUDIOKINETIC,
      AKEFFECTID_ROOMVERB,
      CreateRoomVerbFX,
      CreateRoomVerbFXParams);

  AK::SoundEngine::RegisterPlugin(
      AkPluginTypeEffect,
      AKCOMPANYID_AUDIOKINETIC,
      AKEFFECTID_SOUNDSEEDIMPACT,
      CreateAkSoundSeedImpactFX,
      CreateAkSoundSeedImpactFXParams);

  AK::SoundEngine::RegisterPlugin(
      AkPluginTypeEffect,
      AKCOMPANYID_AUDIOKINETIC,
      AKEFFECTID_PARAMETRICEQ,
      CreateParametricEQFX,
      CreateParametricEQFXParams);

  AK::SoundEngine::RegisterPlugin(
      AkPluginTypeEffect,
      AKCOMPANYID_AUDIOKINETIC,
      AKEFFECTID_DELAY,
      CreateDelayFX,
      CreateDelayFXParams);

  AK::SoundEngine::RegisterPlugin(
      AkPluginTypeEffect,
      AKCOMPANYID_AUDIOKINETIC,
      AKEFFECTID_PEAKLIMITER,
      CreatePeakLimiterFX,
      CreatePeakLimiterFXParams);

  AK::SoundEngine::RegisterCodec(
      AKCOMPANYID_AUDIOKINETIC,
      AKCODECID_VORBIS,
      CreateVorbisFilePlugin,
      CreateVorbisBankPlugin);

  AK::SoundEngine::RegisterPlugin(
      AkPluginTypeSource,
      AKCOMPANYID_AUDIOKINETIC,
      AKSOURCEID_AUDIOINPUT,
      CreateAudioInputSource,
      CreateAudioInputSourceParams);

  //////////////////////////////////////////////////////////////////////////
  // The above code was taken from the Wwise documentation
  //////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
// Properly shutdown Wwise
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::ShutdownWise() {
  AK::SoundEngine::UnregisterAllGameObj();

#ifdef _DEBUG

  // Terminate Communication Services

  AK::Comm::Term();
#endif

  // Terminate the music engine
  //
  AK::MusicEngine::Term();

  // Terminate the sound engine
  //
  if (AK::SoundEngine::IsInitialized()) {
    AK::SoundEngine::Term();
  }

  // Terminate the streaming device and streaming manager
  // CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming
  // device that lives in the Stream Manager, and unregisters itself as the File
  // Location Resolver.
  g_lowLevelIO.Term();

  if (AK::IAkStreamMgr::Get())
    AK::IAkStreamMgr::Get()->Destroy();

  // vorbis
  // Terminate the Memory Manager
  AK::MemoryMgr::Term();
}
////////////////////////////////////////////////////////////////////////////////
// Sets the path to the bank files
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::InitBanks() {
  mBanks[INIT].mLocation = "Source/Wwise/BankFiles/Init.bnk";
  mBanks[SOUNDBANK].mLocation = "Source/Wwise/BankFiles/SoundBank.bnk";
}
////////////////////////////////////////////////////////////////////////////////
// Load in the bank files needed to play sound and music
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::LoadBank(BankID bankid) {
  AK::SoundEngine::LoadBank(
      mBanks[bankid].mLocation, AK_DEFAULT_POOL_ID, mBanks[bankid].mID);
  return;
}
////////////////////////////////////////////////////////////////////////////////
// Unloads a specific bank file
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::UnloadBank(BankID bankid) {
  AK::SoundEngine::UnloadBank(mBanks[bankid].mID);
}
////////////////////////////////////////////////////////////////////////////////
// Unload all the bank files
////////////////////////////////////////////////////////////////////////////////
void CWwiseSoundManager::UnloadAll() {
  for (int i = 0; i < MAX_BANKS; ++i) {
    AK::SoundEngine::UnloadBank(mBanks[i].mID);
  }
}

////// Event Callbacks
void CWwiseSoundManager::SetMusicVolumeCallback(
    IEvent* cEvent,
    IComponent* /*cCenter*/) {
  TFloatEvent* tEvent = (TFloatEvent*)cEvent->GetData();
  CWwiseSoundManager::GetInstance()->SetMusicVolume(tEvent->m_fValue);
}

void CWwiseSoundManager::SetSFXVolumeCallback(
    IEvent* cEvent,
    IComponent* /*cCenter*/) {
  TFloatEvent* tEvent = (TFloatEvent*)cEvent->GetData();
  CWwiseSoundManager::GetInstance()->SetSoundVolume(tEvent->m_fValue);
}
// Register for events
void CWwiseSoundManager::RegisterForEvents() {
  CEventManager::GetInstance()->RegisterEvent(
      "SetSoundVolume", (IComponent*)GetInstance(), SetSFXVolumeCallback);
  CEventManager::GetInstance()->RegisterEvent(
      "SetMusicVolume", (IComponent*)GetInstance(), SetMusicVolumeCallback);

  // BGM Handlers

  // Some of these are commented out so that state changes within the menu
  //	don't restart the song
  string szEventNamePlay = "InitObjects";
  string szEventNamePause = "DisableObjects";
  string szEventNameResume = "EnableObjects";
  szEventNamePlay += MAIN_MENU_STATE;
  szEventNamePause += MAIN_MENU_STATE;
  szEventNameResume += MAIN_MENU_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventNamePlay, (IComponent*)GetInstance(), PlayMenuMusic);
  CEventManager::GetInstance()->RegisterEvent(
      "PlayMenuMusic", (IComponent*)GetInstance(), ResumeMenuMusic);

  szEventNamePlay = "InitObjects";
  szEventNamePause = "DisableObjects";
  szEventNameResume = "EnableObjects";
  string szEventNameUpDate = "Update";
  szEventNamePlay += GAMEPLAY_STATE;
  szEventNamePause += GAMEPLAY_STATE;
  szEventNameResume += GAMEPLAY_STATE;
  szEventNameUpDate += GAMEPLAY_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventNamePlay, (IComponent*)GetInstance(), PlayGameplayMusic);
  CEventManager::GetInstance()->RegisterEvent(
      szEventNamePause, (IComponent*)GetInstance(), PauseGameplayMusic);
  CEventManager::GetInstance()->RegisterEvent(
      szEventNameResume, (IComponent*)GetInstance(), ResumeGameplayMusic);
  CEventManager::GetInstance()->RegisterEvent(
      szEventNameUpDate, (IComponent*)GetInstance(), StopMenuMusic);

  szEventNamePlay = "InitObjects";
  szEventNamePause = "ShutdownObjects";
  szEventNameResume = "EnableObjects";
  szEventNamePlay += PAUSE_STATE;
  szEventNamePause += PAUSE_STATE;
  szEventNameResume += PAUSE_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventNamePlay, (IComponent*)GetInstance(), PlayPauseMusic);
  CEventManager::GetInstance()->RegisterEvent(
      szEventNamePause, (IComponent*)GetInstance(), PausePauseMusic);

  szEventNamePlay = "InitObjects";
  szEventNamePause = "DisableObjects";
  szEventNameResume = "EnableObjects";
  szEventNamePlay += GAME_MODE_SELECT_STATE;
  szEventNamePause += GAME_MODE_SELECT_STATE;
  szEventNameResume += GAME_MODE_SELECT_STATE;

  szEventNamePlay = "InitObjects";
  szEventNamePause = "DisableObjects";
  szEventNameResume = "EnableObjects";
  szEventNamePlay += OPTIONS_STATE;
  szEventNamePause += OPTIONS_STATE;
  szEventNameResume += OPTIONS_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventNamePlay, (IComponent*)GetInstance(), PlayMenuMusic);

  szEventNamePlay = "InitObjects";
  szEventNamePause = "DisableObjects";
  szEventNameResume = "EnableObjects";
  szEventNamePlay += WIN_STATE;
  szEventNamePause += WIN_STATE;
  szEventNameResume += WIN_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventNamePlay, (IComponent*)GetInstance(), PlayMenuMusic);
  CEventManager::GetInstance()->RegisterEvent(
      szEventNameResume, (IComponent*)GetInstance(), ResumeMenuMusic);
}

// Music Callbacks
void CWwiseSoundManager::PlayMenuMusic(IEvent*, IComponent*) {
  if (GetInstance()->m_bGameplayEntered) {
    // us pushed on to state stack (InitObjects)
    GetInstance()->PlayMusic(MENU_MUSIC_PLAY, GLOBAL_ID);
    GetInstance()->PlayTheSound(STOP_ALL_EXCEPT, GLOBAL_ID);
    GetInstance()->m_bGameplayEntered = false;
  }
}
void CWwiseSoundManager::StopMenuMusic(IEvent*, IComponent*) {
  GetInstance()->PlayMusic(MENU_MUSIC_STOP, GLOBAL_ID);
}
void CWwiseSoundManager::PauseMenuMusic(
    IEvent* /*cEvent*/,
    IComponent* /*cCenter*/) {
  // State pushed on top of us (DisableObjects)
  GetInstance()->PlayMusic(MENU_MUSIC_STOP, GLOBAL_ID);
}

void CWwiseSoundManager::ResumeMenuMusic(
    IEvent* /*cEvent*/,
    IComponent* /*cCenter*/) {
  // State on top of us got popped (EnableObjects)
  GetInstance()->PlayMusic(MENU_MUSIC_PLAY, GLOBAL_ID);
  GetInstance()->PlayTheSound(STOP_ALL_EXCEPT, GLOBAL_ID);
}

void CWwiseSoundManager::PlayGameplayMusic(
    IEvent* /*cEvent*/,
    IComponent* /*cCenter*/) {
  // us pushed on to state stack (InitObjects)
  GetInstance()->PlayMusic(
      MENU_MUSIC_STOP,
      GLOBAL_ID); // To make sure we stop playing the menu music
  GetInstance()->m_bGameplayEntered = true;
}

void CWwiseSoundManager::PauseGameplayMusic(
    IEvent* /*cEvent*/,
    IComponent* /*cCenter*/) {
  // State pushed on top of us (DisableObjects)
  GetInstance()->PauseAll();
  GetInstance()->m_bGameplayEntered = true;
}

void CWwiseSoundManager::ResumeGameplayMusic(
    IEvent* /*cEvent*/,
    IComponent* /*cCenter*/) {
  // State on top of us got popped (EnableObjects)
  GetInstance()->ResumeAll();
  GetInstance()->m_bGameplayEntered = true;
}

///////
void CWwiseSoundManager::PlayPauseMusic(
    IEvent* /*cEvent*/,
    IComponent* /*cCenter*/) {
  // us pushed on to state stack (InitObjects)

  GetInstance()->PauseAll();
  GetInstance()->PlayMusic(MENU_MUSIC_PLAY, GLOBAL_ID);
}

void CWwiseSoundManager::PausePauseMusic(
    IEvent* /*cEvent*/,
    IComponent* /*cCenter*/) {
  // State pushed on top of us (DisableObjects)
  GetInstance()->PlayMusic(MENU_MUSIC_STOP, GLOBAL_ID);
}

// Play the correct cart sound associated to the player and character
void CWwiseSoundManager::PlayCartSounds(void) {
  for (unsigned int i = 0; i < 4; ++i) {
    switch (CHUDManager::GetInstance()->GetPlayerCharacter(i)) {
    case BIKER_CHARACTER: {
      GetInstance()->PlayTheSound(BULLDOG_SPEED_PLAY, BIKER_ID);
      break;
    }
    case LARPERS_CHARACTER: {
      GetInstance()->PlayTheSound(WOODCART_SPEED, LARPER_ID);
      break;
    }
    case BANDITOS_CHARACTER: {
      GetInstance()->PlayTheSound(WOODCART_SPEED, BANDITOS_ID);
      break;
    }
    case SCIENTIST_CHARACTER: {
      GetInstance()->PlayTheSound(SCIENTIST_SPEED, SCIENTIST_ID);
      break;
    }
    case SASHA_CHARACTER: {
      GetInstance()->PlayTheSound(SASHA_SPEED, SASHA_ID);
      break;
    }
    case CRYGAME_CHARACTER: {
      GetInstance()->PlayTheSound(SCIENTIST_SPEED, CRYGAME_ID);
      break;
    }
    case STORYTIME_CHARACTER: {
      GetInstance()->PlayTheSound(SCIENTIST_SPEED, STORYTIME_ID);
      break;
    }
    }
  }
}
void CWwiseSoundManager::PlayerHurtSound(int number) {
  switch (CHUDManager::GetInstance()->GetPlayerCharacter(number)) {
  case BIKER_CHARACTER: {
    CWwiseSoundManager::GetInstance()->PlayTheSound(BULLDOG_HURT, BIKER_ID);
    break;
  }
  case BANDITOS_CHARACTER: {
    CWwiseSoundManager::GetInstance()->PlayTheSound(BANDITOS_HURT, BANDITOS_ID);
    break;
  }
  case LARPERS_CHARACTER: {
    CWwiseSoundManager::GetInstance()->PlayTheSound(LARPER_HURT, LARPER_ID);
    break;
  }
  case SCIENTIST_CHARACTER: {
    CWwiseSoundManager::GetInstance()->PlayTheSound(
        SCIENTIST_HURT, SCIENTIST_ID);
    break;
  }
  case SASHA_CHARACTER: {
    CWwiseSoundManager::GetInstance()->PlayTheSound(SASHA_HURT, SASHA_ID);
    break;
  }
  case CRYGAME_CHARACTER: {
    CWwiseSoundManager::GetInstance()->PlayTheSound(CRYGAME_HURT, CRYGAME_ID);
    break;
  }
  case STORYTIME_CHARACTER: {
    CWwiseSoundManager::GetInstance()->PlayTheSound(
        GSTORYTIME_HURT, STORYTIME_ID);
    break;
  }
  }
}
#endif
