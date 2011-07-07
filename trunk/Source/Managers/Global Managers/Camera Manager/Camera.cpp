///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCamera.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	04/07/2011
//
//	Last Changed	:	05/09/2011
//
//  Purpose			:	Camera class that controls the camera movement and view
///////////////////////////////////////////////////////////////////////////////
#include "Camera.h"
#include "..\Object Manager\CObjectManager.h"
#include "..\..\Component Managers\CCollisionManager.h"
#include "..\Sound Manager\CWwiseSoundManager.h"
D3DXVECTOR3 CCamera::s_vWorldUp(0.0f, 1.0f, 0.0f);

CCamera::CCamera(void)
{	
	m_pTarget = NULL;
	m_iQueueSize = 4;
	m_pSound = CWwiseSoundManager::GetInstance();
	D3DXMatrixIdentity(&m_mViewMatrix);
	D3DXMatrixIdentity(&m_mProjectionMatrix);
	D3DXMatrixIdentity(&m_mViewProjectionMatrix);

//	m_cFrustum = CObjectManager::GetInstance()->CreateObject("Frustum1", 0.0f, 0.0f, 0.0f);
//	TPlane frustumPlane;
//	m_cFrustumBounds = CCollisionManager::GetInstance()->CreateCollideableComponent(
//		m_cFrustum, false, false, OBJFRUSTUM);
//	m_cFrustumBounds->SetBVType(BFRUSTUM);

	//NOTE: leave this here for now, once i've got the frustum hubbabaloo up, we're good
//	m_cFrustumBounds->BuildFrustum(m_mViewProjectionMatrix);

}

CCamera::~CCamera(void)
{
}


D3DXMATRIX &CCamera::GetViewMatrix()
{
	D3DXMatrixInverse(&m_mViewMatrix, 0, &m_mViewMatrix);
	m_mViewMatrix._11 *= -1.0f;
	m_mViewMatrix._12 *= -1.0f;
	m_mViewMatrix._13 *= -1.0f;
	m_mViewMatrix._14 *= -1.0f;
	D3DXMatrixInverse(&m_mViewMatrix, 0, &m_mViewMatrix);
	return m_mViewMatrix;
}

void CCamera::ResetCamera()
{
	//D3DXMatrixIdentity(&m_mViewMatrix);
	//D3DXMatrixIdentity(&m_mProjectionMatrix);
	//D3DXMatrixIdentity(&m_mViewProjectionMatrix);
}



void CCamera::Update(float /*fDeltaTime*/)
{

	D3DXMATRIX cameraMatrix;

	m_vEyePos = D3DXVECTOR3(m_frame.GetWorldMatrix()._41, 
		m_frame.GetWorldMatrix()._42, 
		m_frame.GetWorldMatrix()._43);

	m_vLookAtPos.x = m_pTarget->GetWorldMatrix()._41;
	m_vLookAtPos.y = m_pTarget->GetWorldMatrix()._42;
	m_vLookAtPos.z = m_pTarget->GetWorldMatrix()._43;


	////////////////HACK: camera collision////////////////////////////////////
	//okay motherfucker, this is the new shit, it's gonna take care of everything!
	//WOAH-OH-OH-AH-AH-AH-AAAA-HA-AH-AH-AH   -Raymoney-mo'fuckahs!

	CCollisionManager::GetInstance()->TestObjAgainstWall(m_vEyePos);
	
	D3DXMatrixLookAtLH(&cameraMatrix, &m_vEyePos, &m_vLookAtPos, &s_vWorldUp);

	m_Matrices.push(cameraMatrix);

	if (m_Matrices.size() >= m_iQueueSize)
	{
		m_mViewMatrix = m_Matrices.front();
		m_Matrices.pop();
	}

	D3DXVECTOR3 forwardvec;
	forwardvec.x = m_pTarget->GetWorldMatrix()._31;
	forwardvec.y = m_pTarget->GetWorldMatrix()._32;
	forwardvec.z = m_pTarget->GetWorldMatrix()._33;
	
	D3DXVECTOR3 posit;
	posit.x = m_pTarget->GetWorldMatrix()._41;
	posit.y	= m_pTarget->GetWorldMatrix()._42;
	posit.z	= m_pTarget->GetWorldMatrix()._43 + 4.0f;
	
	m_pSound->SetListenerPosition(posit, forwardvec, 1.0f, 0);


	D3DXMatrixMultiply(&m_mViewProjectionMatrix, &GetViewMatrix(), 
		&GetProjectionMatrix());
}

void CCamera::BuildPerspective(float fFieldOfView, float fAspectRatio, 
							   float fZNear, float fZFar)
{
	D3DXMatrixPerspectiveFovLH(&this->m_mProjectionMatrix, fFieldOfView, 
		fAspectRatio, fZNear, fZFar);
}

const D3DXVECTOR3 &CCamera::GetViewPosition()
{
	D3DXVECTOR3 vec = D3DXVECTOR3(m_mViewMatrix._41, m_mViewMatrix._42, 
		m_mViewMatrix._43);
	m_vPostion = vec;
	return m_vPostion;
}

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