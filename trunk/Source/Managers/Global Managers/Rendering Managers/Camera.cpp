///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCamera.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	04/07/2011
//
//	Last Changed	:	04/11/2011
//
//  Purpose			:	Camera class that controls the camera movement and view
///////////////////////////////////////////////////////////////////////////////
#include "Camera.h"

const float CCamera::DEFAULT_SPRING_CONSTANT = 25.0f;
const float CCamera::DEFAULT_DAMPING_CONSTANT = 10.0f;

const float CCamera::DEFAULT_FOVX = 80.0f;
const float CCamera::DEFAULT_ZFAR = 1000.0f;
const float CCamera::DEFAULT_ZNEAR = 1.0f;

const D3DXVECTOR3 CCamera::WORLD_XAXIS(1.0f, 0.0f, 0.0f);
const D3DXVECTOR3 CCamera::WORLD_YAXIS(0.0f, 1.0f, 0.0f);
const D3DXVECTOR3 CCamera::WORLD_ZAXIS(0.0f, 0.0f, 1.0f);

CCamera::CCamera(void)
{
	m_bIsSpringEnable = true;
	m_fSpringConstant = DEFAULT_SPRING_CONSTANT;
	m_fDampingCostant = DEFAULT_DAMPING_CONSTANT;

	m_foffsetDistance = 0.0f;
	m_fheadingDegrees = 0.0f;
	m_fpitchDegrees = 0.0f;

	m_vEyePt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vEyePt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	m_vXaxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vYaxis = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vZaxis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	D3DXMatrixIdentity(&this->m_mProjectionMatrix);
	D3DXMatrixIdentity(&this->m_mViewMatrix);
	D3DXQuaternionIdentity(&this->m_qOrientation);
}

CCamera::~CCamera(void)
{
}

CCamera *CCamera::GetInstance()
{
	static CCamera Instance;
	return &Instance;
}

void CCamera::Initialize()
{
}

void CCamera::BuildPerspective(float fFieldOfView, float fAspectRatio, float fZNear, float fZFar)
{
	D3DXMatrixPerspectiveFovLH(&this->m_mProjectionMatrix, fFieldOfView,fAspectRatio,
		fZNear, fZFar);
}


void CCamera::LookAtTarget(const D3DXVECTOR3 &target)
{
	m_vTarget = target;	
}

void CCamera::LookAtTarget(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &target, const D3DXVECTOR3 &up)
{
	// The offset vector is the vector from the target 'at' position to the
	// 'eye' position. This happens to also be the local negative z axis of the
	// camera. Notice that the offset vector is always relative to the 'at'
	// position.
	m_vEyePt = eye;
	m_vTarget = target;
	m_vTargetYaxis = up;

	m_vZaxis = target - eye;
	D3DXVec3Normalize(&m_vZaxis, &m_vZaxis);

	D3DXVec3Cross(&m_vXaxis, &up, &m_vZaxis);
	D3DXVec3Normalize(&m_vXaxis, &m_vXaxis);

	D3DXVec3Cross(&m_vYaxis, &m_vZaxis, &m_vXaxis);
	D3DXVec3Normalize(&m_vYaxis, &m_vYaxis);
	D3DXVec3Normalize(&m_vXaxis, &m_vXaxis);

	D3DXMatrixIdentity(&m_mViewMatrix);

	m_mViewMatrix(0,0) = m_vXaxis.x;
	m_mViewMatrix(1,0) = m_vXaxis.y;
	m_mViewMatrix(2,0) = m_vXaxis.z;
	m_mViewMatrix(3,0) = -D3DXVec3Dot(&m_vXaxis, &eye);

	m_mViewMatrix(0,1) = m_vYaxis.x;
	m_mViewMatrix(1,1) = m_vYaxis.y;
	m_mViewMatrix(2,1) = m_vYaxis.z;
	m_mViewMatrix(3,1) = -D3DXVec3Dot(&m_vYaxis, &eye);

	m_mViewMatrix(0,2) = m_vZaxis.x;
	m_mViewMatrix(1,2) = m_vZaxis.y;
	m_mViewMatrix(2,2) = m_vZaxis.z;
	m_mViewMatrix(3,2) = -D3DXVec3Dot(&m_vZaxis, &eye);

	D3DXQuaternionRotationMatrix(&m_qOrientation, &m_mViewMatrix);
	
	D3DXVECTOR3 offset = m_vTarget - m_vEyePt;

	m_foffsetDistance = D3DXVec3Length(&offset);

}

void CCamera::Rotate(float fheadingDegrees, float fpitchDegrees)
{
	m_fheadingDegrees = -fheadingDegrees;
	m_fpitchDegrees = -fpitchDegrees;
}

void CCamera::Update(float fDelatTime)
{
	UpdateOrientation(fDelatTime);

	if(m_bIsSpringEnable)
		UpdateViewMatrix(fDelatTime);
	else
		UpdateViewMatrix();
}

void CCamera::UpdateOrientation(float fDeltaTime)
{
	 m_fpitchDegrees *= fDeltaTime;
	 m_fheadingDegrees *= fDeltaTime;

	 float heading = D3DXToRadian(m_fheadingDegrees);
	 float pitch = D3DXToRadian(m_fpitchDegrees);

	 D3DXQUATERNION rot;

	 if(heading != 0.0f)
	 {
		D3DXQuaternionRotationAxis(&rot, &m_vTargetYaxis, heading);
		D3DXQuaternionMultiply(&m_qOrientation, &rot, &m_qOrientation);
	 }
	 
	 if(pitch != 0.0f)
	 {
		 D3DXQuaternionRotationAxis(&rot, &WORLD_XAXIS, pitch);
		 D3DXQuaternionMultiply(&m_qOrientation, &rot, &m_qOrientation);
	 }
}

void CCamera::UpdateViewMatrix()
{
	D3DXQuaternionNormalize(&m_qOrientation, &m_qOrientation);
	D3DXMatrixRotationQuaternion(&m_mViewMatrix, &m_qOrientation);

	m_vXaxis = D3DXVECTOR3(m_mViewMatrix(0,0), m_mViewMatrix(1,0), m_mViewMatrix(2,0));
	m_vYaxis = D3DXVECTOR3(m_mViewMatrix(0,1), m_mViewMatrix(1,1), m_mViewMatrix(2,1));
	m_vZaxis = D3DXVECTOR3(m_mViewMatrix(0,2), m_mViewMatrix(1,2), m_mViewMatrix(2,2));

	m_vEyePt = m_vTarget + m_vZaxis * -m_foffsetDistance;

	m_mViewMatrix(3,0) = -D3DXVec3Dot(&m_vXaxis, &m_vEyePt);
	m_mViewMatrix(3,1) = -D3DXVec3Dot(&m_vYaxis, &m_vEyePt);
	m_mViewMatrix(3,2) = -D3DXVec3Dot(&m_vZaxis, &m_vEyePt);
}

void CCamera::UpdateViewMatrix(float fDeltaTime)
{
	D3DXQuaternionNormalize(&m_qOrientation, &m_qOrientation);
	D3DXMatrixRotationQuaternion(&m_mViewMatrix, &m_qOrientation);

	m_vXaxis = D3DXVECTOR3(m_mViewMatrix(0,0), m_mViewMatrix(1,0), m_mViewMatrix(2,0));
	m_vYaxis = D3DXVECTOR3(m_mViewMatrix(0,1), m_mViewMatrix(1,1), m_mViewMatrix(2,1));
	m_vZaxis = D3DXVECTOR3(m_mViewMatrix(0,2), m_mViewMatrix(1,2), m_mViewMatrix(2,2));

	// Calculate the new camera position. The 'idealPosition' is where the
	// camera should be position. The camera should be positioned directly
	// behind the target at the required offset distance. What we're doing here
	// is rather than have the camera immediately snap to the 'idealPosition'
	// we slowly move the camera towards the 'idealPosition' using a spring
	// system.
	//
	D3DXVECTOR3 idealPosition = m_vTarget + m_vZaxis * -m_foffsetDistance;
	D3DXVECTOR3 displacement = m_vEyePt - idealPosition;
	D3DXVECTOR3 springAcceleration = (-m_fSpringConstant * displacement) - (m_fDampingCostant * m_vVelocity);

	m_vVelocity += springAcceleration * fDeltaTime;
	m_vEyePt += m_vVelocity * fDeltaTime;

	// The view matrix is always relative to the camera's current position
	// 'm_eye'. Since a spring system is being used here 'm_eye' will be
	// relative to 'desiredPosition'. When the camera is no longer being moved
	// 'm_eye' will become the same as 'desiredPosition'. The local x, y, and
	// z axes that were extracted from the camera's orientation 'm_orienation'
	// is correct for the 'desiredPosition' only. We need to recompute these
	// axes so that they're relative to 'm_eye'. Once that's done we can use
	// those axes to reconstruct the view matrix.
	m_vZaxis = m_vTarget - m_vEyePt;
	D3DXVec3Normalize(&m_vZaxis, &m_vZaxis);

	D3DXVec3Cross(&m_vXaxis, &m_vTargetYaxis, &m_vZaxis);
	D3DXVec3Normalize(&m_vXaxis, &m_vXaxis);

	D3DXVec3Cross(&m_vYaxis, &m_vZaxis, &m_vXaxis);
	D3DXVec3Normalize(&m_vYaxis, &m_vYaxis);

	D3DXMatrixIdentity(&m_mViewMatrix);

	m_mViewMatrix(0,0) = m_vXaxis.x;
	m_mViewMatrix(1,0) = m_vXaxis.y;
	m_mViewMatrix(2,0) = m_vXaxis.z;
	m_mViewMatrix(3,0) = -D3DXVec3Dot(&m_vXaxis, &m_vEyePt);

	m_mViewMatrix(0,1) = m_vYaxis.x;
	m_mViewMatrix(1,1) = m_vYaxis.y;
	m_mViewMatrix(2,1) = m_vYaxis.z;
	m_mViewMatrix(3,1) = -D3DXVec3Dot(&m_vYaxis, &m_vEyePt);

	m_mViewMatrix(0,2) = m_vZaxis.x;
	m_mViewMatrix(1,2) = m_vZaxis.y;
	m_mViewMatrix(2,2) = m_vZaxis.z;
	m_mViewMatrix(3,2) = -D3DXVec3Dot(&m_vZaxis, &m_vEyePt);
}

void CCamera::SetSpringConstant(float fspringConstant)
{
	// We're using a critically damped spring system where the damping ratio
	// is equal to one.
	//
	// damping ratio = m_dampingConstant / (2.0f * sqrtf(m_springConstant))
	m_fSpringConstant = fspringConstant;
	m_fDampingCostant = 2.0f * sqrtf(fspringConstant);
}

///////////////////////////////////////////////
//Accessors
///////////////////////////////////////////////
const D3DXMATRIX &CCamera::GetProjectionMatrix(void) const
{
	return m_mProjectionMatrix;
}

D3DXMATRIX CCamera::GetViewProjectionMatrix(void)
{
	return GetViewMatrix() * GetProjectionMatrix();
}

const D3DXMATRIX &CCamera::GetViewMatrix(void) const
{
	return m_mViewMatrix;
}

const D3DXQUATERNION &CCamera::GetOrientation(void) const
{
	return m_qOrientation;
}

const D3DXVECTOR3 &CCamera::GetViewPosition(void) const
{
	return m_vEyePt;
}

const D3DXVECTOR3 &CCamera::GetViewDirection(void) const
{
	return m_vZaxis;
}
const D3DXVECTOR3 &CCamera::GetViewXAxis(void) const
{
	return m_vXaxis;
}

const D3DXVECTOR3 &CCamera::GetViewYAxis(void) const
{
	return m_vYaxis;
}

const D3DXVECTOR3 &CCamera::GetViewZAxis(void) const
{
	return m_vZaxis;
}

float CCamera::GetDampingConstant() const
{
	return m_fDampingCostant;
}

float CCamera::GetOffsetDistance() const
{
	return m_foffsetDistance;
}

float CCamera::GetSpringconstant() const
{
	return m_fSpringConstant;
}

bool CCamera::IsSpringSystemEnabled() const
{
	return m_bIsSpringEnable;
}


////////////////////////////////////
//Local Transform Functions
////////////////////////////////////
void CCamera::ViewRotateLocalX(float fAngle)
{
	D3DXMATRIX  mRotation;
	D3DXVECTOR3 vLocalX(m_mViewMatrix._11, m_mViewMatrix._12, m_mViewMatrix._13);
	D3DXMatrixRotationAxis(&mRotation, &vLocalX, fAngle);

	D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
		this->m_mViewMatrix._43, this->m_mViewMatrix._44);
	this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 

	D3DXMatrixMultiply(&m_mViewMatrix, &m_mViewMatrix, &mRotation);
	this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
	this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}

void CCamera::ViewRotateLocalY(float fAngle)
{
	D3DXMATRIX  mRotation;
	D3DXVECTOR3 vLocalY(m_mViewMatrix._21, m_mViewMatrix._22, m_mViewMatrix._23);
	D3DXMatrixRotationAxis(&mRotation, &vLocalY, fAngle);

	D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
		this->m_mViewMatrix._43, this->m_mViewMatrix._44);
	this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 

	D3DXMatrixMultiply(&m_mViewMatrix, &m_mViewMatrix, &mRotation);
	this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
	this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}

void CCamera::ViewRotateLocalZ(float fAngle)
{
	D3DXMATRIX  mRotation;
	D3DXVECTOR3 vLocalZ(m_mViewMatrix._31, m_mViewMatrix._32, m_mViewMatrix._33);
	D3DXMatrixRotationAxis(&mRotation, &vLocalZ, fAngle);

	D3DXVECTOR4 Position(this->m_mViewMatrix._41, this->m_mViewMatrix._42, 
		this->m_mViewMatrix._43, this->m_mViewMatrix._44);
	this->m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 

	D3DXMatrixMultiply(&m_mViewMatrix, &m_mViewMatrix, &mRotation);
	this->m_mViewMatrix._41 = Position.x; this->m_mViewMatrix._42 = Position.y;
	this->m_mViewMatrix._43 = Position.z; this->m_mViewMatrix._44 = Position.w;
}

void CCamera::ViewTranslateLocal(D3DXVECTOR3 vAxis, bool bFPS)
{
	if (bFPS)
	{
		ViewTranslateLocalX(vAxis.x, bFPS);
		ViewTranslateLocalY(vAxis.y, bFPS);
		ViewTranslateLocalZ(vAxis.z, bFPS);
		return;
	}

	ViewTranslateLocalX(vAxis.x);
	ViewTranslateLocalY(vAxis.y);
	ViewTranslateLocalZ(vAxis.z);
}

void CCamera::ViewTranslateLocalX(float fScale, bool bFPS)
{
	if (bFPS)
	{
		D3DXVECTOR3 vTranslate((this->m_mViewMatrix._11),
			0.0f,
			(this->m_mViewMatrix._13));
		D3DXVec3Normalize(&vTranslate,
			&vTranslate);

		this->m_mViewMatrix._41 += vTranslate.x * fScale;
		this->m_mViewMatrix._42 += vTranslate.y * fScale;
		this->m_mViewMatrix._43 += vTranslate.z * fScale;

		return;
	}
	this->m_mViewMatrix._41 += (this->m_mViewMatrix._11 * fScale);
	this->m_mViewMatrix._42 += (this->m_mViewMatrix._12 * fScale);
	this->m_mViewMatrix._43 += (this->m_mViewMatrix._13 * fScale);
}

void CCamera::ViewTranslateLocalY(float fScale, bool bFPS)
{
	if (bFPS)
	{
		D3DXVECTOR3 vTranslate((this->m_mViewMatrix._21),
			0.0f,
			(this->m_mViewMatrix._23));
		D3DXVec3Normalize(&vTranslate,
			&vTranslate);

		this->m_mViewMatrix._41 += vTranslate.x * fScale;
		this->m_mViewMatrix._42 += vTranslate.y * fScale;
		this->m_mViewMatrix._43 += vTranslate.z * fScale;

		return;
	}
	this->m_mViewMatrix._41 += (this->m_mViewMatrix._21 * fScale);
	this->m_mViewMatrix._42 += (this->m_mViewMatrix._22 * fScale);
	this->m_mViewMatrix._43 += (this->m_mViewMatrix._23 * fScale);
}

void CCamera::ViewTranslateLocalZ(float fScale, bool bFPS)
{
	if (bFPS)
	{
		D3DXVECTOR3 vTranslate((this->m_mViewMatrix._31),
			0.0f,
			(this->m_mViewMatrix._33));
		D3DXVec3Normalize(&vTranslate,
			&vTranslate);

		this->m_mViewMatrix._41 += vTranslate.x * fScale;
		this->m_mViewMatrix._42 += vTranslate.y * fScale;
		this->m_mViewMatrix._43 += vTranslate.z * fScale;

		return;
	}
	this->m_mViewMatrix._41 += (this->m_mViewMatrix._31 * fScale);
	this->m_mViewMatrix._42 += (this->m_mViewMatrix._32 * fScale);
	this->m_mViewMatrix._43 += (this->m_mViewMatrix._33 * fScale);
}

void CCamera::MoveCameraFoward(float fScale)
{
	ViewTranslateLocalZ(fScale);
}
void CCamera::MoveCameraBackward(float fScale)
{
	ViewTranslateLocalZ(-(fScale));
}

void CCamera::RotateCameraLeft(float fAngle)
{
	ViewRotateLocalY(-(fAngle));
}

void CCamera::RotateCameraRight(float fAngle)
{
	ViewRotateLocalY(fAngle);
}

void CCamera::MoveCameraUp(float fScale)
{
	ViewTranslateLocalY(fScale);
}

void CCamera::MoveCameraDown(float fScale)
{
	ViewTranslateLocalY(-(fScale));
}

