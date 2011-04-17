#include "Direct3DManager.h"
#include "ModelManager.h"

#include "Texture Managers\\CTextureManager.h"

// Initialize the Static Object.
Direct3DManager Direct3DManager::m_Instance;


ID3DXEffect	*triangleEffect = NULL;

D3DXMATRIX projMatrix;
D3DXMATRIX viewMatrix;
D3DXMATRIX triangleMatrix;
D3DXMATRIX rotation;

Direct3DManager::Direct3DManager(void)
	: m_lpD3D(NULL), m_lpDev(NULL),
	  m_lpSprite(NULL), m_lpFont(NULL), m_lpLine(NULL),
	  m_hWnd(NULL),
	  m_pVertPosDecl(NULL), m_pVertPosNormTex2DDecl(NULL) 
{
	memset(&m_PresentParams, 0, sizeof(D3DPRESENT_PARAMETERS));
}

Direct3DManager::~Direct3DManager(void)
{
}

// Returns the Single Instance of this Class
Direct3DManager* Direct3DManager::GetInstance(void)
{
	return &m_Instance;
	//static Direct3DManager m_instance;
	//return &m_instance;
}

// Initalize Direct3D
bool Direct3DManager::InitDirect3D(HWND hWnd,
	int nScreenWidth, int nScreenHeight, bool bIsWindowed, bool bVsync)
{
	// Make sure the hWnd is valid.
	if (NULL == hWnd) return false;

	// Set the handle to the window.
	m_hWnd = hWnd;

	// Create the Direct3D9 Object.
	m_lpD3D = Direct3DCreate9(D3D_SDK_VERSION);

	// Make sure the object is valid.
	//if (NULL == m_lpD3D) DXERROR( (LPCWSTR)"Failed to Create D3D Object");
	
	// Setup the parameters for using Direct3D.
	//m_PresentParams.BackBufferWidth				= nScreenWidth;
	//m_PresentParams.BackBufferHeight			= nScreenHeight;
	//m_PresentParams.BackBufferFormat			= (bIsWindowed) ? D3DFMT_UNKNOWN : D3DFMT_R5G6B5;
	//m_PresentParams.BackBufferCount				= 1;
	//m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	//m_PresentParams.MultiSampleQuality			= 0;
	m_PresentParams.SwapEffect					= D3DSWAPEFFECT_FLIP;
	m_PresentParams.hDeviceWindow				= hWnd;
	m_PresentParams.Windowed					= bIsWindowed;
	m_PresentParams.EnableAutoDepthStencil		= true;
	m_PresentParams.AutoDepthStencilFormat		= D3DFMT_D16;
	//m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_2_SAMPLES;
	//m_PresentParams.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	//m_PresentParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	//m_PresentParams.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;//(bVsync) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the Direct3D Device.
	m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_PresentParams, &m_lpDev);

	m_lpDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// create font
	D3DXCreateFont(m_lpDev, 32, 0, FW_NORMAL, NULL, false, 
	  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
	  ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
	  "Arial", &m_lpFont);

	// Create Sprite Object.
	D3DXCreateSprite(m_lpDev, &m_lpSprite);

	// Initalize Vertex Declarations
	InitVertDecls();

	//D3DXMatrixIdentity(&viewMatrix);
	//D3DXMatrixIdentity(&projMatrix);

	/////////////////////////////////////////
	//// Set Projection
	//D3DXMatrixPerspectiveFovLH(&projMatrix, D3DX_PI/4,
	//	(float)nScreenWidth/nScreenHeight, 0.1f, 1000.0f);

	/////////////////////////////////////////
	//// Set View
	//D3DXVECTOR3	eye(0,0,-20.0f);
	//D3DXVECTOR3	lookat(0,0,0);
	//D3DXVECTOR3	up(0,1,0);
	//D3DXMatrixLookAtLH(&viewMatrix, &eye, &lookat, &up);

	// Texture Manager
	CTextureManager::GetInstance()->InitTextureManager(m_lpDev, m_lpSprite);

	CEventManager::GetInstance()->RegisterEvent("Shutdown", NULL, Shutdown);

	//	Return success.
	return true;
}

// Called from initalize to create the vertex declarations
void Direct3DManager::InitVertDecls(void)
{
	// create vertex (position) declaration
	D3DVERTEXELEMENT9 posDecl[] =
	{
		{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
		{0,12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},

		D3DDECL_END()
	};
	m_lpDev->CreateVertexDeclaration(posDecl, &m_pVertPosDecl);

	// create vertex (pos, norm, tex2D) declaration
	D3DVERTEXELEMENT9 declPosNormTex2[] =	
	{
		{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
		{0,12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0,24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},

		D3DDECL_END()
	};
	m_lpDev->CreateVertexDeclaration(declPosNormTex2, &m_pVertPosNormTex2DDecl);

	D3DVERTEXELEMENT9 decl3[] =	//3D
	{
		{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
		{0,12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},

		D3DDECL_END()
	};
	m_lpDev->CreateVertexDeclaration(decl3, &m_pVertPosTex3DDecl);
}

void Direct3DManager::Shutdown(IEvent*, IComponent*)
{
	Direct3DManager::GetInstance()->ShutdownDirect3D();
}


// Shutdown Direct3D
void Direct3DManager::ShutdownDirect3D(void)
{
	SAFE_RELEASE(m_pVertPosTex3DDecl);
	SAFE_RELEASE(m_pVertPosNormTex2DDecl);
	SAFE_RELEASE(m_pVertPosDecl);
	SAFE_RELEASE(m_lpFont);
	SAFE_RELEASE(m_lpLine);
	SAFE_RELEASE(m_lpSprite);
	SAFE_RELEASE(m_lpDev);
	SAFE_RELEASE(m_lpD3D);
}

// Clear Screen
void Direct3DManager::Clear(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
	m_lpDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(ucRed, ucGreen, ucBlue), 1.0f, 0);

	// Check for lost device (could happen from an ALT+TAB or ALT+ENTER).
	if (m_lpDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		m_lpLine->OnLostDevice();
		m_lpSprite->OnLostDevice();
		m_lpDev->Reset(&m_PresentParams);
		m_lpSprite->OnResetDevice();
		m_lpLine->OnResetDevice();
	}
}

// Present to Screen
void Direct3DManager::Present(void)
{
	m_lpDev->Present(NULL, NULL, NULL, NULL);
}

// Resize
void Direct3DManager::ChangeDisplayParam(int nWidth, int nHeight, bool bWindowed)
{
	// Set the new Presentation Parameters.
	m_PresentParams.BackBufferFormat	= (bWindowed) ? D3DFMT_UNKNOWN : D3DFMT_R5G6B5;
	m_PresentParams.Windowed			= bWindowed;
	m_PresentParams.BackBufferWidth	= nWidth;
	m_PresentParams.BackBufferHeight	= nHeight;

	// Reset the device.
	m_lpLine->OnLostDevice();
	m_lpSprite->OnLostDevice();
	m_lpDev->Reset(&m_PresentParams);
	m_lpSprite->OnResetDevice();
	m_lpLine->OnResetDevice();

	// Setup window style flags
	DWORD dwWindowStyleFlags = WS_VISIBLE;

	if (bWindowed)
	{
		dwWindowStyleFlags |= WS_OVERLAPPEDWINDOW;
		ShowCursor(TRUE); // show the mouse cursor
	}
	else
	{
		dwWindowStyleFlags |= WS_POPUP;
		ShowCursor(FALSE); // hide the mouse cursor
	}

	SetWindowLong(m_hWnd, GWL_STYLE, dwWindowStyleFlags);

	//	Set the window to the middle of the screen.
	if (bWindowed)
	{
		// Setup the desired client area size
		RECT rWindow;
		rWindow.left	= 0;
		rWindow.top		= 0;
		rWindow.right	= nWidth;
		rWindow.bottom	= nHeight;

		// Get the dimensions of a window that will have a client rect that
		// will really be the resolution we're looking for.
		AdjustWindowRectEx(&rWindow, 
							dwWindowStyleFlags,
							FALSE, 
							WS_EX_APPWINDOW);
		
		// Calculate the width/height of that window's dimensions
		int windowWidth		= rWindow.right - rWindow.left;
		int windowHeight	= rWindow.bottom - rWindow.top;

		SetWindowPos(m_hWnd, HWND_TOP,	(GetSystemMetrics(SM_CXSCREEN)>>1) - (windowWidth>>1),
										(GetSystemMetrics(SM_CYSCREEN)>>1) - (windowHeight>>1),
										windowWidth, windowHeight, SWP_SHOWWINDOW);
	}
	else
	{

		// Let windows know the window has changed.
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

	}
}

// Device Begin/End
bool Direct3DManager::DeviceBegin(void)
{
	m_lpDev->BeginScene();

	return true;
}
bool Direct3DManager::DeviceEnd(void)
{
	m_lpDev->EndScene();

	return true;
}

// Sprite Begin/End
bool Direct3DManager::SpriteBegin(void)
{
	m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND);

	return true;
}
bool Direct3DManager::SpriteEnd(void)
{
	m_lpSprite->End();

	return true;
}