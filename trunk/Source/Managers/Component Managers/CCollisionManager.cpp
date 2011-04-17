#include "CCollisionManager.h"
#include "../Global Managers/Event Manager/CEventManager.h"
#include "..\..\..\Source\CObject.h"
#include "..\Global Managers\Event Manager\CGoalItemCollected.h"
#include "..\Global Managers\Event Manager\CRamEvent.h"


CCollisionManager::CCollisionManager()
{
	TAABB taabb;
	taabb.cBoxMax = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
	taabb.cBoxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pRoot = MMNEW(CBVHNode)CBVHNode(1, taabb);
}
CCollisionManager::~CCollisionManager()
{

}

CCollisionManager* CCollisionManager::GetInstance()
{
	static CCollisionManager cInstance;
	return &cInstance;
}
void CCollisionManager::Init()
{
//	m_pRoot->SetSplitType(ZSPLIT);
//	m_pRoot->SetIsLeaf(false);
//	InitBVH();
	//register for events
	//	m_pEventManager->RegisterEvent("Update", NULL, Update);
	//	m_pEventManager->RegisterEvent("Shutdown", NULL, Shutdown);
	/*
	m_pEventManager->RegisterEvent("PlayerToPlayer", NULL, CollisionPlayerToPlayer);
	m_pEventManager->RegisterEvent("PlayerRammed", NULL, CollisionPlayerRammed);
	m_pEventManager->RegisterEvent("PlaerToItem", NULL, CollisionPlayerToItem);
	m_pEventManager->RegisterEvent("PlayerToShelf", NULL, CollisionPlayerToShelf);
	m_pEventManager->RegisterEvent("PlayerToEndcap", NULL, CollisionPlayerToEndcap);
	m_pEventManager->RegisterEvent("PlayerToWall", NULL, CollisionPlayerToWall);
	m_pEventManager->RegisterEvent("PlayerToAttackItem", NULL, CollisionPlayerToAttackItem);
	m_pEventManager->RegisterEvent("PlayerToHazard", NULL, CollisionPlayerToHazard);
	*/
}

//component functions
int CCollisionManager::CreateCollideableComponent(lua_State* pLua)
{
	CObject* pObject = (CObject*)lua_topointer(pLua, 1);
	bool bisStatic = (lua_toboolean(pLua, 2) !=0 );
	int type = (int)lua_tointeger(pLua, 3);
	TAABB tAABB;
	TSphere tsphere;
	TPlane tplane;
	CCollideable* pCollideable = CreateCollideableComponent(pObject);
	pCollideable->SetStatic(bisStatic);
	pCollideable->SetBVType(type);
	switch(type)
	{
	case BSPHERE:
		tsphere.cPosition.x = (float)lua_tonumber(pLua, 4);
		tsphere.cPosition.y = (float)lua_tonumber(pLua, 5);
		tsphere.cPosition.z = (float)lua_tonumber(pLua, 6);
		tsphere.fRadius = (float)lua_tonumber(pLua, 7);
		pCollideable->SetSphere(tsphere);
		lua_pop(pLua, 7);
		break;
	case BAABB:
		tAABB.cBoxMax.x = (float)lua_tonumber(pLua, 4);
		tAABB.cBoxMax.y = (float)lua_tonumber(pLua, 5);
		tAABB.cBoxMax.z = (float)lua_tonumber(pLua, 6);
		tAABB.cBoxMin.x = (float)lua_tonumber(pLua, 7);
		tAABB.cBoxMin.y = (float)lua_tonumber(pLua, 8);
		tAABB.cBoxMin.z = (float)lua_tonumber(pLua, 9);
		pCollideable->SetAABB(tAABB);
		lua_pop(pLua, 9);
		break;
	case BPLANE:
		tplane.cPlaneNormal.x = (float)lua_tonumber(pLua, 4);
		tplane.cPlaneNormal.y = (float)lua_tonumber(pLua, 5);
		tplane.cPlaneNormal.z = (float)lua_tonumber(pLua, 6);
		tplane.cPlanePoint.x = (float)lua_tonumber(pLua, 7);
		tplane.cPlanePoint.y = (float)lua_tonumber(pLua, 8);
		tplane.cPlanePoint.z = (float)lua_tonumber(pLua, 9);
		pCollideable->SetPlane(tplane);
		lua_pop(pLua, 9);
		break;
		
	}
	//temp stuff
	
	return 0;
}
CCollideable* CCollisionManager::CreateCollideableComponent(CObject* pParent)
{
	CCollideable* pCollideComp = MMNEW(CCollideable) CCollideable();
	pCollideComp->SetParent(pParent);
	//pCollideComp->Init();
	pParent->AddComponent(pCollideComp);

	GetInstance()->AddNonStatic(pCollideComp, (unsigned)pParent->GetID());
	return pCollideComp;
}


void CCollisionManager::Shutdown(IEvent* pEvent, IComponent* pComponent)
{
}
void CCollisionManager::Update(IEvent* pEvent, IComponent* pComponent)
{

}

void CCollisionManager::AddStatic(CCollideable* cStatic, unsigned int nObjID)
{	
	//adds the static component to the list of statics
	m_cStaticObjects.insert(make_pair(nObjID, cStatic));
}
void CCollisionManager::RemoveStatic(CCollideable* cStatic, unsigned int nObjID)
{
	//find it
	std::map<unsigned int, CCollideable*>::iterator statiter = \
		m_cStaticObjects.find(nObjID);
	//delete it
	m_cStaticObjects.erase(statiter);

}
void CCollisionManager::AddNonStatic(CCollideable* cCollide, unsigned int nObjID)
{
	//adds the non static component to the list of non-statics
	m_cNonStaticObjects.insert(make_pair(nObjID, cCollide));
}
void CCollisionManager::RemoveNonStatic(CCollideable* cCollide, unsigned int nObjID)
{
	std::map<unsigned int, CCollideable*>::iterator nonstatiter = \
		m_cNonStaticObjects.find(nObjID);

	//delete it
	m_cNonStaticObjects.erase(nonstatiter);
}


bool CCollisionManager::PointInCone(D3DXVECTOR3 tPlayerMove, D3DXVECTOR3 tPlayerPos, D3DXVECTOR3 tTargetPos, float fCone)
{
	return false;
}


bool CCollisionManager::SphereToSphereIntersection(TSphere tSphere1, TSphere tSphere2)
{
	float fDist = 0.0f;
	D3DXVECTOR3 vec1to2 = tSphere2.cPosition - tSphere1.cPosition;
	fDist = D3DXVec3Length(&vec1to2);
	//		(tSphere1.cPosition.x-tSphere2.cPosition.x)*(tSphere1.cPosition.x-tSphere2.cPosition.x) + 
	//		(tSphere1.cPosition.y - tSphere2.cPosition.y)*(tSphere1.cPosition.y - tSphere2.cPosition.y) + 
	//		(tSphere1.cPosition.z - tSphere2.cPosition.z)*(tSphere1.cPosition.z - tSphere2.cPosition.z);
	float frads = ((tSphere1.fRadius+tSphere2.fRadius)*(tSphere1.fRadius+tSphere2.fRadius));
	if(fDist <= tSphere1.fRadius+tSphere2.fRadius)
		return true;
	else
		return false;
}
bool CCollisionManager::PlayerToPlayerIntersection(TSphere tSphere1, TSphere tSphere2, 
												   D3DXVECTOR3 tVel1, D3DXVECTOR3 tVel2, 
												   D3DXVECTOR3 &tNew1, D3DXVECTOR3 &tNew2)
{
	//this will handle both ramming and regular cart to cart collision
	float fDist = 0.0f;
	D3DXVECTOR3 vec1to2 = tSphere2.cPosition - tSphere1.cPosition;
	fDist = D3DXVec3Length(&vec1to2);
	float fradii = tSphere1.fRadius+tSphere2.fRadius;
	if(fDist <= fradii)
	{
		float fPen = fradii - fDist;
		D3DXVECTOR3 ts1tos2 = tSphere2.cPosition - tSphere1.cPosition;
		D3DXVECTOR3 ts2tos1 = tSphere1.cPosition - tSphere2.cPosition;
		D3DXVec3Normalize(&ts1tos2, &ts1tos2);
		D3DXVec3Normalize(&ts2tos1, &ts2tos1);
		ts1tos2*=fPen;
		ts2tos1*=fPen;
		tNew1 = ts1tos2;
		tNew2 = ts2tos1;
		return true;
	}
	else
	{
		return false;
	}
}

bool CCollisionManager::SphereToPlaneIntersection(TSphere tSphere, TPlane tPlane,
												  D3DXVECTOR3& tNewSCenter)
{	
	//vector calculated, from plane to sphere, then define distance of the vec3
	D3DXVECTOR3 tV = tSphere.cPosition - tPlane.cPlanePoint;
	float fdist = D3DXVec3Dot(&tV, &tPlane.cPlaneNormal);
	//collision
	if(fdist < tSphere.fRadius)
	{
		//adjust the position by a reflected vector
		tNewSCenter = tSphere.cPosition + 
			(tPlane.cPlaneNormal * (tSphere.fRadius - fdist))*5;
		return true;
	}
	else
	{
		return false;
	}
}

bool CCollisionManager::SphereToOBBIntersection(TSphere tSphere, TOBB tOBB, 
												D3DXVECTOR3& tNewSCenter)
{
	D3DXVECTOR3 tSCentercopy = tSphere.cPosition;
	TPlane tplane1, tplane2, tplane3, tplane4, tplane5, tplane6;

	//front
	D3DXVec3Cross(&tplane1.cPlaneNormal, &(tOBB.cFrontTopLeft-tOBB.cFrontBottomLeft),
		&(tOBB.cFrontBottomRight-tOBB.cFrontBottomLeft) );
	D3DXVec3Normalize(&tplane1.cPlaneNormal, &tplane1.cPlaneNormal);
	tplane1.cPlanePoint = tOBB.cFrontBottomLeft;

	//right
	D3DXVec3Cross(&tplane2.cPlaneNormal, &(tOBB.cFrontTopRight-tOBB.cFrontBottomRight),
		&(tOBB.CBackBottomRight-tOBB.cFrontBottomRight) );
	D3DXVec3Normalize(&tplane2.cPlaneNormal, &tplane2.cPlaneNormal);
	tplane2.cPlanePoint = tOBB.cFrontBottomRight;

	//left
	D3DXVec3Cross(&tplane3.cPlaneNormal, &(tOBB.cBackTopLeft-tOBB.cBackBottomLeft),
		&(tOBB.cFrontBottomLeft-tOBB.cBackBottomLeft) );
	D3DXVec3Normalize(&tplane3.cPlaneNormal, &tplane3.cPlaneNormal);
	tplane3.cPlanePoint = tOBB.cBackBottomLeft;

	//top
	D3DXVec3Cross(&tplane4.cPlaneNormal, &(tOBB.cBackTopLeft-tOBB.cFrontTopLeft),
		&(tOBB.cFrontTopRight-tOBB.cFrontTopLeft) );
	D3DXVec3Normalize(&tplane4.cPlaneNormal, &tplane4.cPlaneNormal);
	tplane4.cPlanePoint = tOBB.cFrontTopLeft;

	//bottom
	D3DXVec3Cross(&tplane5.cPlaneNormal, &(tOBB.cFrontBottomLeft-tOBB.cBackBottomLeft),
		&(tOBB.CBackBottomRight-tOBB.cBackBottomLeft) );
	D3DXVec3Normalize(&tplane5.cPlaneNormal, &tplane5.cPlaneNormal);
	tplane5.cPlanePoint = tOBB.cBackBottomLeft;

	//back
	D3DXVec3Cross(&tplane6.cPlaneNormal, &(tOBB.cBackTopRight-tOBB.CBackBottomRight),
		&(tOBB.cBackBottomLeft-tOBB.CBackBottomRight) );
	D3DXVec3Normalize(&tplane6.cPlaneNormal, &tplane6.cPlaneNormal);
	tplane6.cPlanePoint = tOBB.CBackBottomRight;


	//check order: front - right - left - top - bottom - back
	if( SphereToPlaneIntersection(tSphere, tplane1, tNewSCenter) ||
		SphereToPlaneIntersection(tSphere, tplane2, tNewSCenter) ||
		SphereToPlaneIntersection(tSphere, tplane3, tNewSCenter) ||
		SphereToPlaneIntersection(tSphere, tplane4, tNewSCenter) ||
		SphereToPlaneIntersection(tSphere, tplane5, tNewSCenter) ||
		SphereToPlaneIntersection(tSphere, tplane6, tNewSCenter) )
	{
		//it collided with one of the six planes of the OBB! 
		//SphereToPlane only sets the new center on collision
		//	so if any collision occured(can only be 1(2 if a corner in which
		//	either angle it reflects is acceptable) set the new center
		//	for correction
		tSphere.cPosition = tNewSCenter;
		return true;
	}
	else
	{
		return false;
	}
}
bool CCollisionManager::SphereToAABBIntersection(TSphere tSphere, TAABB tAABB)
{
	tAABB.cBoxMax.x += tSphere.fRadius;
	tAABB.cBoxMax.y += tSphere.fRadius;
	tAABB.cBoxMax.z += tSphere.fRadius;
	tAABB.cBoxMin.x -= tSphere.fRadius;
	tAABB.cBoxMin.y -= tSphere.fRadius;
	tAABB.cBoxMin.z -= tSphere.fRadius;

	if(tSphere.cPosition.x > tAABB.cBoxMin.x && tSphere.cPosition.x < tAABB.cBoxMax.x &&
		tSphere.cPosition.y > tAABB.cBoxMin.y && tSphere.cPosition.y < tAABB.cBoxMax.y &&
		tSphere.cPosition.z > tAABB.cBoxMin.z && tSphere.cPosition.z < tAABB.cBoxMax.z)
	{
		return true;
	}
	else
		return false;

	/*
	//	simply defines and stores the eight points of the AABB into an OBB struct
	TOBB tOBB;
	tOBB.cFrontTopLeft = D3DXVECTOR3(tAABB.cBoxMin.x, tAABB.cBoxMax.y, tAABB.cBoxMin.z);	
	tOBB.cFrontTopRight = D3DXVECTOR3(tAABB.cBoxMax.x, tAABB.cBoxMax.y, tAABB.cBoxMin.z);
	tOBB.cFrontBottomLeft = D3DXVECTOR3(tAABB.cBoxMin);
	tOBB.cFrontBottomRight = D3DXVECTOR3(tAABB.cBoxMax.x, tAABB.cBoxMin.y, tAABB.cBoxMin.z);

	tOBB.cBackTopLeft = D3DXVECTOR3(tAABB.cBoxMin.x, tAABB.cBoxMax.y, tAABB.cBoxMax.z);
	tOBB.cBackTopRight = D3DXVECTOR3(tAABB.cBoxMax);
	tOBB.cBackBottomLeft = D3DXVECTOR3(tAABB.cBoxMin.x, tAABB.cBoxMin.y, tAABB.cBoxMax.z);
	tOBB.CBackBottomRight = D3DXVECTOR3(tAABB.cBoxMax.x, tAABB.cBoxMin.y, tAABB.cBoxMax.z);
	//	now it's just going through the same functionality as "SphereToOBB. . ."
	return SphereToOBBIntersection(tSphere, tOBB, tSphere.cPosition);
	*/
}


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

bool CCollisionManager::LineToPlaneIntersection(TLine tLine, TPlane tPlane, D3DXVECTOR3& tColPoint)
{
	D3DXVECTOR3 vU = tLine.cLineEnd - tLine.cLineStart;
	D3DXVECTOR3 vW = tLine.cLineStart - tPlane.cPlanePoint;
	float fD = D3DXVec3Dot(&tPlane.cPlaneNormal, &vU);
	float fN = D3DXVec3Dot(&tPlane.cPlaneNormal, &vW);

	if(fabs(fD) < 0.0000001f)
	{
		if(fN == 0)
			return true;		//line is inside the plane
		else
			return false;		//line is parallel with the plane(no collision)
	}
	float fND = fN/fD;
	if(fND < 0 || fND > 1)
		return false;

	tColPoint = tLine.cLineStart + fND*vU;
	return true;
}

bool CCollisionManager::LineToOBBIntersection(TLine tLine, TOBB tOBB, D3DXVECTOR3& tColPoint)
{
	return false;
}
bool CCollisionManager::LineToAABBIntersection(TLine tLine, TOBB tAABB, D3DXVECTOR3& tColPoint)
{
	return false;
}


bool CCollisionManager::OBBToOBBIntersection(TOBB tOBB1, TOBB tOBB2, D3DXVECTOR3& tColPoint)
{
	return false;
}

bool CCollisionManager::OBBToPlaneIntersection(TOBB tOBB, TPlane tPlane)
{
	return false;
}

bool CCollisionManager::PlaneToPlaneIntersection(TPlane tPlane1, TPlane tPlane2)
{
	return false;
}
bool CCollisionManager::AABBTOAABBIntersection(TAABB tAABB1, TAABB tAABB2)
{
	return (tAABB1.cBoxMin.x > tAABB2.cBoxMax.x || tAABB1.cBoxMax.x < tAABB2.cBoxMin.x ||
		tAABB1.cBoxMin.y > tAABB2.cBoxMax.y || tAABB1.cBoxMax.y < tAABB2.cBoxMin.y ||
		tAABB1.cBoxMin.z > tAABB2.cBoxMax.z || tAABB1.cBoxMax.z < tAABB2.cBoxMin.z);
}
void CCollisionManager::CheckRam(CCollideable* obj1, CCollideable* obj2, D3DXVECTOR3 tvel1, D3DXVECTOR3 tvel2)
{
	D3DXVECTOR3 t1to2 = obj2->GetSphere().cPosition - obj1->GetSphere().cPosition;
	D3DXVECTOR3 t2to1 = obj1->GetSphere().cPosition - obj2->GetSphere().cPosition;
	if(D3DXVec3Dot(&t1to2, &tvel1) > 0.707f)
	{
		CRamEvent* pcREvent = MMNEWEVENT(CRamEvent) \
			CRamEvent(CIDGen::GetInstance()->GetID("CRamEvent"),\
			obj1, obj1->GetParent(), obj2->GetParent());
		m_pEventManager->GetInstance()->PostEvent(pcREvent, PRIORITY_NORMAL);
	}
	if(D3DXVec3Dot(&t2to1, &tvel2) > 0.707f)
	{
		CRamEvent* pcREvent = MMNEWEVENT(CRamEvent) \
			CRamEvent(CIDGen::GetInstance()->GetID("CRamEvent"),\
			obj2, obj1->GetParent(), obj1->GetParent());
		m_pEventManager->GetInstance()->PostEvent(pcREvent, PRIORITY_NORMAL);
	}
}

void CCollisionManager::SearchForCollision(void)
{
	if(m_cNonStaticObjects.size() < 2)
		return;
	std::map<unsigned int, CCollideable*>::iterator obj1iter; 
	std::map<unsigned int, CCollideable*>::iterator obj2iter; 


	for(obj1iter = m_cNonStaticObjects.begin(); 
		obj1iter != m_cNonStaticObjects.end(); ++obj1iter)
	{
		for(obj2iter = obj1iter; 
			obj2iter != m_cNonStaticObjects.end(); ++obj2iter)
		{
			if((*obj1iter) != (*obj2iter) && !(*obj1iter).second->GetWasChecked())
			{
				TestCollision((*obj1iter).second, (*obj2iter).second);
			}
		}
		(*obj1iter).second->SetWasChecked(true);
	}
	SetAllNotChecked();
}

bool CCollisionManager::TestCollision(CCollideable* obj1, CCollideable* obj2)
{
	bool bCollision = false;
	TSphere so1, so2;

	switch(obj1->GetBVType())
	{
	case BSPHERE:
		{
			so1.cPosition = D3DXVECTOR3(obj1->GetParent()->GetTransform()->GetWorldMatrix()._41,
				obj1->GetParent()->GetTransform()->GetWorldMatrix()._42,
				obj1->GetParent()->GetTransform()->GetWorldMatrix()._43); so1.fRadius = obj1->GetSphere().fRadius;
			obj1->SetSphere(so1);
			switch(obj2->GetBVType())
			{
			case BSPHERE:
				so2.cPosition = D3DXVECTOR3(obj2->GetParent()->GetTransform()->GetWorldMatrix()._41,
					obj2->GetParent()->GetTransform()->GetWorldMatrix()._42,
					obj2->GetParent()->GetTransform()->GetWorldMatrix()._43); so2.fRadius = obj2->GetSphere().fRadius;
				obj1->SetSphere(so1);
				obj2->SetSphere(so2);
				bCollision = SphereToSphereIntersection(obj1->GetSphere(), obj2->GetSphere());
				if(bCollision)
				{
					CollisionResponse(obj1, obj2);
				}
				return bCollision;
			case BAABB:
				bCollision = SphereToAABBIntersection(so1, obj2->GetAABB());
				if(bCollision)
				{
					CollisionResponse(obj1, obj2);
				}
				return bCollision;
			case BPLANE:
				bCollision = SphereToPlaneIntersection(so1, obj2->GetPlane(), obj1->GetSphere().cPosition);
				return bCollision;
			default:
				return bCollision;
				//////////////////////////////////////////////////////////////////////////
			}//first nested switch ends here
	default:
		return bCollision;


		//////////////////////////////////////////////////////////////////////////
		}//first switch statement ends here

	}
}

void CCollisionManager::CollisionResponse(CCollideable* obj1, CCollideable* obj2)
{
	//	printf( "%s has collided with %s \n while at position %f %f %f !\n\n",
	//		obj1->GetParent()->GetID(), obj2->GetParent()->GetID(),
	//		obj1->GetSphere().cPosition.x, obj1->GetSphere().cPosition.y,
	//		obj1->GetSphere().cPosition.z);

	if(0 == _stricmp((char*)obj1->GetParent()->GetID() , "Player") ||
		0 == _stricmp((char*)obj1->GetParent()->GetID() , "Agent1") ||
		0 == _stricmp((char*)obj1->GetParent()->GetID() , "Agent2") ||
		0 == _stricmp((char*)obj1->GetParent()->GetID() , "Agent3") )
	{
		//		char result[32];
		//		strcpy(result, (char*)obj1->GetParent()->GetID());
		if( _stricmp((char*)obj2->GetParent()->GetID(), "GoalItem 0")==0 ||
			_stricmp((char*)obj2->GetParent()->GetID(), "GoalItem 1")==0 ||
			_stricmp((char*)obj2->GetParent()->GetID(), "GoalItem 2")==0 ||
			_stricmp((char*)obj2->GetParent()->GetID(), "GoalItem 3")==0 ||
			_stricmp((char*)obj2->GetParent()->GetID(), "GoalItem 4")==0 ||
			_stricmp((char*)obj2->GetParent()->GetID(), "GoalItem 5")==0 ||
			_stricmp((char*)obj2->GetParent()->GetID(), "GoalItem 6")==0 ||
			_stricmp((char*)obj2->GetParent()->GetID(), "GoalItem 7")==0 )
		{
			CGoalItemCollectedEvent* pcGIC = MMNEWEVENT(CGoalItemCollectedEvent)\
				CGoalItemCollectedEvent(CIDGen::GetInstance()->\
				GetID("GoalItemCollected"), obj1, obj2->GetParent(), obj1->GetParent());
			m_pEventManager->GetInstance()->PostEvent(pcGIC, PRIORITY_NORMAL);
		}
		else if(0 == _stricmp((char*)obj2->GetParent()->GetID() , "Player") ||
			0 == _stricmp((char*)obj2->GetParent()->GetID() , "Agent1") ||
			0 == _stricmp((char*)obj2->GetParent()->GetID() , "Agent2") ||
			0 == _stricmp((char*)obj2->GetParent()->GetID() , "Agent3") )
		{
			D3DXVECTOR3 tvel1 = obj1->GetParent()->GetTransform()->GetLastWorldPosition() - obj1->GetParent()->GetTransform()->GetWorldPosition();
			D3DXVECTOR3 tvel2 = obj2->GetParent()->GetTransform()->GetLastWorldPosition() - obj2->GetParent()->GetTransform()->GetWorldPosition();
			D3DXVECTOR3 trans1, trans2;
			PlayerToPlayerIntersection(obj1->GetSphere(), obj2->GetSphere(), tvel1, tvel2, 
				trans1, trans2);
			obj1->GetParent()->GetTransform()->TranslateFrame(trans2);
			obj2->GetParent()->GetTransform()->TranslateFrame(trans1);
			CheckRam(obj1, obj2, tvel1, tvel2);
		}

	}
 	else if( _stricmp((char*)obj1->GetParent()->GetID(), "GoalItem 0")==0 ||
		_stricmp((char*)obj1->GetParent()->GetID(), "GoalItem 1")==0 ||
		_stricmp((char*)obj1->GetParent()->GetID(), "GoalItem 2")==0 ||
		_stricmp((char*)obj1->GetParent()->GetID(), "GoalItem 3")==0 ||
		_stricmp((char*)obj1->GetParent()->GetID(), "GoalItem 4")==0 ||
		_stricmp((char*)obj1->GetParent()->GetID(), "GoalItem 5")==0 ||
		_stricmp((char*)obj1->GetParent()->GetID(), "GoalItem 6")==0 ||
		_stricmp((char*)obj1->GetParent()->GetID(), "GoalItem 7")==0 )
	{
		if(0 == _stricmp((char*)obj2->GetParent()->GetID() , "Player") ||
			0 == _stricmp((char*)obj2->GetParent()->GetID() , "Agent1") ||
			0 == _stricmp((char*)obj2->GetParent()->GetID() , "Agent2") ||
			0 == _stricmp((char*)obj2->GetParent()->GetID() , "Agent3") )
		{
		CGoalItemCollectedEvent* pcGIC = MMNEWEVENT(CGoalItemCollectedEvent)\
			CGoalItemCollectedEvent(CIDGen::GetInstance()->\
			GetID("GoalItemCollected"), obj2, obj1->GetParent(), obj2->GetParent());
		m_pEventManager->GetInstance()->PostEvent(pcGIC, PRIORITY_NORMAL);
		}
	}
}

void CCollisionManager::InitBVH()
{
	//do some file io stuff maybe
	GenerateBVH(m_pRoot, 0);
}
void CCollisionManager::GenerateBVH(CBVHNode* pCurrNode, int ndepth)
{
	//set up some new nodes
	CBVHNode *rhs, *lhs;
	rhs = MMNEW(CBVHNode)CBVHNode();
	rhs->SetParent(pCurrNode);
	rhs->SetSplitType(-pCurrNode->GetSplitType());
	lhs = MMNEW(CBVHNode)CBVHNode();
	lhs->SetParent(pCurrNode);
	lhs->SetSplitType(-pCurrNode->GetSplitType());

	pCurrNode->SplitBV(pCurrNode, lhs, rhs);

	if(ndepth >= BVHDEPTH_LIMIT)
	{
		return;
	}
	GenerateBVH(lhs, ++ndepth);
	GenerateBVH(rhs, ++ndepth);
	pCurrNode->SetLHS(lhs);
	pCurrNode->SetRHS(rhs);
	
}
void CCollisionManager::LoadBVH(char* szfilename)
{

}
void CCollisionManager::SaveBVH(char* szfilename)
{

}
void CCollisionManager::Traverse()
{

}
//BVHNode functions
void CBVHNode::SplitBV(CBVHNode* pParent, CBVHNode* prhs, CBVHNode* plhs)
{
	TAABB tRBounds, tLBounds;
	switch(-pParent->GetSplitType())
	{
	case XSPLIT:
		{
			tLBounds.cBoxMin = pParent->GetBounds().cBoxMin;
			tLBounds.cBoxMin.y = pParent->GetBounds().cBoxMin.y;
			tLBounds.cBoxMax = D3DXVECTOR3(pParent->GetBounds().cBoxMax.x,
											pParent->GetBounds().cBoxMax.y,
											pParent->GetBounds().cBoxMax.z/2);
			tRBounds.cBoxMin = tLBounds.cBoxMax;
			tRBounds.cBoxMax = pParent->GetBounds().cBoxMax;			
		}
		break;
	case ZSPLIT:
		{
			tLBounds.cBoxMin = pParent->GetBounds().cBoxMin;
			tLBounds.cBoxMax = D3DXVECTOR3(pParent->GetBounds().cBoxMax.x/2,
											pParent->GetBounds().cBoxMax.y,
											pParent->GetBounds().cBoxMax.z);
			tRBounds.cBoxMin = tLBounds.cBoxMin;
			tRBounds.cBoxMax = plhs->GetBounds().cBoxMax;
		}
		break;
	}
	plhs->SetBounds(tLBounds);
	prhs->SetBounds(tRBounds);
}

