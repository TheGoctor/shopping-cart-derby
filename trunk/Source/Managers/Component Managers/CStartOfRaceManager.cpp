#include "CStartOfRaceManager.h"

#include "..\Global Managers\Memory Manager\CMemoryManager.h"

#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

#include "..\..\Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"

#include "..\..\CObject.h"

#include "..\..\Components\StartOfRace\CStartOfRaceComponent.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"

CStartOfRaceManager::CStartOfRaceManager() : m_fCurrentTimerValue(0.0f), 
	m_fTotalDuration(3.0f), m_bEnableInput(true)
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


	// Load Sprite Sheet
	int nTexID0 = CTextureManager::GetInstance()->LoadTexture(
		"Resource\\HUD\\Stoplight0.png", RGB(255,255,255));
	int nTexID1 = CTextureManager::GetInstance()->LoadTexture(
		"Resource\\HUD\\Stoplight1.png", RGB(255,255,255));
	int nTexID2 = CTextureManager::GetInstance()->LoadTexture(
		"Resource\\HUD\\Stoplight2.png", RGB(255,255,255));
	int nTexID3 = CTextureManager::GetInstance()->LoadTexture(
		"Resource\\HUD\\Stoplight3.png", RGB(255,255,255));

	// Create the Object
	m_pDisplay0 = CObjectManager::GetInstance()->CreateObject(
		"StartOfRaceDisplay0");
	m_pDisplay1 = CObjectManager::GetInstance()->CreateObject(
		"StartOfRaceDisplay1");
	m_pDisplay2 = CObjectManager::GetInstance()->CreateObject(
		"StartOfRaceDisplay2");
	m_pDisplay3 = CObjectManager::GetInstance()->CreateObject(
		"StartOfRaceDisplay3");

	// Get Inital Sprite Data
	TSpriteData tSpriteData;
	tSpriteData.m_nTexture = nTexID0;
	tSpriteData.m_nX = 400;
	tSpriteData.m_nY = 150;
	tSpriteData.m_fScaleX = .75f;
	tSpriteData.m_fScaleY = .75f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = (DWORD)-1;//RGB(1.0f, 1.0f, 1.0f);
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	// Add Sprite Comp
	m_pDisplayComponent0 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay3, tSpriteData, false);

	tSpriteData.m_nTexture = nTexID1;
	m_pDisplayComponent1 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay1, tSpriteData, false);
	
	tSpriteData.m_nTexture = nTexID2;
	m_pDisplayComponent2 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay2, tSpriteData, false);
	
	tSpriteData.m_nTexture = nTexID3;
	m_pDisplayComponent3 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay3, tSpriteData, false);
}



void CStartOfRaceManager::HandleUpdate(IEvent* cEvent, IComponent* cCenter)
{
	TUpdateStateEvent* pEv = static_cast<TUpdateStateEvent*>(cEvent->GetData());
	// get dt here
	float fDt = pEv->m_fDeltaTime;


	// if we have at least one component
	if(GetInstance()->m_fCurrentTimerValue < 3.0f)
	{
	// increment timer
		GetInstance()->m_fCurrentTimerValue += fDt;

		// do logic
		GetInstance()->m_bEnableInput = true;

		if(GetInstance()->m_fCurrentTimerValue < 2.7f)
		{
			// change input state to intro (aka disable input)
			SendStateEvent("InputStateChange", (IComponent*)GetInstance(),
				INTRO_STATE, PRIORITY_NORMAL);
		}
		
		if(GetInstance()->m_fCurrentTimerValue > 2.7f)		// 3rd light
		{
			// check/change hud element
			// play sound
		
			GetInstance()->m_pDisplayComponent3->SetActive(true);
			GetInstance()->m_pDisplayComponent2->SetActive(false);
			GetInstance()->m_pDisplayComponent1->SetActive(false);
			GetInstance()->m_pDisplayComponent0->SetActive(false);

			if(GetInstance()->m_bEnableInput)
			{
			// re-enable the input
				SendStateEvent("InputStateChange", (IComponent*)GetInstance(),
					GAMEPLAY_STATE);

				GetInstance()->m_bEnableInput = false;
			}
		}
		else if(GetInstance()->m_fCurrentTimerValue > 2.0f) // 2nd light
		{
			// check/change hud element
			GetInstance()->m_pDisplayComponent3->SetActive(false);
			GetInstance()->m_pDisplayComponent2->SetActive(true);
			GetInstance()->m_pDisplayComponent1->SetActive(false);
			GetInstance()->m_pDisplayComponent0->SetActive(false);
			
		}
		else if(GetInstance()->m_fCurrentTimerValue > 1.5f) // 1st light
		{
			//change hud element
			GetInstance()->m_pDisplayComponent3->SetActive(false);
			GetInstance()->m_pDisplayComponent2->SetActive(false);
			GetInstance()->m_pDisplayComponent1->SetActive(true);
			GetInstance()->m_pDisplayComponent0->SetActive(false);
	
		}
		else if(GetInstance()->m_fCurrentTimerValue > 1.0f) // 1st light
		{
			//change hud element
			GetInstance()->m_pDisplayComponent3->SetActive(false);
			GetInstance()->m_pDisplayComponent2->SetActive(false);
			GetInstance()->m_pDisplayComponent1->SetActive(false);
			GetInstance()->m_pDisplayComponent0->SetActive(true);
			static bool bHasPlayed = false;
			if(!bHasPlayed)
			{
				bHasPlayed = true;
				//Sound Hack
				CWwiseSoundManager::GetInstance()->PlayTheSound(START_COUNTDOWN, GLOBAL_ID);
			}
			// play sound
		}
	}
	else
	{
		// Unshow the component
		GetInstance()->m_pDisplayComponent3->SetActive(false);
		GetInstance()->m_pDisplayComponent2->SetActive(false);
		GetInstance()->m_pDisplayComponent1->SetActive(false);
		GetInstance()->m_pDisplayComponent0->SetActive(false);
	}
}

void CStartOfRaceManager::HandleStateEnter(IEvent*, IComponent*)
{
}

void CStartOfRaceManager::HandleStateExit(IEvent*, IComponent*)
{
}


void CStartOfRaceManager::HandleStateInit(IEvent* cEvent, IComponent* cCenter)
{
	CStartOfRaceManager::GetInstance()->m_fCurrentTimerValue = 0.0f;
	CStartOfRaceManager::GetInstance()->m_bEnableInput = true;
}
