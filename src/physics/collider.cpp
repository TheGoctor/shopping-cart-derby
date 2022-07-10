#include "CCollideable.h"

void CCollideable::Init(void)
{
}
	
//////////////////////////////////////////////////////////////////////////
//	Accessors
//////////////////////////////////////////////////////////////////////////
bool CCollideable::GetStatic(void)
{
	return m_bIsStatic;
}
scd::object* CCollideable::GetParent(void)
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
	TAABB aabb;
	aabb.cBoxMin = (m_cCenterPoint - m_fExtents);
	aabb.cBoxMax = (m_cCenterPoint + m_fExtents);
	return aabb;
}
TOBB CCollideable::GetOBB()
{
	TOBB obb;
	obb.cCenterPoint = m_cCenterPoint;
	obb.tE = m_fExtents;
	for(unsigned i = 0; i < 3; ++i)
		obb.tU[i] = m_tLocalAxes[i];
	return obb;
}
TSphere CCollideable::GetSphere()
{
	TSphere sphere;
	sphere.cPosition = m_cCenterPoint;
	sphere.fRadius = m_fExtents[0];
	return sphere;
}
TLine CCollideable::GetLine()
{
	TLine line;
	line.cLineStart = m_cCenterPoint;
	line.cLineEnd = m_tLocalAxes[0] * m_fExtents.x;	//localeaxes[0] = linedir, fextents[0] = linedistance
	return line;
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
void CCollideable::SetParent(scd::object* cParent)
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

	m_cCenterPoint = (taabb.cBoxMin + taabb.cBoxMax)/2;
	//set local axes during test collision for each shape for ease
	m_fExtents.x = fabs((taabb.cBoxMax.x - taabb.cBoxMin.x)/2.0f);
	m_fExtents.y = fabs((taabb.cBoxMax.y - taabb.cBoxMin.y)/2.0f);
	m_fExtents.z = fabs((taabb.cBoxMax.z - taabb.cBoxMin.z)/2.0f);
	m_tLocalAxes[0] = scd::vector3(1.0f, 0.0f, 0.0f);
	m_tLocalAxes[1] = scd::vector3(0.0f, 1.0f, 0.0f);
	m_tLocalAxes[2] = scd::vector3(0.0f, 0.0f, 1.0f);
}
void CCollideable::SetOBB(TOBB tobb)
{
	m_cCenterPoint = tobb.cCenterPoint;
	m_fExtents = tobb.tE;
	for(unsigned i = 0; i < 3; ++i)
		m_tLocalAxes[i] = tobb.tU[i];
}
void CCollideable::SetSphere(TSphere tsphere)
{
	//		m_BVSphere = tsphere;
	m_cCenterPoint = tsphere.cPosition;
	m_fExtents.x = m_fExtents.y = m_fExtents.z = tsphere.fRadius;
	//set local axes in test collision
}
void CCollideable::SetObjType(unsigned int ntype)
{
	m_nObjType = ntype;
}
void CCollideable::SetIsReactor(bool breact)
{
	m_bIsReactor = breact;
}