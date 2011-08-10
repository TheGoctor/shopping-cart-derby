/************************************************************************
* Filename:			CCollideable.h
* Mod. Date:		07/25/2011
* Mod. Initials:	RN
* Author:			Raymond W. Nieves
* Purpose:			Encapsulates all collision code within a Behavior Object
************************************************************************/
#ifndef _CCOLLIDEABLE_H_
#define _CCOLLIDEABLE_H_

#include <D3dx9math.h>
#include "..\..\IComponent.h"
#include "..\..\Enums.h"

class CObject;
class CBVHNode;
//structures for various bounding volumes
//triangle
struct TTriangle
{
	D3DXVECTOR3 tVert0;
	D3DXVECTOR3 tVert1;
	D3DXVECTOR3 tVert2;
	D3DXVECTOR3 tNorm;
};
//Axis-aligned Bounding Box
struct TAABB
{
	D3DXVECTOR3 cBoxMin;
	D3DXVECTOR3 cBoxMax;
};
//Oriented Bounding Box
struct TOBB
{
	D3DXVECTOR3 cCenterPoint;	//center point of OBB	
	D3DXVECTOR3 tU[3];		//local x, y, and z axes
	D3DXVECTOR3 tE;		//positive halfwidth extents along each axes
};
//Sphere BV
struct TSphere
{
	D3DXVECTOR3 cPosition;
	float		fRadius;
};
//Plane (not so much a BV, but it will represent a collideable surface such as a wall)
struct TPlane
{
	D3DXVECTOR3 cPlaneNormal;
	D3DXVECTOR3 cPlanePoint;
	float fOffset;
};
//Line
struct TLine
{
	D3DXVECTOR3 cLineStart;
	D3DXVECTOR3 cLineEnd;
};
struct TRay
{
	D3DXVECTOR3 cRayStart;
	D3DXVECTOR3 cRayNormal;
};

class CCollideable : public IComponent
{
private:
	//various bounding volumes, objects should only use the ones they need
	/////////////////////////////////
//	TOBB    m_BVOBB;
//	TPlane	m_BVPlane;
//	TLine   m_BVLine;	
//	TFrustum m_BVFrustum;
	/////////////////////////////////

	int		m_nBVType;			//type of bounding volume
	bool	m_bWasChecked;		//if the object was checked on the current frame
	bool	m_bIsStatic;			//true if the object's position is static
	bool	m_bIsReactor;			//true if the object causes a collision reaction or if objects pass through it
	unsigned int m_nObjType;		//object type

	CBVHNode *m_cBVHNode;			//bvh node where this object is located
	CObject *m_cParent;				//pointer to the object holding this	
	
public:
	//new collision volume, works for all shapes
	//SPHERE - centerpoint - derp figure it out,  localaxes = local x/y/z axes, m_fextents = radius
	//AABB - centerpoint - (min+max)/2, local axes = local x/y/z, extents = halfwidth extents along each localaxes[]
	//OBB - this definition is how an obb is best defined, pretty much the same as the aabb definition
	//TRIANGLE - centerpoint = can be the center of the tri, local axes would be the tri verts, extents would be the normal
	//PLANE - just define the way a triangle would be defined with this
	//Line/Ray - centerpoint = start, local axes[0] would be the line direction or end point of segment, extents[0] would be the ray distance
	D3DXVECTOR3 m_cCenterPoint;
	D3DXVECTOR3 m_tLocalAxes[3];
	D3DXVECTOR3 m_fExtents;			//used as a radius for the 3 axes

	//tunneling members
	bool					m_bNextFrameTunneling;			//true if the next frame may have tunneling
	D3DXVECTOR3	m_vPrevPos, m_vPrevNorm;		//previous position, and the normal of the previous frame's collision
	CObject*			m_cTunneledObject;				//object we may have tunneled through

	//default constructor
	CCollideable() 
	{
		m_bWasChecked = false;
	}
	//constructor
	CCollideable(int type) : m_nBVType(type)
	{
	}
	//destructor
	~CCollideable() 
	{
	}
	//initialize function
	void Init(void);

	//Accessors
	bool GetStatic(void);
	unsigned int GetFlags(void);
	CObject* GetParent(void);
	CBVHNode* GetBVHNode(void);
	int GetBVType();
	TAABB GetAABB();
	TOBB GetOBB();
	TSphere GetSphere();
	TPlane GetPlane();
	TLine GetLine();
	bool GetWasChecked();
	unsigned int GetObjType();
	bool GetReactor();
	//Mutators
	void SetStatic(bool bstatic);
	void SetFlags(unsigned int nflag);
	void SetParent(CObject* cParent);
	void SetWasChecked(bool bchecked);
	void SetBVHNode(CBVHNode* cNode);
	void SetBVType(int type);
	void SetAABB(TAABB taabb);
	void SetOBB(TOBB tobb);
	void SetSphere(TSphere tsphere);
	void SetPlane(TPlane tplane);
	void SetLine(TLine tLine);
	void SetObjType(unsigned int ntype);
	void SetIsReactor(bool breact);
};

#endif