// Includes
#include "CUnlockableManager.h"
#include "..\\Event Manager\\CEventManager.h"
#include "..\\Event Manager\\EventStructs.h"
#include "..\\Object Manager\\CObjectManager.h"
#include "..\\..\\..\\CObject.h"
#include <fstream>
#include "..\Console Manager\CConsoleManager.h"
using std::fstream;
using namespace EventStructs;


CUnlockableManager::CUnlockableManager()
{

}
CUnlockableManager::~CUnlockableManager()
{

}
void CUnlockableManager::Init(void)
{
	//register for the event signaling that features have been unlocked
	CEventManager::GetInstance()->RegisterEvent("ItemUnlocked", (IComponent*)GetInstance(), GameWonCallBack);

	//see what we have unlocked
	m_szFileName = "Unlockables.bin";
	fstream fin(m_szFileName.c_str(), ios_base::in | ios_base::binary);
	bool bChar, bSkip, bNotified;
	bChar = bSkip = bNotified = false;

	if(fin.is_open())
	{
		//get unlocked character and other statuses
		fin.read((char*)&bChar, sizeof(bool));
		fin.read((char*)&bSkip, sizeof(bool));
		fin.read((char*)&bNotified, sizeof(bool));
		fin.close();
		fin.clear();
		//		GetInstance()->SetCharactersUnlocked(bChar);
		//		GetInstance()->SetIntroSkip(bSkip);
		//		GetInstance()->SetPlayerNotified(bNotified);
	}
	if(bChar)
	{//if characters are unlocked
		//it was in the file, so the player was notified and all, set the features to unlocked
		GetInstance()->SetCharactersUnlocked(true);
	}
	else
	{
		GetInstance()->SetCharactersUnlocked(false);
	}
	if(bSkip)
	{//the player can now skip the intro screen
		GetInstance()->SetIntroSkip(true);
	}
	else
	{
		GetInstance()->SetIntroSkip(false);
	}
	if(bNotified)
	{
		GetInstance()->SetPlayerNotified(true);
	}
	else
	{
		GetInstance()->SetPlayerNotified(false);
	}

}
void CUnlockableManager::GameWonCallBack(IEvent*, IComponent*)
{//call back for item unlock event
	//on game win, unlock the features
	GetInstance()->UnlockFeatures();
	//and updates the file
	GetInstance()->UpdateFile();
}
void CUnlockableManager::UpdateFile(void)
{
	//update file to reflect unlocked/skippable feature status
	fstream out(m_szFileName.c_str(), ios_base::out | ios_base::trunc);
	if(out.is_open())
	{
		out.write((char*)&m_bCharactersUnlocked, sizeof(bool));
		out.write((char*)&m_bIntroSkip, sizeof(bool));
		out.write((char*)&m_bPlayerNotified, sizeof(bool));
		out.close();
	}
}

void CUnlockableManager::UnlockFeatures()
{
	//set the lua bool to true so the character buttons are selectable
	Debug.CallLuaFunc("SetUnlocked()");
	//set the features to unlocked
//	SetCharactersUnlocked(true);
	SetIntroSkip(true);
//	SetPlayerNotified(true);
	//update the file

	UpdateFile();
}
void CUnlockableManager::Update(void)
{
	//no need
}
void CUnlockableManager::Shutdown(void)
{
	//no need
}




