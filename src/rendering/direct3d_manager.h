#pragma once

// Includes
#include "events/event_manager.h"
#include "events/events.h"
#include "rendering/dx_util.h"

namespace scd {
// Direct 3D Manager
class direct3d_manager {
  LPDIRECT3D9 m_lpD3D;       // The Direct3D object.
  LPDIRECT3DDEVICE9 m_lpDev; // The Direct3D Device.
  LPD3DXSPRITE m_lpSprite;   // The Direct3DX Sprite Interface.
  LPD3DXFONT m_lpFont;       // The Direct3DX Font Interface.
  LPD3DXLINE m_lpLine;       // The Direct3DX Line Interface
  D3DPRESENT_PARAMETERS
  m_PresentParams; // The presentation parameters of the device.
  HWND m_hWnd;     // The handle to the window Direct3D is initialized in.

  // Vertex Decls
  IDirect3DVertexDeclaration9* m_pVertPosDecl;
  IDirect3DVertexDeclaration9* m_pVertPosNormTex2DDecl;
  IDirect3DVertexDeclaration9* m_pVertPosColTex2DDecl;
  // IDirect3DVertexDeclaration9*  m_pVertPosTex3DDecl;

  // Singleton stuff
  Direct3DManager(void);
  Direct3DManager(Direct3DManager& ref);
  Direct3DManager& operator=(Direct3DManager& ref);
  ~Direct3DManager(void);

  // Helper Funcs
  void InitVertDecls(void);
  void ShutdownDirect3D(void);

  ///////////////////////////////////////
  /// array of 256 unsigned shorts for each color for gamma correction
  ///////////////////////////////////////////////////////////////////
  unsigned short m_usRed[256];
  unsigned short m_usBlue[256];
  unsigned short m_usGreen[256];
  float m_fGamma;

public:
  static Direct3DManager* GetInstance(void);
  // Initalize Direct3D
  bool InitDirect3D(
      HWND hWnd,
      int nScreenWidth,
      int nScreenHeight,
      bool bIsWindowed = true,
      bool bVsync = true);

  // Clear Screen
  void Clear(
      unsigned char ucRed = 0,
      unsigned char ucGreen = 0,
      unsigned char ucBlue = 0);

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

  float GetGamma() { return m_fGamma; }

  // Check for Lost Device
  bool CheckForLostDevice(void);

  // SetGamma for the game
  void SetGamma(LPDIRECT3DDEVICE9 _lpD3D, float gamma);

  // Accessors
  LPDIRECT3D9 GetDirect3DObject(void) const { return m_lpD3D; }
  LPDIRECT3DDEVICE9 GetDirect3DDevice(void) const { return m_lpDev; }
  LPD3DXSPRITE GetSprite(void) const { return m_lpSprite; }
  LPD3DXLINE GetLine(void) const { return m_lpLine; }
  LPD3DXFONT GetFont(void) const { return m_lpFont; }
  const D3DPRESENT_PARAMETERS* GetPresentParams(void) const {
    return &m_PresentParams;
  }

  // Vertex Declarations
  IDirect3DVertexDeclaration9* GetVertColorDecl(void) const {
    return m_pVertPosDecl;
  }
  IDirect3DVertexDeclaration9* GetVertNormTex2DDecl(void) const {
    return m_pVertPosNormTex2DDecl;
  }
  IDirect3DVertexDeclaration9* GetVertColTex2DDecl(void) const {
    return m_pVertPosColTex2DDecl;
  }
  // IDirect3DVertexDeclaration9* GetVertTex3DDecl(void) const	  {	return
  // m_pVertPosTex3DDecl;		}

  // Callbacks
  static void Shutdown(IEvent*, IComponent*);
  static void ChangeGamma(IEvent*, IComponent*);
};

} // namespace scd
