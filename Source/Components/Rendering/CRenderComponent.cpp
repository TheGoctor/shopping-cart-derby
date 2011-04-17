#include "CRenderComponent.h"
#include"..\\..\\CObject.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\ModelManager.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\Renderer.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\DXRenderShape.h"

CRenderComponent::CRenderComponent(CObject* pParent, int nModelID,
								   DXRenderContext* pRC,
								   void (*pfRenderFunc)(RenderNode&))
{
	// Set Parent
	m_pcParent = pParent;

	// Setup Shape
	// TODO: FIX THIS V
	m_cRenderShape = MMNEW(DXRenderShape) DXRenderShape();
	m_cRenderShape->SetMesh(ModelManager::GetInstance()->GetMesh(nModelID));
	m_cRenderShape->SetRenderContext(pRC);
	m_cRenderShape->RenderFunc = pfRenderFunc;

	// Set the frame to the objects	
	m_cRenderShape->SetFrame(pParent->GetTransform());
}

void CRenderComponent::Init(void)
{
}

CRenderComponent::~CRenderComponent(void)
{
	//delete m_cRenderShape;
}
