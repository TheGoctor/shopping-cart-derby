////////////////////////////////////////////////////////////////////////////////
// File:	“CInputManager.cpp”
//
// Author: Mac Reichelt (MR)
// 
// Description: This class is used to manage all input from both the keyboard 
//		and a gamepad, It sends enum values based upon the input it receives 
//		from either device that correspond to the apporpriate action bound to
//		those keys/buttons.
//
// Stipulations: There should only be one instance of the Input Manager at any
//		given time.
//
// Interface: The CGame object is responsible for handling the creation and 
//		destruction of the Input Manager.
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ctype.h>
using namespace std;

#include "CInputManager.h"
#include "../Event Manager/CEventManager.h"
#include "../Event Manager/CIDGen.h"
#include "../Event Manager/EventStructs.h"
#include "../../../CGame.h"
#include "../Rendering Managers/Direct3DManager.h"
#include "../Console Manager/CConsoleManager.h"
#include "../Unlockable Manager/CUnlockableManager.h"
using namespace EventStructs;

#define KEYDOWN(name, key) (name[key] & 0x80  ? true : false)
#define BUFFERED_KEYDOWN(key) ((KEYDOWN(m_chKeys, (key)) && !KEYDOWN(m_chPrevKeys, (key))) ? true : false)

//	Constructor
CInputManager::CInputManager()
{
	m_pController = new XINPUT_STATE();

	// Menu Commands
	m_tAccept.m_nID					= CMD_ACCEPT;
	m_tAccept.m_nControllerButton	= XINPUT_GAMEPAD_A;
	m_tAccept.m_chKeyboardKey		= DIK_RETURN;

	m_tBack.m_nID					= CMD_BACK;
	m_tBack.m_nControllerButton		= XINPUT_GAMEPAD_B;
	m_tBack.m_chKeyboardKey			= DIK_ESCAPE;

	m_tUp.m_nID						= CMD_UP;
	m_tUp.m_nControllerButton		= XINPUT_GAMEPAD_DPAD_UP;
	m_tUp.m_chKeyboardKey			= DIK_UP;

	m_tDown.m_nID					= CMD_DOWN;
	m_tDown.m_nControllerButton		= XINPUT_GAMEPAD_DPAD_DOWN;
	m_tDown.m_chKeyboardKey			= DIK_DOWN;

	m_tLeft.m_nID					= CMD_LEFT;
	m_tLeft.m_nControllerButton		= XINPUT_GAMEPAD_DPAD_LEFT;
	m_tLeft.m_chKeyboardKey			= DIK_LEFT;

	m_tRight.m_nID					= CMD_RIGHT;
	m_tRight.m_nControllerButton	= XINPUT_GAMEPAD_DPAD_RIGHT;
	m_tRight.m_chKeyboardKey		= DIK_RIGHT;

	// Gameplay Commands
	m_tMenu.m_nID					= CMD_MENU;
	m_tMenu.m_nControllerButton		= XINPUT_GAMEPAD_START;
	m_tMenu.m_chKeyboardKey			= DIK_ESCAPE;

	m_tAccelerate.m_nID				= CMD_ACCELERATE;
	m_tAccelerate.m_nControllerButton = 0; //XINPUT_GAMEPAD.bRightTrigger;
	m_tAccelerate.m_chKeyboardKey	= DIK_UP;

	m_tDecelerate.m_nID				= CMD_DECELERATE;
	m_tDecelerate.m_nControllerButton = 0; //XINPUT_GAMEPAD.bLeftTrigger;
	m_tDecelerate.m_chKeyboardKey	= DIK_DOWN;

	m_tSteerLeft.m_nID				= CMD_STEERLEFT;
	m_tSteerLeft.m_nControllerButton = 0; //LeftStick
	m_tSteerLeft.m_chKeyboardKey	= DIK_LEFT;
	
	m_tSteerRight.m_nID				= CMD_STEERRIGHT;
	m_tSteerRight.m_nControllerButton = 0; //LeftStick
	m_tSteerRight.m_chKeyboardKey	= DIK_RIGHT;
	
	m_tShoveLeft.m_nID				= CMD_SHOVELEFT;
	m_tShoveLeft.m_nControllerButton = XINPUT_GAMEPAD_LEFT_SHOULDER;
	m_tShoveLeft.m_chKeyboardKey	= DIK_A;
	
	m_tShoveRight.m_nID				= CMD_SHOVERIGHT;
	m_tShoveRight.m_nControllerButton = XINPUT_GAMEPAD_RIGHT_SHOULDER;
	m_tShoveRight.m_chKeyboardKey	= DIK_S;

	m_tDrift.m_nID					= CMD_DRIFT;
	m_tDrift.m_nControllerButton	= XINPUT_GAMEPAD_A; // A?
	m_tDrift.m_chKeyboardKey		= DIK_LSHIFT;
	
	m_tUseItem1.m_nID				= CMD_USEITEM1;
	m_tUseItem1.m_nControllerButton	= XINPUT_GAMEPAD_X; // A?
	m_tUseItem1.m_chKeyboardKey		= DIK_Z;
	
	m_tUseItem2.m_nID				= CMD_USEITEM2;
	m_tUseItem2.m_nControllerButton	= XINPUT_GAMEPAD_B; // A?
	m_tUseItem2.m_chKeyboardKey		= DIK_X;
	
}

//	Destructor
CInputManager::~CInputManager()
{
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“GetInstance”
//
// Return:		CInputManager*	-	The instance of the class
//
// Parameters:	void
//
// Purpose:		This function is used to get an instance of a singleton class
////////////////////////////////////////////////////////////////////////////////
CInputManager* CInputManager::GetInstance()
{
	static CInputManager cInputManager;
	return &cInputManager;
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“Initialize”
//
// Return:		void
//
// Parameters:	HWND hWnd			-	the handle to the window
//				HINSTANCE hInstance	-	the handle to the instance
//
// Purpose:		This function is used to initialize the Input Manager. All 
//				DirectX objects are initialized	and devices are polled.
////////////////////////////////////////////////////////////////////////////////
void CInputManager::Initialize(HWND hWnd, HINSTANCE hInstance)
{
	// Save the hWnd
	m_hWnd = hWnd;

	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, 
		(void**)&m_pDin, 0);
	m_pDin->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);

	// Set the data format and coop level for the keyboard
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
		| DISCL_NOWINKEY);

	// HACK: Fix to accomodate multiple Xbox360 Controllers
	DWORD dwResult = XInputGetState(0, m_pController);

	if(dwResult == ERROR_SUCCESS)
		m_bControllerConnected = true;
	else
		m_bControllerConnected = false;

#ifdef _DEBUG
	m_eGameState = MAIN_MENU_STATE;
#else
	m_eGameState =  INTRO_STATE;
#endif

	CEventManager::GetInstance()->RegisterEvent("GetInput", (IComponent*)GetInstance(), GetInput);
	CEventManager::GetInstance()->RegisterEvent("InputStateChange", (IComponent*)GetInstance(),
		InputStateChange);
	CEventManager::GetInstance()->RegisterEvent("PlayerCreated", (IComponent*)GetInstance(), SetPlayer);

	InitKeyStrings();
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“Shutdown”
//
// Return:		void
//
// Parameters:	void
//
// Purpose:		This function is used to shutdown the Input Manager. All DirectX
//				objects are released.
////////////////////////////////////////////////////////////////////////////////
void CInputManager::Shutdown()
{
	if(m_pController != NULL)
	{
		delete m_pController;
		m_pController = NULL;
	}
	if(m_pKeyboard != NULL)
	{
		m_pKeyboard->Release();
		m_pKeyboard = NULL;
	}
	if(m_pDin != NULL)
	{
		m_pDin->Release();
		m_pDin = NULL;
	}

	delete[] m_szKeyStrings;
}

void CInputManager::InputStateChange(IEvent* pEvent, IComponent*)
{
	// Cast event to CStateEvent
	TStateEvent* pStateEvent = (TStateEvent*)pEvent->GetData();
	GetInstance()->m_eGameState = pStateEvent->m_eToState;
}

void CInputManager::SetPlayer(IEvent* pEvent, IComponent*)
{
	CInputManager* pIM = GetInstance();
	TObjectEvent* pObjEvent = (TObjectEvent*)pEvent->GetData();
	int nPlayerNum = (int)(((char*)pObjEvent->m_pcObj->GetID())[6]) - '0';

	if(nPlayerNum == 0)
	{
		pIM->m_pPlayer0 = pObjEvent->m_pcObj;
	}
	else if(nPlayerNum == 1)
	{
		pIM->m_pPlayer1 = pObjEvent->m_pcObj;
	}
	else if(nPlayerNum == 2)
	{
		pIM->m_pPlayer2 = pObjEvent->m_pcObj;
	}
	else if(nPlayerNum == 3)
	{
		pIM->m_pPlayer3 = pObjEvent->m_pcObj;
	}
}

void CInputManager::Shutdown(IEvent*, IComponent*)
{
	GetInstance()->Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“ReadDevices”
//
// Return:		void
//
// Parameters:	void
//
// Purpose:		This function is used to read the current devices and collect 
//				info on their current state.
////////////////////////////////////////////////////////////////////////////////
void CInputManager::ReadDevices()
{
	m_pKeyboard->Acquire();
	m_pKeyboard->GetDeviceState(sizeof(m_chKeys), (void*)m_chKeys);
}


void CInputManager::GetInput(IEvent*, IComponent*)
{
	// Read Input from Devices based on Current GameState
	CInputManager* pIM = CInputManager::GetInstance();
	
	// Get Current Keyboard state
	pIM->ReadDevices();
	
	// Toggle Fullscreen
	if(KEYDOWN(pIM->m_chKeys, DIK_LALT) && KEYDOWN(pIM->m_chKeys, DIK_RETURN)
		&& !KEYDOWN(pIM->m_chPrevKeys, DIK_RETURN)) // buffered input only on return
	{
		SendInputEvent("ToggleFullScreen", (IComponent*)GetInstance(), pIM->m_pPlayer0, 1.0f);

		CGame::GetInstance()->SetIsWindowed(!CGame::GetInstance()->GetIsWindowed());
		Direct3DManager::GetInstance()->ChangeDisplayParam(CGame::GetInstance()->GetScreenWidth(),
			CGame::GetInstance()->GetScreenHeight(), CGame::GetInstance()->GetIsWindowed());
	}

	// Check for mouse click
	if(GetAsyncKeyState(VK_LBUTTON) && !pIM->m_bPrevLeftMouseDown)
	{
		pIM->m_bPrevLeftMouseDown = true;
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(pIM->m_hWnd, &mousePos);

		SendMouseEvent("LeftMouseClick", (IComponent*)pIM, mousePos.x, mousePos.y);
		char mspos[64];
		sprintf_s(mspos, "LMB: (%d, %d)", mousePos.x, mousePos.y);
		Debug.Print(mspos);
	}
	else if(!GetAsyncKeyState(VK_LBUTTON))
		pIM->m_bPrevLeftMouseDown = false;

	switch(pIM->m_eGameState)
	{
	case INTRO_STATE:
		{
			ShowCursor(false);
			pIM->GetInputIntro();
		}
		break;
	case WIN_STATE:
	case LOSE_STATE:
		{
			pIM->GetInputEndgame();
		}
		break;
	case CREDITS_STATE:
	case KEYBIND_STATE:
	case GAME_MODE_SELECT_STATE:
	case CHARACTER_SELECT_STATE:
	case CHARACTER_SELECT_STATE2:
	case CHARACTER_SELECT_STATE3:
	case CHARACTER_SELECT_STATE4:
	case CHARACTER_SELECT_STATE5:
	case CHARACTER_SELECT_STATE6:
	case CHARACTER_SELECT_STATE7:
	case PAUSE_STATE:
	case HOW_TO_PLAY_STATE:
	case HOW_TO_PLAY_STATE1:
	case HOW_TO_PLAY_STATE2:
	case HOW_TO_PLAY_STATE3:
	case HOW_TO_PLAY_STATE4:
	case HOW_TO_PLAY_STATE5:
	case HOW_TO_PLAY_STATE6:
	case HOW_TO_PLAY_STATE7:
	case HOW_TO_PLAY_STATE8:
	case HOW_TO_PLAY_STATE9:
	case HOW_TO_PLAY_STATE10:
	case HOW_TO_PLAY_STATE11:
	case HOW_TO_PLAY_STATE12:
	case HOW_TO_PLAY_STATE13:
	case HOW_TO_PLAY_STATE14:
	case HOW_TO_PLAY_STATE15:
	case IN_GAME_HOW_TO_PLAY_STATE:
	case LEVEL_SELECT_STATE:
	case LEVEL_SELECT_STATE2:
	case QUIT_CONFIRMATION_STATE:
	case MAIN_MENU_STATE:
		{
			pIM->GetInputMenu();
		}
		break;
	case GAMEPLAY_STATE:
		{
			pIM->GetInputGameplay();
		}
		break;
	case CONSOLE_STATE:
		{
			pIM->GetInputConsole();
		}
		break;
	case PAUSE_OPTIONS_STATE:
	case PAUSE_KEYBINDS_STATE:
	case OPTIONS_STATE:
		{
			pIM->GetInputOptions(); // separate options input because sliders need left and right not buffered
		}
		break;
	case MIN_STATE:
		{

		}
	};
	
	memcpy(pIM->m_chPrevKeys, pIM->m_chKeys, sizeof(pIM->m_chPrevKeys));
}

void CInputManager::GetInputIntro()
{
	if(CUnlockableManager::GetInstance()->GetIntroSkip())
	{
		if(BUFFERED_KEYDOWN(DIK_RETURN))
		{
			SendIEvent("PlayMenuMusic", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);
			SendStateEvent("StateChange", (IComponent*)GetInstance(),
				MAIN_MENU_STATE, PRIORITY_IMMEDIATE);
		}
	}
	if (BUFFERED_KEYDOWN(DIK_RETURN))
	{
		SendIEvent("EnterMainMenu", (IComponent*)GetInstance(), 0, PRIORITY_INPUT);
	}

	DWORD dwResult = XInputGetState(0, m_pController);

	if(dwResult == ERROR_SUCCESS)
	{
		m_bControllerConnected = true;
	}
	else
	{
		m_bControllerConnected = false;
	}

	if(m_bControllerConnected) 
	{
		if(m_pController->dwPacketNumber != m_dwPrevState)
		{
			if(m_pController->Gamepad.wButtons == m_tMenu.m_nControllerButton)
			{
				SendIEvent("EnterMainMenu", (IComponent*)GetInstance(), 0, PRIORITY_INPUT);
			}
		}

		m_dwPrevState = m_pController->dwPacketNumber;
	}
}

void CInputManager::GetInputMenu()
{
#pragma region KEYBOARD
////////////////////////////////////////////////////////////////////////////////
//	KEYBOARD INPUT
////////////////////////////////////////////////////////////////////////////////

	if(BUFFERED_KEYDOWN(m_tAccept.m_chKeyboardKey))
	{
		SendInputEvent("Accept", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tBack.m_chKeyboardKey))
	{		
		SendInputEvent("Back", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tUp.m_chKeyboardKey) || 
		BUFFERED_KEYDOWN(DIK_UP))
	{
		SendInputEvent("Up", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tDown.m_chKeyboardKey)|| 
		BUFFERED_KEYDOWN(DIK_DOWN))
	{
		SendInputEvent("Down", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tLeft.m_chKeyboardKey) || BUFFERED_KEYDOWN(DIK_LEFT))
	{
		SendInputEvent("Left", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tRight.m_chKeyboardKey) || BUFFERED_KEYDOWN(DIK_RIGHT))
	{
		SendInputEvent("Right", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, DIK_GRAVE) && !KEYDOWN(m_chPrevKeys, DIK_GRAVE))
	{
		SendStateEvent("PushState", (IComponent*)GetInstance(), CONSOLE_STATE);
	}

////////////////////////////////////////////////////////////////////////////////
#pragma endregion

#pragma region CONTROLLER
////////////////////////////////////////////////////////////////////////////////
//	XBOX 360 CONTROLLER INPUT
////////////////////////////////////////////////////////////////////////////////

	DWORD dwResult = XInputGetState(0, m_pController);

	if(dwResult == ERROR_SUCCESS)
	{
		m_bControllerConnected = true;
	}
	else
	{
		m_bControllerConnected = false;
	}

	if(m_bControllerConnected) 
	{
////////////////////////////////////////////////////////////////////////////////
// THUMB STICK DEAD ZONE
////////////////////////////////////////////////////////////////////////////////
		if(m_pController->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Right
			SendInputEvent("Right", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLX / 32767))); // <- Max Value
		}
		else if(m_pController->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Left
			SendInputEvent("Left", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLX	/ -32767))); // <- Max Value
		}
		else if(m_pController->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Up
			SendInputEvent("Up", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLY	/ 32767))); // <- Max Value
		}
		else if(m_pController->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Down
			SendInputEvent("Down", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLY	/ -32767))); // <- Max Value
		}
////////////////////////////////////////////////////////////////////////////////

		if(m_pController->dwPacketNumber != m_dwPrevState)
		{
			if(m_pController->Gamepad.wButtons == m_tAccept.m_nControllerButton)
			{
				SendInputEvent("Accept", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tBack.m_nControllerButton)
			{
				SendInputEvent("Back", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tUp.m_nControllerButton)
			{
				SendInputEvent("Up", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tDown.m_nControllerButton)
			{
				SendInputEvent("Down", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tLeft.m_nControllerButton)
			{
				SendInputEvent("Left", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tRight.m_nControllerButton)
			{
				SendInputEvent("Right", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
		}
		m_dwPrevState = m_pController->dwPacketNumber;
	}
////////////////////////////////////////////////////////////////////////////////
#pragma endregion
}


void CInputManager::GetInputOptions()
{
#pragma region KEYBOARD
////////////////////////////////////////////////////////////////////////////////
//	KEYBOARD INPUT
////////////////////////////////////////////////////////////////////////////////

	if(BUFFERED_KEYDOWN(m_tAccept.m_chKeyboardKey))
	{
		SendInputEvent("Accept", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tBack.m_chKeyboardKey))
	{		
		SendInputEvent("Back", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tUp.m_chKeyboardKey) || 
		BUFFERED_KEYDOWN(DIK_UP))
	{
		SendInputEvent("Up", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tDown.m_chKeyboardKey)|| 
		BUFFERED_KEYDOWN(DIK_DOWN))
	{
		SendInputEvent("Down", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, m_tLeft.m_chKeyboardKey) || KEYDOWN(m_chKeys, DIK_LEFT))
	{
		SendInputEvent("Left", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, m_tRight.m_chKeyboardKey) || KEYDOWN(m_chKeys, DIK_RIGHT))
	{
		SendInputEvent("Right", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, DIK_GRAVE) && !KEYDOWN(m_chPrevKeys, DIK_GRAVE))
	{
		SendStateEvent("PushState", (IComponent*)GetInstance(), CONSOLE_STATE);
	}

////////////////////////////////////////////////////////////////////////////////
#pragma endregion

#pragma region CONTROLLER
////////////////////////////////////////////////////////////////////////////////
//	XBOX 360 CONTROLLER INPUT
////////////////////////////////////////////////////////////////////////////////

	DWORD dwResult = XInputGetState(0, m_pController);

	if(dwResult == ERROR_SUCCESS)
	{
		m_bControllerConnected = true;
	}
	else
	{
		m_bControllerConnected = false;
	}

	if(m_bControllerConnected) 
	{
////////////////////////////////////////////////////////////////////////////////
// THUMB STICK DEAD ZONE
////////////////////////////////////////////////////////////////////////////////
		if(m_pController->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Right
			SendInputEvent("Right", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLX / 32767))); // <- Max Value
		}
		else if(m_pController->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Left
			SendInputEvent("Left", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLX	/ -32767))); // <- Max Value
		}
		else if(m_pController->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Up
			SendInputEvent("Up", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLY	/ 32767))); // <- Max Value
		}
		else if(m_pController->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Down
			SendInputEvent("Down", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLY	/ -32767))); // <- Max Value
		}
////////////////////////////////////////////////////////////////////////////////

		if(m_pController->dwPacketNumber != m_dwPrevState)
		{
			if(m_pController->Gamepad.wButtons == m_tAccept.m_nControllerButton)
			{
				SendInputEvent("Accept", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tBack.m_nControllerButton)
			{
				SendInputEvent("Back", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tUp.m_nControllerButton)
			{
				SendInputEvent("Up", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tDown.m_nControllerButton)
			{
				SendInputEvent("Down", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tLeft.m_nControllerButton)
			{
				SendInputEvent("Left", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tRight.m_nControllerButton)
			{
				SendInputEvent("Right", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
		}
		m_dwPrevState = m_pController->dwPacketNumber;
	}
////////////////////////////////////////////////////////////////////////////////
#pragma endregion
}


void CInputManager::GetInputEndgame()
{
#pragma region KEYBOARD
////////////////////////////////////////////////////////////////////////////////
//	KEYBOARD INPUT
////////////////////////////////////////////////////////////////////////////////

	if(BUFFERED_KEYDOWN(m_tAccept.m_chKeyboardKey))
	{
		SendInputEvent("AcceptWinState", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	
	if(KEYDOWN(m_chKeys, DIK_GRAVE) && !KEYDOWN(m_chPrevKeys, DIK_GRAVE))
	{
		SendStateEvent("PushState", (IComponent*)GetInstance(), CONSOLE_STATE);
	}

////////////////////////////////////////////////////////////////////////////////
#pragma endregion

#pragma region CONTROLLER
////////////////////////////////////////////////////////////////////////////////
//	XBOX 360 CONTROLLER INPUT
////////////////////////////////////////////////////////////////////////////////

	DWORD dwResult = XInputGetState(0, m_pController);

	if(dwResult == ERROR_SUCCESS)
	{
		m_bControllerConnected = true;
	}
	else
	{
		m_bControllerConnected = false;
	}

	if(m_bControllerConnected) 
	{
////////////////////////////////////////////////////////////////////////////////
// THUMB STICK DEAD ZONE
////////////////////////////////////////////////////////////////////////////////
		
////////////////////////////////////////////////////////////////////////////////

		if(m_pController->dwPacketNumber != m_dwPrevState)
		{
			if(m_pController->Gamepad.wButtons == m_tAccept.m_nControllerButton)
			{
				SendInputEvent("AcceptWinState", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
		}
		m_dwPrevState = m_pController->dwPacketNumber;
	}
////////////////////////////////////////////////////////////////////////////////
#pragma endregion
}


void CInputManager::GetInputGameplay()
{
////////////////////////////////////////////////////////////////////////////////
//	KEYBOARD INPUT
////////////////////////////////////////////////////////////////////////////////

	if(BUFFERED_KEYDOWN(m_tMenu.m_chKeyboardKey))
	{
		SendInputEvent("Menu", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tShoveLeft.m_chKeyboardKey))
	{
		SendInputEvent("ShoveLeft", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(BUFFERED_KEYDOWN(m_tShoveRight.m_chKeyboardKey))
	{
		SendInputEvent("ShoveRight", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, m_tAccelerate.m_chKeyboardKey))
	{
		SendInputEvent("Accelerate", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, m_tDecelerate.m_chKeyboardKey))
	{
		SendInputEvent("Decelerate", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, m_tSteerRight.m_chKeyboardKey))
	{
		SendInputEvent("SteerRight", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, m_tSteerLeft.m_chKeyboardKey))
	{
		SendInputEvent("SteerLeft", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}
	if(KEYDOWN(m_chKeys, m_tDrift.m_chKeyboardKey)) // Not buffered so you can hold it down
	{
		SendInputEvent("Drift", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}

	if(BUFFERED_KEYDOWN(DIK_RETURN))
	{
		SendStateEvent("GameplayEnterPressed", (IComponent*)GetInstance(), CONSOLE_STATE);
	}


	if(BUFFERED_KEYDOWN(DIK_GRAVE))
	{
		SendStateEvent("PushState", (IComponent*)GetInstance(), CONSOLE_STATE);
	}

	// Use HeldItem Slot 1
	if(BUFFERED_KEYDOWN(m_tUseItem1.m_chKeyboardKey))
	{
		SendInputEvent("UseHeldItem1", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}

	// Use HeldItem Slot 2
	if(BUFFERED_KEYDOWN(m_tUseItem2.m_chKeyboardKey))
	{
		SendInputEvent("UseHeldItem2", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
	}

	// F1 Help
	if(BUFFERED_KEYDOWN(DIK_F1))
	{
		SendStateEvent("PushState", (IComponent*)GetInstance(), IN_GAME_HOW_TO_PLAY_STATE);
	}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	XBOX 360 CONTROLLER INPUT
////////////////////////////////////////////////////////////////////////////////

	DWORD dwResult = XInputGetState(0, m_pController);

	if(dwResult == ERROR_SUCCESS)
	{
		m_bControllerConnected = true;
	}
	else
	{
		m_bControllerConnected = false;
	}

	if(m_bControllerConnected) 
	{
		// For throwing items forward/backwards
		int nBackwards = 0;

////////////////////////////////////////////////////////////////////////////////
// THUMB STICK DEAD ZONE
////////////////////////////////////////////////////////////////////////////////
		if(m_pController->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Right
			SendInputEvent("SteerRight", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLX / 32767.0f))); // <- Max Value
		}
		else if(m_pController->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Left
			SendInputEvent("SteerLeft", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLX / -32767.0f))); // <- Max Value
		}
		if(m_pController->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Up
			SendInputEvent("Up", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLY / 32767.0f))); // <- Max Value
			nBackwards = 1;
		}
		else if(m_pController->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//	Move Down
			SendInputEvent("Down", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.sThumbLY / -32767.0f))); // <- Max Value
			nBackwards = -1;
		}

		if(m_pController->Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			SendInputEvent("Accelerate", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.bRightTrigger / 255.0f)));
		}
		if(m_pController->Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			SendInputEvent("Decelerate", (IComponent*)GetInstance(), m_pPlayer0, 
				(float(m_pController->Gamepad.bLeftTrigger / 255.0f)));
		}
		if(m_pController->Gamepad.wButtons == m_tDrift.m_nControllerButton)
		{
			SendInputEvent("Drift", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);

		}
		if(m_pController->Gamepad.wButtons == XINPUT_GAMEPAD_A)
		{
			// send input "A" button pressed for start of race splash screen
			SendStateEvent("GameplayEnterPressed", (IComponent*)GetInstance(), CONSOLE_STATE);
		}

////////////////////////////////////////////////////////////////////////////////

		// Buffered input stuff
		if(m_pController->dwPacketNumber != m_dwPrevState)
		{
			if(m_pController->Gamepad.wButtons == m_tMenu.m_nControllerButton)
			{
				SendInputEvent("Menu", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tShoveLeft.m_nControllerButton)
			{
				SendInputEvent("ShoveLeft", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tShoveRight.m_nControllerButton)
			{
				SendInputEvent("ShoveRight", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tUseItem1.m_nControllerButton)
			{
				SendInputEvent("UseHeldItem1", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
			if(m_pController->Gamepad.wButtons == m_tUseItem2.m_nControllerButton)
			{
				SendInputEvent("UseHeldItem2", (IComponent*)GetInstance(), m_pPlayer0, 1.0f);
			}
		}

		m_dwPrevState = m_pController->dwPacketNumber;
	}
////////////////////////////////////////////////////////////////////////////////
}

void CInputManager::GetInputConsole()
{
	bool bShift = KEYDOWN(m_chKeys, DIK_LSHIFT) || KEYDOWN(m_chKeys, DIK_RSHIFT);

	if(BUFFERED_KEYDOWN(DIK_RETURN))
		SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '\n');
	if(BUFFERED_KEYDOWN(DIK_GRAVE))
		SendIEvent("PopState", (IComponent*)GetInstance(), NULL, PRIORITY_INPUT);
	if(BUFFERED_KEYDOWN(DIK_PERIOD))
		SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '.');
	if(BUFFERED_KEYDOWN(DIK_COMMA))
		SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), ',');
	if(BUFFERED_KEYDOWN(DIK_APOSTROPHE) && bShift)
		SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '\"');
	if(BUFFERED_KEYDOWN(DIK_SPACE))
		SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), ' ');
	if(BUFFERED_KEYDOWN(DIK_BACKSPACE))
		SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '\b');

	// Alpha-Numeric Characters
	if(BUFFERED_KEYDOWN(DIK_A))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'A');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'a');
	}
	if(BUFFERED_KEYDOWN(DIK_B))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'B');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'b');
	}
	if(BUFFERED_KEYDOWN(DIK_C))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'C');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'c');
	}
	if(BUFFERED_KEYDOWN(DIK_D))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'D');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'd');
	}
	if(BUFFERED_KEYDOWN(DIK_E))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'E');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'e');
	}
	if(BUFFERED_KEYDOWN(DIK_F))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'F');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'f');
	}
	if(BUFFERED_KEYDOWN(DIK_G))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'G');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'g');
	}
	if(BUFFERED_KEYDOWN(DIK_H))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'H');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'h');
	}
	if(BUFFERED_KEYDOWN(DIK_I))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'I');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'i');
	}
	if(BUFFERED_KEYDOWN(DIK_J))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'J');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'j');
	}
	if(BUFFERED_KEYDOWN(DIK_K))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'K');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'k');
	}
	if(BUFFERED_KEYDOWN(DIK_L))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'L');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'l');
	}
	if(BUFFERED_KEYDOWN(DIK_M))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'M');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'm');
	}
	if(BUFFERED_KEYDOWN(DIK_N))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'N');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'n');
	}
	if(BUFFERED_KEYDOWN(DIK_O))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'O');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'o');
	}
	if(BUFFERED_KEYDOWN(DIK_P))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'P');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'p');
	}
	if(BUFFERED_KEYDOWN(DIK_Q))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'Q');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'q');
	}
	if(BUFFERED_KEYDOWN(DIK_R))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'R');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'r');
	}
	if(BUFFERED_KEYDOWN(DIK_S))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'S');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 's');
	}
	if(BUFFERED_KEYDOWN(DIK_T))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'T');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 't');
	}
	if(BUFFERED_KEYDOWN(DIK_U))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'U');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'u');
	}
	if(BUFFERED_KEYDOWN(DIK_V))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'V');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'v');
	}
	if(BUFFERED_KEYDOWN(DIK_W))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'W');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'w');
	}
	if(BUFFERED_KEYDOWN(DIK_X))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'X');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'x');
	}
	if(BUFFERED_KEYDOWN(DIK_Y))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'Y');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'y');
	}
	if(BUFFERED_KEYDOWN(DIK_Z))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'Z');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), 'z');
	}
	if(BUFFERED_KEYDOWN(DIK_1))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '!');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '1');
	}
	if(BUFFERED_KEYDOWN(DIK_2))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '@');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '2');
	}
	if(BUFFERED_KEYDOWN(DIK_3))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '#');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '3');
	}
	if(BUFFERED_KEYDOWN(DIK_4))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '$');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '4');
	}
	if(BUFFERED_KEYDOWN(DIK_5))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '%');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '5');
	}
	if(BUFFERED_KEYDOWN(DIK_6))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '^');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '6');
	}
	if(BUFFERED_KEYDOWN(DIK_7))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '&');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '7');
	}
	if(BUFFERED_KEYDOWN(DIK_8))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '*');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '8');
	}
	if(BUFFERED_KEYDOWN(DIK_9))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '(');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '9');
	}
	if(BUFFERED_KEYDOWN(DIK_0))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), ')');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '0');
	}
	if(BUFFERED_KEYDOWN(DIK_MINUS))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '_');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '-');
	}
	if(BUFFERED_KEYDOWN(DIK_EQUALS))
	{
		if(bShift)
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '+');
		else
			SendConsoleEvent("KeyPressed", (IComponent*)GetInstance(), '=');
	}
}

string CInputManager::SetCommand(bool bController, int nCommandID)
{
	int nButton = 0;
	unsigned char uchKey = 0;

	if(bController)
	{
		nButton = SetControllerCommand(nCommandID);
		if(nButton != 0)
		{
			return GetStringController(nButton);
		}
	}
	else
	{
		uchKey = SetKeyboardCommand(nCommandID);
		if(uchKey != 0)
		{
			return GetStringKeyboard(uchKey);
		}
	}
	return "";
}

BYTE CInputManager::SetKeyboardCommand(int nCommandID, BYTE uchKeyCode)
{
	if(uchKeyCode == 0)
	{
		memset(m_chKeys, 0, 256);

		while(uchKeyCode == 0)
		{
			m_pKeyboard->Acquire();
			m_pKeyboard->GetDeviceState(sizeof(m_chKeys), (void*)m_chKeys);

			for(int i=0; i<256; i++)
			{
				if(KEYDOWN(m_chKeys, i))
				{
					uchKeyCode = (unsigned char)i;
					break;
				}
			}
		}
	}

	switch(nCommandID)
	{
	case CMD_ACCEPT:
		{
			m_tAccept.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_BACK:
		{
			m_tBack.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_UP:
		{
			m_tUp.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_DOWN:
		{
			m_tDown.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_LEFT:
		{
			m_tLeft.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_RIGHT:
		{
			m_tRight.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_MENU:
		{
			m_tMenu.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_ACCELERATE:
		{
			m_tAccelerate.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_DECELERATE:
		{
			m_tDecelerate.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_STEERLEFT:
		{
			m_tSteerLeft.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_STEERRIGHT:
		{
			m_tSteerRight.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_DRIFT:
		{
			m_tDrift.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_USEITEM1:
		{
			m_tUseItem1.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_USEITEM2:
		{
			m_tUseItem2.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_SHOVELEFT:
		{
			m_tShoveLeft.m_chKeyboardKey = uchKeyCode;
		}
		break;
	case CMD_SHOVERIGHT:
		{
			m_tShoveRight.m_chKeyboardKey = uchKeyCode;
		}
		break;
	};

	return uchKeyCode;
}

int CInputManager::GetControllerButtonDown()
{
	XInputGetState(0, m_pController);
	m_dwPrevState = m_pController->dwPacketNumber;

	return m_pController->Gamepad.wButtons;
}


int CInputManager::SetControllerCommand(int nCommandID, int nButton)
{
	XINPUT_KEYSTROKE button = {};
	DWORD dwGetStateResult;
	DWORD dwGetKeyResult;
	int nControllerButton = 0;

	while(nControllerButton == 0)
	{
		if(nButton == 0)
		{
			do
			{
				dwGetStateResult = XInputGetState(0, m_pController);
			}while(m_pController->dwPacketNumber == m_dwPrevState);

			if(dwGetStateResult == ERROR_SUCCESS)
			{
				dwGetKeyResult = XInputGetKeystroke(0, XINPUT_FLAG_GAMEPAD, &button);
			}
			else
			{
				dwGetKeyResult = dwGetStateResult;
			}

			if(dwGetKeyResult == ERROR_SUCCESS)
			{
				switch(button.VirtualKey)
				{
				case VK_PAD_A:
					{
						nControllerButton = XINPUT_GAMEPAD_A;
					}
					break;
				case VK_PAD_B:
					{
						nControllerButton = XINPUT_GAMEPAD_B;
					}
					break;
				case VK_PAD_X:
					{
						nControllerButton = XINPUT_GAMEPAD_X;
					}
					break;
				case VK_PAD_Y:
					{
						nControllerButton = XINPUT_GAMEPAD_Y;
					}
					break;
				case VK_PAD_RSHOULDER:
					{
						nControllerButton = XINPUT_GAMEPAD_RIGHT_SHOULDER;
					}
					break;
				case VK_PAD_LSHOULDER:
					{
						nControllerButton = XINPUT_GAMEPAD_LEFT_SHOULDER;
					}
					break;
				case VK_PAD_DPAD_UP:
					{
						nControllerButton = XINPUT_GAMEPAD_DPAD_UP;
					}
					break;
				case VK_PAD_DPAD_DOWN:
					{
						nControllerButton = XINPUT_GAMEPAD_DPAD_DOWN;
					}
					break;
				case VK_PAD_DPAD_LEFT:
					{
						nControllerButton = XINPUT_GAMEPAD_DPAD_LEFT;
					}
					break;
				case VK_PAD_DPAD_RIGHT:
					{
						nControllerButton = XINPUT_GAMEPAD_DPAD_RIGHT;
					}
					break;
				case VK_PAD_START:
					{
						nControllerButton = XINPUT_GAMEPAD_START;
					}
					break;
				case VK_PAD_BACK:
					{
						nControllerButton = XINPUT_GAMEPAD_BACK;
					}
					break;
				case VK_PAD_LTHUMB_PRESS:
					{
						nControllerButton = XINPUT_GAMEPAD_LEFT_THUMB;
					}
					break;
				case VK_PAD_RTHUMB_PRESS:
					{
						nControllerButton = XINPUT_GAMEPAD_RIGHT_THUMB;
					}
					break;
				};
			}
		}
		else
			nControllerButton = nButton;

		if(nControllerButton != 0)
		{
			switch(nCommandID)
			{
			case CMD_SHOVELEFT:
				m_tShoveLeft.m_nControllerButton = nControllerButton;
				break;
			case CMD_SHOVERIGHT:
				m_tShoveRight.m_nControllerButton = nControllerButton;
				break;
			case CMD_USEITEM1:
				m_tUseItem1.m_nControllerButton = nControllerButton;
				break;
			case CMD_USEITEM2:
				m_tUseItem2.m_nControllerButton = nControllerButton;
				break;
			case CMD_DRIFT:
				m_tDrift.m_nControllerButton = nControllerButton;
				break;
			};
		}
	}
	return nControllerButton;
}

bool CInputManager::IsControllerConnected()
{
	return m_bControllerConnected;
}

string CInputManager::GetStringController(int nButton)
{
	switch(nButton)
	{
	case XINPUT_GAMEPAD_A:
		{
			return "A";
		}
	case XINPUT_GAMEPAD_B:
		{
			return "B";
		}
	case XINPUT_GAMEPAD_X:
		{
			return "X";
		}
	case XINPUT_GAMEPAD_Y:
		{
			return "Y";
		}
	case XINPUT_GAMEPAD_RIGHT_SHOULDER:
		{
			return "Right Shoulder";
		}
	case XINPUT_GAMEPAD_LEFT_SHOULDER:
		{
			return "Left Shoulder";
		}
	case XINPUT_GAMEPAD_START:
		{
			return "Start";
		}
	case XINPUT_GAMEPAD_BACK:
		{
			return "Back";
		}
	case XINPUT_GAMEPAD_RIGHT_THUMB:
		{
			return "Right Thumb";
		}
	case XINPUT_GAMEPAD_LEFT_THUMB:
		{
			return "Left Thumb";
		}
	case CMD_STEERLEFT:
		{
			return "LS";
		}
	case CMD_STEERRIGHT:
		{
			return "RS";
		}
	case CMD_ACCELERATE:
		{
			return "RT";
		}
	case CMD_DECELERATE:
		{
			return "LT";
		}
	default:
		{
			return "";
		}
	};
}

string CInputManager::GetStringKeyboard(unsigned char uchKey)
{
	int nKey = (int)uchKey;
	return m_szKeyStrings[nKey];
}

int CInputManager::GetControllerButton(int nCommand)
{
	switch(nCommand)
	{
	case CMD_ACCEPT:
		{
			return m_tAccept.m_nControllerButton;
		}
	case CMD_BACK:
		{
			return m_tBack.m_nControllerButton;
		}
	case CMD_SHOVELEFT:
		{
			return m_tShoveLeft.m_nControllerButton;
		}
		break;
	case CMD_SHOVERIGHT:
		{
			return m_tShoveRight.m_nControllerButton;
		}
		break;
	case CMD_USEITEM1:
		{
			return m_tUseItem1.m_nControllerButton;
		}
		break;
	case CMD_USEITEM2:
		{
			return m_tUseItem2.m_nControllerButton;
		}
		break;
	case CMD_DRIFT:
		{
			return m_tDrift.m_nControllerButton;
		}
		break;
	case CMD_UP:
		{
			return m_tUp.m_nControllerButton;
		}
	case CMD_DOWN:
		{
			return m_tDown.m_nControllerButton;
		}
	case CMD_LEFT:
		{
			return m_tLeft.m_nControllerButton;
		}
	case CMD_RIGHT:
		{
			return m_tRight.m_nControllerButton;
		}
	case CMD_MENU:
		{
			return m_tMenu.m_nControllerButton;
		}
	case CMD_ACCELERATE:
		{
			return CMD_ACCELERATE;
		}
	case CMD_DECELERATE:
		{
			return CMD_DECELERATE;
		}
	case CMD_STEERLEFT:
		{
			return CMD_STEERLEFT;
		}
	case CMD_STEERRIGHT:
		{
			return CMD_STEERRIGHT;
		}
	default:
		{
			return 0;
		}
	};
}

BYTE CInputManager::GetKeyboardKey(int nCommand)
{
	switch(nCommand)
	{
	case CMD_ACCEPT:
		{
			return m_tAccept.m_chKeyboardKey;
		}
	case CMD_BACK:
		{
			return m_tBack.m_chKeyboardKey;
		}
	case CMD_SHOVELEFT:
		{
			return m_tShoveLeft.m_chKeyboardKey;
		}
		break;
	case CMD_SHOVERIGHT:
		{
			return m_tShoveRight.m_chKeyboardKey;
		}
		break;
	case CMD_USEITEM1:
		{
			return m_tUseItem1.m_chKeyboardKey;
		}
		break;
	case CMD_USEITEM2:
		{
			return m_tUseItem2.m_chKeyboardKey;
		}
		break;
	case CMD_DRIFT:
		{
			return m_tDrift.m_chKeyboardKey;
		}
		break;
	case CMD_UP:
		{
			return m_tUp.m_chKeyboardKey;
		}
	case CMD_DOWN:
		{
			return m_tDown.m_chKeyboardKey;
		}
	case CMD_LEFT:
		{
			return m_tLeft.m_chKeyboardKey;
		}
	case CMD_STEERLEFT:
		{
			return m_tSteerLeft.m_chKeyboardKey;
		}
	case CMD_STEERRIGHT:
		{
			return m_tSteerRight.m_chKeyboardKey;
		}
	case CMD_RIGHT:
		{
			return m_tRight.m_chKeyboardKey;
		}
	case CMD_ACCELERATE:
		{
			return m_tAccelerate.m_chKeyboardKey;
		}
	case CMD_DECELERATE:
		{
			return m_tDecelerate.m_chKeyboardKey;
		}
	case CMD_MENU:
		{
			return m_tMenu.m_chKeyboardKey;
		}
	default:
		{
			return 0;
		}
	};
}

void CInputManager::InitKeyStrings()
{
	m_szKeyStrings = new string[256];

	m_szKeyStrings[DIK_ESCAPE] = "Esc";
	m_szKeyStrings[DIK_1] = "1";
	m_szKeyStrings[DIK_2] = "2";
	m_szKeyStrings[DIK_3] = "3";
	m_szKeyStrings[DIK_4] = "4";
	m_szKeyStrings[DIK_5] = "5";
	m_szKeyStrings[DIK_6] = "6";
	m_szKeyStrings[DIK_7] = "7";
	m_szKeyStrings[DIK_8] = "8";
	m_szKeyStrings[DIK_9] = "9";
	m_szKeyStrings[DIK_0] = "0";
	m_szKeyStrings[DIK_MINUS] = "-";
	m_szKeyStrings[DIK_EQUALS] = "=";
	m_szKeyStrings[DIK_BACK] = "Backspace";
	m_szKeyStrings[DIK_TAB] = "Tab";
	m_szKeyStrings[DIK_Q] = "Q";
	m_szKeyStrings[DIK_W] = "W";
	m_szKeyStrings[DIK_E] = "E";
	m_szKeyStrings[DIK_R] = "R";
	m_szKeyStrings[DIK_T] = "T";
	m_szKeyStrings[DIK_Y] = "Y";
	m_szKeyStrings[DIK_U] = "U";
	m_szKeyStrings[DIK_I] = "I";
	m_szKeyStrings[DIK_O] = "O";
	m_szKeyStrings[DIK_P] = "P";
	m_szKeyStrings[DIK_LBRACKET] = "[";
	m_szKeyStrings[DIK_RBRACKET] = "]";
	m_szKeyStrings[DIK_RETURN] = "Enter";
	m_szKeyStrings[DIK_LCONTROL] = "LCtrl";
	m_szKeyStrings[DIK_A] = "A";
	m_szKeyStrings[DIK_S] = "S";
	m_szKeyStrings[DIK_D] = "D";
	m_szKeyStrings[DIK_F] = "F";
	m_szKeyStrings[DIK_G] = "G";
	m_szKeyStrings[DIK_H] = "H";
	m_szKeyStrings[DIK_J] = "J";
	m_szKeyStrings[DIK_K] = "K";
	m_szKeyStrings[DIK_L] = "L";
	m_szKeyStrings[DIK_SEMICOLON] = ";";
	m_szKeyStrings[DIK_APOSTROPHE] = "'";
	m_szKeyStrings[DIK_GRAVE] = "`";
	m_szKeyStrings[DIK_LSHIFT] = "LShift";
	m_szKeyStrings[DIK_BACKSLASH] = "\\";
	m_szKeyStrings[DIK_Z] = "Z";
	m_szKeyStrings[DIK_X] = "X";
	m_szKeyStrings[DIK_C] = "C";
	m_szKeyStrings[DIK_V] = "V";
	m_szKeyStrings[DIK_B] = "B";
	m_szKeyStrings[DIK_N] = "N";
	m_szKeyStrings[DIK_M] = "M";
	m_szKeyStrings[DIK_COMMA] = ",";
	m_szKeyStrings[DIK_PERIOD] = ".";
	m_szKeyStrings[DIK_SLASH] = "/";
	m_szKeyStrings[DIK_RSHIFT] = "RShift";
	m_szKeyStrings[DIK_MULTIPLY] = "*";
	m_szKeyStrings[DIK_LMENU] = "LAlt";
	m_szKeyStrings[DIK_SPACE] = "Space";
	m_szKeyStrings[DIK_CAPITAL] = "Caps Lock";
	m_szKeyStrings[DIK_F1] = "F1";
	m_szKeyStrings[DIK_F2] = "F2";
	m_szKeyStrings[DIK_F3] = "F3";
	m_szKeyStrings[DIK_F4] = "F4";
	m_szKeyStrings[DIK_F5] = "F5";
	m_szKeyStrings[DIK_F6] = "F6";
	m_szKeyStrings[DIK_F7] = "F7";
	m_szKeyStrings[DIK_F8] = "F8";
	m_szKeyStrings[DIK_F9] = "F9";
	m_szKeyStrings[DIK_F10] = "F10";
	m_szKeyStrings[DIK_NUMLOCK] = "Num Lock";
	m_szKeyStrings[DIK_SCROLL] = "Scroll Lock";
	m_szKeyStrings[DIK_NUMPAD7] = "Numpad 7";
	m_szKeyStrings[DIK_NUMPAD8] = "Numpad 8";
	m_szKeyStrings[DIK_NUMPAD9] = "Numpad 9";
	m_szKeyStrings[DIK_SUBTRACT] = "Numpad -";
	m_szKeyStrings[DIK_NUMPAD4] = "Numpad 4";
	m_szKeyStrings[DIK_NUMPAD5] = "Numpad 5";
	m_szKeyStrings[DIK_NUMPAD6] = "Numpad 6";
	m_szKeyStrings[DIK_ADD] = "Numpad +";
	m_szKeyStrings[DIK_NUMPAD1] = "Numpad 1";
	m_szKeyStrings[DIK_NUMPAD2] = "Numpad 2";
	m_szKeyStrings[DIK_NUMPAD3] = "Numpad 3";
	m_szKeyStrings[DIK_NUMPAD0] = "Numpad 4";
	m_szKeyStrings[DIK_DECIMAL] = "Numpad .";
	m_szKeyStrings[DIK_OEM_102] = "OEM_102";
	m_szKeyStrings[DIK_F11] = "F11";
	m_szKeyStrings[DIK_F12] = "F12";
	m_szKeyStrings[DIK_F13] = "F13";
	m_szKeyStrings[DIK_F14] = "F14";
	m_szKeyStrings[DIK_F15] = "F15";
	m_szKeyStrings[DIK_KANA] = "Kana (JP)";
	m_szKeyStrings[DIK_ABNT_C1] = "/? (Brazilian)";
	m_szKeyStrings[DIK_CONVERT] = "Convert (JP)";
	m_szKeyStrings[DIK_NOCONVERT] = "No Convert (JP)";
	m_szKeyStrings[DIK_YEN] = "Yen (JP)";
	m_szKeyStrings[DIK_ABNT_C2] = "Numpad . (Brazilian)";
	m_szKeyStrings[DIK_NUMPADEQUALS] = "Numpad  =";
	m_szKeyStrings[DIK_PREVTRACK] = "Previous Track";
	m_szKeyStrings[DIK_AT] = "@";
	m_szKeyStrings[DIK_COLON] = ":";
	m_szKeyStrings[DIK_UNDERLINE] = "_";
	m_szKeyStrings[DIK_KANJI] = "Kanji (JP)";
	m_szKeyStrings[DIK_STOP] = "Stop";
	m_szKeyStrings[DIK_AX] = "AX (JP)";
	m_szKeyStrings[DIK_UNLABELED] = "Unlabeled";
	m_szKeyStrings[DIK_NEXTTRACK] = "Next Track";
	m_szKeyStrings[DIK_NUMPADENTER] = "Numpad Enter";
	m_szKeyStrings[DIK_RCONTROL] = "Right Ctrl";
	m_szKeyStrings[DIK_MUTE] = "Mute";
	m_szKeyStrings[DIK_CALCULATOR] = "Calculator";
	m_szKeyStrings[DIK_PLAYPAUSE] = "Play/Pause";
	m_szKeyStrings[DIK_MEDIASTOP] = "Media Stop";
	m_szKeyStrings[DIK_VOLUMEDOWN] = "Volume -";
	m_szKeyStrings[DIK_VOLUMEUP] = "Volume +";
	m_szKeyStrings[DIK_WEBHOME] = "Web Home";
	m_szKeyStrings[DIK_NUMPADCOMMA] = "Numpad ,";
	m_szKeyStrings[DIK_DIVIDE] = "Numpad /";
	m_szKeyStrings[DIK_SYSRQ] = "SYSRQ";
	m_szKeyStrings[DIK_RMENU] = "Right Alt";
	m_szKeyStrings[DIK_PAUSE] = "Pause";
	m_szKeyStrings[DIK_HOME] = "Home";
	m_szKeyStrings[DIK_UP] = "Up";
	m_szKeyStrings[DIK_PRIOR] = "Pg Up";
	m_szKeyStrings[DIK_LEFT] = "Left";
	m_szKeyStrings[DIK_RIGHT] = "Right";
	m_szKeyStrings[DIK_END] = "End";
	m_szKeyStrings[DIK_DOWN] = "Down";
	m_szKeyStrings[DIK_NEXT] = "Pg Dn";
	m_szKeyStrings[DIK_INSERT] = "Insert";
	m_szKeyStrings[DIK_DELETE] = "Delete";
	m_szKeyStrings[DIK_LWIN] = "Left Windows";
	m_szKeyStrings[DIK_RWIN] = "Right Windows";
	m_szKeyStrings[DIK_APPS] = "AppMenu";
	m_szKeyStrings[DIK_POWER] = "System Power";
	m_szKeyStrings[DIK_SLEEP] = "System Sleep";
	m_szKeyStrings[DIK_WAKE] = "System Wake";
	m_szKeyStrings[DIK_WEBSEARCH] = "Web Search";
	m_szKeyStrings[DIK_WEBFAVORITES] = "Web Favorites";
	m_szKeyStrings[DIK_WEBREFRESH] = "Web Refresh";
	m_szKeyStrings[DIK_WEBSTOP] = "Web Stop";
	m_szKeyStrings[DIK_WEBFORWARD] = "Web Forward";
	m_szKeyStrings[DIK_WEBBACK] = "Web Back";
	m_szKeyStrings[DIK_MYCOMPUTER] = "My Computer";
	m_szKeyStrings[DIK_MAIL] = "Mail";
	m_szKeyStrings[DIK_MEDIASELECT] = "Media Select";
}

#undef BUFFERED_KEYDOWN
#undef KEYDOWN