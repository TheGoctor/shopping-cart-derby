/*******************************************************************************
 * Filename:  		CDepartment.cpp
 * Date:      		05/01/2011
 * Mod. Date: 		05/09/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		Departments will hold a set amount of Goal Items.
					These Goal Items will be assoicated to the Department
					in terms of type (i.e. milk in the dairy department).

					A Department will list for a SpawnGoalItem event and will
					spawn a random Goal Item within itself.
 ******************************************************************************/

#include "CDepartment.h"
#include "..\..\Managers\Component Managers\CCollisionManager.h"
#include "..\..\Managers\Component Managers\CSpawningManager.h"
#include "..\..\Managers\Global Managers\Rendering Managers\Renderer.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\CObject.h"
#include "..\..\Source\Managers\Component Managers\CLevelManager.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;
#define NULL 0

CDepartment::CDepartment(CObject* CObj) : m_pParent(CObj)
{
}

CDepartment::~CDepartment()
{
}

void CDepartment::Init(EDepartment eDepartment)
{
	// Register for SpawnGoalItem
	CEventManager::GetInstance()->RegisterEvent("SpawnGoalItem", this, Spawn);

	// Setup department
	m_eType = eDepartment;
	m_bUsed = false;
}

void CDepartment::AcknowledgeDepartments(int nID)
{
	TGoalItemSpawnLocation tGoalItemSpawnLocation;
	string szID;
	char chCounter[32];
	D3DXVECTOR3 cPos;

	// Setup spawn location
	//tGoalItemSpawnLocation.m_cPos = GetSpawnPosition(m_eType);
	tGoalItemSpawnLocation.m_bSpawned = false;

	// Setup ID for obj
	_itoa_s(nID, chCounter, 32, 10);
	szID = "GoalItem ";
	szID += chCounter;

	// Create Object
	CObject* pObj = CObjectManager::GetInstance()->CreateObject(szID, 
		tGoalItemSpawnLocation.m_cPos.x, tGoalItemSpawnLocation.m_cPos.y,
		tGoalItemSpawnLocation.m_cPos.z);

	// Create Goal Item
	tGoalItemSpawnLocation.m_cGoalItem = 
		CSpawningManager::GetInstance()->CreateGoalItemComponent(pObj, 
		(EGoalItemType)(nID));

	// Insert Goal Item into Department's map
	m_cGoalItemSpawnLocations.insert(std::make_pair(m_cGoalItemSpawnLocations.size(),
		tGoalItemSpawnLocation));

	// Set Render/Collision
	Renderer::GetInstance()->CreateRenderComp(pObj, 
		CSpawningManager::GetInstance()->m_nGoalItems[m_eType],
		CSpawningManager::GetInstance()->m_nGoalItemRenderContexts[nID], 2);

	TSphere tsphere;
	tsphere.cPosition = tGoalItemSpawnLocation.m_cPos;
	tsphere.fRadius = 1.5f;
	CCollideable* pCollideable = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pObj, false, false, OBJGITEM);
	pCollideable->SetBVType(BSPHERE);
	pCollideable->SetSphere(tsphere);

	// Scale obj's frame so it's easily seen
	//pObj->GetTransform()->ScaleFrame(tsphere.fRadius, tsphere.fRadius,
	//	tsphere.fRadius);

	// Send the GoalItemInit event
	SendGoalItemEvent("GoalItemInit", this, 
		tGoalItemSpawnLocation.m_cGoalItem->m_eType, 
		tGoalItemSpawnLocation.m_cGoalItem);
}

void CDepartment::Despawn()
{
	// Department is no longer being used
	m_bUsed = false;

	map<int, TGoalItemSpawnLocation, less<unsigned int>,
		CAllocator<pair<int, TGoalItemSpawnLocation>>>::iterator pcIter =
		m_cGoalItemSpawnLocations.begin();

	while(pcIter != m_cGoalItemSpawnLocations.end())
	{
		// This Goal Item is not spawned
		pcIter->second.m_bSpawned = false;

		// Call Goal Item's Despawn function
		//SendGoalItemEvent("GoalItemDespawned", pcGoalItem,
		//	pcGoalItem->m_eType, pcGoalItem,
		//	PRIORITY_IMMEDIATE);

		pcIter->second.m_cGoalItem->Despawn();

		++pcIter;
	}
}

void CDepartment::Spawn(IEvent* piEvent, IComponent* piComponent)
{
	CDepartment* pcComponent = (CDepartment*)piComponent;
	CDepartment* pcDepartment = (CDepartment*)piEvent->GetData();

	if(pcComponent == pcDepartment)
	{
		pcComponent->SpawnGoalItem();
	}
}

void CDepartment::SpawnGoalItem()
{
	int nLocation = rand() % m_cGoalItemSpawnLocations.size();

	map<int, TGoalItemSpawnLocation, less<unsigned int>,
		CAllocator<pair<int, TGoalItemSpawnLocation>>>::iterator pcIter = 
		m_cGoalItemSpawnLocations.find(nLocation);

	pcIter->second.m_bSpawned = true;
	m_bUsed = true;

	pcIter->second.m_cGoalItem->Init(
		pcIter->second.m_cGoalItem->m_eType);
	
	D3DXMATRIX& myMatress = pcIter->second.m_cGoalItem->m_pObject->GetTransform()->GetLocalMatrix();
	myMatress._41 = pcIter->second.m_cPos.x;
	myMatress._42 = pcIter->second.m_cPos.y;
	myMatress._43 = pcIter->second.m_cPos.z;

	pcIter->second.m_cGoalItem->Spawn();
}

D3DXVECTOR3 CDepartment::GetSpawnPosition(EDepartment eDepartment)
{
	D3DXVECTOR3 cPos;

	switch(eDepartment)
	{
	case GORILLA_MILK:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint2")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(-44, 0.75, -20);
			break;
		}
	case CAPTAIN_FLAKEY:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint34")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(-37, 0.75, 4);
			break;
		}
	case PIZZA:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint53")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(12, 0.75, -14);
			break;
		}
	case CAKE:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint33")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(-44, 0.75, 4);
			break;
		}
	case CANNED_BEARS:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint27")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(27, 0.75, -26);
			break;
		}
	case CARROTS:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint60")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(39, 0.75, 22);
			break;
		}
	case SAMMY_SANDWICH:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint68")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(-41, 0.75, 21);
			break;
		}
	case SQUID:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint12")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(-23, 0.75, 4);
			break;
		}
	case AGENT_ORANGE:
		{
			cPos = /*CObjectManager::GetInstance()->GetObjectByName("Waypoint16")->GetTransform()->GetWorldPosition()*/ D3DXVECTOR3(12, 0.75, 11);
			break;
		}
	}

	return D3DXVECTOR3(0, -5, 0);//cPos;
}