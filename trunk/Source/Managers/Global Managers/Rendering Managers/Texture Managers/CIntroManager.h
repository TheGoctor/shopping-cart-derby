////////////////////////////////////////////////////////////////////////////////
//  File			:	"CIntroManager.h"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	05/18/11
//
//	Last Changed	:	05/18/11
//
//	Changed By		:				
//
//  Purpose			:	Wrapper class for manageing Intro screen
////////////////////////////////////////////////////////////////////////////////
#ifndef _CINTROMANAGER_H_
#define _CINTROMANAGER_H_
#include "..\\..\\Rendering Managers\\dxutil.h"
#include "CTextureManager.h"
#include "..\\..\\..\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

class CObjectManager;
class CObject;
class IEvent;
class IComponent;

class CIntroManager
{
private:
	CTextureManager* m_pTM;
	CObjectManager*  m_pOM;

	CSpriteComponent* m_pGPSplash;
	CSpriteComponent* m_pShatteredSplash;
	CSpriteComponent* m_pWwiseSplash;
	CSpriteComponent* m_pFalconSplash;
	CSpriteComponent* m_pTitleScreen;
	CSpriteComponent* m_pPressEnter;

	float m_fSeconds;
	float m_fDT;
	float m_dt;
	float m_fAlphaValue;
	float m_fTitleBarAlphValue;
	bool m_bEnterMenu;
	bool m_pHasPlayed;

int nTexID1;
int nTexID2;
int nTexID3;
int nTexID4;
int nTexID5;
int nTexID6;


	CObject* m_pIntroObj;

	CIntroManager(void);
	~CIntroManager(void);
	CIntroManager& operator=(const CIntroManager&);

	void InitIntro(void);

public:

	static CIntroManager* GetInstance(void) 
	{ 
		static CIntroManager pIntroInstance;
		return &pIntroInstance; 
	}

	void Init(void);
    static void Shutdown(IEvent* pEvent, IComponent* pComp);
	void Update(void);
	void PlayMusic(void);
	void ChangeToMainMenu(void);
	static void EnterMenu(IEvent* pEvent, IComponent* pComp);
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
};

#endif //C_INTROMANAGER_H_