//////////////////////////////////////////////////////////////////////////////////////////////
//	File			:	"WinMain.cpp"
//
//	Author			:	David Brown (DB)
//	Based in part on:
//		-Window code from the book: "Physics for Game Developers" by David M. Bourg.
//		-The previous WinMain.cpp by Jensen Rivera.
//
//	Last Modified	:	3/31/2009
//
//	Purpose			:	To provide a basic window framework for student games.
//
//////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>	//	Needed for Windows Applications.
#include <dbghelp.h>

#include <crtdbg.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
using namespace std;

#include "..\resource.h"

#include "CGame.h"		//	for our game
#include "Managers/Global Managers/Sound Manager/CWwiseSoundManager.h"
#include "Managers/Global Managers/Event Manager/CEventManager.h"
#include "Managers/Global Managers/Event Manager/EventStructs.h"
using namespace EventStructs;
const char* g_szWINDOW_CLASS_NAME	= "SGDWindowClass";			//	Window Class Name.

const char* g_szWINDOW_TITLE		= "Shopping Cart Derby";				//	Window Title.
const int	g_nWINDOW_WIDTH			= 1024;						//	Window Width.
const int	g_nWINDOW_HEIGHT		= 768;						//	Window Height.

//	Windowed or Full screen depending on project setting
#ifdef _DEBUG
	const BOOL	g_bIS_WINDOWED			= TRUE;						
#else
	const BOOL	g_bIS_WINDOWED			= FALSE;
#endif
	
LONG WINAPI errorFunc(_EXCEPTION_POINTERS *pExceptionInfo)
{
	HANDLE hFile = ::CreateFile("dumpfile.mdmp", GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			hFile, MiniDumpNormal, &ExInfo, NULL, NULL );

		MessageBoxA(NULL, "Dump file saved in same directory as .exe file. Please email the file to mac.reichelt@gmail.com with the subject \"Shopping Cart Derby - Crash\"", "Send Error Report", MB_OK);
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//	This is the main message handler of the system.
	PAINTSTRUCT	ps;			//	Used in WM_PAINT.
	HDC			hdc;		//	Handle to a device context.

	//	What is the message 
	switch(msg)
	{
		//	To skip ALT pop up menu (system menu)
		case WM_SYSKEYUP:
		case WM_SYSCHAR:
			return(0);
		break;
		
		//	Handle ALT+F4
		case WM_CLOSE:
		{
			// Sends us a WM_DESTROY
			DestroyWindow(hWnd);			
		}
		break;

		//	and lose/gain focus
		case WM_ACTIVATE:
		{
			//	gaining focus
			if (LOWORD(wParam) != WA_INACTIVE)
			{
				// unpause game code here
			}
			else // losing focus
			{
				// pause game code here
				// Sender cannot be NULL
				SendIEvent("FocusLost", (IComponent*)1, NULL, PRIORITY_NORMAL);
			}
		}
		break;

		case WM_CREATE: 
		{
			//	Do initialization here
			return(0);
		}
		break;

		case WM_PAINT:
		{
			//	Start painting
			hdc = BeginPaint(hWnd,&ps);

			//	End painting
			EndPaint(hWnd,&ps);
			return(0);
		}
		break;

		case WM_DESTROY: 
		{
			//	Kill the application			
			PostQuitMessage(0);
			return(0);
		}
		break;

		default:
		break;
	}

	//	Process any messages that we didn't take care of 
	return (DefWindowProc(hWnd, msg, wParam, lParam));
}

//	Checks to see if the game was already running in another window.
//
//	NOTE:	Don't call this function if your game needs to have more
//			than one instance running on the same computer (i.e. client/server)
BOOL CheckIfAlreadyRunning(void)
{
	//	Find a window of the same window class name and window title
	HWND hWnd = FindWindow(g_szWINDOW_CLASS_NAME, g_szWINDOW_TITLE);

	//	If one was found
	if (hWnd)
	{
		//	If it was minimized
		if (IsIconic(hWnd))
			//	restore it
			ShowWindow(hWnd, SW_RESTORE);

		//	Bring it to the front
		SetForegroundWindow(hWnd);

		return TRUE;
	}

	//	No other copies found running
	return FALSE;
}


BOOL RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX	winClassEx;	//	This will describe the window class we will create.

	//	First fill in the window class structure
	winClassEx.cbSize			= sizeof(winClassEx);
	winClassEx.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClassEx.lpfnWndProc		= WindowProc;
	winClassEx.cbClsExtra		= 0;
	winClassEx.cbWndExtra		= 0;
	winClassEx.hInstance		= hInstance;
	winClassEx.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	winClassEx.hIconSm			= NULL;
	winClassEx.hCursor			= LoadCursor(hInstance, MAKEINTRESOURCE(IDC_POINTER));
	winClassEx.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClassEx.lpszMenuName		= NULL; 
	winClassEx.lpszClassName	= g_szWINDOW_CLASS_NAME;

	//	Register the window class
	return RegisterClassEx(&winClassEx);
}

//	Creates and sizes the window appropriately depending on if 
//	the application is windowed or full screen.
HWND MakeWindow(HINSTANCE hInstance)
{
	// Setup window style flags
	DWORD dwWindowStyleFlags = WS_VISIBLE;

	if (g_bIS_WINDOWED)
	{
		dwWindowStyleFlags |= WS_OVERLAPPEDWINDOW;
	}
	else
	{
		dwWindowStyleFlags |= WS_POPUP;
		ShowCursor(FALSE);	// Stop showing the mouse cursor
	}

	// Setup the desired client area size
	RECT rWindow;
	rWindow.left	= 0;
	rWindow.top		= 0;
	rWindow.right	= g_nWINDOW_WIDTH;
	rWindow.bottom	= g_nWINDOW_HEIGHT;

	// Get the dimensions of a window that will have a client rect that
	// will really be the resolution we're looking for.
	AdjustWindowRectEx(&rWindow, 
						dwWindowStyleFlags,
						FALSE, 
						WS_EX_APPWINDOW);
	
	// Calculate the width/height of that window's dimensions
	int nWindowWidth	= rWindow.right - rWindow.left;
	int nWindowHeight	= rWindow.bottom - rWindow.top;

	//	Create the window
	return CreateWindowEx(WS_EX_APPWINDOW,											//	Extended Style flags.
						  g_szWINDOW_CLASS_NAME,									//	Window Class Name.
						  g_szWINDOW_TITLE,											//	Title of the Window.
						  dwWindowStyleFlags,										//	Window Style Flags.
						  (GetSystemMetrics(SM_CXSCREEN)/2) - (nWindowWidth/2),		//	Window Start Point (x, y). 
						  (GetSystemMetrics(SM_CYSCREEN)/2) - (nWindowHeight/2),	//		-Does the math to center the window over the desktop.
						  nWindowWidth,												//	Width of Window.
						  nWindowHeight,											//	Height of Window.
						  NULL,														//	Handle to parent window.
						  NULL,														//	Handle to menu.
						  hInstance,												//	Application Instance.
						  NULL);													//	Creation parameters.
}

//////////////////////////
//		WinMain			//
//////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	MSG		msg;	//	Generic message.
	HWND	hWnd;	//	Main Window Handle.

	// Don't let more than one instance of the application exist
	//
	// NOTE:	Comment out the following section of code if your game needs to have more
	//			than one instance running on the same computer (i.e. client/server)
	////////////////////////////////////////////////////////////////////////
	if (!hPrevInstance)
	{
		if (CheckIfAlreadyRunning())
			return FALSE;
	}
	////////////////////////////////////////////////////////////////////////

	SetUnhandledExceptionFilter(errorFunc);

#ifdef _DEBUG

	short bufferWidth = 80;
	short bufferHeight = 300;
	short windowWidth = 80;
	short windowHeight = 24;

	// Set up the console window.
	CONSOLE_SCREEN_BUFFER_INFO	coninfo;
	FILE						*pFile;
	int							conHandle;
	HANDLE						stdHandle;
	SMALL_RECT					window = {0,};

	// Allocate console
	AllocConsole();

	// reset console properties
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y	= bufferHeight;
	coninfo.dwSize.X	= bufferWidth;
	window.Left			= 0;
	window.Top			= 0;
	window.Right		= windowWidth - 1;
	window.Bottom		= windowHeight - 1;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), true, &window);

	// Redirect standard output to the console window.
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	pFile = _fdopen(conHandle, "w" );
	*stdout = *pFile;
	setvbuf(stdout, NULL, _IONBF, 0);	// unbuffered

	// Redirect standard input to console
	stdHandle = GetStdHandle(STD_INPUT_HANDLE);
	conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	pFile = _fdopen(conHandle, "r");
	*stdin = *pFile;
	setvbuf(stdin, NULL, _IONBF, 0);	// unbuffered

	// Redirect standard error to the console window.
	stdHandle = GetStdHandle(STD_ERROR_HANDLE);
	conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	pFile = _fdopen(conHandle, "w");
	*stderr = *pFile;
	setvbuf(stderr, NULL, _IONBF, 0);	// unbuffered

	// Allow C++ code to benefit from console redirection.
	// route cout, wcout, cin, wcin, wcerr, cerr, wclog & clog as well
	ios::sync_with_stdio();
#endif

	//	Register the window class
	if (!RegisterWindowClass(hInstance))
		return 0;

	//	Create the window
	hWnd = MakeWindow(hInstance);

	if (!hWnd)
		return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//////////////////////////////////////////
	//	Initialize Game here
	//////////////////////////////////////////
	
//_CrtSetBreakAlloc(198);
	
	CGame*	pGame = CGame::GetInstance();
	pGame->Initialize(hWnd, hInstance, g_nWINDOW_WIDTH, g_nWINDOW_HEIGHT, g_bIS_WINDOWED);

	//////////////////////////////////////////

	//	Enter main event loop
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ 
			//	Test if this is a quit
			if (msg.message == WM_QUIT)
				break;
		
			//	Translate any accelerator keys
			TranslateMessage(&msg);

			//	Send the message to the window proc
			DispatchMessage(&msg);
		}
		
		//////////////////////////////////
		//	Put Game Logic Here
		//////////////////////////////////
		if (pGame->Main() == false)
		{
			PostQuitMessage(0);
			break;
		}
		
		//////////////////////////////////
	}

	/////////////////////////////////////////
	//	Shutdown Game Here
	/////////////////////////////////////////
	
	pGame->Shutdown();

	/////////////////////////////////////////
	
	
	//	Unregister the window class
	UnregisterClass(g_szWINDOW_CLASS_NAME, hInstance);

	//	Return to Windows like this.
	return (int)(msg.wParam);
}