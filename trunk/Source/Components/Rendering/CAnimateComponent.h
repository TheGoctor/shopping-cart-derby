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
#include <map>
using std::map;
#include "..\\..\\IComponent.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CAnimation.h"

// Foward Declares
class CObject;
class CAnimation;
class DXMesh;
class IEvent;

typedef std::map<unsigned int, pair<unsigned int, unsigned int>, less<unsigned int>, 
	CAllocator<pair<unsigned int, pair<unsigned int, unsigned int>>>> AnimMap;

// Animate Component
class CAnimateComponent : public IComponent
{
private:

	// Parent Object
	CObject* m_pcParent;

	// Animation
	CAnimation* m_pAnimation;
	
	// Map of Animations (Key = ID of EventID, Data = ID of Animation)
	AnimMap m_cAnimationList;

	// Mesh
	DXMesh* m_pMesh;

	// Interpolator
	Interpolator m_cInterpolator;

	// Callback Handlers
	void Update(float fDT);

	// Handle to previous animation for a "default" animation
	CAnimation* m_pDefaultAnim;

	unsigned int m_nCurrAnim;
	pair<unsigned int, pair<unsigned int, unsigned int>> m_cCurrAnim;
	CAnimation* m_pCurrentAnimation;

	// On
	bool m_bOn;

public:

	// Constructor
	CAnimateComponent(CObject* pParent, DXMesh* pMesh, Interpolator cInterpolator);

	CObject* GetParent() { return m_pcParent; }

	// Skinning Algo
	void SkinMesh(void);

	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
	static void HandleAnimationSwitchCallback(IEvent* pEvent, IComponent* pComp);
	static void ActivateCallback(IEvent* pEvent, IComponent* pComp);
	static void DeactivateCallback(IEvent* pEvent, IComponent* pComp);
	
	static void PauseUpdateCallback(IEvent* pEvent, IComponent* pComp);

	// On/Off
	void SetOn(bool bOn)
	{ 
		m_bOn = bOn;
	}
	bool IsOn(void) { return m_bOn; }

	// Add to Map
	void AddAnimation(string szEventName, string szPreAnimationName, string szLoopAnimationName);

	// Accessors
	Interpolator& GetInterpolator(void) { return m_cInterpolator; }
};
#endif // _CANIMATECOMPONENT_H_