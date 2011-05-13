#include "CButtonComponent.h"


#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CIDGen.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
using namespace EventStructs;


// pixels for offset:
#define BUTTON_LOCATION_OFFSET_X -75
#define BUTTON_LOCATION_OFFSET_Y -30


CButtonComponent::CButtonComponent(CObject* pObj) : m_pObject(pObj), m_pNext(NULL), m_pPrevious(NULL)
{

}

static void stackDump(lua_State* l)
{
	int i;
	int top = lua_gettop(l);
	for(i=1; i<=top; i++)
	{
		int t = lua_type(l, i);
		switch(t)
		{
		case LUA_TSTRING:
			{
				printf("%s", lua_tostring(l, i));
				break;
			}
		case LUA_TBOOLEAN:
			{
				printf(lua_toboolean(l, i) ? "true" : "false");
				break;
			}
		case LUA_TNUMBER:
			{
				printf("%g", lua_tonumber(l, i));
				break;
			}
		default:
			{
				printf("%s", lua_typename(l, t));
				break;
			}
		};
		printf(" ");
	}
	printf("\n");
}

// Lua stuff
int CButtonComponent::CreateButtonComponent(lua_State* pLua)
{
	stackDump(pLua);

	// top down is in order in param list. Start at bottom with -1 and work your way to the more negative numbers
	CObject* pObj = (CObject*)lua_topointer(pLua, -9);
	string szEventName = (string)lua_tostring(pLua, -8);
	string szButtonTexName = (string)lua_tostring(pLua, -7);
	int nX = lua_tointeger(pLua, -6);
	int nY = lua_tointeger(pLua, -5);
	bool bStartSelected = (lua_toboolean(pLua, -4) != 0);
	int eState = lua_tointeger(pLua, -3);
	void* pData = (void*)lua_touserdata(pLua, -2);
	int nTextureDepth = lua_tointeger(pLua, -1);


	TStateEvent* stateevent = (TStateEvent*)pData;


	CButtonComponent* pButton = CreateButtonComponent(pObj, szEventName, szButtonTexName, nX, nY, bStartSelected, eState, pData, nTextureDepth);

	lua_pop(pLua, 9);

	lua_pushlightuserdata(pLua, pButton);


	return 1;
}

CButtonComponent* CButtonComponent::CreateButtonComponent(CObject* pObj, string szEventName, string szSpriteTextureName, 
														  int nPosX, int nPosY, bool bStartSelected, int eGameState, void* pEventData, int nTextureDepth)
{
	CButtonComponent* comp = MMNEW(CButtonComponent(pObj));

	

	comp->m_szSelectionEvent = szEventName;
	comp->m_pEventData = pEventData;
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
	
	szEvent = "LoadObjects";
	szEvent += (char)comp->m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, ButtonStateLoad);

	comp->Load(szSpriteTextureName, nTextureDepth, nPosX, nPosY);

	pObj->AddComponent(comp);



	return comp;
}


int CButtonComponent::SetNextButtonComponent(lua_State* pLua)
{
	CButtonComponent* pCur = (CButtonComponent*)lua_topointer(pLua, -2);
	CButtonComponent* pNext = (CButtonComponent*)lua_topointer(pLua, -1);
	SetNextButtonComponent(pCur, pNext);

	lua_pop(pLua, 2);
	return 0;
}

void CButtonComponent::SetNextButtonComponent(CButtonComponent* pCurrent, CButtonComponent* pNext)
{
	pCurrent->m_pNext = pNext;
	pNext->m_pPrevious = pCurrent;
}


//	//Lua Stuff




void CButtonComponent::Init(string textureName, int nTextureDepth)
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
		string szButtonTex = "Resource\\HUD\\";
		szButtonTex += textureName;
		nTexID0 = CTextureManager::GetInstance()->LoadTexture(szButtonTex.c_str(), RGB(255,255,255));
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
}

void CButtonComponent::Deactivate()
{
	m_bSelected = false;
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetActive(false);
	}
}

void CButtonComponent::Unshow()
{
	//m_bSelectedStartValue = m_bSelected; // so we save the last time we were in here's spot
	m_bSelected = false;
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
	m_tSpriteDataSelection.m_nZ = 1;

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

void CButtonComponent::PreviousPressed(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);
		
		if(comp->m_pPrevious)
		{
			comp->Deactivate();
			comp->m_pPrevious->Activate();
		}
	}
	
}

void CButtonComponent::NextPressed(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);

		if(comp->m_pNext)
		{
			comp->Deactivate();
			comp->m_pNext->Activate();
		}
	}
}

void CButtonComponent::SelectPressed(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && 
		comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		// if we don't have an event
		if(comp->m_szSelectionEvent.length() == 0)
		{
			//Sound Hack
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_INVALID_SELECTION, GLOBAL_ID);
		}
		else
		{
			//Sound Hack
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SELECT, GLOBAL_ID);

			SendIEvent(comp->m_szSelectionEvent, comp, comp->m_pEventData, PRIORITY_NORMAL);

			// Unshow all the elements
			CButtonComponent* pCur = comp->m_pNext;
			// while it's not null and isn't us (aka full loop)
			while(pCur && pCur != comp)
			{
				pCur->Unshow();
				pCur = pCur->m_pNext;
			}
			// deactivate ourselves
			comp->Unshow();
		}
		
		comp->m_fTimeSinceSelectionEntry = 0.0f;
	}
}

void CButtonComponent::InvalidSelection(IEvent* cEvent, IComponent* cCenter)
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

void CButtonComponent::ButtonStateEnable(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	comp->ReInitValues();

	comp->m_fTimeSinceSelectionEntry = -.1f; // extra time to disable input to prevent carryover keypresses
}

void CButtonComponent::ButtonStateDisable(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	comp->Unshow();
}

void CButtonComponent::ButtonStateLoad(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

}

void CButtonComponent::ButtonStateInit(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();


	string szEvent = "Update";
	szEvent += (char)comp->m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, Update);
	CEventManager::GetInstance()->RegisterEvent("Up", comp, PreviousPressed);
	CEventManager::GetInstance()->RegisterEvent("Down", comp, NextPressed);
	CEventManager::GetInstance()->RegisterEvent("Right", comp, InvalidSelection);
	CEventManager::GetInstance()->RegisterEvent("Left", comp, InvalidSelection);
	CEventManager::GetInstance()->RegisterEvent("Back", comp, InvalidSelection);
	CEventManager::GetInstance()->RegisterEvent("Accept", comp, SelectPressed);


	comp->ReInitValues();

}
