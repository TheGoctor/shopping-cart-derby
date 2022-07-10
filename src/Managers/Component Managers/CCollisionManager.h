/*******************************************************************************
* Filename:			CCollisionManager.h
* Mod. Date:		07/25/2011
* Mod. Initials:	RN
* Author:			Raymond W. Nieves
* Purpose:			Encapsulates collision detection and reaction of all 
*						collideable components
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

//returns whether or not two values have the same sign
#ifndef SameSign
#define SameSign(a,b) ( ((*(unsigned int*)&(a)) & 0x80000000) == ((*(unsigned int*)&(b)) & 0x80000000) )
#endif 

//typedef for maps of collideables
typedef map<unsigned int, CCollideable*, less<unsigned int>, CAllocator<
pair<unsigned int, CCollideable*>>> ColMap;

/*******************************************************************************
//  Collision Manager - handles all collideable components collisions/reactions
*******************************************************************************/
class CCollisionManager
{
private:
	//////////////////////////////////////////////////////////////////////////
	//maps of collideables for static, non-static and the boxes made from the 
	//		level that can be combined
	//////////////////////////////////////////////////////////////////////////
	ColMap m_cStaticObjects;
	ColMap m_cNonStaticObjects;
	ColMap m_cSeparatedBoxes;
	//////////////////////////////////////////////////////////////////////////
	//reflect vector and collision point, for storing the value after it gets set in 
	//		one of the collision functions
	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 *m_pReflect;
	D3DXVECTOR3 *m_pColpt;
	//////////////////////////////////////////////////////////////////////////
	//delta time
	//////////////////////////////////////////////////////////////////////////
	float m_fElapsed;
	//////////////////////////////////////////////////////////////////////////
	//effect objects for item to item collision
	//////////////////////////////////////////////////////////////////////////
	CObject* m_pTurkeyDestroy;
	CObject* m_pPieDestroy;
	CObject* m_pBananaDestroy;
	//////////////////////////////////////////////////////////////////////////
	//wall collision volume indices and triangles
	//////////////////////////////////////////////////////////////////////////
	int		   m_nTriCount;
	TTriangle* m_pWallTris;
	
	CEventManager* m_pEventManager;

	//////////////////////////////////////////////////////////////////////////
	//constructor
	//////////////////////////////////////////////////////////////////////////
	CCollisionManager();
	//////////////////////////////////////////////////////////////////////////
	//copy constructor and assignment operator
	//////////////////////////////////////////////////////////////////////////
	CCollisionManager(const CCollisionManager&);
	CCollisionManager& operator=(const CCollisionManager&);

public:
	//////////////////////////////////////////////////////////////////////////
	//destructor
	//////////////////////////////////////////////////////////////////////////
	~CCollisionManager();
	//////////////////////////////////////////////////////////////////////////
	//gets the instance of the collision manager
	//////////////////////////////////////////////////////////////////////////
	static CCollisionManager* GetInstance();
	//////////////////////////////////////////////////////////////////////////
	//Init - initializess the collision manager
	//////////////////////////////////////////////////////////////////////////
	void Init();
	//////////////////////////////////////////////////////////////////////////
	//shutdown - clears all the memory used by the collision manager and shuts it down
	//////////////////////////////////////////////////////////////////////////
	static void Shutdown(IEvent* pEvent, IComponent* pComponent);
	//////////////////////////////////////////////////////////////////////////
	//returns the map of static object collision volumes
	//////////////////////////////////////////////////////////////////////////
	ColMap& GetStaticObjs() { return m_cStaticObjects; }
	//////////////////////////////////////////////////////////////////////////
	//update
	//////////////////////////////////////////////////////////////////////////
	static void Update(IEvent* pEvent, IComponent* pComponent);
	////////////////////////////////////////////////////////////////////////////////
	// Function: “TestIndividualCollisionSphere”
	//
	// Return:  bool	- true = collision happened(reaction happens within the function), false = no collision
	//
	// Parameters: CCollideable* pCol	- the component to check
	//
	// Purpose:  this function checks for collision with a single object(type BSPHERE)
	//					against all the static components
	////////////////////////////////////////////////////////////////////////////////
	bool TestIndividualCollisionsSphere(CCollideable* pCol);
	////////////////////////////////////////////////////////////////////////////////
	// Function: “FloatEquals”
	//
	// Return:  bool - true = values within fEpsilon of each other, false = they're not
	//
	// Parameters: float fVal1, fVal2 - the two values to compare
	//					  float fEpsilon - the epsilon value of the comparison
	//
	// Purpose:  This function returns whether the two values are within
	//					fEpsilon of one another
	////////////////////////////////////////////////////////////////////////////////
	static bool FloatEquals(float fVal1, float fVal2, float fEpsilon)
	{
		return ( fabs( fVal1 - fVal2 ) < fEpsilon );
	}
	//////////////////////////////////////////////////////////////////////////
	//Get/Set delta time
	//////////////////////////////////////////////////////////////////////////
	void SetTime(float fT)
	{
		m_fElapsed = fT;
	}
	float GetTime(void)
	{
		return m_fElapsed;
	}
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “AddSepBox/Static/NonStatic”
	//
	// Return:  void
	//
	// Parameters: CCollideable*		- component to add
	//					  unsigned int		- id of the component's object
	//
	// Purpose:  These functions add a component to the respective list(static/nonstatic/etc)
	////////////////////////////////////////////////////////////////////////////////
	void AddSepBox(CCollideable* cStatic, unsigned int nObjID);
	void AddStatic(CCollideable* cStatic, unsigned int nObjID);
	void AddNonStatic(CCollideable* cCollide, unsigned int nObjID);
	////////////////////////////////////////////////////////////////////////////////
	// Function: “RemoveSepBox/Static/NonStatic”
	//
	// Return:  void
	//
	// Parameters: CCollideable*		- component to remove
	//					  unsigned int		- id of the component's object
	//
	// Purpose:  These functions remove a component to the 
	//					respective list(static/nonstatic/etc)
	////////////////////////////////////////////////////////////////////////////////
	void RemoveSepBox(CCollideable* cStatic, unsigned int nObjID);
	void RemoveStatic(CCollideable* cStatic, unsigned int nObjID);
	void RemoveNonStatic(CCollideable* cCollide, unsigned int nObjID);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “CreateCollideableComponent"
	//
	// Return:  static int
	//
	// Parameters: lua_State* pLua		- the pointer to the lua state
	//
	// Purpose:  Creates a collideable component with values from lua
	////////////////////////////////////////////////////////////////////////////////
	static int CreateCollideableComponent(lua_State* pLua);
	////////////////////////////////////////////////////////////////////////////////
	// Function: “CreateCollideableComponent”
	//
	// Return:  void
	//
	// Parameters: CObject* pParent		- object to attach the component to
	//					  bool	isStatic				- whether the object is a static or nonstatic object
	//					  bool isReactor			- true if the object reacts to collisions
	//					  unsigned int objType	- the type of object to attach the component to
	//
	// Purpose:  Creates a collideable component with the data passed in
	////////////////////////////////////////////////////////////////////////////////
	static CCollideable* CreateCollideableComponent(CObject* pParent,
		bool isStatic, bool isReactor, unsigned int objType);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “SearchForCollision"
	//
	// Return:  void
	//
	// Parameters: float fDeltaTime	- elapsed time since last frame
	//
	// Purpose:  loops through the components calling TestCollision on certain pairs
	//					ie. all nonstatics to all nonstatics, and all nonstatics to the statics
	////////////////////////////////////////////////////////////////////////////////
	void SearchForCollision(float fDeltaTime);
	////////////////////////////////////////////////////////////////////////////////
	// Function: “TestCollision"
	//
	// Return:  bool		- true = collision happened, false = no collision
	//
	// Parameters: CCollideable* obj1, obj2		- the two objects to test collision with
	//
	// Purpose:  tests collision between two collideable components
	////////////////////////////////////////////////////////////////////////////////
	bool TestCollision(CCollideable* obj1, CCollideable* obj2);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “CondenseCollisionBoxes"
	//
	// Return:  void
	//
	// Parameters: void
	//
	// Purpose: finds all pairs of adjacent AABBs and combines them
	////////////////////////////////////////////////////////////////////////////////
	void CondenseCollisionBoxes(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function: “CreateRenderedComboBoxes"
	//
	// Return:  void
	//
	// Parameters: void
	//
	// Purpose:  creates the renderable collision volumes for all AABBs after combining
	////////////////////////////////////////////////////////////////////////////////
	void CreateRenderedComboBoxes(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function: “CombineLinedBoxes"
	//
	// Return:  void
	//
	// Parameters: void
	//
	// Purpose:  calls condense collision boxes a few times to make sure no adjacent
	//					box pairs are left, and then creates the renderable collision volumes
	//					for them
	////////////////////////////////////////////////////////////////////////////////
	void CombineLinedBoxes(void)
	{
		//by our powers combined
		CondenseCollisionBoxes(); CondenseCollisionBoxes(); CondenseCollisionBoxes();
		//we get pretty sweet collision shapes
		CreateRenderedComboBoxes();
	}
	////////////////////////////////////////////////////////////////////////////////
	// Function: “SetAllNotChecked"
	//
	// Return:  void
	//
	// Parameters: void
	//
	// Purpose:  sets all the collision components' "checked this frame" status to false
	////////////////////////////////////////////////////////////////////////////////
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
	//accessors for the effect objects
	//////////////////////////////////////////////////////////////////////////
	CObject* GetTurkeyDestroyObject(void)
	{
		return m_pTurkeyDestroy;
	}
	CObject* GetPieDestroyObject(void)
	{
		return m_pPieDestroy;
	}
	CObject* GetBananaDestroyObject(void)
	{
		return m_pBananaDestroy;
	}
	CCollideable* GetNonStaticByID(int ObjID)
	{
		return m_cNonStaticObjects.find(ObjID)->second;
	}

	//////////////////////////////////////////////////////////////////////////
	//	Collision functions
	//
	// Function: “<Shape>To<Shape>Intersection"
	//
	// Return:  bool	- true if collision, false if no collision
	//
	// Parameters: CCollideable* obj1, obj2		- the two objects that will be tested
	//					  D3DXVECTOR3* vReflect	- the normal from the collision point
	//					  bool bReact						- if the first object should react to the collision
	//
	// Purpose: These functions take two components for testing collision
	//					the system allows for objects to be used in functions that don't match
	//					a.k.a. - a sphere and a box using OBB to AABB tests will work
	//					the order the shapes are mentioned is the order the objects of
	//					the same respective BVTYPE have to be passed in
	//					vReflect is a vector passed in that is set to the reflect vector or the
	//					surface normal of the collision
	//					bReact is a bool for whether or not the object will react to the collision
	//					with a translation/event/etc
	////////////////////////////////////////////////////////////////////////////////
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
	bool LineToSphereIntersection(TLine tLine, TSphere tSphere, D3DXVECTOR3& tColPoint);
	bool LineToTriangle(TLine tLine, TTriangle triangle, D3DXVECTOR3 & tColpoint);
	bool RayToLineIntersection(TRay tRay, TLine tSegment);
	bool PointInTriangle(TTriangle tTri, D3DXVECTOR3 tTestPt);
		bool RayToSphereIntersection(TRay ray, TSphere sphere, float & t, D3DXVECTOR3 &intersect);
	bool RayToTriangleIntersection(TTriangle tri, TRay ray, float &t);
	bool MovingSphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2, D3DXVECTOR3 tVelocity,
		D3DXVECTOR3* vReflect, bool bReact);
	bool MovingSphereToTriangleIntersection(CCollideable* obj, TTriangle* tri,
		D3DXVECTOR3* vReflect, D3DXVECTOR3* tCol, float &t, bool bReact);
	
	///////////////////////////////////////////////////////////
	//Various support functions used throughout the manager
	///////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “LineEquals"
	//
	// Return:  bool	- true if the same, false if not
	//
	// Parameters: TLine Line1, Line2	- the two lines to check
	//
	// Purpose:  returns whether the two line structs are the same
	////////////////////////////////////////////////////////////////////////////////
	bool LineEquals(TLine Line1, TLine Line2)
	{
		if(Line1.cLineStart == Line2.cLineStart &&
			Line1.cLineEnd == Line2.cLineEnd)
			return true;
		else
			return false;
	}
	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetAABBNormal"
	//
	// Return:  D3DXVECTOR3	- the normal from the point of collision
	//
	// Parameters: D3DXVECTOR3 colPt	- point of collision
	//					  TAABB colBox				- the box to get the normal from
	//
	// Purpose:  gets the normal of the side that the collision point is on on the box
	////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 GetAABBNormal(D3DXVECTOR3 colPt, TAABB colBox);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetSphereNormal"
	//
	// Return:  D3DXVECTOR3	- the normal from the point of collision
	//
	// Parameters: D3DXVECTOR3 colPt	- point of collision
	//					  TSphere colSphere		- the sphere to get the normal from
	//
	// Purpose:  returns the normal of the sphere in the direction of the colPt from the center
	////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 GetSphereNormal(D3DXVECTOR3 colPt, TSphere colSphere);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetReflectedVector"
	//
	// Return:  D3DXVECTOR3	- the reflected vector
	//
	// Parameters: D3DXVECTOR3 vDir	- the vector to reflect
	//					  D3DXVECTOR3 vNorm - the normal to reflect around
	//
	// Purpose:  returns vDir reflected off of vNorm
	////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 GetReflectedVector(D3DXVECTOR3 vDir, D3DXVECTOR3 vNorm);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetCloserPt"
	//
	// Return:  D3DXVECTOR3	- the closer of the two points(tPt1/2)
	//
	// Parameters: D3DXVECTOR3 tPt1, tPt2 - the two points to test
	//					  D3DXVECTOR3 tTestPt		- the point to check distance from
	//
	// Purpose:  takes in two points and a test point, and returns which of the 
	//					first two points is closer to the test point
	////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 GetCloserPt(D3DXVECTOR3 tPt1, D3DXVECTOR3 tPt2, D3DXVECTOR3 tTestpt);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “ClosestPointAABB/OBB"
	//
	// Return:  void
	//
	// Parameters: D3DXVECTOR3 tTestPt - the point to check with
	//					  D3DXVECTOR3 tClosest - the closest point to the AABB/OBB
	//					  TAABB/TOBB tBox	- the AABB/OBB to check with
	//
	// Purpose:  returns the closest point to the given AABB/OBB from the test point
	////////////////////////////////////////////////////////////////////////////////
	void ClosestPointToAABB(TAABB tBox, D3DXVECTOR3 tTestPt, D3DXVECTOR3& tClosest);
	void ClosestPointToOBB(TOBB tBox, D3DXVECTOR3 tTestPt, D3DXVECTOR3& tClosest);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “HalfSpaceTest"
	//
	// Return:  int	- 1 if in front, 0 if on plane, -1 if behind
	//
	// Parameters: D3DXVECTOR3 tNorm	- plane normal
	//					  D3DXVECTOR3 tPlanePt - a point on the plane
	//					  D3DXVECTOR3 tTest	- the point to test against the plane
	//
	// Purpose:  performs a half space test and returns which side the test point 
	//					is on(1=front, -1 = back, 0 = on plane)
	////////////////////////////////////////////////////////////////////////////////
	int HalfSpaceTest(D3DXVECTOR3 tNorm, D3DXVECTOR3 tPlanePt, D3DXVECTOR3 tTest);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “ClosestPtPointTriangle"
	//
	// Return:  D3DXVECTOR3 - closest point
	//
	// Parameters: D3DXVECTOR3 p	- point to test
	//					  D3DXVECTOR3 a,b,c	- the verts of the triangle
	//
	// Purpose: returns the closest point to/on the triangle from the test point
	////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 ClosestPtPointTriangle(D3DXVECTOR3 p, D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetCollisionPtSpheres"
	//
	// Return:  D3DXVECTOR3 - the point of the collision
	//
	// Parameters: CCollideable* obj1,obj2 - the two spheres to test
	//
	// Purpose: finds the center point of the collision between two spheres
	////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 GetCollisionPtSpheres(CCollideable* obj1, CCollideable* obj2);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “ClosestPointOnLine"
	//
	// Return:  D3DXVECTOR3 - the point of the collision
	//
	// Parameters: TLine line - the line to test with 
	//					  D3DXVECTOR3 testpt - the test point
	//
	// Purpose: returns the closest point on the given line from the test point
	////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 ClosestPointOnLine(TLine line, D3DXVECTOR3 testpt);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “TestObjAgainstWall"
	//
	// Return:  int, zero is good from the looks of it
	//
	// Parameters: CCollideable* obj1	- object to test against wall
	//
	// Purpose: tests an individual object against the wall and performs the appropriate 
	//					reaction(translations, etc)
	////////////////////////////////////////////////////////////////////////////////
	int TestObjAgainstWall(CCollideable* obj1);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “TestObjAgainstWall"
	//
	// Return:  int, zero is good from the looks of it
	//
	// Parameters: D3DXVECTOR3 &camFrame - camera frame
	//											  &vFwdVec	- camera forward vector
	//
	// Purpose: this version of TestObjAgainstWall is for the camera
	////////////////////////////////////////////////////////////////////////////////
	int TestObjAgainstWall(D3DXVECTOR3 &cameraFrame, D3DXVECTOR3 &vFwdVec);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “IntersectRayTriangle"
	//
	// Return:  bool	- true if intersection, false if no intersection
	//
	// Parameters: const D3DXVECTOR3& vert0/1/2 - the triangle verts
	//					  const D3DXVECTOR3& norm	- norm of the triangle
	//					  const D3DXVECTOR3& start, d	- the start point and direction(d) of the ray
	//					  float& t	- the time of intersection
	//
	// Purpose: tests whether or not the given ray(start/d) intersects the given 
	//					triangle(vert0-1-2,norm)
	////////////////////////////////////////////////////////////////////////////////
	bool IntersectRayTriangle( const D3DXVECTOR3 &vert0, const D3DXVECTOR3 &vert1, const D3DXVECTOR3 &vert2, 
		const D3DXVECTOR3 &norm, const D3DXVECTOR3 &start, const D3DXVECTOR3 &d, float &t );
};


#endif