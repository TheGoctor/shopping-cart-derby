#include "CCollisionManager.h"
#include "CLevelManager.h"
#include "../Global Managers/Event Manager/CEventManager.h"
#include "../Global Managers/Object Manager/CObjectManager.h"
#include "..\..\..\Source\CObject.h"
#include "..\..\Components\Rendering\CRenderComponent.h"
#include "..\Global Managers\Rendering Managers\Renderer.h"
#include "..\Global Managers\Event Manager\EventStructs.h"
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

			TAABB tAABB;
			TSphere tsphere;
			TPlane tplane;
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
			szColObjID += pObject->GetID();
			CObject* pColObj = CObjectManager::GetInstance()->CreateObject(
				szColObjID, 0.0f, 0.0f, 0.0f, pObject->GetTransform());
			CRenderComponent* pRender = Renderer::GetInstance()->CreateRenderComp(
				pColObj, 23, 0, RF_INDEXED_VERT);
			pColObj->GetTransform()->ScaleFrame(tsphere.fRadius * 2.0f, 
				tsphere.fRadius * 2.0f, tsphere.fRadius * 2.0f);
			if(pCollideable->GetObjType() != OBJCAMERA)
				CLevelManager::GetInstance()->AddRenderCollision(pRender);

			lua_pop(pLua, 9);
			break;
		}
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
	case BPLANE:
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
		}

	}
	//temp stuff

	return 0;
}

void CCollisionManager::Shutdown(IEvent* pEvent, IComponent* pComponent)
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
}

void CCollisionManager::Update(IEvent* pEvent, IComponent* pComponent)
{

}

void CCollisionManager::AddStatic(CCollideable* cStatic, unsigned int nObjID)
{	
	//adds the static component to the list of statics
	pair<unsigned int, CCollideable*> pTest = 
		pair<unsigned int, CCollideable*>(nObjID, cStatic);
	m_cStaticObjects.insert(pTest); //make_pair(nObjID, cStatic));
}

void CCollisionManager::RemoveStatic(CCollideable* cStatic, unsigned int nObjID)
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

void CCollisionManager::RemoveNonStatic(CCollideable* cCollide, unsigned int nObjID)
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
bool CCollisionManager::SphereToSphereIntersection(CCollideable* obj1, CCollideable* obj2)
{
	TSphere s1, s2;
	s1.cPosition = obj1->GetSphere().cPosition;
	s1.fRadius = obj1->GetSphere().fRadius;
	s2.cPosition = obj2->GetSphere().cPosition;
	s2.fRadius = obj2->GetSphere().fRadius;

	float fDist = 0.0f;
	D3DXVECTOR3 v1to2 = s2.cPosition - s1.cPosition;
	fDist = D3DXVec3Length(&v1to2);
	float fRads = ((s1.fRadius+s2.fRadius)*(s1.fRadius+s2.fRadius));
	if(fDist <= s1.fRadius+s2.fRadius)
		return true;
	else
		return false;
}
bool CCollisionManager::SphereToSphereReaction(CCollideable* obj1, CCollideable* obj2)
{

	D3DXVECTOR3 tvel1 = obj1->GetParent()->GetTransform()->GetLastWorldPosition() - obj1->GetParent()->GetTransform()->GetWorldPosition();
	D3DXVECTOR3 tvel2 = obj2->GetParent()->GetTransform()->GetLastWorldPosition() - obj2->GetParent()->GetTransform()->GetWorldPosition();
	TSphere s1, s2;
	s1.cPosition = obj1->GetSphere().cPosition;
	s1.fRadius = obj1->GetSphere().fRadius;
	s2.cPosition = obj2->GetSphere().cPosition;
	s2.fRadius = obj2->GetSphere().fRadius;

	float fDist = 0.0f;
	D3DXVECTOR3 v1to2 = s2.cPosition-s1.cPosition;
	fDist = D3DXVec3Length(&v1to2);
	float fRadii = s1.fRadius+s2.fRadius;
	if(fDist <= fRadii)
	{
		float fPen = fRadii - fDist;
		D3DXVECTOR3 v2to1 = s1.cPosition-s2.cPosition;

		if(D3DXVec3Dot(&v1to2, &tvel1) > 0)
		{
			SendRamEvent("CRamEvent", obj1, obj1->GetParent(), obj2->GetParent());
		}
		if(D3DXVec3Dot(&v2to1, &tvel2) > 0)
		{
			SendRamEvent("CRamEvent", obj2, obj2->GetParent(), obj1->GetParent());
		}
		D3DXVec3Normalize(&v1to2, &v1to2);
		D3DXVec3Normalize(&v2to1, &v2to1);
		v1to2*=fPen;
		v2to1*=fPen;
		obj1->GetParent()->GetTransform()->TranslateFrame(v2to1);
		obj2->GetParent()->GetTransform()->TranslateFrame(v1to2);
		return true;
	}
	else
	{
		return false;
	}

}
bool CCollisionManager::SphereToPlaneIntersection(CCollideable* obj1, CCollideable* obj2)
{
	D3DXVECTOR3 tV = obj1->GetSphere().cPosition - obj2->GetPlane().cPlanePoint;
	D3DXVECTOR3 tVel = obj1->GetParent()->GetTransform()->GetLastWorldPosition() - obj1->GetParent()->GetTransform()->GetWorldPosition();
	float fDist = D3DXVec3Dot(&tV, &obj2->GetPlane().cPlaneNormal);
	D3DXVECTOR3 vReflect = (2*(D3DXVec3Dot(&tVel,&obj2->GetPlane().cPlaneNormal)))*(obj2->GetPlane().cPlaneNormal - tVel);
	if(fDist < obj1->GetSphere().fRadius)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CCollisionManager::SphereToPlaneReaction(CCollideable* obj1, CCollideable* obj2)
{
	D3DXVECTOR3 tV = obj1->GetSphere().cPosition - obj2->GetPlane().cPlanePoint;
	D3DXVECTOR3 tVel = obj1->GetParent()->GetTransform()->GetLastWorldPosition() - obj1->GetParent()->GetTransform()->GetWorldPosition();
	float fDist = D3DXVec3Dot(&tV, &obj2->GetPlane().cPlaneNormal);
	D3DXVECTOR3 vReflect = (2*(D3DXVec3Dot(&tVel,&obj2->GetPlane().cPlaneNormal)))*(obj2->GetPlane().cPlaneNormal - tVel);
	if(fDist < obj1->GetSphere().fRadius)
	{
		D3DXVECTOR3 vtrans = vReflect + (obj2->GetPlane().cPlaneNormal*(obj1->GetSphere().fRadius-fDist));
		obj1->GetParent()->GetTransform()->TranslateFrame(vtrans);
		return true;
	}
	else
	{
		return false;
	}
}

bool CCollisionManager::SphereToOBBIntersection(CCollideable* obj1, CCollideable* obj2)
{
	return false;
}
bool CCollisionManager::SphereToOBBReaction(CCollideable* obj1, CCollideable* obj2)
{
	return false;
}

bool CCollisionManager::SphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2)
{
	D3DXVECTOR3 vclosest;
	for(unsigned iv = 0; iv < 3; ++iv)
	{
		float fval = obj1->GetSphere().cPosition[iv];
		if(fval < obj2->GetAABB().cBoxMin[iv]) fval = obj2->GetAABB().cBoxMin[iv];
		if(fval > obj2->GetAABB().cBoxMax[iv]) fval = obj2->GetAABB().cBoxMax[iv];
		vclosest[iv] = fval;
	}
	D3DXVECTOR3 vfromAABB = obj1->GetSphere().cPosition - vclosest;
	float fDist = D3DXVec3Dot(&vfromAABB, &vfromAABB);
	if(fDist < obj1->GetSphere().fRadius*obj1->GetSphere().fRadius)
	{
		//		D3DXVec3Normalize(&vfromAABB, vfromAABB);
		//		vfromAABB*=(obj1.GetSphere().fRadius-fDist);
		//		obj1->GetParent()->GetTransform()->TranslateFrame(vfromAABB);
		return true;
	}
	else
	{
		return false;
	}
}
bool CCollisionManager::SphereToAABBReaction(CCollideable* obj1, CCollideable* obj2)
{	
	D3DXVECTOR3 vclosest;
	for(unsigned iv = 0; iv < 3; ++iv)
	{
		float fval = obj1->GetSphere().cPosition[iv];
		if(fval < obj2->GetAABB().cBoxMin[iv]) fval = obj2->GetAABB().cBoxMin[iv];
		if(fval > obj2->GetAABB().cBoxMax[iv]) fval = obj2->GetAABB().cBoxMax[iv];
		vclosest[iv] = fval;
	}
	D3DXVECTOR3 vfromAABB = obj1->GetSphere().cPosition - vclosest;
	float fDist = D3DXVec3Dot(&vfromAABB, &vfromAABB);
	if(fDist < obj1->GetSphere().fRadius*obj1->GetSphere().fRadius)
	{
		D3DXVec3Normalize(&vfromAABB, &vfromAABB);
		vfromAABB*=(obj1->GetSphere().fRadius-fDist);
		obj1->GetParent()->GetTransform()->TranslateFrame(vfromAABB);
		return true;
	}
	else
	{
		return false;
	}
}

bool CCollisionManager::OBBToOBBIntersection(CCollideable* obj1, CCollideable* obj2)
{
	return false;
}
bool CCollisionManager::OBBToOBBReaction(CCollideable* obj1, CCollideable* obj2)
{
	return false;
}

bool CCollisionManager::OBBToPlaneIntersection(CCollideable* obj1, CCollideable* obj2)
{
	return false;
}
bool CCollisionManager::OBBToPlaneReaction(CCollideable* obj1, CCollideable* obj2)
{
	return false;
}

bool CCollisionManager::PlaneToPlaneIntersection(CCollideable* obj1, CCollideable* obj2)
{
	return false;
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
bool CCollisionManager::AABBToAABBReaction(CCollideable* obj1, CCollideable* obj2)
{
	return false;
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
bool CCollisionManager::SphereToFrustum(CCollideable* obj1, CCollideable* obj2)
{
	float fDist = 0.0f;
	bool bCollide, bBehind, bIntersect;
	bCollide = bBehind = bIntersect = false;

	fDist = D3DXVec3Dot(&obj2->GetSphere().cPosition,
		&obj1->GetFrustum().tBottom.cPlaneNormal)- obj1->GetFrustum().tBottom.fOffset;
	CheckFrustDist(fDist, obj2, bCollide, bBehind, bIntersect);
	fDist = D3DXVec3Dot(&obj2->GetSphere().cPosition,
		&obj1->GetFrustum().tTop.cPlaneNormal)- obj1->GetFrustum().tTop.fOffset;
	CheckFrustDist(fDist, obj2, bCollide, bBehind, bIntersect);
	fDist = D3DXVec3Dot(&obj2->GetSphere().cPosition,
		&obj1->GetFrustum().tLeft.cPlaneNormal)- obj1->GetFrustum().tLeft.fOffset;
	CheckFrustDist(fDist, obj2, bCollide, bBehind, bIntersect);
	fDist = D3DXVec3Dot(&obj2->GetSphere().cPosition,
		&obj1->GetFrustum().tRight.cPlaneNormal)- obj1->GetFrustum().tRight.fOffset;
	CheckFrustDist(fDist, obj2, bCollide, bBehind, bIntersect);
	fDist = D3DXVec3Dot(&obj2->GetSphere().cPosition,
		&obj1->GetFrustum().tNear.cPlaneNormal)- obj1->GetFrustum().tNear.fOffset;
	CheckFrustDist(fDist, obj2, bCollide, bBehind, bIntersect);
	fDist = D3DXVec3Dot(&obj2->GetSphere().cPosition,
		&obj1->GetFrustum().tFar.cPlaneNormal)- obj1->GetFrustum().tFar.fOffset;
	CheckFrustDist(fDist, obj2, bCollide, bBehind, bIntersect);

	if(bBehind || bIntersect)
	{
		return false;
	}
	return true;
	return false;
}

bool CCollisionManager::AABBToFrustum(CCollideable* obj1, CCollideable* obj2)
{
	return false;
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
bool CCollisionManager::LineToPlaneIntersection(TLine tLine, TPlane tPlane, D3DXVECTOR3& tColPoint)
{
	return false;
}
bool CCollisionManager::LineToOBBIntersection(TLine tLine, TOBB tOBB, D3DXVECTOR3& tColPoint)
{
	return false;
}
bool CCollisionManager::LineToAABBIntersection(TLine tLine, TAABB tAABB, D3DXVECTOR3& tColPoint)
{
	float tmin = 0.0f;
	float tmax = FLT_MAX;
	for(unsigned iax = 0; iax < 3; ++iax)
	{
		if(abs(tLine.cLineEnd[iax]) < 0.00001)
		{
			if(tLine.cLineStart[iax] < tAABB.cBoxMin[iax] || tLine.cLineStart[iax] > tAABB.cBoxMax[iax])
				return false;
		}
		else
		{
			float ood = 1.0f/tLine.cLineEnd[iax];
			float t1 = (tAABB.cBoxMin[iax] - tLine.cLineStart[iax])*ood;
			float t2 = (tAABB.cBoxMax[iax] - tLine.cLineStart[iax])*ood;

			if (t1 > t2)
			{
				float ftemp = t1;
				t1 = t2;
				t2 = ftemp;
			}
			tmin = max(tmin, t1);
			tmax = min(tmax, t2);
			if(tmin > tmax)
			{
				return false;
			}		
		}

	}
	tColPoint = tLine.cLineStart + tLine.cLineEnd *tmin;
	return true;
}
bool CCollisionManager::LineToWayPoint(TLine tLine, D3DXVECTOR3 &tClosestPt)
{
	//returns false and sets tClosestPt to the closest collision point if there was a collision along the line
	//returns true if the line reaches the waypoint uninterrupted
	bool bInterrupt = false;
	bool bReachedWP = true;
	D3DXVECTOR3 tClosestTemp = tLine.cLineEnd;	//init as the end of the line(aka the waypoint, if there is collision, it will change)
	tClosestPt = tLine.cLineEnd;							//this return vector is also set to the end to make sure it's updated at (***)
	std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
		<pair<unsigned int, CCollideable*>>>::iterator pSIter;				//iterator for the static objects
	
	for(pSIter = m_cStaticObjects.begin(); pSIter != m_cStaticObjects.end(); ++pSIter)
	{	//loop through the static objects(non static objects won't have an effect on the waypoint searches)
		switch( (*pSIter).second->GetBVType() )
		{	//check the type of the BV and check for the appropriate collision
		case BSPHERE:
			{	
				bInterrupt = LineToSphereIntersection( tLine, (*pSIter).second->GetSphere(), tClosestTemp );
			}
			break;
		case BAABB:
			{
				bInterrupt = LineToAABBIntersection( tLine, (*pSIter).second->GetAABB(), tClosestTemp );
			}
			break;
		default:
			break;	//object has no BV, just continue to the next
		}
		if(bInterrupt)
		{//if there was collision along the line
			//check if tClosestTemp is closer to the start of the line than the last one
			// if ever bInterrupt is true, then the line did not reach the waypoint uninterrupted
			bReachedWP = false;
			//set the closest point to the closer of the two points: the current closest and the previous one
			tClosestPt = GetCloserPt(tClosestPt, tClosestTemp, tLine.cLineStart); //(***)!
		}
		bInterrupt = false;
	}
	return bReachedWP;
}
//////////////////////////////////////////////////////////////////////////
//Collision Support Functions
// these functions serve to assist the collision check functions
// they will be only be called from within the collision manager
//////////////////////////////////////////////////////////////////////////
bool CCollisionManager::PointInCone(D3DXVECTOR3 tPlayerMove, D3DXVECTOR3 tPlayerPos, 
									D3DXVECTOR3 tTargetPos, float fCone)
{
	return false;
}
D3DXVECTOR3 CCollisionManager::GetCloserPt(D3DXVECTOR3 tPt1, D3DXVECTOR3 tPt2, D3DXVECTOR3 tTestpt)
{
	float fDist1, fDist2;		//distances from tTestpt to tPt1 and tPt2 respectively
	fDist2 = fDist2 = 0.0f;
	//make vectors from each point to the test point and the one with
	//		the smaller distance is returned
	fDist1 = D3DXVec3Length( &D3DXVECTOR3( tTestpt - tPt1 ) );
	fDist2 = D3DXVec3Length( &D3DXVECTOR3( tTestpt - tPt2 ) );
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


void CCollisionManager::SearchForCollision(void)
{
	//////////////////////////////////////////////////////////////////////////
//	TLine liner;
//	liner.cLineEnd = D3DXVECTOR3(0, 50, 0);
//	liner.cLineStart = D3DXVECTOR3(0, 10, 0);
//	D3DXVECTOR3 poop;
//	LineToWayPoint(liner, poop);
	//NOTE: the above code was a test to see if the functionality worked, it did indeed, 
	//			leaving it here in the case that further testing is needed
	//////////////////////////////////////////////////////////////////////////
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
	}
	for(pSobj1 = m_cNonStaticObjects.begin(); pSobj1 != m_cNonStaticObjects.end(); ++pSobj1)
	{
		for(pSobj2 = m_cNonStaticObjects.begin(); pSobj2 != m_cNonStaticObjects.end(); ++pSobj2)
		{
			if( (*pSobj1) != (*pSobj2) &&
				!(*pSobj1).second->GetWasChecked() && !(*pSobj2).second->GetWasChecked() )
			{
				TestCollision((*pSobj1).second, (*pSobj2).second);
			}
		}
		(*pSobj1).second->SetWasChecked(true);
	}
	SetAllNotChecked();
}

bool CCollisionManager::TestCollision(CCollideable* obj1, CCollideable* obj2)
{
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
	switch(obj1->GetObjType())
	{
	case OBJPLAYER:
		{
			switch(obj2->GetObjType())
			{
			case OBJPLAYER:
				{
					if(SphereToSphereReaction(obj1, obj2))
					{
						SendObjectEvent("CartCollision", obj1, obj2->GetParent());
						return true;
					}
					//				return SphereToSphereReaction(obj1, obj2);
				}
				break;
			case OBJGITEM:
				{
					if(SphereToSphereIntersection(obj1, obj2))
					{
						SendGoalItemCollectedEvent("GoalItemCollected", obj1, 
							obj2->GetParent(), obj1->GetParent());
						return true;
					}
				}
				break;
			case OBJPITEM:
				{
					if(SphereToSphereIntersection(obj1, obj2))
					{
						SendGoalItemCollectedEvent("GoalItemCollected", obj1,
							obj2->GetParent(), obj1->GetParent());					//check acc.
						return true;
					}
				}
				break;
			case OBJWALL:
				{
					if(SphereToPlaneReaction(obj1, obj2))
					{
						SendObjectEvent("EnvironmentHit", obj1, obj2->GetParent());
						return true;
					}
					//					SphereToPlaneReaction(obj1, obj2);
				}
				break;
			case OBJSHELF:
				{
					if(SphereToAABBReaction(obj1, obj2))
					{
						SendObjectEvent("ShelfHit", obj1, obj2->GetParent());
						return true;
					}
					//					SphereToAABBReaction(obj1, obj2);
				}
				break;
			case OBJENDCAP:
				{
					if(SphereToAABBReaction(obj1, obj2))
					{
						SendObjectEvent("EndcapHit", obj1, obj2->GetParent());
						return true;
					}
					//					SphereToAABBReaction(obj1, obj2);
				}
				break;
			case OBJCHECKOUT:
				{
					if(SphereToAABBIntersection(obj1, obj2))
					{
						//						obj1->GetParent()->GetTransform()->
						//							TranslateFrame(D3DXVECTOR3(200, 0, 200));
						SendObjectEvent("CheckoutCollision", obj1, obj1->GetParent());
					}
				}
				break;
			default:
				return false;
			}
		}
		break;
	case OBJPITEM:
		{
			//			switch(obj2->GetObjType())
			//			{
			//			case:
			//			default:
			//				return false;
			//			}
		}
		break;
	case OBJCAMERA:
		{
			switch(obj2->GetObjType())
			{
			case OBJWALL:
				{
					if(SphereToPlaneReaction(obj1, obj2))
					{
						return true;
					}
					//					SphereToPlaneReaction(obj1, obj2);
				}
				break;
			case OBJSHELF:
				{
					if(SphereToAABBReaction(obj1, obj2))
					{
						return true;
					}
					//					SphereToAABBReaction(obj1, obj2);
				}
				break;
			case OBJENDCAP:
				{
					if(SphereToAABBReaction(obj1, obj2))
					{
						return true;
					}
					//					SphereToAABBReaction(obj1, obj2);
				}
				break;
			default:
				return false;
			}
		}
		break;
		/*	case OBJFRUSTUM:
		{
		//			if(obj2->GetObjType() == OBJPLAYER)
		//				SendRenderEvent("AddToSet", NULL, obj2->GetParent());	//test: seeing if event made player render
		switch(obj2->GetBVType())
		{
		case BSPHERE:
		{
		if(SphereToFrustum(obj2, obj1))
		{
		SendRenderEvent("AddToSet", (IComponent*)GetInstance(), obj2->GetParent());
		}
		}
		break;
		case BAABB:
		{
		if(AABBToFrustum(obj2, obj1))
		{
		SendRenderEvent("AddToSet", (IComponent*)GetInstance(), obj2->GetParent());
		}
		}
		break;
		default:
		return false;
		}
		}
		break;*/
	default:
		return false;	//no obj type probably means no collision volume, so. . . no good
	}
	return false;
}


void CCollisionManager::InitBVH()
{
	//do some file io stuff maybe
	GenerateBVH(m_pRoot, 0);
}

void CCollisionManager::GenerateBVH(CBVHNode* pCurrNode, int ndepth)
{
	////set up some new nodes
	//CBVHNode *rhs, *lhs;
	//rhs = MMNEW(CBVHNode);
	//rhs->SetParent(pCurrNode);
	//rhs->SetSplitType(-pCurrNode->GetSplitType());
	//lhs = MMNEW(CBVHNode);
	//lhs->SetParent(pCurrNode);
	//lhs->SetSplitType(-pCurrNode->GetSplitType());

	//pCurrNode->SplitBV(pCurrNode, lhs, rhs);

	//if(ndepth >= BVHDEPTH_LIMIT)
	//{
	//	return;
	//}
	//GenerateBVH(lhs, ++ndepth);
	//GenerateBVH(rhs, ++ndepth);
	//pCurrNode->SetLHS(lhs);
	//pCurrNode->SetRHS(rhs);

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
		}
		break;
	case ZSPLIT:
		{
		}
		break;
	default:
		break;
	}
	plhs->SetBounds(tLBounds);
	prhs->SetBounds(tRBounds);
}