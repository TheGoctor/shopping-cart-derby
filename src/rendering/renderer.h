#pragma once

#include "core/containers.h"
#include "rendering/dx_render_context.h"

namespace scd {

class renderer {
  Direct3DManager* m_pD3D; // A pointer to the Direct3D Manager
  ModelManager* m_pMM;     // A pointer to the Model Manager
  DXRenderContextManager* m_pRCM;
  CHUDManager* m_pHUD;
  CEventManager* m_pEM;
  CCameraManager* m_pCM;

  std::map<
      unsigned int,
      CRenderComponent*,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, CRenderComponent*>>>
      m_cRenderComps;

public:
  renderer(void);
  ~renderer(void);

  void Init(HWND hWnd, int nScreenWidth, int nScreenHeight, bool bIsWindowed);

  void LoadModels(void);

  static Renderer* GetInstance(void) {
    static Renderer cRenderer;
    return &cRenderer;
  }

  static void Render(RenderSet& set);

  static void AddToRenderSet(IEvent*, IComponent*);
  static void DestroyObject(IEvent*, IComponent*);

  void RenderScene(void);

  static int CreateRenderComp(lua_State* pLua);
  static CRenderComponent* CreateRenderComp(
      CObject* pParent,
      int nModelID,
      int nRenderContextIdx,
      int nRenderFuncIdx);
  static CRenderComponent* CreateRenderComp(
      CObject* pParent,
      DXMesh* pMesh,
      int nRenderContextIdx,
      int nRenderFuncIdx);

  void Shutdown(void);

  // Callbacks
  static void RenderCallback(IEvent* e, IComponent* comp);
  static void ShutdownCallback(IEvent* e, IComponent* comp);
  static void DestroyComponent(IEvent*, IComponent*);
};

} // namespace scd
