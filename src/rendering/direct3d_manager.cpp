////////////////////////////////////////////////////////////////////////////////
//	File			:	Direct3DManager.cpp
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Initializing and Shuting down of Direct
//						3D Objects
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "Direct3DManager.h"
#include "ModelManager.h"
#include "Texture Managers\\CTextureManager.h"

// Default Constructor
Direct3DManager::Direct3DManager(void) : m_lpD3D(NULL), m_lpDev(NULL),
										 m_lpSprite(NULL), m_lpFont(NULL),
										 m_lpLine(NULL), m_hWnd(NULL),
										 m_pVertPosDecl(NULL),
										 m_pVertPosNormTex2DDecl(NULL),
										 m_pVertPosColTex2DDecl(NULL)
										 //m_pVertPosTex3DDecl(NULL)
{
	// Clear out Present Parameters
	memset(&m_PresentParams, 0, sizeof(D3DPRESENT_PARAMETERS));
}

// Destructor
Direct3DManager::~Direct3DManager(void)
{
}

// Returns the Single Instance of this Class
Direct3DManager* Direct3DManager::GetInstance(void)
{
	static Direct3DManager m_cD3DManager;
	return &m_cD3DManager;
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
	m_PresentParams.BackBufferWidth				= nScreenWidth;
	m_PresentParams.BackBufferHeight			= nScreenHeight;
	m_PresentParams.BackBufferFormat			= (bIsWindowed) ? D3DFMT_UNKNOWN : D3DFMT_R5G6B5;
	m_PresentParams.BackBufferCount				= 1;
	m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParams.MultiSampleQuality			= 0;
	m_PresentParams.SwapEffect					= D3DSWAPEFFECT_FLIP;
	m_PresentParams.hDeviceWindow				= hWnd;
	m_PresentParams.Windowed					= bIsWindowed;
	m_PresentParams.EnableAutoDepthStencil		= true;
	m_PresentParams.AutoDepthStencilFormat		= D3DFMT_D16;
	//m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_2_SAMPLES;
	//m_PresentParams.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_PresentParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	m_PresentParams.PresentationInterval		= (bVsync) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the Direct3D Device.
	m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_PresentParams, &m_lpDev);

	// Set RenderState
	m_lpDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Create Font
	D3DXCreateFont(m_lpDev, 16, 0, FW_NORMAL, NULL, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"Consolas", &m_lpFont);
	
	// Create Line
	D3DXCreateLine(m_lpDev, &m_lpLine);
	m_lpLine->SetWidth(5.0f);

	// Create Sprite Object.
	D3DXCreateSprite(m_lpDev, &m_lpSprite);
	D3DXCreateLine(m_lpDev, &m_lpLine);

	// Initalize Vertex Declarations
	InitVertDecls();

	// Texture Manager
	CTextureManager::GetInstance()->InitTextureManager(m_lpDev, m_lpSprite);

	CEventManager::GetInstance()->RegisterEvent("Shutdown", (IComponent*)GetInstance(), Shutdown);
	CEventManager::GetInstance()->RegisterEvent("ChangeGamma", (IComponent*)GetInstance(), ChangeGamma);

	//	Return success.
	return true;
}

// Called from Initalize to Create the Vertex Declarations
void Direct3DManager::InitVertDecls(void)
{
	// Create Vertex (Position) Declaration
	D3DVERTEXELEMENT9 posDecl[] =
	{
		{0, 0,  D3DDECLTYPE_FLOAT3,	  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,	 0},
		D3DDECL_END()
	};
	m_lpDev->CreateVertexDeclaration(posDecl, &m_pVertPosDecl);

	// Create Vertex (Pos, Norm, Tex2D) Declaration
	D3DVERTEXELEMENT9 declPosNormTex2[] =	
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	m_lpDev->CreateVertexDeclaration(declPosNormTex2, &m_pVertPosNormTex2DDecl);

	// Create Vertex (Pos, Norm, Tex2D) Declaration
	D3DVERTEXELEMENT9 declPosColTex2[] =	
	{
		{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0},
		{0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	m_lpDev->CreateVertexDeclaration(declPosColTex2, &m_pVertPosColTex2DDecl);

	// Create Vertex (Pos, Tex3D) Declaration
	//D3DVERTEXELEMENT9 decl3[] =
	//{
	//	{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	//	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	//	D3DDECL_END()
	//};
	//m_lpDev->CreateVertexDeclaration(decl3, &m_pVertPosTex3DDecl);
}

// Shutdown Shutdown
void Direct3DManager::Shutdown(IEvent*, IComponent*)
{
	Direct3DManager::GetInstance()->ShutdownDirect3D();
}
void Direct3DManager::ShutdownDirect3D(void)
{
	//SAFE_RELEASE(m_pVertPosTex3DDecl);
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
	// Clear the Screen
	m_lpDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(ucRed, ucGreen, ucBlue), 1.0f, 0);

	// Make Sure we can Draw
	CheckForLostDevice();
}

// Check for Lost Device
bool Direct3DManager::CheckForLostDevice(void)
{
	// Check for lost device (could happen from an ALT+TAB or ALT+ENTER).
	if (m_lpDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		// Call appropriate Lost and Reset Device Functions
		m_lpLine->OnLostDevice();
		m_lpSprite->OnLostDevice();
		m_lpFont->OnLostDevice();
		m_lpDev->Reset(&m_PresentParams);
		m_lpFont->OnResetDevice();
		m_lpSprite->OnResetDevice();
		m_lpLine->OnResetDevice();

		// Send Lost Device
		SendIEvent("LostDevice", (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);

		// Return True (Unsafe)
		return true;
	}
	else
	{
		// Return False (Safe)
		return false;
	}
}

// Present to Screen
void Direct3DManager::Present(void)
{
	// Present to the Sceen
	m_lpDev->Present(NULL, NULL, NULL, NULL);
}

// Resize
void Direct3DManager::ChangeDisplayParam(int nWidth, int nHeight, bool bWindowed)
{
	// Set the new Presentation Parameters.
	m_PresentParams.BackBufferFormat	= (bWindowed) ? D3DFMT_UNKNOWN : D3DFMT_R5G6B5;
	m_PresentParams.Windowed			= bWindowed;
	m_PresentParams.BackBufferWidth		= nWidth;
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
		int windowWidth		= rWindow.right  - rWindow.left;
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

// Gamma
void Direct3DManager::SetGamma(LPDIRECT3DDEVICE9 _lpD3D, float gamma)
{
	D3DGAMMARAMP ramp; 
	_lpD3D->GetGammaRamp(0, &ramp);

	//m_fGamma = gamma; // got rid of by smith since this value is used for the sliders
						///and nowhere else and storing it here gives a bad value
	for (unsigned int i = 0; i < 256; ++i)
	{
		m_usRed[i] = ramp.red[i];
		m_usGreen[i] = ramp.green[i];
		m_usBlue[i] = ramp.blue[i];
	}
	
	for (unsigned int i = 0; i < 256; ++i)
	{
		unsigned short val = (unsigned short)(min(255, i * (gamma + 1.0f))) << 8;
		m_usRed[i] = m_usBlue[i] = m_usGreen[i] = val;
	}

	for (unsigned int i = 0; i < 256; ++i)
	{
		ramp.red[i] = m_usRed[i];
		ramp.blue[i] = m_usBlue[i];
		ramp.green[i] = m_usGreen[i];

	}
	
	/*D3DCAPS9 caps;
	HRESULT hr = 0;

	hr = _lpD3D->GetDeviceCaps(&caps);

	if (hr == D3D_OK)
	{
		if (caps.Caps2 == D3DCAPS2_CANCALIBRATEGAMMA)
		{*/
			_lpD3D->SetGammaRamp(0, D3DSGR_CALIBRATE, &ramp);
		/*}
		else 
		{
			_lpD3D->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &ramp);
		}
	}*/
	
}
void Direct3DManager::ChangeGamma(IEvent* pevent, IComponent*)
{
	TFloatEvent* tEvent = (TFloatEvent*)pevent->GetData();
	float casfloat = tEvent->m_fValue;

	GetInstance()->m_fGamma = casfloat; // store the 0-100 float value instead of the calculated value

	casfloat = (((tEvent->m_fValue - 0) * (1.0f - -1.0f)) / (100 - 0)) + -1.0f;
	GetInstance()->SetGamma(GetInstance()->GetDirect3DDevice(),casfloat);
}