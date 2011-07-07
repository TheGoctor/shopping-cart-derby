////////////////////////////////////////////////////////////////////////////////
//  File			:	"CMenuTransition.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	06/02/11
//
//	Last Changed	:	06/02/11
//
//	Changed By		:				
//
//  Purpose			:	Wrapper class for manageing Menu transitions screen
////////////////////////////////////////////////////////////////////////////////
#include "MenuTransition.h"
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
#include "..\Button\CButtonComponent.h"
#include "..\\..\\Enums.h"

CMenuTransition::CMenuTransition() : m_pTM(NULL), m_dt(0.0f), m_fDT(0.0f),
m_fSeconds(0.0f), m_pOM(NULL), m_pFontImage(NULL),
m_pMenuObj(NULL), faded(false), m_fAlphaValue(1.0f), m_bStartFading(false),
m_bGameMode(false), m_bCredits(false), m_bOptions(false), m_bHowToPlay(false)
{
}

CMenuTransition::~CMenuTransition(void)
{
}

CMenuTransition& CMenuTransition::operator=(const CMenuTransition&)
{
	return *this;
}

void CMenuTransition::InitMenu(void)
{

	int nTexID2 = m_pTM->LoadTexture("Resource\\HUD\\T_Main_Menu_D.png");
	m_pMenuObj = m_pOM->CreateObject("MenuBackground");

	TSpriteData tSpriteData;

	tSpriteData.m_nTexture = nTexID2;
	tSpriteData.m_nX = 0;
	tSpriteData.m_nY = 0;
	tSpriteData.m_nZ = 1;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = -1;
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	m_pFontImage = m_pTM->CreateSpriteComp(m_pMenuObj, tSpriteData, 
		false);

}

void CMenuTransition::Init(void)
{
	//Get Singletons
	m_pOM = CObjectManager::GetInstance();
	m_pTM = CTextureManager::GetInstance();

	InitMenu();
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();

	string szEventName = "Update";
	szEventName += MAIN_MENU_STATE; //
	pEM->RegisterEvent(szEventName,	(IComponent*)GetInstance(), 
		UpdateCallback);

	szEventName = "EnableObjects";
	szEventName += MAIN_MENU_STATE;

	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), EnableMenuImage);

	szEventName = "InitObjects";
	szEventName += MAIN_MENU_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), EnableMenuImage);
	
	szEventName = "DisableObjects";
	szEventName += MAIN_MENU_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), Shutdown);

	

	pEM->RegisterEvent("TransToGameMode",	(IComponent*)GetInstance(), GoToGameModeCallback);
	pEM->RegisterEvent("TransToOption",	(IComponent*)GetInstance(), GoToOptionsCallback);
	pEM->RegisterEvent("TransToHowToPlay",	(IComponent*)GetInstance(), GoToHowToPlayCallback);
	pEM->RegisterEvent("TransToCredit",	(IComponent*)GetInstance(), GoToCreditCallback);
}

void CMenuTransition::Update(void)
{
	m_dt += m_fDT;

	if (m_dt > 0.1f)
	{	
		if (StartFading()== true)
		{		
			m_fSeconds += m_fDT;
			if(m_fSeconds < 1.0f)
			{	
				m_fAlphaValue -= 0.1f;
				TSpriteData data = m_pFontImage->GetSpriteData();
				data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue);
				m_pFontImage->SetSpriteData(data);

				if (m_fAlphaValue <= 0.0f)
				{
					faded = true;
					SetStartFading(false);
					if (m_bGameMode == true)
					{
						SendStateEvent("PushState", (IComponent*)GetInstance(),
							GAME_MODE_SELECT_STATE, PRIORITY_NORMAL);
					}
					if (m_bOptions == true)
					{
						SendStateEvent("PushState", (IComponent*)GetInstance(),
							OPTIONS_STATE, PRIORITY_NORMAL);
					}
					if (m_bCredits == true)
					{
						SendStateEvent("PushState", (IComponent*)GetInstance(),
							CREDITS_STATE, PRIORITY_NORMAL);
					}
					if (m_bHowToPlay == true)
					{
						SendStateEvent("PushState", (IComponent*)GetInstance(),
							HOW_TO_PLAY_STATE, PRIORITY_NORMAL);
					}
				}

				m_dt = 0.0f;
			}
			else if(m_fSeconds >= 1.0f)
			{		
				m_fSeconds = 0.0f;
			}
		}
	}
	if (DoneFading() == true)
	{
		GetInstance()->m_pFontImage->SetActive(false);

	}
}

void CMenuTransition::UpdateCallback(IEvent* pEvent, IComponent* /*pComp*/)
{

	// Get values from the event
	TUpdateStateEvent* tEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDt = tEvent->m_fDeltaTime;
	GetInstance()->m_fDT = fDt;

	GetInstance()->Update();

}
void CMenuTransition::Shutdown(IEvent* pEvent, IComponent* pComp)
{
	GetInstance()->m_pFontImage->SetActive(false);
}

void CMenuTransition::GoToGameModeCallback(IEvent* pEvent, IComponent* pComp)
{
	GetInstance()->SetStartFading(true);
	GetInstance()->m_bGameMode = true;
}
void CMenuTransition::GoToOptionsCallback(IEvent* pEvent, IComponent* pComp)
{
	GetInstance()->SetStartFading(true);
	GetInstance()->m_bOptions = true;
}
void CMenuTransition::GoToHowToPlayCallback(IEvent* pEvent, IComponent* pComp)
{
	GetInstance()->SetStartFading(true);
	GetInstance()->m_bHowToPlay = true;
}
void CMenuTransition::GoToCreditCallback(IEvent* pEvent, IComponent* pComp)
{
	GetInstance()->SetStartFading(true);
	GetInstance()->m_bCredits = true;
}
void CMenuTransition::EnableMenuImage(IEvent* pEvent, IComponent* pComp)
{
	GetInstance()->Reset();
}
void CMenuTransition::Reset()
{
	TSpriteData data = GetInstance()->m_pFontImage->GetSpriteData();
	data.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	GetInstance()->m_pFontImage->SetSpriteData(data);

	GetInstance()->m_fAlphaValue = 1.0f;
	GetInstance()->SetFaded(false);
	GetInstance()->m_fSeconds = 0.0f;
	GetInstance()->SetStartFading(false);
	GetInstance()->m_bCredits = false;
	GetInstance()->m_bGameMode = false;
	GetInstance()->m_bHowToPlay = false;
	GetInstance()->m_bOptions = false;

	GetInstance()->m_pFontImage->SetActive(true);

}
