////////////////////////////////////////////////////////////////////////////////
//	File			:	CShadowComp.h
//	Date			:	5/19/11
//	Mod. Date		:	5/19/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object have a Shadow
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _CSHADOWCOMP_H_
#define _CSHADOWCOMP_H_

// Includes
#include "..\\..\\IComponent.h"
#include "..\\..\\Enums.h"
#include "..\\..\\CObject.h"
#include "..\\..\\Managers\\Component Managers\\CComponentManager.h"
#include "..\\..\\Managers\\Global Managers\\Console Manager\\CConsoleManager.h"
//#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\DXMesh.h"

// Foward Declares
class CObject;
class DXMesh;
class CRenderComponent;

// Shadow Component
class CShadowComp : IComponent
{
private:

	// Parent Object
	CObject* m_pParent;

	// Shadow Obj
	CObject* m_pShadowObj;

	// Type
	ECharacterSet m_eType;

	// Render Comp
	CRenderComponent* m_pRenComp;

	// Helper Funcs
	DXMesh* GetShadowMeshByCharacterSet(ECharacterSet eType);

public:

	// Get Auto-Manager
	static CComponentManager<CShadowComp>* GetManager(void)
	{
		// Auto-Manager
		static CComponentManager<CShadowComp> m_cManager;

		return &m_cManager;
	}

	// Constructor
	CShadowComp(CObject* pParent, ECharacterSet eType) : m_pParent(pParent),
		m_eType(eType), m_pRenComp(NULL), m_pShadowObj(NULL)
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

	// Destructor
	~CShadowComp(void)
	{
		Shutdown();
		GetManager()->Remove(this);
	}

	// Shutdown
	void Shutdown(void);

	// Factory
	static int CreateShadowComponent(lua_State* pLua);
	static CShadowComp* CreateShadowComponent(CObject* pParent, ECharacterSet eType)
	{
		return MMNEW(CShadowComp(pParent, eType));
	}

	// Accessors
	CObject* GetParent(void) { return m_pParent; }
	ECharacterSet GetCharacterSetType(void) { return m_eType; }

	// Mutators
	void SetCharaterSetType(ECharacterSet eType) { m_eType = eType; }

	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
	void Update(void);

	// Shutdown 
	static void ShutdownCallback(IEvent* pEvent, IComponent* pComp);
};

#endif // _CSHADOWCOMP_H_