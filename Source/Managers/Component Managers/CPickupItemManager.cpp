#include <iostream>
using std::cout;

#include "CPickupItemManager.h"
#include "../../CObject.h"
#include "../Global Managers/Rendering Managers/Renderer.h"
#include "../../Components/Rendering/CRenderComponent.h"
#include "CCollisionManager.h"
#include "CSpawningManager.h"

#include"..\\Global Managers\\Rendering Managers\\DXRenderContextManager.h"
#include"..\\Global Managers\\Rendering Managers\\DXRenderContext.h"
#include"..\\Global Managers\\Rendering Managers\\ModelManager.h"

#include "..\Global Managers\Object Manager\CObjectManager.h"
#include "..\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

#define MAX_PICKUP_ITEMS_IN_LEVEL 24

CPickupItemManager::CPickupItemManager(CPickupItemManager &/*ref*/)
{

}


	// Init
void CPickupItemManager::Init(void)
{
	/*for(int i=0; i< MAX_PICKUP_ITEMS_IN_LEVEL; i++)
	{
		D3DXVECTOR3 vObjPos(300.0f, 300.0f, 300.0f);
		string szObjName = "PickupItem";
		static int nNumCreated = 0; 
		szObjName += (nNumCreated + '0');
		nNumCreated++;
		CObject* pPickupItem = CObjectManager::GetInstance()->CreateObject(szObjName, vObjPos.x, vObjPos.y, vObjPos.z);


		CPickupItemComponent* pComp = CPickupItemComponent::CreatePickupItemComponent(pPickupItem, D3DXVECTOR3(0,0,0));
		pComp->EventInit();


		pComp->m_pRenderComp = Renderer::GetInstance()->CreateRenderComp(
			pPickupItem, 33, 0, 2);
		
		// Add a collision component to pick it up
		TSphere tsphere;
		tsphere.cPosition = vObjPos;
		tsphere.fRadius = 1.0f;
		CCollideable* pCol = CCollisionManager::GetInstance()->CreateCollideableComponent(pPickupItem, true, false, OBJPITEM);
		pCol->SetBVType(BSPHERE);
		pCol->SetSphere(tsphere);
	}/**/

	string szEvent = "ShutdownObjects";
	szEvent += GAMEPLAY_STATE;

	CEventManager::GetInstance()->RegisterEvent(szEvent.c_str(), (IComponent*)this, Shutdown);
	CEventManager::GetInstance()->RegisterEvent("SpawnPickupItem", (IComponent*)this, SpawnPickupItem);
}

void CPickupItemManager::Shutdown(IEvent*, IComponent*)
{
	list<CPickupItemComponent*, 
		CAllocator<CPickupItemComponent*>>::iterator pIter = GetInstance()->m_cPickupItemComps.begin();

	cout << "Number of Pickup Items: " << GetInstance()->m_cPickupItemComps.size() << endl;

	while(pIter != GetInstance()->m_cPickupItemComps.end())
	{
		GetInstance()->DespawnPickupItem(*pIter);
		++pIter;
	}
}

void CPickupItemManager::AddComponent(CPickupItemComponent* pComp)
{
	m_cPickupItemComps.push_back(pComp);
}

void CPickupItemManager::RemoveComponent(CObject* pObj)
{

	list<CPickupItemComponent*, CAllocator<CPickupItemComponent*>>::iterator iter;
	iter = m_cPickupItemComps.begin();

	// iterate through the list and delete the first instance matching object's pcomp
	while(iter != m_cPickupItemComps.end())
	{
		if((*iter)->GetObject() == pObj)
		{
			//CPickupItemComponent* pComp = *iter;

			m_cPickupItemComps.erase(iter);

			// TODO: comp delete stuff here

			return;
		}
		iter++;
	}
}


void CPickupItemManager::SpawnPickupItem(IEvent* cEvent, IComponent* /*cCenter*/)
{
	TSpawnPickupItemEvent* tEvent = (TSpawnPickupItemEvent*)cEvent->GetData();

	D3DXVECTOR3 vObjPos = tEvent->m_vVec1;
	D3DXVECTOR3 vMoveDir = tEvent->m_vVec2;
	EGoalItemType		nItemID = (EGoalItemType)tEvent->m_nItemID;


	// get a comp
	CPickupItemComponent* pComp = GetInstance()->GetAPickupItem(nItemID);

	pComp->Init();

	pComp->GetObject()->GetTransform()->GetLocalMatrix()._41 = vObjPos.x;
	pComp->GetObject()->GetTransform()->GetLocalMatrix()._42 = vObjPos.y;
	pComp->GetObject()->GetTransform()->GetLocalMatrix()._43 = vObjPos.z;

	// generate a right direction for direction to move so we can have an arc
	D3DXVECTOR3 vRightDir;
	D3DXVec3Cross(&vRightDir, &(D3DXVECTOR3(0.0f ,1.0f, 0.0f)), &vMoveDir);
	D3DXVec3Normalize(&vRightDir, &vRightDir);
	// get a random value so we can have an arc
	int randtwenty = rand()%20;
	float randtwo = randtwenty*.1f;
	// scale by the rand value (-1to1)
	D3DXVec3Scale(&vRightDir, &vRightDir, randtwo);
	// add that left or right dir to the move dir
	vMoveDir += vRightDir;
	D3DXVec3Normalize(&vMoveDir, &vMoveDir);
	

	pComp->m_vDirectionToMove = vMoveDir;
	pComp->m_nItemType = (EGoalItemType)nItemID;

	// Send a Event
	SendPickupItemEvent("PickupItemDropped", (IComponent*)GetInstance(), pComp->GetObject(), pComp->m_nItemType); 
}



void CPickupItemManager::DespawnPickupItem(CPickupItemComponent* pComp)
{
	pComp->GetObject()->GetTransform()->GetLocalMatrix()._41 = 300.0f;
	pComp->GetObject()->GetTransform()->GetLocalMatrix()._42 = 300.0f;
	pComp->GetObject()->GetTransform()->GetLocalMatrix()._43 = 300.0f;
	pComp->m_nItemType = NO_ITEM;

	SendObjectEvent("PickupItemDespawned", (IComponent*)this, pComp->m_pObject);

	pComp->m_bSpawned = false;
}




CPickupItemComponent* CPickupItemManager::GetPickupComp(EGoalItemType eType)
{
	list<CPickupItemComponent*, 
		CAllocator<CPickupItemComponent*>>::iterator pIter =
		m_cPickupItemComps.begin();

	while(pIter != m_cPickupItemComps.end())
	{
		if((*pIter)->GetType() == eType)
		{
			return (*pIter);
		}

		++pIter;
	}

	return NULL;
}


CPickupItemComponent* CPickupItemManager::GetAPickupItem(EGoalItemType eType)
{
	list<CPickupItemComponent*, CAllocator<CPickupItemComponent*>>::iterator iter;
	iter = GetInstance()->m_cPickupItemComps.begin();

	// iterate through the list and find the first instance not used
	while(iter != GetInstance()->m_cPickupItemComps.end())
	{
		// if it's despawned 
		if((*iter)->m_bSpawned == false)
		{
			// get out of here so we can change it further down
			break;
		}
		iter++;
	}

	// get the mesh and context information for the itemID we were given
	DXRenderContext* pContext = DXRenderContextManager::GetInstance()->
		GetContext((ERenderContext)CSpawningManager::GetInstance()->GetGoalItemRenderContext(eType));
	int nMeshId = GetInstance()->GetModelNumber(eType);

	// if the iter isn't the end (aka we broke before the while would have) that means we have an item not in use already made
	if(iter != GetInstance()->m_cPickupItemComps.end())
	{
		CPickupItemComponent* pComp = *iter;

		pComp->m_pRenderComp->SetMesh(nMeshId, pContext);
		return pComp;
	}

	// got here because the list didn't have any unused pickup items

	// create another pickup item
	D3DXVECTOR3 vObjPos(300.0f, 300.0f, 300.0f);
	string szObjName = "PickupItem";
	static int nNumCreated = 0; 
	szObjName += (char)(nNumCreated + '0');
	nNumCreated++;
	CObject* pPickupItem = CObjectManager::GetInstance()->CreateObject(szObjName);
	
	// Let us know it has been created (Note: This is immediate so that the PickupITemDropped message is always received second)
	SendPickupItemEvent("PickupItemSpawned", (IComponent*)GetInstance(), pPickupItem, eType, PRIORITY_IMMEDIATE); 

	CPickupItemComponent* pComp = CPickupItemComponent::CreatePickupItemComponent(pPickupItem, D3DXVECTOR3(0,0,0));
	pComp->EventInit();

	pComp->m_pRenderComp = Renderer::GetInstance()->CreateRenderComp(
		pPickupItem, nMeshId, CSpawningManager::GetInstance()->GetGoalItemRenderContext(eType), 2);
	pComp->m_pRenderComp->SetMesh(nMeshId, pContext);

	// Add a collision component to pick it up
	TSphere tsphere;
	tsphere.cPosition = vObjPos;
	tsphere.fRadius = 1.0f;				// TODO: Is this the radius we want?
	CCollideable* pCol = CCollisionManager::GetInstance()->CreateCollideableComponent(
		pPickupItem, false, false, OBJPITEM);
	pCol->SetBVType(BSPHERE);
	pCol->SetSphere(tsphere);

	//m_cPickupItemComps.push_back(pComp); Dont add because createpickupitemcomp() does it in there
	return pComp;
}

int CPickupItemManager::GetModelNumber(EGoalItemType eType)
{
	return ModelManager::GetInstance()->GetMeshIndexByName(
		CSpawningManager::GetInstance()->GetGoalItemName(eType).c_str());
}
