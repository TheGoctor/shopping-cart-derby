#include "CHasWonComponent.h"


#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CUpdateStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CIDGen.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"

// TODO: Movement needeD?
#include "..\Movement\CMovement.h"

#define DELAYBETWEENINPUT .2f

CHasWonComponent::CHasWonComponent(CObject* pObj) : m_pObject(pObj)
{
	m_pObject = pObj;
}

// Lua stuff
int CHasWonComponent::CreateHasWonComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, 1);
	CHasWonComponent* pSOR = CreateHasWonComponent(pObj);
	
	lua_pop(pLua, 1);
	return 0;
}

CHasWonComponent* CHasWonComponent::CreateHasWonComponent(CObject* pObj)
{
	CHasWonComponent* comp = MMNEW(CHasWonComponent) CHasWonComponent(pObj);

	comp->Init();	

	pObj->AddComponent(comp);

	return comp;
}
//	//Lua Stuff




void CHasWonComponent::Init()
{
	// listens for these events
	CEventManager::GetInstance()->RegisterEvent("UpdateState", this, Update);
	CEventManager::GetInstance()->RegisterEvent("Left", this, LeftPressed);
	CEventManager::GetInstance()->RegisterEvent("Right", this, RightPressed);
	CEventManager::GetInstance()->RegisterEvent("Accept", this, SelectPressed);
	
	// HACK: Change this to handle based on player collision
	CEventManager::GetInstance()->RegisterEvent("Menu", this, GameWon);
	
	m_bDisplayingWinScreen = false;
	m_nMenuSelection = 0;
	m_fTimeSinceLastInput = 0;

	// Init texture stuff
	int nTexID0 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\WinScreen.png", RGB(255,255,255));
	int nTexID1 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\WinScreenSelector.png", RGB(255,255,255));

	// Create the Object
	m_pDisplay0 = CObjectManager::GetInstance()->CreateObject("StartOfRaceDisplay0");
	m_pDisplay1 = CObjectManager::GetInstance()->CreateObject("StartOfRaceDisplay1");

	// Get Inital Sprite Data
	
	m_tSpriteData0.m_nTexture = nTexID0;
	m_tSpriteData0.m_nX = 150;
	m_tSpriteData0.m_nY = 0;
	m_tSpriteData0.m_fScaleX = 1.0f;
	m_tSpriteData0.m_fScaleY = 1.0;
	m_tSpriteData0.m_fRotCenterX = 0.0f;
	m_tSpriteData0.m_fRotCenterY = 0.0f;
	m_tSpriteData0.m_fRot = 0.0f;
	m_tSpriteData0.m_dwColor = -1;//RGB(1.0f, 1.0f, 1.0f);
	m_tSpriteData0.m_tRect.top    = 0;
	m_tSpriteData0.m_tRect.right  = 0;
	m_tSpriteData0.m_tRect.left   = 0;
	m_tSpriteData0.m_tRect.bottom = 0;

	m_tSpriteData1.m_nTexture = nTexID1;
	m_tSpriteData1.m_nX = 267;
	m_tSpriteData1.m_nY = 317;
	m_tSpriteData1.m_fScaleX = 1.0f;
	m_tSpriteData1.m_fScaleY = 1.0;
	m_tSpriteData1.m_fRotCenterX = 0.0f;
	m_tSpriteData1.m_fRotCenterY = 0.0f;
	m_tSpriteData1.m_fRot = 0.0f;
	m_tSpriteData1.m_dwColor = -1;//RGB(1.0f, 1.0f, 1.0f);
	m_tSpriteData1.m_tRect.top    = 0;
	m_tSpriteData1.m_tRect.right  = 0;
	m_tSpriteData1.m_tRect.left   = 0;
	m_tSpriteData1.m_tRect.bottom = 0;


	// Add Sprite Comp
	m_pDisplayComponent0 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay0, m_tSpriteData0, false);
	m_pDisplayComponent1 = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplay1, m_tSpriteData1, false);


}


///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CHasWonComponent::Update(IEvent* cEvent, IComponent* cCenter)
{
	CUpdateStateEvent* eEvent = (CUpdateStateEvent*)cEvent;
	float fDt = eEvent->GetDeltaTime();

	CHasWonComponent* comp = (CHasWonComponent*)cCenter;
	if(comp->m_bDisplayingWinScreen) // might be hack since this wont be created til we want it anyway
	{
		comp->m_fTimeSinceLastInput += fDt;
		comp->m_pDisplayComponent0->SetActive(true);
		comp->m_pDisplayComponent1->SetActive(true);


		if(comp->m_nMenuSelection == 0)
		{
			comp->m_tSpriteData1.m_nX = 267;
		}
		else if(comp->m_nMenuSelection == 1)
		{
			comp->m_tSpriteData1.m_nX = 606;
		}

		comp->m_pDisplayComponent1->SetSpriteData(comp->m_tSpriteData1);
	}
}


// This function is a hack. Ideally we'll lnever need to call this and 
// collision with the register will create a component and initiate the stuff in here
void CHasWonComponent::GameWon(IEvent* cEvent, IComponent* cCenter)
{
	// if one object is the player cart and one object is the cash register
	//CObject* pObj = NULL; // = grab player from event
	//CreateHasWonComponent(pObj);

	CHasWonComponent* comp = (CHasWonComponent*)cCenter;


	// change input state to intro (aka disable input)
	// HACK: Disable input at start
	string szEvent = "InputStateChange";
	//szEvent += EGameState::INTRO_STATE;
	// Send InputStateChange
	// Send InputStateChange
	CStateEvent* pStateEvent = MMNEWEVENT(CStateEvent) CStateEvent(CIDGen::GetInstance()->
		GetID(szEvent), NULL, GAMEPLAY_STATE, MAIN_MENU_STATE);

	CEventManager::GetInstance()->PostEvent(pStateEvent, PRIORITY_NORMAL);
	
	//HACK AGAIN: Don't access directly. Should be handled by event above
	CInputManager::InputStateChange(pStateEvent, NULL);
	// ENDH

	comp->m_bDisplayingWinScreen = true;
}


void CHasWonComponent::LeftPressed(IEvent* cEvent, IComponent* cCenter)
{
	CHasWonComponent* comp = (CHasWonComponent*)cCenter;
		
	if(comp->m_bDisplayingWinScreen && 
		comp->m_fTimeSinceLastInput > DELAYBETWEENINPUT)
	{
		comp->m_nMenuSelection++;
		if(comp->m_nMenuSelection > 1)
		{
			comp->m_nMenuSelection = 0;
		}
		comp->m_fTimeSinceLastInput = 0;
	}
}

void CHasWonComponent::RightPressed(IEvent* cEvent, IComponent* cCenter)
{
	CHasWonComponent* comp = (CHasWonComponent*)cCenter;
		
	if(comp->m_bDisplayingWinScreen &&
		comp->m_fTimeSinceLastInput > DELAYBETWEENINPUT)
	{
		comp->m_nMenuSelection--;
		if(comp->m_nMenuSelection < 0)
		{
			comp->m_nMenuSelection = 1;
		}
		comp->m_fTimeSinceLastInput = 0;
	}

}

void CHasWonComponent::SelectPressed(IEvent* cEvent, IComponent* cCenter)
{
	CHasWonComponent* comp = (CHasWonComponent*)cCenter;
	
	if(comp->m_nMenuSelection == 1) // selected quit
	{
		string szEvent = "ShutdownGame";
		CStateEvent* pStateEvent = MMNEWEVENT(CStateEvent) CStateEvent(CIDGen::GetInstance()->
			GetID(szEvent), NULL, GAMEPLAY_STATE, MAIN_MENU_STATE);

		CEventManager::GetInstance()->PostEvent(pStateEvent, PRIORITY_NORMAL);
	}
}