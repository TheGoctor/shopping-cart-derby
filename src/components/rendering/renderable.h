#pragma once

#include "core/base_component.h"
#include "rendering/dx_render_context.h"

namespace scd::component {

class renderable : public IComponent {
  CObject* m_pcParent;
  DXRenderShape* m_cRenderShape;

public:
  renderable(
      CObject* pParent,
      int nModelID,
      DXRenderContext* pRC,
      void (*pfRenderFunc)(RenderNode&));

  renderable(
      CObject* pParent,
      DXMesh* pMesh,
      DXRenderContext* pRC,
      void (*pfRenderFunc)(RenderNode&));

  void AddToRenderSet(void);
  static void AddToRenderSetCallback(IEvent* pEvent, IComponent* pComp);

  ~renderable(void);

  void Init(void);

  void SetMesh(unsigned int nMeshID, DXRenderContext* pContext);

  inline CObject* GetParent() { return m_pcParent; }

  inline DXRenderShape* GetRenderShape() { return m_cRenderShape; }
};

} // namespace scd::component
