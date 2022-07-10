#include "CButtonComponent.h"


#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CIDGen.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\Managers\Global Managers\Console Manager\CConsoleManager.h"
using namespace EventStructs;


// pixels for offset:
#define BUTTON_LOCATION_OFFSET_X -75
#define BUTTON_LOCATION_OFFSET_Y -30


CButtonComponent::CButtonComponent(CObject* pObj) : m_pObject(pObj), m_pSelectDown(NULL), m_pSelectUp(NULL),
					m_pSelectLeft(NULL), m_pSelectRight(NULL)
{

}

// Lua stuff
int CButtonComponent::CreateButtonComponent(lua_State* pLua)
{
	// top down is in order in param list. Start at bottom with -1 and work your way to the more negative numbers
	CObject* pObj = (CObject*)lua_topointer(pLua, -9);
	string szEventName = (string)lua_tostring(pLua, -8);
	string szOnScrolloverFunc = (string)lua_tostring(pLua, -7);
	string szButtonTexName = (string)lua_tostring(pLua, -6);
	int nX = lua_tointeger(pLua, -5);
	int nY = lua_tointeger(pLua, -4);
	bool bStartSelected = (lua_toboolean(pLua, -3) != 0);
	int eState = lua_tointeger(pLua, -2);
	int nTextureDepth = lua_tointeger(pLua, -1);

	CButtonComponent* pButton = CreateButtonComponent(pObj, szEventName, szOnScrolloverFunc, szButtonTexName, nX, nY, bStartSelected, eState, nTextureDepth);

	lua_pop(pLua, 9);

	lua_pushlightuserdata(pLua, pButton);


	return 1;
}

CButtonComponent* CButtonComponent::CreateButtonComponent(CObject* pObj, string szEventName, string szOnRolloverFunc, string szSpriteTextureName, 
														  int nPosX, int nPosY, bool bStartSelected, int eGameState, int nTextureDepth)
{
	CButtonComponent* comp = MMNEW(CButtonComponent(pObj));

	comp->m_szSelectionFunc = szEventName;
	comp->m_szOnScrolloverFunc = szOnRolloverFunc;
	comp->m_eAssociatedState = (EGameState)eGameState;


	comp->Init(szSpriteTextureName, nTextureDepth);
	comp->m_bSelected = bStartSelected;
	comp->m_bSelectedStartValue = bStartSelected;


	// listens for these events
	string szEvent = "EnableObjects";
	szEvent += (char)comp->m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, ButtonStateEnable);

	szEvent = "DisableObjects";
	szEvent += (char)comp->m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, ButtonStateDisable);
	
	szEvent = "InitObjects";
	szEvent += (char)comp->m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, ButtonStateInit);
	
	szEvent = "ShutdownObjects";
	szEvent += (char)comp->m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, ButtonStateDisable);

	// HACK - update to gameplay to disable buttons to fix creeper  bug
	szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, GameplayUpdate);

	comp->Load(szSpriteTextureName, nTextureDepth, nPosX, nPosY);

	pObj->AddComponent(comp);



	return comp;
}


int CButtonComponent::SetNextButtonComponent(lua_State* pLua)
{
	CButtonComponent* pMe = (CButtonComponent*)lua_topointer(pLua, -5);
	CButtonComponent* pDown = (CButtonComponent*)lua_topointer(pLua, -4);
	CButtonComponent* pUp = (CButtonComponent*)lua_topointer(pLua, -3);
	CButtonComponent* pLeft = (CButtonComponent*)lua_topointer(pLua, -2);
	CButtonComponent* pRight = (CButtonComponent*)lua_topointer(pLua, -1);
	SetNextButtonComponent(pMe, pDown, pUp, pLeft, pRight);

	lua_pop(pLua, 4);
	return 0;
}

void CButtonComponent::SetNextButtonComponent(CButtonComponent* pMe, CButtonComponent* pDown, 
											  CButtonComponent* pUp, CButtonComponent* pLeft, CButtonComponent* pRight)
{
	if(pMe)
	{
		pMe->m_pSelectDown = pDown;
		pMe->m_pSelectUp = pUp;
		pMe->m_pSelectLeft = pLeft;
		pMe->m_pSelectRight = pRight;
	}
}


//	//Lua Stuff




void CButtonComponent::Init(string textureName, int /*nTextureDepth*/)
{
	
	
	m_fTimeSinceSelectionEntry = 0;
	m_fInputCooldown = .2f;
	m_bSelected = false;
	m_bSelectedStartValue = false;
	
	m_pDisplayComponentButton = NULL;
	m_pDisplayComponentSelection = NULL;

	m_pDisplayObjectButton = NULL;
	m_pDisplayObjectSelection = NULL;

	
}

// setup stuff that wont change throughout the game
void CButtonComponent::Load(string textureName, int nTextureDepth, int nPosX, int nPosY)
{
	int nTexID0 = -1;
	int nTexID1 = -1;

	// Init texture stuff
	if(textureName.length() != 0) // if we have a filename
	{
		string szButtonTex = "Resource/HUD/";
		szButtonTex += textureName;
		nTexID0 = CTextureManager::GetInstance()->LoadTexture(szButtonTex.c_str(), RGB(220,240,120));
	}
	
	nTexID1 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\T_Pause_Icon_Glow_D.png", RGB(255,255,255));


	// Create the Object
	string szDispObjName = "ButtonDisplayObject";
	szDispObjName += (char)this;
	m_pDisplayObjectButton = CObjectManager::GetInstance()->CreateObject(szDispObjName);
	szDispObjName = "ButtonSelectionDisplayObject";
	szDispObjName += (char)this;


	m_pDisplayObjectButton = CObjectManager::GetInstance()->CreateObject(szDispObjName);
	m_pDisplayObjectSelection = CObjectManager::GetInstance()->CreateObject(szDispObjName);

	// Get Inital Sprite Data
	
	m_tSpriteDataButton.m_nTexture = nTexID0;
	m_tSpriteDataButton.m_nX = 150;
	m_tSpriteDataButton.m_nY = 200;
	m_tSpriteDataButton.m_nZ = nTextureDepth;
	m_tSpriteDataButton.m_fScaleX = 1.0f;
	m_tSpriteDataButton.m_fScaleY = 1.0f;
	m_tSpriteDataButton.m_fRotCenterX = 0.0f;
	m_tSpriteDataButton.m_fRotCenterY = 0.0f;
	m_tSpriteDataButton.m_fRot = 0.0f;
	m_tSpriteDataButton.m_dwColor = (DWORD)-1;//RGB(1.0f, 1.0f, 1.0f);
	m_tSpriteDataButton.m_tRect.top    = 0;
	m_tSpriteDataButton.m_tRect.right  = 0;
	m_tSpriteDataButton.m_tRect.left   = 0;
	m_tSpriteDataButton.m_tRect.bottom = 0;

	m_tSpriteDataSelection.m_nTexture = nTexID1;
	m_tSpriteDataSelection.m_nX = m_tSpriteDataButton.m_nX + BUTTON_LOCATION_OFFSET_X; // Offset from selection
	m_tSpriteDataSelection.m_nY = 200 + BUTTON_LOCATION_OFFSET_Y;
	m_tSpriteDataSelection.m_nZ = nTextureDepth;
	m_tSpriteDataSelection.m_fScaleX = 1.0f;
	m_tSpriteDataSelection.m_fScaleY = 1.0f;
	m_tSpriteDataSelection.m_fRotCenterX = 0.0f;
	m_tSpriteDataSelection.m_fRotCenterY = 0.0f;
	m_tSpriteDataSelection.m_fRot = 0.0f;
	m_tSpriteDataSelection.m_dwColor = (DWORD)-1;//RGB(1.0f, 1.0f, 1.0f);
	m_tSpriteDataSelection.m_tRect.top    = 0;
	m_tSpriteDataSelection.m_tRect.right  = 0;
	m_tSpriteDataSelection.m_tRect.left   = 0;
	m_tSpriteDataSelection.m_tRect.bottom = 0;



	// Add Sprite Comp
	if(m_tSpriteDataButton.m_nTexture != -1)
	{
		m_pDisplayComponentButton = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplayObjectButton, m_tSpriteDataButton, false); // dont show at start. wait for proper state enter
	}
	if(m_tSpriteDataSelection.m_nTexture != -1)
	{
		m_pDisplayComponentSelection = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplayObjectSelection, m_tSpriteDataSelection, false);
	}

	SetScreenPosition(nPosX, nPosY);

	///////////////////////////////////////////////////////////////////////////
	//// Register for Events
	///////////////////////////////////////////////////////////////////////////

	string szEvent = "Update";
	szEvent += (char)m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);
	CEventManager::GetInstance()->RegisterEvent("Up", this, UpPressed);
	CEventManager::GetInstance()->RegisterEvent("Down", this, DownPressed);
	CEventManager::GetInstance()->RegisterEvent("Right", this, RightPressed); // make right and left invalid since it confuses the EPs
	CEventManager::GetInstance()->RegisterEvent("Left", this, LeftPressed);
	CEventManager::GetInstance()->RegisterEvent("Back", this, InvalidSelection);
	CEventManager::GetInstance()->RegisterEvent("Accept", this, SelectPressed);



}

void CButtonComponent::ReInitValues()
{
	m_bSelected = m_bSelectedStartValue;

	if(m_pDisplayComponentButton != NULL)
	{
		m_pDisplayComponentButton->SetActive(true);
	}	
	
	if(m_bSelected && m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetActive(true);
	}
	
}

void CButtonComponent::Activate()
{
	m_bSelected = true;
	m_fTimeSinceSelectionEntry = 0;
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetActive(true);
	}
	// if we have a rollover event
	if(m_szOnScrolloverFunc.length() != 0)
	{
		Debug.CallLuaFunc(m_szOnScrolloverFunc);
	}
}

void CButtonComponent::Deactivate()
{
	m_bSelected = false;
	m_fTimeSinceSelectionEntry = 0;
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetActive(false);
	}
}

void CButtonComponent::Unshow()
{
	//m_bSelectedStartValue = m_bSelected; // so we save the last time we were in here's spot
	m_bSelected = false;
	m_fTimeSinceSelectionEntry = 0;
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetActive(false);
	}
	if(m_pDisplayComponentButton != NULL)
	{
		m_pDisplayComponentButton->SetActive(false);
	}
}

void CButtonComponent::SetScreenPosition(int nPosX, int nPosY)
{
	m_tSpriteDataButton.m_nX = nPosX;
	m_tSpriteDataButton.m_nY = nPosY;
	m_tSpriteDataButton.m_nZ = 1;
	m_tSpriteDataSelection.m_nX = nPosX + BUTTON_LOCATION_OFFSET_X;
	m_tSpriteDataSelection.m_nY = nPosY + BUTTON_LOCATION_OFFSET_Y;
	m_tSpriteDataSelection.m_nZ = 6;

	if(m_pDisplayComponentButton != NULL)
	{
		m_pDisplayComponentButton->SetSpriteData(m_tSpriteDataButton);
	}
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetSpriteData(m_tSpriteDataSelection);
	}
}



///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CButtonComponent::Update(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDt = eEvent->m_fDeltaTime;

	if(comp->m_bSelected)
	{
		comp->m_fTimeSinceSelectionEntry += fDt;
		
		// Button idle logic here
	}
}

void CButtonComponent::UpPressed(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		if(comp->m_pSelectUp)
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);
			comp->Deactivate();
			comp->m_pSelectUp->Activate();
		}
	}
	
}

void CButtonComponent::DownPressed(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		if(comp->m_pSelectDown)
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);
			comp->Deactivate();
			comp->m_pSelectDown->Activate();
		}
	}
}

void CButtonComponent::RightPressed(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		if(comp->m_pSelectRight)
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);
			comp->Deactivate();
			comp->m_pSelectRight->Activate();
		}
	}
}

void CButtonComponent::LeftPressed(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		if(comp->m_pSelectLeft)
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);
			comp->Deactivate();
			comp->m_pSelectLeft->Activate();
		}
	}
}

void CButtonComponent::SelectPressed(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && 
		comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		// if we don't have an event
		if(comp->m_szSelectionFunc.length() == 0)
		{
			//Sound Hack
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_INVALID_SELECTION, GLOBAL_ID);
		}
		else
		{
			//Sound Hack
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SELECT, GLOBAL_ID);
			
			Debug.CallLuaFunc(comp->m_szSelectionFunc);
			//SendStateEvent(comp->m_szSelectionEvent, comp, (EGameState)comp->m_nToState, PRIORITY_NORMAL);

			// Don't unshow selves. State changes handle that
			/*
			// Unshow all the elements
			CButtonComponent* pCur = comp->m_pSelectDown;
			// while it's not null and isn't us (aka full loop) 
			while(pCur && pCur != comp)
			{
				pCur->Unshow();
				pCur = pCur->m_pSelectDown;
			}
			// deactivate ourselves
			comp->Unshow();
			/**/
		}
		
		comp->m_fTimeSinceSelectionEntry = 0.0f;
	}
}

void CButtonComponent::InvalidSelection(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && 
		comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		//Sound Hack
		CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_INVALID_SELECTION, GLOBAL_ID);
		comp->m_fTimeSinceSelectionEntry = 0.0f;
	}
}

void CButtonComponent::ButtonStateEnable(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	comp->ReInitValues();

	comp->m_fTimeSinceSelectionEntry = -.1f; // extra time to disable input to prevent carryover keypresses
}

void CButtonComponent::ButtonStateDisable(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	comp->Unshow();
}

void CButtonComponent::ButtonStateLoad(IEvent* /*cEvent*/, IComponent* /*cCenter*/)
{
	//CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

}

void CButtonComponent::ButtonStateInit(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	comp->ReInitValues();

}


//HACK: All buttons disable in gameplay
void CButtonComponent::GameplayUpdate(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* pComp = (CButtonComponent*)cCenter;

	pComp->Unshow();
}
