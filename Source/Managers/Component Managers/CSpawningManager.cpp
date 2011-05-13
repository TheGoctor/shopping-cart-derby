/*******************************************************************************
 * Filename:  		CSpawningManager.cpp
 * Date:      		05/04/2011
 * Mod. Date: 		05/09/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This manager will be used to
 *					spawn items on the level and
 *					cycle through them.
 ******************************************************************************/

#include "CSpawningManager.h"
#include "..\Global Managers\Event Manager\EventStructs.h"
#include "..\Global Managers\Rendering Managers\Renderer.h"
#include "..\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Components\rendering\CRenderComponent.h"
#include "..\..\Components\Level\CDepartment.h"
using namespace EventStructs;

//Sound Hack//
#include "..\Global Managers\Sound Manager\CWwiseSoundManager.h"

CSpawningManager::CSpawningManager() : m_fGoalItemWaveTime(10.0f), 
								m_fHeldItemWaveTime(10.0f),
								m_nMaxHeldItems(0)
{
}

void CSpawningManager::Init()
{
	CEventManager* pEM = CEventManager::GetInstance();
	
	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;

	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), Update);
	pEM->RegisterEvent("Shutdown", (IComponent*)GetInstance(), Shutdown);
	pEM->RegisterEvent("Despawned", (IComponent*)GetInstance(), Despawned);
	pEM->RegisterEvent("GoalItemCollected", (IComponent*)GetInstance(), 
		GoalItemCollected);
	pEM->RegisterEvent("DestroyObject", (IComponent*)GetInstance(), 
		DestroyObject);

	szEvent = "EnableObjects";
	szEvent += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), HandleEnableObjects);

	szEvent = "DisableObjects";
	szEvent += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), HandleDisableObjects);
	
	szEvent = "InitObjects";
	szEvent += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), HandleInitObjects);


	m_fGoalItemSpawnTimer = 4.0f;
	m_fHeldItemSpawnTimer = m_fHeldItemWaveTime;

	InitGoalItems();
	InitHeldItems();
}

void CSpawningManager::InitGoalItems()
{
	for(int nIndex = 0; nIndex < MAX_DEPARTMENTS; ++nIndex)
	{
		std::string szID;
		//char chCounter[32];
		//_itoa_s(nIndex, chCounter, 32, 10);
		szID = "Department ";
		szID += nIndex + '0';

		CObject* pObj = CObjectManager::GetInstance()->CreateObject(szID);
		CreateDepartmentComponent(pObj, static_cast<EDepartment>(nIndex));
	}
}

void CSpawningManager::DestroyObject(IEvent* pcEvent, IComponent*)
{
	TObjectEvent* pcObj = (TObjectEvent*)pcEvent->GetData();

	GetInstance()->m_cGoalItems.erase(GetInstance()->m_cGoalItems.find(
		GetInstance()->GetGoalItemType(pcObj->m_pcObj)));
}

void CSpawningManager::InitHeldItems()
{
	// **********************************************************//
	// NOTE: this is unnecessary. Just use rand with the enum to //
	// get the same results										 //
	// **********************************************************//

	//static unsigned nCounter = NO_HELD_ITEM;
	//for(unsigned nIndex = 0; nIndex < MAX_HELD_ITEMS; ++nIndex)
	//{
	//	CHeldItemComponent cNewHeldItem;
	//	EHeldItemType eType = static_cast<EHeldItemType>(++nCounter);
	//	cNewHeldItem.Init(eType);

	//	m_cHeldItemsPool.insert(make_pair(eType, cNewHeldItem));
	//}

	// ************************ END NOTE *********************** //

	m_fHeldItemSpawnTimer = m_fHeldItemWaveTime;
	m_nTotalSpawnedHeldItems = 0;

	// NOTE: Maybe set up to do file I/O either with a binary file or lua
	// HACK :
	TSpawnLocation tSpawnLocation[6];
	tSpawnLocation[0].m_tPosition = D3DXVECTOR3(-14.404f, 0.75f, -14.797f);
	tSpawnLocation[0].m_bUsed = false;
	tSpawnLocation[1].m_tPosition = D3DXVECTOR3(-6.435f, 0.75f, -14.939f);
	tSpawnLocation[1].m_bUsed = false;
	tSpawnLocation[2].m_tPosition = D3DXVECTOR3(-10.343f, 0.75f, 9.105f);
	tSpawnLocation[2].m_bUsed = false;
	tSpawnLocation[3].m_tPosition = D3DXVECTOR3(-37.366f, 0.75f, -9.125f);
	tSpawnLocation[3].m_bUsed = false;
	tSpawnLocation[4].m_tPosition = D3DXVECTOR3(-41.504f, 0.75f, -14.901f);
	tSpawnLocation[4].m_bUsed = false;
	tSpawnLocation[5].m_tPosition = D3DXVECTOR3(18.780f, 0.75f, -14.594f);
	tSpawnLocation[5].m_bUsed = false;
	
	for(unsigned nIndex = 0; nIndex < 6; ++nIndex)
	{
		m_cHeldItemLocations.push_back(tSpawnLocation[nIndex]);
	}
	//
}

CSpawningManager::~CSpawningManager()
{
}

void CSpawningManager::Shutdown(IEvent*, IComponent*)
{
	//map<unsigned int, CHeldItemComponent*, less<unsigned int>,
	//CAllocator<pair<unsigned int, CHeldItemComponent*>>>::iterator pcDelete;
	CSpawningManager* pSM = GetInstance();
	//pcDelete = pSM->m_cSpawnedHeldItems.begin();

	while(pSM->m_cSpawnedHeldItems.empty() == false) //pcDelete != GetInstance()->m_cSpawnedHeldItems.end())
	{
		if(pSM->m_cSpawnedHeldItems.begin()->second)
		{
			MMDEL(pSM->m_cSpawnedHeldItems.begin()->second);
		}
		//++pcDelete;
		pSM->m_cSpawnedHeldItems.erase(pSM->m_cSpawnedHeldItems.begin());
	}

	CEventManager::GetInstance()->UnregisterEvent(CIDGen::GetInstance()->
		GetID("Despawned"), (IComponent*)GetInstance());
}

int CSpawningManager::CreateGoalItemComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -2);
	EGoalItemType eGoalItemType = (EGoalItemType)((int)lua_tonumber(pLua, -1));
	CGoalItems* pLM = CreateGoalItemComponent(pObj, eGoalItemType);

	lua_pop(pLua, 2);

	return 0;
}

CGoalItems* CSpawningManager::CreateGoalItemComponent(CObject* pObj, 
													  EGoalItemType eType)
{
	CGoalItems* comp = MMNEW(CGoalItems(pObj));

	comp->Init(eType);
	pObj->AddComponent(comp);

	CSpawningManager::GetInstance()->m_cGoalItems.insert(make_pair(
		(unsigned int)eType, comp));

	// Send event
	SendGoalItemEvent("GoalItemSpawned", (IComponent*)GetInstance(), eType, 
		comp);

	return comp;
}

int CSpawningManager::CreateDepartmentComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -2);
	EDepartment eDeptType = static_cast<EDepartment>(lua_tointeger(pLua, -1));
	CDepartment* pLM = CreateDepartmentComponent(pObj, eDeptType);

	lua_pop(pLua, 2);

	return 0;
}

CDepartment* CSpawningManager::CreateDepartmentComponent(CObject* pObj, 
														 EDepartment eType)
{
	CDepartment* comp = MMNEW(CDepartment(pObj));

	comp->Init(eType);
	pObj->AddComponent(comp);

	CSpawningManager::GetInstance()->m_cDepartments.insert(make_pair(eType, 
		comp));

	return comp;
}

int CSpawningManager::CreateHeldItemComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -2);
	EHeldItemType eHeldItemType = (EHeldItemType)((int)lua_tonumber(pLua, -1));
	CHeldItemComponent* pLM = CreateHeldItemComponent(pObj, eHeldItemType);

	lua_pop(pLua, 2);

	return 0;
}

CHeldItemComponent* CSpawningManager::CreateHeldItemComponent(CObject* pObj, 
															EHeldItemType eType)
{
	CHeldItemComponent* comp = MMNEW(CHeldItemComponent(pObj));

	comp->Init(eType);
	pObj->AddComponent(comp);

	GetInstance()->m_cHeldItems.insert(make_pair((unsigned int)
		pObj->GetID(), comp));

	// Push into list
	GetInstance()->m_cSpawnedHeldItems.insert(make_pair(pObj->GetID(), comp));

	SendHeldItemEvent("HeldItemSpawned", (IComponent*)GetInstance(), 
		comp->GetType(), comp);

	return comp;
}

void CSpawningManager::GoalItemCollected(IEvent* iEvent, IComponent* iComponent)
{
	// SOUND HACK
	EObjectID eObjectID = EObjectID::BIKER_ID;
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_PICK_UP, GLOBAL_ID);

	//TGoalItemCollectedEvent* tEvent = (TGoalItemCollectedEvent*)iEvent;
	//CGoalItems* pGoalItem = (CGoalItems*)tEvent->m_pcGoalItem;
	//pGoalItem->Despawn();

	//GetInstance()->Despawned(iEvent, iComponent);

	TGoalItemCollectedEvent* tEvent = (TGoalItemCollectedEvent*)iEvent->GetData();

	CObject* pcObj = (CObject*)tEvent->m_pcGoalItem;

	pcObj->GetTransform()->TranslateFrame(D3DXVECTOR3(65535,65535,65535));

// HACK
	return;















	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>>::iterator pcGoalIter;
	pcGoalIter = GetInstance()->m_cGoalItems.begin();

	// TODO: Error checking if you don't want the game to crash
	while(pcObj->GetID() != pcGoalIter->second->GetParent()->GetID())
	{
		++pcGoalIter;
	}

	map<EDepartment, CDepartment*, less<unsigned int>,
		CAllocator<pair<EDepartment, CDepartment*>>>::iterator pcDeptIter;
	pcDeptIter = GetInstance()->m_cDepartments.find((EDepartment)pcGoalIter->
		second->m_eType);

	pcDeptIter->second->Despawn();

	// TODO && HACK : OBJECT SHOULD BE DESTROYED SO NO TRANSLATE IS NEEDED
	pcGoalIter->second->GetParent()->GetTransform()->TranslateFrame(D3DXVECTOR3(-65535,-65535,-65535));
	//

	pcGoalIter->second->Despawn();

	//TGoalItemCollectedEvent* tEvent = (TGoalItemCollectedEvent*)iEvent->GetData();
	//TGoalItemEvent* tEvent = (TGoalItemEvent*)iEvent->GetData();

	//CObject* pcObj = (CObject*)tEvent->m_pcGoalItem;
	//CGoalItems* pcGoalItem = (CGoalItems*)tEvent->m_pcGoalItem;

	//TGoalItemCollectedEvent* pCollectedGoalItem = (TGoalItemCollectedEvent*)iEvent->GetData();
	//CSpawningManager* CSpawningManager = CSpawningManager::GetInstance();

	//list<CGoalItems*, CAllocator<CGoalItems*>>::iterator cFindItem;
	//cFindItem = CSpawningManager->m_cDespawningGoalItems.begin();

	//bool bFound = false;

	//while(cFindItem != CSpawningManager->m_cDespawningGoalItems.end())
	//{
	//	if((*cFindItem)->m_pObject->GetID() == pCollectedGoalItem->m_pcGoalItem->GetID())
	//	{
	//		//D3DXMatrixTranslation(&(*cFindItem)->m_pObject->
	//		//	GetTransform()->GetLocalMatrix(), 65535, 65535, 65535);

	//		list<TSpawnLocation, CAllocator<TSpawnLocation>>::iterator pcSpawnPos;
	//		pcSpawnPos = CSpawningManager->m_cGoalItemLocations.begin();

	//		while(pcSpawnPos != CSpawningManager->m_cGoalItemLocations.end())
	//		{
	//			if((*pcSpawnPos).m_tPosition == 
	//				(*cFindItem)->m_pObject->GetTransform()->GetWorldPosition())
	//			{
	//				D3DXMatrixTranslation(&(*cFindItem)->m_pObject->GetTransform()->
	//					GetLocalMatrix(), 65535, 65535, 65535);

	//				(*pcSpawnPos).m_bUsed = false;
	//				break;
	//			}
	//			++pcSpawnPos;
	//		}

	//		CSpawningManager->m_cGoalItemsPool.push_back(*cFindItem);

	//		CSpawningManager->m_cDespawningGoalItems.remove(*cFindItem);
	//		bFound = true;

	//		break;
	//	}
	//	cFindItem++;
	//}

	//if(bFound == false)
	//{
	//	cFindItem = CSpawningManager->m_cSpawnedGoalItems.begin();

	//	while(cFindItem != CSpawningManager->m_cSpawnedGoalItems.end())
	//	{
	//		if((*cFindItem)->m_pObject->GetID() == pCollectedGoalItem->m_pcGoalItem->GetID())
	//		{
	//			//D3DXMatrixTranslation(&(*cFindItem)->m_pObject->
	//			//GetTransform()->GetLocalMatrix(), 65535, 65535, 65535);

	//			list<TSpawnLocation, CAllocator<TSpawnLocation>>::iterator pcSpawnPos;
	//			pcSpawnPos = CSpawningManager->m_cGoalItemLocations.begin();

	//			while(pcSpawnPos != CSpawningManager->m_cGoalItemLocations.end())
	//			{
	//				if((*pcSpawnPos).m_tPosition == 
	//					(*cFindItem)->m_pObject->GetTransform()->GetWorldPosition())
	//				{
	//					D3DXMatrixTranslation(&(*cFindItem)->m_pObject->GetTransform()->
	//						GetLocalMatrix(), 65535, 65535, 65535);

	//					(*pcSpawnPos).m_bUsed = false;
	//					break;
	//				}
	//				++pcSpawnPos;
	//			}

	//			CSpawningManager->m_cGoalItemsPool.push_back(*cFindItem);

	//			CSpawningManager->m_cSpawnedGoalItems.remove(*cFindItem);

	//			break;
	//		}
	//		cFindItem++;
	//	}
	//}

	//list<CGoalItems*, CAllocator<CGoalItems*>>::iterator ItemIter = 
	//	CSpawningManager->m_cGoalItemsPool.begin();

	//while(ItemIter != CSpawningManager->m_cGoalItemsPool.end())
	//{
	//	(*ItemIter)->m_bSpawned = false;
	//	(*ItemIter)->m_bDespawning = false;
	//	(*ItemIter)->m_fDespawnTimer = 10.0f;

	//	ItemIter++;
	//}
}

void CSpawningManager::Update(IEvent* iEvent, IComponent*)
{
	TUpdateStateEvent* pcUpdateState = (TUpdateStateEvent*)iEvent->GetData();
	CSpawningManager::GetInstance()->UpdateTimer(pcUpdateState->m_fDeltaTime);
}

void CSpawningManager::UpdateTimer(const float fDT)
{
	m_fGoalItemSpawnTimer -= fDT;
	//m_fHeldItemSpawnTimer -= fDT;
	unsigned AddToSet = CIDGen::GetInstance()->GetID("AddToSet");

	//list<CGoalItems*, CAllocator<CGoalItems*>>::iterator pcItemIter;

	// Add GoalItems to Set
	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>>::iterator pcIter;
	pcIter = m_cGoalItems.begin();
	//for(size_t nIndex = 0; nIndex < m_cGoalItems.size(); ++nIndex)
	while(pcIter != m_cGoalItems.end())
	{
		//SendRenderEvent("AddToSet", NULL, m_cGoalItems[nIndex]->m_pObject);
		//SendRenderEvent("AddToSet", NULL, pcIter->second->m_pObject);
		++pcIter;
		//TRenderEvent* newevent = MMNEWEVENT(TRenderEvent)
		//	TRenderEvent((*pcItemIter)->m_pObject);
		//IEvent* pEvent = MMNEWEVENT(IEvent(AddToSet, NULL, newevent));
		//CEventManager::GetInstance()->PostEvent(pEvent, PRIORITY_RENDER);
	}
	//pcItemIter = m_cSpawnedGoalItems.begin();
	//while(pcItemIter != m_cSpawnedGoalItems.end())
	//{
	//	TRenderEvent* newevent = MMNEWEVENT(TRenderEvent)
	//		TRenderEvent((*pcItemIter)->m_pObject);
	//	IEvent* pEvent = MMNEWEVENT(IEvent(AddToSet, NULL, newevent));
	//	CEventManager::GetInstance()->PostEvent(pEvent, PRIORITY_RENDER);

	//	pcItemIter++;
	//}

	//static bool bBlink = true;
	//if(bBlink)
	//{
	//pcItemIter = m_cDespawningGoalItems.begin();
	//while(pcItemIter != m_cDespawningGoalItems.end())
	//{
	//	TRenderEvent* newevent = MMNEWEVENT(TRenderEvent)
	//		TRenderEvent((*pcItemIter)->m_pObject);
	//	IEvent* pEvent = MMNEWEVENT(IEvent(AddToSet, NULL, newevent));
	//	CEventManager::GetInstance()->PostEvent(pEvent, PRIORITY_RENDER);

	//	pcItemIter++;
	//}
	//}
	//bBlink = !bBlink;

	// Add HeldItems to Set
	//list<CHeldItemComponent*, CAllocator<CHeldItemComponent*>>
	//	::iterator pcHeldIter = m_cSpawnedHeldItems.begin();
	//while(pcHeldIter != m_cSpawnedHeldItems.end())
	//{
	//	TRenderEvent* newevent = MMNEWEVENT(TRenderEvent)
	//		TRenderEvent((*pcHeldIter)->GetObject());
	//	IEvent* pEvent = MMNEWEVENT(IEvent(AddToSet, NULL, newevent));
	//	CEventManager::GetInstance()->PostEvent(pEvent, PRIORITY_RENDER);

	//	pcHeldIter++;
	//}

	// Spawn more if needed
	if(m_fGoalItemSpawnTimer <= 0)
	{
		SpawnGoalItem();
		m_fGoalItemSpawnTimer = m_fGoalItemWaveTime;
	}
	
	//if(m_fHeldItemSpawnTimer <= 0)
	//{
	//	SpawnHeldItem();
	//	m_fHeldItemSpawnTimer = m_fHeldItemWaveTime;
	//}
}

void CSpawningManager::Despawned(IEvent* iEvent, IComponent*)
{
	TGoalItemEvent* tEvent = (TGoalItemEvent*)iEvent->GetData();

	CGoalItems* pcGoalItem = (CGoalItems*)tEvent->m_pcGoalItem;


	map<EDepartment, CDepartment*, less<unsigned int>,
		CAllocator<pair<EDepartment, CDepartment*>>>::iterator pcIter = 
		GetInstance()->m_cDepartments.find((EDepartment)pcGoalItem->m_eType);

	pcIter->second->Despawn();
}

void CSpawningManager::SpawnGoalItem()
{
	//int nLastSpawn = -1;
	for(unsigned nSpawnItem = 0; nSpawnItem < 2; ++nSpawnItem)
	{
		int nLocation = rand() % MAX_DEPARTMENTS;

		int debugCounter = 0;
		while((m_cDepartments[static_cast<EDepartment>(nLocation)]->IsSpawned()
			== true))// || (nLocation == nLastSpawn))
		{
			nLocation = rand() % MAX_DEPARTMENTS;
			
			++debugCounter;
			if(debugCounter > 1000)
			{
				// Error in CSpawningManager
				// You should probably tell me
				throw;
			}
		}

		//nLastSpawn = nLocation;

		m_cDepartments[static_cast<EDepartment>(nLocation)]->SpawnGoalItem();
		//SendIEvent("SpawnGoalItem", NULL, 
		//	(void*)m_cDepartments[static_cast<EDepartment>(nLocation)], PRIORITY_NORMAL); 
	}
	///Sound Hack
	EObjectID eObjectID = EObjectID::BIKER_ID;
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_SPAWN, GLOBAL_ID);
}

void CSpawningManager::SpawnHeldItem()
{
	// HACK: because I don't want to spawn held items right now
	return;

	map<EHeldItemType, CHeldItemComponent, less<unsigned int>,
	CAllocator<pair<unsigned int, CHeldItemComponent*>>>::iterator pcItemIter;
	pcItemIter = m_cHeldItemsPool.begin();

	EHeldItemType nItem = (EHeldItemType)(rand() % MAX_HELD_ITEMS);
	pcItemIter->second = m_cHeldItemsPool[nItem];

	std::string szID;
	static int ID = -1;
	char chCounter[32];
	_itoa_s(++ID, chCounter, 32, 10);
	szID = "HeldItem ";
	szID += chCounter;

	list<TSpawnLocation, CAllocator<TSpawnLocation>>::iterator pcSpawnPos;

	while(m_nTotalSpawnedHeldItems < m_nMaxHeldItems)
	{
		CHeldItemComponent* cNewHeldItem = new CHeldItemComponent(
			CObjectManager::GetInstance()->CreateObject(szID));

		CreateHeldItemComponent(cNewHeldItem->GetParent(), pcItemIter->second.GetType());
		Renderer::GetInstance()->CreateRenderComp(cNewHeldItem->GetParent(), 0, 0, 1);
		// TODO : ADD COLLISION COMPONENT

		//cNewHeldItem->Init();
		cNewHeldItem->Spawn();

		pcSpawnPos = m_cHeldItemLocations.begin();
		while(pcSpawnPos != m_cHeldItemLocations.end())
		{
			if((*pcSpawnPos).m_bUsed == false)
			{
				D3DXMatrixTranslation(&(cNewHeldItem->GetParent()->GetTransform()->GetLocalMatrix()),
					(*pcSpawnPos).m_tPosition.x, (*pcSpawnPos).m_tPosition.y, 
					(*pcSpawnPos).m_tPosition.z);

				(*pcSpawnPos).m_bUsed = true;
				break;
			}
			pcSpawnPos++;
		}

		m_cSpawnedHeldItems.insert(make_pair(cNewHeldItem->GetParent()->GetID(), cNewHeldItem));

		++pcItemIter;
		++m_nTotalSpawnedHeldItems;

		if(pcItemIter == m_cHeldItemsPool.end())
		{
			pcItemIter = m_cHeldItemsPool.begin();
		}
	}
}

void CSpawningManager::DespawnGoalItem(const CGoalItems& pcGoalItem)
{
	//list<CGoalItems*, CAllocator<CGoalItems*>>::iterator cFindItem;
	//cFindItem = m_cDespawningGoalItems.begin();

	//while(cFindItem != m_cDespawningGoalItems.end())
	//{
	//	if(*cFindItem == pcGoalItem)
	//	{
	//		//D3DXMatrixTranslation(&(*m_cDespawningGoalItems.begin())->m_pObject->
	//		//	GetTransform()->GetLocalMatrix(), 65535, 65535, 65535);

	//		list<TSpawnLocation, CAllocator<TSpawnLocation>>::iterator pcSpawnPos;
	//		pcSpawnPos = m_cGoalItemLocations.begin();

	//		while(pcSpawnPos != m_cGoalItemLocations.end())
	//		{
	//			if((*pcSpawnPos).m_tPosition == 
	//				(*cFindItem)->m_pObject->GetTransform()->GetWorldPosition())
	//			{
	//				D3DXMatrixTranslation(&(*m_cDespawningGoalItems.begin())->m_pObject->
	//					GetTransform()->GetLocalMatrix(), 65535, 65535, 65535);

	//				(*pcSpawnPos).m_bUsed = false;
	//				break;
	//			}
	//			++pcSpawnPos;
	//		}

	//		m_cGoalItemsPool.push_back(*cFindItem);

	//		SendGoalItemEvent("GoalItemDespawned", NULL, (*cFindItem)->m_eType,
	//			*cFindItem);

	//		m_cDespawningGoalItems.remove(*cFindItem);

	//		break;
	//	}

	//	cFindItem++;
	//}

	//list<CGoalItems*, CAllocator<CGoalItems*>>::iterator ItemIter = m_cGoalItemsPool.begin();
	//while(ItemIter != m_cGoalItemsPool.end())
	//{
	//	(*ItemIter)->m_bSpawned = false;
	//	(*ItemIter)->m_bDespawning = false;
	//	(*ItemIter)->m_fDespawnTimer = 10.0f;

	//	ItemIter++;
	//}
}

EGoalItemType CSpawningManager::GetGoalItemType(CObject* pcObject)
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


void CSpawningManager::HandleEnableObjects(IEvent*, IComponent*)
{

}

void CSpawningManager::HandleDisableObjects(IEvent*, IComponent*)
{

}

void CSpawningManager::HandleInitObjects(IEvent*, IComponent*)
{

}
