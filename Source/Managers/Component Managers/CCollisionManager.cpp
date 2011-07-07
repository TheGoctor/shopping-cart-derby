#include <fstream>

#include "CCollisionManager.h"
#include "CLevelManager.h"
#include "../Global Managers/Event Manager/CEventManager.h"
#include "../Global Managers/Object Manager/CObjectManager.h"
#include "..\..\..\Source\CObject.h"
#include "..\..\Components\Rendering\CRenderComponent.h"
#include "..\Global Managers\Rendering Managers\Renderer.h"
#include "..\Global Managers\Event Manager\EventStructs.h"
#include "..\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\Global Managers\Rendering Managers\Texture Managers\CHUDManager.h"
#include "../Global Managers/Rendering Managers/ModelManager.h"
//for getting speed of objects that move
#include "CMovementManager.h"

using namespace EventStructs;


CCollisionManager::CCollisionManager()
{

}
CCollisionManager::~CCollisionManager()
{
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator pSIter;
	while (!m_cStaticObjects.empty())
	{
		pSIter = m_cStaticObjects.begin();
		MMDEL((*pSIter).second);
		m_cStaticObjects.erase(pSIter);
	}
	m_cStaticObjects.clear();

	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator pNonSIter;
	while (!m_cNonStaticObjects.empty())
	{
		pNonSIter = m_cNonStaticObjects.begin();
		MMDEL((*pNonSIter).second);
		m_cNonStaticObjects.erase(pNonSIter);
	}
	m_cNonStaticObjects.clear();
}

CCollisionManager* CCollisionManager::GetInstance()
{
	static CCollisionManager cCollisionManager;
	return &cCollisionManager;
}

void CCollisionManager::DestroyObject(IEvent* pcEvent, IComponent*)
{
	TObjectEvent* pcObj = (TObjectEvent*)pcEvent->GetData();

	GetInstance()->m_cStaticObjects.erase(GetInstance()->
		m_cStaticObjects.find(pcObj->m_pcObj->GetID()));
}

void CCollisionManager::Init()
{
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName,
		(IComponent*)GetInstance(), Update);
	CEventManager::GetInstance()->RegisterEvent("Shutdown",
		(IComponent*)GetInstance(), Shutdown);
	m_pReflect = MMNEW(D3DXVECTOR3);
	m_pColpt = MMNEW(D3DXVECTOR3);
	GetInstance()->SetTime(0.0f);

	char buffer[64];
	unsigned int num = 0;
	D3DXVECTOR3 *pos_buffer;
	D3DXVECTOR3 *norm_buffer;
	D3DXVECTOR2 *uv_buffer;
	DWORD  *indicies;

	std::fstream fin("Resource\\Collision Volumes\\FFP_3D_Wall_Collision.mesh", ios_base::in | ios_base::binary);

	if(fin.is_open())
	{

		// strlen
		fin.read((char*)&num, 4);
		// name
		fin.read((char*)&buffer, num);
		// loop size
		fin.read((char*)&num, 4);
		VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[num];
		//VERTEX_POSCOLOR *verts = new VERTEX_POSCOLOR[num];
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
			fin.read((char*)&norm_buffer[i].x, sizeof(float));
			fin.read((char*)&norm_buffer[i].y, sizeof(float));
			fin.read((char*)&norm_buffer[i].z, sizeof(float));

		}
		for(unsigned int i = 0; i < num; ++i)
		{
			fin.read((char*)&uv_buffer[i].x, sizeof(float));
			fin.read((char*)&uv_buffer[i].y, sizeof(float));
		}
		for(unsigned int i = 0; i < num; ++i)
		{
			verts[i].position.x = pos_buffer[i].x;
			verts[i].position.y = pos_buffer[i].y;
			verts[i].position.z = pos_buffer[i].z;

			verts[i].normal = norm_buffer[i];
			verts[i].uv.x = uv_buffer[i].x;
			verts[i].uv.y = -uv_buffer[i].y;
		}

		fin.read((char*)&num, 4);
		indicies = new DWORD[num*3];
		m_nTriCount = num;
		this->m_pWallTris = MMNEWARRAYEX(TTriangle, num, HEAPID_GENERAL);
		for(unsigned int i = 0; i < num*3; i+=3)
		{
			fin.read((char*)&indicies[i], 4);
			fin.read((char*)&indicies[i+1], 4);
			fin.read((char*)&indicies[i+2], 4);
		}

		for(unsigned int i = 0; i < num; ++i)
		{
			m_pWallTris[i].tVert0 = verts[indicies[i*3]].position;
			m_pWallTris[i].tVert1 = verts[indicies[i*3+1]].position;
			m_pWallTris[i].tVert2 = verts[indicies[i*3+2]].position;
			D3DXVECTOR3 e0 = m_pWallTris[i].tVert2 - m_pWallTris[i].tVert0;
			D3DXVECTOR3 e1 = m_pWallTris[i].tVert1 - m_pWallTris[i].tVert0;
			D3DXVec3Cross(&m_pWallTris[i].tNorm, &e0, &e1);
			D3DXVec3Normalize(&m_pWallTris[i].tNorm, &m_pWallTris[i].tNorm);

		}

		if(m_pWallTris)
			int x = 0;

		delete [] pos_buffer;
		delete [] norm_buffer;
		delete [] uv_buffer;
		delete [] verts;
		delete [] indicies;

		fin.close();
		fin.clear();

	}
}

CCollideable* CCollisionManager::CreateCollideableComponent(CObject* pParent, 
															bool isStatic, bool isReactor, unsigned int objType)
{
	CCollideable* pCollideComp = MMNEW(CCollideable);
	pCollideComp->SetStatic(isStatic);
	pCollideComp->SetIsReactor(isReactor);
	pCollideComp->SetObjType(objType);

	pCollideComp->SetParent(pParent);
	pParent->AddComponent(pCollideComp);
	if(isStatic)
	{
		GetInstance()->AddStatic(pCollideComp, (unsigned)pParent->GetID());
	}
	else
	{
		GetInstance()->AddNonStatic(pCollideComp, (unsigned)pParent->GetID());
	}
	return pCollideComp;
}

//component functions
int CCollisionManager::CreateCollideableComponent(lua_State* pLua)
{
	int type = (int)lua_tointeger(pLua, -1); // select the last parameter so we know how many to pop off

	switch(type)
	{
	case BSPHERE:
		{
			CObject* pObject = (CObject*)lua_topointer(pLua, -9);
			bool bisStatic = (lua_toboolean(pLua, -8) !=0 );
			bool bisReactor = (lua_toboolean(pLua, -7)!=0);
			int objtype = (int)lua_tointeger(pLua, -6);

			TSphere tsphere;
			CCollideable* pCollideable = CreateCollideableComponent(pObject, bisStatic, bisReactor, objtype);
			pCollideable->SetStatic(bisStatic);
			pCollideable->SetBVType(type);
			pCollideable->SetIsReactor(bisReactor);
			pCollideable->SetObjType(objtype);

			tsphere.cPosition.x = (float)lua_tonumber(pLua, -5);
			tsphere.cPosition.y = (float)lua_tonumber(pLua, -4);
			tsphere.cPosition.z = (float)lua_tonumber(pLua, -3);
			tsphere.fRadius = (float)lua_tonumber(pLua, -2);
			pCollideable->SetSphere(tsphere);

			// Create a Renderable Collision Obj
			string szColObjID = "ColSphere";
			szColObjID += (char)pObject->GetID();
			CObject* pColObj = CObjectManager::GetInstance()->CreateObject(
				szColObjID, 0.0f, 0.0f, 0.0f, 0.0f, pObject);
			// 34 for Spheres
			CRenderComponent* pRender = Renderer::GetInstance()->CreateRenderComp(
				pColObj, 34, 0, RF_INDEXED_VERT);
			pColObj->GetTransform()->ScaleFrame(tsphere.fRadius * 2.0f, 
				tsphere.fRadius * 2.0f, tsphere.fRadius * 2.0f);
			if(pCollideable->GetObjType() != OBJCAMERA)
				CLevelManager::GetInstance()->AddRenderCollision(pRender);

			lua_pop(pLua, 9);
			break;
		}
	case BOBB:
		{
			//d3dxvec3 centerpoint
			//d3dxvec3[3] tU local x y and z axes
			//d3dxvec3 tE positive halfwidth extents along each axis 
			TOBB tBox;
			CObject *pObject = (CObject*)lua_topointer(pLua, -20);
			bool bIsStatic = (lua_toboolean(pLua, -19) != 0);
			bool bIsReactor = (lua_toboolean(pLua, -18) != 0);
			int nObjType = (int)lua_tointeger(pLua, -17);
			CCollideable* pCollideable = CreateCollideableComponent(pObject,
				bIsStatic, bIsReactor, nObjType);
			pCollideable->SetStatic(bIsStatic);
			pCollideable->SetBVType(type);
			pCollideable->SetIsReactor(bIsReactor);
			pCollideable->SetObjType(nObjType);
			tBox.cCenterPoint.x = (float)lua_tonumber(pLua, -16);
			tBox.cCenterPoint.y = (float)lua_tonumber(pLua, -15);
			tBox.cCenterPoint.z = (float)lua_tonumber(pLua, -14);
			tBox.tU[0].x = (float)lua_tonumber(pLua, -13);	//local x
			tBox.tU[0].y = (float)lua_tonumber(pLua, -12);
			tBox.tU[0].z = (float)lua_tonumber(pLua, -11);
			tBox.tU[1].x = (float)lua_tonumber(pLua, -10);//local y
			tBox.tU[1].y = (float)lua_tonumber(pLua, -9);
			tBox.tU[1].z = (float)lua_tonumber(pLua, -8);
			tBox.tU[2].x = (float)lua_tonumber(pLua, -7);//local z
			tBox.tU[2].y = (float)lua_tonumber(pLua, -6);
			tBox.tU[2].z = (float)lua_tonumber(pLua, -5);
			tBox.tE.x = (float)lua_tonumber(pLua, -4);
			tBox.tE.y = (float)lua_tonumber(pLua, -3);
			tBox.tE.z = (float)lua_tonumber(pLua, -2);
			pCollideable->SetOBB(tBox);
			string szColObjID = "ColOBox";
			szColObjID+=(char)pObject->GetID();
			CObject *pColObj = CObjectManager::GetInstance()->CreateObject(
				szColObjID, 0.0f, 0.0f, 0.0f, 0.0f, pObject);

			TAABB testbox;	
			testbox.cBoxMin = -1*D3DXVECTOR3(tBox.tE[0], tBox.tE[1], tBox.tE[2]);
			testbox.cBoxMax = D3DXVECTOR3(tBox.tE[0], tBox.tE[1], tBox.tE[2]);
//			testbox.cBoxMin.z += 0.2f;
//			testbox.cBoxMax.z += 0.2f;
			CRenderComponent* pRender = Renderer::GetInstance()->CreateRenderComp(
				pColObj, ModelManager::GetInstance()->CreateCubeFromAABB(testbox), 0, RF_INDEXED_VERT);
			pColObj->GetTransform()->ScaleFrame(tBox.tE[0] * 2.0f, 
				tBox.tE[1] * 2.0f, tBox.tE[2] * 2.0f);
			CLevelManager::GetInstance()->AddRenderCollision(pRender);
			lua_pop(pLua, 20);
		}
		break;
	case BAABB:
		{
			CObject* pObject = (CObject*)lua_topointer(pLua, -11);
			bool bisStatic = (lua_toboolean(pLua, -10) !=0 );
			bool bisReactor = (lua_toboolean(pLua, -9)!=0);
			int objtype = (int)lua_tointeger(pLua, -8);

			TAABB tAABB;
			TSphere tsphere;
			TPlane tplane;
			CCollideable* pCollideable = CreateCollideableComponent(pObject, bisStatic, bisReactor, objtype);

			pCollideable->SetStatic(bisStatic);
			pCollideable->SetBVType(type);
			pCollideable->SetIsReactor(bisReactor);
			pCollideable->SetObjType(objtype);
			tAABB.cBoxMax.x = (float)lua_tonumber(pLua, -7);
			tAABB.cBoxMax.y = (float)lua_tonumber(pLua, -6);
			tAABB.cBoxMax.z = (float)lua_tonumber(pLua, -5);
			tAABB.cBoxMin.x = (float)lua_tonumber(pLua, -4);
			tAABB.cBoxMin.y = (float)lua_tonumber(pLua, -3);
			tAABB.cBoxMin.z = (float)lua_tonumber(pLua, -2);
			pCollideable->SetAABB(tAABB);
			lua_pop(pLua, 11);
			break;
		}
	case BCAPSULE:
		{
			CObject* pObject = (CObject*)lua_topointer(pLua, -12);
			bool bisStatic = (lua_toboolean(pLua, -11) !=0 );
			bool bisReactor = (lua_toboolean(pLua, -10)!=0);
			int objtype = (int)lua_tointeger(pLua, -9);

			CCollideable* pCol = CreateCollideableComponent(pObject, bisStatic, bisReactor, objtype);

			pCol->SetStatic(bisStatic);
			pCol->SetBVType(type);
			pCol->SetIsReactor(bisReactor);
			pCol->SetObjType(objtype);

			TCapsule tCap;

			tCap.cFront.x = (float)lua_tonumber(pLua, -8);
			tCap.cFront.y = (float)lua_tonumber(pLua, -7);
			tCap.cFront.z = (float)lua_tonumber(pLua, -6);

			tCap.cRear.x = (float)lua_tonumber(pLua, -5);
			tCap.cRear.y = (float)lua_tonumber(pLua, -4);
			tCap.cRear.z = (float)lua_tonumber(pLua, -3);

			tCap.fRadius = (float)lua_tonumber(pLua, -2);
			lua_pop(pLua, 12);

			pCol->SetCapsule(tCap);


			string szColObjID = "ColOBox";
			szColObjID+=(char)pObject->GetID();
			CObject *pColObj = CObjectManager::GetInstance()->CreateObject(
				szColObjID, 0.0f, 0.0f, 0.0f, 0.0f, pObject);
			TAABB testbox;	
			testbox.cBoxMin = tCap.cRear - D3DXVECTOR3(0.5f, 0.0f, 0.0f);
			testbox.cBoxMax = tCap.cFront + D3DXVECTOR3(0.5f, 1.5f, 0.0f);
			CRenderComponent* pRender = Renderer::GetInstance()->CreateRenderComp(
				pColObj, ModelManager::GetInstance()->CreateCubeFromAABB(testbox), 0, RF_INDEXED_VERT);
//			pColObj->GetTransform()->ScaleFrame(tCap.fRadius, tCap.fRadius, tCap.fRadius);
			CLevelManager::GetInstance()->AddRenderCollision(pRender);
			lua_pop(pLua, 20);

			break;
		}
//////////////////////////////////////////////////////////////////////////////////////////////
/*	case BPLANE:
		{
			CObject* pObject = (CObject*)lua_topointer(pLua, -11);
			bool bisStatic = (lua_toboolean(pLua, -10) !=0 );
			bool bisReactor = (lua_toboolean(pLua, -9)!=0);
			int objtype = (int)lua_tointeger(pLua, -8);

			TAABB tAABB;
			TSphere tsphere;
			TPlane tplane;
			CCollideable* pCollideable = CreateCollideableComponent(pObject, bisStatic, bisReactor, objtype);
			pCollideable->SetStatic(bisStatic);
			pCollideable->SetBVType(type);
			pCollideable->SetIsReactor(bisReactor);
			pCollideable->SetObjType(objtype);
			tplane.cPlaneNormal.x = (float)lua_tonumber(pLua, -7);
			tplane.cPlaneNormal.y = (float)lua_tonumber(pLua, -6);
			tplane.cPlaneNormal.z = (float)lua_tonumber(pLua, -5);
			tplane.cPlanePoint.x = (float)lua_tonumber(pLua, -4);
			tplane.cPlanePoint.y = (float)lua_tonumber(pLua, -3);
			tplane.cPlanePoint.z = (float)lua_tonumber(pLua, -2);
			pCollideable->SetPlane(tplane);
			lua_pop(pLua, 11);
			break;
		}*/
//////////////////////////////////////////////////////////////////////////////////////////////
	default:
		break;

	}
	//temp stuff

	return 0;
}
void CCollisionManager::CondenseCollisionBoxes(void)
{
	vector<TAABB> combinedboxes;//once boxes are combined, they are added to this
	vector<TAABB> StaticBoxes;	//uncombined boxes created from CLevelManager.....BuildLevel()
	ColMap::iterator pSiter = m_cStaticObjects.begin();
	while (pSiter != m_cStaticObjects.end())
	{

		if(pSiter->second->GetBVType() == BAABB && pSiter->second->GetObjType() != OBJCHECKOUT)
		{
			TAABB tempBox = pSiter->second->GetAABB();
			StaticBoxes.push_back(tempBox);
			RemoveStatic(pSiter->second, pSiter->second->GetParent()->GetID());
			pSiter = m_cStaticObjects.begin();
		}
		else
		{
			//not my box, not my problem, thats what I say
			pSiter++;
		}
	}
	//Mr Mayor come quick!  we have all the boxes removed!   what shall we do next???
	vector<TAABB>::iterator boxIter1;
	vector<TAABB>::iterator boxIter2;
	for(boxIter1 = StaticBoxes.begin(); boxIter1 != StaticBoxes.end(); boxIter1++)
	{
		TAABB combobox;
		for (boxIter2 = StaticBoxes.begin(); boxIter2 != StaticBoxes.end(); boxIter2++)
		{
			TAABB box1, box2;
			box1 = (*boxIter1);
			box2 = (*boxIter2);
			//either mins have to be equal with the min/max z's being equal to the others opposite z
			if (
				(box1.cBoxMin.x == box2.cBoxMin.x && box1.cBoxMax.x == box2.cBoxMax.x && (box1.cBoxMax.z == box2.cBoxMin.z || box1.cBoxMin.z == box2.cBoxMax.z)) ||
				(box1.cBoxMin.z == box2.cBoxMin.z && box1.cBoxMax.z == box2.cBoxMax.z && (box1.cBoxMax.x == box2.cBoxMin.x || box1.cBoxMin.x == box2.cBoxMax.x))
				)
			{//basically, if they're parallel on the x or z ,and they touch, they're gonna get voltron-ed up in here
				for (unsigned i = 0; i < 3; ++i)
				{//set the min to the min of the two boxes, and the max to obvious of the boxes
					combobox.cBoxMin[i] = box1.cBoxMin[i] <= box2.cBoxMin[i] ? box1.cBoxMin[i] : box2.cBoxMin[i];
					combobox.cBoxMax[i] = box1.cBoxMax[i] >= box2.cBoxMax[i] ? box1.cBoxMax[i] : box2.cBoxMax[i];

				}
				combinedboxes.push_back(combobox);
				StaticBoxes.erase(boxIter2);	
				StaticBoxes.erase(boxIter1);
				boxIter1 = StaticBoxes.begin();
				boxIter2 = StaticBoxes.begin();
			}
		}
	}
	for (unsigned i =0; i < StaticBoxes.size(); ++i)
	{
		combinedboxes.push_back(StaticBoxes[i]);
	}
	for (unsigned i = 0; i < combinedboxes.size(); ++i)
	{
		string szName = "ComboBox";
		szName+=(char)i;
		CObject* pColObj = CObjectManager::GetInstance()->CreateObject(szName);
		CCollideable* pCollideable = CreateCollideableComponent(pColObj, true, true, OBJSHELF);
		pCollideable->SetBVType(BAABB);
		pCollideable->SetAABB(combinedboxes[i]);
		//dont render these, its probably a bad idea
		//		CRenderComponent* pRender = Renderer::GetInstance()->CreateRenderComp(
		//			pColObj, ModelManager::GetInstance()->CreateCubeFromAABB(combinedboxes[i]), 0, RF_INDEXED_VERT);
		//
		//		CLevelManager::GetInstance()->AddRenderCollision(pRender);
	}

}
void CCollisionManager::CreateRenderedComboBoxes()
{
	ColMap::iterator pSiter = m_cStaticObjects.begin();
	while(pSiter != m_cStaticObjects.end())
	{
		if ((*pSiter).second->GetBVType() == BAABB)
		{
			CRenderComponent* pRender = Renderer::GetInstance()->CreateRenderComp(
				(*pSiter).second->GetParent(), ModelManager::GetInstance()->CreateCubeFromAABB((*pSiter).second->GetAABB()), 0, RF_INDEXED_VERT);
			CLevelManager::GetInstance()->AddRenderCollision(pRender);
		}
		++pSiter;
	}

}
void CCollisionManager::Shutdown(IEvent* /* pEvent*/, IComponent* /* pComponen*/)
{
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator<pair<
		unsigned int, CCollideable*>>>::iterator pDelNonStatIter;
	for(pDelNonStatIter = GetInstance()->m_cNonStaticObjects.begin();
		pDelNonStatIter != GetInstance()->m_cNonStaticObjects.end(); ++pDelNonStatIter)
	{
		MMDEL((*pDelNonStatIter).second);
	}
	GetInstance()->m_cNonStaticObjects.clear();

	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator<pair<
		unsigned int, CCollideable*>>>::iterator pDelStatIter;
	for(pDelStatIter = GetInstance()->m_cStaticObjects.begin();
		pDelStatIter != GetInstance()->m_cStaticObjects.end(); ++pDelStatIter)
	{
		MMDEL((*pDelStatIter).second);
	}
	GetInstance()->m_cStaticObjects.clear();

	MMDEL(GetInstance()->m_pReflect);
	MMDEL(GetInstance()->m_pColpt);
}

void CCollisionManager::Update(IEvent* pEvent, IComponent* pComponent)
{
	TUpdateStateEvent* pE = (TUpdateStateEvent*)pEvent->GetData();
	pComponent;
	GetInstance()->SetTime(pE->m_fDeltaTime);
	GetInstance()->SearchForCollision(pE->m_fDeltaTime);
}
void CCollisionManager::AddSepBox(CCollideable* cStatic, unsigned int nObjID)
{
	//adds the static component to the list of statics
	pair<unsigned int, CCollideable*> pTest = 
		pair<unsigned int, CCollideable*>(nObjID, cStatic);
	m_cSeparatedBoxes.insert(pTest); //make_pair(nObjID, cStatic));
}
void CCollisionManager::RemoveSepBox(CCollideable* /*cStatic*/, unsigned int nObjID)
{
	//find it
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator statiter = \
		m_cSeparatedBoxes.find(nObjID);
	//delete it
	m_cSeparatedBoxes.erase(statiter);
}
void CCollisionManager::AddStatic(CCollideable* cStatic, unsigned int nObjID)
{	
	//adds the static component to the list of statics
	pair<unsigned int, CCollideable*> pTest = 
		pair<unsigned int, CCollideable*>(nObjID, cStatic);
	m_cStaticObjects.insert(pTest); //make_pair(nObjID, cStatic));
}

void CCollisionManager::RemoveStatic(CCollideable*, unsigned int nObjID)
{
	//find it
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator statiter = \
		m_cStaticObjects.find(nObjID);
	//delete it
	m_cStaticObjects.erase(statiter);
}

void CCollisionManager::AddNonStatic(CCollideable* cCollide, unsigned int nObjID)
{
	//adds the non static component to the list of non-statics
	m_cNonStaticObjects.insert(make_pair(nObjID, cCollide));
}

void CCollisionManager::RemoveNonStatic(CCollideable*, unsigned int nObjID)
{
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator nonstatiter = \
		m_cNonStaticObjects.find(nObjID);

	//delete it
	m_cNonStaticObjects.erase(nonstatiter);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////collision functions v
//////////////////////////////////////////////////////////////////////////
bool CCollisionManager::SphereToSphereIntersection(CCollideable* obj1, CCollideable* obj2, 
												   D3DXVECTOR3* vReflect, bool bReact)
{
	TSphere s1, s2;
	s1.cPosition = obj1->m_cCenterPoint;//obj1->GetSphere().cPosition;
	s1.fRadius = obj1->m_fExtents[0];//obj1->GetSphere().fRadius;
	s2.cPosition = obj2->m_cCenterPoint;//obj2->GetSphere().cPosition;
	s2.fRadius = obj2->m_fExtents[0];//obj2->GetSphere().fRadius;

	float fDist = 0.0f;
	D3DXVECTOR3 v1to2 = s2.cPosition-s1.cPosition;
	fDist = D3DXVec3Length(&v1to2);
	float fRadii = s1.fRadius+s2.fRadius;
	if(fDist <= fRadii)
	{
		if(!bReact)
		{
			return true;
		}
		float fPen = fRadii - fDist;
		D3DXVECTOR3 v2to1 = s1.cPosition-s2.cPosition;
		D3DXVec3Normalize(&v1to2, &v1to2);
		D3DXVec3Normalize(&v2to1, &v2to1);
		v1to2*=fPen;
		v2to1*=fPen;
		if(obj1->GetObjType() != OBJFSD)
		{
			obj1->GetParent()->GetTransform()->TranslateFrame(v2to1);
		}
		if(obj2->GetObjType() != OBJFSD)
		{
			obj2->GetParent()->GetTransform()->TranslateFrame(v1to2);
		}
		//		D3DXVec3Normalize(&v1to2, &v1to2);
		v1to2 *= -1.0f;
		*vReflect = v1to2;	//for impact event
		D3DXVec3Normalize(vReflect, vReflect);

		return true;
	}
	else
	{
		return false;
	}
}
bool CCollisionManager::SphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2,
												 D3DXVECTOR3* vReflect, bool bReact)
{
	D3DXVECTOR3 vclosest;
	ClosestPointToAABB(obj2->GetAABB(), obj1->m_cCenterPoint, vclosest);

	D3DXVECTOR3 vfromAABB = obj1->GetSphere().cPosition - vclosest;
	float fDist = D3DXVec3Length(&vfromAABB);
	if(fDist <= obj1->GetSphere().fRadius) 
	{
		//egads!  collision!
		if(!bReact)
		{
			return true;
		}
		D3DXVECTOR3 boxNorm = GetAABBNormal(vclosest, obj2->GetAABB());
		obj1->GetParent()->GetTransform()->TranslateFrame( (boxNorm * (obj1->GetSphere().fRadius+0.1f - fDist)) );
		*vReflect = boxNorm;	
		return true;
	}
	else
	{
		return false;
	}
}

bool CCollisionManager::OBBToAABBIntersection(CCollideable* obj1, CCollideable* obj2,
											  D3DXVECTOR3* /*tPosition*/, bool bReact)
{
	float fRA, fRB, fTrans;
	D3DXMATRIX R, AbsR;
	TOBB bA, bB;	//the two boxes, bA is the obb, bB is the AABB made as an OBB
	bA.cCenterPoint = obj1->GetOBB().cCenterPoint;	bA.tE = obj1->GetOBB().tE;
	for(unsigned i = 0; i < 3; ++i)
		bA.tU[i] = obj1->GetOBB().tU[i];
	//turn that axis aligned noise into something a little more oriented
	bB.cCenterPoint = (obj2->GetAABB().cBoxMin + obj2->GetAABB().cBoxMax)/2.0f;
	bB.tU[0] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);	
	bB.tU[1] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	bB.tU[2] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	bB.tE[0] = D3DXVec3Length( &(D3DXVECTOR3(bB.cCenterPoint.x, 0.0f, 0.0f) - D3DXVECTOR3(obj2->GetAABB().cBoxMax.x, 0.0f, 0.0f)) );
	bB.tE[1] = D3DXVec3Length( &(D3DXVECTOR3(0.0f, bB.cCenterPoint.y, 0.0f) - D3DXVECTOR3(0.0f, obj2->GetAABB().cBoxMax.y, 0.0f)) );
	bB.tE[1] = D3DXVec3Length( &(D3DXVECTOR3(0.0f, 0.0f, bB.cCenterPoint.z) - D3DXVECTOR3(0.0f, 0.0f, obj2->GetAABB().cBoxMax.z)) );
	//compute rotation matrix expressing bB in bA's coordinate frame
	for(unsigned i = 0; i < 3; ++i)
		for(unsigned j = 0; j < 3; ++j)
			R.m[i][j] = D3DXVec3Dot(&bA.tU[i], &bB.tU[j]);
	//comput the translation vector vT
	D3DXVECTOR3 vT = bB.cCenterPoint - bA.cCenterPoint;
	//bring the translation into bA's coordinate frame
	vT = D3DXVECTOR3( D3DXVec3Dot(&vT, &bA.tU[0]),
		D3DXVec3Dot(&vT, &bA.tU[1]), D3DXVec3Dot(&vT, &bA.tU[2]) );
	//compute common subexpressions. add in epsilon to counteract
	//	any math erros when two edges are parallel and their cross
	//	product is near NULL
	for(unsigned i = 0; i < 3; ++i)
		for(unsigned j = 0; j < 3; ++j)
			AbsR.m[i][j] = fabs(R.m[i][j]) + FLT_EPSILON;
	//test axes L = A0, L = A1, L = A2
	for(unsigned i = 0; i < 3; ++i)
	{
		fRA = bA.tE[i];
		fRB = bB.tE[0] * AbsR.m[i][0] + bB.tE[1] * AbsR.m[i][1] + bB.tE[2] * AbsR.m[i][2];
		if(fabs(vT[i]) > fRA + fRB)	return false;
	}
	//test axes L = B0, L = B1, L = B2
	for(unsigned i = 0; i < 3; ++i)
	{
		fRA = bA.tE[0] * AbsR.m[0][i] + bA.tE[1] * AbsR.m[1][i] + bA.tE[2] * AbsR.m[2][i];
		fRB = bB.tE[i];
		if(fabs(vT[0] * R.m[0][i] + vT[1] * R.m[1][i] + vT[2] * R.m[2][i]) > fRA + fRB )	return false;
	}
	//test axis L = A0 X B0
	fRA = bA.tE[1] * AbsR.m[2][0] + bA.tE[2] * AbsR.m[1][0];
	fRB = bB.tE[1] * AbsR.m[0][2] + bB.tE[2] * AbsR.m[0][1];
	if(fabs(vT[2] * R.m[1][0] - vT[1] * R.m[2][0]) > fRA + fRB)	return false;
	//test axis L = A0 X B1
	fRA = bA.tE[1] * AbsR.m[2][1] + bA.tE[2] * AbsR.m[1][1];
	fRB = bB.tE[0] * AbsR.m[0][2] + bB.tE[2] * AbsR.m[0][0];
	if(fabs(vT[2]	* R.m[1][1] - vT[1] * R.m[2][1]) > fRA + fRB) return false;
	//test axis L = A0 X B2
	fRA = bA.tE[1] * AbsR.m[2][2] + bA.tE[2] * AbsR.m[1][2];
	fRB = bB.tE[0] * AbsR.m[0][1] + bB.tE[1] * AbsR.m[0][0];
	if(fabs(vT[2]	* R.m[1][2] - vT[1] * R.m[2][2]) > fRA + fRB) return false;
	//test axis L = A1 X B0
	fRA = bA.tE[0] * AbsR.m[2][0] + bA.tE[2] * AbsR.m[0][0];
	fRB = bB.tE[1] * AbsR.m[1][2] + bB.tE[2] * AbsR.m[1][1];
	if(fabs(vT[0]	* R.m[2][0] - vT[2] * R.m[0][0]) > fRA + fRB) return false;
	//test axis L = A1 X B1
	fRA = bA.tE[0] * AbsR.m[2][1] + bA.tE[2] * AbsR.m[0][1];
	fRB = bB.tE[0] * AbsR.m[1][2] + bB.tE[2] * AbsR.m[1][0];
	if(fabs(vT[0]	* R.m[2][1] - vT[2] * R.m[0][1]) > fRA + fRB) return false;
	//test axis L = A1 X B2
	fRA = bA.tE[0] * AbsR.m[2][2] + bA.tE[2] * AbsR.m[0][2];
	fRB = bB.tE[0] * AbsR.m[1][1] + bB.tE[1] * AbsR.m[1][0];
	if(fabs(vT[0]	* R.m[2][2] - vT[2] * R.m[0][2]) > fRA + fRB) return false;
	//test axis L = A2 X B0
	fRA = bA.tE[0] * AbsR.m[1][0] + bA.tE[1] * AbsR.m[0][0];
	fRB = bB.tE[1] * AbsR.m[2][2] + bB.tE[2] * AbsR.m[2][1];
	if(fabs(vT[1]	* R.m[0][0] - vT[0] * R.m[1][0]) > fRA + fRB) return false;
	//test axis L = A2 X B1
	fRA = bA.tE[0] * AbsR.m[1][1] + bA.tE[1] * AbsR.m[0][1];
	fRB = bB.tE[0] * AbsR.m[2][2] + bB.tE[2] * AbsR.m[2][0];
	if(fabs(vT[1]	* R.m[0][1] - vT[0] * R.m[1][1]) > fRA + fRB) return false;
	//test axis L = A2 X B2
	fRA = bA.tE[0] * AbsR.m[1][2] + bA.tE[1] * AbsR.m[0][2];
	fRB = bB.tE[0] * AbsR.m[2][1] + bB.tE[1] * AbsR.m[2][0];
	if(fabs(vT[1]	* R.m[0][2] - vT[0] * R.m[1][2]) > fRA + fRB) return false;
	fTrans = (fRA+fRB) - D3DXVec3Dot(&vT, &vT); //maybe

	if(!bReact)
		return true;
	D3DXVECTOR3 v1to2, v2to1;
	v1to2 = obj2->GetOBB().cCenterPoint - obj1->GetOBB().cCenterPoint;
	v2to1 = obj1->GetOBB().cCenterPoint - obj2->GetOBB().cCenterPoint;
	D3DXVec3Normalize(&v1to2, &v1to2); D3DXVec3Normalize(&v2to1, &v2to1);
	obj1->GetParent()->GetTransform()->TranslateFrame(v1to2 * fTrans/4.0f);
	obj2->GetParent()->GetTransform()->TranslateFrame(v2to1 * fTrans/4.0f);

	//no separating axis found, the OBBs must be intersecting
	return true;
}
void CCollisionManager::ClosestPointToOBB(TOBB tBox, 
										  D3DXVECTOR3 tTestPt, D3DXVECTOR3& tClosest)
{
	D3DXVECTOR3 vVec = tTestPt - tBox.cCenterPoint;
	tClosest = tBox.cCenterPoint;
	//for each OBB axis
	for(unsigned iaxis = 0; iaxis < 3; ++iaxis)
	{
		//project vec onto the axis to get dist
		float fDist = D3DXVec3Dot(&vVec, &tBox.tU[iaxis]);
		//if distance is farther than box extents, clamp it to the box bounds
		if (fDist > tBox.tE[iaxis]) fDist = tBox.tE[iaxis];
		if(fDist < -tBox.tE[iaxis]) fDist = -tBox.tE[iaxis];
		//step that distance along the axis to get world coord
		tClosest+= fDist*tBox.tU[iaxis];
	}
}


bool CCollisionManager::AABBTOAABBIntersection(CCollideable* obj1, CCollideable* obj2)
{
	TAABB BV1, BV2;
	BV1.cBoxMax = obj1->GetAABB().cBoxMax;
	BV1.cBoxMin = obj1->GetAABB().cBoxMin;
	BV2.cBoxMin = obj2->GetAABB().cBoxMin;
	BV2.cBoxMax = obj2->GetAABB().cBoxMax;
	return (BV1.cBoxMin.x > BV2.cBoxMax.x || BV1.cBoxMax.x < BV2.cBoxMin.x ||
		BV1.cBoxMin.y > BV2.cBoxMax.y || BV1.cBoxMax.y < BV2.cBoxMin.y ||
		BV1.cBoxMin.z > BV2.cBoxMax.z || BV1.cBoxMax.z < BV2.cBoxMin.z);
}
//////////////////////////////////////////////////////////////////////////
//Frustum collision functions
// these functions are used for frustum culling
//////////////////////////////////////////////////////////////////////////

void CCollisionManager::CheckFrustDist(float fDist, CCollideable* pCheck,
									   bool &collide, bool &behind, bool &intersect)
{

	if(abs(fDist) <= pCheck->GetSphere().fRadius)
	{
		collide = true;
	}
	else if(fDist < -pCheck->GetSphere().fRadius)
	{
		behind = true;
	}
	else if(fDist <= pCheck->GetSphere().fRadius)
	{
		intersect = true;
	}
}
bool CCollisionManager::SphereToOBBIntersection(CCollideable* obj1, CCollideable* obj2,
												D3DXVECTOR3* /*tPosition*/, bool bReact)
{
	D3DXVECTOR3 tClosest;
	ClosestPointToOBB(obj2->GetOBB(), obj1->GetSphere().cPosition, tClosest);
	//they intersect if the squared distance from sphere center to tClosest is less than
	//			the squared sphere radius
	D3DXVECTOR3 tV = tClosest - obj1->GetSphere().cPosition;
	bool collision = (D3DXVec3Dot(&tV, &tV) <= obj1->GetSphere().fRadius*obj1->GetSphere().fRadius);
	if(!bReact)
		return collision;
	if(collision)
	{
		D3DXVECTOR3 vreflect, vclosest;
		ClosestPointToOBB(obj2->GetOBB(), obj1->GetSphere().cPosition, vclosest);
		vreflect = vclosest - obj1->GetSphere().cPosition;
		D3DXVec3Normalize(&tV, &tV);
		obj2->GetParent()->GetTransform()->TranslateFrame(vreflect*0.25f);//tV * D3DXVec3Length(&vreflect));
	}
	return collision;	
}
bool CCollisionManager::OBBToOBBIntersection(CCollideable* obj1, CCollideable* obj2,
											 D3DXVECTOR3* /*tPosition*/, bool bReact)
{
	//check if they're stuck together
	D3DXVECTOR3 vecto1, vecto2;
	vecto1 = obj1->m_cCenterPoint - obj2->m_cCenterPoint;
	vecto2 = obj2->m_cCenterPoint - obj1->m_cCenterPoint;
	if(D3DXVec3Length(&vecto1) < 1.0f)
	{
		obj1->GetParent()->GetTransform()->TranslateFrame(vecto1);
		obj2->GetParent()->GetTransform()->TranslateFrame(vecto2);		
	}
	/////////////////////////////---end of check stuck
	float fRA, fRB, fTrans;
	D3DXMATRIX R, AbsR;
	TOBB bA, bB;	//the two boxes
	bA.cCenterPoint = obj1->GetOBB().cCenterPoint;	bA.tE = obj1->GetOBB().tE;
	bB.cCenterPoint = obj2->GetOBB().cCenterPoint;	bB.tE = obj2->GetOBB().tE;
	for(unsigned i = 0; i < 3; ++i)
	{
		bA.tU[i] = obj1->GetOBB().tU[i];
		bB.tU[i] = obj2->GetOBB().tU[i];
	}
	//compute rotation matrix expressing bB in bA's coordinate frame
	for(unsigned i = 0; i < 3; ++i)
		for(unsigned j = 0; j < 3; ++j)
			R.m[i][j] = D3DXVec3Dot(&bA.tU[i], &bB.tU[j]);
	//comput the translation vector vT
	D3DXVECTOR3 vT = bB.cCenterPoint - bA.cCenterPoint;
	//bring the translation into bA's coordinate frame
	vT = D3DXVECTOR3( D3DXVec3Dot(&vT, &bA.tU[0]),
		D3DXVec3Dot(&vT, &bA.tU[1]), D3DXVec3Dot(&vT, &bA.tU[2]) );
	//compute common subexpressions. add in epsilon to counteract
	//	any math erros when two edges are parallel and their cross
	//	product is near NULL
	for(unsigned i = 0; i < 3; ++i)
		for(unsigned j = 0; j < 3; ++j)
			AbsR.m[i][j] = fabs(R.m[i][j]) + FLT_EPSILON;
	//test axes L = A0, L = A1, L = A2
	for(unsigned i = 0; i < 3; ++i)
	{
		fRA = bA.tE[i];
		fRB = bB.tE[0] * AbsR.m[i][0] + bB.tE[1] * AbsR.m[i][1] + bB.tE[2] * AbsR.m[i][2];
		if(fabs(vT[i]) > fRA + fRB)	return false;
	}
	//test axes L = B0, L = B1, L = B2
	for(unsigned i = 0; i < 3; ++i)
	{
		fRA = bA.tE[0] * AbsR.m[0][i] + bA.tE[1] * AbsR.m[1][i] + bA.tE[2] * AbsR.m[2][i];
		fRB = bB.tE[i];
		if(fabs(vT[0] * R.m[0][i] + vT[1] * R.m[1][i] + vT[2] * R.m[2][i]) > fRA + fRB )	return false;
	}
	//test axis L = A0 X B0
	fRA = bA.tE[1] * AbsR.m[2][0] + bA.tE[2] * AbsR.m[1][0];
	fRB = bB.tE[1] * AbsR.m[0][2] + bB.tE[2] * AbsR.m[0][1];
	if(fabs(vT[2] * R.m[1][0] - vT[1] * R.m[2][0]) > fRA + fRB)	return false;
	//test axis L = A0 X B1
	fRA = bA.tE[1] * AbsR.m[2][1] + bA.tE[2] * AbsR.m[1][1];
	fRB = bB.tE[0] * AbsR.m[0][2] + bB.tE[2] * AbsR.m[0][0];
	if(fabs(vT[2]	* R.m[1][1] - vT[1] * R.m[2][1]) > fRA + fRB) return false;
	//test axis L = A0 X B2
	fRA = bA.tE[1] * AbsR.m[2][2] + bA.tE[2] * AbsR.m[1][2];
	fRB = bB.tE[0] * AbsR.m[0][1] + bB.tE[1] * AbsR.m[0][0];
	if(fabs(vT[2]	* R.m[1][2] - vT[1] * R.m[2][2]) > fRA + fRB) return false;
	//test axis L = A1 X B0
	fRA = bA.tE[0] * AbsR.m[2][0] + bA.tE[2] * AbsR.m[0][0];
	fRB = bB.tE[1] * AbsR.m[1][2] + bB.tE[2] * AbsR.m[1][1];
	if(fabs(vT[0]	* R.m[2][0] - vT[2] * R.m[0][0]) > fRA + fRB) return false;
	//test axis L = A1 X B1
	fRA = bA.tE[0] * AbsR.m[2][1] + bA.tE[2] * AbsR.m[0][1];
	fRB = bB.tE[0] * AbsR.m[1][2] + bB.tE[2] * AbsR.m[1][0];
	if(fabs(vT[0]	* R.m[2][1] - vT[2] * R.m[0][1]) > fRA + fRB) return false;
	//test axis L = A1 X B2
	fRA = bA.tE[0] * AbsR.m[2][2] + bA.tE[2] * AbsR.m[0][2];
	fRB = bB.tE[0] * AbsR.m[1][1] + bB.tE[1] * AbsR.m[1][0];
	if(fabs(vT[0]	* R.m[2][2] - vT[2] * R.m[0][2]) > fRA + fRB) return false;
	//test axis L = A2 X B0
	fRA = bA.tE[0] * AbsR.m[1][0] + bA.tE[1] * AbsR.m[0][0];
	fRB = bB.tE[1] * AbsR.m[2][2] + bB.tE[2] * AbsR.m[2][1];
	if(fabs(vT[1]	* R.m[0][0] - vT[0] * R.m[1][0]) > fRA + fRB) return false;
	//test axis L = A2 X B1
	fRA = bA.tE[0] * AbsR.m[1][1] + bA.tE[1] * AbsR.m[0][1];
	fRB = bB.tE[0] * AbsR.m[2][2] + bB.tE[2] * AbsR.m[2][0];
	if(fabs(vT[1]	* R.m[0][1] - vT[0] * R.m[1][1]) > fRA + fRB) return false;
	//test axis L = A2 X B2
	fRA = bA.tE[0] * AbsR.m[1][2] + bA.tE[1] * AbsR.m[0][2];
	fRB = bB.tE[0] * AbsR.m[2][1] + bB.tE[1] * AbsR.m[2][0];
	if(fabs(vT[1]	* R.m[0][2] - vT[0] * R.m[1][2]) > fRA + fRB) return false;
	fTrans = (fRA+fRB) - D3DXVec3Dot(&vT, &vT); //maybe

	if(!bReact)
		return true;
	D3DXVECTOR3 v1to2, v2to1;
	v1to2 = obj2->GetOBB().cCenterPoint - obj1->GetOBB().cCenterPoint;
	v2to1 = obj1->GetOBB().cCenterPoint - obj2->GetOBB().cCenterPoint;
	D3DXVec3Normalize(&v1to2, &v1to2); D3DXVec3Normalize(&v2to1, &v2to1);
	obj1->GetParent()->GetTransform()->TranslateFrame(v1to2 * fTrans/4.0f);
	obj2->GetParent()->GetTransform()->TranslateFrame(v2to1 * fTrans/4.0f);

	//no separating axis found, the OBBs must be intersecting
	return true;
}

//////////////////////////////////////////////////////////////////////////
//Collision Line Functions
// these functions are used mostly by AI entities for pathfinding
//////////////////////////////////////////////////////////////////////////
bool CCollisionManager::LineToSphereIntersection(TLine tLine, TSphere tSphere, D3DXVECTOR3& tColPoint)
{
	D3DXVECTOR3 vL = tLine.cLineEnd - tLine.cLineStart;
	D3DXVECTOR3 vLN;
	D3DXVec3Normalize(&vLN, &vL);
	D3DXVECTOR3 vV = tSphere.cPosition - tLine.cLineStart;
	float fD = D3DXVec3Dot(&vLN, &vV);
	D3DXVECTOR3 vNScaled = vLN * fD;

	D3DXVECTOR3 closestpt = tLine.cLineStart + vNScaled;	//closest point

	D3DXVECTOR3 closesttocenter = tSphere.cPosition = closestpt;
	float fDist = D3DXVec3Length(&closesttocenter);
	if(fDist > tSphere.fRadius)
	{
		tColPoint = closesttocenter;
		return true;
	}
	else
	{
		return false;
	}
}
bool CCollisionManager::LineToTriangle(TLine tLine, TTriangle triangle, D3DXVECTOR3 & tColpoint)
{
	bool hasCollided = false;
	tColpoint = tLine.cLineEnd;
	//d0 = dotprod(N, vstart) ||  d1 = dotprod(N, triangvert[0])  || d2 = d0-d1
	float D0, D1, D2, D3, DF;
	D3DXVECTOR3 DL, ColPt;
	D0 = D3DXVec3Dot(&triangle.tNorm, &tLine.cLineStart);
	D1 = D3DXVec3Dot(&triangle.tNorm, &triangle.tVert0);
	D2 = D0 - D1;
	//early out 1: facing away
	if(D2 < 0.0f)
		return hasCollided;
	//also, doesn't cross the plane
	if(D2 < 0.0f && D3DXVec3Dot(&tColpoint, &triangle.tNorm) < 0.0f)
		return hasCollided;
	if(D2 > 0.0f && D3DXVec3Dot(&tColpoint, &triangle.tNorm) > 0.0f)
		return hasCollided;
	DL = (tLine.cLineStart - tColpoint);
	D3 = D3DXVec3Dot(&triangle.tNorm, &DL);	//dot prod(N,L) L = line vector
	if(D3 == 0)
	{
		return hasCollided;
	}
	DF = -(D2/D3);	//TESTNOTE: -(D2/D3)
	tColpoint = DL*DF;
	tColpoint = tColpoint + tLine.cLineStart;
	//check start
	D3DXVECTOR3 edge0, edge1, edge2;
	D3DXVECTOR3 Norm0, Norm1, Norm2;
	D3DXVECTOR3 VectoPoint;
	float fDtoPoint;

	//half space tests
	//edge 0
	edge0 = triangle.tVert1 - triangle.tVert0;
	D3DXVec3Cross(&Norm0, &edge0, &triangle.tNorm);
	//	D3DXVec3Normalize(&Norm0, &Norm0);
	VectoPoint = tColpoint - triangle.tVert0;
	fDtoPoint = D3DXVec3Dot(&Norm0, &VectoPoint);
	if(fDtoPoint > 0.0f)
		return hasCollided;//do somethign to mimic continue instead of hard return
	//edge 1
	edge1 = triangle.tVert2 - triangle.tVert1;
	D3DXVec3Cross(&Norm1, &edge1, &triangle.tNorm);
	//	D3DXVec3Normalize(&Norm1, &Norm1);
	VectoPoint = tColpoint - triangle.tVert1;
	fDtoPoint = D3DXVec3Dot(&Norm1, &VectoPoint);
	if(fDtoPoint > 0.0f)
		return hasCollided;
	//edge 2
	edge2 = triangle.tVert0 - triangle.tVert2;
	D3DXVec3Cross(&Norm2, &edge2, &triangle.tNorm);
	//	D3DXVec3Normalize(&Norm2, &Norm2);
	VectoPoint = tColpoint - triangle.tVert2;
	fDtoPoint = D3DXVec3Dot(&Norm2, &VectoPoint);
	if(fDtoPoint > 0.0f)
		return hasCollided;
	hasCollided = true;

	return hasCollided;

}
bool CCollisionManager::LineToAABBIntersection(TLine tLine, TAABB tAABB, D3DXVECTOR3& tColPoint)
{
	//R(t) = P +td, t>=0
	//p = ray start
	//d = ray direction
	//t = some time on the ray
	D3DXVECTOR3 tColPt, tPrevColPt;
	tColPt = tPrevColPt = tLine.cLineEnd;
	TRay tRay;
	tRay.cRayStart = tLine.cLineStart;
	tRay.cRayNormal = tLine.cLineEnd - tLine.cLineStart;
	// 	D3DXVec3Normalize(&tRay.cRayNormal, &tRay.cRayNormal);
	vector<TTriangle> BBTris = GetAABBTriangles(tAABB);
	unsigned int intersects = 0;	//num intersects
	for(unsigned itris = 0; itris < BBTris.size(); ++itris)
	{
		if(LineToTriangle( tLine, BBTris[itris], tColPt))
		{
			D3DXVECTOR3 linetocolpt = tColPt - tLine.cLineStart;
			D3DXVECTOR3 linetolineend = tLine.cLineEnd - tLine.cLineStart;
			float lentocolpt, lentolineend; lentolineend = lentocolpt = 0.0f;
			lentocolpt = D3DXVec3Dot(&linetocolpt, &linetocolpt);
			lentolineend = D3DXVec3Dot(&linetolineend, &linetolineend);
			if(lentocolpt <= lentolineend)
			{
				tColPoint = GetCloserPt(tColPt, tPrevColPt, tLine.cLineStart);
				tPrevColPt = tColPt;
				++intersects;
			}
		}
	}
	BBTris.clear();
	if (intersects >= 1)
	{
		return true;
	}
	return false;
}
bool CCollisionManager::LineToWayPoint(TLine tLine, D3DXVECTOR3 &tClosestPt)
{
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator pSIter;	
	D3DXVECTOR3 tClosestPtCollision = tLine.cLineEnd;	//closest point of collision
	D3DXVECTOR3 tClosestTemp = tLine.cLineEnd;			//temp closest for function returns
	bool bCollision = false;											//collision function returns here
	bool bWasInterrupted = false;									//set to true if the line ever intersected
	D3DXVECTOR3 tEarlyOutpt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 tEarlyOutVec;	//vector to the closest point on the shape
	D3DXVECTOR3 tlinevec = tLine.cLineStart - tLine.cLineEnd;
	float sRadius = 0.0f;
	for(pSIter = m_cStaticObjects.begin(); pSIter != m_cStaticObjects.end(); ++pSIter)
	{
		switch( (*pSIter).second->GetBVType() )
		{
		case BSPHERE:
			{
				tEarlyOutVec = (*pSIter).second->GetSphere().cPosition - tLine.cLineStart;
				sRadius = (*pSIter).second->GetSphere().fRadius;// * (*pSIter).second->GetSphere().fRadius;
				if(D3DXVec3Length(&tEarlyOutVec)+sRadius < D3DXVec3Length(&tlinevec))
				{
					bCollision = LineToSphereIntersection(tLine, (*pSIter).second->GetSphere(), tClosestTemp);
				}
			}
			break;
		case BAABB:
			{

				ClosestPointToAABB((*pSIter).second->GetAABB(), tLine.cLineStart, tEarlyOutpt);
				tEarlyOutVec = tEarlyOutpt - tLine.cLineStart;
				if(D3DXVec3Dot(&tEarlyOutVec, &tEarlyOutVec) < D3DXVec3Dot(&tlinevec, &tlinevec))
				{
					bCollision = LineToAABBIntersection(tLine, (*pSIter).second->GetAABB(), tClosestTemp);
				}
			}
			break;
		default:
			{
				bCollision = false;
			}
			break;
		}
		if(bCollision)
		{
			D3DXVECTOR3 linetoColpt = tClosestTemp - tLine.cLineStart;
			D3DXVECTOR3 linevector = tLine.cLineEnd - tLine.cLineStart;
			float fLenToColpt = D3DXVec3Dot(&linetoColpt, &linetoColpt);
			float fLenToLineEnd = D3DXVec3Dot(&linevector, &linevector);
			if(fLenToColpt > fLenToLineEnd)
			{//if the distance to the colpt is greater than the length of the line
				bCollision = false;
				continue;		//should go to next iteration of loop
			}
			else
			{
				bWasInterrupted = true;
				tClosestPtCollision = GetCloserPt(tClosestPtCollision, tClosestTemp, tLine.cLineStart);
				tClosestPt = tClosestPtCollision;		//make sure it's set
			}
		}
	}
	return bWasInterrupted;
}

bool CCollisionManager::PointInTriangle(TTriangle tTri, D3DXVECTOR3 tTestPt)
{
	D3DXVECTOR3 Edge0, Edge1, Edge2, Norm0, Norm1, Norm2;
	Edge0 = tTri.tVert1 - tTri.tVert0;
	D3DXVec3Cross(&Norm0, &Edge0, &tTri.tNorm);
	//half space test with v0, Norm0, tTestPt
	if(HalfSpaceTest(Norm0, tTri.tVert0, tTestPt) > 0)
		return false;
	Edge1 = tTri.tVert2 - tTri.tVert1;
	D3DXVec3Cross(&Norm1, &Edge1, &tTri.tNorm);
	//half space test with 
	if(HalfSpaceTest(Norm1, tTri.tVert1, tTestPt) > 0)
		return false;
	Edge2 = tTri.tVert0 - tTri.tVert2;
	D3DXVec3Cross(&Norm2, &Edge2, &tTri.tNorm);
	//half space test
	if (HalfSpaceTest(Norm2, tTri.tVert2, tTestPt) > 0)
		return false;
	return true;
}
int CCollisionManager::HalfSpaceTest(D3DXVECTOR3 tNorm, 
									 D3DXVECTOR3 tPlanePt, D3DXVECTOR3 tTest)
{
	D3DXVECTOR3 tV = tTest - tPlanePt;
	float fD = D3DXVec3Dot(&tNorm, &tV);
	if (fD > 0)
	{
		return 1;	//on positive side of the plane
	}
	else if(fD < 0.0f)
	{
		return -1; //on negative side of the plane
	}
	else //if(fD == 0.0f)
	{
		return 0;	//on the plane
	}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

float Clamp(float n, float min, float max)
{
	if(n < min) return min;
	if(n > max) return max;
	return n;
}

float ClosestPtSegmentSegment(D3DXVECTOR3 p1, D3DXVECTOR3 q1, D3DXVECTOR3 p2, D3DXVECTOR3 q2,
							  float &s, float &t, D3DXVECTOR3 &c1, D3DXVECTOR3 &c2)
{
	D3DXVECTOR3 d1 = q1 - p1;	// Direction vector of segment 1
	D3DXVECTOR3 d2 = q2 - p2;	// Direction vector of segment 2
	D3DXVECTOR3 r = p1 - p2;
	float a = D3DXVec3Dot(&d1, &d1);	// Squared length of segment S1, always nonnegative
	float e = D3DXVec3Dot(&d2, &d2);	// Squared length of segment S2, always nonnegative
	float f = D3DXVec3Dot(&d2, &r);

	// Check if either or both segments degenerate into points
	if ( a <= FLT_EPSILON && e <= FLT_EPSILON)
	{
		// Both segments degenreate into points
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;
		return D3DXVec3Dot(&(c1 - c2), &(c1 - c2));
	}
	if(a <= FLT_EPSILON)
	{
		// First segment degenerates into a point
		s = 0.0f;
		t = f / e;	// s = 0 => t = (b*s + f) / e = f / e
		t = Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = D3DXVec3Dot(&d1, &r);
		if(e <= FLT_EPSILON)
		{
			// Second segment degenerates into a point
			t = 0.0f;
			s = Clamp(-c / a, 0.0f, 1.0f);	// t = 0 => s = (b*t - c) / a = -c / a
		}
		else
		{
			// The general nondegenerate case starts here
			float b = D3DXVec3Dot(&d1, &d2);
			float denom = a*e-b*b;	// Always nonnegative

			// If segments not parallel, compute closest point on L1 to L2 and
			// clamp to segment S1. Else pick arbitrary s (here 0)
			if(denom != 0.0f)
			{
				s = Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
			}
			else
			{
				s = 0.0f;
			}
			// Compute point on L2 closest to S1(s) using
			// t = Dot((P1 + D1*s) - P2,D2) / Dot(D2, D2) = (b*s + f) / e
			t = (b*s + f) / e;

			// If t in [0,1] dont. Else clamp t, recompute s for the new value
			// of t using s = Dot((P2 + D2*t) - P1, D1) / Dot(D1, D1) = (t*b - c) / a
			// and clamp s to [0, 1]
			if(t < 0.0f)
			{
				t = 0.0f;
				s = Clamp(-c / a, 0.0f, 1.0f);
			}
			else
			{
				t = 1.0f;
				s = Clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;
	return D3DXVec3Dot(&(c1 - c2), &(c1 - c2));
}


// Returns 2 times the signed triangle area. The result is positive if
// abc is ccw, negative if abc is cw, zero if abc is degenerate
float Signed2DTriArea(D3DXVECTOR2 a, D3DXVECTOR2 b, D3DXVECTOR2 c)
{
	return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}

// Test if segments ab and cd overlap. If they do, compute and return
// intersection t value along ab and intersection position p
int Test2DSegmentSegment(D3DXVECTOR2 a, D3DXVECTOR2 b, D3DXVECTOR2 c, D3DXVECTOR2 d, float &t, D3DXVECTOR2 &p)
{
	// Sign of areas correspond to which side of ab points c and d are
	float a1 = Signed2DTriArea(a, b, d);	// Compute winding of abd (+ or -)
	float a2 = Signed2DTriArea(a, b, c);	// To intersect, must have sign opposite of a1

	// If c and d are on different sides of ab, areas have different signs 
	if(a1 * a2 < 0.0f)
	{
		// Compute signs for a and b with respect to segment cd
		float a3 = Signed2DTriArea(c, d, a);	// Compute winding of cda (+ or -)
		// Since area is constant a1 - a2 = a3 - a4, or a4 = a3 + a2 - a1
		// float a4 = Signed2DTriArea(c, d, b); // Must have opposite sign of a3
		float a4 = a3 + a2 - a1;
		// Points a and b on different sides of cd if areas have different signs
		if(a3 * a4 < 0.0f) 
		{
			// Segments intersect. Find intersection point along L(t) = a + t * (b - a).
			// Given height h1 of an over cd and height h2 of b over cd,
			// t = h1 / (h1 - h2) = (b*h1/2) / (b*h1/2 - bh2/2) = a3 / (a3 - a4),
			// where b (the base of the triangles cda and cdb, i.e., the length
			// of cd) cancels out.
			t = a3 / (a3 - a4);
			p = a + t * (b - a);
			return 1;
		}
	}
	// Segments not intersecting (or collinear)
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 CCollisionManager::ClosestPointOnLine(TLine line, D3DXVECTOR3 testpt)
{
	D3DXVECTOR3 closest;
	D3DXVECTOR3 linevec, tV;
	float fDot = 0.0f;
	linevec = line.cLineEnd - line.cLineStart;
	D3DXVec3Normalize(&linevec, &linevec);
	tV = testpt - line.cLineStart;
	fDot = D3DXVec3Dot(&linevec, &tV);
	linevec *= fDot;
	closest = line.cLineStart + linevec;
	return closest;
}
bool CCollisionManager::RayToLineIntersection(TRay tRay, TLine tSegment)
{
	float t = 0.0f;
	D3DXVECTOR2 p;

	D3DXVECTOR3 temp = tRay.cRayStart + (tRay.cRayNormal * 10.0f);

	return Test2DSegmentSegment(
		D3DXVECTOR2(tSegment.cLineStart.x, tSegment.cLineStart.z),
		D3DXVECTOR2(tSegment.cLineEnd.x, tSegment.cLineEnd.z),
		D3DXVECTOR2(tRay.cRayStart.x, tRay.cRayStart.z),
		D3DXVECTOR2(temp.x, temp.z),	t, p) == 1 ? true : false;


	/*
	// Our not-so-great collision test
	float fM1, fM2, fB1, fB2;//__1 = segment, __2 = ray
	fM2 = fM1 = fB2 = fB1 = 0.0f;
	D3DXVECTOR2 tRayStart, tRayDir, tLineStart, tLineEnd, tRayVec, tLineVec, colpt;
	tRayStart.x = tRay.cRayStart.x; 
	tRayStart.y = tRay.cRayStart.z;
	tRayDir.x = tRay.cRayNormal.x;
	tRayDir.y = tRay.cRayNormal.z;
	tLineStart.x = tSegment.cLineStart.x - tRayStart.x; 
	tLineStart.y = tSegment.cLineStart.z - tRayStart.y;
	tLineEnd.x = tSegment.cLineEnd.x - tRayStart.x;
	tLineEnd.y = tSegment.cLineEnd.z - tRayStart.y;
	tLineVec = tLineEnd - tLineStart;
	fM2 = tRayDir.y/tRayDir.x;

	fB2 = 0.0f;
	float fX = 0.0f, fY = 0.0f;
	if (tLineVec.x == 0.0f)
	{
	colpt.x = tLineStart.x;
	colpt.y = fM2*tLineStart.x;  
	return ((colpt.y >= tLineStart.y) == (colpt.y <= tLineEnd.y)) 
	&& (D3DXVec2Dot(&colpt, &tRayDir) > 0.0f);
	}
	else
	{
	fM1 = tLineVec.y/tLineVec.x;
	fB1 = tLineStart.y - tLineStart.x*fM1;
	fX = (fB1 - fB2)/(fM2-fM1);
	fY = fM1 * fX + fB1;
	if(FloatEquals(fM1, fM2, FLT_EPSILON)) return false; //parallellofalse
	if((fX >= tLineStart.x) == (fX <= tLineEnd.x))
	{
	colpt = D3DXVECTOR2(fX, fY);
	D3DXVECTOR2 tempvec = colpt;
	return (D3DXVec2Dot(&tRayDir, &tempvec) > 0);
	}
	}
	*/

}
bool CCollisionManager::LineTestAvoidance(TLine /*tLine*/, float /*fRayDist*/, D3DXVECTOR3 &/*tSurfaceNorm*/)
{
	//NOTE: the mac and mac attack are back again wanting some shit done in here - raymoney
	return false;
}
/////////////////////////////////////////////s/////////////////////////////
//Collision Support Functions
// these functions serve to assist the collision check functions
// they will be only be called from within the collision manager
//////////////////////////////////////////////////////////////////////////
//bool CCollisionManager::PointInCone(D3DXVECTOR3 tPlayerMove, D3DXVECTOR3 tPlayerPos, 
//									D3DXVECTOR3 tTargetPos, float fCone)
//{
//	return false;
//}
void CCollisionManager::ClosestPointToAABB(TAABB tBox, D3DXVECTOR3 tTestPt, D3DXVECTOR3& tClosest)
{
	for (unsigned iaxis = 0; iaxis < 3; ++iaxis)
	{
		float fVal = tTestPt[iaxis];
		if(fVal < tBox.cBoxMin[iaxis]) fVal = tBox.cBoxMin[iaxis];	// val = max(val, min[i]
		if(fVal > tBox.cBoxMax[iaxis]) fVal = tBox.cBoxMax[iaxis];// val = min(val, max[i]
		tClosest[iaxis] = fVal;
	}
}
D3DXVECTOR3 CCollisionManager::GetAABBNormal(D3DXVECTOR3 colPt, TAABB colBox)
{
	D3DXVECTOR3 vNorm;
	D3DXVECTOR3 vCenter = (colBox.cBoxMax + colBox.cBoxMin)/2.0f;
	D3DXVECTOR3 vRelativeSide = D3DXVECTOR3(0.0f, vCenter.y, 0.0f);

	if(FloatEquals(colPt.x, colBox.cBoxMax.x, 0.1f))
	{
		vRelativeSide.x = colBox.cBoxMax.x;
		vRelativeSide.z = vCenter.z;
	}
	else if (FloatEquals(colPt.x, colBox.cBoxMin.x, 0.1f))
	{
		vRelativeSide.x = colBox.cBoxMin.x;
		vRelativeSide.z = vCenter.z;
	}

	if(FloatEquals(colPt.z, colBox.cBoxMax.z, 0.1f))
	{
		vRelativeSide.z = colBox.cBoxMax.z;
		vRelativeSide.x = vCenter.x;
	}
	else if (FloatEquals(colPt.z, colBox.cBoxMin.z, 0.1f))
	{
		vRelativeSide.z = colBox.cBoxMin.z;
		vRelativeSide.x = vCenter.x;
	}
	vNorm = vRelativeSide - vCenter;
	D3DXVec3Normalize(&vNorm, &vNorm);

	return vNorm;
}
D3DXVECTOR3 CCollisionManager::GetSphereNormal(D3DXVECTOR3 colPt, TSphere colSphere)
{
	D3DXVECTOR3 vNorm;
	vNorm = colPt - colSphere.cPosition;
	D3DXVec3Normalize(&vNorm, &vNorm);
	return vNorm;
}

D3DXVECTOR3 CCollisionManager::GetReflectedVector(D3DXVECTOR3 vDir, D3DXVECTOR3 vNorm)
{
	D3DXVECTOR3 vReflection;
	//VNew = -2*(v Dot N)*N + v;
	vReflection = -2 * ( D3DXVec3Dot(&vDir, &vNorm) )*vNorm + vDir;
	return vReflection;
}
std::vector<TTriangle> CCollisionManager::GetAABBTriangles(TAABB aabb)
{
	std::vector<TTriangle> tBBTris;
	D3DXVECTOR3 tmin, tmax;
	tmin = aabb.cBoxMin;
	tmax = aabb.cBoxMax;
	D3DXVECTOR3 v0, v1, v2, v3;
	D3DXVECTOR3 e1, e2, tnorm;
	TTriangle newtri;
	//front
	v0 = D3DXVECTOR3( tmin.x, tmax.y, tmin.z );
	v1 = D3DXVECTOR3( tmax.x, tmax.y, tmin.z );
	v2 = D3DXVECTOR3( tmax.x, tmin.y, tmin.z );
	v3 = D3DXVECTOR3( tmin.x, tmin.y, tmin.z );
	e1 = v1 - v0;
	e2 = v3 - v0;
	D3DXVec3Cross(&tnorm, &e1, &e2);
	D3DXVec3Normalize(&tnorm, &tnorm);
	newtri.tNorm = tnorm;
	newtri.tVert0 = v0;	newtri.tVert1 = v1;	newtri.tVert2 = v3;
	tBBTris.push_back(newtri);
	newtri.tVert0 = v1;	newtri.tVert1 = v2;	newtri.tVert2 = v3;
	tBBTris.push_back(newtri);
	//back
	v0 = D3DXVECTOR3( tmax.x, tmax.y, tmax.z );
	v1 = D3DXVECTOR3( tmin.x, tmax.y, tmax.z );
	v2 = D3DXVECTOR3( tmin.x, tmin.y, tmax.z );
	v3 = D3DXVECTOR3( tmax.x, tmin.y, tmax.z );
	//	e1 = v1 - v0;
	//	e2 = v3 - v0;
	//	D3DXVec3Cross(&tnorm, &e1, &e2);
	tnorm = -tnorm;
	//	D3DXVec3Normalize(&tnorm, &tnorm);
	newtri.tNorm = tnorm;
	newtri.tVert0 = v0;	newtri.tVert1 = v1;	newtri.tVert2 = v3;
	tBBTris.push_back(newtri);
	newtri.tVert0 = v1;	newtri.tVert1 = v2;	newtri.tVert2 = v3;
	tBBTris.push_back(newtri);
	//left
	v0 = D3DXVECTOR3( tmin.x, tmax.y, tmax.z );
	v1 = D3DXVECTOR3( tmin.x, tmax.y, tmin.z );
	v2 = D3DXVECTOR3( tmin.x, tmin.y, tmin.z );
	v3 = D3DXVECTOR3( tmin.x, tmin.y, tmax.z );
	e1 = v1 - v0;
	e2 = v3 - v0;
	D3DXVec3Cross(&tnorm, &e1, &e2);
	D3DXVec3Normalize(&tnorm, &tnorm);
	newtri.tNorm = tnorm;
	newtri.tVert0 = v0;	newtri.tVert1 = v1;	newtri.tVert2 = v3;
	tBBTris.push_back(newtri);
	newtri.tVert0 = v1;	newtri.tVert1 = v2;	newtri.tVert2 = v3;
	tBBTris.push_back(newtri);
	//right
	v0 = D3DXVECTOR3( tmax.x, tmax.y, tmin.z );
	v1 = D3DXVECTOR3( tmax.x, tmax.y, tmax.z );
	v2 = D3DXVECTOR3( tmax.x, tmin.y, tmax.z );
	v3 = D3DXVECTOR3( tmax.x, tmin.y, tmin.z );
	//	e1 = v1 - v0;
	//	e2 = v3 - v0;
	//	D3DXVec3Cross(&tnorm, &e1, &e2);
	tnorm = -tnorm;
	newtri.tNorm = tnorm;
	newtri.tVert0 = v0;	newtri.tVert1 = v1;	newtri.tVert2 = v3;
	tBBTris.push_back(newtri);
	newtri.tVert0 = v1;	newtri.tVert1 = v2;	newtri.tVert2 = v3;
	tBBTris.push_back(newtri);
	return tBBTris;
}
D3DXVECTOR3 CCollisionManager::GetCloserPt(D3DXVECTOR3 tPt1, D3DXVECTOR3 tPt2, D3DXVECTOR3 tTestpt)
{
	float fDist1, fDist2;		//distances from tTestpt to tPt1 and tPt2 respectively
	fDist2 = fDist2 = 0.0f;
	//make vectors from each point to the test point and the one with
	//		the smaller distance is returned
	D3DXVECTOR3 v1, v2;
	v1 = D3DXVECTOR3( tTestpt - tPt1 );
	v2 = D3DXVECTOR3( tTestpt - tPt2 );
	fDist1 = D3DXVec3Length( &v1 );
	fDist2 = D3DXVec3Length( &v2 );
	return fDist1 <= fDist2 ? tPt1 : tPt2;	//bamf!
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////collision functions ^
//////////////////////////////////////////////////////////////////////////



void CCollisionManager::CheckRam(CCollideable* obj1, CCollideable* obj2, D3DXVECTOR3 tvel1, D3DXVECTOR3 tvel2)
{
	D3DXVECTOR3 t1to2 = obj2->GetSphere().cPosition - obj1->GetSphere().cPosition;
	D3DXVECTOR3 t2to1 = obj1->GetSphere().cPosition - obj2->GetSphere().cPosition;
	if(D3DXVec3Dot(&t1to2, &tvel1) > 0)
	{
		SendRamEvent("CRamEvent", obj1, obj1->GetParent(), obj2->GetParent());
	}
	if(D3DXVec3Dot(&t2to1, &tvel2) > 0)
	{
		SendRamEvent("CRamEvent", obj2, obj1->GetParent(), obj1->GetParent());
	}
}


void CCollisionManager::SearchForCollision(float /*fDeltaTime*/)
{

	if(m_cNonStaticObjects.size() + m_cStaticObjects.size() < 2)//not enough collideables
		return;
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator pNonSobj1;//first non static iter
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator pSobj1;	//static iter
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator pSobj2;	//static iter

	for(pSobj1 = m_cNonStaticObjects.begin(); pSobj1 != m_cNonStaticObjects.end(); ++pSobj1)
	{
		for(pNonSobj1 = m_cStaticObjects.begin(); pNonSobj1 != m_cStaticObjects.end(); ++pNonSobj1)
		{
			if( 	!(*pSobj1).second->GetWasChecked() && !(*pNonSobj1).second->GetWasChecked() )
			{
				TestCollision( (*pSobj1).second, (*pNonSobj1).second );
			}
		}
		for(pSobj2 = m_cNonStaticObjects.begin(); pSobj2 != m_cNonStaticObjects.end(); ++pSobj2)
		{
			if( (*pSobj1) != (*pSobj2)/* &&
									  !(*pSobj1).second->GetWasChecked() && !(*pSobj2).second->GetWasChecked()*/ )
			{
				TestCollision((*pSobj1).second, (*pSobj2).second);
			}
		}

	TestObjAgainstWall(pSobj1->second);
		(*pSobj1).second->SetWasChecked(true);
	}
	SetAllNotChecked();
}

bool CCollisionManager::TestCollision(CCollideable* obj1, CCollideable* obj2)
{
	D3DXVECTOR3 vObjNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	if(obj1->GetBVType() == BSPHERE)
	{
		TSphere update1;
		update1.cPosition = obj1->GetParent()->GetTransform()->GetWorldPosition();
		update1.cPosition.y = 0.0f;
		update1.fRadius = obj1->GetSphere().fRadius;
		obj1->SetSphere(update1);
	}
	if(obj2->GetBVType() == BSPHERE)
	{
		TSphere update2;
		update2.cPosition = obj2->GetParent()->GetTransform()->GetWorldPosition();
		update2.cPosition.y = 0.0f;
		update2.fRadius = obj2->GetSphere().fRadius;
		obj2->SetSphere(update2);
	}
	if(obj1->GetBVType() == BCAPSULE)
	{
		obj1->m_tLocalAxes[0].x = obj1->GetParent()->GetTransform()->GetLocalMatrix()._11;
		obj1->m_tLocalAxes[0].y = obj1->GetParent()->GetTransform()->GetLocalMatrix()._12;
		obj1->m_tLocalAxes[0].z = obj1->GetParent()->GetTransform()->GetLocalMatrix()._13;
								
		obj1->m_tLocalAxes[1].x = obj1->GetParent()->GetTransform()->GetLocalMatrix()._21;
		obj1->m_tLocalAxes[1].y = obj1->GetParent()->GetTransform()->GetLocalMatrix()._22;
		obj1->m_tLocalAxes[1].z = obj1->GetParent()->GetTransform()->GetLocalMatrix()._23;
								
		obj1->m_tLocalAxes[2].x = obj1->GetParent()->GetTransform()->GetLocalMatrix()._31;
		obj1->m_tLocalAxes[2].y = obj1->GetParent()->GetTransform()->GetLocalMatrix()._32;
		obj1->m_tLocalAxes[2].z = obj1->GetParent()->GetTransform()->GetLocalMatrix()._33;
		TCapsule updatecap;
		updatecap.cRear = obj1->GetParent()->GetTransform()->GetWorldPosition();
		updatecap.cFront = updatecap.cRear * obj1->GetParent()->GetTransform()->GetLocalMatrix()._33*1.0f;
		updatecap.fRadius = obj1->GetSphere().fRadius;
		obj1->SetCapsule(updatecap);
	}
	if(obj1->GetBVType() == BOBB)
	{
		TOBB tbox;
		tbox.cCenterPoint = obj1->GetParent()->GetTransform()->GetWorldPosition();
		tbox.tE = obj1->GetOBB().tE;
		tbox.tU[0].x = obj1->GetParent()->GetTransform()->GetLocalMatrix()._11;
		tbox.tU[0].y = obj1->GetParent()->GetTransform()->GetLocalMatrix()._12;
		tbox.tU[0].z = obj1->GetParent()->GetTransform()->GetLocalMatrix()._13;
		
		tbox.tU[1].x = obj1->GetParent()->GetTransform()->GetLocalMatrix()._21;
		tbox.tU[1].y = obj1->GetParent()->GetTransform()->GetLocalMatrix()._22;
		tbox.tU[1].z = obj1->GetParent()->GetTransform()->GetLocalMatrix()._23;
		
		tbox.tU[2].x = obj1->GetParent()->GetTransform()->GetLocalMatrix()._31;
		tbox.tU[2].y = obj1->GetParent()->GetTransform()->GetLocalMatrix()._32;
		tbox.tU[2].z = obj1->GetParent()->GetTransform()->GetLocalMatrix()._33;
		obj1->SetOBB(tbox);
	}
	if(obj2->GetBVType() == BOBB)
	{
		TOBB tbox;
		tbox.cCenterPoint = obj2->GetParent()->GetTransform()->GetWorldPosition();
		tbox.tE = obj2->GetOBB().tE;
		tbox.tU[0].x = obj2->GetParent()->GetTransform()->GetLocalMatrix()._11;
		tbox.tU[0].y = obj2->GetParent()->GetTransform()->GetLocalMatrix()._12;
		tbox.tU[0].z = obj2->GetParent()->GetTransform()->GetLocalMatrix()._13;
		
		tbox.tU[1].x = obj2->GetParent()->GetTransform()->GetLocalMatrix()._21;
		tbox.tU[1].y = obj2->GetParent()->GetTransform()->GetLocalMatrix()._22;
		tbox.tU[1].z = obj2->GetParent()->GetTransform()->GetLocalMatrix()._23;
		
		tbox.tU[2].x = obj2->GetParent()->GetTransform()->GetLocalMatrix()._31;
		tbox.tU[2].y = obj2->GetParent()->GetTransform()->GetLocalMatrix()._32;
		tbox.tU[2].z = obj2->GetParent()->GetTransform()->GetLocalMatrix()._33;
		obj2->SetOBB(tbox);
	}


	switch(obj1->GetObjType())
	{
	case OBJPLAYER:
		{
			switch(obj2->GetObjType())
			{
			case OBJPLAYER:
				{
//					if(SphereToSphereIntersection(obj1, obj2, m_pReflect, true))
					if(OBBToOBBIntersection(obj1, obj2, m_pReflect, true))
					{
//						SphereToSphereIntersection(obj1, obj2, m_pReflect, true);//this will correctly set the reflect at least
						//player to player
						// Swapped the order of cart ram and cart collision so that
						//   the speed reduction in cart collision doesn't affect cart ram


						// DONT F'ING TOUCH THIS THIS IS WHY WE DROPPED MULTIPLE ITEMS LEAVE THIS ALONE!!!!! - Agent Smith
						SendRamEvent("CartRam", (IComponent*)GetInstance(), obj1->GetParent(), obj2->GetParent());
						SendRamEvent("CartRam", (IComponent*)GetInstance(), obj2->GetParent(), obj1->GetParent());
						// end yell

						//send out the collision response event once for each cart involved
						// object 1 collided with object 2
						//NOTE: the vReflect being passed through the event here is quite possibly wrong, but it is quite possibly right to	-Raymoney Cashdollar
						SendImpactEvent("CartCollision", (IComponent*)GetInstance(), obj1->GetParent(), 
							obj2->GetParent(), *m_pReflect,PRIORITY_IMMEDIATE);		
						//	object 2 collided with object 1
						SendImpactEvent("CartCollision", (IComponent*)GetInstance(), obj2->GetParent(),
							obj1->GetParent(), *m_pReflect, PRIORITY_IMMEDIATE);
						if (CHUDManager::GetInstance()->GetPlayerNum(obj1->GetParent()) == 0 || 
							CHUDManager::GetInstance()->GetPlayerNum(obj2->GetParent()) == 0)
						{
							CWwiseSoundManager::GetInstance()->PlayTheSound(CART_PLAYER_COLLISION, BIKER_ID);
						}
						return true;
					}
				}
				break;
			case OBJGITEM:
				{//player to goal item
//					if(SphereToSphereIntersection(obj1, obj2, m_pReflect, false))
					if(SphereToOBBIntersection(obj2, obj1, m_pReflect, false))
					{
						SendGoalItemCollectedEvent("GoalItemCollision", (IComponent*)GetInstance(), 
							obj2->GetParent(), obj1->GetParent(), PRIORITY_IMMEDIATE);
						return true;
					}
				}
				break;
			case OBJHELDITEM:
				{//player to held item
//					if(SphereToSphereIntersection(obj1, obj2, m_pReflect, false))
					if(SphereToOBBIntersection(obj2, obj1, m_pReflect, false))
					{
						SendHeldItemCollectedEvent("HeldItemCollision",
							(IComponent*)GetInstance(), obj2->GetParent(), obj1->GetParent(), PRIORITY_IMMEDIATE);
						return true;
					}
				}
				break;
			case OBJPITEM:
				{//player to pick up item
//					if(SphereToSphereIntersection(obj1, obj2, m_pReflect, false))
					if(SphereToOBBIntersection(obj2, obj1, m_pReflect, false))
					{
						SendGoalItemCollectedEvent("PickupItemCollision",
							(IComponent*)GetInstance(), obj2->GetParent(), obj1->GetParent(), PRIORITY_IMMEDIATE);
						return true;
					}
				}
				break;
			case OBJWALL:
				{//player to wall
//					if(SphereToPlaneIntersection(obj1, obj2, m_pReflect, true))
//					{
//						SendImpactEvent("EnvironmentHit", (IComponent*)GetInstance(),
//							obj1->GetParent(), obj2->GetParent(), obj2->GetPlane().cPlaneNormal, PRIORITY_IMMEDIATE);
//						////sound hack
//
//						if (CHUDManager::GetInstance()->GetPlayerNum(obj1->GetParent()) == 0 || 
//							CHUDManager::GetInstance()->GetPlayerNum(obj2->GetParent()) == 0)
//						{
//							CWwiseSoundManager::GetInstance()->PlayTheSound(CART_WALL_COLLISION, BIKER_ID);
//						}
//
//						return true;
//					}
					return false;//!!!! hack, needs to think the wall is a tree-ingle
				}
				break;
			case OBJSHELF:
				{//player to shelf
//					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
//					if(OBBToAABBIntersection(obj1, obj2, m_pReflect, true))
//					if(MovingSphereToAABBIntersection(obj1, obj2, m_pReflect, true))
					D3DXVECTOR3 velociraptor = obj1->m_tLocalAxes[2];
					D3DXVec3Normalize(&velociraptor, &velociraptor);
					float fsped = CMovementManager::GetInstance()->GetPlayerSpeed(obj1->GetParent());
					velociraptor *= fsped;
					if(MovingSphereToAABBIntersection(obj1, obj2, velociraptor, m_pReflect, true))
					{
						//						SendObjectEvent("ShelfHit", obj1, obj2->GetParent());
						//						vObjNormal = GetAABBNormal(m_pReflect, obj2->GetAABB());
						SendImpactEvent("EnvironmentHit", (IComponent*)GetInstance(), obj1->GetParent(), 
							obj2->GetParent(), *m_pReflect, PRIORITY_IMMEDIATE);
						return true;
					}
				}
				break;
			case OBJFSD:
				{//player to free standing display(the circle things)
					if(SphereToSphereIntersection(obj1, obj2, m_pReflect, true))
//					if(OBBToAABBIntersection(obj1, obj2, m_pReflect, true))
					{
						// dont send so we just slide off
						SendImpactEvent("EnvironmentHit", (IComponent*)GetInstance(), obj1->GetParent(), 
							obj2->GetParent(), *m_pReflect, PRIORITY_IMMEDIATE);
						return true;
					}
				}
				break;
			case OBJENDCAP:
				{//player to endcap
					D3DXVECTOR3 velociraptor = obj1->m_tLocalAxes[2];
					D3DXVec3Normalize(&velociraptor, &velociraptor);
					float fsped = CMovementManager::GetInstance()->GetPlayerSpeed(obj1->GetParent());
					velociraptor *= fsped;
					if(MovingSphereToAABBIntersection(obj1, obj2, velociraptor, m_pReflect, true))
//					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
//					if(OBBToAABBIntersection(obj1, obj2, m_pReflect, true))
					{
						SendObjectEvent("EndcapHit", (IComponent*)GetInstance(), obj2->GetParent());
						return true;
					}
				}
				break;
			case OBJCHECKOUT:
				{//player to checkout
					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, false))
//					if(OBBToAABBIntersection(obj1, obj2, m_pReflect, false))//really don't need a complex test to see if you're there -raymoney
					{
						SendObjectEvent("CheckoutCollision", obj1, obj1->GetParent());
						return true;
					}
				}
				break;
			case OBJBANANA:
			case OBJJAM:
			case OBJPBUTTER:
			case OBJPIE:
			case OBJTURKEY:
				{//player to used held item
//					if(SphereToSphereIntersection(obj1, obj2, m_pReflect, false))
					if(SphereToOBBIntersection(obj2, obj1, m_pReflect, false))
					{
						SendImpactEvent("HeldItemInWorldPlayerCollision", obj1, 
							obj1->GetParent(), obj2->GetParent(), D3DXVECTOR3(0.0f, 1.0f, 0.0f), PRIORITY_IMMEDIATE);
						return true;
					}
					break;
				}
			default:
				return false;
			}
		}
		break;
	case OBJPITEM:
		{
			switch(obj2->GetObjType())
			{
			case OBJWALL:
				{//pickup item to wall
//					if(SphereToPlaneIntersection(obj1, obj2, m_pReflect, true))
//					{
//						return true;
//					}
					return false;//!!!! hack, needs to see wall as triangle
				}
				break;
			case OBJSHELF:
				{//pickup item to shelf
					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
					{
						//something about camera with shelf collision
						return true;
					}
				}
				break;
			case OBJENDCAP:
				{//pickup item to endcap
					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
					{
						//something about camera to endcap collision
						return true;
					}
				}
				break;
			case OBJFSD:
				{//pickup item to free standing display
					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
					{
						//something about camera to endcap collision
						return true;
					}
				}
				break;
			default:
				return false;
			}
		}
		break;
	case OBJCAMERA:
		{
			switch(obj2->GetObjType())
			{
			case OBJWALL:
				{//kamura to woll
//					if(SphereToPlaneIntersection(obj1, obj2, m_pReflect, true))
//					{
//						return true;
//					}
					return false;	//!!! hack, needs wall triangles
				}
				break;
			case OBJSHELF:
				{//kamura to shelph
					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
					{
						//something about camera with shelf collision
						return true;
					}
				}
				break;
			case OBJENDCAP:
				{//kamura to entkapp
					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
					{
						//something about camera to endcap collision
						return true;
					}
				}
				break;
			default:
				return false;
			}
		}
		break;
	case OBJBANANA:
	case OBJJAM:
	case OBJPBUTTER:
	case OBJPIE:
	case OBJTURKEY:
		{
			//////////////////////////////////////////////////
			//used held items collision with stuff!
			//////////////////////////////////////////////////			
			switch(obj2->GetObjType())
			{
			case OBJSHELF:
				{//any used held item to a shelf
					D3DXVECTOR3 velociraptor;// = obj1->m_tLocalAxes[2];
					velociraptor.x = obj1->GetParent()->GetTransform()->GetLocalMatrix()._31;
					velociraptor.y = obj1->GetParent()->GetTransform()->GetLocalMatrix()._32;
					velociraptor.z = obj1->GetParent()->GetTransform()->GetLocalMatrix()._33;
					obj1->m_tLocalAxes[2] = velociraptor;
					D3DXVec3Normalize(&velociraptor, &velociraptor);
					float fsped = 20.0f;//CMovementManager::GetInstance()->GetPlayerSpeed(obj1->GetParent());
					velociraptor *= fsped;
					if(MovingSphereToAABBIntersection(obj1, obj2, velociraptor, m_pReflect, true))
						//if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
						{
							//						vObjNormal = GetAABBNormal(m_pReflect, obj2->GetAABB());
							obj1->GetParent()->GetTransform()->TranslateFrame((*m_pReflect * 0.5f));
							SendImpactEvent("HeldItemInWorldCollision", obj1, 
								obj1->GetParent(), obj2->GetParent(), *m_pReflect, PRIORITY_IMMEDIATE);
							return true;
						}

				}
				break;
			case OBJENDCAP:
				{//any used held item to an endcap
					D3DXVECTOR3 velociraptor;// = obj1->m_tLocalAxes[2];
					velociraptor.x = obj1->GetParent()->GetTransform()->GetLocalMatrix()._31;
					velociraptor.y = obj1->GetParent()->GetTransform()->GetLocalMatrix()._32;
					velociraptor.z = obj1->GetParent()->GetTransform()->GetLocalMatrix()._33;
					D3DXVec3Normalize(&velociraptor, &velociraptor);
					float fsped = 20.0f;//CMovementManager::GetInstance()->GetPlayerSpeed(obj1->GetParent());
					velociraptor *= fsped;
					if(MovingSphereToAABBIntersection(obj1, obj2, velociraptor, m_pReflect, true))
//					if(SphereToAABBIntersection(obj1, obj2, m_pReflect, true))
					{
						//						vObjNormal = GetAABBNormal(m_pReflect, obj2->GetAABB());
						obj1->GetParent()->GetTransform()->TranslateFrame((*m_pReflect * 0.5f));
						SendImpactEvent("HeldItemInWorldCollision", obj1, 
							obj1->GetParent(), obj2->GetParent(), *m_pReflect, PRIORITY_IMMEDIATE);
						return true;
					}
				}
				break;
			case OBJFSD:
				{//any used held item to a free standing display
					if(SphereToSphereIntersection(obj1, obj2, m_pReflect, true))
					{
						obj1->GetParent()->GetTransform()->TranslateFrame((*m_pReflect * 0.5f));
						if(fabs(m_pReflect->x) > fabs(m_pReflect->z))
						{
							m_pReflect->z = 0.0f;
						}
						else if(fabs(m_pReflect->z) > fabs(m_pReflect->x))
						{
							m_pReflect->x = 0.0f;
						}
						SendImpactEvent("HeldItemInWorldCollision", obj1, 
							obj1->GetParent(), obj2->GetParent(), *m_pReflect, PRIORITY_IMMEDIATE);
						return true;
					}
				}
				break;
			};
		}
		break;
	default:
		return false;	//no obj type probably means no collision volume, so. . . no good
	};
	return false;
}
///-----------------------------------------------------------------------------
///
//Changes:----------------------------------------------------
///
///-----------------------------------------------------------------------------
bool CCollisionManager::MovingSphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2, 
									D3DXVECTOR3 tVelocity, D3DXVECTOR3* vReflect, bool bReact)
{
	TSphere nextsphere, currsphere;
	TAABB box;
	D3DXVECTOR3 currReflect, nextReflect;
	bool currcol, nextcol;	currcol = nextcol = false;
	box.cBoxMin = obj2->GetAABB().cBoxMin;
	box.cBoxMax = obj2->GetAABB().cBoxMax;

	//added security, scale the box by the sphere radius
	for(unsigned i = 0; i < 3; ++i)
	{//scale the aabb by the radius of the player
		box.cBoxMin[i] -= obj1->m_fExtents[0];
		box.cBoxMax[i] += obj1->m_fExtents[0];
	}

	nextsphere.fRadius = currsphere.fRadius = obj1->m_fExtents[0];
	currsphere.cPosition = obj1->m_cCenterPoint;
	nextsphere.cPosition = obj1->m_cCenterPoint + tVelocity;

	//test next position for collision
	obj1->SetSphere(nextsphere);
	nextcol = SphereToAABBIntersection(obj1, obj2, &nextReflect, false);
	//test current position for collision
	obj1->SetSphere(currsphere);
	currcol = SphereToAABBIntersection(obj1, obj2, &currReflect, false);

	if(!currcol && !nextcol)
	{
		//no collision
		return false;
	}
	else if(currcol && nextcol)
	{
		//collides with prev curr and next locations
		//dot < 0 bad
		if(D3DXVec3Dot(&currReflect, &nextReflect) <= 0.0f)
		{//the normal from the next frame is facing a different direction so it's colliding with a different side
			obj1->SetSphere(currsphere);
			SphereToAABBIntersection(obj1, obj2, &currReflect, true);
			*vReflect = currReflect;
			return true;
		}
		else
		{//hmm, i seem to be doing the same thing here, but thats cool because this should mean no tunneling happens

			obj1->SetSphere(currsphere);
			SphereToAABBIntersection(obj1, obj2, &currReflect, true);
			*vReflect = currReflect;
			return true;
		}
	}
	else if(currcol && !nextcol)
	{
		//collides with curr but not next location
		obj1->SetSphere(currsphere);
		SphereToAABBIntersection(obj1, obj2, &currReflect, true);
		*vReflect = currReflect;
		return true;
	}
	else if(!currcol && nextcol)
	{
		return false;
	}
	else
	{
		return false;
	}
}

bool CCollisionManager::RayToSphereIntersection(TRay ray, TSphere sphere, float & t, D3DXVECTOR3 &intersect)
{
	return false;
}
bool CCollisionManager::RayToTriangleIntersection(TTriangle tri, TRay ray, float &t)
{
	return false;
}
bool CCollisionManager::MovingSphereToTriangleIntersection(CCollideable* obj, TTriangle* tri,
														   D3DXVECTOR3* vReflect, D3DXVECTOR3 * tCol, float &t, bool bReact)
{
	return false;
}

D3DXVECTOR3 CCollisionManager::ClosestPtPointTriangle(D3DXVECTOR3 p, D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c)
{
	// Check if P in vertex region outside A
	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ac = c - a;
	D3DXVECTOR3 ap = p - a;
	float d1 = D3DXVec3Dot(&ab, &ap);
	float d2 = D3DXVec3Dot(&ac, &ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return D3DXVECTOR3(0.0f, 0.0f, 0.0f); //b; // barycentric coordinates (1,0,0)

	// Check if P in vertex region outside B
	D3DXVECTOR3 bp = p - b;
	float d3 = D3DXVec3Dot(&ab, &bp);
	float d4 = D3DXVec3Dot(&ac, &bp);
	if(d3 >= 0.0f && d4 <= d3) return D3DXVECTOR3(0.0f, 0.0f, 0.0f); //b; // barycentric coordinates (0,1,0)

	// Check if P in edge region of AB, if so return projection of P onto AB
	float vc = d1*d4 - d3*d2;
	if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f); //a + v * ab; // barycentric coordinates (1-v, v, 0)
	}

	// Check if P in vertex region outside C
	D3DXVECTOR3 cp = p - c;
	float d5 = D3DXVec3Dot(&ab, &cp);
	float d6 = D3DXVec3Dot(&ac, &cp);
	if(d6 >= 0.0f && d5 <= d6) return D3DXVECTOR3(0.0f, 0.0f, 0.0f); //c; // barycentric coordinates (0,0,1)

	// Check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5*d2 - d1*d6;
	if(vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f); //a + w * ac; // barycentric coordinates (1-w,0,w)
	}

	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3*d6 - d5*d4;
	if(va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f); //b + w * (c - b); // barycentric coordinates (0,1-w,w)
	}

	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

int CCollisionManager::TestObjAgainstWall(CCollideable* obj1)
{

	TSphere s = obj1->GetSphere();
	D3DXVECTOR3 mypos = s.cPosition + D3DXVECTOR3(0.0f, 0.1f, 0.0f);

	for(unsigned int i = 0; i < m_nTriCount; ++i)
	{
		D3DXVECTOR3 p = ClosestPtPointTriangle(mypos, m_pWallTris[i].tVert0, m_pWallTris[i].tVert1,
			m_pWallTris[i].tVert2);

		//D3DXVECTOR3 testNorm = (s.cPosition - p);
		//testNorm.y = m_pWallTris[i].tNorm.y;

		if(D3DXVec3Dot(&p, &p) != 0.0f) //D3DXVec3Dot(&testNorm, &m_pWallTris[i].tNorm) >= 1.0f)
		{
			D3DXVECTOR3 v = p - s.cPosition;
			D3DXVECTOR3 test = p + m_pWallTris[i].tNorm * (s.fRadius+ 0.1f);
			if(HalfSpaceTest(m_pWallTris[i].tNorm, test, s.cPosition) < 1) 
			{
				D3DXMATRIX& mymat = obj1->GetParent()->GetTransform()->GetLocalMatrix();
				mymat._41 = test.x;
				mymat._42 = test.y;
				mymat._43 = test.z;			
				if(obj1->GetObjType() == OBJTURKEY || obj1->GetObjType() == OBJPIE)
				{
					*m_pReflect = m_pWallTris[i].tNorm;
					SendImpactEvent("HeldItemInWorldCollision", obj1, 
						obj1->GetParent(), (CObject*)GetInstance(), *m_pReflect, PRIORITY_IMMEDIATE);
				}
			}
		}
	}

	return 0;
}

// this version is for the camera only
int CCollisionManager::TestObjAgainstWall(D3DXVECTOR3 &cameraFrame)
{

	TSphere s;
	s.cPosition = cameraFrame;
	s.fRadius = 0.5f;
	D3DXVECTOR3 mypos = s.cPosition;

	for(unsigned int i = 0; i < m_nTriCount; ++i)
	{
		D3DXVECTOR3 p = ClosestPtPointTriangle(mypos, m_pWallTris[i].tVert0, m_pWallTris[i].tVert1,
			m_pWallTris[i].tVert2);

		if(D3DXVec3Dot(&p, &p) != 0.0f)
		{
			D3DXVECTOR3 v = p - s.cPosition;
			D3DXVECTOR3 test = p + m_pWallTris[i].tNorm * (s.fRadius+ 0.1f);
			if(HalfSpaceTest(m_pWallTris[i].tNorm, test, s.cPosition) < 1) 
			{
				cameraFrame.x = test.x;
				cameraFrame.y = test.y;
				cameraFrame.z = test.z;			
			}
		}
	}

	return 0;
}









