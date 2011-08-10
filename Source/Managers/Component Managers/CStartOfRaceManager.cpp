#include "CStartOfRaceManager.h"

#include "..\Global Managers\Memory Manager\CMemoryManager.h"

#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

#include "..\..\Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"
#include "..\..\Managers\Global Managers\Rendering Managers\DXRenderContextManager.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"

#include "..\..\CObject.h"

#include "..\..\Components\StartOfRace\CStartOfRaceComponent.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\Managers\Global Managers\Rendering Managers\Texture Managers\CHUDManager.h"

CStartOfRaceManager::CStartOfRaceManager() : m_fCurrentTimerValue(0.0f), 
	m_fTotalDuration(3.0f), m_bEnableInput(true), m_bBikeStarted(false), m_pStartingLightContex(NULL), m_bWaitingForInput(false)
{
	string szEventName = "Update";
	szEventName += (char)GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleUpdate);	
	
	szEventName = "EnableObjects";
	szEventName += (char)GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateEnter);	

	szEventName = "DisableObjects";
	szEventName += (char)GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateExit);	

	szEventName = "InitObjects";
	szEventName += (char)GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateInit);	

	CEventManager::GetInstance()->RegisterEvent("GameplayEnterPressed", 
		(IComponent*)GetInstance(), EnterPressed);	

	


	GetInstance()->m_bHasPlayedSound = false;
	GetInstance()->m_bWelcomeSound = false;
	GetInstance()->pSound = CWwiseSoundManager::GetInstance();
	// Get Starting Light Context
	m_pStartingLightContex = DXRenderContextManager::GetInstance()->GetContext(RC_STARTLIGHT);

	TSpriteData tListData;

	tListData.m_nTexture = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\HowToPlay\\T_Start_How_To_Play_D.png");
	tListData.m_nX = 0;
	tListData.m_nY = 0;
	tListData.m_nZ = 0;
	tListData.m_fScaleX = 1.0f;
	tListData.m_fScaleY = 1.0f;
	tListData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	tListData.m_tRect.top    = 0;
	tListData.m_tRect.right  = 0;
	tListData.m_tRect.left   = 0;
	tListData.m_tRect.bottom = 0;
	tListData.m_fRotCenterX = 0.0f;
	tListData.m_fRotCenterY = 0.0f;
	tListData.m_fRot = 0.0f;

	// Get Inital Sprite Data
	CObject* pObj = CObjectManager::GetInstance()->CreateObject("DirectionsOverlay"); // TODO: Clean this up?

	m_pDirectionsComp = CTextureManager::GetInstance()->CreateSpriteComp(
		pObj, tListData, false);

}



void CStartOfRaceManager::HandleUpdate(IEvent* cEvent, IComponent*)
{
	TUpdateStateEvent* pEv = static_cast<TUpdateStateEvent*>(cEvent->GetData());
	// get dt here
	float fDt = pEv->m_fDeltaTime;

	if(GetInstance()->m_bWaitingForInput)
	{
		return;
	}
	
	if(!GetInstance()->m_bWelcomeSound)
	{
		GetInstance()->pSound->PlayTheSound(WELCOME_TO, GLOBAL_ID);

		for (unsigned int playernum = 0; playernum < 4; ++playernum)
		{
			GetInstance()->pSound->SetRPMValueForSound(0, 
				CHUDManager::GetInstance()->GetPlayerCharID(playernum));
		}

		GetInstance()->pSound->PlayCartSounds();
		GetInstance()->m_bWelcomeSound = true;
	}


	// if we're in the countdown
	if(GetInstance()->m_fCurrentTimerValue < 5.0f)
	{
	// increment timer
		GetInstance()->m_fCurrentTimerValue += fDt;

		// do logic
		GetInstance()->m_bEnableInput = true;

		if(GetInstance()->m_fCurrentTimerValue < 4.0f)
		{
			// change input state to intro (aka disable input)
			SendIEvent("DisableMovement", (IComponent*)GetInstance(),
				NULL, PRIORITY_NORMAL);
		}
		
		if(GetInstance()->m_fCurrentTimerValue > 4.0f)		// 3rd light
		{
			// check/change hud element
			// play sound
			GetInstance()->m_pStartingLightContex->SetTexIdx(5);
		
			

			if(GetInstance()->m_bEnableInput)
			{
			// re-enable the input
				SendIEvent("EnableMovement", (IComponent*)GetInstance(),
				NULL, PRIORITY_NORMAL);

				GetInstance()->m_bEnableInput = false;

				SendIEvent("RaceStarted", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);
				
			}
		}
		else if(GetInstance()->m_fCurrentTimerValue > 3.0f) // 2nd light
		{
			GetInstance()->m_pStartingLightContex->SetTexIdx(4);

		}
		else if(GetInstance()->m_fCurrentTimerValue > 2.0f) // 1st light
		{
			GetInstance()->m_pStartingLightContex->SetTexIdx(3);
	
		}
		else if(GetInstance()->m_fCurrentTimerValue > 1.0f ) // 1st light
		{
			GetInstance()->m_pStartingLightContex->SetTexIdx(2);
			
			if(!GetInstance()->m_bHasPlayedSound)
			{
				GetInstance()->pSound->PlayTheSound(START_COUNTDOWN, GLOBAL_ID);
				GetInstance()->m_bHasPlayedSound = true;
			}
			
		}
	}
	else
	{
		
	}
}

void CStartOfRaceManager::HandleStateEnter(IEvent*, IComponent*)
{
	
}

void CStartOfRaceManager::HandleStateExit(IEvent*, IComponent*)
{
	
}

void CStartOfRaceManager::HandleStateInit(IEvent*, IComponent*)
{
	CStartOfRaceManager::GetInstance()->m_fCurrentTimerValue = -5.0f;
	CStartOfRaceManager::GetInstance()->m_bEnableInput = true;
	CStartOfRaceManager::GetInstance()->m_bHasPlayedSound = false;
	CStartOfRaceManager::GetInstance()->m_bWelcomeSound = false;
	CStartOfRaceManager::GetInstance()->m_bBikeStarted = false;
	CStartOfRaceManager::GetInstance()->m_pStartingLightContex->SetTexIdx(1);
	CStartOfRaceManager::GetInstance()->m_pDirectionsComp->SetActive(true);
	GetInstance()->m_bWaitingForInput = true;

	// change input state to intro (aka disable input)
	SendIEvent("DisableMovement", (IComponent*)GetInstance(),
		NULL, PRIORITY_NORMAL);
}


void CStartOfRaceManager::EnterPressed(IEvent*, IComponent*)
{
	GetInstance()->m_bWaitingForInput = false;

	// Unshow the directions overlay
	GetInstance()->m_pDirectionsComp->SetActive(false);	
	SendIEvent("OverlayUnshow", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);
}
