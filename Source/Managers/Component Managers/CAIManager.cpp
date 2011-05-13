/*******************************************************************************
 * Filename:  		CAIManager.cpp
 * Date:      		04/04/2011
 * Mod. Date: 		05/12/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This is the manager for the
				CAIComponent components. Will hold a pointer
				to each of the CAIComponent objects and
				any static information they need.
 ******************************************************************************/
#include <fstream>
#include <string>

#include "CAIManager.h"
#include "..\..\CObject.h"
#include "..\..\Components\AI\CAIComponent.h"
#include "..\Global Managers\Rendering Managers\ModelManager.h"
#include "..\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Rendering Managers\Renderer.h"
#include "..\Global Managers\Memory Manager\CMemoryManager.h"
#include "..\Global Managers\Event Manager\CEventManager.h"
#include "..\Global Managers\Event Manager\EventStructs.h"
#include "..\Component Managers\CCollisionManager.h"
using namespace EventStructs;

CAIManager::CAIManager()
{
}

CAIManager* CAIManager::GetInstance()
{
	static CAIManager cAIManager;
	return &cAIManager;
}

void CAIManager::Init()
{
	LoadAIWaypoints();

	CEventManager::GetInstance()->RegisterEvent(
		"Shutdown", (IComponent*)GetInstance(), Shutdown);
}

void CAIManager::Update(IEvent*, IComponent*)
{
}

void CAIManager::LoadAIWaypoints()
{
	fstream fin("Resource/Node Map/SCD_AI Waypoints.nm", ios_base::binary |
		ios_base::in);

	int nStrLen;
	int nObjectCount = 0;
	TNode *tNode;
	if(fin.is_open())
	{
		// LOAD NODES
		if(fin.is_open())
		{
			fin.read((char*)&nObjectCount, sizeof(int));
			tNode = MMNEWARRAYEX(TNode, nObjectCount, HEAPID_GENERAL);
			for(int i = 0; i < nObjectCount; ++i)
			{
				// name length
				fin.read((char*)&nStrLen, sizeof(int));
				// name
				fin.read((char*)&tNode[i].szName, nStrLen);
				// worldMat
				fin.read((char*)&tNode[i].tWorldMatrix, sizeof(D3DXMATRIX));
			}

		}
		fin.close();
		fin.clear();
	}

	// Search for waypoint nodes
	ModelManager* pMM = ModelManager::GetInstance();
	int nMeshIndex;
	char chBuffer[64];
	char chCounter[32];
	int nNameLength;
	int nOffset;
	int nNodeNumber = -1;
	std::string szID;
	D3DXVECTOR3 cPos;
	for(int nIndex = 0; nIndex < nObjectCount; ++nIndex)
	{
		memset(chBuffer, 0, 64);
		nNameLength = strlen(tNode[nIndex].szName);
		nOffset = 0;

		while(isdigit(tNode[nIndex].szName[nNameLength - nOffset]) || 
			tNode[nIndex].szName[nNameLength - nOffset] == '\0')
		{
			++nOffset;
		}

		memcpy(chBuffer, tNode[nIndex].szName, nNameLength - nOffset + 1);
		chBuffer[nNameLength - nOffset + 1] = '\0';

		// check buffer for "Pathnode"
		if(stricmp(chBuffer, "Pathnode") == 0)
		{
			// Create an object for the node
			_itoa_s(++nNodeNumber, chCounter, 32, 10);
			szID = "Waypoint ";
			szID += chCounter;

			cPos.x = tNode[nIndex].tWorldMatrix._41;
			cPos.y = tNode[nIndex].tWorldMatrix._42;
			cPos.z = tNode[nIndex].tWorldMatrix._43;
			
			CVertex* newVertex = MMNEW(CVertex(
				CObjectManager::GetInstance()->CreateObject(
				szID, cPos.x, cPos.y, cPos.z)));

			newVertex->m_cLocation = cPos;

			// Add render component to it
			Renderer::GetInstance()->CreateRenderComp(
				newVertex->m_pcObject, 23, 0, 1);

			// Push the node into the map of nodes
			m_cWaypointNodes.push_back(newVertex);

			// Yay!!!!!!
		}
	}

	ConnectWaypoints();

	//MMDELARRAYEX(tNode, HEAPID_GENERAL);
}

void CAIManager::ConnectWaypoints()
{
	std::list<CVertex*, CAllocator<CVertex*>>::iterator pCurrNode = 
		m_cWaypointNodes.begin();

	std::list<CVertex*, CAllocator<CVertex*>>::iterator pIter =
		m_cWaypointNodes.begin();

	TLine tLine;
	D3DXVECTOR3 cClosestPoint;
	bool bCurrDupe = false;
	bool bIterDupe = false;

	while(pCurrNode != m_cWaypointNodes.end())
	{
		tLine.cLineStart = (*pCurrNode)->m_cLocation;

		while(pIter != m_cWaypointNodes.end())
		{
			if(*pCurrNode == *pIter)
			{
				++pIter;
				continue;
			}

			tLine.cLineEnd = (*pIter)->m_cLocation;
			
			if(CCollisionManager::GetInstance()->LineToWayPoint(
				tLine, cClosestPoint) == true)
			{
				// Add waypoint node to connections
				bCurrDupe = false;
				bIterDupe = false;

				// Test pCurrNode's edges for duplicates
				std::list<CEdge*, CAllocator<CEdge*>>::iterator pEdge = 
					(*pCurrNode)->m_pcConnections.begin();

				while(pEdge != (*pCurrNode)->m_pcConnections.end())
				{
					if((*pEdge)->m_pcEndpoint == (*pIter))
					{
						bCurrDupe = true;
						break;
					}

					++pEdge;
				}

				// pCurrNode's Edge
				if(bCurrDupe == false)
				{
					CEdge* pCurrEdge = MMNEW(CEdge);
					pCurrEdge->m_pcEndpoint = *pIter;

					(*pCurrNode)->m_pcConnections.push_back(pCurrEdge);
				}

				// Test pIter's edges for duplicates
				std::list<CEdge*, CAllocator<CEdge*>>::iterator pEdge2 = 
					(*pIter)->m_pcConnections.begin();

				while(pEdge2 != (*pIter)->m_pcConnections.end())
				{
					if((*pEdge2)->m_pcEndpoint == (*pCurrNode))
					{
						bIterDupe = true;
						break;
					}

					++pEdge2;
				}

				// pIter's Edge
				if(bIterDupe == false)
				{
					CEdge* pIterEdge = MMNEW(CEdge);
					pIterEdge->m_pcEndpoint = *pIter;

					(*pIter)->m_pcConnections.push_back(pIterEdge);	
				}
			}

			++pIter;
		}

		++pCurrNode;
	}
}

int CAIManager::CreateAIComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -1);
	lua_pop(pLua, 1);
	CAIComponent* pAI = CreateAIComponent(pObj);
	
	return 0;
}

CAIComponent* CAIManager::CreateAIComponent(CObject* pObj)
{
	CAIComponent* comp = MMNEW(CAIComponent(pObj));

	static int nNumCreated = 1;
	
	comp->Init();
	comp->SetControllerNumber(nNumCreated); // starts at 0, but that's player
	pObj->AddComponent(comp);

	CAIManager::GetInstance()->m_cAIComponents.insert(make_pair((unsigned int)
		pObj->GetID(), comp));

	nNumCreated++;

	return comp;
}

// Events Start
void CAIManager::GoalItemCollected(IEvent* iEvent, IComponent*)
{
}

void CAIManager::GoalItemSpawned(IEvent* iEvent, IComponent*)
{
}

void CAIManager::GoalItemDespawned(IEvent* iEvent, IComponent*)
{
}

void CAIManager::Shutdown(IEvent*, IComponent*)
{
	CAIManager* pAIManager = GetInstance();
	
	while(pAIManager->m_cAIComponents.empty() == false)
	{
		MMDEL(pAIManager->m_cAIComponents.begin()->second);

		pAIManager->m_cAIComponents.erase(
			pAIManager->m_cAIComponents.begin());
	}

	while(pAIManager->m_cWaypointNodes.empty() == false)
	{
		list<CVertex*, CAllocator<CVertex*>>::iterator pIter = 
			pAIManager->m_cWaypointNodes.begin();

		while((*pIter)->m_pcConnections.empty() == false)
		{
			std::list<CEdge*, CAllocator<CEdge*>>::iterator pEdgeIter =
				(*pIter)->m_pcConnections.begin();

			MyDelete<CEdge>((*pEdgeIter), HEAPID_GENERAL);
			(*pIter)->m_pcConnections.erase(
				(*pIter)->m_pcConnections.begin());
		}

		MyDelete<CVertex>((*pIter), HEAPID_GENERAL);
		pAIManager->m_cWaypointNodes.erase(
			pAIManager->m_cWaypointNodes.begin());
	}
}
// Events End