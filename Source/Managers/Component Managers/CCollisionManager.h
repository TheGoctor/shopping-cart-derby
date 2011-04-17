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
	list<CCollideable*> m_pcObjects;	//objects in the node(only contains data in a leaf node
	TAABB m_tBV;			//the bounding volume around this node's area
//	list<CBVHNode*> m_pChildren; 
	CBVHNode* m_pParent;
	CBVHNode* m_pRHS;	//right side split
	CBVHNode* m_pLHS;	//left "

public:
	// Constructors
	CBVHNode()
	{
		m_nSplitType = 0;
		m_bIsLeaf = false;
		m_pParent = NULL;
		m_pRHS = NULL;
		m_pLHS = NULL;
		m_tBV.cBoxMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_tBV.cBoxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	CBVHNode(int nsplittype, TAABB tNodeBounds) 
	{
		m_nSplitType = nsplittype; m_tBV = tNodeBounds;
		m_bIsLeaf = false;
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
	// Mutators
	void SetParent(CBVHNode* pParent) 
	{
		m_pParent = pParent;
	}
	void SetSplitType(int nsplittype)
	{
		m_nSplitType = nsplittype;
	}
	void SplitBV(CBVHNode* pParent, CBVHNode* plhs, CBVHNode* prhs);

	bool IsLeafNode(void)
	{	
		return m_bIsLeaf;
//		return m_pChildren.empty();	
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

};

/*******************************************************************************
//  Collision Manager - handles all collideable components collisions/reactions
*******************************************************************************/
class CCollisionManager
{
private:
	map<unsigned int, CCollideable*> m_cStaticObjects;
	map<unsigned int, CCollideable*> m_cNonStaticObjects;
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
	static CCollideable* CreateCollideableComponent(CObject* pParent);





	//tests for collision between two components(called in update)
	void SearchForCollision(void);
	bool TestCollision(CCollideable* obj1, CCollideable* obj2);
	void CollisionResponse(CCollideable* obj1, CCollideable* obj2);//sends appropriate events for collision
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
		std::map<unsigned int, CCollideable*>::iterator insiter; 
		for(insiter = m_cNonStaticObjects.begin(); 
			insiter != m_cNonStaticObjects.end(); ++insiter)
		{
			(*insiter).second->SetWasChecked(false);
		}
	}


	/************************************************************************
	*	PointInCone :		finds if a point is within a specified cone
	*						projected from a player position
	*
	*	Ins:				tPlayerMove	- the movement vector of the object projecting the cone
	*						tPlayerPos	- position of the object to project the cone from
	*						tTargetPos	- position of the target to find in the cone
	*						fCone		- the angle of the projected cone
	*
	*	Returns:			bool - true if the point is in the cone, false if not
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool PointInCone(D3DXVECTOR3 tPlayerMove, D3DXVECTOR3 tPlayerPos, 
		D3DXVECTOR3 tTargetPos, float fCone);

	/************************************************************************
	*	SphereToSphereIntersection :	checks for collision between two spheres
	*
	*	Ins:				tS1Center	- center of the first sphere
	*						fS1Radius	- radius of the first sphere
	*						tS2Center	- center of the second sphere
	*						fS2Radius	- radius of the second sphere
	*
	*	Outs:				colPoint - point of collision (if any)
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool SphereToSphereIntersection(TSphere Sphere1, TSphere Sphere2);

	bool PlayerToPlayerIntersection(TSphere tSphere1, TSphere tSphere2, 
		D3DXVECTOR3 tVel1, D3DXVECTOR3 tVel2, D3DXVECTOR3 &tNew1, D3DXVECTOR3 &tNew2);

	/************************************************************************
	*	SphereToPlaneIntersection :	checks for collision between a sphere 
	*								and a plane
	*
	*	Ins:				tSCenter	- center of the sphere
	*						fSRadius	- radius of the sphere
	*						tPlaneNorm	- normal of the plane
	*						fPlaneOffset- offset of the plane
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool SphereToPlaneIntersection(TSphere tSphere, TPlane tPlane,
		D3DXVECTOR3& tNewSCenter);

	/************************************************************************
	*	SphereToOBBIntersection :	checks for collision between a sphere 
	*								and an oriented bounding box
	*
	*	Ins:				tSCenter	- center of the sphere
	*						fSRadius	- radius of the sphere
	*						tBoxMin		- min bounds of the OBB
	*						fBoxMax		- max bounds of the OBB
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool SphereToOBBIntersection(TSphere tSphere, TOBB tOBB, D3DXVECTOR3& tNewSCenter);

	/************************************************************************
	*	SphereToAABBIntersection :	checks for intersection between 
	*								a sphere and an AABB
	*
	*	Ins:				tSCenter	- center of the sphere
	*						fSRadius	- radius of the sphere
	*						tBoxMin		- min values of the OBB
	*						tBoxMax		- max values of the OBB
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool SphereToAABBIntersection(TSphere tSphere, TAABB tAABB);

	/************************************************************************
	*	LineToSphereIntersection :	checks for intersection between 
	*								a line and a sphere
	*
	*	Ins:				tStart		- start point of the line
	*						tEnd		- end point of the line
	*						tSCenter	- center of the sphere
	*						fRadius		- radius of the sphere
	*
	*	Outs:				colPoint - point of collision (if any)
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool LineToSphereIntersection(TLine tLine, TSphere tSphere, D3DXVECTOR3& tColPoint);

	/************************************************************************
	*	LineToPlaneIntersection :	checks for intersection between 
	*								a line and a plane
	*
	*	Ins:				tStart		- start point of the line
	*						tEnd		- end point of the line
	*						tPlaneNorm	- normal of the plane
	*						fPlaneOffset- offset of the plane
	*
	*	Outs:				colPoint - point of collision (if any)
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool LineToPlaneIntersection(TLine tLine, TPlane tPlane, D3DXVECTOR3& tColPoint);

	/************************************************************************
	*	LineToOBBIntersection :	checks for intersection between 
	*								a line and an OBB
	*
	*	Ins:				tStart		- start point of the line
	*						tEnd		- end point of the line
	*						tBoxMin		- min values of the OBB
	*						tBoxMax		- max values of the OBB
	*
	*	Outs:				colPoint - point of collision (if any)
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool LineToOBBIntersection(TLine tLine, TOBB tOBB, D3DXVECTOR3& tColPoint);


	/************************************************************************
	*	LineToAABBIntersection :	checks for intersection between 
	*								a line and an AABB
	*
	*	Ins:				tStart		- start point of the line
	*						tEnd		- end point of the line
	*						tBoxMin		- min values of the OBB
	*						tBoxMax		- max values of the OBB
	*
	*	Outs:				colPoint - point of collision (if any)
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool LineToAABBIntersection(TLine tLine, TOBB tAABB, D3DXVECTOR3& tColPoint);


	/************************************************************************
	*	OBBToOBBIntersection :	checks for intersection between 
	*								to OBBs
	*
	*	Ins:				tOBB1FTL->tOBB1BBR - the eight points of the first OBB
	*						tOBB2FTL->tOBB2BBR - the eight points of the second OBB
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool OBBToOBBIntersection(TOBB tOBB1, TOBB tOBB2, D3DXVECTOR3& tColPoint);

	/************************************************************************
	*	OBBToPlaneIntersection :	checks for intersection between 
	*								a line and an OBB
	*
	*	Ins:				tStart		- start point of the line
	*						tEnd		- end point of the line
	*						tBoxMin		- min values of the OBB
	*						tBoxMax		- max values of the OBB
	*
	*	Outs:				colPoint - point of collision (if any)
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool OBBToPlaneIntersection(TOBB tOBB, TPlane tPlane);
	/************************************************************************
	*	LineToOBBIntersection :	checks for intersection between 
	*								a line and an OBB
	*
	*	Ins:				tStart		- start point of the line
	*						tEnd		- end point of the line
	*						tBoxMin		- min values of the OBB
	*						tBoxMax		- max values of the OBB
	*
	*	Outs:				colPoint - point of collision (if any)
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool PlaneToPlaneIntersection(TPlane tPlane1, TPlane tPlane2);
	/************************************************************************
	*	LineToOBBIntersection :	checks for intersection between 
	*								a line and an OBB
	*
	*	Ins:				tStart		- start point of the line
	*						tEnd		- end point of the line
	*						tBoxMin		- min values of the OBB
	*						tBoxMax		- max values of the OBB
	*
	*	Outs:				colPoint - point of collision (if any)
	*
	*	Returns:			bool - true if there was a collision, else false
	*
	*	Mod Date:			3/27/2011
	*	Mod Initials:		RN
	************************************************************************/
	bool AABBTOAABBIntersection(TAABB tAABB1, TAABB tAABB2);

};


#endif