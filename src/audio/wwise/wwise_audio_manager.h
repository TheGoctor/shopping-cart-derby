///////////////////////////////////////////////////////////////////////////////
//  File			:	"CWwiseSoundManager.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	3/27/2011
//
//	Last Changed	:	7/22/2011
//
//	Changed By		:	HN
//
//  Purpose			:	Wrapper class for Wwise API
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//#include "audio/wwise/includes/AkFilePackageLowLevelIOBlocking.h"
#include "enums.h"

enum BankID { INIT = 0, SOUNDBANK, MAX_BANKS };

class IEvent;
class IComponent;

class CWwiseSoundManager {
private:
  struct Bank {
    AkBankID mID;
    char* mLocation;
  };

  CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

  Bank mBanks[MAX_BANKS]; // array that holds the banks

  AkGameObjectID m_pObjectID; /// Object ID

  float m_fSoundVolume; /// variable to change SFX volume
  float m_fMusicVolume; /// variable to change music volume
  float m_fDXvolume;    /// variable to change DX volume
  float m_fRPM;         /// variable to change RPM

  float m_fDefSound;   /// variable that holds the default SFX volume
  float m_fDefMusic;   /// variable that holds the default music volume
  float m_fDefDXSound; /// variable that holds the default DX volume
  float m_fDefRPM;     /// variable that holds the default RPM

  bool m_bGameplayEntered;

  ////////////////////////////////////////////////////////////////////////////////
  // Function: "InitWise"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:  Initializes Wwise and all the sound engines need to play sounds
  //
  ////////////////////////////////////////////////////////////////////////////////
  void InitWise(void);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "ShutdownWise"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:  Shutsdowns Wwise and all the engines
  //
  ////////////////////////////////////////////////////////////////////////////////
  void ShutdownWise(void);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "InitBanks"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: Grabs the location of the bank files that need to be loaded
  //
  ////////////////////////////////////////////////////////////////////////////////
  void InitBanks(void);

  CWwiseSoundManager();  //  constructor
  ~CWwiseSoundManager(); //  destructor

public:
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "InitSoundManager"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:  Initializes the manager and loads in all the bank files
  //
  ////////////////////////////////////////////////////////////////////////////////
  void InitSoundManager();
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "RegisterForEvents"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:  Register to listen to events
  //
  ////////////////////////////////////////////////////////////////////////////////
  void RegisterForEvents();
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "GetInstance"
  //
  // Return: static CWwiseSoundManager* - returns an instance of the manager
  //
  // Parameters: void
  //
  // Purpose:  Get an Instance of the manager
  //
  ////////////////////////////////////////////////////////////////////////////////
  static CWwiseSoundManager* GetInstance();
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "ShutdownManager"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:  Shutdown the WwiseManager and unloads all the banks
  //
  ////////////////////////////////////////////////////////////////////////////////
  void ShutdownManager();
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "Update"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:  Update the sound manager to play sounds
  //
  ////////////////////////////////////////////////////////////////////////////////
  void Update();
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "PlyTheSound"
  //
  // Return: void
  //
  // Parameters: SoundID soundid - enum of the sound to play,
  //			   int object	   - the object id that the sound attaches
  //to
  //
  // Purpose:   The a spefic sound IN: the sound ID, the object ID
  //
  ////////////////////////////////////////////////////////////////////////////////
  void PlayTheSound(SoundID soundid, int object);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "PlayMusic"
  //
  // Return: void
  //
  // Parameters: SoundID soundid - enum of the music to play
  //				int object     - the object id that the music attaches
  //to
  //
  // Purpose:  Play the Music passed in IN: the music ID, the object ID
  //
  ////////////////////////////////////////////////////////////////////////////////
  void PlayMusic(SoundID soundid, int object);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "PauseAll"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:   Pauses all the sounds
  //
  ////////////////////////////////////////////////////////////////////////////////
  void PauseAll();
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "Resumes"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:  Resumes all the sounds that were playing
  //
  ////////////////////////////////////////////////////////////////////////////////
  void ResumeAll();
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "StopAll"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:   Stop all the sounds in game
  //
  ////////////////////////////////////////////////////////////////////////////////
  void StopAll();
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "SetScale"
  //
  // Return: void
  //
  // Parameters: int object - the object that you are changing the value for
  //			float scale - the value to set the sound attenuation to
  //
  // Purpose: Sets the distance the object sounds can be heard
  //
  ////////////////////////////////////////////////////////////////////////////////
  void SetScale(int object, float scale);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "SetListenerPosition"
  //
  // Return: void
  //
  // Parameters: D3DXVECTOR3  - the position of the listener(mic)
  //			D3DXVECTOR3	- the forward vector of the listener
  //			float		- how far the listener can hear
  //			int			- the listener value
  //
  // Purpose:  Sets the position of the listener and how far it can hear
  //	needs to be called everytime the listener moves
  //
  ////////////////////////////////////////////////////////////////////////////////
  void SetListenerPosition(
      D3DXVECTOR3 position,
      D3DXVECTOR3 forwardvec,
      float scale,
      int listener);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "SetObjectPosition"
  //
  // Return: void
  //
  // Parameters: int - the object id we are setting
  //			D3DXVECTOR3	- the current position of the object
  //			float - the distance the sound projects from the object
  //
  // Purpose:  Sets the sound objects position and how far the sound projects
  //		should be called everytime the position changes
  ////////////////////////////////////////////////////////////////////////////////
  void SetObjectPosition(int object, D3DXVECTOR3 position, float scale);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "RegisterObject"
  //
  // Return: void
  //
  // Parameters: int - the object ID to register
  //
  // Purpose:  Registers the object ID with wise so it can use sounds
  //
  ////////////////////////////////////////////////////////////////////////////////
  void RegisterObject(int object);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "RegisterHeldObject"
  //
  // Return: int - returns a good id to the object
  //
  // Parameters: void
  //
  // Purpose:  Registers the held object with a good id
  //
  ////////////////////////////////////////////////////////////////////////////////
  int RegisterHeldObject(void);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "UnregisterObject"
  //
  // Return: void
  //
  // Parameters: int - the object ID that needs to be unregistered
  //
  // Purpose:  Unregisters the object to free up memory
  //
  ////////////////////////////////////////////////////////////////////////////////
  void UnregisterObject(int object);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "UnloadAll"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose:  Unloads all the sound banks
  //
  ////////////////////////////////////////////////////////////////////////////////
  void UnloadAll(void);
  ////////////////////////////////////////////////////////////////////////////
  // Accessors
  ////////////////////////////////////////////////////////////////////////////
  float GetMusicVolume() { return m_fMusicVolume; }
  float GetSoundVolume() { return m_fSoundVolume; }
  float GetDXVolume() { return m_fDXvolume; }
  float GetDefaultMusicVolume() { return m_fDefMusic; }
  float GetDefaultSoundVolume() { return m_fDefSound; }
  ////////////////////////////////////////////////////////////////////////////
  // Mutators
  ////////////////////////////////////////////////////////////////////////////
  void SetMusicVolume(float volume);
  void SetSoundVolume(float volume);
  void SetDXVolume(float volume);
  void SetRPMValueForSound(float rpm, int object);
  void SetDefaultSound(float volume) { m_fDefSound = volume; }
  void SetDefaultMusic(float volume) { m_fDefMusic = volume; }
  void SetDefaultDX(float volume) { m_fDefDXSound = volume; }
  void SetDefaultRPM(float rpm) { m_fDefRPM = rpm; }
  ////////////////////////////////////////////////////////////////////////////////
  // Function: �Load Bank�
  //
  // Return:  void
  //
  // Parameters: BankID - takes in the ID of the bank file to load
  //
  // Purpose: Loads the bank files for Wwise to use
  //
  ////////////////////////////////////////////////////////////////////////////////
  void LoadBank(BankID bankid);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "UnloadBank"
  //
  // Return: void
  //
  // Parameters: BankID - the bank to unload
  //
  // Purpose:  Unloads the bank passed in
  //
  ////////////////////////////////////////////////////////////////////////////////
  void UnloadBank(BankID bankid);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "PlayCartSounds"
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: Plays the cart sounds for the correct characters
  //
  ////////////////////////////////////////////////////////////////////////////////
  void PlayCartSounds(void);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "PlayerHurtSound"
  //
  // Return: void
  //
  // Parameters: int - the player number to play the hurt sound for
  //
  // Purpose:  Checks the player number and finds out which character set to
  // play the hurt sound for
  ////////////////////////////////////////////////////////////////////////////////
  void PlayerHurtSound(int number);
  ////////////////////////////////////////////////////////////////////////////////
  // Event Callbacks
  // Callbacks for music changes for stopping and playing the sounds correctly
  ////////////////////////////////////////////////////////////////////////////////
  static void SetMusicVolumeCallback(IEvent* cEvent, IComponent* cCenter);
  static void SetSFXVolumeCallback(IEvent* cEvent, IComponent* cCenter);
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
