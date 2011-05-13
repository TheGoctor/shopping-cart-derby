////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateComponent.h
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object Animate
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _CANIMATECOMPONENT_H_
#define _CANIMATECOMPONENT_H_

// Includes
#include "..\\..\\IComponent.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CAnimation.h"

// Foward Declares
class CObject;
class CAnimation;
class DXMesh;
class IEvent;

class CAnimateComponent : public IComponent
{
private:

	// Parent Object
	CObject* m_pcParent;

	// Animation
	CAnimation* m_pAnimation;

	// Mesh
	DXMesh* m_pMesh;

	// Interpolator
	Interpolator m_cInterpolator;

	// Callback Handlers
	void Update(float fDT);

public:

	// Constructor
	CAnimateComponent(CObject* pParent, DXMesh* pMesh, CAnimation* pAnimation, 
		Interpolator cInterpolator);

	// Skinning Algo
	void SkinMesh(void);

	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

	// Accessors
	Interpolator& GetInterpolator(void)
	{
		return m_cInterpolator;
	}
};



#endif // _CANIMATECOMPONENT_H_