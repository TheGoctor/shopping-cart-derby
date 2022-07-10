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

#include "Components/Collision/CCollideable.h"
#include "CFrame.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class IEvent;
class CEventManager;
class scd::object;

//returns whether or not two values have the same sign
#ifndef SameSign
#define SameSign(a,b) ( ((*(unsigned int*)&(a)) & 0x80000000) == ((*(unsigned int*)&(b)) & 0x80000000) )
#endif 

//typedef for maps of collideables
typedef map<unsigned int, CCollideable*, less<unsigned int>, scd::allocator<
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
	scd::vector3 *m_pReflect;
	scd::vector3 *m_pColpt;
	//////////////////////////////////////////////////////////////////////////
	//delta time
	//////////////////////////////////////////////////////////////////////////
	float m_fElapsed;
	//////////////////////////////////////////////////////////////////////////
	//effect objects for item to item collision
	//////////////////////////////////////////////////////////////////////////
	scd::object* m_pTurkeyDestroy;
	scd::object* m_pPieDestroy;
	scd::object* m_pBananaDestroy;
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
	static void Shutdown(IEvent* pEvent, scd::base_component* pComponent);
	//////////////////////////////////////////////////////////////////////////
	//returns the map of static object collision volumes
	//////////////////////////////////////////////////////////////////////////
	ColMap& GetStaticObjs() { return m_cStaticObjects; }
	//////////////////////////////////////////////////////////////////////////
	//update
	//////////////////////////////////////////////////////////////////////////
	static void Update(IEvent* pEvent, scd::base_component* pComponent);
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
	// Parameters: scd::object* pParent		- object to attach the component to
	//					  bool	isStatic				- whether the object is a static or nonstatic object
	//					  bool isReactor			- true if the object reacts to collisions
	//					  unsigned int objType	- the type of object to attach the component to
	//
	// Purpose:  Creates a collideable component with the data passed in
	////////////////////////////////////////////////////////////////////////////////
	static CCollideable* CreateCollideableComponent(scd::object* pParent,
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
		std::map<unsigned int, CCollideable*, less<unsigned int>, scd::allocator
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
	scd::object* GetTurkeyDestroyObject(void)
	{
		return m_pTurkeyDestroy;
	}
	scd::object* GetPieDestroyObject(void)
	{
		return m_pPieDestroy;
	}
	scd::object* GetBananaDestroyObject(void)
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
	//					  scd::vector3* vReflect	- the normal from the collision point
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
		scd::vector3* vReflect, bool bReact);
	bool SphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2,
		scd::vector3* vReflect, bool bReact);
	bool AABBTOAABBIntersection(CCollideable* obj1, CCollideable* obj2);
	bool CapsuleToCapsuleIntersection(CCollideable* obj1, CCollideable* obj2,
		scd::vector3* tPosition, bool bReact);
	bool CapsuleToSphereIntersection(CCollideable* obj1, CCollideable* obj2,
		scd::vector3* tPosition, bool bReact);
	bool CapsuleToAABBIntersection(CCollideable* obj1, CCollideable* obj2,
		scd::vector3* tPosition, bool bReact);
	bool CapsuleToOBBIntersection(CCollideable* obj1, CCollideable* obj2,
		scd::vector3* tPosition, bool bReact);
	bool SphereToOBBIntersection(CCollideable* obj1, CCollideable* obj2,
		scd::vector3* tPosition, bool bReact);	//bReact is pretty much always going to be true
	bool OBBToOBBIntersection(CCollideable* obj1, CCollideable* obj2,
		scd::vector3* tPosition, bool bReact);
	bool OBBToAABBIntersection(CCollideable* obj1, CCollideable* obj2,
						   scd::vector3* tPosition, bool bReact);
	bool LineToSphereIntersection(TLine tLine, TSphere tSphere, scd::vector3& tColPoint);
	bool LineToTriangle(TLine tLine, TTriangle triangle, scd::vector3 & tColpoint);
	bool RayToLineIntersection(TRay tRay, TLine tSegment);
	bool PointInTriangle(TTriangle tTri, scd::vector3 tTestPt);
		bool RayToSphereIntersection(TRay ray, TSphere sphere, float & t, scd::vector3 &intersect);
	bool RayToTriangleIntersection(TTriangle tri, TRay ray, float &t);
	bool MovingSphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2, scd::vector3 tVelocity,
		scd::vector3* vReflect, bool bReact);
	bool MovingSphereToTriangleIntersection(CCollideable* obj, TTriangle* tri,
		scd::vector3* vReflect, scd::vector3* tCol, float &t, bool bReact);
	
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
	// Return:  scd::vector3	- the normal from the point of collision
	//
	// Parameters: scd::vector3 colPt	- point of collision
	//					  TAABB colBox				- the box to get the normal from
	//
	// Purpose:  gets the normal of the side that the collision point is on on the box
	////////////////////////////////////////////////////////////////////////////////
	scd::vector3 GetAABBNormal(scd::vector3 colPt, TAABB colBox);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetSphereNormal"
	//
	// Return:  scd::vector3	- the normal from the point of collision
	//
	// Parameters: scd::vector3 colPt	- point of collision
	//					  TSphere colSphere		- the sphere to get the normal from
	//
	// Purpose:  returns the normal of the sphere in the direction of the colPt from the center
	////////////////////////////////////////////////////////////////////////////////
	scd::vector3 GetSphereNormal(scd::vector3 colPt, TSphere colSphere);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetReflectedVector"
	//
	// Return:  scd::vector3	- the reflected vector
	//
	// Parameters: scd::vector3 vDir	- the vector to reflect
	//					  scd::vector3 vNorm - the normal to reflect around
	//
	// Purpose:  returns vDir reflected off of vNorm
	////////////////////////////////////////////////////////////////////////////////
	scd::vector3 GetReflectedVector(scd::vector3 vDir, scd::vector3 vNorm);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetCloserPt"
	//
	// Return:  scd::vector3	- the closer of the two points(tPt1/2)
	//
	// Parameters: scd::vector3 tPt1, tPt2 - the two points to test
	//					  scd::vector3 tTestPt		- the point to check distance from
	//
	// Purpose:  takes in two points and a test point, and returns which of the 
	//					first two points is closer to the test point
	////////////////////////////////////////////////////////////////////////////////
	scd::vector3 GetCloserPt(scd::vector3 tPt1, scd::vector3 tPt2, scd::vector3 tTestpt);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “ClosestPointAABB/OBB"
	//
	// Return:  void
	//
	// Parameters: scd::vector3 tTestPt - the point to check with
	//					  scd::vector3 tClosest - the closest point to the AABB/OBB
	//					  TAABB/TOBB tBox	- the AABB/OBB to check with
	//
	// Purpose:  returns the closest point to the given AABB/OBB from the test point
	////////////////////////////////////////////////////////////////////////////////
	void ClosestPointToAABB(TAABB tBox, scd::vector3 tTestPt, scd::vector3& tClosest);
	void ClosestPointToOBB(TOBB tBox, scd::vector3 tTestPt, scd::vector3& tClosest);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “HalfSpaceTest"
	//
	// Return:  int	- 1 if in front, 0 if on plane, -1 if behind
	//
	// Parameters: scd::vector3 tNorm	- plane normal
	//					  scd::vector3 tPlanePt - a point on the plane
	//					  scd::vector3 tTest	- the point to test against the plane
	//
	// Purpose:  performs a half space test and returns which side the test point 
	//					is on(1=front, -1 = back, 0 = on plane)
	////////////////////////////////////////////////////////////////////////////////
	int HalfSpaceTest(scd::vector3 tNorm, scd::vector3 tPlanePt, scd::vector3 tTest);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “ClosestPtPointTriangle"
	//
	// Return:  scd::vector3 - closest point
	//
	// Parameters: scd::vector3 p	- point to test
	//					  scd::vector3 a,b,c	- the verts of the triangle
	//
	// Purpose: returns the closest point to/on the triangle from the test point
	////////////////////////////////////////////////////////////////////////////////
	scd::vector3 ClosestPtPointTriangle(scd::vector3 p, scd::vector3 a, scd::vector3 b, scd::vector3 c);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetCollisionPtSpheres"
	//
	// Return:  scd::vector3 - the point of the collision
	//
	// Parameters: CCollideable* obj1,obj2 - the two spheres to test
	//
	// Purpose: finds the center point of the collision between two spheres
	////////////////////////////////////////////////////////////////////////////////
	scd::vector3 GetCollisionPtSpheres(CCollideable* obj1, CCollideable* obj2);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “ClosestPointOnLine"
	//
	// Return:  scd::vector3 - the point of the collision
	//
	// Parameters: TLine line - the line to test with 
	//					  scd::vector3 testpt - the test point
	//
	// Purpose: returns the closest point on the given line from the test point
	////////////////////////////////////////////////////////////////////////////////
	scd::vector3 ClosestPointOnLine(TLine line, scd::vector3 testpt);
	
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
	// Parameters: scd::vector3 &camFrame - camera frame
	//											  &vFwdVec	- camera forward vector
	//
	// Purpose: this version of TestObjAgainstWall is for the camera
	////////////////////////////////////////////////////////////////////////////////
	int TestObjAgainstWall(scd::vector3 &cameraFrame, scd::vector3 &vFwdVec);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “IntersectRayTriangle"
	//
	// Return:  bool	- true if intersection, false if no intersection
	//
	// Parameters: const scd::vector3& vert0/1/2 - the triangle verts
	//					  const scd::vector3& norm	- norm of the triangle
	//					  const scd::vector3& start, d	- the start point and direction(d) of the ray
	//					  float& t	- the time of intersection
	//
	// Purpose: tests whether or not the given ray(start/d) intersects the given 
	//					triangle(vert0-1-2,norm)
	////////////////////////////////////////////////////////////////////////////////
	bool IntersectRayTriangle( const scd::vector3 &vert0, const scd::vector3 &vert1, const scd::vector3 &vert2, 
		const scd::vector3 &norm, const scd::vector3 &start, const scd::vector3 &d, float &t );
};


#endif