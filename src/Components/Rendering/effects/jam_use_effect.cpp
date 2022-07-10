////////////////////////////////////////////////////////////////////////////////
//	File			:	CJamUseEffectComp.cpp
//	Date			:	7/27/11
//	Mod. Date		:	7/27/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Jam Use Effect
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CJamUseEffectComp.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\ModelManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\Renderer.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CEffectManager.h"

// Defines
#define JUE_MESH_ROT_LOC_AXIS (D3DXVECTOR3(0.0f, 1.0f, 0.0f))
#define JUE_MESH_ROT_LOC_RATE (30.0f)
#define JUE_MESH_MOVE_Y_RATE  (10.0f)
#define JUE_MESH_Y_CAP		  (6.0f)

// Initalize
void CJamUseEffectComp::Init(void)
{
	// Register for Events
	static CEventManager* pEM = CEventManager::GetInstance();

	// Jam
	pEM->RegisterEvent("UseJam", this, JamUseCallback);

	// Update Gameplay
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	// Shutdown Gameplay
	szEventName = "ShutdownObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, ShutdownGameplayCallback);
}

// Shutdown Gameplay
void CJamUseEffectComp::ShutdownGameplayCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Data from Event
	CJamUseEffectComp* pJUVFXC = (CJamUseEffectComp*)pComp;

	// Delete all Active Jams
	JamUseVFXList::iterator vfxIter = pJUVFXC->m_cActiveJams.begin();
	while(vfxIter != pJUVFXC->m_cActiveJams.end())
	{
		TJamUseVFX* ptJam = (*vfxIter);
		MMDEL(ptJam);
		vfxIter = pJUVFXC->m_cActiveJams.erase(vfxIter);
	}

	// Delete all Dead Jams
	vfxIter = pJUVFXC->m_cDeadJams.begin();
	while(vfxIter != pJUVFXC->m_cDeadJams.end())
	{
		TJamUseVFX* ptJam = (*vfxIter);
		MMDEL(ptJam);
		vfxIter = pJUVFXC->m_cDeadJams.erase(vfxIter);
	}
}

// Jam Destructor
TJamUseVFX::~TJamUseVFX(void)
{
	// Clean up Jam
	static CObjectManager* pOM = CObjectManager::GetInstance();
	pOM->DestroyObject(m_pMeshObj);
	m_bActive = false;
}

// Jam Use
void CJamUseEffectComp::JamUseCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Data from Event
	CJamUseEffectComp* pJUVFXC = (CJamUseEffectComp*)pComp;
	TStatusEffectEvent* ptData = (TStatusEffectEvent*)pEvent->GetData();

	// Call Handler
	if(ptData->m_pObject == pJUVFXC->GetParent())
	{
		pJUVFXC->UseJam();
	}
}
void CJamUseEffectComp::UseJam(void)
{
	// Find a Jam
	TJamUseVFX* ptJam = NULL;
	
	// Can we Recycle
	if(m_cDeadJams.size() > 0)
	{
		ptJam = m_cDeadJams.front();
		m_cDeadJams.pop_front();
		ptJam->Reset();
	}
	else
	{
		ptJam = MMNEW(TJamUseVFX);
		ptJam->Init(m_pParentObj);
	}

	// Set as Active
	m_cActiveJams.push_back(ptJam);
}

// Initalize Jam
void TJamUseVFX::Init(CObject* pParentObj)
{
	// Get Singletons
	static CObjectManager* pOM = CObjectManager::GetInstance();
	static ModelManager*   pMM = ModelManager::GetInstance();
	static Renderer*	   pRM = Renderer::GetInstance();
	static CEffectManager* pEM = CEffectManager::GetInstance();

	// Keep a Static Counter
	static int snCount = 0;
	static int snMeshIdx = pMM->GetMeshIndexByName("Jam_MShape", false, false);

	// Create Mesh Obj
	string szObjName = "JamUseMesh";
	szObjName += (char)snCount;
	m_pMeshObj = pOM->CreateObject(szObjName);
	pOM->BindObjects(pParentObj, m_pMeshObj);

	// Create Render Comp
	m_pMeshRenComp = pRM->CreateRenderComp(m_pMeshObj, snMeshIdx,
		RC_JAM, RF_INDEXED_VERT_TEX2);

	// Create Trail
	string szEffectObjName = "JamTrailUp";
	szEffectObjName += (char)snCount;
	CObject* pJamEffect0 = pOM->CreateObject(szEffectObjName);
	pOM->BindObjects(m_pMeshObj, pJamEffect0);

	// Get Emitters
	CParticleEmitter* pPE0 = pEM->GetCloneEmitter("Jam_Use_Trail.bin", pJamEffect0);

	// Create Render Comp
	CRenderComponent* pRenComp0 = pRM->CreateRenderComp(pJamEffect0,
		(DXMesh*)pPE0, RC_JAM_HIT_EFFECT_BLOB, RF_PARTICLE);

	// Create Effect Comps
	m_pTrailEffect = pEM->CreateEffectComp(m_pMeshObj, pPE0, pRenComp0,
		EC_TYPE_JAM_USE, PE_TYPE_JAM_TRAIL_UP);

	// Turn it on
	m_pTrailEffect->SwitchOnOffEmitters(EC_TYPE_JAM_USE, true);

	// Set On
	m_bActive = true;

	// Increment Counter
	++snCount;
}

// Reset Jam
void TJamUseVFX::Reset(void)
{
	// Get Mesh Frame
	CFrame* pFrame = m_pMeshObj->GetTransform();

	// Put Back
	pFrame->TranslateFrame(-pFrame->GetLocalPosition());

	// Turn on Effect
	m_pTrailEffect->SwitchOnOffEmitters(EC_TYPE_JAM_USE, true);

	// Set On
	m_bActive = true;
}

// Update Gameplay
void CJamUseEffectComp::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CJamUseEffectComp* pJUVFXC = (CJamUseEffectComp*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Update Component
	pJUVFXC->Update(fDT);
}
void CJamUseEffectComp::Update(float fDT)
{
	// Update all Active Jams
	JamUseVFXList::iterator vfxIter = m_cActiveJams.begin();
	while(vfxIter != m_cActiveJams.end())
	{
		TJamUseVFX* ptJam = (*vfxIter);
		ptJam->Update(fDT);

		// Check for Dead
		if(ptJam->m_bActive == false)
		{
			m_cDeadJams.push_back(ptJam);

			vfxIter = m_cActiveJams.erase(vfxIter);
		}
		else
		{
			// Render it
			ptJam->m_pMeshRenComp->AddToRenderSet();
			vfxIter++;
		}
	}
}
// Update Jam
void TJamUseVFX::Update(float fDT)
{
	// Get Mesh Frame
	CFrame* pMeshFrame = m_pMeshObj->GetTransform();

	// Rotate Mesh Locally
	pMeshFrame->RotateFrame(JUE_MESH_ROT_LOC_AXIS, JUE_MESH_ROT_LOC_RATE * fDT);

	// Translate Mesh
	pMeshFrame->TranslateFrame(D3DXVECTOR3(0.0f, JUE_MESH_MOVE_Y_RATE, 0.0f) * fDT);

	// Kill Effect
	if(pMeshFrame->GetLocalPosition().y > JUE_MESH_Y_CAP)
	{
		m_bActive = false;
		m_pTrailEffect->SwitchOnOffEmitters(EC_TYPE_JAM_USE, false);
	}
}