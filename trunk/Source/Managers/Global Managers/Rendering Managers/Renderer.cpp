#include "Renderer.h"
#include "RenderNode.h"
#include "RenderSet.h"

#include "Direct3DManager.h"
#include "ModelManager.h"
#include "..\Memory Manager\CMemoryManager.h"
#include "../../../Components/Rendering/CRenderComponent.h"
#include "../../../IComponent.h"
#include "../../../CGame.h"
#include "../../../Managers/Global Managers/Event Manager/CEventManager.h"
#include "../../../Managers/Global Managers/Event Manager/EventStructs.h"
using namespace EventStructs;
#include "../Console Manager/CConsoleManager.h"
#include "../Camera Manager/CCameraManager.h"
#include "../Camera Manager/Camera.h"
#include "../../../CObject.h"
#include "DXRenderShape.h"
#include "DXRenderShape.h"

#include "DXRenderContextManager.h"
#include "Texture Managers\\CHUDManager.h"

#define   FONT_RECT   {0,0,1000,200}
#define   FONT_COLOR   D3DCOLOR_XRGB(255,255,255)

//Viewport hack
//D3DVIEWPORT9 view1;
//D3DVIEWPORT9 view2;
//D3DVIEWPORT9 view3;
//D3DVIEWPORT9 view4;
//D3DVIEWPORT9  vps[4];

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
	
	/*
	//////// Viewport Hack
	view1.Height	= nScreenHeight/2;
	view1.Width		= nScreenWidth/2 ;
	view1.X			= 0;
	view1.Y			= 0;
	view1.MinZ		= 0.0f;
	view1.MaxZ		= 1.0f;

	view2.Height	= nScreenHeight/2;
	view2.Width		= nScreenWidth/2 ;
	view2.X			= nScreenWidth/2;
	view2.Y			= 0;
	view2.MinZ		= 0.0f;
	view2.MaxZ		= 1.0f;

	view2.Height	= nScreenHeight/2;
	view2.Width		= nScreenWidth/2 ;
	view2.X			= 0;
	view2.Y			= nScreenHeight/2;
	view2.MinZ		= 0.0f;
	view2.MaxZ		= 1.0f;

	view2.Height	= nScreenHeight/2;
	view2.Width		= nScreenWidth/2 ;
	view2.X			= nScreenWidth/2;
	view2.Y			= nScreenHeight/2;
	view2.MinZ		= 0.0f;
	view2.MaxZ		= 1.0f;

	vps[0] = view1;
	vps[1] = view2;
	vps[2] = view3;
	vps[3] = view4;	
*/
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

	// Circle
	ModelManager::GetInstance()->LoadModel("Resource/pSphereShape1.mesh");

	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_EmployeeDoor_FIN_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_Endcap_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_FreeDisplaySquareL_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_FreeDisplaySquareM_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_FreeDisplaySquareS_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_FrontDoor_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_GlassCounter_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_GlassFreezer_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_HalfWall_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_LobsterTank_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_OpenFreezerLeft_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_OpenFreezerRight_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_ProduceLeft_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_ProduceRight_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_Register_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_ShortShelf_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadAABB("Resource/FFP_3D_TallShelf_FINShape_Collision.mesh");


	// draw collision volumes
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_EmployeeDoor_FIN_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_Endcap_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplaySquareL_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplaySquareM_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FreeDisplaySquareS_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_FrontDoor_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_GlassCounter_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_GlassFreezer_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_HalfWall_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_LobsterTank_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_OpenFreezerLeft_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_OpenFreezerRight_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_ProduceLeft_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_ProduceRight_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_Register_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_ShortShelf_FINShape_Collision.mesh");
	ModelManager::GetInstance()->LoadModel("Resource/FFP_3D_TallShelf_FINShape_Collision.mesh");

	ModelManager::GetInstance()->LoadModelWithBones("Resource/test_boxShape.mesh");
	
	ModelManager::GetInstance()->LoadAnimFile("Resource/testcubewalk.anim");

	

	// Register For Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("Render",
		(IComponent*)GetInstance(), RenderCallback);

	pEM->RegisterEvent("AddToSet",
		(IComponent*)GetInstance(), AddToRenderSet);

	pEM->RegisterEvent("Shutdown",
		(IComponent*)GetInstance(), ShutdownCallback);

	pEM->RegisterEvent("DestroyObject", (IComponent*)GetInstance(), DestroyComponent);
}

void Renderer::DestroyObject(IEvent* pcEvent, IComponent*)
{
	TObjectEvent* pcObj = (TObjectEvent*)pcEvent->GetData();

	GetInstance()->m_cRenderComps.erase(GetInstance()->m_cRenderComps.find(
		pcObj->m_pcObj->GetID()));
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
	///Viewport Hack
	//for (unsigned int i = 0; i < 4; ++i)
	//{
	
		//m_pD3D->GetDirect3DDevice()->SetViewport(&vps[i]);
		//m_pD3D->GetDirect3DDevice()->Clear(0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffcccccc, 1.0f, 0L);
		// Contexts
		m_pRCM->RenderContexts();

		// Sprites
		SendIEvent("RenderSprites", NULL, NULL, PRIORITY_IMMEDIATE);

		// Cam Pos
		char text[64];
		sprintf_s(text, 64, "Camera Pos: X:%f Y:%f Z:%f\nFPS:%d",
			m_pCM->GetCam()->GetViewPosition().x, 
			m_pCM->GetCam()->GetViewPosition().y,
			m_pCM->GetCam()->GetViewPosition().z,
			CGame::GetInstance()->GetFPS());
		RECT fontRec = FONT_RECT;
		m_pD3D->GetFont()->DrawText(0, text, -1, &fontRec, 0, FONT_COLOR);
		
		CConsoleManager::GetInstance()->DrawConsole();

	//
	//}
	// stop the scene
	m_pD3D->DeviceEnd();
	
	m_pD3D->Present();
}

void Renderer::AddToRenderSet(IEvent* pcEvent, IComponent* pcSender)
{
	// do some lookup to find the right component in the list
	TRenderEvent* pcRendEvent = (TRenderEvent*)pcEvent->GetData();
	CObject* pParent = pcRendEvent->m_pParent;

	map<unsigned int, CRenderComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CRenderComponent*>>>::iterator cIter = 
		Renderer::GetInstance()->m_cRenderComps.find(pParent->GetID());

	CRenderComponent* pcRComp = (*cIter).second;
	
	pcRComp->GetRenderShape()->GetRenderContext()->
		AddRenderNode(pcRComp->GetRenderShape());

	/*map<unsigned int, CRenderComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CRenderComponent*>>>::iterator cIter;
	cIter = Renderer::GetInstance()->m_cRenderComps.begin();
	for(; cIter != Renderer::GetInstance()->m_cRenderComps.end(); cIter++)
	{
		CRenderComponent* pRC = (*cIter).second;

		if(pRC->GetParent() == pParent)
		{
			pRC->GetRenderShape()->GetRenderContext()->
				AddRenderNode(pRC->GetRenderShape());
		}
	}*/
}

int Renderer::CreateRenderComp(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -4);
	int nModelID = (int)lua_tonumber(pLua, -3);
	int nRC = (int)lua_tonumber(pLua, -2);
	int nRendFunc = (int)lua_tonumber(pLua, -1);

	CRenderComponent* pRend = CreateRenderComp(pObj, nModelID, 
		nRC, nRendFunc);

	lua_pop(pLua, 4);
	return 0;
}

CRenderComponent* Renderer::CreateRenderComp(CObject* pParent, int nModelID,
								   int nRenderContextIdx, int nRenderFuncIdx)
{
	DXRenderContext* pRC = GetInstance()->m_pRCM->GetContext((ERenderContext)nRenderContextIdx);
	
	CRenderComponent* comp = MMNEW(CRenderComponent(pParent,
		nModelID, pRC, DXRenderShape::GetRenderFunc((ERenderFunc)nRenderFuncIdx)));	
	
	comp->Init();
	pParent->AddComponent(comp);

	Renderer::GetInstance()->m_cRenderComps.insert(make_pair((unsigned int)pParent->GetID(), comp));

	return comp;
}

CRenderComponent* Renderer::CreateRenderComp(CObject* pParent, DXMesh* pMesh,
								   int nRenderContextIdx, int nRenderFuncIdx)
{
	DXRenderContext* pRC = GetInstance()->m_pRCM->GetContext((ERenderContext)nRenderContextIdx);
	
	CRenderComponent* comp = MMNEW(CRenderComponent(pParent,
		(DXMesh*)pMesh, pRC, DXRenderShape::GetRenderFunc((ERenderFunc)nRenderFuncIdx)));	
	
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

void Renderer::DestroyComponent(IEvent* pEvent, IComponent* pComp)
{
	TObjectEvent* pObjEvent = (TObjectEvent*)pEvent->GetData();
	int nID = pObjEvent->m_pcObj->GetID();
	CRenderComponent* pRendComp = (CRenderComponent*)pEvent->GetSender();

	if((*GetInstance()->m_cRenderComps.find(nID)).second == pRendComp)
	{
		MMDEL(pRendComp);
		GetInstance()->m_cRenderComps.erase(nID);
	}
}
