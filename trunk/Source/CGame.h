/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File:	“CGame.h”
//
// Author: Joseph Leybovich (JL)
// 
// Description: CGame contains the main game loop, and manages state changes as well as the Object
//		Manager. Upon initialization, the CGame object will create instances of the Direct3D 
//		singletons for visuals. It will also instantiate the singletons for the FMOD Sound Manager,
//		Event System, Object Manager and Input Manager objects.
//
// Stipulations: CGame is a singleton.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CGAME_H_
#define _CGAME_H_

#include <windows.h>
#include <mmsystem.h>               // Multimedia header file.
#pragma comment(lib, "winmm.lib")   // Multimedia import library.
#include <vector>
using namespace std;

//	Forward Declarations
class CEventManager;
class Renderer;
class CObject;
class CWwiseSoundManager;
class IEvent;
class IComponent;

class CGame
{
private:

	// Managers
	CEventManager*			m_pEM;				// A pointer to the Event Manager
	Renderer*			    m_pRenderer;
	CWwiseSoundManager*     m_pSound;
	// Screen Props
	int						m_nScreenWidth;		// The width of the screen in pixels
	int						m_nScreenHeight;	// The height of the screen in pixels
	bool					m_bWindowed;		// Whether the game is windowed or fullscreen
	bool					m_bShutdown;

	// For a more accurate timer
	TIMECAPS m_tTC;

	// Time
	float					m_fFrameTime;
	int						m_nFrameCount;
	int						m_nFPS;
	float					m_fDT;				// Time elapsed since last Update
	DWORD					m_dwPrevTime;		// The previous Update's TickCount

	// TODO: Delete when collisions can add to set
	CObject* test;
	CObject* pAnimObj;
	CObject* pAnimObj1;
	
	//	Constructor
	CGame();

	//	Trilogy of Evil
	~CGame();
	CGame(const CGame&);
	CGame& operator=(const CGame&);

public:

	int GetScreenWidth(){return m_nScreenWidth;}
	int GetScreenHeight(){return m_nScreenHeight;}
	bool GetIsWindowed(){return m_bWindowed;}
	void SetScreenWidth(int nWidth){m_nScreenWidth = nWidth;}
	void SetScreenHeight(int nHeight){m_nScreenHeight = nHeight;}
	void SetIsWindowed(bool bIsWindowed){m_bWindowed = bIsWindowed;}
	int GetFPS() { return m_nFPS; }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Function:	“Shutdown”
	//
	// Return:		void
	//
	// Parameters:	void
	//
	// Purpose:		This function is called when shutting the game down. This function shuts down all the singletons
	//				and cleans up any memory that has been allocated.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Shutdown();
	static void ShutdownCallback(IEvent*, IComponent*);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function:	“GetInstance”
//
// Return:		CGame*	-	The instance of the class
//
// Parameters:	void
//
// Purpose:		This function is used to get an instance of a singleton class
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static CGame* GetInstance();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function:	“Initialize”
//
// Return:		void
//
// Parameters:	HWND hWnd			-	A handle to the current window
//				HINSTANCE hInstance	-	A handle to the current instance
//				int nScreenWidth	-	The width of the client rect in pixels
//				int nScreenHeight	-	The height of the client rect in pixels
//				bool bIsWindowed	-	Whether the application is running in windowed mode
//
// Purpose:		This function is used to initialize all the private data members to the passed in values
//				and instantiate singletons. The function will then push CMainMenuState onto m_StateStack.
//				At the end of initialization, the function sets m_dwPrevTime to the current TickCount.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Initialize(HWND hWnd, HINSTANCE hInstance, int nScreenWidth, int nScreenHeight, bool bIsWindowed);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function:	“Main”
//
// Return:		void
//
// Parameters:	void
//
// Purpose:		This function is used as the main game loop. First, it calculates the time elapsed
//				since the last frame. Then Input, Update, and Render are called, consecutively. If
//				Input returns false, Main will return false, ending the game.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Main();
};
#endif