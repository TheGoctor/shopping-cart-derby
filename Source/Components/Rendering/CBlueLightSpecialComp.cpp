////////////////////////////////////////////////////////////////////////////////
//	File			:	CBlueLightSpecialComp.cpp
//	Date			:	7/2/11
//	Mod. Date		:	7/2/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Blue Light Special for Held Items
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CBlueLightSpecialComp.h"
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\Renderer.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\ModelManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\DXRenderShape.h"

// Defines
#define BLS_POS_OFFSET 		(D3DXVECTOR3(-0.5f, 3.25f, 0.0f))
#define BLS_ROT_AXIS   		(D3DXVECTOR3(0.0f, 1.0f, 0.0f))
#define BLS_ROT_RATE   		(10.0f)
#define BLS_TEX_SWITCH_TIME (0.5f)

// Initalize
void CBlueLightSpecialComp::Init(void)
{
	// Get Singletons
	CObjectManager* pOM = CObjectManager::GetInstance();
	Renderer* pRM		= Renderer::GetInstance();
	ModelManager* pMM   = ModelManager::GetInstance();
	CEventManager* pEM  = CEventManager::GetInstance();

	// Create Blue Light Obj
	string szObjName = "BlueLightSpecial";
	szObjName += (char*)m_pParentObj->GetID();
	m_pBLSObj = pOM->CreateObject(szObjName);
	
	// Position Light Relative to Endcap
	pOM->BindObjects(m_pParentObj, m_pBLSObj);
	m_pBLSObj->GetTransform()->TranslateFrame(BLS_POS_OFFSET);

	// Get Blue Light Mesh
	int nMeshIdx = pMM->GetMeshIndexByName("FFP_3D_BlueLight_FINShape",
		false, false);

	// Create Render Comp
	m_pRenComp = pRM->CreateRenderComp(m_pBLSObj, nMeshIdx,
		RC_BLUE_LIGHT_SPECIAL, RF_TEXTURE_CHANGE_INST);

	// Register for Events

	// Update
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);
}

// Shutdown
void CBlueLightSpecialComp::Shutdown(void)
{
}

// Update
void CBlueLightSpecialComp::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CBlueLightSpecialComp* pBLSComp = (CBlueLightSpecialComp*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Update Component
	if(pBLSComp->IsActive())
		pBLSComp->Update(fDT);
}
void CBlueLightSpecialComp::Update(float fDT)
{
	// Update Timers
	m_fRot += fDT * BLS_ROT_RATE;
	m_fTexSwitchTimer += fDT;

	// Change Texture
	if(m_fTexSwitchTimer >= BLS_TEX_SWITCH_TIME)
	{
		if(m_pRenComp->GetRenderShape()->GetTexIdx() == 1)
			m_pRenComp->GetRenderShape()->SetTexIdx(2);
		else
			m_pRenComp->GetRenderShape()->SetTexIdx(1);

		m_fTexSwitchTimer = 0.0f;
	}
	m_pEndcapRenComp->GetRenderShape()->SetTexIdx(2);

	/*if(m_fRot >= D3DX_PI * 2.0f)
	{
		m_fRot = 0.0f;
	}*/

	CFrame* pFrame = m_pBLSObj->GetTransform();
	//D3DXVECTOR3 vPos = pFrame->GetLocalPosition();
	//pFrame->TranslateFrame(-pFrame->GetLocalPosition());
	//D3DXVECTOR3 vRotVec = pFrame->GetLocalPosition();
	//vRotVec.x = 0.0f;
	//vRotVec.z = 0.0f;
	pFrame->RotateFrame(BLS_ROT_AXIS, m_fRot);
	//pFrame->TranslateFrame(vPos);

	// Render
	m_pRenComp->AddToRenderSet();
}
