///////////////////////////////////////////////////////////////////////////////
//  File			:	"CWwiseSoundManager.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	3/27/2011
//
//	Last Changed	:	5/04/2011
//
//	Changed By		:	HN
//
//  Purpose			:	Wrapper class for Wwise API
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FALCONFISH_WWISE_SOUND_MANAGER_H
#define _FALCONFISH_WWISE_SOUND_MANAGER_H

#include "../../Global Managers/Rendering Managers/dxutil.h"
#include "../../../Wwise/includes/AkFilePackageLowLevelIOBlocking.h"
#include "../../../Enums.h"


enum BankID{INIT = 0, SOUNDBANK, MAX_BANKS};

class IEvent;
class IComponent;


class CWwiseSoundManager
{
private:

	struct Bank
	{
		AkBankID mID;
		char *mLocation;
	};

	CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

	Bank  mBanks[MAX_BANKS];	// array that holds the banks

	AkGameObjectID m_pObjectID; /// Object ID

	float m_fSoundVolume;		/// variable to change SFX volume
	float m_fMusicVolume;		/// variable to change music volume
	float m_fDXvolume;			/// variable to change DX volume
	float m_fRPM;				/// variable to change RPM

	float m_fDefSound;			/// variable that holds the default SFX volume
	float m_fDefMusic;			/// variable that holds the default music volume
	float m_fDefDXSound;		/// variable that holds the default DX volume
	float m_fDefRPM;			/// variable that holds the default RPM 

	void InitWise();			// initialize Wwise
	void ShutdownWise();		// Shutsdown Wwise
	void InitBanks();			// loads in the bank

	CWwiseSoundManager();		//  constructor
	~CWwiseSoundManager();		//  destructor

public:

	void InitSoundManager();
	void RegisterForEvents();
	static CWwiseSoundManager* GetInstance();
	void ShutdownManager();
	void Update();

	void PlayTheSound(SoundID soundid, int object);
	void PlayMusic(SoundID soundid, int object);
	void PauseAll();
	void ResumeAll();
	void StopAll();

	void SetScale(int object, float scale);
	void SetListenerPosition(D3DXVECTOR3 position, D3DXVECTOR3 forwardvec ,
							float scale, int listener);
	void SetObjectPosition(int object, D3DXVECTOR3 position, float scale);

	void RegisterObject(int object);
	int RegisterHeldObject();
	void UnregisterObject(int object);
	void UnloadAll();

	float GetMusicVolume() 
	{
		return m_fMusicVolume;
	}
	float GetSoundVolume() 
	{
		return m_fSoundVolume;
	}
	float GetDXVolume()
	{
		return m_fDXvolume;
	}
	float GetDefaultMusicVolume()
	{
		return m_fDefMusic;
	}
	float GetDefaultSoundVolume()
	{
		return m_fDefSound;
	}

	void SetMusicVolume(float volume);
	void SetSoundVolume(float volume);
	void SetDXVolume(float volume);
	void SetRPMValueForSound(float rpm, int object);

	void SetDefaultSound(float volume) 
	{ 
		m_fDefSound = volume; 
	}
	void SetDefaultMusic(float volume) 
	{ 
		m_fDefMusic = volume; 
	}
	void SetDefaultDX(float volume)
	{
		m_fDefDXSound = volume;
	}
	void SetDefaultRPM(float rpm)
	{
		m_fDefRPM = rpm;
	}

	void LoadBank(BankID bankid);
	void UnloadBank(BankID bankid);

	// Event Callbacks
	static void SetMusicVolumeCallback(IEvent* cEvent, IComponent* cCenter);
	static void SetSFXVolumeCallback(IEvent* cEvent, IComponent* cCenter);

	// Callbacks for state change music changes
	static void PlayMenuMusic(IEvent* cEvent, IComponent* cCenter);
	static void PauseMenuMusic(IEvent* cEvent, IComponent* cCenter);
	static void ResumeMenuMusic(IEvent* cEvent, IComponent* cCenter);
	static void PlayGameplayMusic(IEvent* cEvent, IComponent* cCenter);
	static void PauseGameplayMusic(IEvent* cEvent, IComponent* cCenter);
	static void ResumeGameplayMusic(IEvent* cEvent, IComponent* cCenter);
	static void PlayPauseMusic(IEvent* cEvent, IComponent* cCenter);
	static void PausePauseMusic(IEvent* cEvent, IComponent* cCenter);
	static void ResumePauseMusic(IEvent* cEvent, IComponent* cCenter);
	static void StopMenuMusic(IEvent* cEvent, IComponent* cCenter);

};

#endif