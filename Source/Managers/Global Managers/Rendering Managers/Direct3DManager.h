#ifndef _CD3DMANAGER_H_
#define _CD3DMANAGER_H_

#include "dxutil.h"
#include "..\Event Manager\CEventManager.h"

#define SAFE_RELEASE(p)			if (p) { p->Release(); p = NULL; }

class Direct3DManager
{
	LPDIRECT3D9				m_lpD3D;		 // The Direct3D object.
	LPDIRECT3DDEVICE9		m_lpDev;		 // The Direct3D Device.
	LPD3DXSPRITE			m_lpSprite; 	 // The Direct3DX Sprite Interface.
	LPD3DXFONT				m_lpFont;        // The Direct3DX Font Interface.
	LPD3DXLINE				m_lpLine;        // The Direct3DX Line Interface
	D3DPRESENT_PARAMETERS	m_PresentParams; // The presentation parameters of the device.
	HWND					m_hWnd;			 // The handle to the window Direct3D is initialized in.

	// Vertex Decls
	IDirect3DVertexDeclaration9 *  m_pVertPosDecl;
	IDirect3DVertexDeclaration9 *  m_pVertPosNormTex2DDecl;
	IDirect3DVertexDeclaration9 *  m_pVertPosTex3DDecl;

	// Singleton stuff
	static Direct3DManager	m_Instance;
	Direct3DManager(void);
	Direct3DManager(Direct3DManager &ref);
	Direct3DManager &operator=(Direct3DManager &ref);

	void InitVertDecls(void);

	///////////////////////////////////////
	

public:

	~Direct3DManager(void);
	static Direct3DManager* GetInstance(void);
	// Initalize Direct3D
	bool InitDirect3D(HWND hWnd, int nScreenWidth, int nScreenHeight, bool bIsWindowed = true, bool bVsync = true);
private:
	// Shutdown Direct3D
	void ShutdownDirect3D(void);
public:
	// Clear Screen
	void Clear(unsigned char ucRed = 0, unsigned char ucGreen = 0, unsigned char ucBlue = 0);

	// Present to the Screen
	void Present(void);

	// Resize
	void ChangeDisplayParam(int nWidth, int nHeight, bool bWindowed);

	// Device Begin/End
	bool DeviceBegin(void);
	bool DeviceEnd(void);

	// Sprite Begin/End
	bool SpriteBegin(void);
	bool SpriteEnd(void);

	void DrawMesh();

	// Accessors
	LPDIRECT3D9	GetDirect3DObject(void) const 
	{ 
		return m_lpD3D;			 
	} 
	LPDIRECT3DDEVICE9 GetDirect3DDevice(void) const 
	{ 
		return m_lpDev;			 
	}  
	LPD3DXSPRITE GetSprite(void) const 
	{ 
		return m_lpSprite;		 
	} 
	LPD3DXLINE GetLine(void) const 
	{ 
		return m_lpLine;			 
	}
	LPD3DXFONT GetFont(void) const 
	{ 
		return m_lpFont;			 
	} 

	const D3DPRESENT_PARAMETERS * GetPresentParams(void) const 
	{ 
		return &m_PresentParams;	 
	} 

	IDirect3DVertexDeclaration9 * GetVertNormTex2DDecl(void) const 
	{ 
		return m_pVertPosNormTex2DDecl; 
	}
	IDirect3DVertexDeclaration9 * GetVertColorDecl(void) const 
	{ 
		return m_pVertPosDecl; 
	}
	IDirect3DVertexDeclaration9 * GetVertTex3DDecl(void) const 
	{ 
		return m_pVertPosTex3DDecl; 
	}

	static void Shutdown(IEvent*, IComponent*);

};

#endif	// _CD3DMANAGER_H_