///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCamera.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	04/07/2011
//
//  Last Changed	:	04/11/2011
//
//  Purpose			:	Camera class that controls the camera movement and view
///////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "dxutil.h"


class CCamera
{
private:
	CCamera(void);
	CCamera(const CCamera &);
	CCamera &operator=(const CCamera &);

	static const D3DXVECTOR3 WORLD_XAXIS;
	static const D3DXVECTOR3 WORLD_YAXIS;
	static const D3DXVECTOR3 WORLD_ZAXIS;
	static const float DEFAULT_FOVX;
	static const float DEFAULT_ZFAR;
	static const float DEFAULT_ZNEAR;
	static const float DEFAULT_SPRING_CONSTANT;
	static const float DEFAULT_DAMPING_CONSTANT;

	D3DXMATRIX			m_mViewMatrix;
	D3DXMATRIX			m_mProjectionMatrix;
	D3DXQUATERNION		m_qOrientation;

	D3DXVECTOR3			m_vTarget;
	D3DXVECTOR3			m_vTargetYaxis;
	D3DXVECTOR3         m_vEyePt;
	D3DXVECTOR3			m_vXaxis;
	D3DXVECTOR3			m_vYaxis;
	D3DXVECTOR3			m_vZaxis;
	D3DXVECTOR3			m_vVelocity;

	float				m_fFieldofVision;
	float				m_fZnearplane;
	float				m_fZfarplane;
	float				m_fSpringConstant;
	float				m_fDampingCostant;
	float				m_foffsetDistance;
	float				m_fheadingDegrees;
	float				m_fpitchDegrees;
	bool				m_bIsSpringEnable;


public:
	~CCamera(void);
	static D3DXVECTOR3 s_vWorldUp;
	static CCamera* GetInstance();

	//Projection Matrix Functions
	void LookAtTarget(const D3DXVECTOR3 &_target);
	void LookAtTarget(const D3DXVECTOR3 &_eye, const D3DXVECTOR3 &_target, const D3DXVECTOR3 &_up);
	void Initialize();
	void BuildPerspective(float fFieldOfView, float fAspect, float fZNear, float fZFar);
	//void NormalizeViewMatrix(void);
	void Update(float fDelatTime);
	void Rotate(float fheadingDegrees, float fpitchDegrees);

	//Accessors
	const D3DXQUATERNION &GetOrientation() const;
	const D3DXMATRIX &GetProjectionMatrix() const;
	const D3DXMATRIX &GetViewMatrix() const;
	const D3DXVECTOR3 &GetViewPosition() const;
	const D3DXVECTOR3 &GetViewDirection() const;
	const D3DXVECTOR3 &GetViewXAxis() const;
	const D3DXVECTOR3 &GetViewYAxis() const;
	const D3DXVECTOR3 &GetViewZAxis() const;
		D3DXMATRIX GetViewProjectionMatrix();

	float GetDampingConstant() const;
	float GetOffsetDistance() const;
	float GetSpringconstant() const;
	bool IsSpringSystemEnabled() const;

	//Mutators
	void EnableSpringSystem(bool benableSpring) 
	{ 
		m_bIsSpringEnable = benableSpring; 
	}
	void SetOffsetDistance(float foffsetDistance ) 
	{ 
		m_foffsetDistance = foffsetDistance; 
	}
	void SetSpringConstant(float fspringConstant);
	//Local Transform Functions
	void MoveCameraFoward(float fScale);
	void MoveCameraBackward(float fScale);
	void RotateCameraLeft(float fAngle);
	void RotateCameraRight(float fAngle);
	void MoveCameraUp(float fScale);
	void MoveCameraDown(float fScale);

private:
	void ViewRotateLocalX(float fAngle);
	void ViewRotateLocalY(float fAngle);
	void ViewRotateLocalZ(float fAngle);
	void ViewTranslateLocal(D3DXVECTOR3 vAxis, bool bFPS = false);
	void ViewTranslateLocalX(float fScale, bool bFPS = false);
	void ViewTranslateLocalY(float fScale, bool bFPS = false);
	void ViewTranslateLocalZ(float fScale, bool bFPS = false);

	void UpdateOrientation(float fDeltaTime);
	void UpdateViewMatrix();
	void UpdateViewMatrix(float fDeltaTime);

};
#endif //_CAMERA_H_