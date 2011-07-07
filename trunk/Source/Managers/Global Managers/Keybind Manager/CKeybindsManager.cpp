#include "CKeybindsManager.h"

#include "..\Memory Manager\CMemoryManager.h"

#include "..\Input Manager\CInputManager.h"
#include "..\Event Manager\EventStructs.h"
using namespace EventStructs;

#include "..\Rendering Managers\Texture Managers\CTextureManager.h"
#include "..\Object Manager\CObjectManager.h"

#include "..\..\..\CObject.h"

#include <iostream>
using namespace std;


#define KEYDOWN(name, key) (name[key] & 0x80  ? true : false)



CKeybindsManager::CKeybindsManager()
{
}

void CKeybindsManager::Init()
{
	m_fTimer = 0.0f;
	m_bCatchingInput = false;
	m_bReadyForControllerCheck = false;

	// set unbindable keys (by DIK_ value)
	m_lUnbindableKeys.push_back(DIK_RETURN);
	m_lUnbindableKeys.push_back(DIK_ESCAPE);
	m_lUnbindableKeys.push_back(DIK_GRAVE);

	
	string szEventName = "Update";
	szEventName += (char)KEYBIND_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleUpdate);	
	
	szEventName = "EnableObjects";
	szEventName += (char)KEYBIND_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateEnter);	

	szEventName = "DisableObjects";
	szEventName += (char)KEYBIND_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateExit);	

	szEventName = "InitObjects";
	szEventName += (char)KEYBIND_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateInit);	

// do the same for the pause version of this
	szEventName = "Update";
	szEventName += (char)PAUSE_KEYBINDS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleUpdate);	
	
	szEventName = "EnableObjects";
	szEventName += (char)PAUSE_KEYBINDS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateEnter);	

	szEventName = "DisableObjects";
	szEventName += (char)PAUSE_KEYBINDS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateExit);	

	szEventName = "InitObjects";
	szEventName += (char)PAUSE_KEYBINDS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, 
		(IComponent*)GetInstance(), HandleStateInitPause);	


	CEventManager::GetInstance()->RegisterEvent("SetKey", (IComponent*)GetInstance(), HandleStateInit);	


	// load bmap font stuff	
	CBitmapFont font(-1, 13, 15, 33);
	font.LoadFont("Resource\\BitmapFont.png", "Resource\\BitmapFont_Width.bin");

	float fScale = 2.0f;

	int x = 525;
	int y = 340;

	D3DXCOLOR textColor(1.0f, .6f, .2f, 255);

//
	m_tKeyInfo[0].nKey = CMD_ACCELERATE;
	string szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[0].nKey));

	CObject* pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText0");
	m_tKeyInfo[0].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[0].x = x;
	m_vMainMenuLocation[0].y = y;

//
	y = 400;
	x += 10;

	m_tKeyInfo[1].nKey = CMD_DECELERATE;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[1].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText1");
	m_tKeyInfo[1].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[1].x = x;
	m_vMainMenuLocation[1].y = y;

//
	y = 460;
	x -= 20;

	m_tKeyInfo[2].nKey = CMD_STEERRIGHT;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[2].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText2");
	m_tKeyInfo[2].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[2].x = x;
	m_vMainMenuLocation[2].y = y;



//
	y = 530;
	x -= 20;

	m_tKeyInfo[3].nKey = CMD_STEERLEFT;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[3].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText3");
	m_tKeyInfo[3].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[3].x = x;
	m_vMainMenuLocation[3].y = y;


//
	y = 600;
	x -= 20;

	m_tKeyInfo[4].nKey = CMD_DRIFT;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[4].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText4");
	m_tKeyInfo[4].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[4].x = x;
	m_vMainMenuLocation[4].y = y;

//
	x = 845;
	y = 315;

	m_tKeyInfo[5].nKey = CMD_USEITEM1;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[5].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText5");
	m_tKeyInfo[5].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[5].x = x;
	m_vMainMenuLocation[5].y = y;

//
	y = 360;

	m_tKeyInfo[6].nKey = CMD_USEITEM2;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[6].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText6");
	m_tKeyInfo[6].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[6].x = x;
	m_vMainMenuLocation[6].y = y;

//
	y = 400;

	m_tKeyInfo[7].nKey = CMD_SHOVELEFT;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[7].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText7");
	m_tKeyInfo[7].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[7].x = x;
	m_vMainMenuLocation[7].y = y;

//
	y = 450;

	m_tKeyInfo[8].nKey = CMD_SHOVERIGHT;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[8].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText8");
	m_tKeyInfo[8].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);

	m_vMainMenuLocation[8].x = x;
	m_vMainMenuLocation[8].y = y;


	x -= 15;
	y = 545;

	m_tKeyInfo[9].nKey = CMD_MENU;
	szName = CInputManager::GetInstance()->GetStringKeyboard(CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[9].nKey));

	pBitObj = CObjectManager::GetInstance()->CreateObject("KebindsText9");
	m_tKeyInfo[9].pFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, szName,
		font, x, y, fScale, textColor, false);
	
	m_vMainMenuLocation[9].x = x;
	m_vMainMenuLocation[9].y = y;


	m_vPauseMenuLocation[0].x = 410;
	m_vPauseMenuLocation[0].y = 290;

	m_vPauseMenuLocation[1].x = 440;
	m_vPauseMenuLocation[1].y = 340;

	m_vPauseMenuLocation[2].x = 410;
	m_vPauseMenuLocation[2].y = 390;

	m_vPauseMenuLocation[3].x = 410;
	m_vPauseMenuLocation[3].y = 450;

	m_vPauseMenuLocation[4].x = 404;
	m_vPauseMenuLocation[4].y = 510;


	m_vPauseMenuLocation[5].x = 800;
	m_vPauseMenuLocation[5].y = 291;

	m_vPauseMenuLocation[6].x = 800;
	m_vPauseMenuLocation[6].y = 340;

	m_vPauseMenuLocation[7].x = 800;
	m_vPauseMenuLocation[7].y = 390;

	m_vPauseMenuLocation[8].x = 800;
	m_vPauseMenuLocation[8].y = 450;

	m_vPauseMenuLocation[9].x = 800;
	m_vPauseMenuLocation[9].y = 510;



	// save defaults for set to default feature
	for(int i=0; i < NUM_REBINDABLE_KEYS; i++)
	{
		m_tDefaultKeyInfo[i] = CInputManager::GetInstance()->GetKeyboardKey(m_tKeyInfo[i].nKey);
		m_tDefaultControllerButtonInfo[i] = CInputManager::GetInstance()->GetControllerButton(m_tKeyInfo[i].nKey);
	}


	// Set up directions font
	
	pBitObj = CObjectManager::GetInstance()->CreateObject("Directionsf");
	m_pDirectionsFont = CTextureManager::GetInstance()->CreateBitmapFontComp(pBitObj, "Press a key to bind",
		font, 350, 700, 2.0f, D3DXCOLOR(1.0f, .4f, .4f, 1.0f), false);
}



void CKeybindsManager::HandleUpdate(IEvent* cEvent, IComponent*)
{
	TUpdateStateEvent* pEv = static_cast<TUpdateStateEvent*>(cEvent->GetData());
	// get dt here
	float fDt = pEv->m_fDeltaTime;

	CKeybindsManager* pMgr = GetInstance();
	pMgr->m_fTimer += fDt;

	if(pMgr->m_bCatchingInput)
	{
		// "Press a key to bind" text show because we're catching input
		pMgr->m_pDirectionsFont->SetActive(true);
		// make it flash by sin()ing the alpha channel
		pMgr->m_pDirectionsFont->SetColor(D3DXCOLOR(1.0f, .4f, .4f, 
			sin(pMgr->m_fTimer)* ( sin(pMgr->m_fTimer) > 0.0f ? 1.0f : -1.0f))); // ^^ oscilation not ^v (look at the shape of those letters)

		// if there's no controller connected (keybaord input)
		if(!CInputManager::GetInstance()->IsControllerConnected())
		{
			// catch input
			BYTE* pKeys = CInputManager::GetInstance()->GetKeys();
			int nKeyPressed = -1;

			// update the buffer
			CInputManager::GetInstance()->ReadDevices();

			// look for the first keydown that isn't enter (partly because we dont wanna overbind enter and partly so we don't catch enter as the input)
			for(int i=0; i<256; i++) // 256 is size of chkeys buffer
			{
				if(KEYDOWN(pKeys, i))
				{
					// if the button pressed was escape
					if(i == DIK_ESCAPE)
					{
						// stop taking input, reset to normal color and get out of this check
						// set its color back to normal
						pMgr->m_tKeyInfo[pMgr->m_nIndexToChange].pFont->SetColor(D3DXCOLOR(1.0f, .6f, .2f, 1.0f));

						// reset our variables. we're no longer catching input
						pMgr->m_bCatchingInput = false;
						pMgr->m_nIndexToChange = -1;
						pMgr->m_pDirectionsFont->SetActive(false);

						// set input state back to the menu state
						SendStateEvent("InputStateChange", (IComponent*)GetInstance(),
							KEYBIND_STATE, PRIORITY_IMMEDIATE);

						// stop the rest of this update
						return;
					}

					nKeyPressed = i;

					// iterate through the list of unbindable keys to see if the keydown is one we don't want to bind
					list<unsigned int, CEventAllocator<unsigned int>>::iterator iter;
					iter = pMgr->m_lUnbindableKeys.begin();
					while(iter != pMgr->m_lUnbindableKeys.end())
					{
						// if the key we stored as unbindable is the key that was pressed
						if((int)(*iter) == i)
						{
							// undo that set (since we can't break 'cause we're in a while already
							nKeyPressed = -1;
						}

						iter++;
					}

				}
			}

			// if we caught a key this frame
			if(nKeyPressed != -1)
			{
				// loop through our current keybinds
				for(int i=0; i<NUM_REBINDABLE_KEYS; i++)
				{
					// if the key chosen is currently being used in another slot
					if(nKeyPressed == CInputManager::GetInstance()->GetKeyboardKey(pMgr->m_tKeyInfo[i].nKey))
					{
						// get rid of that so we only have 1 key by that type
						CInputManager::GetInstance()->SetKeyboardCommand(pMgr->m_tKeyInfo[i].nKey, (BYTE)-1);
						pMgr->m_tKeyInfo[i].pFont->SetWord("<n/a>");
					}
				}

				// set the actual one to the key pressed
				CInputManager::GetInstance()->SetKeyboardCommand(pMgr->m_tKeyInfo[pMgr->m_nIndexToChange].nKey, (BYTE)nKeyPressed);
				// update the string so it shows
				pMgr->m_tKeyInfo[pMgr->m_nIndexToChange].pFont->SetWord(CInputManager::GetInstance()->GetStringKeyboard(
					CInputManager::GetInstance()->GetKeyboardKey(pMgr->m_tKeyInfo[pMgr->m_nIndexToChange].nKey)));


				// reset visible stuff
				// set its color back to normal
				pMgr->m_tKeyInfo[pMgr->m_nIndexToChange].pFont->SetColor(D3DXCOLOR(1.0f, .6f, .2f, 1.0f));

				// reset our variables. we're no longer catching input
				pMgr->m_bCatchingInput = false;
				pMgr->m_nIndexToChange = -1;
				pMgr->m_pDirectionsFont->SetActive(false);

				// set input state back to the menu state
						SendStateEvent("InputStateChange", (IComponent*)GetInstance(),
							KEYBIND_STATE, PRIORITY_IMMEDIATE);


			}
		}
		else // controller is connected
		{
			int nKeyPressed = CInputManager::GetInstance()->GetControllerButtonDown();

			// loop until they're not holding down a anymore so we can actually catch the button that isn't the button to tell it to bind
			while(pMgr->m_bReadyForControllerCheck == false)
			{
				nKeyPressed = CInputManager::GetInstance()->GetControllerButtonDown();

				if(nKeyPressed != XINPUT_GAMEPAD_A)
				{
					pMgr->m_bReadyForControllerCheck = true;
				}
			}


			// if we got valid input
			if(nKeyPressed != 0)
			{
				// get the string of the button pressed for new input
				string szNewName = CInputManager::GetInstance()->GetStringController(nKeyPressed);

				
					// if it returned a string (aka it fount input)
					if(szNewName.length() != 0)
					{
						// loop through our current keybinds
						for(int i=0; i<NUM_REBINDABLE_KEYS; i++)
						{
							// if the key chosen is currently being used in another slot
							if(szNewName == CInputManager::GetInstance()->GetStringController(
								CInputManager::GetInstance()->GetControllerButton(pMgr->m_tKeyInfo[i].nKey)))
							{
								// get rid of that so we only have 1 key by that type
								CInputManager::GetInstance()->SetControllerCommand(pMgr->m_tKeyInfo[i].nKey, -1);
								pMgr->m_tKeyInfo[i].pFont->SetWord("<n/a>");
							}
						}

						CInputManager::GetInstance()->SetControllerCommand(pMgr->m_tKeyInfo[pMgr->m_nIndexToChange].nKey, nKeyPressed);
						pMgr->m_tKeyInfo[pMgr->m_nIndexToChange].pFont->SetWord(szNewName);

						// set its color back to normal
						pMgr->m_tKeyInfo[pMgr->m_nIndexToChange].pFont->SetColor(D3DXCOLOR(1.0f, .6f, .2f, 1.0f));

						// reset our variables. we're no longer catching input
						pMgr->m_bCatchingInput = false;
						pMgr->m_nIndexToChange = -1;
						pMgr->m_pDirectionsFont->SetActive(false);
						pMgr->m_bReadyForControllerCheck = false;

						// set input state back to the menu state
						SendStateEvent("InputStateChange", (IComponent*)GetInstance(),
							KEYBIND_STATE, PRIORITY_IMMEDIATE);
					}

			}
		}
	}

}

void CKeybindsManager::HandleStateEnter(IEvent*, IComponent*)
{
	CKeybindsManager* pMgr = GetInstance();

	for(int i=0; i<NUM_REBINDABLE_KEYS; i++)
	{
		GetInstance()->m_tKeyInfo[i].pFont->SetActive(true);
	}

	pMgr->m_bCatchingInput = false;
	pMgr->m_nIndexToChange = -1;
	pMgr->m_pDirectionsFont->SetActive(false);

}

void CKeybindsManager::HandleStateExit(IEvent*, IComponent*)
{
	CKeybindsManager* pMgr = GetInstance();

	for(int i=0; i<NUM_REBINDABLE_KEYS; i++)
	{
		GetInstance()->m_tKeyInfo[i].pFont->SetActive(false);
	}

	pMgr->m_bCatchingInput = false;
	pMgr->m_nIndexToChange = -1;
	pMgr->m_pDirectionsFont->SetActive(false);
}


void CKeybindsManager::HandleStateInit(IEvent* pEvent, IComponent*)
{
	CKeybindsManager* pMgr = GetInstance();
	for(int i=0; i<NUM_REBINDABLE_KEYS; i++)
	{
		// set the position based on which menu we just entered. We're in main menu version because this is the callback for that
		GetInstance()->m_tKeyInfo[i].pFont->SetX(GetInstance()->m_vMainMenuLocation[i].x);
		GetInstance()->m_tKeyInfo[i].pFont->SetY(GetInstance()->m_vMainMenuLocation[i].y);

		GetInstance()->m_tKeyInfo[i].pFont->SetActive(true);
		// if we're controller input
		if(CInputManager::GetInstance()->IsControllerConnected())
		{
			// if the key returns "" stringm, set it to n/a
			GetInstance()->m_tKeyInfo[i].pFont->SetWord(("" == CInputManager::GetInstance()->GetStringController(
				CInputManager::GetInstance()->GetControllerButton(pMgr->m_tKeyInfo[i].nKey))) ? "<n/a>" :
					CInputManager::GetInstance()->GetStringController(
					CInputManager::GetInstance()->GetControllerButton(pMgr->m_tKeyInfo[i].nKey))
				);
		}
		else // else we're keyboard input
		{
			GetInstance()->m_tKeyInfo[i].pFont->SetWord(("" ==CInputManager::GetInstance()->GetStringKeyboard(
				CInputManager::GetInstance()->GetKeyboardKey(pMgr->m_tKeyInfo[i].nKey))) ? "<n/a>" : 
					CInputManager::GetInstance()->GetStringKeyboard(
					CInputManager::GetInstance()->GetKeyboardKey(pMgr->m_tKeyInfo[i].nKey))
				);
		}
	}

	pMgr->m_bCatchingInput = false;
	pMgr->m_nIndexToChange = -1;
	pMgr->m_pDirectionsFont->SetActive(false);

}


void CKeybindsManager::HandleStateInitPause(IEvent* pEvent, IComponent*)
{
	CKeybindsManager* pMgr = GetInstance();
	for(int i=0; i<NUM_REBINDABLE_KEYS; i++)
	{
		// set the position based on which menu we just entered. We're in pause version because this is the callback for that
		GetInstance()->m_tKeyInfo[i].pFont->SetX(GetInstance()->m_vPauseMenuLocation[i].x);
		GetInstance()->m_tKeyInfo[i].pFont->SetY(GetInstance()->m_vPauseMenuLocation[i].y);


		GetInstance()->m_tKeyInfo[i].pFont->SetActive(true);
		// if we're controller input
		if(CInputManager::GetInstance()->IsControllerConnected())
		{
			// if the key returns "" stringm, set it to n/a
			GetInstance()->m_tKeyInfo[i].pFont->SetWord(("" == CInputManager::GetInstance()->GetStringController(
				CInputManager::GetInstance()->GetControllerButton(pMgr->m_tKeyInfo[i].nKey))) ? "<n/a>" :
					CInputManager::GetInstance()->GetStringController(
					CInputManager::GetInstance()->GetControllerButton(pMgr->m_tKeyInfo[i].nKey))
				);
		}
		else // else we're keyboard input
		{
			GetInstance()->m_tKeyInfo[i].pFont->SetWord(("" ==CInputManager::GetInstance()->GetStringKeyboard(
				CInputManager::GetInstance()->GetKeyboardKey(pMgr->m_tKeyInfo[i].nKey))) ? "<n/a>" : 
					CInputManager::GetInstance()->GetStringKeyboard(
					CInputManager::GetInstance()->GetKeyboardKey(pMgr->m_tKeyInfo[i].nKey))
				);
		}
	}

	pMgr->m_bCatchingInput = false;
	pMgr->m_nIndexToChange = -1;
	pMgr->m_pDirectionsFont->SetActive(false);

}



int CKeybindsManager::SetBindDefaultCallback(lua_State* /*pLua*/)
{
	CKeybindsManager* pMgr = GetInstance();
	
	for(int i=0; i<NUM_REBINDABLE_KEYS; i++)
	{
		// set to default saved from init
		CInputManager::GetInstance()->SetKeyboardCommand(pMgr->m_tKeyInfo[i].nKey, pMgr->m_tDefaultKeyInfo[i]);

		// update the visible info
		GetInstance()->m_tKeyInfo[i].pFont->SetActive(true);
		// if we're controller input
		if(CInputManager::GetInstance()->IsControllerConnected())
		{
			GetInstance()->m_tKeyInfo[i].pFont->SetWord(CInputManager::GetInstance()->GetStringController(
				CInputManager::GetInstance()->GetControllerButton(pMgr->m_tKeyInfo[i].nKey)));
		}
		else // else we're keyboard input
		{
			GetInstance()->m_tKeyInfo[i].pFont->SetWord(CInputManager::GetInstance()->GetStringKeyboard(
				CInputManager::GetInstance()->GetKeyboardKey(pMgr->m_tKeyInfo[i].nKey)));
		}
	}


	
	return 0;
}

int CKeybindsManager::SetKeyCallback(lua_State* pLua)
{
	CKeybindsManager* pMgr = GetInstance();
	
	int nIndexToChange = lua_tointeger(pLua, -1);

	// if we're not currently catching input and if it's a valid index
	if(!pMgr->m_bCatchingInput && nIndexToChange >= 0 && nIndexToChange < NUM_REBINDABLE_KEYS)
	{
		// set this spot's color to a color showing we have it selected
		pMgr->m_tKeyInfo[nIndexToChange].pFont->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// show text sayinng to press key now

		// set the index to change and set the bool so update can handle
		pMgr->m_bCatchingInput = true;
		pMgr->m_nIndexToChange = nIndexToChange;


		SendStateEvent("InputStateChange", (IComponent*)GetInstance(),
			MIN_STATE, PRIORITY_IMMEDIATE);

		if(CInputManager::GetInstance()->IsControllerConnected())
		{
			pMgr->m_bReadyForControllerCheck = false;
		}
	}

	lua_pop(pLua, 1);
	return 0;
}




#undef KEYDOWN