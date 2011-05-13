////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateManager.cpp
//	Date			:	5/12/11
//	Mod. Date		:	5/12/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Factory for Animate Components
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CAnimateManager.h"
#include "..\\..\\Components\\Rendering\\CAnimateComponent.h"
#include "..\\Global Managers\\Rendering Managers\\CAnimation.h"
#include "..\\..\\CObject.h"

// Constructor
CAnimateManager::CAnimateManager(void)
{
}

// Destructor
CAnimateManager::~CAnimateManager(void)
{
}

// Factory
CAnimateComponent* CAnimateManager::CreateAnimateComp(CObject* pParent, DXMesh* pMesh,
											CAnimation* pAnimation)
{
	// Create Interpolator
	Interpolator cInterpolator;
	cInterpolator.SetAnimation(pAnimation);
	cInterpolator.SetMesh(pMesh);
	
	// Create Component
	CAnimateComponent* comp = MMNEW(CAnimateComponent) CAnimateComponent(pParent,
		pMesh, pAnimation, cInterpolator);

	comp->GetInterpolator().BuildHiearchy();
	
	// Attach to Obj
	pParent->AddComponent(comp);
	//pParent->GetTransform()->AddChildFrame(&comp->GetInterpolator().GetCurrentBoneFrames()[0]);

	// Insert into Map
	GetInstance()->m_cAnimateComps.insert(make_pair((unsigned int)pParent->GetID(), comp));

	return comp;
}