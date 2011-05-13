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
	CEventManager::GetInstance()->RegisterEvent("SpawnGoalItem", this, Spawn);
	m_eType = eDepartment;
	m_bUsed = false;

	TGoalItemSpawnLocation tGoalItemSpawnLocation;

	tGoalItemSpawnLocation.m_cGoalItem = (EGoalItemType)(eDepartment);
	tGoalItemSpawnLocation.m_cPos = D3DXVECTOR3(0, 1, 0);
	tGoalItemSpawnLocation.m_bSpawned = false;

	m_cGoalItemSpawnLocations.insert(std::make_pair(m_cGoalItemSpawnLocations.size(),
		tGoalItemSpawnLocation));

	SendGoalItemEvent("GoalItemInit", this, tGoalItemSpawnLocation.m_cGoalItem, NULL);
}

void CDepartment::Despawn()
{
	m_bUsed = false;

	map<int, TGoalItemSpawnLocation, less<unsigned int>,
		CAllocator<pair<int, TGoalItemSpawnLocation>>>::iterator pcIter =
		m_cGoalItemSpawnLocations.begin();

	while(pcIter != m_cGoalItemSpawnLocations.end())
	{
		pcIter->second.m_bSpawned = false;

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

	static int nID = -1;
	std::string szID;
	char chCounter[32];
	_itoa_s(++nID, chCounter, 32, 10);
	szID = "GoalItem ";
	szID += chCounter;

	// Set Position
	D3DXVECTOR3 cPos = GetSpawnPosition(m_eType);
	//cPos.x = m_cGoalItemSpawnLocations[nLocation].m_cPos.x;
	//cPos.y = m_cGoalItemSpawnLocations[nLocation].m_cPos.y;
	//cPos.z = m_cGoalItemSpawnLocations[nLocation].m_cPos.z;

	// Create Object
	CObject* pObj = CObjectManager::GetInstance()->CreateObject(szID, 
		cPos.x, cPos.y, cPos.z);

	// Create Goal Item
	CSpawningManager::GetInstance()->CreateGoalItemComponent(pObj, 
		m_cGoalItemSpawnLocations[nLocation].m_cGoalItem);
	
	// Set Render/Collision
	Renderer::GetInstance()->CreateRenderComp(pObj, 23, 0, 1);
	TSphere tsphere;
	tsphere.cPosition = cPos;
	tsphere.fRadius = 1.5f;
	CCollideable* pCollideable = CCollisionManager::GetInstance()->
		CreateCollideableComponent(pObj, true, false, OBJGITEM);
	pCollideable->SetBVType(BSPHERE);
	pCollideable->SetSphere(tsphere);

	//pObj->GetTransform()->TranslateFrame(D3DXVECTOR3(cPos.x, cPos.y, cPos.z));
	//pObj->GetTransform()->ScaleFrame(tsphere.fRadius, tsphere.fRadius*3.0f,
	//	tsphere.fRadius);
}

D3DXVECTOR3 CDepartment::GetSpawnPosition(EDepartment eDepartment)
{
	D3DXVECTOR3 cPos;

	switch(eDepartment)
	{
	case 0:
		{
			cPos = D3DXVECTOR3(-14.5f, 0.75f, -5.0f);
			break;
		}
	case 1:
		{
			cPos = D3DXVECTOR3(6.0f, 0.75f, 10.5f);
			break;
		}
	case 2:
		{
			cPos = D3DXVECTOR3(7.8f, 0.75f, 22.6f);
			break;
		}
	case 3:
		{
			cPos = D3DXVECTOR3(-10.5f, 0.75f, 19.3f);
			break;
		}
	case 4:
		{
			cPos = D3DXVECTOR3(32.4f, 0.75f, -13.5f);
			break;
		}
	case 5:
		{
			cPos = D3DXVECTOR3(3.0f, 0.75f, -19.5f);
			break;
		}
	case 6:
		{
			cPos = D3DXVECTOR3(7.8f, 0.75f, -10.67f);
			break;
		}
	case 7:
		{
			cPos = D3DXVECTOR3(-41.45f, 0.75f, -5.67f);
			break;
		}
	case 8:
		{
			cPos = D3DXVECTOR3(0.0f, 0.75f, 0.0f);
			break;
		}
	}

	return cPos;
}