////////////////////////////////////////////////////////////////////////////////
//	File			:	CSkidMarks.h
//	Date			:	6/5/11
//	Mod. Date		:	6/5/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Skid Mark Effect
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CSKIDMARKS_H_
#define _CSKIDMARKS_H_

// Includes
#include "..\\..\\IComponent.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\DXMesh.h"
#include "..\\..\\Managers\\Component Managers\\CComponentManager.h"

// Foward Declares
class CObject;
class CRenderComponent;
class CEffectComponent;

// Skid Mark Mesh
class CSkidMarkMesh : public DXMesh
{
private:

	// Parent Frame
	CFrame* m_pParentFrame;

	// Render Comp
	CRenderComponent* m_pRenComp;

	// UV Switch
	bool m_bUVSwitch;

	// Active
	bool m_bActive;

	// Width
	float m_fWidth;

	// Alpha
	float m_fAlpha;

public:

	// Constructor
	CSkidMarkMesh(void);

	// Grow
	void Grow(void);
	void DecreaseAlpha(float fDT);

	// Reset
	void ResetSkidMesh(void);

	// Accessors
	CRenderComponent* GetRenderComp(void) { return m_pRenComp; }
	bool IsActive(void) { return m_bActive; }
	float GetWidth(void) { return m_fWidth; }
	float GetAlpha(void) { return m_fAlpha; }

	// Mutators
	void SetParent(CFrame* pParentFrame) { m_pParentFrame = pParentFrame; }
	void SetRenderComponent(CRenderComponent* pRenComp) { m_pRenComp = pRenComp; }
	void SetActive(bool bActive) { m_bActive = bActive; }
	void SetWidth(float fWidth) { m_fWidth = fWidth; } 
};

// Skid Pair
class CSkidMeshPair
{
private:

	// Meshs
	CSkidMarkMesh m_cLeftMesh;
	CSkidMarkMesh m_cRightMesh;

	// Frames
	CFrame* m_pLeftFrame;
	CFrame* m_pRightFrame;

	// Active Flags
	bool m_bActive;
	bool m_bFading;

public:

	// Constructor
	CSkidMeshPair(void) : m_bActive(false), m_bFading(false),
						  m_pLeftFrame(NULL), m_pRightFrame(NULL)
	{
	}

	// Accessors
	CSkidMarkMesh* GetLeftMesh(void) { return &m_cLeftMesh; }
	CSkidMarkMesh* GetRightMesh(void) { return &m_cRightMesh; }
	bool IsActive(void) { return m_bActive; } 
	bool IsFading(void) { return m_bFading; } 

	// Mutators
	void SetFrames(CFrame* pLeftFrame, CFrame* pRightFrame)
	{
		m_pLeftFrame = pLeftFrame;
		m_pRightFrame = pRightFrame;
	}
	void SetActive(bool bActive)
	{ 
		m_bActive = bActive;
		m_cLeftMesh.SetActive(bActive);
		m_cRightMesh.SetActive(bActive);
	}
	void SetFading(bool bFading) { m_bFading = bFading; }

	// Create Meshs
	void CreateMeshs(ERenderContext eRC);

	// Grow Skids
	void GrowSkids(void);

	// Reset Meshs
	void ResetMeshes(void);

	// Render Skids
	void AddToSet(void);

	// Update
	void UpdateAlpha(float fDT);

	// Set Width
	void SetWidth(float fWidth)
	{
		m_cLeftMesh.SetWidth(fWidth);
		m_cRightMesh.SetWidth(fWidth);
	}
};

// Type Definitions
typedef list<CSkidMeshPair*, scd::allocator<CSkidMeshPair*>> SkidMarkMeshList;

// Skid Marks Component
class CSkidMarks : public IComponent
{
private:

	// Parent Object
	CObject* m_pParentObj;

	// Character Set Type
	ECharacterSet  m_eCharacterSetType;
	ERenderContext m_eRenderContext;

	// Skid Emitter Objs
	CObject* m_pLeftSkidEmitter;
	CObject* m_pRightSkidEmitter;

	// Skid Meshs
	SkidMarkMeshList m_cActiveSkidMarkMeshes;
	SkidMarkMeshList m_cDeadSkidMarkMeshes;
	CSkidMeshPair* m_pCurrentSkidMeshPair;

	// Propulsion Effects
	CEffectComponent* m_pLDustEffect;
	CEffectComponent* m_pRDustEffect;

	CObject* m_pLDustEmitter;
	CObject* m_pRDustEmitter;

	// Timer
	float m_fDropTimer;
	float m_fWidth;

	// Is Active Flags
	bool m_bDrift;
	bool m_bPrevDrift;
	bool m_bAccelerating;

	// Factory
	void CreateNewPair(void);

	// Callback Handlers
	void Update(float fDT);
	void Drift(TInputEvent* pcObjEvent);
	void Accelerate(TInputEvent* pcObjEvent);

public:

	// Helper Funcs
	static D3DXVECTOR3 FindLeftEmitterPos(ECharacterSet eType);
	static D3DXVECTOR3 FindRightEmitterPos(ECharacterSet eType);
	static float FindWidth(ECharacterSet eType);
	static ERenderContext FindRenderContext(ECharacterSet eType);
	static bool CheckForPlayerMatch(CObject* pParentObj, CObject* pOtherObj);


	// Get Auto-Manager
	static CComponentManager<CSkidMarks>* GetManager(void)
	{
		// Auto-Manager
		static CComponentManager<CSkidMarks> m_cManager;

		return &m_cManager;
	}

	// Constructor
	CSkidMarks(CObject* pParent, ECharacterSet eType) : m_pParentObj(pParent),
														m_eCharacterSetType(eType),
														m_fDropTimer(0.0f),
														m_fWidth(0.1f),
														m_bDrift(false),
														m_bPrevDrift(false),
														m_bAccelerating(false),
														m_pLeftSkidEmitter(NULL),
														m_pCurrentSkidMeshPair(NULL),
														m_pLDustEffect(NULL),
														m_pRDustEffect(NULL),
														m_pLDustEmitter(NULL),
														m_pRDustEmitter(NULL),
														m_eRenderContext(RC_CART_SKID_MARKS)
								   
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

	// Component
	~CSkidMarks(void)
	{
		//MMDEL(m_pLeftSkids);
		//Shutdown();
		GetManager()->Remove(this);
	}


	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
	static void DriftCallback(IEvent* pEvent, IComponent* pComp);
	static void AccelerateCallback(IEvent* pEvent, IComponent* pComp);
	static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

	// Factory
	static int CreateSkidMarksComponent(lua_State* pLua);
	static CSkidMarks* CSkidMarks::CreateSkidMarksComponent(CObject* pParent, ECharacterSet eType)
	{
		return MMNEW(CSkidMarks(pParent, eType));
	}

	// Accessors
	CObject* GetParent(void) { return m_pParentObj; }
	bool IsDrifting(void) { return m_bDrift; }
	bool WasDrifting(void) { return m_bPrevDrift; }
	float GetWidth(void) { return m_fWidth; }

	// Mutators
	void SetWidth(float fWidth) { m_fWidth = fWidth; }
};

#endif // _CSKIDMARKS_H_