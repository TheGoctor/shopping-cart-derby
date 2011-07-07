/*******************************************************************************
 * Filename:  		CAIManager.cpp
 * Date:      		04/04/2011
 * Mod. Date: 		06/04/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This is the manager for the
				CAIComponent components. Will hold a pointer
				to each of the CAIComponent objects and
				any static information they need.
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <map>
using std::multimap;

#include "CAIManager.h"
#include "..\..\CObject.h"
#include "..\..\Components\AI\CAIComponent.h"
#include "..\Global Managers\Rendering Managers\ModelManager.h"
#include "..\Global Managers\Rendering Managers\Direct3DManager.h"
#include "..\Global Managers\Object Manager\CObjectManager.h"
#include "..\Global Managers\Rendering Managers\Renderer.h"
#include "..\Global Managers\Memory Manager\CMemoryManager.h"
#include "..\Global Managers\Event Manager\CEventManager.h"
#include "..\Global Managers\Event Manager\EventStructs.h"
#include "..\Global Managers\Console Manager\CConsoleManager.h"
#include "CCollisionManager.h"
#include "CLevelManager.h"
using namespace EventStructs;

int CAIManager::SetGoalTri(lua_State* pLua)
{
	string szName = lua_tostring(pLua, -2);
	int nTri = lua_tointeger(pLua, -1);
	lua_pop(pLua, 2);

	unsigned int nID = CIDGen::GetInstance()->GetID(szName.c_str());

	map<unsigned int, CAIComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator pIter = 
		GetInstance()->m_cAIComponents.find(nID);

	if(pIter == GetInstance()->m_cAIComponents.end())
	{
		cout << "Bad Iter" << endl;
		Debug.Print("Bad Iter");
		return 0;
	}

	char szDebug[128];

	if(nTri != -1)
	{
		map<int, TTri*, less<int>,
			CAllocator<pair<int, TTri*>>>::iterator pTriIter =
			GetInstance()->m_cTris.find(nTri);

		if(pTriIter == GetInstance()->m_cTris.end())
		{
			cout << "Bad triangle" << endl;
			Debug.Print("Bad triangle");
			return 0;
		}
		
		pIter->second->m_vGoalPosition = pTriIter->second->tPos;
		pIter->second->bLogical = false;
	}
	else
	{
		pIter->second->bLogical = true;
	}

	sprintf(szDebug, "Name: %s  Triangle: %d  ID: %d", szName.c_str(), nTri, nID);
	Debug.Print(szDebug);

	return 0;
}

CAIManager::CAIManager()
{
}

CAIManager* CAIManager::GetInstance()
{
	static CAIManager cAIManager;
	return &cAIManager;
}

void CAIManager::ToggleInfo(IEvent*, IComponent*)
{
	GetInstance()->m_bShowInfo = !GetInstance()->m_bShowInfo;
}

void CAIManager::Init()
{
	m_bShowInfo = false;
	SendStringEvent("LoadLevel", (IComponent*)GetInstance(), "");

	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), Update);
	pEM->RegisterEvent("GoalItemInit", (IComponent*)GetInstance(), GoalItemInit);
	pEM->RegisterEvent("PlayerCreated", (IComponent*)GetInstance(), SetupOpponents);
	pEM->RegisterEvent("ToggleInfo", (IComponent*)GetInstance(), ToggleInfo);
	pEM->RegisterEvent("LoadLevel", (IComponent*)GetInstance(), LoadLevel);
	pEM->RegisterEvent("Shutdown", (IComponent*)GetInstance(), Shutdown);

	// TODO : Register for events to remove/delete AIComponents
}

void CAIManager::LoadLevel(IEvent* pEvent, IComponent*)
{
	TStringEvent* pStringEvent = (TStringEvent*)pEvent->GetData();

	CAIManager::GetInstance()->LoadNavMesh(pStringEvent->m_szString);
	CAIManager::GetInstance()->CreateConnections(pStringEvent->m_szString);
	CAIManager::GetInstance()->CreateLookUpTable(pStringEvent->m_szString);
}

void CAIManager::RemoveAIComponent(IEvent* pEvent, IComponent*)
{
	TObjectEvent* pObjectEvent = (TObjectEvent*)pEvent->GetData();
	CObject* pObject = pObjectEvent->m_pcObj;

	CAIManager* pAIManager = GetInstance();

	map<unsigned int, CAIComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator pIter =
		pAIManager->m_cAIComponents.begin();

	while(pIter != pAIManager->m_cAIComponents.end())
	{
		if(pIter->second->m_pObject->GetID() == pObject->GetID())
		{
			pObject->RemoveComponent(pIter->second);
			pAIManager->m_cAwaitingDeletion.push_back(
				pIter->second);
			break;
		}
		++pIter;
	}
}

void CAIManager::DeleteAIComponent(IEvent*, IComponent*)
{
	CAIManager* pAIManager = GetInstance();

	list<CAIComponent*, CAllocator<CAIComponent*>>::iterator pIter =
		pAIManager->m_cAwaitingDeletion.begin();

	while(pIter != pAIManager->m_cAwaitingDeletion.end())
	{
		MMDEL(*pIter);
		++pIter;
	}
}

void CAIManager::SetupOpponents(IEvent* piEvent, IComponent* /*piComponent*/)
{
	// NOTE: This must be done AFTER the CAIComponent is created
	CAIManager* pcAIManager = GetInstance();
	TObjectEvent* pcObject = (TObjectEvent*)piEvent->GetData();

	pcAIManager->m_cPlayers.push_back(pcObject->m_pcObj);

	//if(pcAIManager->m_cPlayers.size() != 3)
	//{
	//	return;
	//}

	std::map<unsigned int, CAIComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator pCompAIIter;
	pCompAIIter = GetInstance()->m_cAIComponents.begin();

	while(pCompAIIter != GetInstance()->m_cAIComponents.end())
	{
		pCompAIIter->second->SetupOpponents(pcObject->m_pcObj);
		++pCompAIIter;
	}

	// Previously in Manager
	//std::list<CObject*,	CAllocator<CObject*>>::iterator pcPlayerIter = 
	//	CAIManager::GetInstance()->m_cPlayers.begin();
	//bool bAlreadyAdded = false;

	//while(pcPlayerIter != CAIManager::GetInstance()->m_cPlayers.end())
	//{
	//	if(*pcPlayerIter == pcObject->m_pcObj)
	//	{
	//		bAlreadyAdded = true;
	//		break;
	//	}
	//	pcPlayerIter++;
	//}

	//if(bAlreadyAdded == false)
	//{
	//	CAIManager::GetInstance()->m_cPlayers.push_back(pcObject->m_pcObj);
	//}

	//// Component part
	//if(pcAI->m_pObject->GetID() != pcObject->m_pcObj->GetID())
	//{
	//	pcAI->m_tKnowledge.m_cOpponents[pcAI->m_tKnowledge.m_nOpponentID].m_nPlayer =
	//		pcObject->m_pcObj->GetID();
	//	++pcAI->m_tKnowledge.m_nOpponentID;
	//}
}

void CAIManager::GoalItemInit(IEvent* piEvent, IComponent*)
{
	TGoalItemEvent* pcGoalItem = (TGoalItemEvent*) piEvent->GetData();

	GetInstance()->m_cInitedGoalItems.push_back(pcGoalItem->m_eGoalItemType);
}

void CAIManager::Update(IEvent*, IComponent*)
{
#if _DEBUG
#endif
}

void CAIManager::LoadNavMesh(string szFileName)
{
	DXMesh pMeshBuffer[75];
	unsigned int currIndex = 0;
	unsigned int num = 0;
	char buffer[128];

	m_cTris.clear();

	memset(buffer, 0, 128);

	string szPath = "Resource\\Node Map\\" + szFileName + "NavMesh.mesh";
	fstream fin(szPath.c_str(), ios_base::in | ios_base::binary);

	if(fin.is_open())
	{
		D3DXVECTOR3 *pos_buffer;
		D3DXVECTOR3 *norm_buffer;
		D3DXVECTOR2 *uv_buffer;
		DWORD  *indicies;

		// strlen
		fin.read((char*)&num, 4);
		// name
		fin.read((char*)&pMeshBuffer[currIndex].m_szName, num);
		// loop size
		fin.read((char*)&num, 4);
		VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[num];
		pos_buffer = new D3DXVECTOR3[num];
		norm_buffer = new D3DXVECTOR3[num];
		uv_buffer = new D3DXVECTOR2[num];
		// vertices
		for(unsigned int i = 0; i < num; ++i)
		{
			fin.read((char*)&pos_buffer[i].x, sizeof(float));
			fin.read((char*)&pos_buffer[i].y, sizeof(float));
			fin.read((char*)&pos_buffer[i].z, sizeof(float));
		}
		for(unsigned int i = 0; i < num; ++i)
		{
			verts[i].position.x = pos_buffer[i].x;
			verts[i].position.y = pos_buffer[i].y;
			verts[i].position.z = pos_buffer[i].z;

			verts[i].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			verts[i].uv.x = 0.0f;
			verts[i].uv.y = 0.0f;
		}

		fin.read((char*)&num, 4);
		indicies = new DWORD[num*3];
			
		fin.read((char*)indicies, num*12);

		int numTri = 0;
		for(unsigned i = 0; i < num*3;)
		{
			TTri* tri = MMNEW(TTri);

			for(int j = 0; j < 3; ++j)
			{
				tri->tEdges[j].cLineStart = verts[indicies[i]].position;
				tri->m_nIndice[j] = indicies[i];
				++i;
			}

			tri->tEdges[0].cLineEnd = tri->tEdges[1].cLineStart;
			tri->tEdges[1].cLineEnd = tri->tEdges[2].cLineStart;
			tri->tEdges[2].cLineEnd = tri->tEdges[0].cLineStart;

			tri->tPos = (tri->tEdges[0].cLineStart +
				tri->tEdges[1].cLineStart + 
				tri->tEdges[2].cLineStart) / 3;

			m_cTris.insert(make_pair(numTri, tri));
			++numTri;
		}

		delete [] pos_buffer;
		delete [] norm_buffer;
		delete [] uv_buffer;
		delete [] verts;
		delete [] indicies;

		fin.close();
		fin.clear();
	}
}

bool CAIManager::IsConnected(const TTri& lhs, const TTri& rhs) const
{
	int a, b, c, x, y, z;
	int connections = 0;

	a = lhs.m_nIndice[0];
	b = lhs.m_nIndice[1];
	c = lhs.m_nIndice[2];
	x = rhs.m_nIndice[0];
	y = rhs.m_nIndice[1];
	z = rhs.m_nIndice[2];

	if( (a == x) ||
		(a == y) ||
		(a == z))
	{
		++connections;
	}
	if( (b == x) ||
		(b == y) ||
		(b == z))
	{
		++connections;
	}
	if( (c == x) ||
		(c == y) ||
		(c == z))
	{
		++connections;
	}
	if(connections > 1)
	{
		return true;
	}

	return false;
}

void CAIManager::CreateConnections(string szFileName)
{
	string szPath = "Resource\\Node Map\\" + szFileName + "Connections.bin";
	fstream in(szPath.c_str(), ios_base::in | ios_base::binary);

	if(in.is_open())
	{
		map<int, TTri*, less<int>,
			CAllocator<pair<int, TTri*>>>::iterator pTriIter = m_cTris.begin();

		TTri* pConnection;

		int numConnections;
		int tri;

		while(pTriIter != m_cTris.end())
		{
			in.read((char*)&numConnections, 4);

			do
			{
				in.read((char*)&tri, 4);
				pConnection = m_cTris.find(tri)->second;
				pTriIter->second->m_cConnections.insert(make_pair(tri, pConnection));
			}
			while(pTriIter->second->m_cConnections.size() != numConnections);

			++pTriIter;
		}
		in.clear();
		in.close();
	}
	else
	{
		map<int, TTri*, less<int>,
			CAllocator<pair<int, TTri*>>>::iterator pStart = m_cTris.begin();

		map<int, TTri*, less<int>,
			CAllocator<pair<int, TTri*>>>::iterator pEnd;

		while(pStart != m_cTris.end())
		{
			pEnd = m_cTris.begin();

			int i = 0;
			while(pEnd != m_cTris.end())
			{
				if(pStart->second == pEnd->second)
				{
					++pEnd;
					continue;
				}

				if(IsConnected(*pStart->second, *pEnd->second))
				{
					pStart->second->m_cConnections.insert(make_pair(
						pEnd->first, pEnd->second));
				}

				++pEnd;
			}

			if(pStart->second->m_cConnections.size() == 0)
			{
				throw;
			}

			++pStart;
		}

		// Save out the connections
		fstream out(szPath.c_str(), ios_base::out | ios_base::binary);

		if(out.is_open())
		{
			pStart = m_cTris.begin();
			int numConnections;
			int first;

			while(pStart != m_cTris.end())
			{
				map<int, TTri*, less<int>,
					CAllocator<pair<int, TTri*>>>::iterator pConnectionIter = 
					pStart->second->m_cConnections.begin();

				// Number of connections
				numConnections = pStart->second->m_cConnections.size();
				out.write((char*)&numConnections, 4);

				// For each connection
				while(pConnectionIter != pStart->second->m_cConnections.end())
				{
					first = pConnectionIter->first;
					out.write((char*)&first, 4);
					++pConnectionIter;
				}

				++pStart;
			}
			out.clear();
			out.close();
		}
	}
}

void CAIManager::CreateLookUpTable(string szFileName)
{
	m_nLookup = MMNEWARRAYEX(char, m_cTris.size()*m_cTris.size(), HEAPID_GENERAL);

	string szPath = "Resource\\Node Map\\" + szFileName + "LookupTable.bin";
	fstream in(szPath.c_str(), ios_base::in | ios_base::binary);

	if(in.is_open())
	{
		in.read(m_nLookup, m_cTris.size()*m_cTris.size());
		in.close();
		in.clear();
	}
	else
	{
		for(unsigned nStart = 0; nStart < m_cTris.size(); ++nStart)
		{
			for(unsigned nEnd = 0; nEnd < m_cTris.size(); ++nEnd)
			{
				// Fill out the Lookup table
				m_nLookup[LookUpCellAlgorithm(nStart, nEnd)] = Pathplan(nStart, nEnd);
			}
		}

		// Write out to a file
		fstream out(szPath.c_str(), ios_base::out | ios_base::binary);

		if(out.is_open())
		{
			out.write(m_nLookup, m_cTris.size()*m_cTris.size());
			out.close();
			out.clear();
		}
	}
}

int CAIManager::CreateAIComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -1);
	lua_pop(pLua, 1);
	CreateAIComponent(pObj);
	
	return 0;
}

CAIComponent* CAIManager::CreateAIComponent(CObject* pObj)
{
	CAIComponent* comp = MMNEW(CAIComponent(pObj));

	static int nNumCreated = 1;

	comp->Init();
	comp->m_tKnowledge.m_nOpponentID = 0;
	comp->SetControllerNumber(nNumCreated); // starts at 0, but that's player
	pObj->AddComponent(comp);
	GetInstance()->m_cAIComponents.insert(make_pair((unsigned int)
		pObj->GetID(), comp));

	nNumCreated++;

	return comp;
}

// Events Start
void CAIManager::Shutdown(IEvent*, IComponent*)
{
	CAIManager* pAIManager = GetInstance();

	while(pAIManager->m_cOpen.empty() == false)
	{
		MyDelete<TPlannerNode>(*pAIManager->m_cOpen.begin(), HEAPID_GENERAL);

		pAIManager->m_cOpen.erase(
			pAIManager->m_cOpen.begin());
	}

	while(pAIManager->m_cAwaitingDeletion.empty() == false)
	{
		MyDelete<CAIComponent>(*pAIManager->m_cAwaitingDeletion.begin(),
			HEAPID_GENERAL);

		pAIManager->m_cAwaitingDeletion.erase(
			pAIManager->m_cAwaitingDeletion.begin());
	}

	while(pAIManager->m_cCreated.empty() == false)
	{
		MMDEL(pAIManager->m_cCreated.begin()->second);

		pAIManager->m_cCreated.erase(
			pAIManager->m_cCreated.begin());
	}

	while(pAIManager->m_cTris.empty() == false)
	{
		MMDEL(pAIManager->m_cTris.begin()->second);

		pAIManager->m_cTris.erase(
			pAIManager->m_cTris.begin());
	}
	
	while(pAIManager->m_cAIComponents.empty() == false)
	{
		MMDEL(pAIManager->m_cAIComponents.begin()->second);

		pAIManager->m_cAIComponents.erase(
			pAIManager->m_cAIComponents.begin());
	}

	while(pAIManager->m_cPlayers.empty() == false)
	{
		MMDEL(*(pAIManager->m_cPlayers.begin()));

		pAIManager->m_cPlayers.erase(
			pAIManager->m_cPlayers.begin());
	}

	while(pAIManager->m_cSpawnableGoalItems.empty() == false)
	{
		MMDEL(pAIManager->m_cSpawnableGoalItems.begin()->second);

		pAIManager->m_cSpawnableGoalItems.erase(
			pAIManager->m_cSpawnableGoalItems.begin());
	}

	map<int, TTri*, less<int>, 
		CAllocator<pair<int, TTri*>>>::iterator pTriIter = 
		pAIManager->m_cTris.begin();
	while(pTriIter != pAIManager->m_cTris.end())
	{
		MMDEL(pTriIter->second);
		++pTriIter;
	}
	pAIManager->m_cTris.clear();

	MMDELARRAYEX(pAIManager->m_nLookup, HEAPID_GENERAL);
	MMDEL(pAIManager->m_pNavMesh);
}
// Events End

int CAIManager::Pathplan(const int nStart, const int nEnd)
{
	// Make sure we aren't
	// pathfinding to ourselves
	if(nStart == nEnd)
	{
		return 3;
	}

	// Inits and Declares
	TPlannerNode* tCurrent;
	TTri* cLineStart = m_cTris[nStart];
	TTri* cLineEnd = m_cTris[nEnd];

	// Create a starting node
	TPlannerNode* cLineStartingNode = MMNEW(TPlannerNode);
	cLineStartingNode->m_pTriangle = cLineStart;
	cLineStartingNode->m_fHeuristicCost = sqrtf(
		GetDistance(cLineStart->tPos, cLineEnd->tPos));
	cLineStartingNode->m_fFinalCost =
		cLineStartingNode->m_fGivenCost + cLineStartingNode->m_fHeuristicCost;

	// Push it into the open container
	m_cOpen.insert(cLineStartingNode);

	// Put it into the created container
	m_cCreated[cLineStartingNode->m_pTriangle] = cLineStartingNode;

	while(m_cOpen.empty() == false)
	{
		// Get the current node
		tCurrent = *m_cOpen.begin();
		m_cOpen.erase(m_cOpen.begin());

		if(tCurrent->m_pTriangle == cLineEnd)
		{
			// Found our goal!
			while( (tCurrent->m_pParent != NULL) &&
				(tCurrent->m_pParent != cLineStartingNode) )
			{
				tCurrent = tCurrent->m_pParent;
			}

			// Leave this while loop
			// and clean up memory
			break;
		}

		// Get an iter for the current's connections
		map<int, TTri*, less<int>,
			CAllocator<pair<int, TTri*>>>::iterator pConnectionIter =
			tCurrent->m_pTriangle->m_cConnections.begin();

		// Create an iter for the created container
		map<TTri*, TPlannerNode*, less<TTri*>,
			CAllocator<pair<TTri*, TPlannerNode*>>>::iterator pCreatedIter;

		while(pConnectionIter != tCurrent->m_pTriangle->m_cConnections.end())
		{
			// Check to see if the current connection is already
			// in the created container
			pCreatedIter = m_cCreated.find(pConnectionIter->second);

			// Get temp cost
			float fTempCost = tCurrent->m_fGivenCost + sqrtf(
				GetDistance(tCurrent->m_pTriangle->tPos, pConnectionIter->second->tPos));

			if(pCreatedIter == m_cCreated.end())
			{
				// This triangle is not already in the created list
				// Therefore we add it
				TPlannerNode* tNewNode = MMNEW(TPlannerNode);
				
				tNewNode->m_pParent = tCurrent;
				tNewNode->m_pTriangle = pConnectionIter->second;
				tNewNode->m_fGivenCost = fTempCost;
				tNewNode->m_fHeuristicCost = sqrtf(
					GetDistance(tNewNode->m_pTriangle->tPos, cLineEnd->tPos));
				tNewNode->m_fFinalCost = tNewNode->m_fGivenCost + tNewNode->m_fHeuristicCost;
				
				m_cOpen.insert(tNewNode);
				m_cCreated[tNewNode->m_pTriangle] = tNewNode;
			}
			else
			{
				// This triangle is already in the created list
				// and we should check to see if it is more
				// optimal to take this path instead of the old
				if(pCreatedIter->second->m_fGivenCost > fTempCost)
				{
					m_cOpen.erase(pCreatedIter->second);
					pCreatedIter->second->m_fGivenCost = fTempCost;
					pCreatedIter->second->m_fFinalCost = 
						pCreatedIter->second->m_fGivenCost + pCreatedIter->second->m_fHeuristicCost;
					pCreatedIter->second->m_pParent = tCurrent;
					m_cOpen.insert(pCreatedIter->second);
				}
			}
			
			++pConnectionIter;
		}
	}

	// Make an iter of all the tris in the path
	map<int, TTri*, less<int>,
		CAllocator<pair<int, TTri*>>>::iterator pIter = m_cTris.begin();

	// Find the iterator so we
	// can return the index
	while( (pIter != m_cTris.end()) &&
		(pIter->second != tCurrent->m_pTriangle) )
	{
		++pIter;
	}

	// Make sure it's valid
	int nEdge = -1;
	int nTri = -1;
	if(pIter != m_cTris.end())
	{
		TLine a, b, c, x, y, z;

		a = cLineStart->tEdges[0];
		b = cLineStart->tEdges[1];
		c = cLineStart->tEdges[2];
		x = pIter->second->tEdges[0];
		y = pIter->second->tEdges[1];
		z = pIter->second->tEdges[2];

		if( (LinesEqual(a, x)) ||
			(LinesEqual(a, y)) ||
			(LinesEqual(a, z)) )
		{
			nEdge = 0;
		}
		else if( (LinesEqual(b, x)) ||
			(LinesEqual(b, y)) ||
			(LinesEqual(b, z)) )
		{
			nEdge = 1;
		}
		else if( (LinesEqual(c, x)) ||
			(LinesEqual(c, y)) ||
			(LinesEqual(c, z)) )
		{
			nEdge = 2;
		}
		else
		{
			throw;
		}

		nTri = pIter->first;
	}

	// Clean up memory
	map<TTri*, TPlannerNode*, less<TTri*>,
		CAllocator<pair<TTri*, TPlannerNode*>>>::iterator pDelIter = 
		m_cCreated.begin();
	while(pDelIter != m_cCreated.end())
	{
		MMDEL(pDelIter->second);
		++pDelIter;
	}
	m_cOpen.clear();
	m_cCreated.clear();

	return nEdge;
}

D3DXVECTOR3 CAIManager::ClosestPtPointTriangle(D3DXVECTOR3 p, D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c)
{
	// Check if P in vertex region outside A
	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ac = c - a;
	D3DXVECTOR3 ap = p - a;
	float d1 = D3DXVec3Dot(&ab, &ap);
	float d2 = D3DXVec3Dot(&ac, &ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return b; // barycentric coordinates (1,0,0)

	// Check if P in vertex region outside B
	D3DXVECTOR3 bp = p - b;
	float d3 = D3DXVec3Dot(&ab, &bp);
	float d4 = D3DXVec3Dot(&ac, &bp);
	if(d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)

	// Check if P in edge region of AB, if so return projection of P onto AB
	float vc = d1*d4 - d3*d2;
	if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		return a + v * ab; // barycentric coordinates (1-v, v, 0)
	}

	// Check if P in vertex region outside C
	D3DXVECTOR3 cp = p - c;
	float d5 = D3DXVec3Dot(&ab, &cp);
	float d6 = D3DXVec3Dot(&ac, &cp);
	if(d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)

	// Check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5*d2 - d1*d6;
	if(vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		return a + w * ac; // barycentric coordinates (1-w,0,w)
	}

	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3*d6 - d5*d4;
	if(va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + w * (c - b); // barycentric coordinates (0,1-w,w)
	}

	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

bool CAIManager::LinesEqual(const TLine& lhs, TLine& rhs)
{
	if( (lhs.cLineStart == rhs.cLineStart) &&
		(lhs.cLineEnd == rhs.cLineEnd) )
	{
		return true;
	}

	if( (lhs.cLineStart == rhs.cLineEnd) &&
		(lhs.cLineEnd == rhs.cLineStart) )
	{
		return true;
	}

	return false;
}

string& CAIManager::GetAIInfo()
{
	if(m_bShowInfo == true)
	{
		map<unsigned int, CAIComponent*, less<unsigned int>,
			CAllocator<pair<unsigned int, CAIComponent*>>>::iterator pIter =
			m_cAIComponents.begin();

		m_szInfo = "AI:  | ";

		while(pIter != m_cAIComponents.end())
		{
			switch(pIter->second->nAnimation)
			{
			case 0:
				{
					// bulldog
					m_szInfo += " Bulldog: ";
					break;
				}
			case 1:
				{
					// banditos
					m_szInfo += " Banditos: ";
					break;
				}
			case 2:
				{
					// larpers
					m_szInfo += " Larpers: ";
					break;
				}
			case 3:
				{
					// scientist
					m_szInfo += " Scientist: ";
					break;
				}
			case 4:
				{
					// sasha
					m_szInfo += " SASHA: ";
					break;
				}
			}

			switch(pIter->second->m_eCurrentState)
			{
			case 0:
				{
					m_szInfo += " Collective  |";
					break;
				}
			case 1:
				{
					m_szInfo += " Aggressive  |";
					break;
				}
			case 2:
				{
					m_szInfo += " Defensive  |";
					break;
				}
			default:
				{
					m_szInfo += " No State  |";
					break;
				}
			}

			++pIter;
		}
	}
	else
	{
		m_szInfo == "";
	}

	return m_szInfo;
}