////////////////////////////////////////////////////////////////////////////////
//	File			:	CSlipVFXComp.cpp
//	Date			:	6/26/11
//	Mod. Date		:	6/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Slip VFX
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CSlipVFXComp.h"
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CEffectManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"

// Initalize
void CSlipVFXComp::Init(void)
{
	// Get Singletons
	CEventManager* pEM = CEventManager::GetInstance();
	CEffectManager* pEffMan = CEffectManager::GetInstance();

	// Create Drips
	CreateLeftDrip();
	CreateRightDrip();

	// Register for Events

	// Slip
	string szEventName = "SlipEffect";
	pEM->RegisterEvent(szEventName, this, SlipCallback);

	// Slow
	szEventName = "SlowEffect";
	pEM->RegisterEvent(szEventName, this, SlowCallback);

	// Update
	szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	// Destroy Obj
	//pEM->RegisterEvent("DestroyObject", this, DestroyObjectCallback);
}

// Shutdown
void CSlipVFXComp::Shutdown(void)
{
	CEventManager* pEM = CEventManager::GetInstance();
	CObjectManager* pOM = CObjectManager::GetInstance();

	pOM->DestroyObject(m_pLeftEmitter);
	pOM->DestroyObject(m_pRightEmitter);

	// Unregister for Events
	pEM->UnregisterEventAll(this);
}

// Helper Funcs
void CSlipVFXComp::CreateLeftDrip(void)
{
	// Get Singletons
	CObjectManager* pOM = CObjectManager::GetInstance();
	CEffectManager* pEffMan = CEffectManager::GetInstance();

	// Create Effect Obj
	string szObjName = (char*)m_pParentObj->GetID();
	szObjName += "SlipDripLeft";
	m_pLeftEmitter = pOM->CreateObject(szObjName);
	pOM->BindObjects(m_pParentObj, m_pLeftEmitter);
	m_pLeftEmitter->GetTransform()->TranslateFrame(D3DXVECTOR3(1.0f, 0.0f, 0.0f));

	// Create Effect Comp
	m_pLeftEffect = pEffMan->CreateSlipDripComp(m_pLeftEmitter, true);
}
void CSlipVFXComp::CreateRightDrip(void)
{
	// Get Singletons
	CObjectManager* pOM = CObjectManager::GetInstance();
	CEffectManager* pEffMan = CEffectManager::GetInstance();

	// Create Effect Obj
	string szObjName = (char*)m_pParentObj->GetID();
	szObjName += "SlipDripRight";
	m_pRightEmitter = pOM->CreateObject(szObjName);
	pOM->BindObjects(m_pParentObj, m_pRightEmitter);
	m_pRightEmitter->GetTransform()->TranslateFrame(D3DXVECTOR3(-1.0f, 0.0f, 0.0f));

	// Create Effect Comp
	m_pRightEffect = pEffMan->CreateSlipDripComp(m_pRightEmitter, false);
}

// Slip
void CSlipVFXComp::SlipCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CSlipVFXComp* pSVFXC = (CSlipVFXComp*)pComp;
	pSVFXC->Slip((TStatusEffectEvent*)pEvent->GetData());
}
void CSlipVFXComp::Slip(TStatusEffectEvent* pcEvent)
{
	// Did we Slip
	if(pcEvent->m_pObject->GetID() == m_pParentObj->GetID())
	{
		m_pLeftEffect->ChangeContext(EC_TYPE_BANANA_SLIP, PE_TYPE_BANANA_DRIP, RC_DRIP_LEFT);
		m_pRightEffect->ChangeContext(EC_TYPE_BANANA_SLIP, PE_TYPE_BANANA_DRIP, RC_DRIP_RIGHT);
		m_pLeftEffect->SetDeadTimer(EC_TYPE_BANANA_SLIP, 0.0f);
		m_pRightEffect->SetDeadTimer(EC_TYPE_BANANA_SLIP, 0.0f);
		
		m_pLeftEffect->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);
		m_pRightEffect->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);
	}
}

// Slow
void CSlipVFXComp::SlowCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CSlipVFXComp* pSVFXC = (CSlipVFXComp*)pComp;
	pSVFXC->Slow((TStatusEffectEvent*)pEvent->GetData());
}
void CSlipVFXComp::Slow(TStatusEffectEvent* pcEvent)
{
	// Did we Slow
	if(pcEvent->m_pObject->GetID() == m_pParentObj->GetID())// && m_fSlowCooldown == 0.0f)
	{
		if(m_pLeftEffect->GetCooldown(EC_TYPE_BANANA_SLIP) > 0.0f)
			return;

		//m_fSlowCooldown = 3.0f;
		m_pLeftEffect->ChangeContext(EC_TYPE_BANANA_SLIP, PE_TYPE_BANANA_DRIP, RC_PB_DRIP_LEFT);
		m_pRightEffect->ChangeContext(EC_TYPE_BANANA_SLIP, PE_TYPE_BANANA_DRIP, RC_PB_DRIP_RIGHT);
		m_pLeftEffect->SetDeadTimer(EC_TYPE_BANANA_SLIP, 0.0f);
		m_pRightEffect->SetDeadTimer(EC_TYPE_BANANA_SLIP, 0.0f);
		
		m_pLeftEffect->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);
		m_pRightEffect->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);
		m_pLeftEffect->SetCooldown(EC_TYPE_BANANA_SLIP, 0.5f);
		m_pRightEffect->SetCooldown(EC_TYPE_BANANA_SLIP, 0.5f);
	}
}

// Update
void CSlipVFXComp::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CSlipVFXComp* pSM = (CSlipVFXComp*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Update Component
	pSM->Update(fDT);
}
void CSlipVFXComp::Update(float fDT)
{
	if(m_fSlowCooldown > 0.0f)
	{
		m_fSlowCooldown -= fDT;

		if(m_fSlowCooldown > 0.0f)
			m_fSlowCooldown = 0.0f;
	}
}

// Destroy Object
void CSlipVFXComp::DestroyObjectCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CSlipVFXComp* pSVFXC = (CSlipVFXComp*)pComp;
	TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();
	
	if(pSVFXC->GetParent()->GetID() == pData->m_pcObj->GetID())
	{
		pSVFXC->Shutdown();
	}
}