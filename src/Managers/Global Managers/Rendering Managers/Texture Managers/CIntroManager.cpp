////////////////////////////////////////////////////////////////////////////////
//  File			:	"CIntroManager.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	05/18/11
//
//	Last Changed	:	07/24/11
//
//	Changed By		:	HN			
//
//  Purpose			:	Wrapper class for manageing Intro screen
////////////////////////////////////////////////////////////////////////////////
#include "CIntroManager.h"
#include "..\\..\\Object Manager\\CObjectManager.h"
#include "..\\..\\Event Manager\\CEventManager.h"
#include "..\\..\\Event Manager\\EventStructs.h"
#include "..\\..\\..\\..\\Enums.h"
#include "..\\..\\Sound Manager\CWwiseSoundManager.h"
#include"..\\..\\Unlockable Manager\\CUnlockableManager.h"
#include "CTextureManager.h"

using namespace EventStructs;

CIntroManager::CIntroManager() : m_pFalconSplash(NULL), m_pGPSplash(NULL),
m_pTM(NULL), m_pShatteredSplash(NULL), m_pWwiseSplash(NULL),
m_pOM(NULL), m_pIntroObj(NULL), m_fAlphaValue(1.0f),
m_fSeconds(0.0f), m_pTitleScreen(NULL), m_fTitleBarAlphValue(1.0f),
m_bEnterMenu(false), m_pHasPlayed(false)
{
}

CIntroManager::~CIntroManager(void)
{
}

//Loads all the assets for all the splash screens
void CIntroManager::InitIntro(void)
{
	nTexID1 = m_pTM->LoadTexture("Resource\\Splash Screens\\GPGlogo.png");
	nTexID2 = m_pTM->LoadTexture("Resource\\Splash Screens\\shatteredStudiosSplashConcept.tga");
	nTexID3 = m_pTM->LoadTexture("Resource\\Splash Screens\\wwiseSplash.tga");
	nTexID4 = m_pTM->LoadTexture("Resource\\Splash Screens\\FalconFishMascot.jpg");
	nTexID5 = m_pTM->LoadTexture("Resource\\HUD\\T_Title_Screen_D.png");
	nTexID6 = m_pTM->LoadTexture("Resource\\HUD\\T_Press_Start_D.png");

	m_pIntroObj = m_pOM->CreateObject("IntroSplash");
	////////////////////////////////////////////////////////////////////////////
	// Get Inital Sprite Data  for GP games splash screen
	////////////////////////////////////////////////////////////////////////////
	TSpriteData tSpriteData;

	tSpriteData.m_nTexture = nTexID1;
	tSpriteData.m_nX = 100;
	tSpriteData.m_nY = -10;
	tSpriteData.m_nZ = 1;
	tSpriteData.m_fScaleX = .2f;
	tSpriteData.m_fScaleY = .2f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	m_pGPSplash = m_pTM->CreateSpriteComp(m_pIntroObj, tSpriteData, 
		true);

	////////////////////////////////////////////////////////////////////////////
	// Get Inital Sprite Data  for Shattered Studios splash screen
	////////////////////////////////////////////////////////////////////////////
	tSpriteData.m_nTexture = nTexID2;
	tSpriteData.m_nX = 0;
	tSpriteData.m_nY = -100;
	tSpriteData.m_nZ = 1;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	m_pShatteredSplash = m_pTM->CreateSpriteComp(m_pIntroObj, tSpriteData, 
		false);
	////////////////////////////////////////////////////////////////////////////
	// Get Inital Sprite Data  for Wwise splash screen
	////////////////////////////////////////////////////////////////////////////
	tSpriteData.m_nTexture = nTexID3;
	tSpriteData.m_nX = 0;
	tSpriteData.m_nY = -100;
	tSpriteData.m_nZ = 1;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	m_pWwiseSplash = m_pTM->CreateSpriteComp(m_pIntroObj, tSpriteData, 
		false);
	////////////////////////////////////////////////////////////////////////////
	// Get Inital Sprite Data  for Falcon Fish splash screen
	////////////////////////////////////////////////////////////////////////////
	tSpriteData.m_nTexture = nTexID4;
	tSpriteData.m_nX = 275;
	tSpriteData.m_nY = 125;
	tSpriteData.m_nZ = 1;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = 0;
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	m_pFalconSplash = m_pTM->CreateSpriteComp(m_pIntroObj, tSpriteData, 
		false);
	////////////////////////////////////////////////////////////////////////////
	// Get Inital Sprite Data  for Title screen
	////////////////////////////////////////////////////////////////////////////
	tSpriteData.m_nTexture = nTexID5;
	tSpriteData.m_nX = 0;
	tSpriteData.m_nY = 0;
	tSpriteData.m_nZ = 1;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = 0;
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	m_pTitleScreen = m_pTM->CreateSpriteComp(m_pIntroObj, tSpriteData, 
		false);

	tSpriteData.m_nTexture = nTexID6;
	tSpriteData.m_nX = 275;
	tSpriteData.m_nY = 650;
	tSpriteData.m_nZ = 2;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = 0;
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	m_pPressEnter = m_pTM->CreateSpriteComp(m_pIntroObj, tSpriteData, 
		false);
}

void CIntroManager::Init(void)
{
	//Get Singletons
	m_pOM = CObjectManager::GetInstance();
	m_pTM = CTextureManager::GetInstance();

	InitIntro();
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	int eAssociatedState = INTRO_STATE;
	string szEventName = "Update";
	szEventName += (char)eAssociatedState; // catch update for only gameplay state

	pEM->RegisterEvent(szEventName,	(IComponent*)GetInstance(), UpdateCallback);


	pEM->RegisterEvent("EnterMainMenu", (IComponent*)GetInstance(), EnterMenu);

	szEventName = "ShutdownObjects";
	szEventName += (char)eAssociatedState;
	pEM->RegisterEvent(szEventName, (IComponent*)GetInstance(), Shutdown);

}

void CIntroManager::Update(void)
{
	m_dt += m_fDT;
	m_fSeconds += m_fDT;
	if (m_dt > 0.1f)
	{
		////////////////////////////////////////////////////////////////////////
		// Fade in and out for GP studios splash screen
		////////////////////////////////////////////////////////////////////////
		if (m_pGPSplash->IsActive())
		{
			if (m_fSeconds < 1)
			{
				TSpriteData data = m_pGPSplash->GetSpriteData();
				m_fAlphaValue += 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_dt = 0.0f;
				m_pGPSplash->SetSpriteData(data);
			}
			if(m_fSeconds > 1 && m_fSeconds < 2)
			{
				m_fAlphaValue = 1.0f;
				m_dt = 0.0f;
			}
			if (m_fSeconds > 2 && m_fSeconds < 3)
			{
				TSpriteData data = m_pGPSplash->GetSpriteData();
				m_fAlphaValue -= 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_dt = 0.0f;
				m_pGPSplash->SetSpriteData(data);
			}
			if (m_fSeconds > 3)
			{
				m_pGPSplash->SetActive(false);
				m_pShatteredSplash->SetActive(true);
				m_fAlphaValue = 0.0f;
				m_dt = 0.0f;
				m_fSeconds = 0.0f;

			}

		}
		////////////////////////////////////////////////////////////////////////
		//Fade in and out for Shattered Studios Screen
		////////////////////////////////////////////////////////////////////////
		if (m_pShatteredSplash->IsActive())
		{
			if (m_fSeconds < 1)
			{
				TSpriteData data = m_pShatteredSplash->GetSpriteData();
				m_fAlphaValue += 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_dt = 0.0f;
				m_pShatteredSplash->SetSpriteData(data);
			}
			if(m_fSeconds > 1 && m_fSeconds < 2)
			{
				m_fAlphaValue = 1.0f;
				m_dt = 0.0f;
			}
			if (m_fSeconds > 2 && m_fSeconds < 3)
			{
				TSpriteData data = m_pShatteredSplash->GetSpriteData();
				m_fAlphaValue -= 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_dt = 0.0f;
				m_pShatteredSplash->SetSpriteData(data);
			}
			if (m_fSeconds > 3)
			{
				m_pShatteredSplash->SetActive(false);
				m_pWwiseSplash->SetActive(true);
				m_fAlphaValue = 0.0f;
				m_dt = 0.0f;
				m_fSeconds = 0.0f;

			}

		}
		////////////////////////////////////////////////////////////////////////
		//Fade in and out for Wwise Splash Screen
		////////////////////////////////////////////////////////////////////////
		if (m_pWwiseSplash->IsActive())
		{
			if (m_fSeconds < 1)
			{
				TSpriteData data = m_pWwiseSplash->GetSpriteData();
				m_fAlphaValue += 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_dt = 0.0f;
				m_pWwiseSplash->SetSpriteData(data);
			}
			if(m_fSeconds > 1 && m_fSeconds < 2)
			{
				m_fAlphaValue = 1.0f;
				m_dt = 0.0f;
			}
			if (m_fSeconds > 2 && m_fSeconds < 3)
			{
				TSpriteData data = m_pWwiseSplash->GetSpriteData();
				m_fAlphaValue -= 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_dt = 0.0f;
				m_pWwiseSplash->SetSpriteData(data);
			}
			if (m_fSeconds > 3)
			{
				m_pWwiseSplash->SetActive(false);
				m_pFalconSplash->SetActive(true);
				m_fAlphaValue = 0.0f;
				m_dt = 0.0f;
				m_fSeconds = 0.0f;
				m_bEnterMenu = false;
			}

		}
		////////////////////////////////////////////////////////////////////////
		//Fade in and out for Falcon Fish Splash Screen
		////////////////////////////////////////////////////////////////////////
		if (m_pFalconSplash->IsActive())
		{
			if (m_fSeconds < 1)
			{
				TSpriteData data = m_pFalconSplash->GetSpriteData();
				m_fAlphaValue += 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_dt = 0.0f;
				m_pFalconSplash->SetSpriteData(data);
			}
			if(m_fSeconds > 1 && m_fSeconds < 2)
			{
				m_fAlphaValue = 1.0f;
				m_dt = 0.0f;
			}
			if (m_fSeconds > 2 && m_fSeconds < 3)
			{
				TSpriteData data = m_pFalconSplash->GetSpriteData();
				m_fAlphaValue -= 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_dt = 0.0f;
				m_pFalconSplash->SetSpriteData(data);
			}
			if (m_fSeconds > 3)
			{
				m_pFalconSplash->SetActive(false);
				m_pTitleScreen->SetActive(true);
				m_pPressEnter->SetActive(true);
				m_fAlphaValue = 0.0f;
				m_dt = 0.0f;
				m_fSeconds = 0.0f;
				m_bEnterMenu = false;
				PlayMusic();

			}
		}
		////////////////////////////////////////////////////////////////////////
		//Fade in and out for Title Screen
		////////////////////////////////////////////////////////////////////////
		if (m_pTitleScreen->IsActive())
		{
			if (m_fSeconds < 1)
			{
				TSpriteData data = m_pTitleScreen->GetSpriteData();
				m_fAlphaValue += 0.3f;
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_pTitleScreen->SetSpriteData(data);

				TSpriteData enter = m_pPressEnter->GetSpriteData();
				enter.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_pPressEnter->SetSpriteData(enter);

				m_dt = 0.0f;
			}
			if(m_fSeconds > 1 && m_fSeconds < 1.1)
			{
				m_fAlphaValue = 1.0f;
				m_dt = 0.0f;
			}
			else if (m_fSeconds > 1.1 && m_fSeconds < 1.2f )
			{
				m_fTitleBarAlphValue -= 0.5f;

				TSpriteData enter = m_pPressEnter->GetSpriteData();
				enter.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fTitleBarAlphValue);
				m_pPressEnter->SetSpriteData(enter);

				m_dt = 0.0f;
			}
			else if (m_fSeconds > 1.2f && m_fSeconds < 1.3f)
			{
				m_fTitleBarAlphValue += 0.5f;

				TSpriteData enter = m_pPressEnter->GetSpriteData();
				enter.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, m_fTitleBarAlphValue);
				m_pPressEnter->SetSpriteData(enter);

				m_dt = 0.0f;

			}
			else if (m_fSeconds > 1.3f)
			{
				if (m_bEnterMenu)
				{
					if (!m_pHasPlayed)
					{	
						m_pHasPlayed = true;
						CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_CASHREGISTER, GLOBAL_ID);
					}
					m_pPressEnter->SetActive(false);
					if(m_fSeconds > 1.3f && m_fSeconds < 2.3f)
					{
						m_fAlphaValue -= 0.1f;
						TSpriteData data = m_pTitleScreen->GetSpriteData();
						data.m_dwColor =D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
						m_pTitleScreen->SetSpriteData(data);
					}
					if(m_fSeconds > 2.3)
					{
						ChangeToMainMenu();
					}
				}
				else
				{
					m_fSeconds = 1.1f;
				}
			}
		}
	}


}

void CIntroManager::UpdateCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get values from the event
	TUpdateStateEvent* tEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDt = tEvent->m_fDeltaTime;
	GetInstance()->m_fDT = fDt;

	GetInstance()->Update();

}
////////////////////////////////////////////////////////////////////////
//Unload all the textures
////////////////////////////////////////////////////////////////////////
void CIntroManager::Shutdown(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	CIntroManager* pIM = CIntroManager::GetInstance();

	pIM->m_pWwiseSplash->SetActive(false);
	pIM->m_pGPSplash->SetActive(false);
	pIM->m_pShatteredSplash->SetActive(false);
	pIM->m_pFalconSplash->SetActive(false);
	pIM->m_pTitleScreen->SetActive(false);
	pIM->m_pPressEnter->SetActive(false);
	pIM->m_bEnterMenu =	false;
	pIM->m_pTM->UnloadTexture(pIM->nTexID1);
	pIM->m_pTM->UnloadTexture(pIM->nTexID2);
	pIM->m_pTM->UnloadTexture(pIM->nTexID3);
	pIM->m_pTM->UnloadTexture(pIM->nTexID4);
	pIM->m_pTM->UnloadTexture(pIM->nTexID5);
	pIM->m_pTM->UnloadTexture(pIM->nTexID6);
}
////////////////////////////////////////////////////////////////////////
//Makes sure you enter the Main menu correctly
////////////////////////////////////////////////////////////////////////
void CIntroManager::EnterMenu(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	CUnlockableManager::GetInstance()->SetIntroSkip(true);
	CUnlockableManager::GetInstance()->UpdateFile();
	GetInstance()->m_bEnterMenu = true;	
}
void CIntroManager::PlayMusic(void)
{
	CWwiseSoundManager::GetInstance()->PlayMusic(MENU_MUSIC_PLAY, GLOBAL_ID);
}
void CIntroManager::ChangeToMainMenu()
{
	CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_OPTION_CHANGE, GLOBAL_ID);
	SendStateEvent("StateChange", (IComponent*)GetInstance(),
		MAIN_MENU_STATE, PRIORITY_IMMEDIATE);
}