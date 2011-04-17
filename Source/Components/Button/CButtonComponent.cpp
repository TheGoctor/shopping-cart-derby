#include "CButtonComponent.h"


#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CUpdateStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CIDGen.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"


// pixels for offset:
#define BUTTON_LOCATION_OFFSET 50

CObject*			CButtonComponent::m_pDisplayObjectBackground;
CSpriteComponent*	CButtonComponent::m_pDisplayComponentBackground;
TSpriteData			CButtonComponent::m_tSpriteDataBackground;

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
				printf(lua_toboolean(l, i)!=0 ? "true" : "false");
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

	CObject* pObj = (CObject*)lua_topointer(pLua, 1);
	string szEventName = (string)lua_tostring(pLua, 2);
	string szButtonTexName = (string)lua_tostring(pLua, 3);
	int nX = lua_tointeger(pLua, 4);
	int nY = lua_tointeger(pLua, 5);
	bool bStartSelected = (lua_toboolean(pLua, 6) != 0);
	CButtonComponent* pSOR = CreateButtonComponent(pObj, szEventName, szButtonTexName, nX, nY, bStartSelected);

	lua_pop(pLua, 6);

	lua_pushlightuserdata(pLua, pSOR);


	return 1;
}

CButtonComponent* CButtonComponent::CreateButtonComponent(CObject* pObj, string szEventName, string szSpriteTextureName, 
														  int nPosX, int nPosY, bool bStartSelected)
{
	CButtonComponent* comp = MMNEW(CButtonComponent) CButtonComponent(pObj);
	comp->m_szSelectionEvent = szEventName;

	comp->Init(szSpriteTextureName);
	comp->m_bSelected = bStartSelected;
	if(bStartSelected)
	{
		comp->Activate();
	}
	else
	{
		comp->Deactivate();
	}

	comp->SetScreenPosition(nPosX, nPosY);

	pObj->AddComponent(comp);

	return comp;
}


int CButtonComponent::SetNextButtonComponent(lua_State* pLua)
{
	
	CButtonComponent* pCur = (CButtonComponent*)lua_topointer(pLua, 1);
	CButtonComponent* pNext = (CButtonComponent*)lua_topointer(pLua, 2);
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




void CButtonComponent::Init(string textureName)
{
	// listens for these events
	string szEvent = "Update";
	szEvent += MAIN_MENU_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);
	CEventManager::GetInstance()->RegisterEvent("Up", this, PreviousPressed);
	CEventManager::GetInstance()->RegisterEvent("Down", this, NextPressed);
	CEventManager::GetInstance()->RegisterEvent("Accept", this, SelectPressed);
	
	m_fTimeSinceSelectionEntry = 0;
	m_fInputCooldown = .2f;
	m_bSelected = false;

	// Init texture stuff
	string szButtonTex = "Resource\\HUD\\";
	szButtonTex += textureName;
	int nTexID0 = CTextureManager::GetInstance()->LoadTexture(szButtonTex.c_str(), RGB(255,255,255));
	int nTexID1 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\MainMenuSelector.png", RGB(255,255,255));

	static bool hasBeenCreated = false;
	if(!hasBeenCreated)
	{
		int nTexIDMOO = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\MainMenuBackground.png", RGB(255,255,255));
		m_pDisplayObjectBackground = CObjectManager::GetInstance()->CreateObject("MainMenuBackground");


		m_tSpriteDataBackground.m_nTexture = nTexIDMOO;
		m_tSpriteDataBackground.m_nX = 0;
		m_tSpriteDataBackground.m_nY = 0;
		m_tSpriteDataBackground.m_nZ = 5;

		m_tSpriteDataBackground.m_fScaleX = 1.0f;
		m_tSpriteDataBackground.m_fScaleY = 1.0f;
		m_tSpriteDataBackground.m_fRotCenterX = 0.0f;
		m_tSpriteDataBackground.m_fRotCenterY = 0.0f;
		m_tSpriteDataBackground.m_fRot = 0.0f;
		m_tSpriteDataBackground.m_dwColor = -1;//RGB(1.0f, 1.0f, 1.0f);
		m_tSpriteDataBackground.m_tRect.top    = 0;
		m_tSpriteDataBackground.m_tRect.right  = 0;
		m_tSpriteDataBackground.m_tRect.left   = 0;
		m_tSpriteDataBackground.m_tRect.bottom = 0;

		m_pDisplayComponentBackground = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplayObjectBackground, m_tSpriteDataBackground, false);
		m_pDisplayComponentBackground->SetActive(true);
	
		hasBeenCreated = true;
	}

	// Create the Object
	string szDispObjName = "ButtonDisplayObject";
	szDispObjName += (unsigned int)this;
	m_pDisplayObjectButton = CObjectManager::GetInstance()->CreateObject(szDispObjName);
	szDispObjName = "ButtonSelectionDisplayObject";
	szDispObjName += (unsigned int)this;

	m_pDisplayObjectButton = CObjectManager::GetInstance()->CreateObject(szDispObjName);
	m_pDisplayObjectSelection = CObjectManager::GetInstance()->CreateObject(szDispObjName);

	// Get Inital Sprite Data
	
	m_tSpriteDataButton.m_nTexture = nTexID0;
	m_tSpriteDataButton.m_nX = 150;
	m_tSpriteDataButton.m_nY = 200;
	m_tSpriteDataButton.m_nZ = 10;
	m_tSpriteDataButton.m_fScaleX = 1.0f;
	m_tSpriteDataButton.m_fScaleY = 1.0f;
	m_tSpriteDataButton.m_fRotCenterX = 0.0f;
	m_tSpriteDataButton.m_fRotCenterY = 0.0f;
	m_tSpriteDataButton.m_fRot = 0.0f;
	m_tSpriteDataButton.m_dwColor = -1;//RGB(1.0f, 1.0f, 1.0f);
	m_tSpriteDataButton.m_tRect.top    = 0;
	m_tSpriteDataButton.m_tRect.right  = 0;
	m_tSpriteDataButton.m_tRect.left   = 0;
	m_tSpriteDataButton.m_tRect.bottom = 0;

	m_tSpriteDataSelection.m_nTexture = nTexID1;
	m_tSpriteDataSelection.m_nX = m_tSpriteDataButton.m_nX - BUTTON_LOCATION_OFFSET; // Offset from selection
	m_tSpriteDataSelection.m_nY = 200;
	m_tSpriteDataSelection.m_nZ = 11;
	m_tSpriteDataSelection.m_fScaleX = 1.0f;
	m_tSpriteDataSelection.m_fScaleY = 1.0f;
	m_tSpriteDataSelection.m_fRotCenterX = 0.0f;
	m_tSpriteDataSelection.m_fRotCenterY = 0.0f;
	m_tSpriteDataSelection.m_fRot = 0.0f;
	m_tSpriteDataSelection.m_dwColor = -1;//RGB(1.0f, 1.0f, 1.0f);
	m_tSpriteDataSelection.m_tRect.top    = 0;
	m_tSpriteDataSelection.m_tRect.right  = 0;
	m_tSpriteDataSelection.m_tRect.left   = 0;
	m_tSpriteDataSelection.m_tRect.bottom = 0;

	// Add Sprite Comp
	m_pDisplayComponentButton = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplayObjectButton, m_tSpriteDataButton, true); // kinda hacky: show from start of life of button
	m_pDisplayComponentSelection = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplayObjectSelection, m_tSpriteDataSelection, false);


}

void CButtonComponent::Activate()
{
	m_bSelected = true;
	m_fTimeSinceSelectionEntry = 0;
	m_pDisplayComponentSelection->SetActive(true);
}

void CButtonComponent::Deactivate()
{
	m_bSelected = false;
	m_pDisplayComponentSelection->SetActive(false);
}

void CButtonComponent::Unshow()
{
	m_bSelected = false;
	m_pDisplayComponentSelection->SetActive(false);
	m_pDisplayComponentButton->SetActive(false);
	m_pDisplayComponentBackground->SetActive(false);

}

void CButtonComponent::SetScreenPosition(int nPosX, int nPosY)
{
	m_tSpriteDataButton.m_nX = nPosX;
	m_tSpriteDataButton.m_nY = nPosY;
	m_tSpriteDataButton.m_nZ = 1;
	m_tSpriteDataSelection.m_nX = nPosX - BUTTON_LOCATION_OFFSET;
	m_tSpriteDataSelection.m_nY = nPosY;
	m_tSpriteDataSelection.m_nZ = 1;

	m_pDisplayComponentButton->SetSpriteData(m_tSpriteDataButton);
	m_pDisplayComponentSelection->SetSpriteData(m_tSpriteDataSelection);
	
}



///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CButtonComponent::Update(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	CUpdateStateEvent* eEvent = (CUpdateStateEvent*)cEvent;
	float fDt = eEvent->GetDeltaTime();


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
		
		if(comp->m_pPrevious)
		{
			comp->m_pPrevious->Activate();
			comp->Deactivate();
		}
	}
	
}

void CButtonComponent::NextPressed(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		if(comp->m_pNext)
		{
			comp->m_pNext->Activate();
			comp->Deactivate();
		}
	}
}

void CButtonComponent::SelectPressed(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	if(comp->m_bSelected && comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		IEvent* pEvent = MMNEWEVENT(IEvent) IEvent(CIDGen::GetInstance()->
				GetID(comp->m_szSelectionEvent), comp);

		CEventManager::GetInstance()->PostEvent(pEvent, PRIORITY_SHUTDOWN);

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
}