/***********************************************
 * Filename:  		CAIComponent.cpp
 * Date:      		04/04/2011
 * Mod. Date: 		06/10/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This is the AI's main class
			which is responsable for changing
			the AI's state based on weights.
					The AI will have three main
			states: Defensive, Collective, and 
			Aggressive. Each state will dertermine
			how the AI behaves in each situation.
 ************************************************/
#include <iostream>
using std::cout;

#include "CAIComponent.h"
#include "..\Level\CGoalItemComponent.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Component Managers\CAIManager.h"
#include "..\..\Managers\Component Managers\CSpawningManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\Managers\Component Managers\CPickupItemManager.h"
#include "..\..\Managers\Global Managers\Memory Manager\CMemoryManager.h"
#include "..\..\Managers\Component Managers\CCollisionManager.h"
#include "..\..\Managers\Global Managers\Console Manager\CConsoleManager.h"
#include "..\..\Managers\Global Managers\Rendering Managers\Renderer.h"
#include "..\..\Managers\Global Managers\Rendering Managers\DXMesh.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\Rendering\CRenderComponent.h"
using namespace EventStructs;

static float Magnitude(D3DXVECTOR3 pt)
{
	return sqrtf((pt.x * pt.x) + (pt.y * pt.y) + (pt.z * pt.z));
}

static float AngleBetweenVectors(D3DXVECTOR3 pt1, D3DXVECTOR3 pt2)
{
	float fDotProduct = D3DXVec3Dot(&pt1, &pt2);

	float fMagnitude =  Magnitude(pt1) * Magnitude(pt2);

	if (fMagnitude == 0.0f) {return 0.0f;}

	float fAngle = acos( fDotProduct / fMagnitude );

	if (_isnan(fAngle)) {return 0.0f;}

	return fAngle;
}

inline static float DegreesToRadians(const float fDegrees)
{
	return fDegrees * (3.14159265f / 180.0f);
}

// Hack cuz we don't have
// time to fix it
// Yay!!
int nNumber = 0;
CAIComponent::CAIComponent(CObject* pObj) : m_pObject(pObj)
{
}

CAIComponent::~CAIComponent()
{
	while(m_tKnowledge.m_cGoalItemsInLevel.empty() == false)
	{
		list<TGoalItem, CAllocator<TGoalItem>>::iterator pIter =
			m_tKnowledge.m_cGoalItemsInLevel.begin();

		MyDelete<TGoalItem>(&(*pIter), HEAPID_GENERAL);
		
		m_tKnowledge.m_cGoalItemsInLevel.erase(
			m_tKnowledge.m_cGoalItemsInLevel.begin());
	}

	while(m_tKnowledge.m_cNeededGoalItems.empty() == false)
	{
		list<EGoalItemType, CAllocator<EGoalItemType>>::iterator pIter =
			m_tKnowledge.m_cNeededGoalItems.begin();

		MyDelete<EGoalItemType>(&(*pIter), HEAPID_GENERAL);

		m_tKnowledge.m_cNeededGoalItems.erase(
			m_tKnowledge.m_cNeededGoalItems.begin());
	}

	for(unsigned nIndex = 0; nIndex < 3; ++nIndex)
	{
		while(m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.empty() == false)
		{
			list<EGoalItemType, CAllocator<EGoalItemType>>::iterator pIter =
				m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin();

			MyDelete<EGoalItemType>(&(*pIter), HEAPID_GENERAL);

			m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.erase(
				m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin());
		}
	}
}

void CAIComponent::InitCB(IEvent*, IComponent* pComp)
{
	CAIComponent* cAIComp = (CAIComponent*)pComp;

	cAIComp->m_pfWeight[COLLECTIVE] = 0;
	cAIComp->m_pfWeight[AGGRESSIVE] = 0;
	cAIComp->m_pfWeight[DEFENSIVE] = 0;

	cAIComp->m_eCurrentState = NOT_INIT;

	cAIComp->m_tKnowledge.m_cGoalItemsInLevel.clear();
	cAIComp->m_tKnowledge.m_peMyHeldItems[0] = NO_HELD_ITEM;
	cAIComp->m_tKnowledge.m_peMyHeldItems[1] = NO_HELD_ITEM;
	cAIComp->m_tKnowledge.m_cNeededGoalItems.clear();
	cAIComp->m_tKnowledge.m_cHeldItemsInLevel.clear();
	cAIComp->m_tKnowledge.m_nDistanceFromGoal = 0;
	cAIComp->m_tKnowledge.m_nOpponentID = 0;

	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator pItemIter =
		CAIManager::GetInstance()->m_cInitedGoalItems.begin();

	while(pItemIter != CAIManager::GetInstance()->m_cInitedGoalItems.end())
	{
		cAIComp->m_tKnowledge.m_cNeededGoalItems.push_back(*pItemIter);
		++pItemIter;
	}

	for(unsigned nPlayers = 0; nPlayers < 3; ++nPlayers)
	{
		//cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_nPlayer = -1;
		cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_bHasItem = false;
		cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_bTarget = false;
		cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.clear();
		cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_chAmtGoalItems = 0;
		cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_nItem = -1;
		cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_nAggression = 0;
	}

	cAIComp->m_fTimer = 1.0f;

	cAIComp->m_nFramesToAvoidObstacle = 0;
	cAIComp->m_vTargetPosition = D3DXVECTOR3(0, 0, -65535);

	cAIComp->m_vAvoidanceTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	cAIComp->m_nControllerNumber = ++nNumber;
	cAIComp->m_nFramesWeAreOnThisTarget = 500;
	cAIComp->m_nFramesToAccel = 3;

	cAIComp->m_fReactionDistance = 0.1f;
	cAIComp->m_fSideReactionDistance = 0.1f;

	cAIComp->m_nTargetTriangle = -1;
	cAIComp->m_nCurrentTriangle = -1;
	cAIComp->m_fwaaaaAaaaAAAAAattttt = 0;

	cAIComp->m_tTargetPlayerLine.cRayNormal = D3DXVECTOR3(65535, 65535, 65535);

	cAIComp->CreateMeshTriangle();
	cAIComp->bDrawTris = false;
	cAIComp->bLogical = true;

	cAIComp->m_pMiniGoalTri = NULL;
	cAIComp->fThreshold[0] = 0.5f;
	cAIComp->fThreshold[1] = 0.8f;

	// Alert anyone who is waiting on AI
	// that AI is now up and running
	SendIEvent("AIInited", cAIComp, NULL, PRIORITY_RENDER);
}

void CAIComponent::Init()
{
	CEventManager* pEM = CEventManager::GetInstance();

	string szEvent = "Update";
	string szEvent2 = "InitObjects";
	string szEvent3 = "ShutdownObjects";
	szEvent += GAMEPLAY_STATE;
	szEvent2 += GAMEPLAY_STATE;
	szEvent3 += GAMEPLAY_STATE;

	pEM->RegisterEvent("PlayerCharacterSet", this, PlayerCreated);
	
	pEM->RegisterEvent(szEvent, this, Update);
	pEM->RegisterEvent(szEvent2, this, InitCB);
	pEM->RegisterEvent(szEvent3, this, Shutdown);
	pEM->RegisterEvent("CartCollision", this, PlayerAttacked);
	pEM->RegisterEvent("GoalItemCollected", this, GoalItemCollected);
	pEM->RegisterEvent("PickupItemCollected", this,	PickupItemCollected);
	pEM->RegisterEvent("ShowRamSpeed", this, SetRamSpeed);
	pEM->RegisterEvent("HeldItemSpawned", this, HeldItemSpawned);
	pEM->RegisterEvent("HeldItemCollected", this, HeldItemCollected);
	pEM->RegisterEvent("PlayerAttacked", this, PlayerAttacked);
	pEM->RegisterEvent("GoalItemSpawned", this, GoalItemSpawned);
	pEM->RegisterEvent("GoalItemDespawned", this, GoalItemDespawned);
	pEM->RegisterEvent("SpeedValue", this, UpdateSpeeds);
	pEM->RegisterEvent("ItemDropped", this, ItemDropped);


	pEM->RegisterEvent("ToggleAI", this, ToggleAI);
	pEM->RegisterEvent("ToggleTris", this, ToggleTris);
	pEM->RegisterEvent("ShowTurnSpeed", this, ShowTurnSpeed);

	string szEventName = "Update";
	szEventName += PAUSE_STATE;
	pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);
	
	//szEventName = "Update";
	//szEventName += CONSOLE_STATE;
	//pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);
	//
	//szEventName = "Update";
	//szEventName += PAUSE_KEYBINDS_STATE;
	//pEM->RegisterEvent(szEvent, this, PauseUpdateCallback); 
	//
	//szEventName = "Update";
	//szEventName += PAUSE_OPTIONS_STATE;
	//pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);
}

void CAIComponent::PlayerCreated(IEvent* pEvent, IComponent* pComp)
{
	TTwoIntEvent* pTwoInts = (TTwoIntEvent*)pEvent->GetData();
	CAIComponent* pcAI = (CAIComponent*)pComp;

	if(pcAI->m_nControllerNumber != pTwoInts->m_nInt1)
	{
		// Not us
		return;
	}

	pcAI->nAnimation = pTwoInts->m_nInt2;
}

void CAIComponent::Shutdown(IEvent*, IComponent* pComp)
{
	CAIComponent* pcAI = (CAIComponent*)pComp;

	list<TGoalItem, CAllocator<TGoalItem>>::iterator pGoalItemLevelIter;
	while(pcAI->m_tKnowledge.m_cGoalItemsInLevel.empty() == false)
	{
		pGoalItemLevelIter =	pcAI->m_tKnowledge.m_cGoalItemsInLevel.begin();

		MyDelete<TGoalItem>(&(*pGoalItemLevelIter), HEAPID_GENERAL);

		pcAI->m_tKnowledge.m_cGoalItemsInLevel.erase(
			pcAI->m_tKnowledge.m_cGoalItemsInLevel.begin());
	}

	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator pNeededItemsIter;
	while(pcAI->m_tKnowledge.m_cNeededGoalItems.empty() == false)
	{
		pNeededItemsIter =	pcAI->m_tKnowledge.m_cNeededGoalItems.begin();

		MyDelete<EGoalItemType>(&(*pNeededItemsIter), HEAPID_GENERAL);

		pcAI->m_tKnowledge.m_cNeededGoalItems.erase(
			pcAI->m_tKnowledge.m_cNeededGoalItems.begin());
	}

	for(unsigned nIndex = 0; nIndex < 3; ++nIndex)
	{
		list<EGoalItemType, CAllocator<EGoalItemType>>::iterator pIter;
		while(pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.empty() == false)
		{
			pIter =	pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin();

			MyDelete<EGoalItemType>(&(*pIter), HEAPID_GENERAL);

			pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.erase(
				pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin());
		}
	}

	list<THeldItem, CAllocator<THeldItem>>::iterator pHeldItemsLevelIter;
	while(pcAI->m_tKnowledge.m_cHeldItemsInLevel.empty() == false)
	{
		pHeldItemsLevelIter =	pcAI->m_tKnowledge.m_cHeldItemsInLevel.begin();

		MyDelete<THeldItem>(&(*pHeldItemsLevelIter), HEAPID_GENERAL);

		pcAI->m_tKnowledge.m_cHeldItemsInLevel.erase(
			pcAI->m_tKnowledge.m_cHeldItemsInLevel.begin());
	}

	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator pDroppedItemsIter;
	while(pcAI->m_tKnowledge.m_cDroppedItems.empty() == false)
	{
		pDroppedItemsIter =	pcAI->m_tKnowledge.m_cDroppedItems.begin();

		MyDelete<EGoalItemType>(&(*pDroppedItemsIter), HEAPID_GENERAL);

		pcAI->m_tKnowledge.m_cDroppedItems.erase(
			pcAI->m_tKnowledge.m_cDroppedItems.begin());
	}

	MMDEL(pcAI->m_pMesh);
	MMDEL(pcAI->m_pRendComp);
}

// Events Start
void CAIComponent::ItemDropped(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemCollectedEvent* pcGoalItem = (TGoalItemCollectedEvent*)piEvent->GetData();

	unsigned pType = (unsigned int)pcGoalItem->m_pcGoalItem;
	std::map<unsigned int, CObject*, less<unsigned int>,
		CAllocator<pair<unsigned int, CObject*>>>::iterator pIter = 
		CAIManager::GetInstance()->m_cSpawnableGoalItems.find(pType);

	pcAI->m_tKnowledge.m_cDroppedItems.push_back(
		static_cast<EGoalItemType>(pType));

	if(pcAI->m_pObject->GetID() == pcGoalItem->m_pcCollector->GetID())
	{
		// If we dropped the item
		pcAI->m_tKnowledge.m_cNeededGoalItems.push_back(
			static_cast<EGoalItemType>(pType));
	}
	else // If an opponent dropped the item
	{
		// Find which opponent
		int nIndex = -1;
		int i;
		for(i = 0; i < 3; ++i)
		{
			if(pcAI->m_tKnowledge.m_cOpponents[i].m_nPlayer ==
				pcGoalItem->m_pcCollector->GetID())
			{
				nIndex = i;
				break;
			}
		}

		// Decrease how many Goal Items they have
		pcAI->m_tKnowledge.m_cOpponents[nIndex].m_chAmtGoalItems--;

		// Check if they're within our radar
		if(pcAI->WithinRadar(pcGoalItem->m_pcCollector->GetTransform()->GetWorldPosition()))
		{
			list<EGoalItemType, CAllocator<EGoalItemType>>::iterator ItemIter;
			ItemIter = pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin();

			// Loop through the collector's collected Goal Items
			while(ItemIter != pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.end())
			{
				if(*ItemIter == static_cast<EGoalItemType>(pType))
				{
					// Remove the Goal Item from their collected list
					pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.remove(*ItemIter);
					break;
				}

				++ItemIter;
			}
		}
	}
}

void CAIComponent::SetupOpponents(CObject* pcObject)
{
	//CAIComponent* pcAI = (CAIComponent*)piComponent;
	//TObjectEvent* pcObject = (TObjectEvent*)piEvent->GetData();

	// Previously in Manager
	//std::list<CObject*,	CAllocator<CObject*>>::iterator pcPlayerIter = 
	//	CAIManager::GetInstance()->m_cPlayers.begin();
	//bool bAlreadyAdded = false;

	//while(pcPlayerIter != CAIManager::GetInstance()->m_cPlayers.end())
	//{
	//	if(*pcPlayerIter == pcObject)
	//	{
	//		bAlreadyAdded = true;
	//		break;
	//	}
	//	pcPlayerIter++;
	//}

	//if(bAlreadyAdded == false)
	//{
	//	CAIManager::GetInstance()->m_cPlayers.push_back(pcObject);
	//}

	// Component part
	if(m_pObject->GetID() != pcObject->GetID())
	{
		m_tKnowledge.m_cOpponents[m_tKnowledge.m_nOpponentID].m_nPlayer =
			pcObject->GetID();
		++m_tKnowledge.m_nOpponentID;
	}
}

void CAIComponent::ToggleAI(IEvent*, IComponent* pComp)
{
	pComp->ToggleIsActive();
}

void CAIComponent::ToggleTris(IEvent*, IComponent* pComp)
{
	CAIComponent* pcAI = (CAIComponent*)pComp;
	pcAI->bDrawTris = !pcAI->bDrawTris;
}

void CAIComponent::Use(EHeldItemType eType)
{
	for(int nItem = 0; nItem < 2; ++nItem)
	{
		if(m_tKnowledge.m_peMyHeldItems[nItem] == eType)
		{
			if(nItem == 0)
				SendInputEvent("UseHeldItem1", this, m_pObject, 1.0f);
			else
				SendInputEvent("UseHeldItem2", this, m_pObject, 1.0f);

			m_tKnowledge.m_peMyHeldItems[nItem] = NO_HELD_ITEM;
			break;
		}
	}
}

CRenderComponent* CAIComponent::CreateMeshTriangle()
{
	string szObjName = "meshTri";
	CObject* pObj = CObjectManager::GetInstance()->CreateObject(szObjName);

	// Store both
	m_pMesh = MMNEW(DXMesh);
	m_pRendComp = Renderer::GetInstance()->CreateRenderComp(pObj, m_pMesh, RC_FLAT, RF_VERT);

	return m_pRendComp;
}

void CAIComponent::StartDraw()
{
	if(bDrawTris == false)
		return;

	TMeshVertexInfo& pVertInfo = m_pMesh->GetVertInfo();
	// Add verts to mesh to be drawn
	pVertInfo.m_vVertices.clear();
	pVertInfo.m_vColors.clear();
}

void CAIComponent::EndDraw()
{
	if(bDrawTris == false)
		return;

	TMeshVertexInfo& pVertInfo = m_pMesh->GetVertInfo();
	// Render all lines and tris... and maybe spheres or w/e
	int count = pVertInfo.m_vVertices.size();
	for(int v=0; v<count; v++)
	{
		pVertInfo.m_vVertices[v].y = 0.1f;
	}
	m_pMesh->SetColoredTexturedVertexInfo(&pVertInfo);
	m_pRendComp->AddToRenderSet();
}

void CAIComponent::DrawTriangle(D3DXVECTOR3 vVert0, D3DXVECTOR3 vVert1, D3DXVECTOR3 vVert2, D3DXCOLOR cColor)
{
	if(bDrawTris == false)
		return;

	TMeshVertexInfo& pVertInfo = m_pMesh->GetVertInfo();

	pVertInfo.m_vVertices.push_back(vVert0);
	pVertInfo.m_vColors.push_back(cColor);
	pVertInfo.m_vUV.push_back(D3DXVECTOR2(0.0f, 0.0f));

	pVertInfo.m_vVertices.push_back(vVert1);
	pVertInfo.m_vColors.push_back(cColor);
	pVertInfo.m_vUV.push_back(D3DXVECTOR2(0.0f, 0.0f));

	pVertInfo.m_vVertices.push_back(vVert2);
	pVertInfo.m_vColors.push_back(cColor);
	pVertInfo.m_vUV.push_back(D3DXVECTOR2(0.0f, 0.0f));
}

void CAIComponent::DrawTriangle(TTri* tTri, D3DXCOLOR cColor)
{
	DrawTriangle(tTri->tEdges[0].cLineStart, tTri->tEdges[1].cLineStart, tTri->tEdges[2].cLineStart, cColor);
}

void CAIComponent::DrawTriangle(TTriangle* tTri, D3DXCOLOR cColor)
{
	DrawTriangle(tTri->tVert0, tTri->tVert1, tTri->tVert2, cColor);
}

void CAIComponent::DrawLine(D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, D3DXCOLOR cColor)
{
	DrawTriangle(vStart, vEnd, vEnd, cColor);
}

void CAIComponent::Update(IEvent* piEvent, IComponent* piComponent)
{
	// nNumber is a HACK
	nNumber = 0;
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	CAIManager* cAIManager = CAIManager::GetInstance();
	CCollisionManager* cCollision = CCollisionManager::GetInstance();
	TUpdateStateEvent* pcUpdate = (TUpdateStateEvent*)piEvent->GetData();

	//if(pcAI->m_pObject->GetID() != CIDGen::GetInstance()->GetID("Player1"))
	//{
	//	// HACK !!!
	//	return;
	//}

	pcAI->m_fTimer += pcUpdate->m_fDeltaTime;
	SendRenderEvent("AddToSet", pcAI, pcAI->m_pObject, PRIORITY_IMMEDIATE);

	if(CInputManager::GetInstance()->GetState() == GAMEPLAY_STATE)
	{
		D3DXVECTOR3 Heading;
		Heading.x = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._31;
		Heading.y = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._32;
		Heading.z = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._33;
		D3DXVECTOR3 myPosition = pcAI->m_pObject->GetTransform()->GetWorldPosition();
		myPosition += Heading * 0.5f;

		// Go to checkout behavior
		if(pcAI->m_tKnowledge.m_cNeededGoalItems.size() == 0)
		{
			pcAI->m_eCurrentState = NOT_INIT;
			pcAI->m_vGoalPosition = D3DXVECTOR3(0, 0 , 68);
		}
		else
		{
			if(pcAI->m_fTimer > 0.4f)
			{
				pcAI->m_fTimer = 0.0f;
				pcAI->EvaluateStateWeights();
			}
		}

		pcAI->StartDraw();

		// Declare/Initiliaze
		//int nGoalEdge = -1;
		int nHeadingEdge = -1;
		int nAStarEdge = -1;
		int nMiniGoalTriID = -1;
		bool bCurFound = false; 
		bool bTarFound = false;
		TTriangle tri;
		D3DXVECTOR3 ToGoal;
		D3DXVECTOR3 GoalPosition;

		// Find GoalPosition, CurrentTriangle and TargetTriangle
		map<int, TTri*, less<int>, CAllocator<pair<int, TTri*>>>::iterator 
			pFindIter = cAIManager->m_cTris.begin();
		tri.tNorm = D3DXVECTOR3(0, 1, 0);
		while(pFindIter != cAIManager->m_cTris.end())
		{
			if(bCurFound == true &&
				bTarFound == true)
			{
				break;
			}

			if(cAIManager->GetDistance(pFindIter->second->tPos, 
				pcAI->m_vGoalPosition) < 500)
			{
				// TargetTriangle
				tri.tVert0 = pFindIter->second->tEdges[0].cLineStart;
				tri.tVert1 = pFindIter->second->tEdges[1].cLineStart;
				tri.tVert2 = pFindIter->second->tEdges[2].cLineStart;

				if(cCollision->PointInTriangle(tri, pcAI->m_vGoalPosition) == true)
				{
					pcAI->DrawTriangle(tri.tVert0, tri.tVert1, tri.tVert2, D3DXCOLOR(255, 0, 0, 255));
					pcAI->DrawLine(myPosition, pFindIter->second->tPos, D3DXCOLOR(0, 0, 255, 255));
					pcAI->m_nTargetTriangle = pFindIter->first;
					GoalPosition = pFindIter->second->tPos;
					bTarFound = true;
				}
			}

			if(cAIManager->GetDistance(pFindIter->second->tPos,
				myPosition) < 500)
			{
				// CurrentTriangle
				tri.tVert0 = pFindIter->second->tEdges[0].cLineStart;
				tri.tVert1 = pFindIter->second->tEdges[1].cLineStart;
				tri.tVert2 = pFindIter->second->tEdges[2].cLineStart;

				if(cCollision->PointInTriangle(tri, myPosition) == true)
				{
					pcAI->DrawTriangle(tri.tVert0, tri.tVert1, tri.tVert2, D3DXCOLOR(126, 126, 126, 255));
					pcAI->m_nCurrentTriangle = pFindIter->first;
					bCurFound = true;
				}
			}

			++pFindIter;
		}

		if(bTarFound == false)
		{
			// We didn't find a new target...
			// Find the triangle closest to the goal position
			pFindIter = cAIManager->m_cTris.begin();
			TTri* tClosestTri;
			int nTriNumber;
			float fDist = FLT_MAX;
			float fTempDist;
			while(pFindIter != cAIManager->m_cTris.end())
			{
				fTempDist = cAIManager->GetDistance(pFindIter->second->tPos, 
					pcAI->m_vGoalPosition);

				if(fTempDist < fDist)
				{
					// We found a closer triangle than the last one
					fDist = fTempDist;
					tClosestTri = pFindIter->second;
					nTriNumber = pFindIter->first;
				}

				++pFindIter;
			}

			// We found the closest triangle so
			// set it as our target
			pcAI->DrawTriangle(tClosestTri->tEdges[0].cLineStart,
				tClosestTri->tEdges[1].cLineStart, tClosestTri->tEdges[2].cLineStart,
				D3DXCOLOR(255, 0, 0, 255));

			pcAI->DrawLine(myPosition, tClosestTri->tPos, D3DXCOLOR(0, 0, 255, 255));
			pcAI->m_nTargetTriangle = nTriNumber;
			GoalPosition = tClosestTri->tPos;
		}

		if(bCurFound == false)
		{
			// We aren't in a triangle
			// So lets just go back to the last
			// triangle we were in
			if(pcAI->m_nCurrentTriangle == -1)
			{
				pcAI->m_nCurrentTriangle = rand() % cAIManager->m_cTris.size();
			}

			pFindIter = cAIManager->m_cTris.find(pcAI->m_nCurrentTriangle);
			tri.tVert0 = pFindIter->second->tEdges[0].cLineStart;
			tri.tVert1 = pFindIter->second->tEdges[1].cLineStart;
			tri.tVert2 = pFindIter->second->tEdges[2].cLineStart;

			D3DXMATRIX& myMatress = pcAI->m_pObject->GetTransform()->GetLocalMatrix();
			myMatress._41 = pFindIter->second->tPos.x - Heading.x;
			myMatress._42 = pFindIter->second->tPos.y - Heading.y;
			myMatress._43 = pFindIter->second->tPos.z - Heading.z;
		}

	// Find which edge the agent should be trying to go through
	TTri *pTri = cAIManager->m_cTris.find(pcAI->m_nCurrentTriangle)->second;
	map<int, TTri*, less<int>,
		CAllocator<pair<int, TTri*>>>::iterator pCurTriIter = pTri->m_cConnections.begin();

	nAStarEdge = cAIManager->m_nLookup[cAIManager->LookUpCellAlgorithm(pcAI->m_nCurrentTriangle,
		pcAI->m_nTargetTriangle)];

	int nNextTriID = -1;
	// Find next triangle
	while(pCurTriIter != pTri->m_cConnections.end())
	{
		for(int i = 0; i < 3; ++i)
		{
			if(cAIManager->LinesEqual(pTri->tEdges[nAStarEdge],
				pCurTriIter->second->tEdges[i]))
			{
				pcAI->tNextTri = pCurTriIter->second;
				nNextTriID = pCurTriIter->first;

				// To easily get out of the while loop
				// without using a goto
				pCurTriIter = pTri->m_cConnections.end();
				--pCurTriIter;
				//
				break;
			}
		}

		++pCurTriIter;
	}
#pragma endregion

	if(nNextTriID != -1)
	{
		// Find entire path within distance
		float fDistanceOfPath = (pcAI->m_fSpeed * pcAI->m_fSpeed) * 0.66f;
		map<int, TTri*, less<int>,
			CAllocator<pair<int, TTri*>>>::iterator pTemp =
			cAIManager->m_cTris.find(nNextTriID);

		do
		{
			if(pTemp->first == pcAI->m_nTargetTriangle)
			{
				// We found our Goal
				pcAI->m_pMiniGoalTri = pTemp->second;
				break;
			}

			pcAI->DrawTriangle(pTemp->second->tEdges[0].cLineStart, 
				pTemp->second->tEdges[1].cLineStart,
				pTemp->second->tEdges[2].cLineStart, D3DXCOLOR(126, 126, 126, 255));

			// Find which edge the AI should try to
			// be exiting out of
			nAStarEdge = cAIManager->m_nLookup[cAIManager->LookUpCellAlgorithm(pTemp->first,
				pcAI->m_nTargetTriangle)];

			map<int, TTri*, less<int>,
				CAllocator<pair<int, TTri*>>>::iterator pConnectionIter = 
				pTemp->second->m_cConnections.begin();

			// Loop through all connections and look for
			// the next triangle in the path
			while(pConnectionIter != pTemp->second->m_cConnections.end())
			{
				// For each edge
				for(int nEdge = 0; nEdge < 3; ++nEdge)
				{
					// If the pConnectionIter's current edge is
					// equal to the current triangle's AStarEdge
					if(cAIManager->LinesEqual(pTemp->second->tEdges[nAStarEdge],
						pConnectionIter->second->tEdges[nEdge]))
					{
						// Set the MiniGoalTri to pTemp
						pcAI->DrawLine(pTemp->second->tPos, 
							pConnectionIter->second->tPos, D3DXCOLOR(255, 255, 0, 255));
						pcAI->m_pMiniGoalTri = pTemp->second;
						nMiniGoalTriID = pTemp->first;

						// Set pTemp to pConnectionIter
						pTemp = cAIManager->m_cTris.find(pConnectionIter->first);

						// To easily get out of the while
						// loop without using a goto
						pConnectionIter = pTemp->second->m_cConnections.end();
						--pConnectionIter;
						break;
						//
					}
				}
				++pConnectionIter;
			}
		}
		while(fDistanceOfPath > pcAI->GetDistance(myPosition, pTemp->second->tPos));
		pcAI->DrawLine(myPosition, pcAI->m_pMiniGoalTri->tPos, D3DXCOLOR(0, 255, 0, 255));
		pcAI->DrawLine(myPosition, pTemp->second->tPos, D3DXCOLOR(126, 0, 126, 255));
	}

	if(pcAI->m_pMiniGoalTri == NULL)
	{
		return;
	}

	// Find which edge the heading collides with
	pFindIter = cAIManager->m_cTris.find(pcAI->m_nCurrentTriangle);
	TRay HeadingRay;
	HeadingRay.cRayStart = myPosition;
	HeadingRay.cRayNormal = Heading * 20.0f;
	for(int nEdge = 0; nEdge < 3; ++nEdge)
	{
		if(cCollision->RayToLineIntersection(HeadingRay, 
			pFindIter->second->tEdges[nEdge]) == true)
		{
			nHeadingEdge = nEdge;
			break;
		}
	}

	// Error checking
	if( (nHeadingEdge == -1) )
	{
		pcAI->DrawLine(myPosition, Heading + myPosition, D3DXCOLOR(0, 255, 255, 255));
		pcAI->EndDraw();
		return;
	}

	// Move
	nAStarEdge = cAIManager->m_nLookup[cAIManager->LookUpCellAlgorithm(pcAI->m_nCurrentTriangle,
			pcAI->m_nTargetTriangle)];

	D3DXVECTOR3 ToGoalNorm;
	ToGoal = pcAI->m_pMiniGoalTri->tPos - myPosition;
	if(nMiniGoalTriID == -1)
	{
		// Steer towards goal
		ToGoal = pcAI->m_vGoalPosition - myPosition;

		D3DXVECTOR3 cRtVec, cRtVecNorm;
		cRtVec.x = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._11;
		cRtVec.y = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._12;
		cRtVec.z = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._13;
		cRtVec *= -1;
		D3DXVec3Normalize(&cRtVecNorm, &cRtVec);
		D3DXVec3Normalize(&ToGoalNorm, &ToGoal);

		float fDot = D3DXVec3Dot(&ToGoalNorm, &cRtVecNorm);

		if(fDot > 0.0f)
		{
			if(pcAI->GetIsActive())
			{
				SendInputEvent("SteerRight", pcAI, pcAI->m_pObject, 1.0f);
			}
		}
		else
		{
			if(pcAI->GetIsActive())
			{
				SendInputEvent("SteerLeft", pcAI, pcAI->m_pObject, 1.0f);
			}
		}

		if(pcAI->GetIsActive())
		{
			SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, 1.0f);
		}
	}
	else if(nHeadingEdge == nAStarEdge)
	{
		// Accelerate by some value
		D3DXVec3Normalize(&ToGoalNorm, &ToGoal);
		float fd = D3DXVec3Dot(&Heading, &ToGoalNorm);

		float fMove1 = 1 - (D3DXToDegree(pcAI->fTurnSpeed) * pcAI->fThreshold[0]) / 90.0f;
		float fMove2 = 1 - (D3DXToDegree(pcAI->fTurnSpeed) * pcAI->fThreshold[1]) / 90.0f;

		if(fd > fMove1)
		{			
			if(pcAI->GetIsActive())
			{
				SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, fd);
			}
		}
		else if( (fd > fMove2) &&
			(pcAI->m_fSpeed > 5) )
		{			
			if(pcAI->GetIsActive())
			{
				SendInputEvent("Decelerate", pcAI, pcAI->m_pObject, 1 - fd);
			}
		}
		else
		{			
			if(pcAI->GetIsActive())
			{
				SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, fabs(fd));
			}
		}
	}
	else if(pcAI->m_nCurrentTriangle == pcAI->m_nTargetTriangle)
	{
		// We're in the triangle we need to be in
		// So steer towards the goal
		if(pcAI->GetIsActive())
		{
			SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, 1.0f);
		}

		ToGoal = pcAI->m_vGoalPosition - myPosition;

		// Check if we should turn left or right
		D3DXVECTOR3 cRtVec, cRtVecNorm;
		cRtVec.x = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._11;
		cRtVec.y = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._12;
		cRtVec.z = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._13;
		cRtVec *= -1;
		D3DXVec3Normalize(&cRtVecNorm, &cRtVec);
		D3DXVec3Normalize(&ToGoalNorm, &ToGoal);
		float fDot = D3DXVec3Dot(&ToGoalNorm, &cRtVecNorm);

		//cout << "Inside target triangle!" << endl;

		if(fDot > 0.0f)
		{
			//cout << "Turning Right" << endl;

			if(pcAI->GetIsActive())
			{
				SendInputEvent("SteerRight", pcAI, pcAI->m_pObject, 1.0f);
			}
		}
		else
		{
			//cout << "Turning Left" << endl;
			
			if(pcAI->GetIsActive())
			{
				SendInputEvent("SteerLeft", pcAI, pcAI->m_pObject, 1.0f);
			}
		}
	}
	else
	{
		// Steer Toward Next Tri
		if(pcAI->m_fSpeed > 5)
		{
			if(pcAI->GetIsActive())
			{
				SendInputEvent("Decelerate", pcAI, pcAI->m_pObject, 1.0f);
			}
		}
		else
		{
			if(pcAI->GetIsActive())
			{
				SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, 1.0f);
			}
		}

		D3DXVECTOR3 cRtVec, cRtVecNorm;
		cRtVec.x = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._11;
		cRtVec.y = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._12;
		cRtVec.z = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._13;
		cRtVec *= -1;
		D3DXVec3Normalize(&cRtVecNorm, &cRtVec);
		D3DXVec3Normalize(&ToGoalNorm, &ToGoal);

		float fDot = D3DXVec3Dot(&ToGoalNorm, &cRtVecNorm);

		if(fDot > 0.0f)
		{
			if(pcAI->GetIsActive())
			{
				SendInputEvent("SteerRight", pcAI, pcAI->m_pObject, 1.0f);
			}
		}
		else
		{
			if(pcAI->GetIsActive())
			{
				SendInputEvent("SteerLeft", pcAI, pcAI->m_pObject, 1.0f);
			}
		}
	}

	pcAI->DrawLine(myPosition, Heading + myPosition, D3DXCOLOR(0, 255, 255, 255));

	pcAI->EndDraw();
}
#pragma endregion

	pcAI->UseHeldItem();
}

void CAIComponent::Update()
{
	// This horrible thing is how you
	// call a member function pointer
	// that calls a member function
	//((*this).*(m_pfUpdateState))();
}

void CAIComponent::UpdateCollective()
{
	//EvaluateOpponentWeights();
	//EvaluateHeldItemWeights();
	EvaluateGoalItemWeights();
}

void CAIComponent::UpdateAggressive()
{
	bool bPassive = false;
	EvaluateOpponentWeights();

	if(GetDistance(m_pTargetPlayer->GetTransform()->GetWorldPosition(),
		m_pObject->GetTransform()->GetWorldPosition()) < 250)
	{
		if(m_fSpeed < m_fRammingSpeed)
		{
			// We aren't going fast enough
			bPassive = true;
		}
	}

	if(bPassive == true)
	{
		m_bTargetItem = false;
		EvaluateGoalItemWeights();
		EvaluateHeldItemWeights();

		if(m_bTargetItem == false)
		{
			map<int, TTri*, less<int>,
				CAllocator<pair<int, TTri*>>>::iterator pIter;

			pIter = 
				CAIManager::GetInstance()->m_cTris.find(
				rand() % CAIManager::GetInstance()->m_cTris.size());

			m_vGoalPosition = pIter->second->tPos;
		}
	}
}

void CAIComponent::UpdateDefensive()
{
	EvaluateGoalItemWeights();
	EvaluateHeldItemWeights();
}

CObject* CAIComponent::GetOpponent(int nPlayerID)
{
	CAIManager* pCAIManager = CAIManager::GetInstance();
	list<CObject*,	CAllocator<CObject*>>::iterator OpponentIter = 
		pCAIManager->m_cPlayers.begin();

	while(OpponentIter != pCAIManager->m_cPlayers.end())
	{
		if((*OpponentIter)->GetID() == nPlayerID)
		{
			return *OpponentIter;;
		}

		OpponentIter++;
	}

	return NULL;
}

D3DXVECTOR3 CAIComponent::GetOpponentPos(int nPlayerID)
{
	CAIManager* pCAIManager = CAIManager::GetInstance();
	list<CObject*,	CAllocator<CObject*>>::iterator OpponentIter = 
		pCAIManager->m_cPlayers.begin();

	D3DXVECTOR3 cPlayerPos;

	while(OpponentIter != pCAIManager->m_cPlayers.end())
	{
		if((*OpponentIter)->GetID() == nPlayerID)
		{
			cPlayerPos = (*OpponentIter)->GetTransform()->
				GetWorldPosition();

			return cPlayerPos;
		}

		OpponentIter++;
	}

	return D3DXVECTOR3(0, 0, 0);
}

CObject* CAIComponent::GetClosestOpponent()
{
	CAIManager* pCAIManager = CAIManager::GetInstance();
	list<CObject*,	CAllocator<CObject*>>::iterator OpponentIter = 
		pCAIManager->m_cPlayers.begin();

	CObject* cOpponents[3];
	float fDist[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
	unsigned index = 0;

	while((OpponentIter != pCAIManager->m_cPlayers.end()))
	{
		if((*OpponentIter)->GetID() != m_pObject->GetID())
		{
			cOpponents[index] = *OpponentIter;
			fDist[index] = GetDistance(m_pObject->GetTransform()->GetWorldPosition(),
				(*OpponentIter)->GetTransform()->GetWorldPosition());
			++index;
		}

		++OpponentIter;
	}

	if((fDist[0] <= fDist[1]) && (fDist[0] <= fDist[2]))
	{
		return cOpponents[0];
	}
	else if(fDist[1] <= fDist[2])
	{
		return cOpponents[1];
	}
	else
	{
		return cOpponents[2];
	}
}

void CAIComponent::GoalItemInit(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemEvent* pcGoalItem = (TGoalItemEvent*) piEvent->GetData();

	pcAI->m_tKnowledge.m_cNeededGoalItems.push_back(
		pcGoalItem->m_eGoalItemType);
}

void CAIComponent::GoalItemCollected(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemCollectedEvent* pcCollected = 
		(TGoalItemCollectedEvent*)piEvent->GetData();

	EGoalItemType pType = CSpawningManager::GetInstance()->
					GetGoalItemType(pcCollected->m_pcGoalItem);

	// Look for which player found it
	// if Our ID == Collector's ID
	if(pcAI->m_pObject->GetID() != pcCollected->m_pcCollector->GetID())
	{
		// We didn't collect it
		// For each opponent...
		for(unsigned nPlayer = 0; nPlayer < 3; ++nPlayer)
		{
			// If we found the opponent who collected the item
			if(pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_nPlayer ==
				pcCollected->m_pcCollector->GetID())
			{
				// Duplicate check
				bool bDupe = false;
				
				list<EGoalItemType, CAllocator<EGoalItemType>>::iterator
					ItemIter = pcAI->m_tKnowledge.m_cOpponents[nPlayer]
				.m_cGoalItems.begin();

				// Loop through the Goal Items this opponent
				// already collected
				while(ItemIter != 
					pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.end())
				{
					// Check againts the Object's ID since that's how
					// the AI stores needed/collected Goal Items
					if((*ItemIter) == pType)
					{
						// We already have them marked down as
						// collecting that Goal Item
						bDupe = true;
						break;
					}
					ItemIter++;
				}

				// If they didn't already collect the Goal Item
				if(bDupe == false)
				{
					// Increment the amount of Goal Items we know they have
					++pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_chAmtGoalItems;

					// Get their position
					D3DXVECTOR3 cOpponentPos;
					cOpponentPos = pcCollected->m_pcCollector->GetTransform()->
						GetWorldPosition();

					// Look to see if it was in radar
					//if(WithinRadar(cOpponentPos))
					{
						// Add the Goal Item into their known
						// collected Goal Items
						pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.
							push_back(pType);
					}

					break;
				}
			}
		}
	}
	else // This AI collected the Goal Item
	{
		list<EGoalItemType, CAllocator<EGoalItemType>>::iterator 
			cNeededIter = pcAI->m_tKnowledge.m_cNeededGoalItems.begin();

		// Loop through our needed list
		while(cNeededIter != pcAI->m_tKnowledge.m_cNeededGoalItems.end())
		{
			if((*cNeededIter) == pType)
			{
				// Remove from our needed list
				MyDelete<EGoalItemType>(&(*cNeededIter), HEAPID_GENERAL);
				pcAI->m_tKnowledge.m_cNeededGoalItems.erase(cNeededIter);
				break;
			}

			cNeededIter++;
		}
	}

	// Remove Goal Item from spawned list
	list<TGoalItem, CAllocator<TGoalItem>>::iterator cGoalItemIter =
		pcAI->m_tKnowledge.m_cGoalItemsInLevel.begin();

	while(cGoalItemIter != pcAI->m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		if(cGoalItemIter->m_nGoalItem == pType)
		{
			MyDelete<TGoalItem>(&(*cGoalItemIter), HEAPID_GENERAL);
			pcAI->m_tKnowledge.m_cGoalItemsInLevel.erase(cGoalItemIter);
			return;
		}

		cGoalItemIter++;
	}
}

void CAIComponent::PlayerAttacked(IEvent* pEvent, IComponent* pComp)
{
	TImpactEvent* pImpactEvent = (TImpactEvent*)pEvent->GetData();
	CAIComponent* pcAI = (CAIComponent*)pComp;

	if(pImpactEvent->m_pcCollider->GetID() != pcAI->m_pObject->GetID())
	{
		// Not us
		return;
	}

	if( (pcAI->m_pTargetPlayer == NULL) ||
		(pImpactEvent->m_pCollidedWith->GetID() != pcAI->m_pTargetPlayer->GetID()) )
	{
		// Either we don't have a target,
		// or hit player isn't our target
		return;
	}

	if(pcAI->m_fSpeed >= pcAI->m_fRammingSpeed)
	{
		// We're going fast enough to ram
	}
	else
	{
		// We need to gain more speed
	}
}

void CAIComponent::GoalItemSpawned(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemEvent* pcGoalItem = (TGoalItemEvent*) piEvent->GetData();

	TGoalItem tGoalItem;
	tGoalItem.m_nGoalItem = pcGoalItem->m_eGoalItemType;
	tGoalItem.m_pfWeight = 0;
	tGoalItem.m_cPos.x = pcGoalItem->m_pcGoalItem->GetParent()->GetTransform()->GetWorldMatrix()._41;
	tGoalItem.m_cPos.y = pcGoalItem->m_pcGoalItem->GetParent()->GetTransform()->GetWorldMatrix()._42;
	tGoalItem.m_cPos.z = pcGoalItem->m_pcGoalItem->GetParent()->GetTransform()->GetWorldMatrix()._43;
	pcAI->m_tKnowledge.m_cGoalItemsInLevel.push_back(tGoalItem);
}

void CAIComponent::GoalItemDespawned(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemEvent* pcGoalItem = (TGoalItemEvent*) piEvent->GetData();

	list<TGoalItem, CAllocator<TGoalItem>>::iterator cGoalItemIter =
		pcAI->m_tKnowledge.m_cGoalItemsInLevel.begin();

	while(cGoalItemIter != pcAI->m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		if(cGoalItemIter->m_nGoalItem == pcGoalItem->m_eGoalItemType)
		{
			MyDelete<TGoalItem>(&(*cGoalItemIter), HEAPID_GENERAL);
			pcAI->m_tKnowledge.m_cGoalItemsInLevel.erase(cGoalItemIter);
			return;
		}

		cGoalItemIter++;
	}
}
// Events End

void CAIComponent::EvaluateStateWeights()
{
	if(bLogical == false)
	{
		return;
	}

	m_pfWeight[COLLECTIVE] = 0;
	m_pfWeight[AGGRESSIVE] = 0;
	m_pfWeight[DEFENSIVE] = 0;

	// Iterator for Goal Items in the level
	list<TGoalItem, CAllocator<TGoalItem>>::iterator			ItemIter;

	// Iterator for any needed Goal Item
	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator	NeededIter;

	// Iterator for any Goal Item the opponent has
	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator	OpponentIter;

	// Assign the Goal Item Level iter
	ItemIter = m_tKnowledge.m_cGoalItemsInLevel.begin();
	
	// Increase the collective weight per
	// Goal Item needed
	while(ItemIter != m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		// Reset the NeededIter
		NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

		while(NeededIter != m_tKnowledge.m_cNeededGoalItems.end())
		{
			if(*NeededIter == ItemIter->m_nGoalItem)
			{
				m_pfWeight[COLLECTIVE] += 0.2f;
				break;
			}

			NeededIter++;
		}

		ItemIter++;
	}

	// Get an iterator for all the dropped
	// items in the level
	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator pDroppedIter = 
		m_tKnowledge.m_cDroppedItems.begin();

	// Increase the collective weight
	// for all dropped items in the level
	// that are needed
	while(pDroppedIter != m_tKnowledge.m_cDroppedItems.end())
	{
		// Reset the NeededIter iterator
		NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

		while(NeededIter != m_tKnowledge.m_cNeededGoalItems.end())
		{
			if(*NeededIter == *pDroppedIter)
			{
				m_pfWeight[COLLECTIVE] += 0.2f;
				break;
			}
			++NeededIter;
		}
		++pDroppedIter;
	}

	// Aggressive Weight

	// Loop through each opponent and look to see if they have
	// a Goal Item that we need and increase aggressive weight
	// if they do
	//if(m_pfWeight[COLLECTIVE] < 0.35f)
	{
		for(unsigned nPlayers = 0; nPlayers < 3; ++nPlayers)
		{
			// Reset our knowledge of the
			// current player
			m_tKnowledge.m_cOpponents[nPlayers].m_bHasItem = false;

			if(m_tKnowledge.m_cOpponents[nPlayers].m_chAmtGoalItems == 8)
			{
				m_pfWeight[AGGRESSIVE] += 1.0f;
			}

			// Set iterator for opponent's Goal Items
			OpponentIter = 
				m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.begin();

			// Increase aggressive weight depending
			// on how aggresive they have been
			m_pfWeight[AGGRESSIVE] += 
				m_tKnowledge.m_cOpponents[nPlayers].m_nAggression * 0.1f;

			// Mark them as target if they keep
			// attacking us
			//if(m_tKnowledge.m_cOpponents[nPlayers].m_nAggression > 2)
			//{
			//	m_pfWeight[AGGRESSIVE] += 0.15f;
			//}

			// Search through their list of Goal Items
			while(OpponentIter != 
				m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.end())
			{
				NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

				// Search through our needed list with
				// with what they have
				while(NeededIter != m_tKnowledge.m_cNeededGoalItems.end())
				{
					// Increase aggressive weight if they
					// have an item we want
					if(*OpponentIter == *NeededIter)
					{
						m_pfWeight[AGGRESSIVE] += 0.1f;
						m_tKnowledge.m_cOpponents[nPlayers].m_bHasItem = true;
						m_tKnowledge.m_cOpponents[nPlayers].m_nItem = 
							*NeededIter;
					}

					NeededIter++;
				}

				OpponentIter++;
			}
		}
	}

	// Increase defensive weight if
	// collective or aggressive are low
	if(m_pfWeight[COLLECTIVE] < 0.20f)
	{
		m_pfWeight[DEFENSIVE] += 0.30f;
	}
	if(m_pfWeight[AGGRESSIVE] <= 0.15f)
	{
		m_pfWeight[DEFENSIVE] += 0.15f;
	}

	// Get New State
	if((m_pfWeight[DEFENSIVE] > m_pfWeight[COLLECTIVE]) &&
		(m_pfWeight[DEFENSIVE] > m_pfWeight[AGGRESSIVE]))
	{
		m_eCurrentState = DEFENSIVE;
		UpdateDefensive();
	}
	else if(m_pfWeight[COLLECTIVE] >= m_pfWeight[AGGRESSIVE])
	{
		m_eCurrentState = COLLECTIVE;
		UpdateCollective();
	}
	else
	{
		m_eCurrentState = AGGRESSIVE;
		UpdateAggressive();
	}
}

void CAIComponent::EvaluateHeldItemWeights()
{
	if( (m_tKnowledge.m_peMyHeldItems[0] != NO_HELD_ITEM) &&
		(m_tKnowledge.m_peMyHeldItems[1] != NO_HELD_ITEM) )
	{
		// Either we're already going towards
		// a Held Item or we can't hold any more
		return;
	}

	THeldItem tHeldItem;
	bool bPowerup;
	
	if(m_pfWeight[COLLECTIVE] > m_pfWeight[AGGRESSIVE])
	{
		bPowerup = true;
	}
	else
	{
		bPowerup = false;
	}

	if(FindNearbyHeldItem(bPowerup, tHeldItem))
	{
		m_bTargetItem = true;
		m_vGoalPosition = tHeldItem.m_cPos;
	}
}

void CAIComponent::EvaluateGoalItemWeights()
{
	list<TGoalItem, CAllocator<TGoalItem>>::iterator			ItemIter;
	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator	NeededIter;
	D3DXVECTOR3 VecTo;
	float fMyDist(0);
	float fDist(0);

	// See if there's a dropped item nearby
	// that we need
	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator	DroppedIter;
	DroppedIter = m_tKnowledge.m_cDroppedItems.begin();

	CPickupItemComponent* pPickup;
	while(DroppedIter != m_tKnowledge.m_cDroppedItems.end())
	{
		pPickup = CPickupItemManager::GetInstance()->GetPickupComp(
			(*DroppedIter));

		if(pPickup == NULL)
		{
			++DroppedIter;
			continue;
		}

		if(GetDistance(pPickup->GetObject()->GetTransform()->GetWorldPosition(),
			m_pObject->GetTransform()->GetWorldPosition()) <= 400)
		{
			m_vGoalPosition = pPickup->GetObject()->GetTransform()->GetWorldPosition();
			return;
		}

		++DroppedIter;
	}

	ItemIter = m_tKnowledge.m_cGoalItemsInLevel.begin();
	TGoalItem* pcTarget = NULL;

	// Loop through each Goal Item which the AI knows is in the level.
	// {
	//		Loop through all Goal Items the AI currently needs and
	//		checks to see if any of the Goal Items in the level matches
	//		any of the Goal Items the AI needs.
	// }
	while(ItemIter != m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

		while(NeededIter != m_tKnowledge.m_cNeededGoalItems.end())
		{
			if(*NeededIter == ItemIter->m_nGoalItem)
			{
				// This will take the distance from each goal item
				// the agent is and make it a value between 0 - 1,
				// where the closer the item is, the higher the
				// value is. This is then added to the item's weight
				D3DXVECTOR3 cMe;
				D3DXVECTOR3 cHeading, cHeadingNorm;
				D3DXVECTOR3 cToGoal, cToGoalNorm;
				cMe.x = m_pObject->GetTransform()->GetWorldMatrix()._41;
				cMe.y = m_pObject->GetTransform()->GetWorldMatrix()._42;
				cMe.z = m_pObject->GetTransform()->GetWorldMatrix()._43;
				cHeading.x = m_pObject->GetTransform()->GetWorldMatrix()._31;
				cHeading.y = m_pObject->GetTransform()->GetWorldMatrix()._32;
				cHeading.z = m_pObject->GetTransform()->GetWorldMatrix()._33;

				cToGoal = ItemIter->m_cPos - cMe;
				D3DXVec3Normalize(&cHeadingNorm, &cHeading);
				D3DXVec3Normalize(&cToGoalNorm, &cToGoal);

				fMyDist = GetDistance(ItemIter->m_cPos, cMe);
				float MyWeight = sqrtf(fMyDist) + (m_fSpeed * D3DXVec3Dot(&cHeadingNorm, &cToGoalNorm));
				//cout << sqrtf(fMyDist) << " + (" << m_fSpeed << " * " << D3DXVec3Dot(&cHeadingNorm, &cToGoalNorm) <<
				//	") = " << MyWeight << endl;

				// This will loop through each opponent in your minimap
				// and find out their weight and then will subtract
				// from each of the item's weights
				//list<CObject*,	CAllocator<CObject*>>::iterator iter;
				//iter = CAIManager::GetInstance()->m_cPlayers.begin();

				CObject* pOpponent = GetClosestOpponent();
				//while((*iter) != pOpponent)
				//{
				//	++iter;
				//}

				D3DXVECTOR3 OpponentPos;
				D3DXVECTOR3 OpponentHeading;

				//for(unsigned nCurPlayer = 0; nCurPlayer <
				//	CAIManager::GetInstance()->m_cPlayers.size(); ++nCurPlayer)
				{
					// If the current player is not me
					if(pOpponent->GetID() != m_pObject->GetID())
					{
						OpponentPos.x = pOpponent->GetTransform()->GetWorldMatrix()._41;
						OpponentPos.y = pOpponent->GetTransform()->GetWorldMatrix()._42;
						OpponentPos.z = pOpponent->GetTransform()->GetWorldMatrix()._43;

						OpponentHeading.x = pOpponent->GetTransform()->GetLocalMatrix()._31;
						OpponentHeading.y = pOpponent->GetTransform()->GetLocalMatrix()._32;
						OpponentHeading.z = pOpponent->GetTransform()->GetLocalMatrix()._33;

						VecTo = ItemIter->m_cPos - OpponentPos;
						fDist = GetDistance(ItemIter->m_cPos, OpponentPos);

						D3DXVECTOR3 OpponentHeadingNorm, VecToNorm;
						D3DXVec3Normalize(&OpponentHeadingNorm, &OpponentHeading);
						D3DXVec3Normalize(&VecToNorm, &VecTo);

						//if(D3DXVec3Dot(&OpponentHeading, &VecTo) < 0)
						{
							// Find opponent in knowledge
							int nIndex;
							for(nIndex = 0; ; ++nIndex)
							{
								if(m_tKnowledge.m_cOpponents[nIndex].m_nPlayer == 
									pOpponent->GetID())
								{
									break;
								}
							}

							float OpponentWeight = sqrtf(fDist) + 
								m_tKnowledge.m_cOpponents[nIndex].m_fCurrentSpeed * 
								D3DXVec3Dot(&OpponentHeadingNorm, &VecToNorm);

							ItemIter->m_pfWeight = OpponentWeight;
							ItemIter->m_pfWeight -= MyWeight;
							cout << ItemIter->m_pfWeight << endl;

							if(ItemIter->m_pfWeight < 0.0f)
							{
								ItemIter->m_pfWeight = 0.0f;
							}

						}
					}
					//iter++;
				}

				if(pcTarget == NULL)
				{
					pcTarget = &(*ItemIter);
				}
				else
				{
					if(pcTarget->m_pfWeight < ItemIter->m_pfWeight)
					{
						pcTarget = &(*ItemIter);
					}
				}
			}

			NeededIter++;
		}

		ItemIter++;
	}

	if((pcTarget != NULL) && (pcTarget->m_pfWeight > 0))
	{
		m_bTargetItem = true;
		m_vGoalPosition = pcTarget->m_cPos;
	}
}

void CAIComponent::EvaluateOpponentWeights()
{
	CObject* pTempTargetPlayer;
	m_pTargetPlayer = NULL;

	for(unsigned CurOpponent = 0; CurOpponent < 
		CAIManager::GetInstance()->m_cPlayers.size(); ++CurOpponent)
	{
		if(m_tKnowledge.m_cOpponents[CurOpponent].m_chAmtGoalItems == 8)
		{
			m_pTargetPlayer = GetOpponent(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
			break;
		}

		if(m_tKnowledge.m_cOpponents[CurOpponent].m_bHasItem == true)
		{
			if(m_pTargetPlayer == NULL)
			{
				m_pTargetPlayer = GetOpponent(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
			}
			else
			{
				pTempTargetPlayer = GetOpponent(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);

				// If pTempTargetPlayer is < pTargetPlayer
				if(GetDistance(m_pTargetPlayer->GetTransform()->GetWorldPosition(), 
					m_pObject->GetTransform()->GetWorldPosition()) >
					GetDistance(pTempTargetPlayer->GetTransform()->GetWorldPosition(), 
					m_pObject->GetTransform()->GetWorldPosition()))
				{
					m_pTargetPlayer = GetOpponent(
						m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
				}
			}
		}
	}

	if(m_pTargetPlayer == NULL)
	{
		m_pTargetPlayer = GetClosestOpponent();
	}

	if(m_pTargetPlayer != NULL)
	{
		m_vGoalPosition = m_pTargetPlayer->GetTransform()->GetWorldPosition();
	}
}

bool CAIComponent::WithinRadar(const D3DXVECTOR3& cPos) const
{
	return GetDistance(m_pObject->GetTransform()->GetWorldPosition(), cPos) < MINIMAP_RADIUS;
}

float CAIComponent::GetDistance(const D3DXVECTOR3 &cPos1, const D3DXVECTOR3 &cPos2) const
{
	float x = cPos2.x - cPos1.x;
	float z = cPos2.z - cPos1.z;
	return ( (x * x) + (z * z) );
}

void CAIComponent::UpdateSpeeds(IEvent* pEvent, IComponent* pComp)
{
	TSpeedEvent* pSpeed = (TSpeedEvent*)pEvent->GetData();
	CAIComponent* pcAI = (CAIComponent*)pComp;

	if(pcAI->m_pObject->GetID() == pSpeed->m_pObject->GetID())
	{
		pcAI->m_fSpeed = pSpeed->m_fSpeed;
		return;
	}

	for(int nIndex = 0; nIndex < 3; ++nIndex)
	{
		if(pcAI->m_tKnowledge.m_cOpponents[nIndex].m_nPlayer ==
			pSpeed->m_pObject->GetID())
		{
			pcAI->m_tKnowledge.m_cOpponents[nIndex].m_fCurrentSpeed = 
				pSpeed->m_fSpeed;

			return;
		}
	}
}

bool CAIComponent::IsPowerup(int eType)
{
	return eType <= MAX_ATTACK_ITEMS;
}

void CAIComponent::HeldItemSpawned(IEvent* pEvent, IComponent* pComp)
{
	THeldItemSpawned* pHeldItem = (THeldItemSpawned*)pEvent->GetData();
	CAIComponent* pcAI = (CAIComponent*)pComp;

	THeldItem tHeldItem;
	tHeldItem.m_cPos = pHeldItem->m_pHeldItem->GetParent()->GetTransform()->GetWorldPosition();
	tHeldItem.m_nHeldItem = pHeldItem->m_pHeldItem->GetType();
	tHeldItem.m_bPowerup = pcAI->IsPowerup(pHeldItem->m_pHeldItem->GetType());
	tHeldItem.m_nID = pHeldItem->m_pHeldItem->GetParent()->GetID();
	pcAI->m_tKnowledge.m_cHeldItemsInLevel.push_back(tHeldItem);
}

void CAIComponent::HeldItemCollected(IEvent* pEvent, IComponent* pComp)
{
	THeldItemCollected* pHeldItem = (THeldItemCollected*)pEvent->GetData();
	CAIComponent* pcAI = (CAIComponent*)pComp;

	if(pcAI->m_tKnowledge.m_cHeldItemsInLevel.size() == 0)
	{
		return;
	}

	list<THeldItem, CAllocator<THeldItem>>::iterator pHeldItemIter =
		pcAI->m_tKnowledge.m_cHeldItemsInLevel.begin();

	while(pHeldItemIter != pcAI->m_tKnowledge.m_cHeldItemsInLevel.end())
	{
		if((*pHeldItemIter).m_nID == 
			pHeldItem->m_pcHeldItem->GetID())
		{
			// Found our iterator
			break;
		}
		++pHeldItemIter;
	}

	if(pHeldItemIter == pcAI->m_tKnowledge.m_cHeldItemsInLevel.end())
	{
		return;
	}

	if(pHeldItem->m_pcCollector->GetID() == 
		pcAI->m_pObject->GetID())
	{
		for(int nItem = 0; nItem < 2; ++nItem)
		{
			if(pcAI->m_tKnowledge.m_peMyHeldItems[nItem] == NO_HELD_ITEM)
			{
				pcAI->m_tKnowledge.m_peMyHeldItems[nItem] = pHeldItemIter->m_nHeldItem;
				break;
			}
		}
	}

	if(pHeldItemIter != pcAI->m_tKnowledge.m_cHeldItemsInLevel.end())
	{
		MyDelete<THeldItem>(&(*pHeldItemIter), HEAPID_GENERAL);
		pcAI->m_tKnowledge.m_cHeldItemsInLevel.erase(pHeldItemIter);
	}
}

bool CAIComponent::FindNearbyHeldItem(bool bPowerup, THeldItem& tHeldItem)
{
	// Find the closest HeldItem to the AI
	// that is of the correct type

	if(m_tKnowledge.m_cHeldItemsInLevel.size() == 0)
	{
		return false;
	}
	
	// This will be our starting
	// distance. It will be increased
	// until we find an item, or it
	// reaches the size of X
	float fIncrement = 50.0f;
	float fDist = fIncrement;

	list<THeldItem, CAllocator<THeldItem>>::iterator pHeldItemIter;

	while(fDist < 3600)
	{
		pHeldItemIter = m_tKnowledge.m_cHeldItemsInLevel.begin();

		while(pHeldItemIter != m_tKnowledge.m_cHeldItemsInLevel.end())
		{
			if(GetDistance(
				pHeldItemIter->m_cPos, 
				m_pObject->GetTransform()->GetWorldPosition()) <= fDist)
			{
				if(IsPowerup(pHeldItemIter->m_nHeldItem) == bPowerup)
				{
					if(m_eCurrentState == AGGRESSIVE)
					{
						if( (pHeldItemIter->m_nHeldItem == TURKEY) ||
							(pHeldItemIter->m_nHeldItem == SOUP) ||
							(pHeldItemIter->m_nHeldItem == DONUT) )
						{
							tHeldItem = *pHeldItemIter;
							return true;
						}
					}
					else
					{
						tHeldItem = *pHeldItemIter;
						return true;
					}
				}
			}

			++pHeldItemIter;
		}
		fDist += fIncrement;
	}

	return false;
}

void CAIComponent::UseHeldItem()
{
	// Check to see if the agent has any held items
	if( (m_tKnowledge.m_peMyHeldItems[0] == NO_HELD_ITEM) &&
		(m_tKnowledge.m_peMyHeldItems[1] == NO_HELD_ITEM) )
	{
		return;
	}

	list<CObject*,	CAllocator<CObject*>>::iterator OpponentIter = 
		CAIManager::GetInstance()->m_cPlayers.begin();

	CObject* pOpponents[3];
	int index = 0;

	// Get a pointer to each opponent
	while((OpponentIter != CAIManager::GetInstance()->m_cPlayers.end()))
	{
		if((*OpponentIter)->GetID() != m_pObject->GetID())
		{
			pOpponents[index] = *OpponentIter;
			++index;
		}

		++OpponentIter;
	}

	D3DXVECTOR3 MyHeading;
	D3DXVECTOR3 OpponentHeading;
	D3DXVECTOR3 MyHeadingNorm;
	D3DXVECTOR3 OpponentHeadingNorm;
	D3DXVECTOR3 ToPlayerPos;
	D3DXVECTOR3 ToPlayerNorm;
	D3DXVECTOR3 ToAgent;
	float fDot;

	// Get agent's heading
	MyHeading.x = m_pObject->GetTransform()->GetWorldMatrix()._31;
	MyHeading.y = m_pObject->GetTransform()->GetWorldMatrix()._32;
	MyHeading.z = m_pObject->GetTransform()->GetWorldMatrix()._33;
	D3DXVec3Normalize(&MyHeadingNorm, &MyHeading);
	
	// Use anything that we don't
	// need an opponent around for
	Use(JAM);

	// Loop through each opponent and see
	// if we should use an item that needs
	// los or distance requirements
	for(int i = 0; i < (CAIManager::GetInstance()->m_cPlayers.size() - 1); ++i)
	{
		// Get opponent's heading
		OpponentHeading.x = pOpponents[i]->GetTransform()->GetWorldMatrix()._31;
		OpponentHeading.y = pOpponents[i]->GetTransform()->GetWorldMatrix()._32;
		OpponentHeading.z = pOpponents[i]->GetTransform()->GetWorldMatrix()._33;
		D3DXVec3Normalize(&OpponentHeadingNorm, &OpponentHeading);

		// Check Turkey and Pie

		// Get vector from agent's heading to opponent
		m_tTargetPlayerLine.cRayNormal = 
			pOpponents[i]->GetTransform()->GetWorldPosition() - 
			(MyHeading + m_pObject->GetTransform()->GetWorldPosition());

		// Check the distance
		if(GetDistance(m_tTargetPlayerLine.cRayStart, m_tTargetPlayerLine.cRayNormal) > 400)
		{
			// Target is too far away;
			continue;
		}

		// Normalize and dot product
		D3DXVec3Normalize(&ToPlayerPos, &m_tTargetPlayerLine.cRayNormal);
		fDot = D3DXVec3Dot(&MyHeadingNorm, &ToPlayerPos);

		// Check to see if they're infront
		if(fDot > 0.95f)
		{
			Use(TURKEY);
			Use(PIE);
		}

		// Check Soup

		// Get vector from agent to opponent
		ToPlayerPos = m_pObject->GetTransform()->GetWorldPosition() -
			(OpponentHeading + pOpponents[i]->GetTransform()->GetWorldPosition());

		// Normalize and dot product
		D3DXVec3Normalize(&ToPlayerNorm, &ToPlayerPos);
		fDot = D3DXVec3Dot(&OpponentHeadingNorm, &ToPlayerNorm);

		if(fDot > 0.95f)
		{
			Use(SOUP);
		}

		// Use dropped items since someone
		// is nearby
		Use(BANANA);
		Use(PEANUT_BUTTER);

		// Use Donut when about to ram
		if(m_eCurrentState == AGGRESSIVE)
		{
			Use(DONUT);
		}
	}
}

void CAIComponent::SetRamSpeed(IEvent* pEvent, IComponent* pComp)
{
	TSpeedEvent* pSpeedEvent = (TSpeedEvent*)pEvent->GetData();
	CAIComponent* pcAI = (CAIComponent*)pComp;

	if(pSpeedEvent->m_pObject->GetID() == pcAI->m_pObject->GetID())
	{
		pcAI->m_fRammingSpeed = pSpeedEvent->m_fSpeed;
	}
}

void CAIComponent::ShowTurnSpeed(IEvent* pEvent, IComponent* pComp)
{
	TSpeedEvent* pSpeedEvent = (TSpeedEvent*)pEvent->GetData();
	CAIComponent* pcAI = (CAIComponent*)pComp;

	if(pSpeedEvent->m_pObject->GetID() == pcAI->m_pObject->GetID())
	{
		pcAI->fTurnSpeed = pSpeedEvent->m_fSpeed;
	}
}

void CAIComponent::PickupItemCollected(IEvent* pEvent, IComponent* pComp)
{
	TPickupItemCollectedEvent* pPickup = (TPickupItemCollectedEvent*)pEvent->GetData();
	CAIComponent* pcAI = (CAIComponent*)pComp;

	EGoalItemType pType = pPickup->m_eItemType;

	// Look for which player found it
	// if Our ID == Collector's ID
	if(pcAI->m_pObject->GetID() != pPickup->m_pcCollector->GetID())
	{
		// We didn't collect it
		// For each opponent...
		for(unsigned nPlayer = 0; nPlayer < 3; ++nPlayer)
		{
			// If we found the opponent who collected the item
			if(pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_nPlayer ==
				pPickup->m_pcCollector->GetID())
			{
				// Duplicate check
				bool bDupe = false;

				list<EGoalItemType, CAllocator<EGoalItemType>>::iterator
					ItemIter = pcAI->m_tKnowledge.m_cOpponents[nPlayer]
				.m_cGoalItems.begin();

				// Loop through the Goal Items this opponent
				// already collected
				while(ItemIter != 
					pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.end())
				{
					// Check againts the Object's ID since that's how
					// the AI stores needed/collected Goal Items
					if((*ItemIter) == pType)
					{
						// We already have them marked down as
						// collecting that Goal Item
						bDupe = true;
						break;
					}
					ItemIter++;
				}

				// If they didn't already collect the Goal Item
				if(bDupe == false)
				{
					// Increment the amount of Goal Items we know they have
					++pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_chAmtGoalItems;

					// Add the Goal Item into their known
					// collected Goal Items
					pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.
						push_back(pType);

					break;
				}
			}
		}
	}
	else // This AI collected the Goal Item
	{
		list<EGoalItemType, CAllocator<EGoalItemType>>::iterator 
			cNeededIter = pcAI->m_tKnowledge.m_cNeededGoalItems.begin();

		// Loop through our needed list
		while(cNeededIter != pcAI->m_tKnowledge.m_cNeededGoalItems.end())
		{
			if((*cNeededIter) == pType)
			{
				// Remove from our needed list
				MyDelete<EGoalItemType>(&(*cNeededIter), HEAPID_GENERAL);
				pcAI->m_tKnowledge.m_cNeededGoalItems.erase(cNeededIter);
				break;
			}

			cNeededIter++;
		}
	}

	// Remove Dropped Item from dropped list
	list<EGoalItemType, CAllocator<EGoalItemType>>::iterator cDroppedIter =
		pcAI->m_tKnowledge.m_cDroppedItems.begin();

	while(cDroppedIter != pcAI->m_tKnowledge.m_cDroppedItems.end())
	{
		if(*cDroppedIter == pType)
		{
			MyDelete<EGoalItemType>(&(*cDroppedIter), HEAPID_GENERAL);
			pcAI->m_tKnowledge.m_cDroppedItems.erase(cDroppedIter);
			return;
		}

		++cDroppedIter;
	}
}


void CAIComponent::PauseUpdateCallback(IEvent*, IComponent* pComp)
{
	// Get the Effect Comp
	CAIComponent* comp = (CAIComponent*)pComp;
	SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_UPDATE);
}
