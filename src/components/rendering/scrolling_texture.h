////////////////////////////////////////////////////////////////////////////////
//	File			:	CScrollingUVComp.h
//	Date			:	7/12/11
//	Mod. Date		:	7/12/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets Meshes Scroll UVs
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CSCROLLINGUVCOMP_H_
#define _CSCROLLINGUVCOMP_H_

// Includes
#include "..\\..\\IComponent.h"
#include "..\\..\\CObject.h"
#include "..\\..\\Managers\\Component Managers\\CComponentManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\DXMesh.h"

// Scolling UVs
class CScrollingUVComp : public IComponent
{
private:

	// Parent Object
	CObject* m_pParentObj;

	// Character Set Type
	ECharacterSet m_eCharacterSetType;

	// Mesh
	DXMesh* m_pMesh;

	// Modifiers
	float m_fSpeed;
	float m_fTopSpeed;
	float m_fMinSpeed;

	// Handlers
	void Update(float fDT);
	void UpdateSciCart(float fDT);
	void UpdateStoryTimeCart(float fDT);
	void UpdateEndGameCart(float fDT);

	// Helper Funcs
	static bool CheckForPlayerMatch(CObject* pParentObj, CObject* pOtherObj);

public:

	// Get Auto-Manager
	static CComponentManager<CScrollingUVComp>* GetManager(void)
	{
		// Auto-Manager
		static CComponentManager<CScrollingUVComp> m_cManager;

		return &m_cManager;
	}

	// Constructor
	CScrollingUVComp(CObject* pParent, DXMesh* pMesh, ECharacterSet eType) 
		: m_pParentObj(pParent), m_pMesh(pMesh), m_eCharacterSetType(eType),
		  m_fSpeed(1.0f), m_fTopSpeed(10.0f), m_fMinSpeed(1.0f)
														
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
	~CScrollingUVComp(void)
	{
		// Remove from Manager
		GetManager()->Remove(this);
	}

	// Factory
	static int CreateScrollingUVComponent(lua_State* pLua);
	static CScrollingUVComp* CreateScrollingUVComponent(CObject* pParent,
		DXMesh* pMesh, ECharacterSet eType)
	{
		return MMNEW(CScrollingUVComp(pParent, pMesh, eType));
	}

	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
	static void AccelerateCallback(IEvent* pEvent, IComponent* pComp);
	static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

	// Accessors
	CObject* GetParent(void) { return m_pParentObj; }
};

#endif // _CSCROLLINGUVCOMP_H_