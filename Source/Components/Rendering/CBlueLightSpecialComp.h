////////////////////////////////////////////////////////////////////////////////
//	File			:	CBlueLightSpecialComp.h
//	Date			:	7/2/11
//	Mod. Date		:	7/2/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Blue Light Special for Held Items
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CBLUELIGHTSPECIALCOMP_H_
#define _CBLUELIGHTSPECIALCOMP_H_

// Includes
#include "..\\..\\IComponent.h"
#include "..\\..\\CObject.h"
#include "CRenderComponent.h"
#include "..\\..\\Managers\\Component Managers\\CComponentManager.h"

// Foward Declares
class DXMesh;
class CRenderComponent;

// Blue Light Special
class CBlueLightSpecialComp : public IComponent
{
private:

	// Parent Object
	CObject* m_pParentObj;

	// Blue Light Obj
	CObject* m_pBLSObj;

	// Mesh
	DXMesh* m_pMesh;

	// Render Comps
	CRenderComponent* m_pRenComp;
	CRenderComponent* m_pEndcapRenComp;

	// Rotation
	float m_fRot;

	// Texture Switch
	float m_fTexSwitchTimer;

	// Active
	bool m_bActive;

	// Handlers
	void Update(float fDT);

public:

	// Get Auto-Manager
	static CComponentManager<CBlueLightSpecialComp>* GetManager(void)
	{
		// Auto-Manager
		static CComponentManager<CBlueLightSpecialComp> m_cManager;

		return &m_cManager;
	}

	// Initalize
	void Init(void);

	// Shutdown
	void Shutdown(void);

	// Constructor
	CBlueLightSpecialComp(CRenderComponent* pRC) : m_pParentObj(NULL),
												   m_pBLSObj(NULL), m_pMesh(NULL),
											       m_pRenComp(NULL), m_pEndcapRenComp(pRC),
												   m_fRot(0.0f), m_fTexSwitchTimer(0.0f),
											       m_bActive(false)
										      
	{
		// Set Parent
		m_pParentObj = m_pEndcapRenComp->GetParent();

		// Add to Auto Manager
		GetManager()->Add(this);

		// Add Component to Parent
		m_pParentObj->AddComponent(this);

		// Initalize
		Init();
	}

	// Destructor
	~CBlueLightSpecialComp(void)
	{
		Shutdown();
		GetManager()->Remove(this);
	}

	// Factory
	static CBlueLightSpecialComp* CreateBlueLightSpecialComponent(CRenderComponent* pRC)
	{
		return MMNEW(CBlueLightSpecialComp(pRC));
	}

	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

	// Accessors
	CObject* GetParent(void) { return m_pParentObj; }
	bool IsActive(void)		 { return m_bActive;    }

	// Mutators
	void SetActive(bool bActive) { m_bActive = bActive; }
};

#endif // _CBLUELIGHTSPECIALCOMP_H_
