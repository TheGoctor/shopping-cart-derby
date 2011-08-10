#include "CLevelManager.h"
#include <iostream>
#include <fstream>
using namespace std;

#include "..\Global Managers\Event Manager\CEventManager.h"
#include "..\Global Managers\Event Manager\CIDGen.h"
#include "..\Global Managers\Event Manager\EventStructs.h"
#include "..\..\Components\Level\CGoalItemComponent.h"
#include "..\Global Managers\Rendering Managers\Renderer.h"
#include "..\Global Managers\Rendering Managers\ModelManager.h"
#include "..\Component Managers\CCollisionManager.h"
#include "..\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Components\rendering\CRenderComponent.h"
#include "..\..\Components\rendering\CBlueLightSpecialComp.h"
#include "CSpawningManager.h"
using namespace EventStructs;

// Not used
//#include "..\..\Components\Level\CHeldItemComponent.h"
//#include "..\Global Managers\Memory Manager\CMemoryManager.h"
//#include "..\Global Managers\Rendering Managers\dxutil.h"
//#include "..\..\CObject.h"

//Sound Hack//
#include "..\Global Managers\Sound Manager\CWwiseSoundManager.h"

CLevelManager::CLevelManager()
{
}

void CLevelManager::Init()
{
	CEventManager* pEM = CEventManager::GetInstance();

	// Get Singleton
	IComponent* pInstance = (IComponent*)GetInstance();
	
	// Level Select
	pEM->RegisterEvent("Level1Selected", pInstance, Level1SelectedCallback);

	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);

	// render the world when paused and in console
	szEvent = "Update";
	szEvent += PAUSE_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);
	szEvent = "Update";
	szEvent += CONSOLE_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);
	szEvent = "Update";
	szEvent += PAUSE_OPTIONS_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);
	szEvent = "Update";
	szEvent += PAUSE_KEYBINDS_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);
	szEvent = "Update";
	szEvent += QUIT_CONFIRMATION_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);
	szEvent = "Update";
	szEvent += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);
	szEvent = "Update";
	szEvent += IN_GAME_HOW_TO_PLAY_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);

	// Exit Gameplay
	//szEvent = "ShutdownObjects";
	//szEvent += GAMEPLAY_STATE;
	//pEM->RegisterEvent(szEvent, pInstance, Shutdown);
	pEM->RegisterEvent("Shutdown", pInstance, Shutdown);

	szEvent = "Update";
	szEvent += WIN_STATE;
	pEM->RegisterEvent(szEvent, pInstance, Update);

	BuildLevel("Resource/Node Map/SCD_NodeMap.nm");


	m_bDrawCollision = false;
	m_bDrawGeometry = true;

	BuildShadows("Resource/Level Geo/Shadows/Shadow_Node_Map.nm");

	//// Boundaries on walls
	//std::string szWallName1 = "MaxXWall";
	//CObject* pMaxXWall = CObjectManager::GetInstance()->CreateObject(szWallName1);
	//std::string szWallName2 = "MinXWall";
	//CObject* pMinXWall = CObjectManager::GetInstance()->CreateObject(szWallName2);
	//std::string szWallName3 = "MaxZWall";
	//CObject* pMaxZWall = CObjectManager::GetInstance()->CreateObject(szWallName3);
	//std::string szWallName4 = "MinZWall";
	//CObject* pMinZWall = CObjectManager::GetInstance()->CreateObject(szWallName4);

	//TPlane MaxX, MinX, MaxZ, MinZ;
	//MaxX.cPlanePoint = D3DXVECTOR3(48.0f, 0.0f, 0.0f);
	//MaxX.cPlaneNormal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);

	//MinX.cPlanePoint = D3DXVECTOR3(-48.0f, 0.0f, 0.0f);
	//MinX.cPlaneNormal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	//MaxZ.cPlanePoint = D3DXVECTOR3(0.0f, 0.0f, 36.0f);
	//MaxZ.cPlaneNormal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//MinZ.cPlanePoint = D3DXVECTOR3(0.0f, 0.0f, -36.0f);
	//MinZ.cPlaneNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	//CCollideable* pxmax = CCollisionManager::GetInstance()->
	//	CreateCollideableComponent(pMaxXWall, true, true, OBJWALL);
	//pxmax->SetBVType(BPLANE);
	//pxmax->SetPlane(MaxX);
	//CCollideable* pxmin = CCollisionManager::GetInstance()->
	//	CreateCollideableComponent(pMinXWall, true, true, OBJWALL);
	//pxmin->SetBVType(BPLANE);
	//pxmin->SetPlane(MinX);
	//CCollideable* pzmax = CCollisionManager::GetInstance()->
	//	CreateCollideableComponent(pMaxZWall, true, true, OBJWALL);
	//pzmax->SetBVType(BPLANE);
	//pzmax->SetPlane(MaxZ);
	//CCollideable* pzmin = CCollisionManager::GetInstance()->
	//	CreateCollideableComponent(pMinZWall, true, true, OBJWALL);
	//pzmin->SetBVType(BPLANE);
	//pzmin->SetPlane(MinZ);
}

// Change State
void CLevelManager::Level1SelectedCallback(IEvent*, IComponent*)
{
	static CLevelManager* pLM = CLevelManager::GetInstance();

	// Build Level
	//pLM->BuildLevel("Resource/Node Map/SCD_NodeMap.nm");
	//pLM->BuildShadows("Resource/Level Geo/Shadows/Shadow_Node_Map.nm");

	// Change States
	SendStateEvent("StateChange", (IComponent*)pLM,
		GAMEPLAY_STATE, PRIORITY_IMMEDIATE);
}

CLevelManager::~CLevelManager()
{
	MMDELARRAYEX(m_levelNodes, HEAPID_GENERAL);
	MMDELARRAYEX(m_shadowNodes, HEAPID_GENERAL);
}

void CLevelManager::AddRenderCollision(CRenderComponent* pRC)
{
	m_cLevelCollisionRenderComps.push_back(pRC);
}

void CLevelManager::Shutdown(IEvent*, IComponent*)
{
	CLevelManager* pLM = CLevelManager::GetInstance();
	list<CRenderComponent*, CAllocator<CRenderComponent*>>::iterator pLIter;
	pLIter = pLM->m_cLevelRenderComps.begin();
	while(pLIter != pLM->m_cLevelRenderComps.end())
	{
		MMDEL(*pLIter);
		++pLIter;
	}
	pLM->m_cLevelRenderComps.clear();

	list<CRenderComponent*, CAllocator<CRenderComponent*>>::iterator pCIter;
	pCIter = pLM->m_cLevelCollisionRenderComps.begin();
	while(pCIter != pLM->m_cLevelCollisionRenderComps.end())
	{
		MMDEL(*pCIter);
		++pCIter;
	}
	pLM->m_cLevelCollisionRenderComps.clear();
}

void CLevelManager::BuildLevel(char* szNodeMap)
{
	fstream fin(szNodeMap, ios_base::in | ios_base::binary);
	int strlen = 0;
	m_objectcount = 0;
	double matrixbuffer = 0.0;

	// LOAD NODES
	if(fin.is_open())
	{
		fin.read((char*)&m_objectcount, sizeof(int));
		m_levelNodes = MMNEWARRAYEX(CNode, m_objectcount, HEAPID_GENERAL);
		for(int i = 0; i < m_objectcount; ++i)
		{
			// name length
			fin.read((char*)&strlen, sizeof(int));
			// name
			fin.read((char*)&m_levelNodes[i].szName, strlen);
			// worldMat
			fin.read((char*)&m_levelNodes[i].tWorldMatrix, sizeof(D3DXMATRIX));
		}

	}
	fin.close();
	fin.clear();

	// ADD Game Objects based on the loaded in nodes
	D3DXVECTOR3 vWorldSpacePos;
	D3DXVECTOR3 vRotateAxis;
	ModelManager* pMM = ModelManager::GetInstance();
	CObjectManager* pOM = CObjectManager::GetInstance();
	Renderer* pRenMan = Renderer::GetInstance(); 
	CRenderComponent* pRC;
	CObject *pObj, *pColObj;

	int nMeshIDToNotCreateRenderCompFor = ModelManager::GetInstance()->GetMeshIndexByName("FFP_3D_CheckoutCollision_FIN");

	for(int i = 0; i < m_objectcount; ++i)
	{
		vWorldSpacePos.x = m_levelNodes[i].tWorldMatrix._41;
		vWorldSpacePos.y = m_levelNodes[i].tWorldMatrix._42;
		vWorldSpacePos.z = m_levelNodes[i].tWorldMatrix._43;

		D3DXMATRIX rot(m_levelNodes[i].tWorldMatrix);
		rot._41 = 0.0f;
		rot._42 = 0.0f;
		rot._43 = 0.0f;
		rot._44 = 0.0f;

		// if it starts with a p, it must be a player
		if(tolower(m_levelNodes[i].szName[0]) == 'p')
		{
			unsigned int strlen = std::strlen(m_levelNodes[i].szName);
			std::string player("Player");
			for(unsigned int inner = 0; inner < strlen; ++inner)
			{
				// if you hit a number, you will know which player spawn it is
				if(isdigit(m_levelNodes[i].szName[inner]))
				{
					// get right player
					player += m_levelNodes[i].szName[inner];
					CObject* playPtr = pOM->GetObjectByName(player);
					// move player
					if(!playPtr)
						continue;
					playPtr->GetTransform()->GetLocalMatrix() = 
						m_levelNodes[i].tWorldMatrix;
					// we are done here
					continue;
				}
			}
		}
		
		int meshindex = pMM->GetMeshIndexByName(m_levelNodes[i].szName);	
		if(meshindex < 0 || meshindex == nMeshIDToNotCreateRenderCompFor)
		{
			continue;
		}

		// Create an object
		string szObjID(m_levelNodes[i].szName);
		//szObjID += static_cast<char>(i);
		pObj = pOM->CreateObject(szObjID);

		pObj->GetTransform()->GetLocalMatrix() = m_levelNodes[i].tWorldMatrix;

		int rcidx = meshindex + 1, rfunc = 2;

		// Change Func for Starting Light
		if(rcidx == RC_STARTLIGHT)
		{
			rfunc = RF_TEXTURE_CHANGE;
		}
		if(rcidx == RC_ENDCAP)
		{
			rfunc = RF_TEXTURE_CHANGE_INST;

			// since it's an endcap, we can fill out the info for it
			TSpawnLocation tSpawnLocation;
			tSpawnLocation.m_bUsed = false;
			tSpawnLocation.m_cEndCap = pObj;
			tSpawnLocation.m_tPosition.x = pObj->GetTransform()->GetWorldMatrix()._41;
			tSpawnLocation.m_tPosition.y = pObj->GetTransform()->GetWorldMatrix()._42;
			tSpawnLocation.m_tPosition.z = pObj->GetTransform()->GetWorldMatrix()._43;
			tSpawnLocation.m_tRotation = pObj->GetTransform()->GetWorldMatrix();		

			CSpawningManager::GetInstance()->AddHeldItemSpawnLocation(tSpawnLocation);
		}

		pRC = pRenMan->CreateRenderComp(pObj,
			meshindex, 
			rcidx, rfunc);

		// Endcaps
		if(rcidx == RC_ENDCAP)
		{
			CBlueLightSpecialComp::CreateBlueLightSpecialComponent(pRC);
		}

		// Add a Shadow
		//FindAndCreateShadow(pObj, (ERenderContext)rcidx);

		// Save it for Renderering
		m_cLevelRenderComps.push_back(pRC);

		// stupid circle collision hack
		if(meshindex == RC_FSDCL-1 || meshindex == RC_FSDCM-1 || meshindex == RC_FSDCS-1)
		{
			TAABB aabb, localAABB;
			D3DXMatrixIdentity(&rot);
			if(pMM->GetAABBByNameWithOffset(m_levelNodes[i].szName, vWorldSpacePos, rot, aabb, localAABB))
			{
				TSphere sphere;
				sphere.cPosition = vWorldSpacePos;
				sphere.fRadius = fabs((localAABB.cBoxMax.x - localAABB.cBoxMin.x) * 0.5f);
				CCollideable* pcol = CCollisionManager::GetInstance()->
					CreateCollideableComponent(pObj,true, true, OBJFSD);
				pcol->SetBVType(BSPHERE);
				pcol->SetSphere(sphere);
				continue;
			}
		}

		//// Create an object to render collision
		string szColObjID = "ColRen";
		szColObjID += (char)i;
		pColObj = pOM->CreateObject(szColObjID, 0.0, 0.0, 0.0, 0.0f, pObj);

		//// look for collision
		TAABB aabb;
		TAABB localAABB;
		if(pMM->GetAABBByNameWithOffset(m_levelNodes[i].szName, vWorldSpacePos, rot, aabb, localAABB))
		{
			CCollideable* pCollideable = CCollisionManager::GetInstance()->
				CreateCollideableComponent(pColObj, true, true, OBJSHELF);
			pCollideable->SetBVType(BAABB);
			pCollideable->SetAABB(aabb);
			//NOTE: stop adding renderable collision volumes on these, I'm doing it in CCollisionManager when I
			//			put all the boxes together, so stop! -Raymoney
		}
	}

	// load the checkout location
	LoadLocations();
}

// Build Shadows for Level
void CLevelManager::BuildShadows(char* szNodeMap)
{
	// Get Singletons
	ModelManager* pMM = ModelManager::GetInstance();
	CObjectManager* pOM = CObjectManager::GetInstance();
	Renderer* pRenMan = Renderer::GetInstance(); 

	// Open the File
	fstream fin(szNodeMap, ios_base::in | ios_base::binary);
	int strlen = 0;
	int nShadowObjectCount = 0;
	double matrixbuffer = 0.0;

	// LOAD NODES
	if(fin.is_open())
	{
		fin.read((char*)&nShadowObjectCount, sizeof(int));
		m_shadowNodes = MMNEWARRAYEX(CNode, nShadowObjectCount, HEAPID_GENERAL);
		for(int i = 0; i < nShadowObjectCount; ++i)
		{
			// name length
			fin.read((char*)&strlen, sizeof(int));
			// name
			fin.read((char*)&m_shadowNodes[i].szName, strlen);
			// worldMat
			fin.read((char*)&m_shadowNodes[i].tWorldMatrix, sizeof(D3DXMATRIX));
		}

	}
	fin.close();
	fin.clear();

	// ADD Game Objects based on the loaded in nodes
	D3DXVECTOR3 vWorldSpacePos;
	D3DXVECTOR3 vRotateAxis;
	CRenderComponent* pRC;
	CObject* pShadowObj;
	for(int i = 0; i < nShadowObjectCount; ++i)
	{
		vWorldSpacePos.x = m_shadowNodes[i].tWorldMatrix._41;
		vWorldSpacePos.y = m_shadowNodes[i].tWorldMatrix._42;
		vWorldSpacePos.z = m_shadowNodes[i].tWorldMatrix._43;

		D3DXMATRIX rot(m_shadowNodes[i].tWorldMatrix);
		rot._41 = 0.0f;
		rot._42 = 0.0f;
		rot._43 = 0.0f;
		rot._44 = 0.0f;

		// Get the Shadow Mesh
		//char* szStripedName;
		int meshindex = pMM->GetMeshIndexByName(m_shadowNodes[i].szName);//, false, true, &szStripedName);	
		if(meshindex < 0)
		{
			continue;
		}

		// Create Shadow Obj
		string szObjID(m_shadowNodes[i].szName);
		szObjID += i;
		pShadowObj = pOM->CreateObject(szObjID);
		pShadowObj->GetTransform()->GetLocalMatrix() = m_shadowNodes[i].tWorldMatrix;

		// Find the Context
		ERenderContext eShadowRenderContext;

		if(meshindex == 82 || meshindex == 83 || meshindex == 84)
		{
			eShadowRenderContext = RC_CIRCLE_SHADOW;
		}
		/*if(meshindex == 80)
		{
			eShadowRenderContext = RC_SHELF_SHADOW;
		}*/
		if(meshindex == 85)
		{
			eShadowRenderContext = RC_FSDS_L_SHADOW;
		}
		if(meshindex == 86)
		{
			eShadowRenderContext = RC_SQUARE_SHADOW;
		}
		if(meshindex == 87)
		{
			eShadowRenderContext = RC_CASHIER_SHADOW;
		}


		// Create Render Comp
		pRC = pRenMan->CreateRenderComp(pShadowObj,
			meshindex, eShadowRenderContext, RF_INDEXED_VERT_TEX2);

		// Save it for Renderering
		m_cLevelRenderComps.push_back(pRC);
	}

}

void CLevelManager::FindAndCreateShadow(CObject* pParentObj, ERenderContext eObjRenContext)
{
	//// Get Singletons
	//ModelManager* pMM = ModelManager::GetInstance();
	//CObjectManager* pOM = CObjectManager::GetInstance();
	//Renderer* pRenMan = Renderer::GetInstance(); 

	//

	//// Find the Shadow Mesh and Context Pair
	//switch(eObjRenContext)
	//{
	//case RC_FSDCS: // Small Circle FSD
	//	{
	//		pShadowObj = pOM->CreateObject(szShadowObjName);
	//		eShadowRenderContext = RC_CIRCLE_SHADOW;
	//		szShadowMeshName = "Shadow_Circle_Small_MShape";
	//		break;
	//	}
	//case RC_FSDCM: // Medium Circle FSD
	//	{
	//		pShadowObj = pOM->CreateObject(szShadowObjName);
	//		eShadowRenderContext = RC_CIRCLE_SHADOW;
	//		szShadowMeshName = "Shadow_Circle_Medium_MShape";
	//		break;
	//	}
	//case RC_FSDCL: // Huge Circle FSD
	//	{
	//		pShadowObj = pOM->CreateObject(szShadowObjName);
	//		eShadowRenderContext = RC_CIRCLE_SHADOW;
	//		szShadowMeshName = "Shadow_Circle_Huge_MShape";
	//		break;
	//	}
	//default:
	//break;
	//}

	//// If we made a Shadow
	//if(pShadowObj)
	//{
	//	// Connect to Parent
	//	pOM->BindObjects(pParentObj, pShadowObj);
	//	//pShadowObj->GetTransform()->TranslateFrame(D3DXVECTOR3(0.0f, 0.1f, 0.0f));

	//	// Get Shadow Mesh idx
	//	int nMeshIdx = pMM->GetMeshIndexByName(szShadowMeshName.c_str(), false, false);
	//	DXMesh* pShadowMesh = pMM->GetMesh(nMeshIdx);

	//	// Create Render Comp
	//	CRenderComponent* pRC = pRenMan->CreateRenderComp(pShadowObj,
	//		pShadowMesh, eShadowRenderContext, RF_INDEXED_VERT_TEX2);

	//	// Save it for Renderering
	//	m_cLevelRenderComps.push_back(pRC);
	//}
}

void CLevelManager::DrawCollision()
{
	m_bDrawCollision = !m_bDrawCollision;
}

void CLevelManager::DrawGeometry()
{
	m_bDrawGeometry = !m_bDrawGeometry;
}

void CLevelManager::Update(IEvent*, IComponent*)
{
	unsigned AddToSet = CIDGen::GetInstance()->GetID("AddToSet");

	if(CLevelManager::GetInstance()->GetDrawGeometry())
	{
		// Render the Geometry
		list<CRenderComponent*, CAllocator<CRenderComponent*>>::iterator pcGeoIter;
		pcGeoIter = CLevelManager::GetInstance()->m_cLevelRenderComps.begin();
		CRenderComponent* pRC;
		while(pcGeoIter != CLevelManager::GetInstance()->m_cLevelRenderComps.end())
		{
			// Get the Obj
			pRC = (*pcGeoIter);

			SendRenderEvent("AddToSet", (IComponent*)GetInstance(), pRC->GetParent(), PRIORITY_IMMEDIATE);

			// Inc the Iter
			pcGeoIter++;
		}
	}

	if(CLevelManager::GetInstance()->GetDrawCollision())
	{
		// Render the Collision Geometry
		list<CRenderComponent*, CAllocator<CRenderComponent*>>::iterator pcColIter;
		pcColIter = CLevelManager::GetInstance()->m_cLevelCollisionRenderComps.begin();

		CRenderComponent* pCRC;
		while(pcColIter != CLevelManager::GetInstance()->m_cLevelCollisionRenderComps.end())
		{
			// Get the Obj
			pCRC = (*pcColIter);

			SendRenderEvent("AddToSet", (IComponent*)GetInstance(), pCRC->GetParent(), PRIORITY_IMMEDIATE);

			// Inc the Iter
			pcColIter++;
		}
	}
	
	//CCollisionManager::GetInstance()->SearchForCollision();
}

int CLevelManager::ToggleCollisionVision(lua_State* ptr)
{
	CLevelManager::GetInstance()->DrawCollision();
	return 0;
}

int CLevelManager::ToggleGeometryVision(lua_State* ptr)
{
	CLevelManager::GetInstance()->DrawGeometry();
	return 0;
}

// Load location and info for checkout
void CLevelManager::LoadLocations()
{
	// Search for endcap nodes
	//ModelManager* pMM = ModelManager::GetInstance();
	//int nMeshIndex;
	char chBuffer[64];
	char chTempBuffer[64];
	//char chCounter[32];
	int nNameLength;
	//int nNodeNumber = -1;
	std::string szID;
	D3DXVECTOR3 cPos;
	CLevelManager::CNode* tNode = CLevelManager::GetInstance()->GetLevelNodes();

	for(int nIndex = 0; nIndex < CLevelManager::GetInstance()->GetNumNodes(); ++nIndex)
	{
		memset(chBuffer, 0, 64);
		memset(chTempBuffer, 0, 64);
		nNameLength = strlen(tNode[nIndex].szName);

		// find an instance of "Endcap" using string tokenizer

		// copy the current node's name into our buffer
		memcpy(chBuffer, tNode[nIndex].szName, nNameLength);

		// init the strtok to deliminate based on underscores
		char* nextToken = NULL;
		char* szTokenizer = strtok_s(chBuffer, "_", &nextToken);

		// while strtoke doesn't return null (aka it's not at the end of the tokenizing string)
		while(szTokenizer != NULL)
		{	
			// if a non case sensative compare matches what we're looking for
			if(_stricmp(szTokenizer, "CheckoutCollision") == 0)
			{
				// Create an object for the node with the data
				m_pCheckoutNode = tNode[nIndex];
				SendIEvent("InitCheckoutLocation", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);

				// don't return since we have to delete below
				break;;
			}

			// assign chTempBuffer to the next token in the string (not a new one because we passed null)
			szTokenizer = strtok_s(NULL, "_", &nextToken);
		}
	}

	// load the checkout collision volume
	std::string szCheckout = "CheckoutVolume";
	CObject* pCheckout = CObjectManager::GetInstance()->CreateObject(szCheckout);
	//CObject* pCheckout = MMNEW(CObject) CObject(szCheckout);
	
	// find the min and max for the mesh for the checkout volume to use for the collision volume
	int nMeshIndex = ModelManager::GetInstance()->GetMeshIndexByName("FFP_3D_CheckoutCollision_FIN");
	DXMesh* colMesh = ModelManager::GetInstance()->GetMesh(nMeshIndex);
	

	TAABB pCHAABB;
	pCHAABB.cBoxMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pCHAABB.cBoxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	TMeshVertexInfo* pOriginalVerts = ModelManager::GetInstance()->GetCloneVerts(colMesh->GetName());

	
	for(unsigned i=0; i<pOriginalVerts->m_vVertices.size(); i++)
	{
		D3DXVECTOR3 curVert = pOriginalVerts->m_vVertices[i];

		if(curVert.x < pCHAABB.cBoxMin.x)
		{
			pCHAABB.cBoxMin.x = curVert.x;
		}
		if(curVert.x > pCHAABB.cBoxMax.x)
		{
			pCHAABB.cBoxMax.x = curVert.x;
		}

		if(curVert.y < pCHAABB.cBoxMin.y)
		{
			pCHAABB.cBoxMin.y = curVert.y;
		}
		if(curVert.y > pCHAABB.cBoxMax.y)
		{
			pCHAABB.cBoxMax.y = curVert.y;
		}

		if(curVert.z < pCHAABB.cBoxMin.z)
		{
			pCHAABB.cBoxMin.z = curVert.z;
		}
		if(curVert.z > pCHAABB.cBoxMax.x)
		{
			pCHAABB.cBoxMax.z = curVert.z;
		}
	}

	// offset to node position
	pCHAABB.cBoxMax.x += m_pCheckoutNode.tWorldMatrix._41;
	pCHAABB.cBoxMin.x += m_pCheckoutNode.tWorldMatrix._41;
	
	pCHAABB.cBoxMax.y += m_pCheckoutNode.tWorldMatrix._42;
	pCHAABB.cBoxMin.y += m_pCheckoutNode.tWorldMatrix._42;
	
	pCHAABB.cBoxMax.z += m_pCheckoutNode.tWorldMatrix._43;
	pCHAABB.cBoxMin.z += m_pCheckoutNode.tWorldMatrix._43;

	// now offset the node to the center of the box
	D3DXVECTOR3 vNewNodePos = (pCHAABB.cBoxMax + pCHAABB.cBoxMin)*.5f; // average the points to get the center
	m_pCheckoutNode.tWorldMatrix._41 = vNewNodePos.x;
	m_pCheckoutNode.tWorldMatrix._42 = vNewNodePos.y;
	m_pCheckoutNode.tWorldMatrix._43 = vNewNodePos.z;

	CCollideable* pcheck = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pCheckout, true, false, OBJCHECKOUT);
	pcheck->SetBVType(BAABB);
	pcheck->SetAABB(pCHAABB);



	CObject* pNonOrientedPallets = CObjectManager::GetInstance()->CreateObject("nonOrientPal");
	TSphere sphur;
	sphur.cPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	sphur.fRadius = 1.5f;
	CCollideable* pNOPal = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pNonOrientedPallets, true, true, OBJFSD);
	pNOPal->SetBVType(BSPHERE);
	pNOPal->SetSphere(sphur);
	pNOPal->GetParent()->GetTransform()->TranslateFrame(D3DXVECTOR3(-50.5, 0, 44));

	CObject* pNonOrientedPallets2 = CObjectManager::GetInstance()->CreateObject("nonOrientPal2");
	TSphere sphur2;
	sphur2.cPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	sphur2.fRadius = 1.5f;
	CCollideable* pNOPal2 = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pNonOrientedPallets2, true, true, OBJFSD);
	pNOPal2->SetBVType(BSPHERE);
	pNOPal2->SetSphere(sphur2);
	pNOPal2->GetParent()->GetTransform()->TranslateFrame(D3DXVECTOR3(46, 0, 47));
}