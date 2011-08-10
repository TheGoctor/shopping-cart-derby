///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCamera.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	04/07/2011
//
//  Last Changed	:	07/26/2011
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

	queue<D3DXMATRIX>	m_Matrices;					
	size_t				m_iQueueSize;				

	D3DXMATRIX			m_mViewMatrix;				
	D3DXMATRIX			m_mProjectionMatrix;		
	D3DXMATRIX			m_mViewProjectionMatrix;
	D3DXMATRIX			m_mWorld;
	D3DXVECTOR3			m_vPostion;
	D3DXVECTOR3			m_vEyePos;
	D3DXVECTOR3			m_vLookAtPos;
	CObject*			m_cFrustum;
	CCollideable*		m_cFrustumBounds;
	TSphere				m_tCameraSphere;	
	D3DXVECTOR3			m_collisiontransform;
	CFrame					m_frame;
	CFrame					*m_pTarget;
	CFrame					m_Target;
	static D3DXVECTOR3		s_vWorldUp;

public:
	//Constructor
	CCamera(void);
	//Copy constructor
	CCamera(const CCamera &);
	//Assignment operator
	CCamera &operator=(const CCamera &);
	//Destructor
	~CCamera(void);
	
	///////////////////////////////////////////////
	//Accessors
	////////////////////////////////////////////
	D3DXMATRIX &GetProjectionMatrix() 
	{
		return this->m_mProjectionMatrix;
	}
	const D3DXMATRIX &GetViewProjectionMatrix()
	{
		return this->m_mViewProjectionMatrix;
	}
	CFrame *GetTarget(void)
	{
		return m_pTarget;
	}
	CFrame &GetFrame(void)
	{
		return m_frame;
	}
	CFrame &GetTarget1(void)
	{
		return m_Target;
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
	D3DXMATRIX &GetViewMatrix();
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
	////////////////////////////////////////////////////////////////////////////////
	// Function: "BuildPerspective"
	//
	// Return: void
	//
	// Parameters: float -	the field of view value
	//			float	-	the Aspect of the camera
	//			float	-	the near plane
	//			float	-	the far plane
	//			
	// Purpose: Builds the projection matrix for the camera
	//    
	////////////////////////////////////////////////////////////////////////////////
	void BuildPerspective(float fFieldOfView, float fAspect, 
		float fZNear, float fZFar);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "Update"
	//
	// Return: void
	//
	// Parameters: float - delta time
	//
	// Purpose:  updates the cameras position and does the look at 
	//    
	////////////////////////////////////////////////////////////////////////////////
	void Update(float fDeltaTime);
};
#endif //_CAMERA_H_