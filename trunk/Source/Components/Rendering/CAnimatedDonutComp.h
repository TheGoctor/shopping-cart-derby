////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimatedDonutComp.h
//	Date			:	6/10/11
//	Mod. Date		:	6/10/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Animated Donut
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CANIMATEDDONUTCOMP_H_
#define _CANIMATEDDONUTCOMP_H_

// LUA
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// Includes
#include "CAnimateComponent.h"
#include "CEffectComponent.h"
#include "..\\..\\IComponent.h"
#include "..\\..\\CObject.h"
#include "..\\..\\Managers\\Component Managers\\CComponentManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

// Animated Donut
class CAnimatedDonutComp : public IComponent
{
private:

	// Parent Object
	CObject* m_pParentObj;

	// Render Comp
	CAnimateComponent* m_pAnimComp;

	// Effect Comp
	CEffectComponent* m_pPuffEffect;

	// Active
	bool m_bActive;

	// Duration
	float m_fDuration;

	// Helper Funcs
	bool CheckForPlayerMatch(CObject* pOtherObj);

public:

	// Get Auto-Manager
	static CComponentManager<CAnimatedDonutComp>* GetManager(void)
	{
		// Auto-Manager
		static CComponentManager<CAnimatedDonutComp> m_cManager;

		return &m_cManager;
	}

	// Initalize
	void Init(void);

	// Shutdown
	void Shutdown(void);

	// Constructor
	CAnimatedDonutComp(CObject* pParent) : m_pParentObj(pParent), m_bActive(false),
										   m_pAnimComp(NULL), m_fDuration(0.0f), m_pPuffEffect(NULL)
	{
		// Add to Auto Manager
		GetManager()->Add(this);

		// Add Component to Parent
		pParent->AddComponent(this);

		// Initalize
		Init();
	}

	// Destructor
	~CAnimatedDonutComp(void)
	{
		Shutdown();
		GetManager()->Remove(this);
	}

	// Factory
	static int CreateAnimatedDonutComponent(lua_State* pLUA);
	static CAnimatedDonutComp* CreateAnimatedDonutComponent(CObject* pParent)
	{
		return MMNEW(CAnimatedDonutComp(pParent));
	}

	// Callbacks
	void UseDonut(TStatusEffectEvent* pcObjEvent);
	static void UseDonutCallback(IEvent* pEvent, IComponent* pComp);
	static void ShutdownCallBack(IEvent*, IComponent*);
	static void WinInitCallback(IEvent*, IComponent* pComp);
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

	void DespawnDonut(TStatusEffectEvent* pcObjEvent);
	static void DespawnDonutCallback(IEvent* pEvent, IComponent* pComp);

	// Accessors
	CObject* GetParent(void) { return m_pParentObj; }
	bool IsActive(void)		 { return m_bActive;    }

	// Mutators
	void SetActive(bool bActive) { m_bActive = bActive; }
};

#endif