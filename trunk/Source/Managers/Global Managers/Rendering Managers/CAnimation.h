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
#include <map>
using std::map;

#include "..\Memory Manager\CRenderAllocator.h"
#include "..\..\..\CFrame.h"
#include "..\Memory Manager\CDynArray.h"

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
	CFrame  m_cFrame;		// The Position of the Bone in the Frame

	// Constructor
	TKeyFrame(void);
};

// Bone
struct TBone
{
	string					m_szBoneName;		// The Name of the Bone
	unsigned int			m_nBoneIdx;		// The Index of the Bone
	unsigned int			m_nParentBoneIdx;	// The Parent's Index
	unsigned int			m_nNumKeyFrames;	// The Number of Frames that apply to the Bone
	CDynArray<TKeyFrame>	m_cKeyFrames;		// The List of Key Frames that apply to the Bone
	unsigned int			m_nNumChildren;	// The Number of Children
	CDynArray<unsigned int> m_cChildrenIdxs;	// The Indices of the Children

	// Constructor
	TBone() : m_nBoneIdx(0), m_nParentBoneIdx(0), m_nNumKeyFrames(0), m_nNumChildren(0) { }
	// Destructor
	~TBone(void);
};

// Bone Influence
struct TBoneInfluence
{
	unsigned int	m_nBoneIndex;		// The Index of the Bone
	float			m_fWeight;		// The Amount of Influence applied from the Bone

	// Constructor
	TBoneInfluence() : m_nBoneIndex(0), m_fWeight(0.0f) { }
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

	float				m_fDuration;	// The Time it takes to go through the Animation
	unsigned int		m_nNumBones;	// The Number of Bones that this Animation has
	CDynArray<TBone>	m_cBones;		// List of Bones

	bool m_bLooping;
public:

	// Constructor
	CAnimation(void);

	// Destructor
	~CAnimation(void);

	// Add Bone
	void AddBone(TBone tBone);
	
	// Accessors
	CDynArray<TBone>& GetBones(void)	{ return m_cBones;		}
	unsigned int   GetNumBones(void)	{ return m_nNumBones;	}
	float		   GetDuration(void)	{ return m_fDuration;	}
	bool		   GetLooping()			{ return m_bLooping;    }

	// Mutators
	void SetDuration(float fDuration) { m_fDuration = fDuration; }
	void SetNumBones(unsigned int nNumBones)   { m_nNumBones = nNumBones; }
	void SetLooping(bool bLoop) { m_bLooping = bLoop; }
};

// Interpolator
class Interpolator
{
	friend class CAnimationBlender;
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