/*******************************************************************************
* Filename:			CCollisionManager.h
* Mod. Date:		03/27/2011
* Mod. Initials:	RN
* Author:			Raymond W. Nieves
* Purpose:			Encapsulates collision detection and reaction of all 
*					collideable components
*******************************************************************************/
#ifndef _CCOLLISIONMANAGER_H_
#define _CCOLLISIONMANAGER_H_

#include <map>
#include <list>
using std::map;
using std::list;

#include "../../Components/Collision/CCollideable.h"
#include "..\..\CFrame.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class IEvent;
class CEventManager;
class CObject;
/*
#ifndef HalfSpaceTest
#define HalfSpaceTest(plane, testPoint) (dot_product( (plane).m_Normal, (testPoint) ) - (plane).m_Offset)
#endif
*/

#ifndef SameSign
#define SameSign(a,b) ( ((*(unsigned int*)&(a)) & 0x80000000) == ((*(unsigned int*)&(b)) & 0x80000000) )
#endif 

//static bool FloatEquals(float fVal1, float fVal2, float fEpsilon)
//{
//	return ( fabs( fVal1 - fVal2 ) < fEpsilon );
//}

typedef map<unsigned int, CCollideable*, less<unsigned int>, CAllocator<
pair<unsigned int, CCollideable*>>> ColMap;

/*******************************************************************************
//  Collision Manager - handles all collideable components collisions/reactions
*******************************************************************************/
class CCollisionManager
{
private:
	ColMap m_cStaticObjects;
	ColMap m_cNonStaticObjects;
	ColMap m_cSeparatedBoxes;
	D3DXVECTOR3 *m_pReflect;
	D3DXVECTOR3 *m_pColpt;
	float m_fElapsed;
	
	int		   m_nTriCount;
	TTriangle* m_pWallTris;
	
	CEventManager* m_pEventManager;

	//constructor
	CCollisionManager();
	CCollisionManager(const CCollisionManager&);
	CCollisionManager& operator=(const CCollisionManager&);

public:
	~CCollisionManager();
	static CCollisionManager* GetInstance();
	void Init();
	static void Shutdown(IEvent* pEvent, IComponent* pComponent);
	static void DestroyObject(IEvent*, IComponent*);
	/************************************************************************
	*	Update :		updates the BVH nodes of all objects that have moved
	*					since the last update
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	ColMap& GetStaticObjs() { return m_cStaticObjects; }

	static void Update(IEvent* pEvent, IComponent* pComponent);
	static bool FloatEquals(float fVal1, float fVal2, float fEpsilon)
	{
		return ( fabs( fVal1 - fVal2 ) < fEpsilon );
	}
	
	void SetTime(float fT)
	{
		m_fElapsed = fT;
	}
	float GetTime(void)
	{
		return m_fElapsed;
	}
	//Condenses aisles made up of multiple objects into one collision shape

	// accessors
	void AddSepBox(CCollideable* cStatic, unsigned int nObjID);
	void AddStatic(CCollideable* cStatic, unsigned int nObjID);
	void AddNonStatic(CCollideable* cCollide, unsigned int nObjID);
	// mutators
	void RemoveSepBox(CCollideable* cStatic, unsigned int nObjID);
	void RemoveStatic(CCollideable* cStatic, unsigned int nObjID);
	void RemoveNonStatic(CCollideable* cCollide, unsigned int nObjID);
	
	//component functions
	static int CreateCollideableComponent(lua_State* pLua);
	static CCollideable* CreateCollideableComponent(CObject* pParent,
		bool isStatic, bool isReactor, unsigned int objType);

	//tests for collision between two components(called in update)
	void SearchForCollision(float fDeltaTime);
	bool TestCollision(CCollideable* obj1, CCollideable* obj2);
	void CheckRam(CCollideable* obj1, CCollideable* obj2, D3DXVECTOR3 tvel1, D3DXVECTOR3 tvel2);
	void CondenseCollisionBoxes(void);
	void CreateRenderedComboBoxes(void);
	void CombineLinedBoxes(void)
	{
		//by our powers combined
		CondenseCollisionBoxes(); CondenseCollisionBoxes(); CondenseCollisionBoxes();
		//we get pretty sweet collision shapes
		CreateRenderedComboBoxes();
	}

	void SetAllNotChecked(void)
	{
		std::map<unsigned int, CCollideable*, less<unsigned int>, CAllocator
			<pair<unsigned int, CCollideable*>>>::iterator insiter; 
		for(insiter = m_cNonStaticObjects.begin(); 
			insiter != m_cNonStaticObjects.end(); ++insiter)
		{
			(*insiter).second->SetWasChecked(false);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Collision functions
	// these functions take in the shapes to test collision with
	// all intersection functions return if there was a collision
	// all reaction functions handle the collision reaction if there was one
	//////////////////////////////////////////////////////////////////////////


	bool SphereToSphereIntersection(CCollideable* obj1, CCollideable* obj2, 
		D3DXVECTOR3* vReflect, bool bReact);


	bool SphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2,
		D3DXVECTOR3* vReflect, bool bReact);

	bool AABBTOAABBIntersection(CCollideable* obj1, CCollideable* obj2);

	bool CapsuleToCapsuleIntersection(CCollideable* obj1, CCollideable* obj2,
		D3DXVECTOR3* tPosition, bool bReact);
	bool CapsuleToSphereIntersection(CCollideable* obj1, CCollideable* obj2,
		D3DXVECTOR3* tPosition, bool bReact);
	bool CapsuleToAABBIntersection(CCollideable* obj1, CCollideable* obj2,
		D3DXVECTOR3* tPosition, bool bReact);
	bool CapsuleToOBBIntersection(CCollideable* obj1, CCollideable* obj2,
		D3DXVECTOR3* tPosition, bool bReact);

	bool SphereToOBBIntersection(CCollideable* obj1, CCollideable* obj2,
		D3DXVECTOR3* tPosition, bool bReact);	//bReact is pretty much always going to be true
	bool OBBToOBBIntersection(CCollideable* obj1, CCollideable* obj2,
		D3DXVECTOR3* tPosition, bool bReact);
	bool OBBToAABBIntersection(CCollideable* obj1, CCollideable* obj2,
						   D3DXVECTOR3* tPosition, bool bReact);


	//frustum collisions

	//////////////////////////////////////////////////////////////////////////
	//Collision Line Functions
	// these functions are used mostly by AI entities for pathfinding
	//////////////////////////////////////////////////////////////////////////
	bool LineToSphereIntersection(TLine tLine, TSphere tSphere, D3DXVECTOR3& tColPoint);
	bool LineToAABBIntersection(TLine tLine, TAABB tAABB, D3DXVECTOR3& tColPoint);
	bool LineToTriangle(TLine tLine, TTriangle triangle, D3DXVECTOR3 & tColpoint);
	bool RayToLineIntersection(TRay tRay, TLine tSegment);
	bool PointInTriangle(TTriangle tTri, D3DXVECTOR3 tTestPt);
	bool LineEquals(TLine Line1, TLine Line2)
	{
		if(Line1.cLineStart == Line2.cLineStart &&
			Line1.cLineEnd == Line2.cLineEnd)
			return true;
		else
			return false;
	}

	bool RayToSphereIntersection(TRay ray, TSphere sphere, float & t, D3DXVECTOR3 &intersect);
	bool RayToTriangleIntersection(TTriangle tri, TRay ray, float &t);

	bool MovingSphereToTriangleIntersection(CCollideable* obj, TTriangle* tri,
		D3DXVECTOR3* vReflect, D3DXVECTOR3* tCol, float &t, bool bReact);
	///////////////////////////////////////////////////////
	//returns false and sets tClosestPt to the closest collision point
	//returns true if the line reaches the waypoint uninterrupted
	///////////////////////////////////////////////////////
	bool LineToWayPoint(TLine tLine, D3DXVECTOR3 &tClosestPt);

	///////////////////////////////////////////////////////
	//returns true if the line collides with anything the player would collide with
	//returns false if nothing is found along the line
	///////////////////////////////////////////////////////
	bool LineTestAvoidance(TLine tLine, float fRayDist, D3DXVECTOR3 &tSurfaceNorm);

	//////////////////////////////////////////////////////////////////////////
	//Collision Support Functions
	// these functions serve to assist the collision check functions
	// they will be only be called from within the collision manager
	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 GetAABBNormal(D3DXVECTOR3 colPt, TAABB colBox);
	D3DXVECTOR3 GetSphereNormal(D3DXVECTOR3 colPt, TSphere colSphere);
	D3DXVECTOR3 GetReflectedVector(D3DXVECTOR3 vDir, D3DXVECTOR3 vNorm);
	bool PointInCone(D3DXVECTOR3 tPlayerMove, D3DXVECTOR3 tPlayerPos, 
		D3DXVECTOR3 tTargetPos, float fCone);
	void CheckFrustDist(float fDist, CCollideable* pCheck, bool &collide, bool &behind, bool &intersect);
	D3DXVECTOR3 GetCloserPt(D3DXVECTOR3 tPt1, D3DXVECTOR3 tPt2, D3DXVECTOR3 tTestpt);
	std::vector<TTriangle> GetAABBTriangles(TAABB aabb);
	void ClosestPointToAABB(TAABB tBox, D3DXVECTOR3 tTestPt, D3DXVECTOR3& tClosest);
	void ClosestPointToOBB(TOBB tBox, D3DXVECTOR3 tTestPt, D3DXVECTOR3& tClosest);
	//	CObject* PickObject(D3DXVECTOR3 tMouseScreenCoord, 
	//		D3DXMATRIX mMVMatrix, D3DXMATRIX mProjMatrix, )
	int HalfSpaceTest(D3DXVECTOR3 tNorm, D3DXVECTOR3 tPlanePt, D3DXVECTOR3 tTest);
	D3DXVECTOR3 ClosestPtPointTriangle(D3DXVECTOR3 p, D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c);
	bool MovingSphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2, D3DXVECTOR3 tVelocity,
		D3DXVECTOR3* vReflect, bool bReact);

	D3DXVECTOR3 ClosestPointOnLine(TLine line, D3DXVECTOR3 testpt);

	int TestObjAgainstWall(CCollideable* obj1);
	// this version is for the camera only
	int TestObjAgainstWall(D3DXVECTOR3 &cameraFrame);
};


#endif