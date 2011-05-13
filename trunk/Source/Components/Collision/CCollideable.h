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
#include "..\..\Enums.h"

class CObject;
class CBVHNode;

//Axis-aligned Bounding Box
struct TAABB
{
	D3DXVECTOR3 cBoxMin;
	D3DXVECTOR3 cBoxMax;
	//	TAABB()
	//	{
	//		cBoxMin = D3DXVECTOR3(0.0f,0.0f,0.0f);
	//		cBoxMax = D3DXVECTOR3(0.0f,0.0f,0.0f);
	//	}
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
	float fOffset;
};
struct TLine
{
	D3DXVECTOR3 cLineStart;
	D3DXVECTOR3 cLineEnd;
};
struct TFrustum
{
	TPlane tNear, tFar, tTop, tBottom, tLeft, tRight;
};



class CCollideable : public IComponent
{
private:
	//various bounding volumes, objects should only use the ones they need
	TAABB	m_BVAABB;
	TOBB    m_BVOBB;
	TSphere	m_BVSphere;
	TPlane	m_BVPlane;
	TLine   m_BVLine;	
	TFrustum m_BVFrustum;
	int		m_nBVType;
	bool	m_bWasChecked;

	CBVHNode *m_cBVHNode;			//bvh node where this object is located
	CObject *m_cParent;				//pointer to the object holding this
	unsigned int m_nFlags;			//flags to determine if the object is "dirty"
	unsigned int m_nObjType;		//object type
	bool	m_bIsStatic;			//true if the object's position is static
	bool	m_bIsReactor;			//true if the object causes a collision reaction or if objects pass through it
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
	void Init(void);
	void BuildFrustum(D3DXMATRIX mViewProj);

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
	TFrustum GetFrustum();
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
	void SetFrustum(TFrustum tfrust);
	void SetFrustumPlane(TPlane frustplane, TPlane &target);	//sets target to frustplane
	void SetObjType(unsigned int ntype);
	void SetIsReactor(bool breact);
};

#endif