#include "renderable.h"

#include "core/object.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "rendering/dx_mesh.h"
#include "rendering/dx_render_context_manager.h"
#include "rendering/dx_render_shape.h"
#include "rendering/model_manager.h"

namespace scd::component {

renderable::renderable(
    CObject* pParent,
    int nModelID,
    DXRenderContext* pRC,
    void (*pfRenderFunc)(RenderNode&)) {
  // Set Parent
  m_pcParent = pParent;

  // Setup Shape
  m_cRenderShape = MMNEW(DXRenderShape);
  m_cRenderShape->SetMesh(ModelManager::GetInstance()->GetMesh(nModelID));
  m_cRenderShape->SetRenderContext(pRC);
  m_cRenderShape->RenderFunc = pfRenderFunc;

  // Set the frame to the objects
  m_cRenderShape->SetFrame(pParent->GetTransform());
}

renderable::renderable(
    CObject* pParent,
    DXMesh* pMesh,
    DXRenderContext* pRC,
    void (*pfRenderFunc)(RenderNode&)) {
  // Set Parent
  m_pcParent = pParent;

  // Setup Shape
  m_cRenderShape = MMNEW(DXRenderShape);
  m_cRenderShape->SetMesh(pMesh);
  m_cRenderShape->SetRenderContext(pRC);
  m_cRenderShape->RenderFunc = pfRenderFunc;

  // Set the frame to the objects
  m_cRenderShape->SetFrame(pParent->GetTransform());
}

void renderable::Init(void) {
  // Register for Events
  // CEventManager* pEM = CEventManager::GetInstance();

  // Add to Set
  /*pEM->RegisterEvent("AddToSet" + m_pcParent->GetID(),
          this, AddToRenderSetCallback);*/
}

void renderable::AddToRenderSetCallback(IEvent* /*pEvent*/, IComponent* pComp) {
  // Get Data from Event
  // TRenderEvent* pcRendEvent = (TRenderEvent*)pEvent->GetData();
  // CObject* pParent = pcRendEvent->m_pParent;

  renderable* pRC = (renderable*)pComp;
  pRC->AddToRenderSet();
}

void renderable::AddToRenderSet(void) {
  m_cRenderShape->GetRenderContext()->AddRenderNode(m_cRenderShape);
}

renderable::~renderable(void) {
  // delete m_cRenderShape;
  MMDEL(m_cRenderShape);
}

void renderable::SetMesh(unsigned int nMeshID, DXRenderContext* pContext) {
  DXMesh* pMesh = ModelManager::GetInstance()->GetMesh(nMeshID);
  m_cRenderShape->SetMesh(pMesh);
  m_cRenderShape->SetRenderContext(pContext);
}
} // namespace scd::component
