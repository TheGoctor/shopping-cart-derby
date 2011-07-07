////////////////////////////////////////////////////////////////////////////////
//	File			:	CVictoryVFXComp.h
//	Date			:	6/10/11
//	Mod. Date		:	6/10/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Victory VFX
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CVICTORYVFXCOMP_H_
#define _CVICTORYVFXCOMP_H_

// Includes
#include "CEffectComponent.h"
#include "..\\..\\IComponent.h"

#include "..\\..\\CObject.h"
#include "..\\..\\Managers\\Component Managers\\CComponentManager.h"

// Defines
#define MAX_FIREWORKS (2)

// Foward Declares

// Struct
struct TFirework
{
	// Timer
	float m_fTimer;

	// Effect Obj
	CObject* m_pEmitterObj;

	// Effect Comp
	CEffectComponent* m_pEffect;

	// Raising
	bool m_bRising;

	// Raise Cap
	float m_fYCap;

	// Constructor
	TFirework(void) : m_fTimer(0.0f), m_pEmitterObj(NULL), m_pEffect(NULL), m_bRising(true), m_fYCap(0.0f)
	{
	}

	// Init
	void Init(CObject* pParent);

	// Update
	void Update(float fDT);

	// Helper Funcs
	void ResetFirework(void);
};

// Class
class CVictoryVFXComp : public IComponent
{
private:

	// Parent Object
	CObject* m_pParentObj;

	// Effects
	TFirework m_tFireworks[MAX_FIREWORKS];
	CEffectComponent* m_pConfettiEffect;

	// Active Flag
	bool	m_bActive;


	// Handlers
	void Update(float fDT);

public:

	// Get Auto-Manager
	static CComponentManager<CVictoryVFXComp>* GetManager(void)
	{
		// Auto-Manager
		static CComponentManager<CVictoryVFXComp> m_cManager;

		return &m_cManager;
	}

	// Initalize
	void Init(void);

	// Shutdown
	void Shutdown(void);

	// Constructor
	CVictoryVFXComp(CObject* pParent) : m_pParentObj(pParent),
										m_pConfettiEffect(NULL),
										m_bActive(false)										
	{
		// Add to Auto Manager
		GetManager()->Add(this);

		// Add Component to Parent
		pParent->AddComponent(this);

		// Initalize
		Init();
	}

	// Component
	~CVictoryVFXComp(void)
	{
		Shutdown();
		GetManager()->Remove(this);
	}

	// Factory
	static CVictoryVFXComp* CreateVictoryVFXComponent(CObject* pParent)
	{
		return MMNEW(CVictoryVFXComp(pParent));
	}

	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
	static void PlayerWonCallback(IEvent* pEvent, IComponent* pComp);
	static void ShutdownCallback(IEvent* pEvent, IComponent* pComp);
	static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

	// Accessors
	bool IsActive(void) { return m_bActive; }
	CObject* GetParent(void) { return m_pParentObj; }

	// Mutators
	void SetActive(bool bActive) { m_bActive = bActive; }
	void SetParent(CObject* pParentObj) { m_pParentObj = pParentObj; }
};

#endif