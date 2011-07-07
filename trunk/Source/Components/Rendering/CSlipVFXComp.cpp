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
		m_pLeftEffect->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);
		m_pRightEffect->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);
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