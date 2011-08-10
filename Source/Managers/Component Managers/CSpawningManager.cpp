/*******************************************************************************
 * Filename:  		CSpawningManager.cpp
 * Date:      		05/04/2011
 * Mod. Date: 		06/09/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This manager will be used to
 *					spawn items on the level and
 *					cycle through them.
 ******************************************************************************/
#include <fstream>
#include "CSpawningManager.h"
#include "CCollisionManager.h"
#include "CLevelManager.h"
#include "..\..\Components\Level\CDepartment.h"
#include "..\Global Managers\Event Manager\EventStructs.h"
#include "..\Global Managers\Rendering Managers\Renderer.h"
#include "..\Global Managers\Rendering Managers\ModelManager.h"
#include "..\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Components\Level\CHeldItemComponent.h"
#include "..\..\Components\Level\CGoalItemComponent.h"
using namespace EventStructs;

CSpawningManager::CSpawningManager() : m_fGoalItemWaveTime(3.0f),
								m_nMaxHeldItems(15)
{
}

CSpawningManager::~CSpawningManager()
{
}

void CSpawningManager::Init()
{
	CEventManager* pEM = CEventManager::GetInstance();

	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;

	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), Update);
	pEM->RegisterEvent("Shutdown", (IComponent*)GetInstance(), Shutdown);
	pEM->RegisterEvent("GoalItemDespawned", (IComponent*)GetInstance(), Despawned);
	pEM->RegisterEvent("GoalItemCollected", (IComponent*)GetInstance(), 
		GoalItemCollected);
	pEM->RegisterEvent("DestroyObject", (IComponent*)GetInstance(), 
		DestroyObject);
	pEM->RegisterEvent("GoalItemCollision", (IComponent*)GetInstance(), 
		GoalItemCollision);
	pEM->RegisterEvent("OverlayUnshow", (IComponent*)GetInstance(), 
		HandleStartUpdate);
	
	szEvent = "InitObjects";
	szEvent += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), HandleInitObjects);

	// Load models
	// GOAL ITEM MESHES AND RENDER COMPS

	m_szGoalItemNames[GORILLA_MILK] = "FFP_3D_GorillaMilk_FIN";
	m_szGoalItemNames[CAPTAIN_FLAKEY] = "FFP_3D_CaptainFlakey_FIN";
	m_szGoalItemNames[PIZZA] = "FFP_3D_Blue_Bomber_Pizza_FIN";
	m_szGoalItemNames[CAKE] = "FFP_3D_Cake_FIN";
	m_szGoalItemNames[CANNED_BEARS] = "FFP_3D_CannedBears_FIN";
	m_szGoalItemNames[CARROTS] = "FFP_3D_Carrots_FIN";
	m_szGoalItemNames[SAMMY_SANDWICH] = "FFP_3D_SammySandwich_FIN";
	m_szGoalItemNames[SQUID] = "FFP_3D_Squid_FIN";
	m_szGoalItemNames[AGENT_ORANGE] = "FFP_3D_AgentOrange_FIN";


	string szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[GORILLA_MILK] + ".mesh";
	m_nGoalItems[GORILLA_MILK] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());

	szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[CAPTAIN_FLAKEY] + ".mesh";
	m_nGoalItems[CAPTAIN_FLAKEY] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());
	
	szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[PIZZA] + ".mesh";
	m_nGoalItems[PIZZA] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());
	
	szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[CAKE] + ".mesh";
	m_nGoalItems[CAKE] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());
	
	szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[CANNED_BEARS] + ".mesh";
	m_nGoalItems[CANNED_BEARS] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());
	
	szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[CARROTS] + ".mesh";
	m_nGoalItems[CARROTS] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());
	
	szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[SAMMY_SANDWICH] + ".mesh";
	m_nGoalItems[SAMMY_SANDWICH] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());
	
	szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[SQUID] + ".mesh";
	m_nGoalItems[SQUID] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());
	
	szMeshPath = "Resource/Goal Item Meshes/" + m_szGoalItemNames[AGENT_ORANGE] + ".mesh";
	m_nGoalItems[AGENT_ORANGE] = ModelManager::GetInstance()->LoadModel((char*)szMeshPath.c_str());

	m_nGoalItemRenderContexts[GORILLA_MILK] = RC_GORILLA_MILK;
	m_nGoalItemRenderContexts[CAPTAIN_FLAKEY] = RC_CAPTAIN_FLAKEY;
	m_nGoalItemRenderContexts[PIZZA] = RC_PIZZA;
	m_nGoalItemRenderContexts[CAKE] = RC_CAKE;
	m_nGoalItemRenderContexts[CANNED_BEARS] = RC_CANNED_BEARS;
	m_nGoalItemRenderContexts[CARROTS] = RC_CARROTS;
	m_nGoalItemRenderContexts[SAMMY_SANDWICH] = RC_SAMMY_SANDWICH;
	m_nGoalItemRenderContexts[SQUID] = RC_SQUID;
	m_nGoalItemRenderContexts[AGENT_ORANGE] = RC_AGENT_ORANGE;

	/// HELD ITEM MESH AND RENDER CONTEXTS
	ModelManager* pMM = ModelManager::GetInstance();
	m_nHeldItemMeshIDs[BANANA] 		  = pMM->GetMeshIndexByName("Banana_MShape");
	m_nHeldItemMeshIDs[SOUP]   		  = pMM->GetMeshIndexByName("Chicken_NoodleSoup_MShape");
	m_nHeldItemMeshIDs[DONUT]  		  = pMM->GetMeshIndexByName("Donut");
	m_nHeldItemMeshIDs[TURKEY] 		  = pMM->GetMeshIndexByName("FFP_3D_IceTurkeyBox_FIN");
	m_nHeldItemMeshIDs[JAM]	   		  = pMM->GetMeshIndexByName("Jam_MShape");
	m_nHeldItemMeshIDs[PEANUT_BUTTER] = pMM->GetMeshIndexByName("Peanut_butter_MShape");
	m_nHeldItemMeshIDs[PIE]			  = pMM->GetMeshIndexByName("Pie_MShape");
	m_nHeldItemMeshIDs[ENERGY_DRINK]  = pMM->GetMeshIndexByName("Thors_Thunder_MShape");

	m_nHeldItemRenderCompIDs[BANANA] = RC_BANANA;
	m_nHeldItemRenderCompIDs[SOUP] = RC_CHICKENSOUP;
	m_nHeldItemRenderCompIDs[DONUT] = RC_DONUT;
	m_nHeldItemRenderCompIDs[TURKEY] = RC_TURKEYBOX;
	m_nHeldItemRenderCompIDs[JAM] = RC_JAM;
	m_nHeldItemRenderCompIDs[PEANUT_BUTTER] = RC_PEANUTBUTTER;
	m_nHeldItemRenderCompIDs[PIE] = RC_PIE;
	m_nHeldItemRenderCompIDs[ENERGY_DRINK] = RC_THOR;

	m_fGoalItemSpawnTimer = 4.0f;

	InitGoalItems();
	InitHeldItems();

	LoadGoalItemLocations();

	// Pick a location (Department) to disable for this game
	int nLocation = rand() % MAX_DEPARTMENTS;
	m_cDepartments[static_cast<EDepartment>(nLocation)]->Deactivate();
	map<EDepartment, CDepartment*, less<unsigned int>,
		CAllocator<pair<EDepartment, CDepartment*>>>::iterator pIter =
		m_cDepartments.begin();

	int nID = -1;

	// Only acknowledge Departments that weren't picked
	while(pIter != m_cDepartments.end())
	{
		++nID;

		if((pIter->second->IsSpawned() == false) && (nID != nLocation))
		{
			pIter->second->AcknowledgeDepartments(nID);
		}

		++pIter;
	}
}

void CSpawningManager::Shutdown(IEvent*, IComponent*)
{
	// Clear all memory
	CSpawningManager* pSM = GetInstance();

	map<EDepartment, CDepartment*, less<unsigned int>, CAllocator
		<pair<EDepartment, CDepartment*>>>::iterator pDeptIter;
	pDeptIter = pSM->m_cDepartments.begin();
	while(pDeptIter != pSM->m_cDepartments.end())
	{
		MMDEL(pDeptIter->second);
		++pDeptIter;
	}
	pSM->m_cDepartments.clear();

	map<unsigned int, CGoalItems*, less<unsigned int>, CAllocator
		<pair<unsigned int, CGoalItems*>>>::iterator pGoalIter;
	pGoalIter = pSM->m_cGoalItems.begin();
	while(pGoalIter != pSM->m_cGoalItems.end())
	{
		MMDEL(pGoalIter->second);
		++pGoalIter;
	}
	pSM->m_cGoalItems.clear();

	pSM->m_cHeldItemLocations.clear();

	map<unsigned int, CHeldItemComponent*, less<unsigned int>, CAllocator
		<pair<unsigned int, CHeldItemComponent*>>>::iterator pHeldIter;
	pHeldIter = pSM->m_cHeldItems.begin();
	while(pHeldIter != pSM->m_cHeldItems.end())
	{
		MMDEL(pHeldIter->second);
		++pHeldIter;
	}
	pSM->m_cHeldItems.clear();
}

void CSpawningManager::InitGoalItems()
{
	for(int nIndex = 0; nIndex < MAX_DEPARTMENTS; ++nIndex)
	{
		std::string szID;
		szID = "Department ";
		szID += (char)(nIndex + '0');

		CObject* pObj = CObjectManager::GetInstance()->CreateObject(szID);
		CreateDepartmentComponent(pObj, static_cast<EDepartment>(nIndex));
	}
}

void CSpawningManager::InitHeldItems()
{
	m_nTotalSpawnedHeldItems = 0;

	string szID;
	char chCounter[32];
	for(unsigned nIndex = 0; nIndex < m_nMaxHeldItems; ++nIndex)
	{
		_itoa_s(nIndex, chCounter, 32, 10);
		szID = "HeldItem ";
		szID += chCounter;

		CObject* pObj = CObjectManager::GetInstance()->CreateObject(szID, 
			0, 0, 0);
		CHeldItemComponent* pHeldItemComp =  CreateHeldItemComponent(pObj);

		// Set Render/Collision
																			// 34 for spheres
		CRenderComponent* pRenderComp = Renderer::GetInstance()->CreateRenderComp(pObj, 34, 0, 2);
		pHeldItemComp->SetRenderComponent(pRenderComp);

		TSphere tsphere;
		tsphere.cPosition = pObj->GetTransform()->GetWorldPosition();
		tsphere.fRadius = 1.5f;
		CCollideable* pCollideable = CCollisionManager::GetInstance()->
			CreateCollideableComponent(pObj, false, false, OBJHELDITEM);
		pCollideable->SetBVType(BSPHERE);
		pCollideable->SetSphere(tsphere);

		// Scale obj's frame so it's easily seen
		pObj->GetTransform()->ScaleFrame(tsphere.fRadius, tsphere.fRadius,
			tsphere.fRadius);
	}
}

void CSpawningManager::LoadGoalItemLocations()
{
	fstream fin("Resource\\Node Map\\GoalItems.nm", 
		ios_base::in | ios_base::binary);
	int nstrlen = 0;
	int objectcount = 0;
	//double matrixbuffer = 0.0;
	CLevelManager::CNode* levelNodes;

	// LOAD NODES
	if(fin.is_open())
	{
		fin.read((char*)&objectcount, sizeof(int));
		levelNodes = MMNEWARRAYEX(CLevelManager::CNode, objectcount, HEAPID_GENERAL);
		for(int i = 0; i < objectcount; ++i)
		{
			// name length
			fin.read((char*)&nstrlen, sizeof(int));
			// name
			fin.read((char*)&levelNodes[i].szName, nstrlen);
			// worldMat
			fin.read((char*)&levelNodes[i].tWorldMatrix, sizeof(D3DXMATRIX));
		}

		fin.close();
		fin.clear();
	}
	else
	{
		Debug.Print("Can't open GoalItems.nm");
		return;
	}

	// Create strings of wanted GoalItems
	string* pGoalItems = MMNEWARRAYEX(string, objectcount, HEAPID_GENERAL);
	pGoalItems[DAIRY]			= "Dairy";
	pGoalItems[DRY_GOODS]		= "DryGoods";
	pGoalItems[FREEZER]			= "Freezer";
	pGoalItems[BAKERY]			= "Bakery";
	pGoalItems[CANNED_GOODS]	= "CannedGoods";
	pGoalItems[PRODUCE]			= "Produce";
	pGoalItems[DELI]			= "Deli";
	pGoalItems[MEATS]			= "Meats";
	pGoalItems[SNACKS]			= "Snacks";

	// Search for nodes
	char chBuffer[64];
	char chTempBuffer[64];
	int nNameLength;
	int nGoalItem = 0;
	string szID;
	D3DXVECTOR3 cPos;

	for(int nIndex = 0; nIndex < objectcount; ++nIndex)
	{
		memset(chBuffer, 0, 64);
		memset(chTempBuffer, 0, 64);
		nNameLength = strlen(levelNodes[nIndex].szName);

		// copy the current node's name into our buffer
		memcpy(chBuffer, levelNodes[nIndex].szName, nNameLength);

		// init the strtok to deliminate based on _
		char* nextToken = NULL;
		char* szTokenizer = strtok_s(chBuffer, "_", &nextToken);

		// while strtoke doesn't return null (aka it's not at the end of the tokenizing string)
		if(szTokenizer != NULL)
		{	
			for(unsigned nDepartment = 0; nDepartment < MAX_DEPARTMENTS; ++nDepartment)
			{
				// if a non case sensative compare matches what we're looking for
				if(_stricmp(szTokenizer, pGoalItems[nDepartment].c_str()) == 0)
				{
					map<EDepartment, CDepartment*, less<unsigned int>,
						CAllocator<pair<EDepartment, CDepartment*>>>::iterator DepartmentIter =
						m_cDepartments.find(static_cast<EDepartment>(nDepartment));

					if(DepartmentIter == m_cDepartments.end())
					{
						Debug.Print("Error in SpawningManager.cpp 'LoadGoalItemLocations'");
						return;
					}

					TGoalItemSpawnLocation tSpawnLocation;
					tSpawnLocation.m_cPos.x = levelNodes[nIndex].tWorldMatrix._41;
					tSpawnLocation.m_cPos.y = levelNodes[nIndex].tWorldMatrix._42;
					tSpawnLocation.m_cPos.z = levelNodes[nIndex].tWorldMatrix._43;

					DepartmentIter->second->m_cGoalItemSpawnLocations.insert(make_pair(
						DepartmentIter->second->m_cGoalItemSpawnLocations.size(), tSpawnLocation));

					break;
				}
			}
		}
	}

	MMDELARRAYEX(levelNodes, HEAPID_GENERAL);
}

void CSpawningManager::LoadHeldItemLocations()
{
	// Search for endcap nodes
	char chBuffer[64];
	char chTempBuffer[64];
	int nNameLength;
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
			if(_stricmp(szTokenizer, "Endcap") == 0)
			{
				break;
			}

			// assign chTempBuffer to the next token in the string (not a new one because we passed null)
			szTokenizer = strtok_s(NULL, "_", &nextToken);
		}
	}
}

int CSpawningManager::CreateGoalItemComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -2);
	EGoalItemType eGoalItemType = (EGoalItemType)((int)lua_tonumber(pLua, -1));
	CreateGoalItemComponent(pObj, eGoalItemType);

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

	return comp;
}

int CSpawningManager::CreateDepartmentComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -2);
	EDepartment eDeptType = static_cast<EDepartment>(lua_tointeger(pLua, -1));
	CreateDepartmentComponent(pObj, eDeptType);

	lua_pop(pLua, 2);

	return 0;
}

CDepartment* CSpawningManager::CreateDepartmentComponent(CObject* pObj, 
														 EDepartment eType)
{
	CDepartment* comp = MMNEW(CDepartment(pObj));

	comp->Init(eType);
	pObj->AddComponent(comp);

	GetInstance()->m_cDepartments.insert(make_pair(eType, 
		comp));

	return comp;
}

int CSpawningManager::CreateHeldItemComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -1);
	CreateHeldItemComponent(pObj);

	lua_pop(pLua, 1);

	return 0;
}

CHeldItemComponent* CSpawningManager::CreateHeldItemComponent(CObject* pObj)
{
	CHeldItemComponent* comp = MMNEW(CHeldItemComponent(pObj));

	comp->Init();
	pObj->AddComponent(comp);

	// Push into list
	GetInstance()->m_cHeldItems.insert(make_pair(pObj->GetID(), comp));

	return comp;
}

void CSpawningManager::DestroyObject(IEvent* pcEvent, IComponent*)
{
	TObjectEvent* pObjEvent = (TObjectEvent*)pcEvent->GetData();
	EGoalItemType eID = GetInstance()->GetGoalItemType(pObjEvent->m_pcObj);

	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>>::iterator pIter;
	pIter = GetInstance()->m_cGoalItems.find(eID);
	if(pIter != GetInstance()->m_cGoalItems.end())
	{
		GetInstance()->m_cGoalItems.erase(pIter);
	}
}

void CSpawningManager::GoalItemCollected(IEvent* iEvent, IComponent*)
{
	TGoalItemCollectedEvent* tEvent = (TGoalItemCollectedEvent*)iEvent->GetData();

	CObject* pcObj = (CObject*)tEvent->m_pcGoalItem;

	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>>::iterator pcGoalIter;
	pcGoalIter = GetInstance()->m_cGoalItems.begin();

	while(pcObj->GetID() != pcGoalIter->second->GetParent()->GetID())
	{
		++pcGoalIter;
	}

	map<EDepartment, CDepartment*, less<unsigned int>,
		CAllocator<pair<EDepartment, CDepartment*>>>::iterator pcDeptIter;

	// Find Department where Goal Item has been collected
	pcDeptIter = GetInstance()->m_cDepartments.find((EDepartment)pcGoalIter->
		second->GetType());

	// Despawn Department
	pcDeptIter->second->Despawn();
}

void CSpawningManager::Update(IEvent* iEvent, IComponent*)
{
	TUpdateStateEvent* pcUpdateState = (TUpdateStateEvent*)iEvent->GetData();
	CSpawningManager::GetInstance()->UpdateTimer(pcUpdateState->m_fDeltaTime);
}

void CSpawningManager::UpdateTimer(const float fDT)
{
	if(m_bGameHasntStartedYet)
	{
		return;
	}

	m_fGoalItemSpawnTimer -= fDT;

	// Spawn more if needed
	if(m_fGoalItemSpawnTimer <= 0)
	{
		SpawnGoalItem();
		m_fGoalItemSpawnTimer = m_fGoalItemWaveTime;
	}
}

void CSpawningManager::Despawned(IEvent* iEvent, IComponent*)
{
	TGoalItemEvent* tEvent = (TGoalItemEvent*)iEvent->GetData();
	CGoalItems* pcGoalItem = (CGoalItems*)tEvent->m_pcGoalItem;

	// Find the Department
	map<EDepartment, CDepartment*, less<unsigned int>,
		CAllocator<pair<EDepartment, CDepartment*>>>::iterator pcIter = 
		GetInstance()->m_cDepartments.find((EDepartment)pcGoalItem->GetType());

	// Call Department's Despawn function
	pcIter->second->Despawn();
}

void CSpawningManager::SpawnGoalItem()
{
	// Get a random Department
	int nLocation = rand() % MAX_DEPARTMENTS;

	// If that Department already has a spawned Goal Item...
	int debugCounter = 0;
	while((m_cDepartments[static_cast<EDepartment>(nLocation)]->IsSpawned()))
	{
		// ... get a new Department
		nLocation = rand() % MAX_DEPARTMENTS;

		// If this hits 1001 then there's a problem
		++debugCounter;
		if(debugCounter > 1000)
		{
			return;
		}
	}

	// Call SpawnGoalItem on the picked Department
	m_cDepartments[static_cast<EDepartment>(nLocation)]->SpawnGoalItem();
}

EGoalItemType CSpawningManager::GetGoalItemType(CObject* pcObject)
{
	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>>::iterator ItemIter;

	ItemIter = m_cGoalItems.begin();

	while(ItemIter != m_cGoalItems.end())
	{
		if(ItemIter->second->GetParent()->GetID() == pcObject->GetID())
		{
			// Found associated goal item. Return its type
			return ItemIter->second->GetType();
		}

		ItemIter++;
	}

	// didn't find anything
	return NO_ITEM;
}

EHeldItemType CSpawningManager::GetHeldItemType(CObject* pcObject)
{
	map<unsigned int, CHeldItemComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CHeldItemComponent*>>>::iterator ItemIter;

	ItemIter = m_cHeldItems.begin();

	while(ItemIter != m_cHeldItems.end())
	{
		if(ItemIter->second->GetParent()->GetID() == pcObject->GetID())
		{
			// Found associated held item. Return its type
			return ItemIter->second->GetType();
		}

		ItemIter++;
	}

	// didn't find it
	return NO_HELD_ITEM;
}

void CSpawningManager::HandleInitObjects(IEvent*, IComponent*)
{
	GetInstance()->m_fGoalItemSpawnTimer = 6.0f;	
	GetInstance()->m_bGameHasntStartedYet = true;
}

void CSpawningManager::GoalItemCollision(IEvent* iEvent, IComponent*)
{
	TGoalItemCollectedEvent* tEvent = (TGoalItemCollectedEvent*)iEvent->GetData();

	CObject* pGoalItem = tEvent->m_pcGoalItem;
	CObject* pCollector = tEvent->m_pcCollector;
	EGoalItemType eType = GetInstance()->GetGoalItemType(pGoalItem);

	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>>::iterator pIter;
	pIter = GetInstance()->m_cGoalItems.find(eType);

	if((pIter == GetInstance()->m_cGoalItems.end()) || 
		(pIter->second->GetIsCollidable() == false))
	{
		// Either pIter is bad or the item isn't spawned
		return;
	}

	// Tell everyone else
	SendGoalItemCollectedEvent("GoalItemCollected", (IComponent*)GetInstance(),
		pGoalItem, pCollector);
}


int CSpawningManager::GetGoalItemMeshID(int nItemID)
{
	if(nItemID > -1 && nItemID < MAX_GOAL_ITEMS)
	{
		return m_nGoalItems[nItemID];
	}

	return -1;
}

bool CSpawningManager::IsSpawned(EGoalItemType eType)
{
	map<unsigned int, CGoalItems*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CGoalItems*>>>::iterator pIter = 
		m_cGoalItems.find(eType);

	if(pIter == m_cGoalItems.end())
	{
		return false;
	}

	return pIter->second->GetIsSpawned();
}

void CSpawningManager::HandleStartUpdate(IEvent*, IComponent*)
{
	GetInstance()->m_bGameHasntStartedYet = false;
}