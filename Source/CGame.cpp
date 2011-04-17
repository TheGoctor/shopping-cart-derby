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
#include "Managers/Global Managers/Event Manager/CUpdateStateEvent.h"
#include "Managers/Global Managers/Event Manager/IEvent.h"
#include "Managers/Global Managers/Event Manager/CIDGen.h"
#include "Managers/Global Managers/Object Manager/CObjectManager.h"
#include "Managers/Component Managers/CMovementManager.h"
#include "Managers/Global Managers/Rendering Managers/Renderer.h"
#include "Managers/Global Managers/Event Manager/CRenderEvent.h"
#include "Managers/Component Managers/CLevelManager.h"
#include "Managers/Component Managers/CAIManager.h"
#include "Managers/Component Managers/CStartOfRaceManager.h"
#include "Managers\Global Managers\Rendering Managers\Direct3DManager.h"
#include "Managers\Global Managers\Rendering Managers\ModelManager.h"
#include "Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"
#include "Managers\Component Managers\CCollisionManager.h"
#include "Managers\\Global Managers\\Rendering Managers\\Texture Managers\\CHUDManager.h"
#include "Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "Managers\Global Managers\State Manager\CStateManager.h"
#include "Components\\HasWon\\CHasWonComponent.h"
#include "Components\\Button\\CButtonComponent.h"


/////////////////////
// REMOVE THIS LATER
#include "Managers/Global Managers/Rendering Managers/Camera.h"
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
	static CGame instance;
	return &instance;
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
	// Init Managers
	CMemoryManager::GetInstance()->Initialize();
	CInputManager::GetInstance()->Initialize(hWnd, hInstance);
	CStateManager::GetInstance()->Init();
	
	m_pRenderer = Renderer::GetInstance();
	m_pRenderer->Init(hWnd, nScreenWidth, nScreenHeight, bIsWindowed);


	CAIManager::GetInstance()->Init();
	m_pSound = CWwiseSoundManager::GetInstance();
	m_pSound->InitSoundManager();
	m_pSound->RegisterObject(OBJECTMUSIC);
	m_pSound->PlayMusic(MUSICLOOP_PLAY);
	
	m_pEM = CEventManager::GetInstance();

	CCollisionManager::GetInstance()->Init();
	

	//// LUA TEST CODE //
	//// calling c++ func from lua
	//	lua_State* pLuaStateC(lua_open());
	//	luaL_openlibs(pLuaStateC);
	//	lua_register(pLuaStateC, "TestLuaFunc", TestLuaFunc);

	//	if(luaL_dofile(pLuaStateC, "Source/Scripts/luatest.lua"))
	//	{
	//		const char* error = lua_tostring(pLuaStateC, -1);
	//		MessageBoxA(0, error, "luatest.lua", MB_OK);
	//		lua_pop(pLuaStateC, 1);
	//	}
	//	lua_getglobal(pLuaStateC, "LuaToC");
	//	lua_pcall(pLuaStateC, 0, 1, 0);
	//	int nPowerLevel = (int)lua_tonumber(pLuaStateC, -1);
	//	lua_pop(pLuaStateC, 1);
	//	lua_close(pLuaStateC);
	//// calling lua func from c++
	//	lua_State* pLuaStateL(lua_open());
	//	luaL_openlibs(pLuaStateL);

	//	if(luaL_dofile(pLuaStateL, "Source/Scripts/luatest.lua"))
	//	{
	//		const char* error = lua_tostring(pLuaStateL, -1);
	//		MessageBoxA(0, error, "luatest.lua", MB_OK);
	//		lua_pop(pLuaStateL, 1);
	//	}
	//	lua_getglobal(pLuaStateL, "CToLua");
	//	lua_pcall(pLuaStateL, 0, 1, 0);
	//	//int nPowerLevel = (int)lua_tonumber(pLuaStateL, 1);
	//	lua_pop(pLuaStateL, 1);
	//	lua_close(pLuaStateL);

		lua_State* pL(lua_open());
		luaL_openlibs(pL);
		lua_register(pL, "CreateObj", 
			&CObjectManager::CreateObject);
		lua_register(pL, "CreateMovementComponent", 
			&CMovementManager::CreateMovementComponent);
		lua_register(pL, "CreateRenderComp",
			&Renderer::CreateRenderComp);
		lua_register(pL, "CreateSpriteComp",
			&CTextureManager::CreateSpriteComp);
		lua_register(pL, "CreateAIComponent",
			&CAIManager::CreateAIComponent);
		lua_register(pL, "CreateStartOfRaceComponent",
			&CStartOfRaceManager::CreateStartOfRaceComponent);
		lua_register(pL, "CreateCollideableComponent",
			&CCollisionManager::CreateCollideableComponent);
		lua_register(pL, "CreateHasWonComponent",
			&CHasWonComponent::CreateHasWonComponent);
		lua_register(pL, "CreateButtonComponent",
			&CButtonComponent::CreateButtonComponent);
		lua_register(pL, "SetNextButtonComponent",
			&CButtonComponent::SetNextButtonComponent);

		if(luaL_dofile(pL, "Source/Scripts/luatest.lua"))
		{
			const char* error = lua_tostring(pL, -1);
			MessageBoxA(0, error, "luatest.lua", MB_OK);
			lua_pop(pL, 1);
		}
		lua_getglobal(pL, "InitValues");
		lua_pcall(pL, 0, 0, 0);
		//lua_getglobal(pL, "CreateStateObjs");
		//lua_pcall(pL, 0, 0, 0);
		lua_close(pL);

		CObjectManager::GetInstance();
		CMovementManager::GetInstance();

	//// END LUA TEST CODE //
		test = CObjectManager::GetInstance()->GetObject();


	// HUD
	CHUDManager::GetInstance()->Init();
	CLevelManager::GetInstance()->Init();
	CStartOfRaceManager::GetInstance(); // create the mgr

	// Screen Properties
	m_nScreenWidth = nScreenWidth;
	m_nScreenHeight = nScreenHeight;
	m_bWindowed = bIsWindowed;

	// Rand
	srand(GetTickCount());

	// Register for Events
	m_pEM->RegisterEvent("ShutdownGame", NULL, ShutdownCallback);

	// Time
	m_dwPrevTime = GetTickCount();
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
	IEvent* pShutdownEvent = MMNEWEVENT(IEvent) IEvent(CIDGen::GetInstance()->
		GetID("Shutdown"), NULL);
	
	m_pEM->ClearEvents();
	m_pEM->PostEvent(pShutdownEvent, PRIORITY_IMMEDIATE);

	// Shutdown Managers
	if(CInputManager::GetInstance())
		CInputManager::GetInstance()->Shutdown();

	m_pSound->GetInstance()->ShutdownManager();
	if(CGame::GetInstance()->m_pEM)
		CGame::GetInstance()->m_pEM->Shutdown();
	m_pEM = NULL;

	m_bShutdown = false;
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
	DWORD dwStartTimeStamp = GetTickCount();
	m_fDT = (float)(dwStartTimeStamp - m_dwPrevTime) / 1000.0f;
	m_dwPrevTime = dwStartTimeStamp;

	m_nFrameCount += 1;
	m_fFrameTime += m_fDT;

	// Cap the dt
	if(m_fDT > 0.1f)
	{
		m_fDT = 0.1f;
	}

	if(m_fFrameTime >= 1)
	{
		m_nFPS = m_nFrameCount;
		m_fFrameTime = 0;
		m_nFrameCount = 0;
	}
	
	// Input
	IEvent* pInputEvent = MMNEWEVENT(IEvent) IEvent(CIDGen::GetInstance()->
		GetID("GetInput"), NULL);

	m_pEM->PostEvent(pInputEvent, PRIORITY_INPUT);

	// Update
	CUpdateStateEvent* pUpdateEvent = MMNEWEVENT(CUpdateStateEvent) 
		CUpdateStateEvent(CIDGen::GetInstance()->GetID("UpdateState"), 
		NULL, m_fDT);

	m_pEM->PostEvent(pUpdateEvent, PRIORITY_UPDATE);

	// Render
	CRenderEvent* pE = MMNEWEVENT(CRenderEvent) CRenderEvent(CIDGen::
			GetInstance()->GetID("AddToSet"), NULL, test);
		CEventManager::GetInstance()->PostEvent(pE, PRIORITY_NORMAL);

	IEvent* pRenderEvent = MMNEWEVENT(IEvent) IEvent(CIDGen::GetInstance()
		->GetID("Render"), NULL);

	m_pEM->PostEvent(pRenderEvent, PRIORITY_RENDER);

	// Process Events
	m_pEM->ProcessEvents();

	// Render?
	//m_pRenderer->RenderScene();

	if(!m_bShutdown)
	{
		Shutdown();
	}
	
	return m_bShutdown;
}

