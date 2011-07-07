////////////////////////////////////////////////////////////////////////////////
//	File			:	CSkidMarks.cpp
//	Date			:	6/5/11
//	Mod. Date		:	6/5/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Skid Mark Effect
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////
#include "CSkidMarks.h"
#include "CRenderComponent.h"
#include "CEffectComponent.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\ModelManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\Renderer.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CEffectManager.h"
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"

////////////////////////////////////////////////////////////////////////////////
// DEFINES
////////////////////////////////////////////////////////////////////////////////
#define SKID_MARK_DROP_TIME (0.0001f)
#define SKID_MARK_END_TIME (0.3f)
#define SKID_MARK_ALPHA_RATE (255.0f)

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

// Mesh
CSkidMarkMesh::CSkidMarkMesh(void) : DXMesh(), m_pRenComp(NULL), 
									 m_bActive(false), m_fWidth(0.1f),
									 m_fAlpha(255.0f), m_bUVSwitch(false)
{
	SetMode(D3DPT_TRIANGLESTRIP);
}

// Helper Funcs
D3DXVECTOR3 CSkidMarks::FindLeftEmitterPos(ECharacterSet eType)
{
	D3DXVECTOR3 vVec;

	switch(eType)
	{
	case CS_BIKERS:
		vVec = D3DXVECTOR3(0.4f, 0.0f, -0.5f);
		break;
	case CS_SASHA:
		vVec = D3DXVECTOR3(0.45f, 0.0f, -0.45f);
		break;
	case CS_LARPERS:
		vVec = D3DXVECTOR3(0.3f, 0.0f, -0.45f);
		break;
	default:
		vVec = D3DXVECTOR3(0.4f, 0.0f, -0.5f);
		break;
	}

	return vVec;
}

D3DXVECTOR3 CSkidMarks::FindRightEmitterPos(ECharacterSet eType)
{
	D3DXVECTOR3 vVec;

	switch(eType)
	{
	case CS_BIKERS:
		vVec = D3DXVECTOR3(-0.4f, 0.0f, -0.5f);
		break;
	case CS_SASHA:
		vVec = D3DXVECTOR3(-0.45f, 0.0f, -0.45f);
		break;
	case CS_LARPERS:
		vVec = D3DXVECTOR3(-0.3f, 0.0f, -0.45f);
		break;
	default:
		vVec = D3DXVECTOR3(-0.4f, 0.0f, -0.5f);
		break;
	}

	return vVec;
}

float CSkidMarks::FindWidth(ECharacterSet eType)
{
	float fWidth;

	switch(eType)
	{
	case CS_BIKERS:
		fWidth = 0.1f;
		break;
	case CS_SCIENTISTS:
		fWidth = 0.3f;
		break;
	case CS_BANDITOS:
		fWidth = 0.05f;
		break;
	case CS_LARPERS:
		fWidth = 0.05f;
		break;
	case CS_SASHA:
		fWidth = 0.075f;
		break;
	default:
		fWidth = 0.1f;
		break;
	}

	return fWidth;
}

ERenderContext CSkidMarks::FindRenderContext(ECharacterSet eType)
{
	ERenderContext eContext;

	switch(eType)
	{
	case CS_SCIENTISTS:
		eContext = RC_SCI_SKIDS;
		break;
	default:
		eContext = RC_CART_SKID_MARKS;
		break;
	}

	return eContext;
}

bool CSkidMarks::CheckForPlayerMatch(CObject* pParentObj, CObject* pOtherObj)
{
	// Get Object Names
	char* szEventObjName = (char*)pOtherObj->GetID();
	char* szCompObjName  = (char*)pParentObj->GetID();

	// Check for a name Match
	bool bMatch = true;
	for(int i = 0; i < 7; ++i)
	{
		if(szEventObjName[i] != szCompObjName[i])
		{
			bMatch = false;
		}
	}

	return bMatch;
}


// Initalize
void CSkidMarks::Init(void)
{
	// Get Singletons
	CObjectManager* pOM = CObjectManager::GetInstance();
	CEffectManager* pEffMan = CEffectManager::GetInstance();

	// Create Skid Mark Objs

	// Left
	string szObjName = "LeftSkidMarkEmitter";
	szObjName += (char*)m_pParentObj->GetID();
	m_pLeftSkidEmitter = pOM->CreateObject(szObjName);
	pOM->BindObjects(m_pParentObj, m_pLeftSkidEmitter);
	m_pLeftSkidEmitter->GetTransform()->TranslateFrame(FindLeftEmitterPos(m_eCharacterSetType));

	// Right
	szObjName = "RightSkidMarkEmitter";
	szObjName += (char*)m_pParentObj->GetID();
	m_pRightSkidEmitter = pOM->CreateObject(szObjName);
	pOM->BindObjects(m_pParentObj, m_pRightSkidEmitter);
	m_pRightSkidEmitter->GetTransform()->TranslateFrame(FindRightEmitterPos(m_eCharacterSetType));

	// Set Width
	SetWidth(FindWidth(m_eCharacterSetType));

	// Set Ren Context
	m_eRenderContext = FindRenderContext(m_eCharacterSetType);

	// Dust Effect
	if(m_eCharacterSetType == CS_BIKERS)
	{
		szObjName = "LDustEmitter";
		szObjName += (char*)m_pParentObj->GetID();
		m_pLDustEmitter = pOM->CreateObject(szObjName);
		pOM->BindObjects(m_pParentObj, m_pLDustEmitter);
		m_pLDustEmitter->GetTransform()->TranslateFrame(D3DXVECTOR3(0.4f, 1.0f, -1.0f));
		m_pLDustEffect = pEffMan->CreateDustEffectComponent(m_pLDustEmitter);
		szObjName = "RDustEmitter";
		szObjName += (char*)m_pParentObj->GetID();
		m_pRDustEmitter = pOM->CreateObject(szObjName);
		pOM->BindObjects(m_pParentObj, m_pRDustEmitter);
		m_pRDustEmitter->GetTransform()->TranslateFrame(D3DXVECTOR3(-0.4f, 1.0f, -1.0f));
		m_pRDustEffect = pEffMan->CreateDustEffectComponent(m_pRDustEmitter);
	}
	else if(m_eCharacterSetType == CS_SCIENTISTS)
	{
		szObjName = "SciGlow";
		szObjName += (char*)m_pParentObj->GetID();
		m_pGlowEmitter = pOM->CreateObject(szObjName);
		pOM->BindObjects(m_pParentObj, m_pGlowEmitter);
		m_pGlowEmitter->GetTransform()->TranslateFrame(D3DXVECTOR3(0.0f, 0.0f, -0.5f));
		m_pGlowEffect = pEffMan->CreateScientistEffectComponent(m_pGlowEmitter);
	}


	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();

	// Update
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	//szEventName = "InitObjects";
	//szEventName += GAMEPLAY_STATE;
	//pEM->RegisterEvent(szEventName, this, UpdateCallback);

	// Drift
	pEM->RegisterEvent("DriftEffect", this, DriftCallback);

	// Accelerate
	pEM->RegisterEvent("AccelerateEffect", this, AccelerateCallback);

	// Destroy Obj
	pEM->RegisterEvent("DestroyObject", this, DestroyObjectCallback);

	//szEventName = "ShutdownObjects";
	//szEventName += GAMEPLAY_STATE;
	//pEM->RegisterEvent(szEventName, this, DestroyObjectCallback);
}

// Shutdown
void CSkidMarks::Shutdown(void)
{
	// Unregister for Events
	CEventManager* pEM = CEventManager::GetInstance();
	CObjectManager* pOM = CObjectManager::GetInstance();

	pEM->UnregisterEventAll(this);

	pOM->DestroyObject(m_pLeftSkidEmitter);
	pOM->DestroyObject(m_pRightSkidEmitter);

	if(m_pLDustEffect && m_pLDustEmitter)
	{
		m_pLDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, false);
		pOM->DestroyObject(m_pLDustEmitter);
	}
	if(m_pRDustEffect && m_pRDustEmitter)
	{
		m_pRDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, false);
		pOM->DestroyObject(m_pRDustEmitter);
	}
	if(m_pGlowEffect && m_pGlowEmitter)
	{
		m_pGlowEffect->SwitchOnOffEmitters(EC_TYPE_SCIENTIST_GLOW, false);
		pOM->DestroyObject(m_pGlowEmitter);
	}

	// Clean Active
	SkidMarkMeshList::iterator skidIter = m_cActiveSkidMarkMeshes.begin();
	while(skidIter != m_cActiveSkidMarkMeshes.end())
	{
		CSkidMeshPair* pMesh = *skidIter;
		MMDEL(pMesh);

		skidIter++;
	}

	// Clean Dead
	skidIter = m_cDeadSkidMarkMeshes.begin();
	while(skidIter != m_cDeadSkidMarkMeshes.end())
	{
		CSkidMeshPair* pMesh = *skidIter;
		MMDEL(pMesh);

		skidIter++;
	}
}

////////////////////////////////////////////////////////////////////////////////
// FACTORY
////////////////////////////////////////////////////////////////////////////////

// New Pair
void CSkidMarks::CreateNewPair(void)
{
	// Can we Recycle
	if(m_cDeadSkidMarkMeshes.size() > 0)
	{
		CSkidMeshPair* pSMP = m_cDeadSkidMarkMeshes.front();
		m_cDeadSkidMarkMeshes.pop_front();
		pSMP->ResetMeshes();
		m_cActiveSkidMarkMeshes.push_back(pSMP);
		m_pCurrentSkidMeshPair = pSMP;
	}
	else // Create a New Pair
	{
		CSkidMeshPair* pSMP = MMNEW(CSkidMeshPair);
		pSMP->SetWidth(m_fWidth);
		pSMP->SetFrames(m_pLeftSkidEmitter->GetTransform(), m_pRightSkidEmitter->GetTransform());
		pSMP->CreateMeshs(m_eRenderContext);
		m_cActiveSkidMarkMeshes.push_back(pSMP);
		pSMP->SetActive(true);
		m_pCurrentSkidMeshPair = pSMP;
	}

	m_pCurrentSkidMeshPair->GrowSkids();
}

// Reset Meshs
void CSkidMeshPair::ResetMeshes(void)
{
	SetActive(true);
	m_bFading = false;

	m_cLeftMesh.ResetSkidMesh();
	m_cRightMesh.ResetSkidMesh();
}
void CSkidMarkMesh::ResetSkidMesh(void)
{
	m_fAlpha = 255.0f;
	m_tVertInfo.m_vVertices.clear();
	m_tVertInfo.m_vColors.clear();
	m_tVertInfo.m_vUV.clear();
}

// Create Meshs
void CSkidMeshPair::CreateMeshs(ERenderContext eRC)
{
	static int nSkidCounter = 0;

	// Create Mesh Obj
	string szObjName = "LeftSkidMesh";
	szObjName += (char)nSkidCounter;
	CObject* pSkidMeshObjLeft = CObjectManager::GetInstance()->CreateObject(szObjName);
	szObjName = "RightSkidMesh";
	szObjName += (char)nSkidCounter;
	CObject* pSkidMeshObjRight = CObjectManager::GetInstance()->CreateObject(szObjName);

	// Create Mesh
	m_cLeftMesh.SetParent(m_pLeftFrame);
	m_cRightMesh.SetParent(m_pRightFrame);

	// Create Render Comp
	CRenderComponent* pSkidRenCompLeft = Renderer::GetInstance()->CreateRenderComp(pSkidMeshObjLeft,
		(DXMesh*)&m_cLeftMesh, eRC, RF_VERT_TEX2);
	CRenderComponent* pSkidRenCompRight = Renderer::GetInstance()->CreateRenderComp(pSkidMeshObjRight,
		(DXMesh*)&m_cRightMesh, eRC, RF_VERT_TEX2);

	// Connect Ren Comp to Mesh
	m_cLeftMesh.SetRenderComponent(pSkidRenCompLeft);
	m_cRightMesh.SetRenderComponent(pSkidRenCompRight);

	nSkidCounter++;
}

// Component
int CSkidMarks::CreateSkidMarksComponent(lua_State* pLua)
{
	CObject* pParent    = (CObject*)lua_topointer(pLua, -2);
	ECharacterSet eType = (ECharacterSet)lua_tointeger(pLua, -1);

	lua_pop(pLua, 2);

	CreateSkidMarksComponent(pParent, eType);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// UPDATES
////////////////////////////////////////////////////////////////////////////////

// Component
void CSkidMarks::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CSkidMarks* pSM = (CSkidMarks*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Update Component
	pSM->Update(fDT);
}
void CSkidMarks::Update(float fDT)
{
	// Increment Timer
	m_fDropTimer += fDT;

	if(m_bAccelerating)
	{
		if(m_pLDustEffect)
		{
			m_pLDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, true);//0.001F * fDT);
			m_pLDustEffect->SetLifespan(EC_TYPE_CART_SMOKE, 0.001F * fDT);
		}
		if(m_pRDustEffect)
		{
			m_pRDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, true);//SetLifespan(EC_TYPE_CART_SMOKE, 0.001F * fDT);
			m_pRDustEffect->SetLifespan(EC_TYPE_CART_SMOKE, 0.001F * fDT);
		}
		if(m_pGlowEffect)
		{
			m_pGlowEffect->SwitchOnOffEmitters(EC_TYPE_SCIENTIST_GLOW, true);
			//m_pGlowEffect->SetLifespan(EC_TYPE_SCIENTIST_GLOW, 0.001F * fDT);
		}
	}

	// If we are currently droping Skids
	if(m_bDrift)
	{
		// If we have been dropping skids keep growing
		if(m_bPrevDrift)
		{
			if(m_pCurrentSkidMeshPair)
			{
				if(m_pCurrentSkidMeshPair->IsActive())
				{
					if(m_fDropTimer > SKID_MARK_DROP_TIME)
					{
						m_pCurrentSkidMeshPair->GrowSkids();
						m_fDropTimer = 0.0f;
					}

					//m_pCurrentSkidMeshPair->AddToSet();
				}
			}
			else
			{
				CreateNewPair();
			}
		}
		else // We just started Drifting
		{
			CreateNewPair();
		}

		m_bPrevDrift = true;
	}
	else if(m_bPrevDrift)
	{
		// Just stop Drifting
		m_pCurrentSkidMeshPair->SetFading(true);
		m_pCurrentSkidMeshPair = NULL;

		//if(m_pLDustEffect)
		//{
		//	m_pLDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, true);
		//	m_pRDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, true);
		//}

		/*if(m_pGlowEffect)
		{
			m_pGlowEffect->SwitchOnOffEmitters(EC_TYPE_SCIENTIST_GLOW, true);
		}*/

		m_bPrevDrift = false;
	}

	m_bDrift = false;

	// Update Alpha & Render Active
	SkidMarkMeshList::iterator skidIter = m_cActiveSkidMarkMeshes.begin();
	while(skidIter != m_cActiveSkidMarkMeshes.end())
	{
		CSkidMeshPair* pPair = *skidIter;

		pPair->UpdateAlpha(fDT);

		if(pPair->IsActive())
		{
			pPair->AddToSet();
			skidIter++;
		}
		else // Switch Lists
		{
			m_cDeadSkidMarkMeshes.push_back(pPair);
			skidIter = m_cActiveSkidMarkMeshes.erase(skidIter);
		}
	}

	m_bAccelerating = false;
}

// Render Skids
void CSkidMeshPair::AddToSet(void)
{
	m_cLeftMesh.GetRenderComp()->AddToRenderSet();
	m_cRightMesh.GetRenderComp()->AddToRenderSet();
}

// Grow Skids
void CSkidMeshPair::GrowSkids(void)
{
	m_cLeftMesh.Grow();
	m_cRightMesh.Grow();
}

// Alpha
void CSkidMeshPair::UpdateAlpha(float fDT)
{
	if(m_bFading)
	{
		m_cLeftMesh.DecreaseAlpha(fDT);
		m_cRightMesh.DecreaseAlpha(fDT);

		if(m_cLeftMesh.IsActive() == false && m_cRightMesh.IsActive() == false)
			m_bActive = false;
	}
}

// Mesh
void CSkidMarkMesh::Grow(void)
{
	// Create Verts at Right Position
	D3DXVECTOR3 vLeft  = D3DXVECTOR3(m_fWidth, 0.1f, 0.0f);
	D3DXVECTOR3 vRight = D3DXVECTOR3(-m_fWidth, 0.1f, 0.0f);
	D3DXVec3TransformCoord(&vLeft, &vLeft, &m_pParentFrame->GetWorldMatrix());
	D3DXVec3TransformCoord(&vRight, &vRight, &m_pParentFrame->GetWorldMatrix());

	// Color and UVs
	D3DCOLOR dwColor = D3DCOLOR_ARGB((DWORD)m_fAlpha, 255, 255, 255);

	// UVs
	D3DXVECTOR2 vUV1;
	D3DXVECTOR2 vUV2;
	if(m_bUVSwitch)
	{
		vUV1 = D3DXVECTOR2(0.0f, 1.0f);
		vUV2 = D3DXVECTOR2(1.0f, 1.0f);
	}
	else
	{
		vUV1 = D3DXVECTOR2(0.0f, 0.0f);
		vUV2 = D3DXVECTOR2(1.0f, 0.0f);
	}
	m_bUVSwitch = !m_bUVSwitch;

	// Update Vert Info
	m_tVertInfo.m_vVertices.push_back(vLeft);
	m_tVertInfo.m_vVertices.push_back(vRight);
	m_tVertInfo.m_vColors.push_back(dwColor);
	m_tVertInfo.m_vColors.push_back(dwColor);
	m_tVertInfo.m_vUV.push_back(vUV1);
	m_tVertInfo.m_vUV.push_back(vUV2);
	
	// Set Vertex Buffer
	SetColoredTexturedVertexInfo(&m_tVertInfo);
}

// Alpha
void CSkidMarkMesh::DecreaseAlpha(float fDT)
{
	// Decrease current Alpha
	m_fAlpha -= SKID_MARK_ALPHA_RATE * fDT;

	if(m_fAlpha <= 0.0f)
	{
		m_fAlpha = 0.0f;
		m_bActive = false;
	}

	D3DCOLOR dwColor = D3DCOLOR_ARGB((DWORD)m_fAlpha, 255, 255, 255);

	int nNumVerts = m_tVertInfo.m_vColors.size();
	for(int i = 0; i < nNumVerts; ++i)
	{
		m_tVertInfo.m_vColors[i] = dwColor;
	}

	// Set Vertex Buffer
	SetColoredTexturedVertexInfo(&m_tVertInfo);
}

////////////////////////////////////////////////////////////////////////////////
// DRIFT
////////////////////////////////////////////////////////////////////////////////
void CSkidMarks::DriftCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CSkidMarks* pSM = (CSkidMarks*)pComp;
	pSM->Drift((TInputEvent*)pEvent->GetData());
}
void CSkidMarks::Drift(TInputEvent* pcObjEvent)
{
	// Check if we are Drifting
	if(CheckForPlayerMatch(m_pParentObj, pcObjEvent->m_pPlayer))//pcObjEvent->m_pPlayer == GetParent())
	{
		m_bDrift = true;

		if(m_pLDustEffect)
		{
			m_pLDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, false);
			m_pRDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, false);
		}

		if(m_pGlowEffect)
		{
			m_pGlowEffect->SwitchOnOffEmitters(EC_TYPE_SCIENTIST_GLOW, false);
		}
	}
}

// Accelerate
void CSkidMarks::AccelerateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CSkidMarks* pSM = (CSkidMarks*)pComp;
	pSM->Accelerate((TInputEvent*)pEvent->GetData());
}
void CSkidMarks::Accelerate(TInputEvent* pcObjEvent)
{
	// Check if we are Drifting
	if(CheckForPlayerMatch(m_pParentObj, pcObjEvent->m_pPlayer))//pcObjEvent->m_pPlayer == GetParent())
	{
		if(pcObjEvent->m_fAmount > 0.0f)
		{
			m_bAccelerating = true;

			//if(m_pLDustEffect)
			//{
			//	m_pLDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, false);
			//	m_pRDustEffect->SwitchOnOffEmitters(EC_TYPE_CART_SMOKE, false);
			//}

			//if(m_pGlowEffect)
			//{
			//	m_pGlowEffect->SwitchOnOffEmitters(EC_TYPE_SCIENTIST_GLOW, false);
			//}
		}

		
	}
}

///////////////////////////////////////////////////////////////////////////////
// CLEAN UP
///////////////////////////////////////////////////////////////////////////////

// Destroy Object
void CSkidMarks::DestroyObjectCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CSkidMarks* pSMC = (CSkidMarks*)pComp;
	TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();
	
	if(pSMC->GetParent()->GetID() == pData->m_pcObj->GetID())
	{
		//pSMC->m_bActive = false;
		//pIVFXComp->m_pEffect->SwitchOnOffEmitters(EC_TYPE_CHICKEN_INV, false);
		pSMC->Shutdown();
		pSMC->m_pGlowEffect = NULL;
		pSMC->m_pLDustEffect = NULL;
		pSMC->m_pRDustEffect = NULL;
	}
}