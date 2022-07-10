#pragma once

#include "core/base_component.h"
#include "rendering/dx_render_context.h"

namespace scd::component {

class renderable : public scd::base_component {
  dx_render_shape* _render_shape;

public:
  renderable(
      scd::object& owner,
      int nModelID,
      DXRenderContext* pRC,
      void (*pfRenderFunc)(RenderNode&));

  renderable(
      scd::object& owner,
      DXMesh* pMesh,
      DXRenderContext* pRC,
      void (*pfRenderFunc)(RenderNode&));

  void AddToRenderSet(void);
  static void AddToRenderSetCallback(IEvent* pEvent, IComponent* pComp);

  ~renderable(void);

  void Init(void);

  void SetMesh(unsigned int nMeshID, DXRenderContext* pContext);

  inline DXRenderShape* GetRenderShape() { return m_cRenderShape; }
};

} // namespace scd::component
