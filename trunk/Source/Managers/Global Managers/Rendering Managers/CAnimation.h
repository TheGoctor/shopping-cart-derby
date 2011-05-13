////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimation.h
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates Animation Data and Functionality
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _CANIMATION_H_
#define _CANIMATION_H_

// Includes
#include <vector>
using namespace std;
#include "..\\..\\..\\CFrame.h"

// Foward Declares
class DXMesh;

// Structures
struct TKeyFrame
{
	float	m_fKeyTime;
	float	m_fTweenTime;
	CFrame  m_cFrame;
};

struct TBone
{
	string				 m_szBoneName;
	unsigned int		 m_nBoneIdx;
	unsigned int		 m_nParentBoneIdx;
	int					 m_nNumKeyFrames;
	vector<TKeyFrame>	 m_cKeyFrames;
	int					 m_nNumChildren;
	vector<unsigned int> m_cChildrenIdxs;
};

struct TBoneInfluence
{
	int	  m_nBoneIndex;
	float m_fWeight;
};

// Animation
class CAnimation
{
	friend class Interpolator;

private:

	float		  m_fDuration;
	int			  m_nNumBones;
	vector<TBone> m_cBones;

public:


	// Constructor
	CAnimation(void);

	// Add Bone
	void AddBone(TBone tBone);

	// Setup Tween Times
	void SetupTweenTimes(void);
	
	// Accessors
	vector<TBone>& GetBones(void)
	{
		return m_cBones;
	}

	int GetNumBones(void)
	{
		return m_nNumBones;
	}

	float GetDuration(void)
	{
		return m_fDuration;
	}

	// Mutators
	void SetDuration(float fDuration) { m_fDuration = fDuration; }
	void SetNumBones(int nNumBones)   { m_nNumBones = nNumBones; }

};

class Interpolator
{
	CAnimation *m_pAnimation;
	DXMesh* m_pMesh;
	std::vector<CFrame> m_vBoneFrames;

	float m_fCurrentTime;

	D3DXMATRIX interpolate(D3DXMATRIX a, D3DXMATRIX b, float fPercent);
public:

	Interpolator(void) : m_pAnimation(0) {}
	~Interpolator(void) { m_vBoneFrames.clear(); }

	void BuildHiearchy(void);

	void SetAnimation(CAnimation *newAnim);
	void SetMesh(DXMesh *newMesh);
	void AddTime(float t) { m_fCurrentTime += t; }
	void SetTime(float t) { m_fCurrentTime = t; }
	void Process(void);

	std::vector<CFrame>& GetCurrentBoneFrames(void) { return m_vBoneFrames; }
};

#endif // _CANIMATION_H_