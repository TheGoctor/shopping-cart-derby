#include "Renderer.h"
#include "RenderNode.h"
#include "RenderSet.h"

#include "Direct3DManager.h"
#include "ModelManager.h"
#include "..\Memory Manager\CMemoryManager.h"
#include "../../../Components/Rendering/CRenderComponent.h"
#include "../../../IComponent.h"
#include "../../../Managers/Global Managers/Event Manager/CEventManager.h"
#include "../../../Managers/Global Managers/Event Manager/CRenderEvent.h"
#include "../../../Managers/Global Managers/Event Manager/CMoveCameraEvent.h"
#include "CCameraManager.h"
#include "Camera.h"
#include "../../../CObject.h"
#include "DXRenderShape.h"
#include "DXRenderShape.h"

#include "DXRenderContextManager.h"
#include "Texture Managers\\CHUDManager.h"

#define   FONT_RECT   {0,0,1000,200}
#define   FONT_COLOR   D3DCOLOR_XRGB(255,255,255)

Renderer::Renderer(void)
{
}

Renderer::~Renderer(void)
{
}

void Renderer::Init(HWND hWnd, int nScreenWidth, 
					   int nScreenHeight, bool bIsWindowed)
{
	// Event Man
	m_pEM = CEventManager::GetInstance();

	// Init Sub-Managers
	m_pD3D = Direct3DManager::GetInstance();
	m_pD3D->InitDirect3D(hWnd, nScreenWidth, nScreenHeight, bIsWindowed, true);

	m_pRCM = DXRenderContextManager::GetInstance();
	m_pRCM->Initialize();

	// Sprites
	//CTextureManager::GetInstance()->InitTextureManager(m_pD3D->GetDirect3DDevice(), 

	// Cam Manager
	m_pCM = CCameraManager::GetInstance();
	m_pCM->Init(nScreenWidth, nScreenHeight);

	// Load
	ModelManager::GetInstance()->LoadModel("Resource/Cart.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_Ceiling_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_Endcap_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_Floor_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplayCircleL_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplayCircleM_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplayCircleS_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplaySquareL_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplaySquareM_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplaySquareS_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FrontDoor_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_GlassCounter_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_GlassFreezer_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_HalfWall_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_LobsterTank_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_OpenFreezerLeft_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_OpenFreezerRight_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_ProduceLeft_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_ProduceRight_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_Register_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_ShortShelf_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_TallShelf_FINShape.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_Wall_FINShape.mesh");

	// Register For Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("Render",
		NULL, RenderCallback);

	pEM->RegisterEvent("AddToSet",
		NULL, AddToRenderSet);

	pEM->RegisterEvent("Shutdown",
		NULL, ShutdownCallback);
}

void Renderer::Render(RenderSet &set)
{
	RenderNode* pRenderNode = set.GetHead();
	while(NULL != pRenderNode)	// While the node is a valid pointer
	{
		// Render each Node
		pRenderNode->RenderProcess();

		// Move pointer to the next one
		pRenderNode = pRenderNode->GetNext();
	}

	set.ClearRenderSet();
}

void Renderer::RenderScene(void)
{
	// clear the back buffer to blue
	m_pD3D->Clear(128, 128, 128);

	// start the scene
	m_pD3D->DeviceBegin();

	//

		// Contexts
		m_pRCM->RenderContexts();

		// Sprites
		IEvent* pRenderEvent = MMNEWEVENT(IEvent) IEvent(CIDGen::GetInstance()->
			GetID("RenderSprites"), NULL);
		m_pEM->PostEvent(pRenderEvent, PRIORITY_IMMEDIATE);

		// Cam Pos
		char text[64];
		sprintf_s(text, 64, "Camera Pos: X = %f Y = %f Z = %f",
			m_pCM->GetCam()->GetViewPosition().x, 
			m_pCM->GetCam()->GetViewPosition().y,
			m_pCM->GetCam()->GetViewPosition().z);
		RECT fontRec = FONT_RECT;
		m_pD3D->GetFont()->DrawText(0, text, -1, &fontRec, 0, FONT_COLOR);

	//

	// stop the scene
	m_pD3D->DeviceEnd();
	
	m_pD3D->Present();
}

void Renderer::AddToRenderSet(IEvent* pcEvent, IComponent* pcSender)
{
	// do some lookup to find the right component in the list
	CRenderEvent* pcRendEvent = (CRenderEvent*)pcEvent;
	CObject* pParent = pcRendEvent->GetParent();

	map<unsigned int, CRenderComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CRenderComponent*>>>::iterator cIter = 
		Renderer::GetInstance()->m_cRenderComps.find(pParent->GetID());
	CRenderComponent* pcRComp = (*cIter).second;
	
	pcRComp->GetRenderShape()->GetRenderContext()->
		AddRenderNode(pcRComp->GetRenderShape());

	//std::map<CRenderComponent*>::iterator cIter;
	//cIter = Renderer::GetInstance()->m_cRenderComps.begin();
	//for(; cIter != Renderer::GetInstance()->m_cRenderComps.end(); cIter++)
	//{
	//	if((*cIter)->GetParent() == pParent)
	//	{
	//		(*cIter)->GetRenderShape().GetRenderContext()->
	//			AddRenderNode(&(*cIter)->GetRenderShape());
	//	}
	//}
}

int Renderer::CreateRenderComp(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, 1);
	int nModelID = (int)lua_tonumber(pLua, 2);
	int nRC = (int)lua_tonumber(pLua, 3);
	int nRendFunc = (int)lua_tonumber(pLua, 4);

	CRenderComponent* pRend = CreateRenderComp(pObj, nModelID, 
		nRC, nRendFunc);

	lua_pop(pLua, 4);
	return 0;
}

CRenderComponent* Renderer::CreateRenderComp(CObject* pParent, int nModelID,
								   int nRenderContextIdx, int nRenderFuncIdx)
{
	DXRenderContext* pRC = GetInstance()->m_pRCM->GetContext((ERenderContext)nRenderContextIdx);
	
	CRenderComponent* comp = MMNEW(CRenderComponent) CRenderComponent(pParent,
		nModelID, pRC, DXRenderShape::GetRenderFunc((ERenderFunc)nRenderFuncIdx));	
	
	comp->Init();
	pParent->AddComponent(comp);

	Renderer::GetInstance()->m_cRenderComps.insert(make_pair((unsigned int)pParent->GetID(), comp));

	return comp;
}

void Renderer::RenderCallback(IEvent* e, IComponent* comp)
{
	Renderer::GetInstance()->RenderScene();
}

// Shutdown
void Renderer::ShutdownCallback(IEvent* e, IComponent* comp)
{
	GetInstance()->Shutdown();
}

void Renderer::Shutdown(void)
{
	// Components
	map<unsigned int, CRenderComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CRenderComponent*>>>::iterator cIter;
	cIter = m_cRenderComps.begin();
	while(cIter != m_cRenderComps.end())
	{
		if(cIter->second)
		{
			MMDEL(CRenderComponent, cIter->second);
		}

		cIter++;
	}
}
