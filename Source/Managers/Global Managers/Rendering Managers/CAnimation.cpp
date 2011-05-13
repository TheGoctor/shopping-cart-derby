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

// Constructor
CAnimation::CAnimation(void) : m_fDuration(0.0f)
{

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
	for(unsigned int bone = 0; bone < m_nNumBones; bone++)
	{
		// Get Bone Pointer
		TBone* pBone = &m_cBones[bone];

		// Loop through each KeyFrame
		pBone->m_nNumKeyFrames = pBone->m_cKeyFrames.size();
		for(unsigned int frame = 0; frame < pBone->m_nNumKeyFrames; frame++)
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

	while(m_fCurrentTime >= m_pAnimation->GetDuration())
		m_fCurrentTime -= m_pAnimation->GetDuration();

	//m_vBoneFrames.clear();
	//m_vBoneFrames.resize(m_pAnimation->GetBones().size());

	// Loop Through Each Bone
	for(unsigned int bone = 0; bone < m_pAnimation->GetNumBones(); ++bone)
	{
		// Loop through each Key Frame
		for(unsigned int frame = 0; frame < m_pAnimation->GetBones()[bone].m_nNumKeyFrames; ++frame)
		{
			if(m_fCurrentTime < m_pAnimation->GetBones()[bone].m_cKeyFrames[frame].m_fKeyTime 
				+ m_pAnimation->GetBones()[bone].m_cKeyFrames[frame].m_fTweenTime)
			{
				currIdx = nextIndex = frame;
				++nextIndex %= m_pAnimation->GetBones()[bone].m_nNumKeyFrames;
				break;
			}
		}

		// Update Frames
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
		m_vBoneFrames[bone].GetLocalMatrix() = interpolate(m_pAnimation->GetBones()[bone].m_cKeyFrames[currIdx].m_cFrame.GetLocalMatrix(),
			m_pAnimation->GetBones()[bone].m_cKeyFrames[nextIndex].m_cFrame.GetLocalMatrix(), fPercentTime);
		m_vBoneFrames[bone].Update();
	}
}

void Interpolator::SetAnimation(CAnimation *newAnim)
{
	m_pAnimation = newAnim;
	m_vBoneFrames.resize( m_pAnimation->GetBones().size() );
	for(unsigned int frame = 0; frame < m_pAnimation->GetNumBones(); ++frame)
	{
		m_vBoneFrames[frame] = m_pAnimation->GetBones()[frame].m_cKeyFrames[0].m_cFrame;
	}

	SetTime(0.0f);
}

void Interpolator::SetMesh(DXMesh *newMesh)
{
	m_pMesh = newMesh;
}

void Interpolator::BuildHiearchy(void)
{
	for(int bone = 0; bone < m_pAnimation->GetNumBones(); ++bone)
	{
		for(int child = 0; child < m_pMesh->GetBones()[bone].m_nNumChildren; ++child)
		{
			m_vBoneFrames[bone].AddChildFrame(
				&m_vBoneFrames[m_pMesh->GetBones()[bone].m_cChildrenIdxs[child]]);
		}
	}
}

D3DXMATRIX  Interpolator::interpolate(D3DXMATRIX a, D3DXMATRIX b, float fPercent)
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

	// Zero out old pos
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
