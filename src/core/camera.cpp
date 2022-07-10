///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCamera.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	04/07/2011
//
//	Last Changed	:	07/26/2011
//
//  Purpose			:	Camera class that controls the camera movement and view
///////////////////////////////////////////////////////////////////////////////
#include "Camera.h"
#include "..\Object Manager\CObjectManager.h"
#include "..\..\Component Managers\CCollisionManager.h"
D3DXVECTOR3 CCamera::s_vWorldUp(0.0f, 1.0f, 0.0f);

CCamera::CCamera(void)
{	
	m_pTarget = NULL;
	m_iQueueSize = 4;

	D3DXMatrixIdentity(&m_mViewMatrix);
	D3DXMatrixIdentity(&m_mProjectionMatrix);
	D3DXMatrixIdentity(&m_mViewProjectionMatrix);
}

CCamera::~CCamera(void)
{
}

scd::transform &CCamera::GetViewMatrix()
{
	D3DXMatrixInverse(&m_mViewMatrix, 0, &m_mViewMatrix);
	m_mViewMatrix._11 *= -1.0f;
	m_mViewMatrix._12 *= -1.0f;
	m_mViewMatrix._13 *= -1.0f;
	m_mViewMatrix._14 *= -1.0f;
	D3DXMatrixInverse(&m_mViewMatrix, 0, &m_mViewMatrix);
	return m_mViewMatrix;
}

void CCamera::Update(float fDeltaTime)
{
	float fTime = 0.0f;
	fTime += fDeltaTime;

	scd::transform cameraMatrix;

	m_vEyePos = D3DXVECTOR3(m_frame.GetWorldMatrix()._41, 
		m_frame.GetWorldMatrix()._42, 
		m_frame.GetWorldMatrix()._43);

	m_vLookAtPos.x = m_pTarget->GetWorldMatrix()._41;
	m_vLookAtPos.y = m_pTarget->GetWorldMatrix()._42;
	m_vLookAtPos.z = m_pTarget->GetWorldMatrix()._43;

	CCollisionManager::GetInstance()->TestObjAgainstWall( m_vEyePos, (m_vLookAtPos-m_vEyePos));

	D3DXMatrixLookAtLH(&cameraMatrix, &m_vEyePos, &m_vLookAtPos, &s_vWorldUp);

	m_Matrices.push(cameraMatrix);

	if (m_Matrices.size() >= m_iQueueSize)
	{
		m_mViewMatrix = m_Matrices.front();
		m_Matrices.pop();
	}

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
	m_vPostion = D3DXVECTOR3(m_mViewMatrix._41, m_mViewMatrix._42, 
		m_mViewMatrix._43);
	return m_vPostion;
}