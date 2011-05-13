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

//defines the depth limit of the BVH
#define BVHDEPTH_LIMIT 3
//enum for splitting type(either splitting across x or z axis)
enum ESPLITTYPE {XSPLIT = -1, ZSPLIT = 1};  //nextsplittype = ~prevsplittype
class CBVHNode
{
private:
	int m_nSplitType;			//set to X/Z-SPLIT to determine how the nodes children were split
	bool m_bIsLeaf;
	list<CCollideable*, CAllocator<CCollideable*>> m_pcObjects;	//objects in the node(only contains data in a leaf node
	TAABB m_tBV;			//the bounding volume around this node's area
	//	list<CBVHNode*> m_pChildren; 
	CBVHNode* m_pParent;
	CBVHNode* m_pRHS;	//right side split
	CBVHNode* m_pLHS;	//left "

public:
	// Constructors
	CBVHNode() : m_nSplitType(0), m_bIsLeaf(false), m_pParent(NULL), 
		m_pRHS(NULL), m_pLHS(NULL)
	{
		m_tBV.cBoxMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_tBV.cBoxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	CBVHNode(int nsplittype, TAABB tNodeBounds) : m_nSplitType(nsplittype),
		m_tBV(tNodeBounds), m_bIsLeaf(false)
	{
	}

	// Destructor
	~CBVHNode()
	{
	}

	// Accessors
	CBVHNode* GetParent(void)
	{
		return m_pParent;
	}
	CBVHNode* GetRHS(void)
	{
		return m_pRHS;
	}
	TAABB GetBounds()
	{
		return m_tBV;
	}
	CBVHNode* GetLHS(void)
	{
		return m_pLHS;
	}
	int GetSplitType(void)
	{
		return m_nSplitType;
	}
	bool IsLeafNode(void)
	{	
		return m_bIsLeaf;
	}
	// Mutators
	void SetParent(CBVHNode* pParent) 
	{
		m_pParent = pParent;
	}
	void SetSplitType(int nsplittype)
	{
		m_nSplitType = nsplittype;
	}
	void SetIsLeaf(bool bleaf)
	{
		m_bIsLeaf = bleaf;
	}
	void SetLHS(CBVHNode* lhs)
	{
		m_pLHS = lhs;
	}
	void SetRHS(CBVHNode* rhs)
	{
		m_pRHS = rhs;
	}
	void SetBounds(TAABB tbounds)
	{
		m_tBV.cBoxMax = tbounds.cBoxMax;
		m_tBV.cBoxMin = tbounds.cBoxMin;
	}

	void SplitBV(CBVHNode* pParent, CBVHNode* plhs, CBVHNode* prhs);
};

/*******************************************************************************
//  Collision Manager - handles all collideable components collisions/reactions
*******************************************************************************/
class CCollisionManager
{
private:
	map<unsigned int, CCollideable*, less<unsigned int>, CAllocator<
		pair<unsigned int, CCollideable*>>> m_cStaticObjects;
	map<unsigned int, CCollideable*, less<unsigned int>, CAllocator<
		pair<unsigned int, CCollideable*>>> m_cNonStaticObjects;
	CBVHNode*							m_pRoot;				//BVH root node

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
	static void Update(IEvent* pEvent, IComponent* pComponent);

	// accessors
	void AddStatic(CCollideable* cStatic, unsigned int nObjID);
	void AddNonStatic(CCollideable* cCollide, unsigned int nObjID);
	// mutators
	void RemoveStatic(CCollideable* cStatic, unsigned int nObjID);
	void RemoveNonStatic(CCollideable* cCollide, unsigned int nObjID);

	//component functions
	static int CreateCollideableComponent(lua_State* pLua);
	static CCollideable* CreateCollideableComponent(CObject* pParent,
		bool isStatic, bool isReactor, unsigned int objType);

	//tests for collision between two components(called in update)
	void SearchForCollision(void);
	bool TestCollision(CCollideable* obj1, CCollideable* obj2);
	void CheckRam(CCollideable* obj1, CCollideable* obj2, D3DXVECTOR3 tvel1, D3DXVECTOR3 tvel2);
	void InitBVH();
	/************************************************************************
	*	GenerateBVH :	Generates a BVH from all of the static and non-static
	*					objects in the scene.  depth of the hierarchy is 
	*					determined by the defined "BVHDEPTH_LIMIT"
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	void GenerateBVH(CBVHNode* pCurrNode, int ndepth);

	/***************************************************************************
	*	Traverse :		Traverses the BVH for collision detection/culling/etc
	*
	*	Mod Date:		4/12/11
	*	Mod Initials:	RN
	***************************************************************************/
	void Traverse(void);

	/************************************************************************
	*	LoadBVH :		Loads a valid BVH from a file
	*
	*	Ins :			szfilename - the name of the file to read the BVH
	*								from.  if no file exists, this function
	*								calls "GenerateBVH" to recreate it
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	void LoadBVH(char* szfilename);		

	/************************************************************************
	*	SaveBVH :		saves the current BVH tree to a file
	*
	*	Ins :			szfilename - the name of the file to save the BVH to
	*								if the file does not exist it will be
	*								created
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	void SaveBVH(char* szfilename);	

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


	bool SphereToSphereIntersection(CCollideable* obj1, CCollideable* obj2);
	bool SphereToSphereReaction(CCollideable* obj1, CCollideable* obj2);

	bool SphereToPlaneIntersection(CCollideable* obj1, CCollideable* obj2);
	bool SphereToPlaneReaction(CCollideable* obj1, CCollideable* obj2);

	bool SphereToOBBIntersection(CCollideable* obj1, CCollideable* obj2);
	bool SphereToOBBReaction(CCollideable* obj1, CCollideable* obj2);

	bool SphereToAABBIntersection(CCollideable* obj1, CCollideable* obj2);
	bool SphereToAABBReaction(CCollideable* obj1, CCollideable* obj2);

	bool OBBToOBBIntersection(CCollideable* obj1, CCollideable* obj2);
	bool OBBToOBBReaction(CCollideable* obj1, CCollideable* obj2);

	bool OBBToPlaneIntersection(CCollideable* obj1, CCollideable* obj2);
	bool OBBToPlaneReaction(CCollideable* obj1, CCollideable* obj2);

	bool PlaneToPlaneIntersection(CCollideable* obj1, CCollideable* obj2);

	bool AABBTOAABBIntersection(CCollideable* obj1, CCollideable* obj2);
	bool AABBToAABBReaction(CCollideable* obj1, CCollideable* obj2);

	//frustum collisions
	bool SphereToFrustum(CCollideable* obj1, CCollideable* obj2);
	bool AABBToFrustum(CCollideable* obj1, CCollideable* obj2);

	//////////////////////////////////////////////////////////////////////////
	//Collision Line Functions
	// these functions are used mostly by AI entities for pathfinding
	//////////////////////////////////////////////////////////////////////////
	bool LineToSphereIntersection(TLine tLine, TSphere tSphere, D3DXVECTOR3& tColPoint);
	bool LineToPlaneIntersection(TLine tLine, TPlane tPlane, D3DXVECTOR3& tColPoint);
	bool LineToOBBIntersection(TLine tLine, TOBB tOBB, D3DXVECTOR3& tColPoint);
	bool LineToAABBIntersection(TLine tLine, TAABB tAABB, D3DXVECTOR3& tColPoint);

	//////////////////////////////////////////////////////////////////////////
	//returns false and sets tClosestPt to the closest collision point
	//returns true if the line reaches the waypoint uninterrupted
	//////////////////////////////////////////////////////////////////////////
	bool LineToWayPoint(TLine tLine, D3DXVECTOR3 &tClosestPt);

	//////////////////////////////////////////////////////////////////////////
	//Collision Support Functions
	// these functions serve to assist the collision check functions
	// they will be only be called from within the collision manager
	//////////////////////////////////////////////////////////////////////////

	bool PointInCone(D3DXVECTOR3 tPlayerMove, D3DXVECTOR3 tPlayerPos, 
		D3DXVECTOR3 tTargetPos, float fCone);
	void CheckFrustDist(float fDist, CCollideable* pCheck, bool &collide, bool &behind, bool &intersect);
	D3DXVECTOR3 GetCloserPt(D3DXVECTOR3 tPt1, D3DXVECTOR3 tPt2, D3DXVECTOR3 tTestpt);

};


#endif