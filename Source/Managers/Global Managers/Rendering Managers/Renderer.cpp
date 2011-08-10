#include "Renderer.h"
#include "RenderNode.h"

#include "Direct3DManager.h"
#include "ModelManager.h"
#include "..\Memory Manager\CMemoryManager.h"
#include "../../../Components/Rendering/CRenderComponent.h"
#include "../../../IComponent.h"
#include "../../../CGame.h"
#include "../../../Managers/Global Managers/Event Manager/CEventManager.h"
#include "../../../Managers/Global Managers/Event Manager/EventStructs.h"
#include "../../Component Managers/CAIManager.h"
using namespace EventStructs;
#include "../Console Manager/CConsoleManager.h"
#include "../Camera Manager/CCameraManager.h"
#include "../Camera Manager/Camera.h"
#include "../../../CObject.h"
#include "DXRenderShape.h"
#include "DXRenderShape.h"

#include "DXRenderContextManager.h"
#include "CEffectManager.h"
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

	// Models
	//LoadModels();

	// Effects
	//CEffectManager::GetInstance()->LoadEmitters();
	
	// Register For Events
	m_pEM->RegisterEvent("Render",
		(IComponent*)GetInstance(), RenderCallback);

	m_pEM->RegisterEvent("AddToSet",
		(IComponent*)GetInstance(), AddToRenderSet);

	m_pEM->RegisterEvent("Shutdown",
		(IComponent*)GetInstance(), ShutdownCallback);

	m_pEM->RegisterEvent("DestroyObject", (IComponent*)GetInstance(), DestroyComponent);
}

void Renderer::LoadModels(void)
{
	ModelManager* pMM = ModelManager::GetInstance();

	// Load Level Geometry
	pMM->LoadModel("Resource/Level Geo/FFP_3D_AbandonedCart_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_ACDuct_FIN.mesh");				
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BakerySign_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BasicCutoutA_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BasicCutoutB_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BasicCutoutC_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BoysRestroom_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_ButcherBakeryWindow_FIN.mesh");	
	pMM->LoadModel("Resource/Level Geo/FFP_3D_ButcherSign_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_CardBoardBoxes_FIN.mesh");		
	pMM->LoadModel("Resource/Level Geo/FFP_3D_CeilingSet0_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_CeilingSet1_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_DairyItems_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_DeliSign_FIN.mesh");				
	pMM->LoadModel("Resource/Level Geo/FFP_3D_DeliWindow_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_EmployeeDoor_FIN.mesh");			
	pMM->LoadModel("Resource/Level Geo/FFP_3D_EmployeeOfTheMonth_FIN.mesh");	
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Endcap_FIN.mesh");				
	pMM->LoadModel("Resource/Level Geo/FFP_3D_ExitSign_FIN.mesh");				
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet0_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet1_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet2_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet3_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet4_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet5_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet6_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet7_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet8_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorSet9_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FloorTrim_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FrontDoor_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FSDCircleL_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FSDCircleM_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FSDCircleS_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FSDSquareL_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FSDSquareM_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_FSDSquareS_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_GirlsRestroom_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_GlassCounter_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_GlassFreezer_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_HalfWall_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_HelpDesk_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_LightFixtures_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_LobsterCutout_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_LobsterTank_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_MeatSection_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_OpenFreezerLeft_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_OpenFreezerRight_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Outside_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Pallet_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Pirate_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_ProduceLeft_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_ProduceRight_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Register_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Shelf_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_ShelfStuff_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Snowman_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_StartingLine_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Umbra_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_WallSet0_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_WallSet1_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_WallSupports_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BreakUpSign0_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BreakUpSign1_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_Alien_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_LittleEvelyns_FIN.mesh");

	// these meshes will be sharing a texture and render context, so they will be a special case when loading the level
	/*pMM->LoadModel("Resource/Level Geo/FFP_3D_BasicCutoutA_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BasicCutoutB_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_BasicCutoutC_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_StartBanner_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_StartFrame0_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_StartFrame1_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_StartLight0_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_StartLight1_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_StartLight2_FIN.mesh");
	pMM->LoadModel("Resource/Level Geo/FFP_3D_StartLight3_FIN.mesh");*/



	// Collision volumes
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_CardBoardBoxes_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_Endcap_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_FrontDoor_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_FSDCircleL_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_FSDCircleM_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_FSDCircleS_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_FSDSquareL_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_FSDSquareM_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_FSDSquareS_FIN_Collision.mesh");
	//pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_GlassCounter_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_GlassFreezer_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_HalfWall_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_HelpDesk_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_LobsterCutout_FIN_Collision.mesh");
	//pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_LobsterTank_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_OpenFreezerLeft_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_OpenFreezerRight_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_Pallet_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_Pirate_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_ProduceLeft_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_ProduceRight_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_Register_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_Shelf_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_Snowman_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_Umbra_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_Alien_FIN_Collision.mesh");
	pMM->LoadAABB("Resource/Collision Volumes/FFP_3D_LittleEvelyns_FIN_Collision.mesh");

	// PowerUps
	pMM->LoadModel("Resource/PowerUps/Banana_MShape.mesh");
	pMM->LoadModel("Resource/PowerUps/Chicken_NoodleSoup_MShape.mesh");

	pMM->LoadModelWithBones("Resource/PowerUps/FFP_3D_DONUTS_FINShape.mesh");

	pMM->LoadModel("Resource/PowerUps/FFP_3D_IceTurkeyBox_FIN.mesh");
	pMM->LoadModel("Resource/PowerUps/Jam_MShape.mesh");
	pMM->LoadModel("Resource/PowerUps/Peanut_butter_MShape.mesh");
	pMM->LoadModel("Resource/PowerUps/Pie_MShape.mesh");
	pMM->LoadModel("Resource/PowerUps/Thors_Thunder_MShape.mesh");
	pMM->LoadModel("Resource/PowerUps/TurkeyShape.mesh");
	pMM->LoadModel("Resource/PowerUps/FFP_3D_PeanutPile_FINShape.mesh");
	pMM->LoadModel("Resource/PowerUps/Donut.mesh");

	// Shadows
	pMM->LoadModel("Resource/Character Models/Shadows/FFP_3D_BikerShadow_FIN.mesh");
	pMM->LoadModel("Resource/Character Models/Shadows/FFP_3D_BanditosShadow_FIN.mesh");
	pMM->LoadModel("Resource/Character Models/Shadows/FFP_3D_LarperShadow_FIN.mesh");
	pMM->LoadModel("Resource/Character Models/Shadows/FFP_3D_ScientistShadow_FIN.mesh");
	pMM->LoadModel("Resource/Character Models/Shadows/FFP_3D_SashaShadow_FIN.mesh");

	// Level Shadows
	pMM->LoadModel("Resource/Level Geo/Shadows/Shadow_Circle_Small_M.mesh");
	pMM->LoadModel("Resource/Level Geo/Shadows/Shadow_Circle_Medium_M.mesh");
	pMM->LoadModel("Resource/Level Geo/Shadows/Shadow_Circle_Huge_M.mesh");
	pMM->LoadModel("Resource/Level Geo/Shadows/Shadow_FSD_Square_L.mesh");
	pMM->LoadModel("Resource/Level Geo/Shadows/Shadow_Special.mesh");
	pMM->LoadModel("Resource/Level Geo/Shadows/Shadow_Cashier_M.mesh");

	pMM->LoadModel("Resource/Collision Volumes/FFP_3D_CheckoutCollision_FIN.mesh");

	pMM->LoadModel("Resource/PowerUps/FFP_3D_BlueLamp_FINShape.mesh"); // 86
}

void Renderer::DestroyObject(IEvent* pcEvent, IComponent*)
{
	TObjectEvent* pcObj = (TObjectEvent*)pcEvent->GetData();

	GetInstance()->m_cRenderComps.erase(GetInstance()->m_cRenderComps.find(
		pcObj->m_pcObj->GetID()));
}

void Renderer::Render(RenderSet &set)
{
	RenderSet::iterator pRenderNode;
	pRenderNode = set.begin();
	while(pRenderNode != set.end())	// While the node is a valid pointer
	{
		// Render each Node
		(*pRenderNode)->RenderProcess();

		// Move pointer to the next one
		++pRenderNode;
	}

	set.clear();
}

void Renderer::RenderScene(void)
{
	// clear the back buffer to blue
	m_pD3D->Clear(0, 0, 0);

	//// HACK: Add every renderable object to their render sets
	//map<unsigned int, CRenderComponent*, less<unsigned int>, CAllocator<
	//	pair<unsigned int, CRenderComponent*>>>::iterator pCompIter;
	//pCompIter = m_cRenderComps.begin();
	//while(pCompIter != m_cRenderComps.end())
	//{
	//	SendRenderEvent("AddToSet", (IComponent*)GetInstance(),
	//		pCompIter->second->GetParent(), PRIORITY_IMMEDIATE);
	//	++pCompIter;
	//}
	//// END HACK

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
		SendIEvent("RenderSprites", (IComponent*)GetInstance(),
			NULL, PRIORITY_IMMEDIATE);
#ifdef _DEBUG

		// Cam Pos
		char text[256];
		sprintf_s(text, 256, "Camera Pos: X:%f Y:%f Z:%f\t\tMemory Used(General): %d / %d bytes\nFPS:%d\t\t\tMemory Used(Events): %d / %d bytes",
			m_pCM->GetCam()->GetViewPosition().x, 
			m_pCM->GetCam()->GetViewPosition().y,
			m_pCM->GetCam()->GetViewPosition().z,
			CMemoryManager::GetInstance()->GetGHeapMemUsed(),
			CMemoryManager::GetInstance()->GetGHeapMemSize(),
			CGame::GetInstance()->GetFPS(),
			CMemoryManager::GetInstance()->GetEHeapMemUsed(),
			CMemoryManager::GetInstance()->GetEHeapMemSize());
		RECT fontRec = FONT_RECT;
		m_pD3D->GetFont()->DrawText(0, text, -1, &fontRec, 0, FONT_COLOR);

#else
		RECT fontRec = FONT_RECT;
		m_pD3D->GetFont()->DrawText(0, CAIManager::GetInstance()->GetAIInfo().c_str(), -1, &fontRec, 0, FONT_COLOR);
#endif

		CConsoleManager::GetInstance().DrawConsole();

	//
	//}
	// stop the scene
	m_pD3D->DeviceEnd();
	
	m_pD3D->Present();
}

void Renderer::AddToRenderSet(IEvent* pcEvent, IComponent*)
{
	// do some lookup to find the right component in the list
	TRenderEvent* pcRendEvent = (TRenderEvent*)pcEvent->GetData();
	CObject* pParent = pcRendEvent->m_pParent;

	map<unsigned int, CRenderComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CRenderComponent*>>>::iterator cIter = 
		Renderer::GetInstance()->m_cRenderComps.find(pParent->GetID());

	if(cIter != GetInstance()->m_cRenderComps.end())
	{
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
}

int Renderer::CreateRenderComp(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -4);
	
	int nModelID = -1;
	DXMesh* pMesh = NULL;

	if(lua_type(pLua, -3) == LUA_TNUMBER)
		nModelID = lua_tointeger(pLua, -3);
	else
		pMesh = (DXMesh*)lua_topointer(pLua, -3);

	int nRC = (int)lua_tonumber(pLua, -2);
	int nRendFunc = (int)lua_tonumber(pLua, -1);

	CRenderComponent* pRend;

	if(pMesh != NULL)
		pRend = CreateRenderComp(pObj, pMesh, nRC, nRendFunc);
	else
		pRend = CreateRenderComp(pObj, nModelID, nRC, nRendFunc);

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

void Renderer::RenderCallback(IEvent*, IComponent*)
{
	Renderer::GetInstance()->RenderScene();
}

// Shutdown
void Renderer::ShutdownCallback(IEvent*, IComponent*)
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
			MMDEL(cIter->second);
		}

		cIter++;
	}

	m_cRenderComps.clear();
}

void Renderer::DestroyComponent(IEvent* pEvent, IComponent*)
{
	TObjectEvent* pObjEvent = (TObjectEvent*)pEvent->GetData();
	int nID = pObjEvent->m_pcObj->GetID();

	map<unsigned int, CRenderComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CRenderComponent*>>>::iterator pIter;
	pIter = GetInstance()->m_cRenderComps.find(nID);
	if(pIter != GetInstance()->m_cRenderComps.end())
	{
		pObjEvent->m_pcObj->RemoveComponent(pIter->second);
		CEventManager::GetInstance()->UnregisterEventAll(pIter->second);
		MMDEL(pIter->second);
		GetInstance()->m_cRenderComps.erase(pIter);
	}
}