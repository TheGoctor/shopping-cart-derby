///////////////////////////////////////////////////////////////////////////////
//  File			:	"CWwiseSoundManager.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	3/27/2011
//
//	Last Changed	:	4/13/2011
//
//  Purpose			:	Wrapper class for Wwise API
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FALCONFISH_WWISE_SOUND_MANAGER_H
#define _FALCONFISH_WWISE_SOUND_MANAGER_H

#include "..\..\Global Managers\Rendering Managers\dxutil.h"
#include "../../../Wwise/Wwise_IDs.h"
#include "../../../Wwise/includes/AkFilePackageLowLevelIOBlocking.h"

#define EventID AkUniqueID

enum SoundID{ MENU_SELECT = 0, MENU_OPTION_CHANGE, INVALID_SELECTION,  
			CART_MOVEMENT, CART_BRAKE, CART_COLLISION, ITEM_DROP, ITEM_SPAWN, PICK_UP, 
			MUSICLOOP_PLAY, MUSICLOOP_STOP, MAX_SOUNDS };

enum BankID{INIT = 0, SOUNDBANK, MAX_BANKS};


class CWwiseSoundManager
{
private:
	struct Sound
	{
		AkUniqueID mPlayEventID; //ID for sound in the Sound bank
	};

	struct Bank
	{
		AkBankID mID;
		char *mLocation;
	};

	CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

	EventID mSounds[MAX_SOUNDS];// array that holds all sounds
	Bank  mBanks[MAX_BANKS];	// array that holds the banks

	AkGameObjectID m_pGlobalID; /// Object ID

	unsigned int mTotalID;		/// total object IDs

	float m_fSoundVolume;		/// variable to change SFX volume
	float m_fMusicVolume;		/// variable to change music volume

	float m_fDefSound;			/// variable that holds the default SFX volume
	float m_fDefMusic;			/// variable that holds the default music volume

	void InitWise();			// initialize Wwise
	void ShutdownWise();		// Shutsdown Wwise
	void InitSounds();			// loads all sounds
	void InitBanks();			// loads in the bank
	void RegisterPlugins();		// registers Wwise plugins

	CWwiseSoundManager();		//  constructor
	~CWwiseSoundManager();		//  destructor

public:

	void InitSoundManager();
	static CWwiseSoundManager* GetInstance();
	void ShutdownManager();
	void Update();

	void PlaySound(SoundID soundid, AkGameObjectID object);
	void PlayMusic(SoundID soundid);
	void Pause(AkGameObjectID object);
	void PauseAll();
	void Stop(AkGameObjectID object);
	void Resume(AkGameObjectID object);
	void StopAll();

	void SetScale(AkGameObjectID object, float scale);
	void SetListener(D3DXVECTOR3 position, float scale);
	void SetObjectPosition(AkGameObjectID object, D3DXVECTOR3 position);

	void RegisterObject(AkGameObjectID object);
	void UnregisterObject(AkGameObjectID object);
	void UnloadAll();

	float GetMusicVolume() 
	{
		return m_fMusicVolume;
	}
	float GetSoundVolume() 
	{
		return m_fSoundVolume;
	}

	void SetMusicVolume(float volume);
	void SetSoundVolume(float volume);

	void SetDefaultSound(float volume) 
	{ 
		m_fDefSound = volume; 
	}
	void SetDefaultMusic(float volume) 
	{ 
		m_fDefMusic = volume; 
	}

	void LoadBank(BankID bankid);
	void UnloadBank(BankID bankid);

};

#endif