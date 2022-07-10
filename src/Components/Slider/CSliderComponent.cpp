#include "CSliderComponent.h"

#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CIDGen.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\CObject.h"
#include "..\..\Enums.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\Managers\Global Managers\Rendering Managers\Direct3DManager.h"
using namespace EventStructs;

#include <iostream>
using namespace std;

CSliderComponent::CSliderComponent(CObject* pObj) : CButtonComponent(pObj), m_fSliderValue(0.0f), m_fStateEnteredSliderValue(69.0f)
{

}

// Lua stuff

// Lua Prototype: CreateSliderComponent(Object, "EventName", "ButtonTexture.png", nBarStartX, nBarStartY, nBarEndX, nBarEndY, 
//										nTextureDepth, bStartSelected, nActiveState)
int CSliderComponent::CreateSliderComponent(lua_State* pLua)
{
	// top down is in order in param list. Start at bottom with -1 and work your way to the more negative numbers
	CObject* pObj = (CObject*)lua_topointer(pLua, -11);
	string szEventName = (string)lua_tostring(pLua, -10);
	string szButtonTexName = (string)lua_tostring(pLua, -9);
	int nStartX = lua_tointeger(pLua, -8);
	int nStartY = lua_tointeger(pLua, -7);
	int nEndX = lua_tointeger(pLua, -6);
	int nEndY = lua_tointeger(pLua, -5);
	int nTextureDepth = lua_tointeger(pLua, -4);
	bool bStartSelected = (lua_toboolean(pLua, -3) != 0);
	int	nSliderFor = lua_tointeger(pLua, -2); 
	int eState = lua_tointeger(pLua, -1);


	CSliderComponent* pSlider = CreateSliderComponent(pObj, szEventName, szButtonTexName, 
		nStartX, nStartY, nEndX, nEndY, nTextureDepth, bStartSelected, nSliderFor, eState);

	lua_pop(pLua, 10);

	lua_pushlightuserdata(pLua, pSlider);

	return 1;
}

CSliderComponent* CSliderComponent::CreateSliderComponent(CObject* pObj, string szEventName, string szSpriteTextureName, 
														  int nStartX, int nStartY, int nEndX, int nEndY, int nTextureDepth,
														  bool bStartSelected, int nSliderFor, int eGameState)
{
	CSliderComponent* comp = MMNEW(CSliderComponent(pObj));

	comp->m_szSelectionFunc = szEventName;
	comp->m_eAssociatedState = (EGameState)eGameState;

	comp->m_nSliderFor = nSliderFor;

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
	// top down is in order in param list. Start at bottom with -1 and work your way to the more negative numbers
	
	CSliderComponent* pComp = (CSliderComponent*)lua_touserdata(pLua, -2);
	float fAmount = (float)lua_tonumber(pLua, -1);

	if(pComp)
	{
		pComp->SetSliderToValue(fAmount, false);
		SendFloatEvent(pComp->m_szSelectionFunc, pComp, fAmount, PRIORITY_NORMAL);
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

	szEvent = "Update";
	szEvent += (char)GetAssociatedState();
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);
	CEventManager::GetInstance()->RegisterEvent("Up", this, UpPressed);
	CEventManager::GetInstance()->RegisterEvent("Down", this, DownPressed);
	CEventManager::GetInstance()->RegisterEvent("Right", this, SlideRight);
	CEventManager::GetInstance()->RegisterEvent("Left", this, SlideLeft);
	CEventManager::GetInstance()->RegisterEvent("Back", this, InvalidSelection);
	CEventManager::GetInstance()->RegisterEvent("Accept", this, SelectPressed);
	
	CEventManager::GetInstance()->RegisterEvent("CancelOptionsMain", this, SliderCancelMain);
	CEventManager::GetInstance()->RegisterEvent("CancelOptionsPause", this, SliderCancelPause);

	

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


void CSliderComponent::SelectPressed(IEvent* /*cEvent*/, IComponent* cCenter)
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

void CSliderComponent::SlideRight(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CSliderComponent* comp = (CSliderComponent*)cCenter;
	if(comp->m_bSelected && 
		comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		comp->MoveSliderOneTick(1); // positive for right

		comp->CalculateAndSendFloatEvent();

		// sound for it
		// sound cooldown
		static float fAmountToWait = 0.0f;
		fAmountToWait+= .1f;

		// if the sound cd is up and we wanna play a ticking sound on this slider (aka slider = 2, which is sfx slider)
		if(fAmountToWait > 1.0f && comp->m_nSliderFor == 2)
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);
			fAmountToWait = 0.0f;
		}
	}
}

void CSliderComponent::SlideLeft(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CSliderComponent* comp = (CSliderComponent*)cCenter;
	if(comp->m_bSelected && 
		comp->m_fTimeSinceSelectionEntry > comp->m_fInputCooldown)
	{
		comp->MoveSliderOneTick(-1); // negative for left

		comp->CalculateAndSendFloatEvent();

		// sound for it
		// sound cooldown
		static float fAmountToWait = 0.0f;
		fAmountToWait+= .1f;

		// if the sound cd is up and we wanna play a ticking sound on this slider (aka slider = 2, which is sfx slider)
		if(fAmountToWait > 1.0f && comp->m_nSliderFor == 2)
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(MENU_SCROLL, GLOBAL_ID);
			fAmountToWait = 0.0f;
		}
	}
}


void CSliderComponent::CalculateAndSendFloatEvent()
{
	// from start to current position on the bar
	D3DXVECTOR3 vVolumeVec = D3DXVECTOR3((float)m_tSpriteDataButton.m_nX, (float)m_tSpriteDataButton.m_nY, 0.0f) - m_vStartPosition; 
	// from start to end (Entire bar)
	D3DXVECTOR3 vSliderBarVec = m_vEndPosition - m_vStartPosition;

	float fVolumeMag = D3DXVec3LengthSq(&vVolumeVec);
	float fSliderBarMag = D3DXVec3LengthSq(&vSliderBarVec);

	float fVal = max(min(fVolumeMag / fSliderBarMag, 1.0f), 0.0f); // current/total 0-1 clamped between 0 and 1 (min(vals, 1) puts us from -inf to 1
																	//			and max(newvals, 0) puts us 0 to inf, so the overlap is 0 to 1 
	fVal *= 100; // make it 0-100 instead of 0-1
	SendFloatEvent(m_szSelectionFunc, this, fVal, PRIORITY_NORMAL);
	//cout << "fVal:\t" << fVal << endl;
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
	m_tSpriteDataButton.m_nX += (int)vSliderBarDirection.x;
	m_tSpriteDataButton.m_nY += (int)vSliderBarDirection.y;

	// apply that movement to the selected picture slider (so they match up on location)
	m_tSpriteDataSelection.m_nX += (int)vSliderBarDirection.x;
	m_tSpriteDataSelection.m_nY += (int)vSliderBarDirection.y;

	// update with new sprite data locations
	if(m_pDisplayComponentButton != NULL)
	{
		m_pDisplayComponentButton->SetSpriteData(m_tSpriteDataButton);
	}
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetSpriteData(m_tSpriteDataSelection);
	}

	// from start to current position on the bar
	D3DXVECTOR3 vVolumeVec = D3DXVECTOR3((float)m_tSpriteDataButton.m_nX, (float)m_tSpriteDataButton.m_nY, 0.0f) - m_vStartPosition; 
	// from start to end (Entire bar)
	D3DXVECTOR3 vSliderBarVec = m_vEndPosition - m_vStartPosition;

	float fVolumeMag = D3DXVec3LengthSq(&vVolumeVec);
	fSliderBarMag = D3DXVec3LengthSq(&vSliderBarVec);

	float fVal = max(min(fVolumeMag / fSliderBarMag, 1.0f), 0.0f); // current/total 0-1 clamped between 0 and 1 (min(vals, 1) puts us from -inf to 1
																	//			and max(newvals, 0) puts us 0 to inf, so the overlap is 0 to 1 
	fVal *= 100; // make it 0-100 instead of 0-1
	
	m_fSliderValue = fVal;
}


void CSliderComponent::SetSliderToValue(float fAmount, bool fSendValue)
{
	/* // This isn't necessary anymroe but keeping just in case
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
	m_tSpriteDataButton.m_nX = (int)vNewPos.x;
	m_tSpriteDataButton.m_nY = (int)vNewPos.y;

	// apply that movement to the selected picture slider (so they match up on location)
	m_tSpriteDataSelection.m_nX = (int)vNewPos.x;
	m_tSpriteDataSelection.m_nY = (int)vNewPos.y;

	// update with new sprite data locations
	if(m_pDisplayComponentButton != NULL)
	{
		m_pDisplayComponentButton->SetSpriteData(m_tSpriteDataButton);
	}
	if(m_pDisplayComponentSelection != NULL)
	{
		m_pDisplayComponentSelection->SetSpriteData(m_tSpriteDataSelection);
	}

	/**/

	
	if(m_fSliderValue > fAmount)
	{
		int nNumHit = 0;
		while(m_fSliderValue >= fAmount)
		{
			MoveSliderOneTick(-1); 

			nNumHit++;

			if(nNumHit > 300)
			{
				//CWwiseSoundManager::GetInstance()->PlayTheSound(SASHA_ALLGOALITEMS, GLOBAL_ID);
				break;
			}
		}
	}
	else if(m_fSliderValue < fAmount)
	{
		int nNumHit = 0;
		while(m_fSliderValue <= fAmount)
		{
			MoveSliderOneTick(1);	
			
			nNumHit++;

			if(nNumHit > 300)
			{
				//CWwiseSoundManager::GetInstance()->PlayTheSound(SASHA_ALLGOALITEMS, GLOBAL_ID);
				break;
			}
		}
	}


	// let the slider stuff know that it changed.
	if(fSendValue)
	{
		CalculateAndSendFloatEvent();
	}
}


void CSliderComponent::SliderStateInit(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CButtonComponent* comp = (CButtonComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	CSliderComponent* pSliderComp = (CSliderComponent*)cCenter;

	switch(pSliderComp->m_nSliderFor)
	{
	case 1:
		pSliderComp->SetSliderToValue(CWwiseSoundManager::GetInstance()->GetMusicVolume(), false);
		//cout << CWwiseSoundManager::GetInstance()->GetMusicVolume() << "\t1" << endl;
		break;
	case 2:
		pSliderComp->SetSliderToValue(CWwiseSoundManager::GetInstance()->GetSoundVolume(), false);
		//cout << CWwiseSoundManager::GetInstance()->GetSoundVolume() << "\t2" << endl;
		break;
	case 3:
		pSliderComp->SetSliderToValue(Direct3DManager::GetInstance()->GetGamma(), false);
		//cout << Direct3DManager::GetInstance()->GetGamma() << "\t3" << endl;
		break;
	};

	cout << "From " << pSliderComp->m_fStateEnteredSliderValue << " to " << pSliderComp->m_fSliderValue << endl;
	pSliderComp->m_fStateEnteredSliderValue = pSliderComp->m_fSliderValue;

	comp->ReInitValues();
}


void CSliderComponent::SliderCancelMain(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CSliderComponent* pSliderComp = (CSliderComponent*)cCenter;

	pSliderComp->SliderCancelNonStatic(OPTIONS_STATE);
}

void CSliderComponent::SliderCancelPause(IEvent* /*cEvent*/, IComponent* cCenter)
{
	CSliderComponent* pSliderComp = (CSliderComponent*)cCenter;

	pSliderComp->SliderCancelNonStatic(PAUSE_OPTIONS_STATE);
}

void CSliderComponent::SliderCancelNonStatic(int eState)
{
	if(m_eAssociatedState == eState)
	{
		cout << "Canceled to " << m_fStateEnteredSliderValue << endl;
		SendFloatEvent(m_szSelectionFunc, this, m_fStateEnteredSliderValue, PRIORITY_NORMAL);
	}
	
}