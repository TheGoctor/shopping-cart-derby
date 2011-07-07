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
#include <map>
using namespace std;
#include "..\\Memory Manager\\CRenderAllocator.h"
#include "..\\..\\..\\CFrame.h"

// Foward Declares
class DXMesh;

// Type Definitions
typedef map<unsigned int, CFrame, less<unsigned int>, 
		CRenderAllocator<pair<unsigned int, CFrame>>> FrameMap;

////////////////////////////////////////////////////////////////////////////////
// Structures
////////////////////////////////////////////////////////////////////////////////

// Key Frame
struct TKeyFrame
{
	float	m_fKeyTime;		// Time of Frame
	float	m_fTweenTime;	// The Time between this Frame and the Next Frame
	CFrame  m_cFrame;		// The Position of the Bone in the Frame

	// Constructor
	TKeyFrame(void);
};

// Bone
struct TBone
{
	string				 m_szBoneName;		// The Name of the Bone
	unsigned int		 m_nBoneIdx;		// The Index of the Bone
	unsigned int		 m_nParentBoneIdx;	// The Parent's Index
	int					 m_nNumKeyFrames;	// The Number of Frames that apply to the Bone
	vector<TKeyFrame>	 m_cKeyFrames;		// The List of Key Frames that apply to the Bone
	int					 m_nNumChildren;	// The Number of Children
	vector<unsigned int> m_cChildrenIdxs;	// The Indices of the Children

	// Destructor
	~TBone(void);
};

// Bone Influence
struct TBoneInfluence
{
	int	  m_nBoneIndex;		// The Index of the Bone
	float m_fWeight;		// The Amount of Influence applied from the Bone
};

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

// Animation
class CAnimation
{
private:

	// Let Interpolator Access Private Data
	friend class Interpolator;

	float		  m_fDuration;	// The Time it takes to go through the Animation
	int			  m_nNumBones;	// The Number of Bones that this Animation has
	vector<TBone> m_cBones;		// List of Bones

public:

	// Constructor
	CAnimation(void);

	// Destructor
	~CAnimation(void);

	// Add Bone
	void AddBone(TBone tBone);

	// Setup Tween Times
	void SetupTweenTimes(void);
	
	// Accessors
	vector<TBone>& GetBones(void)		{ return m_cBones;		}
	int			   GetNumBones(void)	{ return m_nNumBones;	}
	float		   GetDuration(void)	{ return m_fDuration;	}

	// Mutators
	void SetDuration(float fDuration) { m_fDuration = fDuration; }
	void SetNumBones(int nNumBones)   { m_nNumBones = nNumBones; }
};

// Interpolator
class Interpolator
{
	CAnimation *m_pAnimation;	// The Current Animation
	DXMesh* m_pMesh;			// The Mesh to Apply Skinning To (Must be a Copy)
	FrameMap m_cBoneFrames;		// List of Bone Frames
	float m_fCurrentTime;		// The Current Time into the Animation

	// Interpolation used for Skinning
	D3DXMATRIX animInterpolate(D3DXMATRIX a, D3DXMATRIX b, float lamda);

	// NON-FUNCTIONAL
	D3DXMATRIX interpolateWithScale(D3DXMATRIX a, D3DXMATRIX b, float lamda);
public:

	// Constructor
	Interpolator(void);

	// Destructor
	~Interpolator(void);

	// Connect the Frames
	void BuildHiearchy(void);

	// Process and Interpolate
	void Process(void);

	// Accessors
	CAnimation* GetAnimation()			 { return m_pAnimation;  }
	FrameMap& GetCurrentBoneFrames(void) { return m_cBoneFrames; }

	// Mutators
	void SetAnimation(CAnimation *newAnim);
	void SetMesh(DXMesh *newMesh);
	void AddTime(float t) { m_fCurrentTime += t; }
	void SetTime(float t) { m_fCurrentTime = t; }
};
#endif // _CANIMATION_H_