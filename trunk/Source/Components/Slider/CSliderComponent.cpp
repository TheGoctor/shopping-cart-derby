#include "CSliderComponent.h"

#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CIDGen.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
using namespace EventStructs;

CSliderComponent::CSliderComponent(CObject* pObj) : CButtonComponent(pObj)
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

// Lua Prototype: CreateSliderComponent(Object, "EventName", "ButtonTexture.png", nBarStartX, nBarStartY, nBarEndX, nBarEndY, 
//										nTextureDepth, bStartSelected, nActiveState)
int CSliderComponent::CreateSliderComponent(lua_State* pLua)
{
	stackDump(pLua);

	// top down is in order in param list. Start at bottom with -1 and work your way to the more negative numbers
	CObject* pObj = (CObject*)lua_topointer(pLua, -10);
	string szEventName = (string)lua_tostring(pLua, -9);
	string szButtonTexName = (string)lua_tostring(pLua, -8);
	int nStartX = lua_tointeger(pLua, -7);
	int nStartY = lua_tointeger(pLua, -6);
	int nEndX = lua_tointeger(pLua, -5);
	int nEndY = lua_tointeger(pLua, -4);
	int nTextureDepth = lua_tointeger(pLua, -3);
	bool bStartSelected = (lua_toboolean(pLua, -2) != 0);
	int eState = lua_tointeger(pLua, -1);


	CSliderComponent* pSlider = CreateSliderComponent(pObj, szEventName, szButtonTexName, 
		nStartX, nStartY, nEndX, nEndY, nTextureDepth, bStartSelected, eState);

	lua_pop(pLua, 10);

	lua_pushlightuserdata(pLua, pSlider);

	return 1;
}

CSliderComponent* CSliderComponent::CreateSliderComponent(CObject* pObj, string szEventName, string szSpriteTextureName, 
														  int nStartX, int nStartY, int nEndX, int nEndY, int nTextureDepth,
														  bool bStartSelected, int eGameState)
{
	CSliderComponent* comp = MMNEW(CSliderComponent(pObj));

	comp->m_szSelectionEvent = szEventName;
	comp->m_eAssociatedState = (EGameState)eGameState;


	comp->Init(szSpriteTextureName, nTextureDepth);
	comp->m_bSelected = bStartSelected;
	comp->m_bSelectedStartValue = bStartSelected;

	comp->SetScreenPosition(nStartX, nStartY, nEndX, nEndY);

	pObj->AddComponent(comp);

	return comp;
}

// set slider to certain value
int CSliderComponent::SetSliderValue(lua_State* pLua)
{
	stackDump(pLua);

	// top down is in order in param list. Start at bottom with -1 and work your way to the more negative numbers
	
	CSliderComponent* pComp = (CSliderComponent*)lua_touserdata(pLua, -2);
	float fAmount = lua_tonumber(pLua, -1);

	if(pComp)
	{
		pComp->SetSliderToValue(fAmount);
	}

	lua_pop(pLua, 2);

	return 0;
}

//	//Lua Stuff
void CSliderComponent::Init(string textureName, int nTextureDepth)
{
	// listens for these events
	string szEvent = "EnableObjects";
	szEvent += (char)m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, ButtonStateEnable);

	szEvent = "DisableObjects";
	szEvent += (char)m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, ButtonStateDisable);
	
	szEvent = "InitObjects";
	szEvent += (char)m_eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, SliderStateInit);
	

	m_fTimeSinceSelectionEntry = 0;
	m_fInputCooldown = .2f;
	m_bSelected = false;
	m_bSelectedStartValue = false;

	// Init texture stuff
	string szButtonTex = "Resource\\HUD\\";
	szButtonTex += textureName;
	int nTexID0 =  CTextureManager::GetInstance()->LoadTexture(szButtonTex.c_str(), RGB(255,255,255));


	int nTexID1 = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\T_Pause_Icon_Glow_D.png", RGB(255,255,255));

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
	m_tSpriteDataSelection.m_nX = m_tSpriteDataButton.m_nX; // keep at same spot
	m_tSpriteDataSelection.m_nY = 200;
	m_tSpriteDataSelection.m_nZ = nTextureDepth+1;
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

	m_pDisplayComponentButton = NULL;
	m_pDisplayComponentSelection = NULL;

	// Add Sprite Comp
	if(m_tSpriteDataButton.m_nTexture != -1)
	{
		m_pDisplayComponentButton = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplayObjectButton, m_tSpriteDataButton, false); // dont show at start. wait for proper state enter
	}
	if(m_tSpriteDataSelection.m_nTexture != -1)
	{
		m_pDisplayComponentSelection = CTextureManager::GetInstance()->CreateSpriteComp(m_pDisplayObjectSelection, m_tSpriteDataSelection, false);
	}
}

void CSliderComponent::SetScreenPosition(int nPosX, int nPosY, int nEndX, int nEndY)
{
	m_tSpriteDataButton.m_nX = nPosX;
	m_tSpriteDataButton.m_nY = nPosY;
	m_tSpriteDataButton.m_nZ = 1;
	m_tSpriteDataSelection.m_nX = nPosX;
	m_tSpriteDataSelection.m_nY = nPosY;
	m_tSpriteDataSelection.m_nZ = 1;

	m_vEndPosition.x = (float)nEndX;
	m_vEndPosition.y = (float)nEndY;
	m_vEndPosition.z = 0.0f;
	m_vStartPosition.x = (float)nPosX;
	m_vStartPosition.y = (float)nPosY;
	m_vStartPosition.z = 0.0f;
	
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


void CSliderComponent::SelectPressed(IEvent* cEvent, IComponent* cCenter)
{
	CSliderComponent* comp = (CSliderComponent*)cCenter;
	if(comp->m_bSelected && 
		comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		// since there's no action on a select for these
		//Sound Hack
		CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_INVALID_SELECTION, GLOBAL_ID);
	}
}

void CSliderComponent::SlideRight(IEvent* cEvent, IComponent* cCenter)
{
	CSliderComponent* comp = (CSliderComponent*)cCenter;
	if(comp->m_bSelected && 
		comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		comp->MoveSliderOneTick(1); // positive for right

		comp->CalculateAndSendFloatEvent();
	}
}

void CSliderComponent::SlideLeft(IEvent* cEvent, IComponent* cCenter)
{
	CSliderComponent* comp = (CSliderComponent*)cCenter;
	if(comp->m_bSelected && 
		comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		comp->MoveSliderOneTick(-1); // negative for left

		comp->CalculateAndSendFloatEvent();
	}
}


void CSliderComponent::CalculateAndSendFloatEvent()
{
	// from start to current position on the bar
	D3DXVECTOR3 vVolumeVec = D3DXVECTOR3(m_tSpriteDataButton.m_nX, m_tSpriteDataButton.m_nY, 0.0f) - m_vStartPosition; 
	// from start to end (Entire bar)
	D3DXVECTOR3 vSliderBarVec = m_vEndPosition - m_vStartPosition;

	float fVolumeMag = D3DXVec3LengthSq(&vVolumeVec);
	float fSliderBarMag = D3DXVec3LengthSq(&vSliderBarVec);

	float fVal = max(min(fVolumeMag / fSliderBarMag, 1.0f), 0.0f); // current/total 0-1 clamped between 0 and 1 (min(vals, 1) puts us from -inf to 1
																	//			and max(newvals, 0) puts us 0 to inf, so the overlap is 0 to 1 
	fVal *= 100; // make it 0-100 instead of 0-1
	SendFloatEvent(m_szSelectionEvent, this, fVal, PRIORITY_NORMAL);
}

void CSliderComponent::MoveSliderOneTick(int nDirection)
{
		// Get comparison values
	D3DXVECTOR3 vSliderBarDirection = m_vEndPosition - m_vStartPosition; // from start to end
	float fSliderBarMag = D3DXVec3Length(&vSliderBarDirection); // length of whole slider bar
	D3DXVec3Normalize(&vSliderBarDirection, &vSliderBarDirection); // norm so we can do stuff with it with scaling

	// calculate the "tick" of the movement
	float fAmountToMoveBy = (fSliderBarMag/100.0f) * (float)nDirection; // 1/100 of the size of the bar in the direction specified
	D3DXVec3Scale(&vSliderBarDirection, &vSliderBarDirection, fAmountToMoveBy);

	// if we are outside the bounds
	D3DXVECTOR3 vSliderToStart = m_vStartPosition - D3DXVECTOR3(m_tSpriteDataButton.m_nX+vSliderBarDirection.x, m_tSpriteDataButton.m_nY+vSliderBarDirection.y, 0.0f);
	D3DXVECTOR3 vSliderToEnd = m_vEndPosition - D3DXVECTOR3(m_tSpriteDataButton.m_nX+vSliderBarDirection.x, m_tSpriteDataButton.m_nY+vSliderBarDirection.y, 0.0f);

	// if the angle between them is less than 90 (dot is positive), we know the slider is no longer between them
	if(D3DXVec3Dot(&vSliderToStart, &vSliderToEnd) > 0.0f)
	{
		// and thus we gtfo
		return;
	}

	// else the new values are in bounds

	// apply that movement to the slider
	m_tSpriteDataButton.m_nX += vSliderBarDirection.x;
	m_tSpriteDataButton.m_nY += vSliderBarDirection.y;

	// apply that movement to the selected picture slider (so they match up on location)
	m_tSpriteDataSelection.m_nX += vSliderBarDirection.x;
	m_tSpriteDataSelection.m_nY += vSliderBarDirection.y;

	// update with new sprite data locations
	if(m_pDisplayComponentButton != NULL)
	{
		m_pDisplayComponentButton->SetSpriteData(m_tSpriteDataButton);
	}
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetSpriteData(m_tSpriteDataSelection);
	}

	// sound cooldown
	static float fAmountToWait = 0.0f;
	fAmountToWait+= .1f;

	if(fAmountToWait > 1.0f)
	{
		CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);
		fAmountToWait = 0.0f;
	}
}


void CSliderComponent::SetSliderToValue(float fAmount)
{
		// Get comparison values
	D3DXVECTOR3 vSliderBarDirection = m_vEndPosition - m_vStartPosition; // from start to end
	float fSliderBarMag = D3DXVec3Length(&vSliderBarDirection); // length of whole slider bar
	D3DXVec3Normalize(&vSliderBarDirection, &vSliderBarDirection); // norm so we can do stuff with it with scaling

	float fVal = (fSliderBarMag*fAmount) / 100.0f; // convert from 0-100 space to start-finish space

	D3DXVec3Scale(&vSliderBarDirection, &vSliderBarDirection, fVal); // offset from start the amount we want is
	D3DXVECTOR3 vNewPos = vSliderBarDirection + m_vStartPosition;

	// if we are outside the bounds
	D3DXVECTOR3 vSliderToStart = m_vStartPosition - vNewPos;
	D3DXVECTOR3 vSliderToEnd = m_vEndPosition - vNewPos;

	// if the angle between them is less than 90 (dot is positive), we know the slider is no longer between them
	if(D3DXVec3Dot(&vSliderToStart, &vSliderToEnd) > 0.0f)
	{
		// and thus we gtfo
		return;
	}

	// else the new values are in bounds

	// apply that movement to the slider
	m_tSpriteDataButton.m_nX = vNewPos.x;
	m_tSpriteDataButton.m_nY = vNewPos.y;

	// apply that movement to the selected picture slider (so they match up on location)
	m_tSpriteDataSelection.m_nX = vNewPos.x;
	m_tSpriteDataSelection.m_nY = vNewPos.y;

	// update with new sprite data locations
	if(m_pDisplayComponentButton != NULL)
	{
		m_pDisplayComponentButton->SetSpriteData(m_tSpriteDataButton);
	}
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetSpriteData(m_tSpriteDataSelection);
	}
}


void CSliderComponent::SliderStateInit(IEvent* cEvent, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	string szEvent = "Update";
	szEvent += (char)comp->GetAssociatedState();
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, Update);
	CEventManager::GetInstance()->RegisterEvent("Up", comp, PreviousPressed);
	CEventManager::GetInstance()->RegisterEvent("Down", comp, NextPressed);
	CEventManager::GetInstance()->RegisterEvent("Right", comp, SlideRight);
	CEventManager::GetInstance()->RegisterEvent("Left", comp, SlideLeft);
	CEventManager::GetInstance()->RegisterEvent("Back", comp, InvalidSelection);
	CEventManager::GetInstance()->RegisterEvent("Accept", comp, SelectPressed);


	comp->ReInitValues();
}