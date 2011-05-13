///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCamera.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	04/07/2011
//
//  Last Changed	:	05/04/2011
//
//	Changed By		:	HN
//
//  Purpose			:	Camera class that controls the camera movement and view
///////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "../../../CFrame.h"
#include "../../../Components/Collision/CCollideable.h"
#include <queue>
using std::queue;

class CCamera
{
private:
	CCamera(void);
	CCamera(const CCamera &);
	CCamera &operator=(const CCamera &);

	queue<D3DXMATRIX> m_Matrices;
	size_t m_iQueueSize;

	D3DXMATRIX			m_mViewMatrix;
	D3DXMATRIX			m_mProjectionMatrix;
	D3DXMATRIX			m_mViewProjectionMatrix;
	D3DXMATRIX			m_mWorld;

	D3DXVECTOR3			m_vPostion;
	D3DXVECTOR3			m_vEyePos;
	D3DXVECTOR3			m_vLookAtPos;

	CObject*			m_cFrustum;
	CCollideable*		m_cFrustumBounds;

	CFrame				m_frame;
	CFrame				*m_pTarget;

	static CCamera		*m_pCCamera;

public:
	~CCamera(void);
	CFrame					m_Target;
	static D3DXVECTOR3 s_vWorldUp;

	static CCamera* GetInstance();

	///////////////////////////////////////////////
	//Accessors
	////////////////////////////////////////////
	const D3DXMATRIX &GetProjectionMatrix() 
	{
		return m_mProjectionMatrix;
	}
	const D3DXMATRIX &GetViewProjectionMatrix()
	{
		return m_mViewProjectionMatrix;
	}

	const D3DXMATRIX &GetViewMatrix();

	CFrame *GetTarget(void)
	{
		return m_pTarget;
	}

	CFrame &GetFrame(void)
	{
		return m_frame;
	}

	D3DXVECTOR3 GetEyePos()
	{
		return m_vEyePos;
	}

	D3DXVECTOR3 GetLookAtPos()
	{
		return m_vLookAtPos;
	}
	const D3DXVECTOR3 &GetViewPosition();

	/////////////////////////////////////////////////////
	//Mutators/
	////////////////////////////////////////////////////
	void SetTarget(CFrame *pTarget)
	{
		m_pTarget = pTarget;
	}

	void SetFrameParent(CFrame *frame)
	{
		m_frame.SetParent(frame);
	}

	void SetLookAtPos(D3DXVECTOR3 lookat)
	{
		m_vLookAtPos = lookat;
	}

	void SetEyePos(D3DXVECTOR3 eyepos)
	{
		m_vEyePos = eyepos;
	}
	////////////////////////////////////////////////////////////////////////////
	//Accesors
	///////////////////////////////////////////////////////////////////////////
	void BuildPerspective(float fFieldOfView, float fAspect, 
		float fZNear, float fZFar);
	void Initialize();
	void Update(float fDeltaTime);

};
#endif //_CAMERA_H_