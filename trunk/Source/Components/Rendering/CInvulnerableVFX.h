////////////////////////////////////////////////////////////////////////////////
//	File			:	CInvulnerableVFX.h
//	Date			:	6/10/11
//	Mod. Date		:	6/10/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Chiken Soup VFX
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CINVULNERABLEVFX_H_
#define _CINVULNERABLEVFX_H_

// Includes
#include "CEffectComponent.h"
#include "..\\..\\Managers\\Component Managers\\CComponentManager.h"
#include "..\\..\\IComponent.h"
#include "..\\..\\CObject.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

//#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\DXMesh.h"

// Foward Declares
class DXMesh;

// Invulnerable Effect
class CInvulnerableVFX : public IComponent
{
private:

	// Parent Object
	CObject* m_pParentObj;

	// Mesh
	DXMesh* m_pMesh;

	// Effect
	CEffectComponent* m_pEffect;

	// Active
	bool m_bActive;

	// Direction
	float m_fDir;

	// Timer
	float m_fInvDuration;
	float m_fBoostDuration;
	float m_fBoostAnimTimer;

	// Handlers
	void Invulnerable(TStatusEffectEvent* pcObjEvent);
	void Boost(TStatusEffectEvent* pcObjEvent);
	void InvulnerableEnd(TObjectEvent* pcObjEvent);
	void Update(float fDT);
	void TurnOff(void);

	// Helper Funcs
	bool CheckForPlayerMatch(CObject* pOtherObj);

public:

	// Get Auto-Manager
	static CComponentManager<CInvulnerableVFX>* GetManager(void)
	{
		// Auto-Manager
		static CComponentManager<CInvulnerableVFX> m_cManager;

		return &m_cManager;
	}

	// Initalize
	void Init(void);

	// Shutdown
	void Shutdown(void);

	// Constructor
	CInvulnerableVFX(CObject* pParent, DXMesh* pMesh) : m_pParentObj(pParent),
														m_pMesh(pMesh),
														m_bActive(false),
														m_fDir(1.0f),
														m_fInvDuration(0.0f),
														m_fBoostDuration(0.0f),
														m_fBoostAnimTimer(0.0f)
	{
		// Add to Auto Manager
		GetManager()->Add(this);

		// Add Component to Parent
		pParent->AddComponent(this);

		// Initalize
		Init();
	}

	// Component
	~CInvulnerableVFX(void)
	{
		Shutdown();
		GetManager()->Remove(this);
	}

	// Factory
	static int CreateInvulnerableVFXComponent(lua_State* pLUA);
	static CInvulnerableVFX* CreateInvulnerableVFXComponent(CObject* pParent, DXMesh* pMesh)
	{
		return MMNEW(CInvulnerableVFX(pParent, pMesh));
	}


	// Callbacks
	static void ShutdownCallBack(IEvent*, IComponent*);

		// Invulnerable
		static void InvulnerableCallback(IEvent* pEvent, IComponent* pComp);
		static void InvulnerableEndCallback(IEvent* pEvent, IComponent* pComp);

		// Boost
		static void BoostCallback(IEvent* pEvent, IComponent* pComp);

		// Update
		static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

		// Win State
		static void WinStateInitCallback(IEvent* pEvent, IComponent* pComp);

		// Destroy Object
		static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

	// Accessors
	CObject* GetParent(void) { return m_pParentObj; }
	bool IsActive(void)		 { return m_bActive;    }

	// Mutators
	void SetActive(bool bActive) { m_bActive = bActive; }

};

#endif // _CINVULNERABLEVFX_H_