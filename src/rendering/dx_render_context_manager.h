#pragma once

#include "enums.h"
#include "rendering/dx_render_context.h"

namespace scd {
class dx_render_context_manager {
private:
  ///////////////////////////////////////////////
  // Render contexts
  DXRenderContext m_cContexts[RC_MAX];

  // Map of Contexts (Key = ID of Texture Name, Data = DXRenderContext)
  scd::map<unsigned int, DXRenderContext*> m_cDynamicContexts;

  void BuildRenderContexts();
  typedef scd::map<unsigned int, ID3DXEffect*> ShaderMap;
  ShaderMap m_cShaderFiles;

  ID3DXEffect* LoadShader(const char* szFXFile);

public:
  dx_render_context_manager();
  ~dx_render_context_manager();

  ///////////////////////////////////////////////
  // Standard methods

  void Initialize();
  void RenderContexts();

  // Init
  void Init(void);

  // Shutdown
  static void ShutdownCallback(IEvent*, IComponent*);
  void Shutdown(void);

  // Factory
  DXRenderContext* CreateRenderContext(
      string szTexFile = "default.jpg",
      string szEffectFile = "Texture2D.fx",
      void (*pRenderFunc)(RenderNode& node) =
          DXRenderContext::TexturedRenderContextFunc);

  ///////////////////////////////////////////////
  // Render context accessors
  DXRenderContext* GetContext(ERenderContext eContextIdx);
};
} // namespace scd
