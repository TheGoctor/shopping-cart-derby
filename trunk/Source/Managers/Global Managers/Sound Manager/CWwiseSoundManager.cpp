////////////////////////////////////////////////////////
//  File			:	"CWwiseSoundManager.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	3/27/2011
//
//	Last Changed	:	4/13/2001
//
//  Purpose			:	Wrapper class for Wwise API
//
////////////////////////////////////////////////////////
#include"CWwiseSoundManager.h"
//includes
/////////////////////////////////////////
#ifdef _DEBUG
#include "../../../Wwise/includes/AkCommunication.h"
#endif

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include<AK/MusicEngine/Common/AkMusicEngine.h>
#include<AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/Tools/Common/AkMonitorError.h>
#include <AK/Plugin/AkVorbisFactory.h>
#ifdef _DEBUG
#include <AK/Comm/AkCommunication.h>
#endif

#include <iostream>
#include "../../../Wwise/includes/akfilepackagelowlevelioblocking.h"
//Plugins
#include "../../../Wwise/Plugin/AkRoomVerbFXFactory.h"
#include "../../../Wwise/Plugin/AkSoundSeedImpactFXFactory.h"
#include "../../../Wwise/Plugin/AkParametricEQFXFactory.h"
#include "../../../Wwise/Plugin/AkVorbisFactory.h"
#include "../../../Wwise/Plugin/AkAudioInputSourceFactory.h"
#include "../../../Wwise/Plugin/AkDelayFXFactory.h"
#include "../../../Wwise/Plugin/AkPeakLimiterFXFactory.h"

#define SOUND_VOLUME_DEFAULT 50.0f
#define MUSIC_VOLUME_DEFAULT 15.0f

// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
namespace AK
{
#ifdef WIN32
	void * AllocHook( size_t in_size )
	{
		return malloc( in_size );
	}
	void FreeHook( void * in_ptr )
	{
		free( in_ptr );
	}
	// Note: VirtualAllocHook() may be used by I/O pools of the default implementation
	// of the Stream Manager, to allow "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING
	// - refer to the Windows SDK documentation for more details). This is NOT mandatory;
	// you may implement it with a simple malloc().
	void * VirtualAllocHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwAllocationType,
		DWORD in_dwProtect
		)
	{
		return VirtualAlloc( in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect );
	}
	void VirtualFreeHook( 
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwFreeType
		)
	{
		VirtualFree( in_pMemAddress, in_size, in_dwFreeType );
	}
#endif
}



CWwiseSoundManager::CWwiseSoundManager()
{
}

CWwiseSoundManager::~CWwiseSoundManager()
{
}

CWwiseSoundManager* CWwiseSoundManager::GetInstance()
{
	static CWwiseSoundManager instance;
	return &instance;
}

void CWwiseSoundManager::InitSoundManager()
{
	mTotalID = 2;
	m_pGlobalID = 1;
	
	InitWise();
	InitSounds();
	InitBanks();
	
	LoadBank(INIT);
	 LoadBank(SOUNDBANK);
	
	 m_fDefSound = SOUND_VOLUME_DEFAULT;
	 m_fDefMusic = MUSIC_VOLUME_DEFAULT;

	 SetSoundVolume(m_fDefSound);
	 SetMusicVolume(m_fDefMusic);
}

void CWwiseSoundManager::Update()
{
	AK::SoundEngine::RenderAudio();
}

void CWwiseSoundManager::ShutdownManager()
{
	StopAll();

	UnloadAll();

	ShutdownWise();
}

void CWwiseSoundManager::PlaySound(SoundID soundid, AkGameObjectID object)
{
	AK::SoundEngine::PostEvent(mSounds[soundid],object);
}

void CWwiseSoundManager::PlayMusic(SoundID soundid)
{
	AK::SoundEngine::PostEvent(mSounds[soundid],m_pGlobalID);
}

void CWwiseSoundManager::Pause(AkGameObjectID object)
{
	AK::SoundEngine::PostEvent(AK::EVENTS::PAUSE_ALL,object);
}

void CWwiseSoundManager::PauseAll()
{
	AK::SoundEngine::PostEvent(AK::EVENTS::PAUSE_ALL, m_pGlobalID);
}

void CWwiseSoundManager::Resume(AkGameObjectID object)
{
	AK::SoundEngine::PostEvent(AK::EVENTS::RESUME_ALL,object);
}

void CWwiseSoundManager::Stop(AkGameObjectID object)
{
	AK::SoundEngine::PostEvent(AK::EVENTS::STOP_ALL, object);
}

void CWwiseSoundManager::StopAll()
{
	AK::SoundEngine::PostEvent(AK::EVENTS::STOP_ALL, m_pGlobalID);
}

void CWwiseSoundManager::SetScale(AkGameObjectID object, float scale)
{
	AK::SoundEngine::SetAttenuationScalingFactor(object, scale);
}


void CWwiseSoundManager::SetMusicVolume(float volume)
{
	m_fMusicVolume = volume;
	AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::MX_VOLUME,m_fMusicVolume);
}

void CWwiseSoundManager::SetSoundVolume(float volume)
{
	m_fSoundVolume = volume;
	AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::FX_VOLUME,m_fSoundVolume);
	AK::SoundEngine::SetRTPCValue(AK::GAME_PARAMETERS::DX_VOLUME,m_fSoundVolume);
}

void CWwiseSoundManager::SetListener(D3DXVECTOR3 position, float scale)
{
	AkListenerPosition pos;
	pos.Position.X = position.x;
	pos.Position.Y = position.y;
	pos.Position.Z = position.z;

	AK::SoundEngine::SetListenerPosition(pos);
	AK::SoundEngine::SetListenerScalingFactor(0,scale);
}

void CWwiseSoundManager::SetObjectPosition(AkGameObjectID object, D3DXVECTOR3 position)
{
	AkSoundPosition pos;
	pos.Position.X = position.x;
	pos.Position.Y = position.y;
	pos.Position.Z = position.z;
	AK::SoundEngine::SetPosition(object, pos);
}

void CWwiseSoundManager::RegisterObject(AkGameObjectID object)
{
	object = mTotalID;
	AK::SoundEngine::RegisterGameObj(object);
	++mTotalID;
}

void CWwiseSoundManager::UnregisterObject(AkGameObjectID object)
{
	AK::SoundEngine::UnregisterGameObj(object);
	//--mTotalID;
}
void CWwiseSoundManager::InitWise()
{
	//Memory Manager
	AkMemSettings memSettings;
	memSettings.uMaxNumPools = 20;
	if ( AK::MemoryMgr::Init( &memSettings ) != AK_Success )
	{
		assert( ! "Could not create the memory manager." );
	}

	//Stream manager
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);
	if(!AK::StreamMgr::Create(stmSettings))
	{
		assert( ! "Could not create the Stream manager." );
	}


	//IO shit
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
	if(g_lowLevelIO.Init(deviceSettings)!=AK_Success)
	{
		assert( ! "Could not create the IO manager." );
	}


	g_lowLevelIO.SetBankPath(AKTEXT(""));
	g_lowLevelIO.SetLangSpecificDirName(AKTEXT("English(US)/"));

	//Sound engine
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
	if(AK::SoundEngine::Init(&initSettings,&platformInitSettings)!=AK_Success)
	{
		assert( ! "Could not create the Sound Engine." );
	}

	//Music engine
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);
	if(AK::MusicEngine::Init(&musicInit)!=AK_Success)
	{
		assert( ! "Could not create the Music Engine." );
	}
#ifdef _DEBUG

	//#ifndef AK_OPTIMIZED
	//
	// Initialize communications (not in release build!)
	//
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings( commSettings );
	if ( AK::Comm::Init( commSettings ) != AK_Success )
	{
		assert( ! "Could not initialize communication." );
		//return false;
	}
	//#endif // AK_OPTIMIZED



#endif // AK_OPTIMIZED	AkMusicSettings musicSettings;

	RegisterPlugins();

	AK::SoundEngine::RegisterCodec(AKCOMPANYID_AUDIOKINETIC, AKCODECID_VORBIS, CreateVorbisFilePlugin, CreateVorbisBankPlugin);

	//AK::MotionEngine::SetPlayerListener(0,0);
	AK::SoundEngine::SetListenerPipeline(0, true, false);

}
void CWwiseSoundManager::ShutdownWise()
{
	AK::SoundEngine::UnregisterAllGameObj();

	AK::SoundEngine::RenderAudio();

#ifdef _DEBUG
	//#ifndef AK_OPTIMIZED
	//
	// Terminate Communication Services
	//
	AK::Comm::Term();
	//#endif // AK_OPTIMIZED
#endif

	// Terminate the music engine
	//
	AK::MusicEngine::Term();


	// Terminate the sound engine
	//
	if(AK::SoundEngine::IsInitialized())
	{
		AK::SoundEngine::Term();
	}

	// Terminate the streaming device and streaming manager

	// CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device 
	// that lives in the Stream Manager, and unregisters itself as the File Location Resolver.   
	g_lowLevelIO.Term();

	if ( AK::IAkStreamMgr::Get() )
		AK::IAkStreamMgr::Get()->Destroy();

	//vorbis
	// Terminate the Memory Manager
	AK::MemoryMgr::Term();

}
void CWwiseSoundManager::InitBanks()
{
	mBanks[INIT].mLocation = "Source/Wwise/BankFiles/Init.bnk";
	mBanks[SOUNDBANK].mLocation = "Source/Wwise/BankFiles/SoundBank.bnk";

}

void CWwiseSoundManager::InitSounds()
{
	AK::SoundEngine::RegisterGameObj(m_pGlobalID);

	mSounds[MENU_SELECT] = AK::EVENTS::PLAY_FX_2D_MAGICPICKUP;
	mSounds[MENU_OPTION_CHANGE] = AK::EVENTS::PLAY_FX_2D_BEEP;
	mSounds[INVALID_SELECTION] = AK::EVENTS::PLAY_FX_2D_MENUCANCEL;
	mSounds[CART_MOVEMENT] = AK::EVENTS::PLAY_FX_2D_WEAPONWHOOSH;
	mSounds[CART_BRAKE] = AK::EVENTS::PLAY_DX_2D_LAND;
	mSounds[CART_COLLISION] = AK::EVENTS::PLAY_FX_3D_SMASHCRATE;
	mSounds[ITEM_DROP] = AK::EVENTS::PLAY_FX_2D_AMMOPICKUP;
	mSounds[ITEM_SPAWN] = AK::EVENTS::PLAY_FX_2D_SHEILDPICKUP;
	mSounds[PICK_UP] = AK::EVENTS::PLAY_FX_2D_COINPICKUP;
	mSounds[MUSICLOOP_PLAY] = AK::EVENTS::PLAY_MX_MUSICLOOP_06;
	mSounds[MUSICLOOP_STOP] = AK::EVENTS::STOP_MX_MUSICLOOP_06;
}

void CWwiseSoundManager::LoadBank(BankID bankid)
{
	AKRESULT result = AK::SoundEngine::LoadBank(mBanks[bankid].mLocation,AK_DEFAULT_POOL_ID,mBanks[bankid].mID);
	return;
}

void CWwiseSoundManager::UnloadBank(BankID bankid)
{
	AK::SoundEngine::UnloadBank(mBanks[bankid].mID);
}

void CWwiseSoundManager::UnloadAll()
{
	for(int i = 0; i < MAX_BANKS; ++i)
	{
		AK::SoundEngine::UnloadBank(mBanks[i].mID);
	}
}

void CWwiseSoundManager::RegisterPlugins()
{
	// Register RoomVerb FX plugin
	AK::SoundEngine::RegisterPlugin( AkPluginTypeEffect,
		AKCOMPANYID_AUDIOKINETIC,
		AKEFFECTID_ROOMVERB,
		CreateRoomVerbFX,
		CreateRoomVerbFXParams);

	AK::SoundEngine::RegisterPlugin( AkPluginTypeEffect,
		AKCOMPANYID_AUDIOKINETIC,
		AKEFFECTID_SOUNDSEEDIMPACT,
		CreateAkSoundSeedImpactFX,
		CreateAkSoundSeedImpactFXParams);

	AK::SoundEngine::RegisterPlugin( AkPluginTypeEffect,
		AKCOMPANYID_AUDIOKINETIC,
		AKEFFECTID_PARAMETRICEQ,
		CreateParametricEQFX,
		CreateParametricEQFXParams);

	AK::SoundEngine::RegisterPlugin( AkPluginTypeEffect,
		AKCOMPANYID_AUDIOKINETIC,
		AKEFFECTID_DELAY,
		CreateDelayFX,
		CreateDelayFXParams);

	AK::SoundEngine::RegisterPlugin( AkPluginTypeEffect,
		AKCOMPANYID_AUDIOKINETIC,
		AKEFFECTID_PEAKLIMITER,
		CreatePeakLimiterFX,
		CreatePeakLimiterFXParams);

	AK::SoundEngine::RegisterCodec(
		AKCOMPANYID_AUDIOKINETIC, 
		AKCODECID_VORBIS,
		CreateVorbisFilePlugin,
		CreateVorbisBankPlugin);

	AK::SoundEngine::RegisterPlugin( AkPluginTypeSource,
		AKCOMPANYID_AUDIOKINETIC,
		AKSOURCEID_AUDIOINPUT,
		CreateAudioInputSource,
		CreateAudioInputSourceParams);

}
