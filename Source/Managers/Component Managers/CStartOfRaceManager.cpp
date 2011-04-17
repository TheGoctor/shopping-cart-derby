#include "CStartOfRaceManager.h"

#include "..\Global Managers\Memory Manager\CMemoryManager.h"

#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CInputEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CUpdateStateEvent.h"
#include "..\..\Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"

#include "..\..\CObject.h"

#include "..\..\Components\StartOfRace\CStartOfRaceComponent.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"

CStartOfRaceManager::CStartOfRaceManager() : m_fCurrentTimerValue(0.0f), m_fTotalDuration(3.0f)
{
	// MACR DONT CHANGE ANY OF THIS SHIT FOR NEW GOC MODEL. 
	//	We only want this logic called once so we dont draw 4 things or play 4 sounds
	//		A component existing merely lets the system know to do this behavior


	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, NULL, HandleUpdate);	


	CWwiseSoundManager::GetInstance();
	// Load Sprite Sheet
	int nTexID0 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\Stoplight0.png", RGB(255,255,255));
	int nTexID1 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\Stoplight1.png", RGB(255,255,255));
	int nTexID2 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\Stoplight2.png", RGB(255,255,255));
	int nTexID3 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\Stoplight3.png", RGB(255,255,255));

	// Create the Object
	m_pDisplay0 = CObjectManager::GetInstance()->CreateObject("StartOfRaceDisplay0");
	m_pDisplay1 = CObjectManager::GetInstance()->CreateObject("StartOfRaceDisplay1");
	m_pDisplay2 = CObjectManager::GetInstance()->CreateObject("StartOfRaceDisplay2");
	m_pDisplay3 = CObjectManager::GetInstance()->CreateObject("StartOfRaceDisplay3");

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
	tSpriteData.m_dwColor = -1;//RGB(1.0f, 1.0f, 1.0f);
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

// Lua stuff
int CStartOfRaceManager::CreateStartOfRaceComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, 1);
	CStartOfRaceComponent* pSOR = CreateStartOfRaceComponent(pObj);
	
	lua_pop(pLua, 1);
	return 0;
}

CStartOfRaceComponent* CStartOfRaceManager::CreateStartOfRaceComponent(CObject* pObj)
{
	CStartOfRaceComponent* comp = MMNEW(CStartOfRaceComponent) CStartOfRaceComponent(pObj);

	comp->Init();	

	pObj->AddComponent(comp);

	CStartOfRaceManager::GetInstance()->m_lStartOfRaceComponents.push_back(comp);

	return comp;
}
//	//Lua Stuff


void CStartOfRaceManager::HandleUpdate(IEvent* cEvent, IComponent* cCenter)
{
	CUpdateStateEvent* pEv = static_cast<CUpdateStateEvent*>(cEvent);
	// get dt here
	float fDt = pEv->GetDeltaTime();

	// increment timer
		GetInstance()->m_fCurrentTimerValue += fDt;


	// disable input once
	static bool bInputDisabled = false;
	if(bInputDisabled)
	{

		
		bInputDisabled = true;
	}


	// if we have at least one component
	if(GetInstance()->m_lStartOfRaceComponents.size())
	{
		// do logic

		static bool bEnableInput = true;

		if(GetInstance()->m_fCurrentTimerValue < 4.0f)
		{
			// HACK: Disable input at start
			// change input state to intro (aka disable input)
			string szEvent = "InputStateChange";
			CStateEvent* pInputEvent = MMNEWEVENT(CStateEvent) CStateEvent(CIDGen::GetInstance()->
				GetID(szEvent), NULL, GAMEPLAY_STATE, INTRO_STATE);

			CEventManager::GetInstance()->PostEvent(pInputEvent, PRIORITY_NORMAL);
		}
		
		// if hud element change is needed
		if(GetInstance()->m_fCurrentTimerValue > 5.0f)
		{
			GetInstance()->m_lStartOfRaceComponents.clear();
		}
		if(GetInstance()->m_fCurrentTimerValue > 4.0f)		// 3rd light
		{
			// check/change hud element
			// play sound
			//Sound Hack//
			static bool bHasPlayed = false;
			if(!bHasPlayed)
			{
				bHasPlayed = true;
				CWwiseSoundManager::GetInstance()->PlaySound(MENU_SELECT, 1);
			}
			GetInstance()->m_pDisplayComponent3->SetActive(true);
			GetInstance()->m_pDisplayComponent2->SetActive(false);
			GetInstance()->m_pDisplayComponent1->SetActive(false);
			GetInstance()->m_pDisplayComponent0->SetActive(false);

			if(bEnableInput)
			{
			// re-enable the input
			string szEvent = "InputStateChange";
			//szEvent += EGameState::GAMEPLAY_STATE; // don't send that tacked on? put in event data
			// Send InputStateChange
			CStateEvent* pInputEvent = MMNEWEVENT(CStateEvent) CStateEvent(CIDGen::GetInstance()->
				GetID(szEvent), NULL, INTRO_STATE, GAMEPLAY_STATE);

			CEventManager::GetInstance()->PostEvent(pInputEvent, PRIORITY_INPUT);
			/**/

			// HACK: Don't access directly. This should be handled by the event above
			//CInputManager::InputStateChange(pInputEvent, NULL);
			// ENDH

			bEnableInput = false;
			}
		}
		else if(GetInstance()->m_fCurrentTimerValue > 3.0f) // 2nd light
		{
			// check/change hud element
			GetInstance()->m_pDisplayComponent3->SetActive(false);
			GetInstance()->m_pDisplayComponent2->SetActive(true);
			GetInstance()->m_pDisplayComponent1->SetActive(false);
			GetInstance()->m_pDisplayComponent0->SetActive(false);
			
			static bool bHasPlayed = false;
			if(!bHasPlayed)
			{
				bHasPlayed = true;
				CWwiseSoundManager::GetInstance()->PlaySound(MENU_OPTION_CHANGE, 1);
			}
			// play sound
		}
		else if(GetInstance()->m_fCurrentTimerValue > 2.0f) // 1st light
		{
			//change hud element
			GetInstance()->m_pDisplayComponent3->SetActive(false);
			GetInstance()->m_pDisplayComponent2->SetActive(false);
			GetInstance()->m_pDisplayComponent1->SetActive(true);
			GetInstance()->m_pDisplayComponent0->SetActive(false);
			static bool bHasPlayed = false;
			if(!bHasPlayed)
			{
				bHasPlayed = true;
				CWwiseSoundManager::GetInstance()->PlaySound(MENU_OPTION_CHANGE, 1);
			}
			// play sound
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
				CWwiseSoundManager::GetInstance()->PlaySound(MENU_OPTION_CHANGE, 1);
			}
			// play sound
		}
		else								// no light
		{
			// check/change hud element
			//GetInstance()->m_pDisplayComponent3->SetActive(!GetInstance()->m_pDisplayComponent1->IsActive());
			// play sound
		}

		// if timer is at point to start race
			// change input state to game

		// if timer is at total end
			// stop rendering hud stuff and clean up all that
			// remove the components from the objects and our list
			// obj.removecomp(comp*)
			//MMDEL(CStartOfRaceComponent, this); // clean up my memory
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