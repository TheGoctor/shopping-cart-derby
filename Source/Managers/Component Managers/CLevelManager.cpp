#include "CLevelManager.h"

#include "..\Global Managers\Event Manager\CEventManager.h"
#include "..\Global Managers\Event Manager\CObjectEvent.h"
#include "..\Global Managers\Event Manager\CGoalItemEvent.h"
#include "..\Global Managers\Event Manager\CUpdateStateEvent.h"
#include "..\Global Managers\Event Manager\CGoalItemCollected.h"
#include "..\Global Managers\Event Manager\CRenderEvent.h"
#include "..\Global Managers\Event Manager\CIDGen.h"
#include "..\Global Managers\Event Manager\IEvent.h"
#include "..\..\Components\Level\CGoalItemComponent.h"
#include "..\Global Managers\Memory Manager\CMemoryManager.h"
#include "..\Global Managers\Rendering Managers\Renderer.h"
#include "..\Global Managers\Rendering Managers\ModelManager.h"
#include "..\Global Managers\Rendering Managers\dxutil.h"
#include "..\..\CObject.h"
#include "..\Component Managers\CCollisionManager.h"
#include "..\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Components\rendering\CRenderComponent.h"
//Sound Hack//
#include "..\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include <iostream>
#include <fstream>
using namespace std;

CLevelManager::CLevelManager() : m_fWaveTime(7.0f)
{
	m_fSpawnTimer = 0;
}

void CLevelManager::Init()
{
	CEventManager* pEM = CEventManager::GetInstance();
	
	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;

	pEM->RegisterEvent("Shutdown", NULL, Shutdown);
	//pEM->RegisterEvent("UpdateState", NULL, Update);
	pEM->RegisterEvent(szEvent, NULL, Update);
	pEM->RegisterEvent("Despawned", NULL, Despawned);
	pEM->RegisterEvent("GoalItemCollected", NULL, GoalItemCollected);

	BuildLevel("Resource/Node Map/SCD_NodeMap.nm");

	unsigned nCounter = 0;
	m_szGoalItem = "GoalItem";

	for(unsigned GoalItem = 0; GoalItem < 8; ++GoalItem)
	{
		// TODO : REMOVE THIS
		// HACK
//Hack:
		static EGoalItemType eGoalItemType = EGoalItemType(int(NO_ITEM));
		eGoalItemType = EGoalItemType(int(eGoalItemType) + 1);
		if(eGoalItemType == MAX_ITEMS)
		{
			eGoalItemType = NO_ITEM;
			eGoalItemType = EGoalItemType(int(eGoalItemType) + 1);
		}
		// /HACK /TODO
		char chCounter[32];
		_itoa_s(eGoalItemType, chCounter, 32, 10);
		m_szGoalItem = "GoalItem ";
		m_szGoalItem += chCounter;

		//if((stricmp(m_szGoalItem.c_str(), "GoalItem 4") != 0) ||
		//	(stricmp(m_szGoalItem.c_str(), "GoalItem 8") != 0))
		//	goto Hack;

		// Create object
		CObject* pNewObject = MMNEW(CObject) CObject(m_szGoalItem);

		// Add GoalItemComponent
		CreateGoalItemComponent(pNewObject, eGoalItemType);

		// Set Position
		D3DXVECTOR3 cPos;
		cPos.x = 65535;
		cPos.y = 0;
		cPos.z = 0;
		pNewObject->GetTransform()->TranslateFrame(cPos);

		// Set Render/Collision
		Renderer::GetInstance()->CreateRenderComp(pNewObject, 2, 0, 1);
		TSphere tsphere;
		tsphere.cPosition = cPos;
		tsphere.fRadius = 1.0f;
		CCollideable* pCollideable = CCollisionManager::GetInstance()->
			CreateCollideableComponent(pNewObject);
		pCollideable->SetBVType(BSPHERE);
		pCollideable->SetSphere(tsphere);
	}

	m_fSpawnTimer = 0;

}

CLevelManager::~CLevelManager()
{
	CEventManager::GetInstance()->UnregisterEvent(CIDGen::GetInstance()->GetID("Despawned"), NULL);
}

void CLevelManager::Shutdown(IEvent* IDT, IComponent*)
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
			fin.read((char*)&m_levelNodes[i].tName, strlen);
			for(unsigned int matloop = 0; matloop < 16; ++matloop)
			{
				// local
				fin.read((char*)&matrixbuffer, sizeof(double));
				m_levelNodes[i].tLocalMatrix.m[matloop / 4][matloop % 4] = (float)matrixbuffer;
			}
			for(unsigned int matloop = 0; matloop < 16; ++matloop)
			{
				// world
				fin.read((char*)&matrixbuffer, sizeof(double));
				m_levelNodes[i].tWorldMatrix.m[matloop / 4][matloop % 4] = (float)matrixbuffer;
			}
		}

	}
	fin.close();
	fin.clear();


	// ADD Game Objects based on the loaded in nodes
	D3DXVECTOR3 vbuff;
	ModelManager* pMM = ModelManager::GetInstance();
	CObjectManager* pOM = CObjectManager::GetInstance();
	Renderer* pRenMan = Renderer::GetInstance(); 
	CRenderComponent* pRC;
	CObject* pObj;
	for(int i = 0; i < m_objectcount-1; ++i)
	{
		vbuff.x = m_levelNodes[i].tWorldMatrix._41;
		vbuff.y = m_levelNodes[i].tWorldMatrix._42;
		vbuff.z = -m_levelNodes[i].tWorldMatrix._43;

		// Create an object
		string szObjID = "Geo";
		szObjID += i;
		pObj = pOM->CreateObject(szObjID);
		
		int meshindex = pMM->GetMeshIndexByName(m_levelNodes[i].tName);
		
		if(meshindex < 0)
		{
			continue;
		}
		
		int rcidx = 1, rfunc = 2;
		if(meshindex == 4)
		{
			rcidx = 1;
			rfunc = 2;
		}

		pObj->GetTransform()->TranslateFrame(vbuff);
		pRC = pRenMan->CreateRenderComp(pObj,
			meshindex, 
			0, 1);

		// Save it for Renderering
		m_cLevelRenderComps.push_back(pRC);
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

int CLevelManager::CreateGoalItemComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, 1);
	EGoalItemType eGoalItemType = (EGoalItemType)((int)lua_tonumber(pLua, 2));
	CGoalItems* pLM = CreateGoalItemComponent(pObj, eGoalItemType);

	lua_pop(pLua, 2);

	return 0;
}

CGoalItems* CLevelManager::CreateGoalItemComponent(CObject* pObj, EGoalItemType eType)
{
	CGoalItems* comp = MMNEW(CGoalItems) CGoalItems(pObj);

	comp->Init(eType);
	pObj->AddComponent(comp);

	CLevelManager::GetInstance()->m_cGoalItems.insert(make_pair((unsigned int)
		pObj->GetID(), comp));

	// Push into list
	m_cGoalItemsPool.push_back(comp);

	CGoalItemEvent* pcGoalItem = MMNEWEVENT(CGoalItemEvent) CGoalItemEvent(CIDGen::GetInstance()->
		GetID("GoalItemInit"), NULL, comp, comp->m_eType);
	CEventManager::GetInstance()->PostEvent(pcGoalItem, PRIORITY_NORMAL);

	return comp;
}

void SetDespawnTimer(const float fDespawnTime)
{
}

void CLevelManager::GoalItemCollected(IEvent* iEvent, IComponent*)
{
	CWwiseSoundManager::GetInstance()->PlaySound(PICK_UP, 1);

	CGoalItemCollectedEvent* pCollectedGoalItem = (CGoalItemCollectedEvent*)iEvent;

	list<CGoalItems*, CAllocator<CGoalItems*>>::iterator cFindItem;
	cFindItem = CLevelManager::GetInstance()->m_cDespawningGoalItems.begin();

	bool bFound = false;

	while(cFindItem != CLevelManager::GetInstance()->m_cDespawningGoalItems.end())
	{
		if((*cFindItem)->m_pObject->GetID() == pCollectedGoalItem->GetGoalItem()->GetID())
		{
			D3DXMatrixTranslation(&(*cFindItem)->m_pObject->
				GetTransform()->GetLocalMatrix(), 65535, 65535, 65535);

			CLevelManager::GetInstance()->m_cGoalItemsPool.push_back(*cFindItem);

			CLevelManager::GetInstance()->m_cDespawningGoalItems.remove(*cFindItem);
			bFound = true;

			break;
		}
		cFindItem++;
	}

	if(bFound == false)
	{
		cFindItem = CLevelManager::GetInstance()->m_cSpawnedGoalItems.begin();

		while(cFindItem != CLevelManager::GetInstance()->m_cSpawnedGoalItems.end())
		{
			if((*cFindItem)->m_pObject->GetID() == pCollectedGoalItem->GetGoalItem()->GetID())
			{
				D3DXMatrixTranslation(&(*cFindItem)->m_pObject->
				GetTransform()->GetLocalMatrix(), 65535, 65535, 65535);

				CLevelManager::GetInstance()->m_cGoalItemsPool.push_back(*cFindItem);

				CLevelManager::GetInstance()->m_cSpawnedGoalItems.remove(*cFindItem);

				break;
			}
			cFindItem++;
		}
	}

	list<CGoalItems*, CAllocator<CGoalItems*>>::iterator ItemIter = CLevelManager::GetInstance()->m_cGoalItemsPool.begin();
	while(ItemIter != CLevelManager::GetInstance()->m_cGoalItemsPool.end())
	{
		(*ItemIter)->m_bSpawned = false;
		(*ItemIter)->m_bDespawning = false;
		(*ItemIter)->m_fDespawnTimer = 10.0f;

		ItemIter++;
	}
}

void CLevelManager::Update(IEvent* iEvent, IComponent*)
{
	CUpdateStateEvent* pcUpdateState = (CUpdateStateEvent*)iEvent;
	CLevelManager::GetInstance()->UpdateTimer(pcUpdateState->GetDeltaTime());

	std::map<unsigned int, CGoalItems*, less<unsigned int>, CAllocator<pair<unsigned int, CGoalItems*>>>::iterator iter;
	iter = GetInstance()->m_cGoalItems.begin();

	while(iter != GetInstance()->m_cGoalItems.end())
	{
		if(iter->second)
		{
			iter->second->Update(pcUpdateState->GetDeltaTime());
		}
		iter++;
	}
	CCollisionManager::GetInstance()->SearchForCollision();
}

void CLevelManager::UpdateTimer(const float fDT)
{
	m_fSpawnTimer -= fDT;

	// Render the Geometry
	list<CRenderComponent*, CAllocator<CRenderComponent*>>::iterator pcGeoIter;
	pcGeoIter = m_cLevelRenderComps.begin();

	CRenderComponent* pRC;
	while(pcGeoIter != m_cLevelRenderComps.end())
	{
		// Get the Obj
		pRC = (*pcGeoIter);

		CRenderEvent* newevent = MMNEWEVENT(CRenderEvent)
			CRenderEvent(CIDGen::GetInstance()->
			GetID("AddToSet"), NULL, pRC->GetParent());
		CEventManager::GetInstance()->PostEvent(newevent, PRIORITY_RENDER);

		// Inc the Iter
		pcGeoIter++;
		//break;
	}

	// Create Render Event
	list<CGoalItems*, CAllocator<CGoalItems*>>::iterator pcItemIter;

	pcItemIter = m_cSpawnedGoalItems.begin();
	while(pcItemIter != m_cSpawnedGoalItems.end())
	{
		CRenderEvent* pRenderEvent = MMNEWEVENT(CRenderEvent) CRenderEvent(CIDGen::GetInstance()->
			GetID("AddToSet"), NULL, (*pcItemIter)->m_pObject);

		CEventManager::GetInstance()->PostEvent(pRenderEvent, PRIORITY_NORMAL);

		pcItemIter++;
	}

	static bool bBlink = true;

	if(bBlink)
	{
	pcItemIter = m_cDespawningGoalItems.begin();
	while(pcItemIter != m_cDespawningGoalItems.end())
	{
		CRenderEvent* pRenderEvent = MMNEWEVENT(CRenderEvent) CRenderEvent(CIDGen::GetInstance()->
			GetID("AddToSet"), NULL, (*pcItemIter)->m_pObject);

		CEventManager::GetInstance()->PostEvent(pRenderEvent, PRIORITY_NORMAL);

		pcItemIter++;
	}
	}
	bBlink = !bBlink;

	if(m_fSpawnTimer <= 0)
	{
		SpawnGoalItem();
		m_fSpawnTimer = m_fWaveTime;
	}
}

void CLevelManager::Despawned(IEvent* IObject, IComponent*)
{
	CGoalItemEvent* pcGoalItem = (CGoalItemEvent*)IObject;
	CLevelManager::GetInstance()->DespawnGoalItem(pcGoalItem->GetGoalItem());
}

void CLevelManager::SpawnGoalItem()
{
	CEventManager* pEM = CEventManager::GetInstance();

	for(unsigned SpawnItem = 0; SpawnItem < 2; ++SpawnItem)
	{
		if(m_cSpawnedGoalItems.size() > 0)
		{
			m_cDespawningGoalItems.push_back(*m_cSpawnedGoalItems.begin());
			
			CGoalItemEvent* pcGoalItem = MMNEWEVENT(CGoalItemEvent) CGoalItemEvent(CIDGen::GetInstance()->
				GetID("StartDespawnTimer"), NULL, *m_cSpawnedGoalItems.begin(), (*m_cSpawnedGoalItems.begin())->m_eType);
			pEM->PostEvent(pcGoalItem, PRIORITY_NORMAL);

			(*m_cSpawnedGoalItems.begin())->SetDespawnTimer(5.0f);

			m_cSpawnedGoalItems.pop_front();
		}
	}

	for(unsigned SpawnItem = 0; SpawnItem < 2; ++SpawnItem)
	{
		if(m_cGoalItemsPool.size() > 0)
		{			
			D3DXMatrixTranslation(&(*m_cGoalItemsPool.begin())->m_pObject->
				GetTransform()->GetLocalMatrix(), 
				((float)RAND_FLOAT(0, 80.0f) - 40.0f), 0, 
				(float)RAND_FLOAT(0, 60.0f) - 30.0f);

			m_cSpawnedGoalItems.push_back(*m_cGoalItemsPool.begin());

			char chID[11];
			strcpy_s(chID, 11,(char*)((*m_cGoalItemsPool.begin())->m_pObject->GetID()));
			char chEnum = chID[9];
			EGoalItemType eGoalItemType = (EGoalItemType)atoi(&chEnum);

			CGoalItemEvent* pcGoalItem = MMNEWEVENT(CGoalItemEvent) CGoalItemEvent(CIDGen::GetInstance()->
				GetID("GoalItemSpawned"), NULL, (*m_cGoalItemsPool.begin()), (*m_cGoalItemsPool.begin())->m_eType);
			pEM->PostEvent(pcGoalItem, PRIORITY_NORMAL);

			m_cGoalItemsPool.pop_front();
		}
	}

	list<CGoalItems*, CAllocator<CGoalItems*>>::iterator ItemIter = m_cSpawnedGoalItems.begin();
	while(ItemIter != m_cSpawnedGoalItems.end())
	{
		(*ItemIter)->m_bSpawned = true;

		ItemIter++;
	}

	//ItemIter = m_cDespawningGoalItems.begin();
	//while(ItemIter != m_cDespawningGoalItems.end())
	//{
	//	(*ItemIter)->m_bDespawning = true;

	//	ItemIter++;
	//}

	//Sound Hack//
	CWwiseSoundManager::GetInstance()->PlaySound(ITEM_SPAWN , 1);
}

void CLevelManager::DespawnGoalItem(CGoalItems* pcGoalItem)
{
	list<CGoalItems*, CAllocator<CGoalItems*>>::iterator cFindItem;
	cFindItem = m_cDespawningGoalItems.begin();

	while(cFindItem != m_cDespawningGoalItems.end())
	{
		if(*cFindItem == pcGoalItem)
		{
			D3DXMatrixTranslation(&(*m_cDespawningGoalItems.begin())->m_pObject->
				GetTransform()->GetLocalMatrix(), 65535, 65535, 65535);

			m_cGoalItemsPool.push_back(*cFindItem);

			CGoalItemEvent* pcGoalItem = MMNEWEVENT(CGoalItemEvent) CGoalItemEvent(CIDGen::GetInstance()->
				GetID("GoalItemDespawned"), NULL, (*cFindItem), (*cFindItem)->m_eType);
			CEventManager::GetInstance()->PostEvent(pcGoalItem, PRIORITY_NORMAL);

			m_cDespawningGoalItems.remove(*cFindItem);

			break;
		}

		cFindItem++;
	}

	list<CGoalItems*, CAllocator<CGoalItems*>>::iterator ItemIter = m_cGoalItemsPool.begin();
	while(ItemIter != m_cGoalItemsPool.end())
	{
		(*ItemIter)->m_bSpawned = false;
		(*ItemIter)->m_bDespawning = false;
		(*ItemIter)->m_fDespawnTimer = 10.0f;

		ItemIter++;
	}
}

EGoalItemType CLevelManager::GetGoalItemType(CObject* pcObject)
{
	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>>::iterator ItemIter;

	ItemIter = m_cGoalItems.begin();

	while(ItemIter != m_cGoalItems.end())
	{
		if(ItemIter->second->m_pObject->GetID() == pcObject->GetID())
		{
			return ItemIter->second->m_eType;
		}

		ItemIter++;
	}

	return NO_ITEM;
}