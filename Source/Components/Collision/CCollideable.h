/************************************************************************
* Filename:			CCollideable.h
* Mod. Date:		03/27/2011
* Mod. Initials:	RN
* Author:			Raymond W. Nieves
* Purpose:			Encapsulates all collision code within a Behavior Object
************************************************************************/
#ifndef _CCOLLIDEABLE_H_
#define _CCOLLIDEABLE_H_

#include <D3dx9math.h>
#include "..\..\IComponent.h"

class CObject;
class CBVHNode;

//type enum for the shape of the bounding volume of an object
enum EBVSHAPE {BSPHERE = 1, BAABB = 2, BOBB = 3, BLINE = 4, BPLANE = 5};



//Axis-aligned Bounding Box
struct TAABB
{
	D3DXVECTOR3 cBoxMin;
	D3DXVECTOR3 cBoxMax;
};
//Oriented Bounding Box
struct TOBB
{
	D3DXVECTOR3 cFrontTopLeft;
	D3DXVECTOR3 cFrontTopRight;
	D3DXVECTOR3 cFrontBottomLeft;
	D3DXVECTOR3 cFrontBottomRight;

	D3DXVECTOR3 cBackTopLeft;
	D3DXVECTOR3 cBackTopRight;
	D3DXVECTOR3 cBackBottomLeft;
	D3DXVECTOR3 CBackBottomRight;
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
};
struct TLine
{
	D3DXVECTOR3 cLineStart;
	D3DXVECTOR3 cLineEnd;
};



class CCollideable : public IComponent
{
private:
	//various bounding volumes, objects should only use the ones they need
	TAABB	m_BVAABB;
	TOBB	m_BVOBB;
	TSphere	m_BVSphere;
	TPlane	m_BVPlane;
	TLine   m_BVLine;	
	int		m_nBVType;
	bool	m_bWasChecked;

	CBVHNode *m_cBVHNode;			//bvh node where this object is located
	CObject *m_cParent;				//pointer to the object holding this
	unsigned int m_nFlags;			//flags to determine if the object is "dirty"
	bool	m_bIsStatic;			//true if the object's position is static
public:
	CCollideable() 
	{
		m_bWasChecked = false;
	}

	CCollideable(int type) : m_nBVType(type)
	{
	}

	~CCollideable() 
	{
	}

	void Init(void)
	{

	}

	//////////////////////////////////////////////////////////////////////////
	//	Accessors
	//////////////////////////////////////////////////////////////////////////
	bool GetStatic(void)
	{
		return m_bIsStatic;
	}
	unsigned int GetFlags(void)
	{
		return m_nFlags;
	}
	CObject* GetParent(void)
	{
		return m_cParent;
	}
	CBVHNode* GetBVHNode(void)
	{
		return m_cBVHNode;
	}

	int GetBVType()
	{
		return m_nBVType;
	}
	TAABB GetAABB()
	{
		return m_BVAABB;
	}
	TOBB GetOBB()
	{
		return m_BVOBB;
	}
	TSphere GetSphere()
	{
		return m_BVSphere;
	}
	TPlane GetPlane()
	{
		return m_BVPlane;
	}
	bool GetWasChecked()
	{
		return m_bWasChecked;
	}
	//////////////////////////////////////////////////////////////////////////
	//	Mutators
	//////////////////////////////////////////////////////////////////////////
	void SetStatic(bool bstatic)
	{
		m_bIsStatic = bstatic;
	}
	void SetFlags(unsigned int nflag)
	{
		m_nFlags = nflag;
	}
	void SetParent(CObject* cParent)
	{
		m_cParent = cParent;
	}
	void SetWasChecked(bool bchecked)
	{
		m_bWasChecked = bchecked;
	}
	void SetBVHNode(CBVHNode* cNode)
	{
		m_cBVHNode = cNode;
	}

	void SetBVType(int type)
	{
		m_nBVType = type;
	}
	void SetAABB(TAABB taabb)
	{
		//		m_BVAABB = taabb;
		m_BVAABB.cBoxMax.x = taabb.cBoxMax.x;
		m_BVAABB.cBoxMax.y = taabb.cBoxMax.y;
		m_BVAABB.cBoxMax.z = taabb.cBoxMax.z;
		m_BVAABB.cBoxMin.x = taabb.cBoxMin.x;
		m_BVAABB.cBoxMin.y = taabb.cBoxMin.y;
		m_BVAABB.cBoxMin.z = taabb.cBoxMin.z;
	}
	void SetOBB(TOBB tobb)
	{
		m_BVOBB = tobb;
	}
	void SetSphere(TSphere tsphere)
	{
		//		m_BVSphere = tsphere;
		m_BVSphere.cPosition.x = tsphere.cPosition.x;
		m_BVSphere.cPosition.y = tsphere.cPosition.y;
		m_BVSphere.cPosition.z = tsphere.cPosition.z;
		m_BVSphere.fRadius = tsphere.fRadius;
	}
	void SetPlane(TPlane tplane)
	{
		//		m_BVPlane = tplane;
		m_BVPlane.cPlaneNormal.x = tplane.cPlaneNormal.x;
		m_BVPlane.cPlaneNormal.y = tplane.cPlaneNormal.y;
		m_BVPlane.cPlaneNormal.z = tplane.cPlaneNormal.z;
		m_BVPlane.cPlanePoint.x = tplane.cPlanePoint.x;
		m_BVPlane.cPlanePoint.y = tplane.cPlanePoint.y;
		m_BVPlane.cPlanePoint.z = tplane.cPlanePoint.z;
	}

};

#endif
