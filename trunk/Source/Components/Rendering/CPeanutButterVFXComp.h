////////////////////////////////////////////////////////////////////////////////
//	File			:	CPeanutButterVFXComp.h
//	Date			:	6/26/11
//	Mod. Date		:	6/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Peanut Butter Effect
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CPEANUTBUTTERVFXCOMP_H_
#define _CPEANUTBUTTERVFXCOMP_H_

// Includes
#include "CSkidMarks.h"

// Peanut Butter Effect
class CPeanutButterVFXComp : public IComponent
{
private:

	// Parent Object
	CObject* m_pParentObj;

	// Character Set Type
	ECharacterSet m_eCharacterSetType;

	// Skid Emitter Objs
	CObject* m_pLeftSkidEmitter;
	CObject* m_pRightSkidEmitter;

	// Skid Mesh Pair
	CSkidMeshPair* m_pSkidMeshPair;

	// Timers
	float m_fDuration;

	// Handlers
	void Update(float fDT);
	void Slow(TStatusEffectEvent* pcObjEvent);
	void Invulnerable(TStatusEffectEvent* pcObjEvent);

public:

	// Get Auto-Manager
	static CComponentManager<CPeanutButterVFXComp>* GetManager(void)
	{
		// Auto-Manager
		static CComponentManager<CPeanutButterVFXComp> m_cManager;

		return &m_cManager;
	}

	// Constructor
	CPeanutButterVFXComp(CObject* pParent, ECharacterSet eType) : m_pParentObj(pParent),
																  m_eCharacterSetType(eType),
																  m_pLeftSkidEmitter(NULL),
																  m_pRightSkidEmitter(NULL),
																  m_pSkidMeshPair(NULL),
																  m_fDuration(0.0f)
	{
		// Add to Auto Manager
		GetManager()->Add(this);

		// Add Component to Parent
		pParent->AddComponent(this);

		// Initalize
		Init();
	}

	// Initalize
	void Init(void);

	// Shutdown
	void Shutdown(void);

	// Destructor
	~CPeanutButterVFXComp(void)
	{
		// Remove from Manager
		GetManager()->Remove(this);
	}

	// Factory
	static int CreatePBVFXComponent(lua_State* pLua);
	static CPeanutButterVFXComp* CreatePBVFXComponent(CObject* pParent, ECharacterSet eType)
	{
		return MMNEW(CPeanutButterVFXComp(pParent, eType));
	}

	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
	static void SlowCallback(IEvent* pEvent, IComponent* pComp);
	static void InvulnerableCallback(IEvent* pEvent, IComponent* pComp);
	static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

	// Accessors
	CObject* GetParent(void) { return m_pParentObj; }

};

#endif // _CPEANUTBUTTERVFXCOMP_H_