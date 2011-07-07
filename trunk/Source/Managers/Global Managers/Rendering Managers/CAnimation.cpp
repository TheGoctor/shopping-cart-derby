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
TKeyFrame::TKeyFrame(void) : m_fKeyTime(0.0f), m_fTweenTime(0.0f), m_cFrame()
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
	// Clean up Key Frames
	m_cKeyFrames.clear();

	// Clean up Child Indices
	m_cChildrenIdxs.clear();
}
CAnimation::~CAnimation(void)
{
	// Clean up Bones
	for(int bone = 0; bone < m_nNumBones; ++bone)
	{
		m_cBones[bone].m_cChildrenIdxs.clear();
		
		// Clean up Key Frames
		for(int frame = 0; frame < m_cBones[bone].m_nNumKeyFrames; ++frame)
		{
			m_cBones[bone].m_cKeyFrames.clear();
		}
	}
	m_cBones.clear();
}
Interpolator::~Interpolator(void)
{
	// Clean up Frames
	m_cBoneFrames.clear();
}


// Add Bone
void CAnimation::AddBone(TBone tBone)
{
	m_cBones.push_back(tBone);
}

// Setup Tween Times
void CAnimation::SetupTweenTimes(void)
{
	m_nNumBones = m_cBones.size();

	// Loop through each Bone
	for( int bone = 0; bone < m_nNumBones; bone++)
	{
		// Get Bone Pointer
		TBone* pBone = &m_cBones[bone];

		// Loop through each KeyFrame
		pBone->m_nNumKeyFrames = pBone->m_cKeyFrames.size();
		for( int frame = 0; frame < pBone->m_nNumKeyFrames; frame++)
		{
			// Get KeyFrame Pointer
			TKeyFrame* pKeyFrame = &pBone->m_cKeyFrames[frame];

			if(frame == pBone->m_nNumKeyFrames - 1)
			{
				// set the last keyframes tween time
				pKeyFrame->m_fTweenTime = m_fDuration - pKeyFrame->m_fKeyTime;
			}
			else
			{
				// set the difference of the 2 frames
				pKeyFrame->m_fTweenTime = pBone->m_cKeyFrames[frame+1].m_fKeyTime - pKeyFrame->m_fKeyTime;
			}
		}
	}
}

// Interpolate
void Interpolator::Process(void)
{
	// Find Current Time by Index
	int currIdx = 0;
	int nextIndex = 0;

	while(m_fCurrentTime  < 0.0f)
		m_fCurrentTime += m_pAnimation->GetDuration();

	// Comment this out to prevent animations from looping
	//while(m_fCurrentTime >= m_pAnimation->GetDuration())
	//	m_fCurrentTime -= m_pAnimation->GetDuration();

	// If the time is up, stop animating
	if(m_fCurrentTime >= m_pAnimation->GetDuration())
	{
		m_pAnimation = NULL;
		return;
	}

	//m_vBoneFrames.clear();
	//m_vBoneFrames.resize(m_pAnimation->GetBones().size());
	//BuildHiearchy();

	// Loop Through Each Bone
	for( int bone = 0; bone < m_pAnimation->GetNumBones(); ++bone)
	{
		// Loop through each Key Frame
		for(int frame = 0; frame < m_pAnimation->GetBones()[bone].m_nNumKeyFrames; ++frame)
		{
			// Move to the Next Frame
			if(m_fCurrentTime < m_pAnimation->GetBones()[bone].m_cKeyFrames[frame].m_fKeyTime 
				+ m_pAnimation->GetBones()[bone].m_cKeyFrames[frame].m_fTweenTime)
			{
				currIdx = nextIndex = frame;
				++nextIndex %= m_pAnimation->GetBones()[bone].m_nNumKeyFrames;
				break;
			}
		}

		// Find Lamda
		float fPercentTime;
		if(nextIndex != 0)
		{
			fPercentTime = m_fCurrentTime - m_pAnimation->GetBones()[bone].m_cKeyFrames[currIdx].m_fKeyTime;
			fPercentTime /= (m_pAnimation->GetBones()[bone].m_cKeyFrames[nextIndex].m_fKeyTime - m_pAnimation->GetBones()[bone].m_cKeyFrames[currIdx].m_fKeyTime);
		}
		else
		{
			fPercentTime = m_fCurrentTime - m_pAnimation->GetBones()[bone].m_cKeyFrames[currIdx].m_fKeyTime;
			fPercentTime /= (m_pAnimation->GetDuration() - m_pAnimation->GetBones()[bone].m_cKeyFrames[currIdx].m_fKeyTime);
		}

		//m_vBoneFrames[bone].CloneLocal(m_pAnimation->GetBones()[bone].m_cKeyFrames[currIdx].m_cFrame);

		// Interpolate
		m_cBoneFrames[bone].GetLocalMatrix() = animInterpolate(m_pAnimation->GetBones()[bone].m_cKeyFrames[currIdx].m_cFrame.GetLocalMatrix(),
			m_pAnimation->GetBones()[bone].m_cKeyFrames[nextIndex].m_cFrame.GetLocalMatrix(), fPercentTime);
		
		//BuildHiearchy();
		//m_vBoneFrames[bone].Update();
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
	//m_vBoneFrames.resize( m_pAnimation->GetBones().size() );
	for( int frame = 0; frame < m_pAnimation->GetNumBones(); ++frame)
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
			//{
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