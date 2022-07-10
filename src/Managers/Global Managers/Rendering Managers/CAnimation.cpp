////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimation.cpp
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates Animation Data and Functionality
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CAnimation.h"
#include "DXMesh.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////
TKeyFrame::TKeyFrame(void) : m_fKeyTime(0.0f), m_cFrame()
{
}
CAnimation::CAnimation(void) : m_fDuration(0.0f), m_nNumBones(0), m_cBones()
{
}
Interpolator::Interpolator(void) : m_pAnimation(NULL), m_pMesh(NULL),
								   m_fCurrentTime(0.0f)
{
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTORS
////////////////////////////////////////////////////////////////////////////////
TBone::~TBone(void)
{
	//// Clean up Key Frames
	//m_cKeyFrames.clear();

	//// Clean up Child Indices
	//m_cChildrenIdxs.clear();
}
CAnimation::~CAnimation(void)
{
	// Clean up Bones
	//for(int bone = 0; bone < m_nNumBones; ++bone)
	//{
	//	m_cBones[bone].m_cChildrenIdxs.clear();
	//	
	//	// Clean up Key Frames
	//	for(int frame = 0; frame < m_cBones[bone].m_nNumKeyFrames; ++frame)
	//	{
	//		m_cBones[bone].m_cKeyFrames.clear();
	//	}
	//}
	//m_cBones.clear();
}
Interpolator::~Interpolator(void)
{
	// Clean up Frames
	//m_cBoneFrames.clear();
}


// Add Bone
void CAnimation::AddBone(TBone tBone)
{
	m_cBones.push_back(tBone);
}

// Setup Tween Times
//void CAnimation::SetupTweenTimes(void)
//{
	//m_nNumBones = m_cBones.size();

	//// Loop through each Bone
	//for( int bone = 0; bone < m_nNumBones; bone++)
	//{
	//	// Get Bone Pointer
	//	TBone* pBone = &m_cBones[bone];

	//	// Loop through each KeyFrame
	//	pBone->m_nNumKeyFrames = pBone->m_cKeyFrames.size();
	//	for( int frame = 0; frame < pBone->m_nNumKeyFrames; frame++)
	//	{
	//		// Get KeyFrame Pointer
	//		TKeyFrame* pKeyFrame = &pBone->m_cKeyFrames[frame];

	//		if(frame == pBone->m_nNumKeyFrames - 1)
	//		{
	//			// set the last keyframes tween time
	//			pKeyFrame->m_fTweenTime = m_fDuration - pKeyFrame->m_fKeyTime;
	//		}
	//		else
	//		{
	//			// set the difference of the 2 frames
	//			pKeyFrame->m_fTweenTime = pBone->m_cKeyFrames[frame+1].m_fKeyTime - pKeyFrame->m_fKeyTime;
	//		}
	//	}
	//}
//}

// Interpolate
void Interpolator::Process(void)
{
	while(m_fCurrentTime  < 0.0f)
		m_fCurrentTime += m_pAnimation->GetDuration();

	if(m_pAnimation->GetLooping())
	{
		while(m_fCurrentTime >= m_pAnimation->GetDuration())
			m_fCurrentTime -= m_pAnimation->GetDuration();
	}
	else
	{
		// If the time is up, stop animating
		if(m_fCurrentTime >= m_pAnimation->GetDuration())
		{
			m_pAnimation = NULL;
			return;
		}
	}

	int nNumBones = m_pAnimation->GetNumBones();

	// Loop Through Each Bone
	//m_cBoneFrames.resize(nNumBones);
	for( int bone = 0; bone < nNumBones; ++bone)
	{
		TBone* pCurrBone = &m_pAnimation->GetBones()[bone];
		// Find Current Time by Index
		int nCurrFrame = -1;
		int nNextFrame = 0;

		// Get current frame
		int nNumFrames = pCurrBone->m_nNumKeyFrames;

		// Loop through each Key Frame
		for(int frame = 1; frame < nNumFrames; ++frame)
		{
			// Move to the Next Frame
			if(m_fCurrentTime < pCurrBone->m_cKeyFrames[frame].m_fKeyTime)
			{
				nCurrFrame = frame - 1;
				nNextFrame = frame;
				break;
			}
		}

		if(nCurrFrame == -1)
			return; //nCurrFrame = nNextFrame = nNumFrames - 1;

		float fCurrTime = pCurrBone->m_cKeyFrames[nCurrFrame].m_fKeyTime;
		float fNextTime = pCurrBone->m_cKeyFrames[nNextFrame].m_fKeyTime;

		TKeyFrame* pCurrFrame = &pCurrBone->m_cKeyFrames[nCurrFrame];

		// Find how far into the current frame the current time is
		float fTotalFrameTime, fCurrFrameTime;
		float fLambda;

		if(nNextFrame != 0)
		{
			fTotalFrameTime = fNextTime - fCurrTime;
		}
		else
		{
			fTotalFrameTime = m_pAnimation->m_fDuration - fCurrTime;
		}

		fCurrFrameTime = m_fCurrentTime - fCurrTime;
		fLambda = fCurrFrameTime / fTotalFrameTime;

		// Interpolate
		m_cBoneFrames[bone].GetLocalMatrix() = animInterpolate(pCurrFrame->m_cFrame.GetLocalMatrix(),
			pCurrBone->m_cKeyFrames[nNextFrame].m_cFrame.GetLocalMatrix(), fLambda);
	}

	// Update Frames' Matrices
	m_cBoneFrames[0].Update();
}

// Set New Animation
void Interpolator::SetAnimation(CAnimation *newAnim)
{
	// return to allow current animation to play
	if(newAnim == NULL || newAnim == m_pAnimation)
		return;

	// Clone the Matrices
	m_pAnimation = newAnim;
	//m_cBoneFrames.resize(m_pAnimation->GetNumBones());
	for(unsigned int frame = 0; frame < m_pAnimation->GetNumBones(); ++frame)
	{
		m_cBoneFrames[frame].CloneLocal(m_pAnimation->GetBones()[frame].m_cKeyFrames[0].m_cFrame);
	}

	SetTime(0.0f);
}

// Set Mesh
void Interpolator::SetMesh(DXMesh *newMesh)
{
	m_pMesh = newMesh;
}

// Make Connections
void Interpolator::BuildHiearchy(void)
{
	unsigned int nNumBones = m_pMesh->GetBones().size();
	for(unsigned int bone = 0; bone < nNumBones; ++bone)
	{
		for(int child = 0; child < m_pMesh->GetBones()[bone].m_nNumChildren; ++child)
		{
			// Connect to Children
			//if(bone != m_pMesh->GetBones()[bone].m_cChildrenIdxs[child])
	//	{
				m_cBoneFrames[bone].AddChildFrame(
					&m_cBoneFrames[m_pMesh->GetBones()[bone].m_cChildrenIdxs[child]]);
	//}
		}
	}

	//m_vBoneFrames[0].Update();
}

// Interpolate
D3DXMATRIX  Interpolator::animInterpolate(D3DXMATRIX a, D3DXMATRIX b, float fPercent)
{
	// Get the Mag of Pos Vecs
	D3DXVECTOR3 vPosA = D3DXVECTOR3(a._41, a._42, a._43);
	D3DXVECTOR3 vPosB = D3DXVECTOR3(b._41, b._42, b._43);
	float fMagA = D3DXVec3Length(&vPosA);
	float fMagB = D3DXVec3Length(&vPosB);

	// Normalize
	D3DXVec3Normalize(&vPosA, &vPosA);
	D3DXVec3Normalize(&vPosB, &vPosB);

	// Get New Pos
	D3DXVECTOR3 vNewPos;
	D3DXVec3Lerp(&vNewPos, &vPosA, &vPosB, fPercent);

	// Scale New Pos
	float fNewMag =  ((fMagB - fMagA) * (fPercent)) + fMagA;
	vNewPos *= fNewMag;

	// Zero out Pos
	a._41 = 0.0f;
	a._42 = 0.0f;
	a._43 = 0.0f;
	b._41 = 0.0f;
	b._42 = 0.0f;
	b._43 = 0.0f;

	// Slerp Between Rotation Values
	D3DXQUATERNION qA, qB, qC;
	D3DXQuaternionRotationMatrix(&qA, &a);
	D3DXQuaternionRotationMatrix(&qB, &b);

	D3DXQuaternionSlerp(&qC, &qA, &qB, fPercent);

	D3DXMATRIX result;
	D3DXMatrixRotationQuaternion(&result, &qC);

	// Apply New Position
	result._41 = vNewPos.x;
	result._42 = vNewPos.y;
	result._43 = vNewPos.z;

	return result;
}

// NON-FUNCTIONAL
D3DXMATRIX Interpolator::interpolateWithScale(D3DXMATRIX a, D3DXMATRIX b, float fPercent)
{
	// Get the Mag of Pos Vecs
	D3DXVECTOR3 vPosA = D3DXVECTOR3(a._41, a._42, a._43);
	D3DXVECTOR3 vPosB = D3DXVECTOR3(b._41, b._42, b._43);
	float fMagA = D3DXVec3Length(&vPosA);
	float fMagB = D3DXVec3Length(&vPosB);

	// Normalize
	D3DXVec3Normalize(&vPosA, &vPosA);
	D3DXVec3Normalize(&vPosB, &vPosB);

	// Get New Pos
	D3DXVECTOR3 vNewPos;
	D3DXVec3Lerp(&vNewPos, &vPosA, &vPosB, fPercent);

	// Scale New Pos
	float fNewMag =  ((fMagB - fMagA) * (fPercent)) + fMagA;
	vNewPos *= fNewMag;

	// Zero out Pos
	a._41 = 0.0f;
	a._42 = 0.0f;
	a._43 = 0.0f;
	b._41 = 0.0f;
	b._42 = 0.0f;
	b._43 = 0.0f;

	//
	D3DXQUATERNION qA, qB, qC;
	D3DXQuaternionRotationMatrix(&qA, &a);
	D3DXQuaternionRotationMatrix(&qB, &b);

	D3DXQuaternionSlerp(&qC, &qA, &qB, fPercent);

	D3DXMATRIX result;
	D3DXMatrixRotationQuaternion(&result, &qC);

	result._41 = vNewPos.x;
	result._42 = vNewPos.y;
	result._43 = vNewPos.z;

	return result;
}