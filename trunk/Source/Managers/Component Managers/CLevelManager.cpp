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
	
	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;

	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), Update);
	pEM->RegisterEvent("Shutdown", (IComponent*)GetInstance(), Shutdown);

	BuildLevel("Resource/Node Map/SCD_NodeMap.nm");

	std::string szCheckout = "Checkout";
	CObject* pCheckout = CObjectManager::GetInstance()->CreateObject(szCheckout);
	//CObject* pCheckout = MMNEW(CObject) CObject(szCheckout);
	TAABB pCHAABB;
	pCHAABB.cBoxMax = D3DXVECTOR3(3.5f, 5.0f, -27.0f);
	pCHAABB.cBoxMin = D3DXVECTOR3(-22.0f, -5.0f, -35.0f);
	CCollideable* pcheck = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pCheckout, true, false, OBJCHECKOUT);
	pcheck->SetBVType(BAABB);
	pcheck->SetAABB(pCHAABB);

	// Boundaries on walls
	std::string szWallName1 = "MaxXWall";
	CObject* pMaxXWall = CObjectManager::GetInstance()->CreateObject(szWallName1);
	std::string szWallName2 = "MinXWall";
	CObject* pMinXWall = CObjectManager::GetInstance()->CreateObject(szWallName2);
	std::string szWallName3 = "MaxZWall";
	CObject* pMaxZWall = CObjectManager::GetInstance()->CreateObject(szWallName3);
	std::string szWallName4 = "MinZWall";
	CObject* pMinZWall = CObjectManager::GetInstance()->CreateObject(szWallName4);

	TPlane MaxX, MinX, MaxZ, MinZ;
	MaxX.cPlanePoint = D3DXVECTOR3(48.0f, 0.0f, 0.0f);
	MaxX.cPlaneNormal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);

	MinX.cPlanePoint = D3DXVECTOR3(-48.0f, 0.0f, 0.0f);
	MinX.cPlaneNormal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	MaxZ.cPlanePoint = D3DXVECTOR3(0.0f, 0.0f, 36.0f);
	MaxZ.cPlaneNormal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	MinZ.cPlanePoint = D3DXVECTOR3(0.0f, 0.0f, -36.0f);
	MinZ.cPlaneNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	CCollideable* pxmax = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pMaxXWall, true, true, OBJWALL);
	pxmax->SetBVType(BPLANE);
	pxmax->SetPlane(MaxX);
	CCollideable* pxmin = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pMinXWall, true, true, OBJWALL);
	pxmin->SetBVType(BPLANE);
	pxmin->SetPlane(MinX);
	CCollideable* pzmax = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pMaxZWall, true, true, OBJWALL);
	pzmax->SetBVType(BPLANE);
	pzmax->SetPlane(MaxZ);
	CCollideable* pzmin = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pMinZWall, true, true, OBJWALL);
	pzmin->SetBVType(BPLANE);
	pzmin->SetPlane(MinZ);
}

CLevelManager::~CLevelManager()
{
}

void CLevelManager::AddRenderCollision(CRenderComponent* pRC)
{
	m_cLevelCollisionRenderComps.push_back(pRC);
}

void CLevelManager::Shutdown(IEvent*, IComponent*)
{
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
		m_levelNodes = new CNode[m_objectcount];
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
	for(int i = 0; i < m_objectcount-1; ++i)
	{
		vWorldSpacePos.x = m_levelNodes[i].tWorldMatrix._41;
		vWorldSpacePos.y = m_levelNodes[i].tWorldMatrix._42;
		vWorldSpacePos.z = -m_levelNodes[i].tWorldMatrix._43;

		D3DXMATRIX rot(m_levelNodes[i].tWorldMatrix);
		rot._41 = 0.0f;
		rot._42 = 0.0f;
		rot._43 = 0.0f;
		rot._44 = 0.0f;

		// Create an object
		string szObjID = "Geo";
		szObjID += static_cast<char>(i);
		pObj = pOM->CreateObject(szObjID);

		// Create an object to render collision
		string szColObjID = "ColRen";
		szColObjID += static_cast<char>(i);
		pColObj = pOM->CreateObject(szColObjID, 0.0, 0.0, 0.0, pObj->GetTransform());
		
		int meshindex = pMM->GetMeshIndexByName(m_levelNodes[i].szName);	
		if(meshindex < 0)
		{
			continue;
		}
		int rcidx = meshindex + 1, rfunc = 2;

		m_levelNodes[i].tWorldMatrix._43 *= -1;

		pObj->GetTransform()->GetLocalMatrix() = m_levelNodes[i].tWorldMatrix;

		pRC = pRenMan->CreateRenderComp(pObj,
			meshindex, 
			rcidx, rfunc);

		// Save it for Renderering
		//pRC->Init();
		m_cLevelRenderComps.push_back(pRC);

		//// look for collision
		TAABB aabb;
		TAABB localAABB;
		if(pMM->GetAABBByNameWithOffset(m_levelNodes[i].szName, vWorldSpacePos, rot,aabb, localAABB))
		{
			CCollideable* pCollideable = CCollisionManager::GetInstance()->
				CreateCollideableComponent(pColObj, true, true, OBJSHELF);
			pCollideable->SetBVType(BAABB);
			pCollideable->SetAABB(aabb);
			CRenderComponent* pRender = Renderer::GetInstance()->CreateRenderComp(
				pColObj, ModelManager::GetInstance()->CreateCubeFromAABB(localAABB), 0, RF_INDEXED_VERT);

			//pRender->Init();
			AddRenderCollision(pRender);
		}
	}

	delete [] m_levelNodes;
}

bool CLevelManager::AddCollision(char* szMeshName)
{
	string bvmeshname(szMeshName);
	bvmeshname += "_Collision.mesh";
	//ModelManager::GetInstance()->LoadModel(bvmeshname.c_str());
	
	return false;
}

void CLevelManager::Update(IEvent*, IComponent*)
{
	unsigned AddToSet = CIDGen::GetInstance()->GetID("AddToSet");

	// Render the Geometry
	list<CRenderComponent*, CAllocator<CRenderComponent*>>::iterator pcGeoIter;
	pcGeoIter = CLevelManager::GetInstance()->m_cLevelRenderComps.begin();

	CRenderComponent* pRC;
	while(pcGeoIter != CLevelManager::GetInstance()->m_cLevelRenderComps.end())
	{
		// Get the Obj
		pRC = (*pcGeoIter);

		//TRenderEvent* newevent = MMNEWEVENT(TRenderEvent)
		//	TRenderEvent(pRC->GetParent());
		//IEvent* pEvent = MMNEWEVENT(IEvent) IEvent(AddToSet, NULL, newevent);
		//CEventManager::GetInstance()->PostEvent(pEvent, PRIORITY_RENDER);

		SendRenderEvent("AddToSet", NULL, pRC->GetParent());

		// Inc the Iter
		pcGeoIter++;
	}

	// Render the Collision Geometry
	list<CRenderComponent*, CAllocator<CRenderComponent*>>::iterator pcColIter;
	pcColIter = CLevelManager::GetInstance()->m_cLevelCollisionRenderComps.begin();

	CRenderComponent* pCRC;
	while(pcColIter != CLevelManager::GetInstance()->m_cLevelCollisionRenderComps.end())
	{
		// Get the Obj
		pCRC = (*pcColIter);

		// Don't use this vvv here because it'll be slower
		TRenderEvent* newevent = MMNEWEVENT(TRenderEvent(pCRC->GetParent()));
		IEvent* pEvent = MMNEWEVENT(IEvent(AddToSet, NULL, newevent));
		CEventManager::GetInstance()->PostEvent(pEvent, PRIORITY_RENDER);

		// Inc the Iter
		pcColIter++;
	}
	
	CCollisionManager::GetInstance()->SearchForCollision();
}