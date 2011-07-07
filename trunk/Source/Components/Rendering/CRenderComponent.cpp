#include "CRenderComponent.h"
#include"..\\..\\CObject.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\ModelManager.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\DXRenderShape.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\DXRenderContextManager.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\DXMesh.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

CRenderComponent::CRenderComponent(CObject* pParent, int nModelID,
								   DXRenderContext* pRC,
								   void (*pfRenderFunc)(RenderNode&))
{
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

CRenderComponent::CRenderComponent(CObject* pParent, DXMesh* pMesh, DXRenderContext* pRC,
		void (*pfRenderFunc)(RenderNode&))
{
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

void CRenderComponent::Init(void)
{
	// Register for Events
	//CEventManager* pEM = CEventManager::GetInstance();

		// Add to Set
		/*pEM->RegisterEvent("AddToSet" + m_pcParent->GetID(),
			this, AddToRenderSetCallback);*/
}

void CRenderComponent::AddToRenderSetCallback(IEvent* /*pEvent*/,
											  IComponent* pComp)
{
	// Get Data from Event
	//TRenderEvent* pcRendEvent = (TRenderEvent*)pEvent->GetData();
	//CObject* pParent = pcRendEvent->m_pParent;

	CRenderComponent* pRC = (CRenderComponent*)pComp;
	pRC->AddToRenderSet();
}

void CRenderComponent::AddToRenderSet(void)
{
	m_cRenderShape->GetRenderContext()->AddRenderNode(m_cRenderShape);
}

CRenderComponent::~CRenderComponent(void)
{
	//delete m_cRenderShape;
	MMDEL(m_cRenderShape);
}

void CRenderComponent::SetMesh(unsigned int nMeshID, DXRenderContext* pContext)
{
	DXMesh* pMesh = ModelManager::GetInstance()->GetMesh(nMeshID);
	m_cRenderShape->SetMesh(pMesh);
	m_cRenderShape->SetRenderContext(pContext);
	
}