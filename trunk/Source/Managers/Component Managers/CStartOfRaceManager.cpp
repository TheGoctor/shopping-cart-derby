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

CStartOfRaceManager::CStartOfRaceManager() : m_fCurrentTimerValue(0.0f), 
	m_fTotalDuration(3.0f), m_bEnableInput(true), m_bBikeStarted(false), m_pStartingLightContex(NULL)
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
	//int nTexID0 = CTextureManager::GetInstance()->LoadTexture(
	//	"Resource\\HUD\\Stoplight0.png", RGB(255,255,255));
	//int nTexID1 = CTextureManager::GetInstance()->LoadTexture(
	//	"Resource\\HUD\\Stoplight1.png", RGB(255,255,255));
	//int nTexID2 = CTextureManager::GetInstance()->LoadTexture(
	//	"Resource\\HUD\\Stoplight2.png", RGB(255,255,255));
	//int nTexID3 = CTextureManager::GetInstance()->LoadTexture(
	//	"Resource\\HUD\\Stoplight3.png", RGB(255,255,255));

	//// Create the Object
	//m_pDisplay0 = CObjectManager::GetInstance()->CreateObject(
	//	"StartOfRaceDisplay0");
	//m_pDisplay1 = CObjectManager::GetInstance()->CreateObject(
	//	"StartOfRaceDisplay1");
	//m_pDisplay2 = CObjectManager::GetInstance()->CreateObject(
	//	"StartOfRaceDisplay2");
	//m_pDisplay3 = CObjectManager::GetInstance()->CreateObject(
	//	"StartOfRaceDisplay3");

	//// Get Inital Sprite Data
	//TSpriteData tSpriteData;
	//tSpriteData.m_nTexture = nTexID0;
	//tSpriteData.m_nX = 400;
	//tSpriteData.m_nY = 150;
	//tSpriteData.m_fScaleX = .75f;
	//tSpriteData.m_fScaleY = .75f;
	//tSpriteData.m_fRotCenterX = 0.0f;
	//tSpriteData.m_fRotCenterY = 0.0f;
	//tSpriteData.m_fRot = 0.0f;
	//tSpriteData.m_dwColor = (DWORD)-1;//RGB(1.0f, 1.0f, 1.0f);
	//tSpriteData.m_tRect.top    = 0;
	//tSpriteData.m_tRect.right  = 0;
	//tSpriteData.m_tRect.left   = 0;
	//tSpriteData.m_tRect.bottom = 0;

	//// Add Sprite Comp
	//m_pDisplayComponent0 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay3, tSpriteData, false);

	//tSpriteData.m_nTexture = nTexID1;
	//m_pDisplayComponent1 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay1, tSpriteData, false);
	//
	//tSpriteData.m_nTexture = nTexID2;
	//m_pDisplayComponent2 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay2, tSpriteData, false);
	//
	//tSpriteData.m_nTexture = nTexID3;
	//m_pDisplayComponent3 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay3, tSpriteData, false);

	GetInstance()->m_bHasPlayedSound = false;

	// Get Starting Light Context
	m_pStartingLightContex = DXRenderContextManager::GetInstance()->GetContext(RC_STARTLIGHT);
}



void CStartOfRaceManager::HandleUpdate(IEvent* cEvent, IComponent*)
{
	TUpdateStateEvent* pEv = static_cast<TUpdateStateEvent*>(cEvent->GetData());
	// get dt here
	float fDt = pEv->m_fDeltaTime;



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
		
			/*GetInstance()->m_pDisplayComponent3->SetActive(true);
			GetInstance()->m_pDisplayComponent2->SetActive(false);
			GetInstance()->m_pDisplayComponent1->SetActive(false);
			GetInstance()->m_pDisplayComponent0->SetActive(false);*/

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

			// check/change hud element
			/*GetInstance()->m_pDisplayComponent3->SetActive(false);
			GetInstance()->m_pDisplayComponent2->SetActive(true);
			GetInstance()->m_pDisplayComponent1->SetActive(false);
			GetInstance()->m_pDisplayComponent0->SetActive(false);*/

			if (!GetInstance()->m_bBikeStarted)
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(BULLDOG_BIKE_START, BIKER_ID);
				
				GetInstance()->m_bBikeStarted = true;
			}
			
			
		}
		else if(GetInstance()->m_fCurrentTimerValue > 2.0f) // 1st light
		{
			GetInstance()->m_pStartingLightContex->SetTexIdx(3);

			//change hud element
			/*GetInstance()->m_pDisplayComponent3->SetActive(false);
			GetInstance()->m_pDisplayComponent2->SetActive(false);
			GetInstance()->m_pDisplayComponent1->SetActive(true);
			GetInstance()->m_pDisplayComponent0->SetActive(false);*/
	
		}
		else if(GetInstance()->m_fCurrentTimerValue > 1.0f ) // 1st light
		{
			GetInstance()->m_pStartingLightContex->SetTexIdx(2);

			//change hud element
			/*GetInstance()->m_pDisplayComponent3->SetActive(false);
			GetInstance()->m_pDisplayComponent2->SetActive(false);
			GetInstance()->m_pDisplayComponent1->SetActive(false);
			GetInstance()->m_pDisplayComponent0->SetActive(true);*/
			
			if(!GetInstance()->m_bHasPlayedSound)
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(START_COUNTDOWN, GLOBAL_ID);
				GetInstance()->m_bHasPlayedSound = true;
			}
			// play sound
		}
	}
	else
	{
		// Unshow the component
		/*GetInstance()->m_pDisplayComponent3->SetActive(false);
		GetInstance()->m_pDisplayComponent2->SetActive(false);
		GetInstance()->m_pDisplayComponent1->SetActive(false);
		GetInstance()->m_pDisplayComponent0->SetActive(false);*/
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
	CStartOfRaceManager::GetInstance()->m_fCurrentTimerValue = -2.5f;
	CStartOfRaceManager::GetInstance()->m_bEnableInput = true;
	CStartOfRaceManager::GetInstance()->m_bHasPlayedSound = false;
	CStartOfRaceManager::GetInstance()->m_bBikeStarted = false;
	CStartOfRaceManager::GetInstance()->m_pStartingLightContex->SetTexIdx(1);
}