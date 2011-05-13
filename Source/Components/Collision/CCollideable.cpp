#include "CCollideable.h"

void CCollideable::Init(void)
{
}
void CCollideable::BuildFrustum(D3DXMATRIX mViewProj)
{
	//view and proj combined

	//Left
	m_BVFrustum.tLeft.cPlaneNormal.x = mViewProj._14 + mViewProj._11;
	m_BVFrustum.tLeft.cPlaneNormal.y = mViewProj._24 + mViewProj._21;
	m_BVFrustum.tLeft.cPlaneNormal.z = mViewProj._34 + mViewProj._31;
	m_BVFrustum.tLeft.fOffset = mViewProj._44 + mViewProj._41;

	//Right
	m_BVFrustum.tRight.cPlaneNormal.x = mViewProj._14 - mViewProj._11;
	m_BVFrustum.tRight.cPlaneNormal.y = mViewProj._24 - mViewProj._21;
	m_BVFrustum.tRight.cPlaneNormal.z = mViewProj._34 - mViewProj._31;
	m_BVFrustum.tRight.fOffset = mViewProj._44 - mViewProj._41;

	//Top
	m_BVFrustum.tTop.cPlaneNormal.x = mViewProj._14 - mViewProj._12;
	m_BVFrustum.tTop.cPlaneNormal.x = mViewProj._24 - mViewProj._22;
	m_BVFrustum.tTop.cPlaneNormal.x = mViewProj._34 - mViewProj._32;
	m_BVFrustum.tTop.fOffset = mViewProj._44 - mViewProj._42;

	//Bottom
	m_BVFrustum.tBottom.cPlaneNormal.x = mViewProj._14 + mViewProj._12;
	m_BVFrustum.tBottom.cPlaneNormal.x = mViewProj._24 + mViewProj._22;
	m_BVFrustum.tBottom.cPlaneNormal.x = mViewProj._34 + mViewProj._32;
	m_BVFrustum.tBottom.fOffset = mViewProj._44 + mViewProj._42;

	//Near
	m_BVFrustum.tNear.cPlaneNormal.x = mViewProj._13;
	m_BVFrustum.tNear.cPlaneNormal.y = mViewProj._23;
	m_BVFrustum.tNear.cPlaneNormal.z = mViewProj._33;
	m_BVFrustum.tNear.fOffset = mViewProj._43;

	//Far
	m_BVFrustum.tFar.cPlaneNormal.x = mViewProj._14 - mViewProj._13;
	m_BVFrustum.tFar.cPlaneNormal.y = mViewProj._24 - mViewProj._23;
	m_BVFrustum.tFar.cPlaneNormal.z = mViewProj._34 - mViewProj._33;
	m_BVFrustum.tFar.fOffset = mViewProj._44 - mViewProj._43;


}
//////////////////////////////////////////////////////////////////////////
//	Accessors
//////////////////////////////////////////////////////////////////////////
bool CCollideable::GetStatic(void)
{
	return m_bIsStatic;
}
unsigned int CCollideable::GetFlags(void)
{
	return m_nFlags;
}
CObject* CCollideable::GetParent(void)
{
	return m_cParent;
}
CBVHNode* CCollideable::GetBVHNode(void)
{
	return m_cBVHNode;
}

int CCollideable::GetBVType()
{
	return m_nBVType;
}
TAABB CCollideable::GetAABB()
{
	return m_BVAABB;
}
TOBB CCollideable::GetOBB()
{
	return m_BVOBB;
}
TSphere CCollideable::GetSphere()
{
	return m_BVSphere;
}
TPlane CCollideable::GetPlane()
{
	return m_BVPlane;
}
TFrustum CCollideable::GetFrustum()
{
	return m_BVFrustum;
}
bool CCollideable::GetWasChecked()
{
	return m_bWasChecked;
}
unsigned int CCollideable::GetObjType()
{
	return m_nObjType;
}
bool CCollideable::GetReactor()
{
	return m_bIsReactor;
}
//////////////////////////////////////////////////////////////////////////
//	Mutators
//////////////////////////////////////////////////////////////////////////
void CCollideable::SetStatic(bool bstatic)
{
	m_bIsStatic = bstatic;
}
void CCollideable::SetFlags(unsigned int nflag)
{
	m_nFlags = nflag;
}
void CCollideable::SetParent(CObject* cParent)
{
	m_cParent = cParent;
}
void CCollideable::SetWasChecked(bool bchecked)
{
	m_bWasChecked = bchecked;
}
void CCollideable::SetBVHNode(CBVHNode* cNode)
{
	m_cBVHNode = cNode;
}

void CCollideable::SetBVType(int type)
{
	m_nBVType = type;
}
void CCollideable::SetAABB(TAABB taabb)
{
	//		m_BVAABB = taabb;
	m_BVAABB.cBoxMax.x = taabb.cBoxMax.x;
	m_BVAABB.cBoxMax.y = taabb.cBoxMax.y;
	m_BVAABB.cBoxMax.z = taabb.cBoxMax.z;
	m_BVAABB.cBoxMin.x = taabb.cBoxMin.x;
	m_BVAABB.cBoxMin.y = taabb.cBoxMin.y;
	m_BVAABB.cBoxMin.z = taabb.cBoxMin.z;
}
void CCollideable::SetOBB(TOBB tobb)
{
	m_BVOBB = tobb; //TODOTODOTODO  
}
void CCollideable::SetSphere(TSphere tsphere)
{
	//		m_BVSphere = tsphere;
	m_BVSphere.cPosition.x = tsphere.cPosition.x;
	m_BVSphere.cPosition.y = tsphere.cPosition.y;
	m_BVSphere.cPosition.z = tsphere.cPosition.z;
	m_BVSphere.fRadius = tsphere.fRadius;
}
void CCollideable::SetPlane(TPlane tplane)
{
	//		m_BVPlane = tplane;
	m_BVPlane.cPlaneNormal.x = tplane.cPlaneNormal.x;
	m_BVPlane.cPlaneNormal.y = tplane.cPlaneNormal.y;
	m_BVPlane.cPlaneNormal.z = tplane.cPlaneNormal.z;
	m_BVPlane.cPlanePoint.x = tplane.cPlanePoint.x;
	m_BVPlane.cPlanePoint.y = tplane.cPlanePoint.y;
	m_BVPlane.cPlanePoint.z = tplane.cPlanePoint.z;
	//	if(tplane.fOffset)		//NOTE: this s is bananas, but ideally planes should be normal.xyz and offset, so 
	//								change may be imminent! -Raymoney
	//	{
	//		m_BVPlane.fOffset = tplane.fOffset;
	//		m_BVPlane.cPlanePoint = m_BVPlane.cPlaneNormal*tplane.fOffset;
	//	}
}
void CCollideable::SetFrustumPlane(TPlane frustplane, TPlane &target)
{
	target.cPlaneNormal = frustplane.cPlaneNormal;
	target.fOffset = frustplane.fOffset;
	target.cPlanePoint = target.cPlaneNormal*target.fOffset;
}
void CCollideable::SetFrustum(TFrustum tfrust)
{
	SetFrustumPlane(tfrust.tTop, m_BVFrustum.tTop);				//top
	SetFrustumPlane(tfrust.tBottom, m_BVFrustum.tBottom);		//bottom	
	SetFrustumPlane(tfrust.tLeft, m_BVFrustum.tLeft);				//left
	SetFrustumPlane(tfrust.tRight, m_BVFrustum.tRight);			//right
	SetFrustumPlane(tfrust.tNear, m_BVFrustum.tNear);				//near
	SetFrustumPlane(tfrust.tFar, m_BVFrustum.tFar);				//far
}
void CCollideable::SetObjType(unsigned int ntype)
{
	m_nObjType = ntype;
}
void CCollideable::SetIsReactor(bool breact)
{
	m_bIsReactor = breact;
}