/////////////////////////////////////////////////////////////////////////////////
////  File			:	"CSoundManager.h"
////
////  Author			:	Huy Nguyen (HN)
////
////	Team			:	Falcon Fish Productions
////
////  Date Created	:	4/13/2011
////
////	Last Changed	:	4/14/2011
////
////  Purpose			:	Wrapper class for WwiseSoundManager for componenets
////
/////////////////////////////////////////////////////////////////////////////////
//#ifndef _CSOUNDMANAGER_H_
//#define _CSOUNDMANAGER_H_
//
//#include <map>
//#include "../../Components/Sound/CSoundComponent.h"
//#include "../Global Managers/Memory Manager/CAllocator.h"
//
//class IEvent;
//class IComponent;
//class CWwiseSoundManager;
//
//class CSoundManager
//{
//private:
//	CWwiseSoundManager* m_pSound;
//	
//	///////////////
//	//Constructor//
//	///////////////
//	CSoundManager()
//	{
//	}
//	CSoundManager(const CSoundManager&)
//	{
//	}
//	CSoundManager& operator=(const CSoundManager&)
//	{
//	}
//	std::map<unsigned int, CSoundComponent*, less<unsigned int>,
//		CAllocator<pair<unsigned int, CSoundComponent*>>>	m_cSoundComps;	// A list of all the Sound Components
//public:
//	// Singleton Instance
//	static CSoundManager* GetInstance(void) 
//	{ 
//		static CSoundManager instance; 
//		return &instance; 
//	}
//
//	// Initalize
//	void Init();
//
//	// Accessors
//	inline CWwiseSoundManager* GetSound(void)
//	{
//		return m_pSound;
//	}
//
//	// Callbacks
//	static void PlayGameMusic(IEvent* e, IComponent* comp);
//	static void PlayMainMenuMusic(IEvent* e, IComponent* comp);
//	static void PlaySFX(IEvent* e, IComponent* comp);
//	static void StopMusic(IEvent* e, IComponent* comp);
//	static void PauseAll(IEvent* e, IComponent* comp);
//	static void ResumeAll(IEvent* e, IComponent* comp);
//};
//
//
//#endif //_CSOUNDMANAGER_H_