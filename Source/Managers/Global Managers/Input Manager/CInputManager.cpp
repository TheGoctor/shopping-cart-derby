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
#include "../Event Manager/CStateEvent.h"
#include "../Event Manager/CInputEvent.h"

#define CONTROLLER 0

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
	m_tUp.m_chKeyboardKey			= DIK_W;

	m_tDown.m_nID					= CMD_DOWN;
	m_tDown.m_nControllerButton		= XINPUT_GAMEPAD_DPAD_DOWN;
	m_tDown.m_chKeyboardKey			= DIK_S;

	m_tLeft.m_nID					= CMD_LEFT;
	m_tLeft.m_nControllerButton		= XINPUT_GAMEPAD_DPAD_LEFT;
	m_tLeft.m_chKeyboardKey			= DIK_A;

	m_tRight.m_nID					= CMD_RIGHT;
	m_tRight.m_nControllerButton	= XINPUT_GAMEPAD_DPAD_RIGHT;
	m_tRight.m_chKeyboardKey		= DIK_D;

	// Gameplay Commands
	m_tMenu.m_nID					= CMD_MENU;
	m_tMenu.m_nControllerButton		= XINPUT_GAMEPAD_START;
	m_tMenu.m_chKeyboardKey			= DIK_ESCAPE;

	m_tAccelerate.m_nID				= CMD_ACCELERATE;
	m_tAccelerate.m_nControllerButton = 0; //XINPUT_GAMEPAD.bRightTrigger;
	m_tAccelerate.m_chKeyboardKey	= DIK_W;

	m_tDecelerate.m_nID				= CMD_DECELERATE;
	m_tDecelerate.m_nControllerButton = 0; //XINPUT_GAMEPAD.bLeftTrigger;
	m_tDecelerate.m_chKeyboardKey	= DIK_S;

	m_tSteerLeft.m_nID				= CMD_STEERLEFT;
	m_tSteerLeft.m_nControllerButton = 0; //LeftStick
	m_tSteerLeft.m_chKeyboardKey	= DIK_A;
	
	m_tSteerRight.m_nID				= CMD_STEERRIGHT;
	m_tSteerRight.m_nControllerButton = 0; //LeftStick
	m_tSteerRight.m_chKeyboardKey	= DIK_D;
	
	m_tShoveLeft.m_nID				= CMD_SHOVELEFT;
	m_tShoveLeft.m_nControllerButton = XINPUT_GAMEPAD_LEFT_SHOULDER;
	m_tShoveLeft.m_chKeyboardKey	= DIK_Q;
	
	m_tShoveRight.m_nID				= CMD_SHOVERIGHT;
	m_tShoveRight.m_nControllerButton = XINPUT_GAMEPAD_RIGHT_SHOULDER;
	m_tShoveRight.m_chKeyboardKey	= DIK_E;

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
	static CInputManager instance;
	return &instance;
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
	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, 
		(void**)&m_pDin, 0);
	m_pDin->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
		| DISCL_NOWINKEY);

	// HACK: Fix to accomodate multiple Xbox360 Controllers
	DWORD dwResult = XInputGetState(0, m_pController);

	if(dwResult == ERROR_SUCCESS)
		m_bControllerConnected = true;
	else
		m_bControllerConnected = false;

	// HACK: Change to INTRO_STATE
	m_eGameState = MAIN_MENU_STATE;

	m_pfGetInput = &CInputManager::GetInputMenu;

	CEventManager::GetInstance()->RegisterEvent("GetInput", NULL, GetInput);
	CEventManager::GetInstance()->RegisterEvent("InputStateChange", NULL,
		InputStateChange);

	//InitKeyStrings();
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

	//delete[] m_szKeyStrings;
}

void CInputManager::InputStateChange(IEvent* pEvent, IComponent* pComponent)
{
	// Cast event to CStateEvent
	CStateEvent* pStateEvent = (CStateEvent*)pEvent;
	GetInstance()->m_eGameState = pStateEvent->GetToState();
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
void CInputManager::GetInput(IEvent*, IComponent*)
{
	// Read Input from Devices based on Current GameState
	CInputManager* pIM = CInputManager::GetInstance();
	switch(pIM->m_eGameState)
	{
	case INTRO_STATE:
		{
			pIM->GetInputIntro();
		}
		break;
	case PAUSE_STATE:
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
	};
}

void CInputManager::GetInputIntro()
{
}

void CInputManager::GetInputMenu()
{
	CEventManager* pEM = CEventManager::GetInstance();

#pragma region KEYBOARD
////////////////////////////////////////////////////////////////////////////////
//	KEYBOARD INPUT
////////////////////////////////////////////////////////////////////////////////
#define KEYDOWN(name, key) (name[key] & 0x80  ? true : false)

	m_pKeyboard->Acquire();

	m_pKeyboard->GetDeviceState(sizeof(m_chKeys), (void*)m_chKeys);

	if(KEYDOWN(m_chKeys, m_tAccept.m_chKeyboardKey) /*&& !KEYDOWN(m_chPrevKeys, 
		m_tAccept.m_chKeyboardKey)*/)
	{
		//m_cInput.push(m_tAccept);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
			CInputEvent(CIDGen::GetInstance()->GetID("Accept"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if(KEYDOWN(m_chKeys, m_tBack.m_chKeyboardKey) /*&& !KEYDOWN(m_chPrevKeys, 
		m_tBack.m_chKeyboardKey)*/)
	{
		//m_cInput.push(m_tBack);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("Back"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if((KEYDOWN(m_chKeys, m_tUp.m_chKeyboardKey) /*&& !KEYDOWN(m_chPrevKeys, 
		m_tUp.m_chKeyboardKey)*/) || (KEYDOWN(m_chKeys, DIK_UP) /*&& !KEYDOWN(
		m_chPrevKeys, DIK_UP)*/))
	{
		//m_cInput.push(m_tUp);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("Up"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if((KEYDOWN(m_chKeys, m_tDown.m_chKeyboardKey) /*&& !KEYDOWN(m_chPrevKeys, 
		m_tDown.m_chKeyboardKey)*/) || (KEYDOWN(m_chKeys, DIK_DOWN) /*&& !KEYDOWN(
		m_chPrevKeys, DIK_DOWN)*/))
	{
		//m_cInput.push(m_tDown);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("Down"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if((KEYDOWN(m_chKeys, m_tLeft.m_chKeyboardKey) /*&& !KEYDOWN(m_chPrevKeys, 
		m_tLeft.m_chKeyboardKey)*/) || (KEYDOWN(m_chKeys, DIK_LEFT) /*&& !KEYDOWN(
		m_chPrevKeys, DIK_LEFT)*/))
	{
		//m_cInput.push(m_tLeft);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("Left"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if((KEYDOWN(m_chKeys, m_tRight.m_chKeyboardKey) /*&& !KEYDOWN(m_chPrevKeys, 
		m_tRight.m_chKeyboardKey)*/) || (KEYDOWN(m_chKeys, DIK_RIGHT) /*&& !KEYDOWN(
		m_chPrevKeys, DIK_RIGHT)*/))
	{
		//m_cInput.push(m_tRight);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("Right"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}

#undef KEYDOWN
////////////////////////////////////////////////////////////////////////////////
#pragma endregion

#if CONTROLLER
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
		if(m_pController->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
			&& (m_cInput.empty() || m_cInput.back().m_nID != m_tRight.m_nID))
		{
			//	Move Right
			//m_cInput.push(m_tRight);

			CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
			CInputEvent(CIDGen::GetInstance()->GetID(
				"Right"), NULL, 0, m_pController->Gamepad.sThumbLX
				/ 32767); // <- Max Value
			pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
		}
		else if(m_pController->Gamepad.sThumbLX < 
			-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && (m_cInput.empty() || 
			m_cInput.back().m_nID != m_tLeft.m_nID))
		{
			//	Move Left
			//m_cInput.push(m_tLeft);

			CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
			CInputEvent(CIDGen::GetInstance()->GetID(
				"Left"), NULL, 0, m_pController->Gamepad.sThumbLX
				/ 32767); // <- Max Value
			pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
		}
		else if(m_pController->Gamepad.sThumbLY > 
			XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && (m_cInput.empty() || 
			m_cInput.back().m_nID != m_tLeft.m_nID))
		{
			//	Move Up
			//m_cInput.push(m_tUp);
			CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
			CInputEvent(CIDGen::GetInstance()->GetID(
				"Up"), NULL, 0, m_pController->Gamepad.sThumbLY
				/ 32767); // <- Max Value
			pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
		}
		else if(m_pController->Gamepad.sThumbLY < 
			-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && (m_cInput.empty() || 
			m_cInput.back().m_nID != m_tLeft.m_nID))
		{
			//	Move Down
			//m_cInput.push(m_tDown);
			CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
			CInputEvent(CIDGen::GetInstance()->GetID(
				"Down"), NULL, 0, m_pController.Gamepad.sThumbLY
				/ 32767); // <- Max Value
			pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
		}
////////////////////////////////////////////////////////////////////////////////

		if(m_pController->dwPacketNumber != m_dwPrevState)
		{
			if(m_pController->Gamepad.wButtons == m_tAccept.m_nControllerButton)
			{
				//m_cInput.push(m_tAccept);
				CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
				CInputEvent(CIDGen::GetInstance()->GetID(
					"Accept"), NULL, 0, 1.0f);
				pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
			}
			if(m_pController->Gamepad.wButtons == m_tBack.m_nControllerButton)
			{
				//m_cInput.push(m_tBack);
				CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
				CInputEvent(CIDGen::GetInstance()->GetID(
					"Back"), NULL, 0, 1.0f);
				pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
			}
			if(m_pController->Gamepad.wButtons == m_tUp.m_nControllerButton)
			{
				//m_cInput.push(m_tUp);
				CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
				CInputEvent(CIDGen::GetInstance()->GetID(
					"Up"), NULL, 0, 1.0f);
				pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
			}
			if(m_pController->Gamepad.wButtons == m_tDown.m_nControllerButton)
			{
				//m_cInput.push(m_tDown);
				CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
				CInputEvent(CIDGen::GetInstance()->GetID(
					"Down"), NULL, 0, 1.0f);
				pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
			}
			if(m_pController->Gamepad.wButtons == m_tLeft.m_nControllerButton)
			{
				//m_cInput.push(m_tLeft);
				CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
				CInputEvent(CIDGen::GetInstance()->GetID(
					"Left"), NULL, 0, 1.0f);
				pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
			}
			if(m_pController->Gamepad.wButtons == m_tRight.m_nControllerButton)
			{
				//m_cInput.push(m_tRight);
				CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
				CInputEvent(CIDGen::GetInstance()->GetID(
					"Right"), NULL, 0, 1.0f);
				pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
			}
		}
		m_dwPrevState = m_pController->dwPacketNumber;
	}
////////////////////////////////////////////////////////////////////////////////
#pragma endregion
#endif

	memcpy(m_chPrevKeys, m_chKeys, sizeof(m_chPrevKeys));
}

void CInputManager::GetInputGameplay()
{
	CEventManager* pEM = CEventManager::GetInstance();

////////////////////////////////////////////////////////////////////////////////
//	KEYBOARD INPUT
////////////////////////////////////////////////////////////////////////////////
#define KEYDOWN(name, key) (name[key] & 0x80  ? true : false)

	m_pKeyboard->Acquire();

	m_pKeyboard->GetDeviceState(sizeof(m_chKeys), (void*)m_chKeys);

	if(KEYDOWN(m_chKeys, m_tMenu.m_chKeyboardKey) && !KEYDOWN(m_chPrevKeys, 
		m_tMenu.m_chKeyboardKey))
	{
		//m_cInput.push(m_tMenu);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("Menu"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if(KEYDOWN(m_chKeys, m_tShoveLeft.m_chKeyboardKey) && !KEYDOWN(m_chPrevKeys, 
		m_tShoveLeft.m_chKeyboardKey))
	{
		//m_cInput.push(m_tBack);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("ShoveLeft"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if(KEYDOWN(m_chKeys, m_tShoveRight.m_chKeyboardKey) && !KEYDOWN(m_chPrevKeys, 
		m_tShoveRight.m_chKeyboardKey))
	{
		//m_cInput.push(m_tBack);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("ShoveRight"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if((KEYDOWN(m_chKeys, m_tUp.m_chKeyboardKey) || KEYDOWN(m_chKeys, DIK_UP))
		/*&& (m_cInput.empty() || m_cInput.back().m_nID != m_tUp.m_nID)*/)
	{
		//m_cInput.push(m_tUp);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("Accelerate"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if((KEYDOWN(m_chKeys, m_tDown.m_chKeyboardKey) || KEYDOWN(m_chKeys, DIK_DOWN))
		/*&& (m_cInput.empty() || m_cInput.back().m_nID != m_tDown.m_nID)*/)
	{
		//m_cInput.push(m_tDown);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("Decelerate"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if((KEYDOWN(m_chKeys, m_tLeft.m_chKeyboardKey) || KEYDOWN(m_chKeys, DIK_LEFT))
		/*&& (m_cInput.empty() || m_cInput.back().m_nID != m_tLeft.m_nID)*/)
	{
		//m_cInput.push(m_tLeft);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("SteerLeft"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	if((KEYDOWN(m_chKeys, m_tRight.m_chKeyboardKey) || KEYDOWN(m_chKeys, DIK_RIGHT))
		/*&& (m_cInput.empty() || m_cInput.back().m_nID != m_tRight.m_nID)*/)
	{
		//m_cInput.push(m_tRight);
			// - OR - //
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
		CInputEvent(CIDGen::GetInstance()->GetID("SteerRight"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}

	// HACK: To Demonstrate Boosting
	if(KEYDOWN(m_chKeys, DIK_SPACE) && !KEYDOWN(m_chPrevKeys, DIK_SPACE))
	{
		CInputEvent* pInputEvent = MMNEWEVENT(CInputEvent)
			CInputEvent(CIDGen::GetInstance()->GetID("Boost"), NULL, 0, 1.0f);
		pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
	}

#undef KEYDOWN
////////////////////////////////////////////////////////////////////////////////

#if CONTROLLER
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
		if(m_pController->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
			&& (m_cInput.empty() || m_cInput.back().m_nID != m_tRight.m_nID))
		{
			//	Move Right
			m_cInput.push(m_tRight);
		}
		else if(m_pController->Gamepad.sThumbLX < 
			-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && (m_cInput.empty() || 
			m_cInput.back().m_nID != m_tLeft.m_nID))
		{
			//	Move Left
			m_cInput.push(m_tLeft);
		}
		else if(m_pController->Gamepad.sThumbLY > 
			XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && (m_cInput.empty() || 
			m_cInput.back().m_nID != m_tLeft.m_nID))
		{
			//	Move Up
			m_cInput.push(m_tUp);
		}
		else if(m_pController->Gamepad.sThumbLY < 
			-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && (m_cInput.empty() || 
			m_cInput.back().m_nID != m_tLeft.m_nID))
		{
			//	Move Down
			m_cInput.push(m_tDown);
		}
////////////////////////////////////////////////////////////////////////////////

		if(m_pController->dwPacketNumber != m_dwPrevState)
		{
			if(m_pController->Gamepad.wButtons == m_tAccept.m_nControllerButton)
			{
				m_cInput.push(m_tAccept);
			}
			if(m_pController->Gamepad.wButtons == m_tBack.m_nControllerButton)
			{
				m_cInput.push(m_tBack);
			}
			//if(m_pController->Gamepad.wButtons == m_tJump.m_nControllerButton)
			//	m_cInput.push(m_tJump);
			//if(m_pController->Gamepad.wButtons == m_tAction.m_nControllerButton)
			//	m_cInput.push(m_tAction);
			//if(m_pController->Gamepad.wButtons == m_tFlair.m_nControllerButton)
			//	m_cInput.push(m_tFlair);
			if(m_pController->Gamepad.wButtons == m_tUp.m_nControllerButton)
			{
				m_cInput.push(m_tUp);
			}
			if(m_pController->Gamepad.wButtons == m_tDown.m_nControllerButton)
			{
				m_cInput.push(m_tDown);
			}
			if(m_pController->Gamepad.wButtons == m_tLeft.m_nControllerButton)
			{
				m_cInput.push(m_tLeft);
			}
			if(m_pController->Gamepad.wButtons == m_tRight.m_nControllerButton)
			{
				m_cInput.push(m_tRight);
			}
		}
		m_dwPrevState = m_pController->dwPacketNumber;

	}
////////////////////////////////////////////////////////////////////////////////
#endif

	memcpy(m_chPrevKeys, m_chKeys, sizeof(m_chPrevKeys));
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“ClearInput”
//
// Return:		void
//
// Parameters:	void
//
// Purpose:		This function is used to clear the input for all devices and the
//				Input queue.
////////////////////////////////////////////////////////////////////////////////
void CInputManager::ClearInput()
{
	while(!m_cInput.empty())
	{
		m_cInput.pop();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“GetAction”
//
// Return:		int	-	the enumerated value of the action at the front of the
//						input queue
//
// Parameters:	void
//
// Purpose:		This function is used to return the enumerated value of the 
//				action at the front of the input queue and pop that action from
//				the queue.
////////////////////////////////////////////////////////////////////////////////
//int CInputManager::GetAction()
//{
//	int nActionID = 0;
//	if(!m_cInput.empty())
//	{
//		nActionID = m_cInput.front().m_nID;
//		m_cInput.pop();
//	}
//	return nActionID;
//}

////////////////////////////////////////////////////////////////////////////////
// Function:	“PeekAction”
//
// Return:		int	-	the enumerated value of the action at the front of the 
//						input queue
//
// Parameters:	void
//
// Purpose:		This function is used to return the enumerated value of the 
//				action at the front	of the input queue.
////////////////////////////////////////////////////////////////////////////////
//int CInputManager::PeekAction()
//{
//	int nActionID = 0;
//	if(!m_cInput.empty())
//	{
//		nActionID = m_cInput.front().m_nID;
//	}
//	return nActionID;
//}

#if CONTROLLER
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
		bool bKeyInput = true;
		memset(m_chKeys, 0, 256);

		while(uchKeyCode == 0) //bKeyInput)
		{
			m_pKeyboard->Acquire();
			m_pKeyboard->GetDeviceState(sizeof(m_chKeys), (void*)m_chKeys);

			for(int i=0; i<256; i++)
			{
				//if(m_chKeys[i] != 0)
				//{
				//	bKeyInput = false;
				//	break;
				//}

				if(m_chKeys[i] & 0x80)
				{
					uchKeyCode = (unsigned char)i;
					break;
				}
			}
		}

		//for (int i = 0; i < 256; i++)
		//{
		//	if (KEYDOWN(m_chKeys, (unsigned char)i ))
		//	{
		//		uchKeyCode = (unsigned char)i;
		//		break;
		//	}
		//}
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
	//case CMD_JUMP:
	//	m_tJump.m_chKeyboardKey = uchKeyCode;
	//	break;
	//case CMD_ACTION:
	//	m_tAction.m_chKeyboardKey = uchKeyCode;
	//	break;
	//case CMD_FLAIR:
	//	m_tFlair.m_chKeyboardKey = uchKeyCode;
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
	};

	ClearInput();
	return uchKeyCode;
}

int CInputManager::SetControllerCommand(int nCommandID, int nButton)
{
	XINPUT_KEYSTROKE button;
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
			case CMD_ACCEPT:
				{
					m_tAccept.m_nControllerButton = nControllerButton;
				}
				break;
			case CMD_BACK:
				{
					m_tBack.m_nControllerButton = nControllerButton;
				}
				break;
			//case CMD_JUMP:
			//	m_tJump.m_nControllerButton = nControllerButton;
			//	break;
			//case CMD_ACTION:
			//	m_tAction.m_nControllerButton = nControllerButton;
			//	break;
			//case CMD_FLAIR:
			//	m_tFlair.m_nControllerButton = nControllerButton;
				break;
			case CMD_UP:
				{
					m_tUp.m_nControllerButton = nControllerButton;
				}
				break;
			case CMD_DOWN:
				{
					m_tDown.m_nControllerButton = nControllerButton;
				}
				break;
			case CMD_LEFT:
				{
					m_tLeft.m_nControllerButton = nControllerButton;
				}
				break;
			case CMD_RIGHT:
				{
					m_tRight.m_nControllerButton = nControllerButton;
				}
				break;
			case CMD_MENU:
				{
					m_tMenu.m_nControllerButton = nControllerButton;
				}
				break;
			};
		}
	}
	ClearInput();
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
	//case CMD_JUMP:
	//	return m_tJump.m_nControllerButton;
	//case CMD_ACTION:
	//	return m_tAction.m_nControllerButton;
	//case CMD_FLAIR:
	//	return m_tFlair.m_nControllerButton;
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
	//case CMD_JUMP:
	//	return m_tJump.m_chKeyboardKey;
	//case CMD_ACTION:
	//	return m_tAction.m_chKeyboardKey;
	//case CMD_FLAIR:
	//	return m_tFlair.m_chKeyboardKey;
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
	case CMD_RIGHT:
		{
			return m_tRight.m_chKeyboardKey;
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
	m_szKeyStrings[DIK_LCONTROL] = "Left Ctrl";
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
	m_szKeyStrings[DIK_LSHIFT] = "Left Shift";
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
	m_szKeyStrings[DIK_RSHIFT] = "Right Shift";
	m_szKeyStrings[DIK_MULTIPLY] = "*";
	m_szKeyStrings[DIK_LMENU] = "Left Alt";
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
#endif