////////////////////////////////////////////////////////////////////////////////
// File:	“CGame.cpp”
//
// Author: Joseph Leybovich (JL)
// 
// Description: CGame contains the main game loop, and manages state changes as
//		well as the Object Manager. Upon initialization, the CGame object will 
//		create instances of the Direct3D singletons for visuals. It will also 
//		instantiate the singletons for the FMOD Sound Manager, Event System, 
//		Object Manager and Input Manager objects.
//
// Stipulations: CGame is a singleton.
//
////////////////////////////////////////////////////////////////////////////////

// Include the Windows Multimedia Library:
#include <iostream>
#include <fstream>
using namespace std;

// LUA TEST CODE //
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
// END LUA TEST CODE //


#include "CGame.h"
#include "Managers/Global Managers/Memory Manager/CMemoryManager.h"
#include "Managers/Global Managers/Input Manager/CInputManager.h"
#include "Managers/Global Managers/Event Manager/CEventManager.h"
#include "Managers/Global Managers/Event Manager/EventStructs.h"
#include "Managers/Global Managers/Event Manager/IEvent.h"
#include "Managers/Global Managers/Event Manager/CIDGen.h"
#include "Managers/Global Managers/Object Manager/CObjectManager.h"
#include "Managers/Component Managers/CInventoryManager.h"
#include "Managers/Global Managers/Rendering Managers/Renderer.h"
#include "Managers/Component Managers/CLevelManager.h"
#include "Managers/Component Managers/CSpawningManager.h"
#include "Managers/Component Managers/CAIManager.h"
#include "Managers/Component Managers/CStartOfRaceManager.h"
#include "Managers/Component Managers/CPickupItemManager.h"
#include "Managers/Component Managers/CHeldItemManager.h"
#include "Managers\Global Managers\Rendering Managers\Direct3DManager.h"
#include "Managers\Global Managers\Rendering Managers\ModelManager.h"
#include "Managers\Global Managers\Keybind Manager\CKeybindsManager.h"
#include "Managers\Component Managers\CCollisionManager.h"
#include "Managers\Global Managers\Rendering Managers\Texture Managers\CHUDManager.h"
#include "Managers\Global Managers\Rendering Managers\Texture Managers\CIntroManager.h"
#include "Managers\Global Managers\Rendering Managers\Texture Managers\CCreditManager.h"
#include "Managers\Global Managers\Endgame Manager\CEndgameManager.h"
#include "Managers\Global Managers\Unlockable Manager\CUnlockableManager.h"

//#include "Managers\\Global Managers\\\Rendering Managers\\Texture Managers\\CCharacterSelectManager.h";
#include "Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "Managers\Global Managers\State Manager\CStateManager.h"
#include "Managers\Global Managers\Console Manager\CConsoleManager.h"
#include "Managers\Component Managers\CAnimateManager.h"

// Test Anim
#include "Components\\Rendering\\CAnimateComponent.h"
#include "Managers\Global Managers\Rendering Managers\DXRenderContextManager.h"
#include "Managers\Global Managers\Rendering Managers\CEffectManager.h"

// Not needed any more (I don't think)
//#include "Managers/Component Managers/CMovementManager.h"
//#include "Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"
//#include "Components\\Button\\CButtonComponent.h"
//#include "Components\\Slider\\CSliderComponent.h"
//#include "Components\\Scroller\\CScrollerComponent.h"
//#include "Managers/Component Managers/CAnimateManager.h"

/////////////////////
// REMOVE THIS LATER
#include "Managers/Global Managers/Camera Manager/Camera.h"
////////////////////

extern const AkGameObjectID OBJECTMUSIC = 0;
// Constructor
CGame::CGame() : m_fDT(0.0f), m_fFrameTime(0.0f),
				 m_nFPS(0), m_nFrameCount(0),
				 m_pEM(NULL), m_bShutdown(true)
{
}

//	Destructor
CGame::~CGame()
{
	
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“GetInstance”
//
// Return:		CGame*	-	The instance of the class
//
// Parameters:	void
//
// Purpose:		This function is used to get an instance of a singleton class
////////////////////////////////////////////////////////////////////////////////
CGame* CGame::GetInstance()
{
	static CGame cGame;
	return &cGame;
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“Initialize”
//
// Return:		void
//
// Parameters:	HWND hWnd			-	A handle to the current window
//				HINSTANCE hInstance	-	A handle to the current instance
//				int nScreenWidth	-	The width of the client rect in pixels
//				int nScreenHeight	-	The height of the client rect in pixels
//				bool bIsWindowed	-	Whether the application is running in
//										windowed mode
//
// Purpose:		This function is used to initialize all the private data members
//				to the passed in values	and instantiate singletons. The function
//				will then push CMainMenuState onto m_StateStack. At the end of 
//				initialization, the function sets m_dwPrevTime to the current 
//				TickCount.
////////////////////////////////////////////////////////////////////////////////

void CGame::Initialize(HWND hWnd, HINSTANCE hInstance, int nScreenWidth, 
					   int nScreenHeight, bool bIsWindowed)
{
	
	// Increase the accuracy/precision of the Windows timer:
	timeGetDevCaps(&m_tTC, sizeof(TIMECAPS));
	timeBeginPeriod(m_tTC.wPeriodMin);

	// Rand
	srand(timeGetTime()); //GetTickCount());

	// Init Managers
	CMemoryManager::GetInstance()->Initialize(50 MB, 5 MB, 20 MB, 75 MB);
	CInputManager::GetInstance()->Initialize(hWnd, hInstance);
	CStateManager::GetInstance()->Init();
	
	m_pRenderer = Renderer::GetInstance();
	m_pRenderer->Init(hWnd, nScreenWidth, nScreenHeight, bIsWindowed);
	m_pRenderer->LoadModels();

	m_pSound = CWwiseSoundManager::GetInstance();
	m_pSound->InitSoundManager();
	m_pSound->RegisterObject(GLOBAL_ID);
	m_pSound->RegisterObject(BIKER_ID);
	m_pSound->RegisterObject(LARPER_ID);
	m_pSound->RegisterObject(SASHA_ID);
	m_pSound->RegisterObject(BANDITOS_ID);
	m_pSound->RegisterObject(SCIENTIST_ID);
	m_pSound->RegisterObject(CRYGAME_ID);
	m_pSound->RegisterObject(STORYTIME_ID);
	m_pSound->RegisterObject(ITEM_ID);
	m_pEM = CEventManager::GetInstance();

	CCollisionManager::GetInstance()->Init();
	ModelManager::GetInstance()->Init();

	CAIManager::GetInstance()->Init();
	// Place this where you want to initialize Lua
	CConsoleManager::GetInstance().Initialize();

	CObjectManager::GetInstance();
	CMovementManager::GetInstance();

	// Inventory
	CInventoryManager::GetInstance()->Init();

	// HUD
	CIntroManager::GetInstance()->Init();
	CCreditManager::GetInstance()->Init();
	//CCharacterSelectManager::GetInstance()->Init();
	CHUDManager::GetInstance()->Init();
	CLevelManager::GetInstance()->Init();

	 //level manager just made a whoooole bunch of boxes, I'm gonna fix this, HAS TO BE AFTER LEVEL MANAGER INIT!!!!!!!!! -Raymoney
	CCollisionManager::GetInstance()->CombineLinedBoxes();	

	CStartOfRaceManager::GetInstance(); // create the mgr
	CSpawningManager::GetInstance()->Init();
	CPickupItemManager::GetInstance()->Init();
	CHeldItemManager::GetInstance()->Init();

	CAnimateManager::GetInstance()->Init();

	CUnlockableManager::GetInstance()->Init();
	CEndgameManager::GetInstance()->Init();

	// set the screen parameters
	SetScreenWidth(nScreenWidth);
	SetScreenHeight(nScreenHeight);
	SetIsWindowed(bIsWindowed);

	// Initialize Effect Manager
	CEffectManager::GetInstance()->Init();

	// init keybinds manager
	CKeybindsManager::GetInstance()->Init();
	
	// Register for Events
	m_pEM->RegisterEvent("ShutdownGame", (IComponent*)GetInstance(), ShutdownCallback);

	// Time
	m_dwPrevTime = timeGetTime();	//GetTickCount();
}
	
////////////////////////////////////////////////////////////////////////////////
// Function:	“Shutdown”
//
// Return:		void
//
// Parameters:	void
//
// Purpose:		This function is called when shutting the game down. This 
//				function shuts down all the singletons and cleans up any memory
//				that has been allocated.
////////////////////////////////////////////////////////////////////////////////
void CGame::Shutdown()
{
	// Unregister Events
	
	m_pEM->ClearEvents();
	SendIEvent("Shutdown", (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);

	// Shutdown Managers
	if(CInputManager::GetInstance())
		CInputManager::GetInstance()->Shutdown();

	m_pSound->GetInstance()->ShutdownManager();
	if(CGame::GetInstance()->m_pEM)
		CGame::GetInstance()->m_pEM->Shutdown();
	m_pEM = NULL;

	m_bShutdown = false;
	
	// Restore the accuracy/precision of the Windows timer:
	timeEndPeriod(m_tTC.wPeriodMin);
}

void CGame::ShutdownCallback(IEvent*, IComponent*)
{
	GetInstance()->m_bShutdown = false;
}

////////////////////////////////////////////////////////////////////////////////
// Function:	“Main”
//
// Return:		void
//
// Parameters:	void
//
// Purpose:		This function is used as the main game loop. First, it 
//				calculates the time elapsed since the last frame. Then Input, 
//				Update, and Render are called, consecutively. If Input returns
//				false, Main will return false, ending the game.
////////////////////////////////////////////////////////////////////////////////
bool CGame::Main()
{
	// Time
	m_pSound->Update();
	DWORD dwStartTimeStamp = timeGetTime(); //GetTickCount();
	m_fDT = (float)(dwStartTimeStamp - m_dwPrevTime) / 1000.0f;
	m_dwPrevTime = dwStartTimeStamp;

	m_nFrameCount += 1;
	m_fFrameTime += m_fDT;

	// Cap the dt
	if(m_fDT > .2f)
	{
		m_fDT = .2f;
	}

	if(m_fFrameTime >= 1)
	{
		m_nFPS = m_nFrameCount;
		m_fFrameTime = 0;
		m_nFrameCount = 0;
		cout << "FPS: " << m_nFPS << endl;
	}
	
	// Input
	SendIEvent("GetInput", (IComponent*)GetInstance(), NULL, PRIORITY_INPUT);

	// Check for Lost Device
	Direct3DManager::GetInstance()->CheckForLostDevice();

	// Update
	SendUpdateEvent("UpdateState", (IComponent*)GetInstance(), m_fDT);

	// Collision Test
	SendIEvent("Collision", (IComponent*)GetInstance(), NULL, PRIORITY_COLLISION);

	// Render
	SendIEvent("Render", (IComponent*)GetInstance(), NULL, PRIORITY_RENDER);

	// Process Events
	m_pEM->ProcessEvents();
	
	return m_bShutdown;
}