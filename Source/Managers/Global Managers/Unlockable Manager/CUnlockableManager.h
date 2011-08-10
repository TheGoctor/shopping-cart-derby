////////////////////////////////////////////////////////////////////////////////
//	File			:	CUnlockableManager.h
//	Date			:	7/19/11
//	Mod. Date		:	7/19/11
//	Mod. Initials	:	RN
//	Author			:	Ray Nieves
//	Purpose			:	Handles unlocking characters from winning the game
//						game.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CUNLOCKABLEMANAGER_H_
#define _CUNLOCKABLEMANAGER_H_

#include <string>
#include <map>
#include <set>
#include <list>
using namespace std;

#include "../Memory Manager/CEventAllocator.h"
#include "..\..\..\Enums.h"
#include "..\\Event Manager\\IEvent.h"
//class IEvent;
class IComponent;
typedef unsigned int EventID;

enum Unlockables{EU_CHAR_ST = 1, EU_CHAR_CG };//in case we have any more unlockable stuff

class CUnlockableManager
{
private:

	//bools for which characters are unlocked
	std::string m_szFileName;
	bool m_bCharactersUnlocked;	//false unless the player has unlocked the hidden characters
	bool m_bPlayerNotified;			//false unless the player has been alerted to the new characters
	bool m_bIntroSkip;					//false unless the player has reached the main menu once

	//constructo
	CUnlockableManager();
	// two parts of the trinity
	CUnlockableManager(const CUnlockableManager&);
	CUnlockableManager& operator=(const CUnlockableManager&);

public:
	//destructo
	~CUnlockableManager();
	//getinstance
	static CUnlockableManager* GetInstance()
	{
		static CUnlockableManager pcUnLockableManager;
		return &pcUnLockableManager;
	}

	void Init(void);
	static void Update(void);
	static void Shutdown(void);
	//callback for player win
	static void GameWonCallBack(IEvent*, IComponent*);
	//unlocks all features that are locked(skipping intro, secret characters, etc)
	void UnlockFeatures();
	//updates the file to reflect any changes
	void UpdateFile(void);

	//accessors and mutators
	std::string GetFileName(void)
	{
		return m_szFileName;
	}
	bool GetCharactersUnlocked(void)
	{
		return m_bCharactersUnlocked;
	}
	bool GetPlayerNotified(void)
	{
		return m_bPlayerNotified;
	}
	bool GetIntroSkip(void)
	{
		return m_bIntroSkip;
	}
	void SetIntroSkip(bool introskip)
	{
		m_bIntroSkip = introskip;
		UpdateFile();
	}
	void SetFileName(std::string szFileName)
	{
		m_szFileName = szFileName;
	}
	void SetCharactersUnlocked(bool bunlocked)
	{
		m_bCharactersUnlocked = bunlocked;

		if(bunlocked)
			UnlockFeatures();
		UpdateFile();
	}
	void SetPlayerNotified(bool bnotified)
	{
		m_bPlayerNotified = bnotified;
		UpdateFile();
	}
};

#endif