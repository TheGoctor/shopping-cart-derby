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

CCamera *CCamera::GetInstance()
{
	static CCamera cCameraInstance;
	return &cCameraInstance;
}

const D3DXMATRIX &CCamera::GetViewMatrix()
{
	return m_mViewMatrix;
}


void CCamera::Update(float fDeltaTime)
{

	m_vEyePos = D3DXVECTOR3(m_frame.GetWorldMatrix()._41, 
		m_frame.GetWorldMatrix()._42, 
		m_frame.GetWorldMatrix()._43);


	m_vLookAtPos.x = m_pTarget->GetWorldMatrix()._41;
	m_vLookAtPos.y = m_pTarget->GetWorldMatrix()._42;
	m_vLookAtPos.z = m_pTarget->GetWorldMatrix()._43;

	D3DXMATRIX cameraMatrix;

	D3DXMatrixLookAtLH(&cameraMatrix, &m_vEyePos, &m_vLookAtPos, &s_vWorldUp);

	m_Matrices.push(cameraMatrix);

	if (m_Matrices.size() >= m_iQueueSize)
	{
		m_mViewMatrix = m_Matrices.front();
		m_Matrices.pop();
	}

	D3DXMatrixMultiply(&m_mViewProjectionMatrix, &GetViewMatrix(), 
		&GetProjectionMatrix());

	CWwiseSoundManager::GetInstance()->SetListener(m_vEyePos, 0.5f, 0);

//NOTE: leave this here for now, once i've got the frustum hubbabaloo up, we're good
//	m_cFrustumBounds->BuildFrustum(m_mViewProjectionMatrix);
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