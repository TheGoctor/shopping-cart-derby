////////////////////////////////////////////////////////////////////////////////
//	File			:	CEffectComponent.cpp
//	Date			:	5/19/11
//	Mod. Date		:	5/19/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object have a Effect
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CEffectComponent.h"
#include "CRenderComponent.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
#include"..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\CParticleEmitter.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\CParticleEmitter.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\DXRenderContextManager.h"
#include"..\\..\\Managers\\Global Managers\\Rendering Managers\\DXRenderShape.h"

////////////////////////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////////////////////////
TEffect::TEffect(void) : m_eType(EC_TYPE_MIN), m_bOn(true), m_fCooldown(0.0f),
						 m_fLifespan(0.0f), m_fTimer(0.0f)
{
}
CEffectComponent::CEffectComponent(CObject* pParent) : m_pcParent(pParent)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();

	// Update
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	szEventName = "Update";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, this, WinStateCallback);

	// Exit Gameplay
	/*szEventName = "ShutdownObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, ShutdownCallback);*/

	// Destroy Obj
	pEM->RegisterEvent("DestroyObject", this, DestroyObjectCallback);
}

////////////////////////////////////////////////////////////////////////////////
// Destructors
////////////////////////////////////////////////////////////////////////////////
TEffect::~TEffect(void)
{
	m_cEmitterRenderComps.clear();
}
CEffectComponent::~CEffectComponent(void)
{
	m_cEffectMap.clear();
}

// Destroy Onj
void CEffectComponent::DestroyObjectCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CEffectComponent* pEC = (CEffectComponent*)pComp;
	TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();
	
	if(pEC->GetParent()->GetID() == pData->m_pcObj->GetID())
	{
		//pSMC->m_bActive = false;
		//pIVFXComp->m_pEffect->SwitchOnOffEmitters(EC_TYPE_CHICKEN_INV, false);
		pEC->Shutdown();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Handlers
////////////////////////////////////////////////////////////////////////////////
void TEffect::Update(float fDT)
{
	// Check for Timer
	if(m_fTimer > 0.0f)
	{
		m_fTimer -= fDT;
		if(m_fTimer <= 0.0f)
		{
			SwitchOnOffEmitters(true);
			m_fTimer = 0.0f;
		}
	}
	
	// Check if its off
	if(m_bOn == false)
	{
		return;
	}

	// Lifespan
	bool bCutoff = false;
	if(m_fLifespan > 0.0f)
	{
		m_fLifespan -= fDT;

		if(m_fLifespan <= 0.0f)
		{
			bCutoff = true;
		}
	}

	// Update and Render all the Emitters
	EmitterRenderCompMap::iterator emitterIter;
	emitterIter = m_cEmitterRenderComps.begin();
	while(emitterIter != m_cEmitterRenderComps.end())
	{
		// Get a Pointer to the Emitter
		CParticleEmitter* pPE = (*emitterIter).first;
		CRenderComponent* pRC = (*emitterIter).second;

		// Update
		if(pPE->IsOn())
		{
			pPE->UpdateEmitter(fDT);

			/*if(bFlash && m_eType == PE_TYPE_BOOST)
			{*/
				pRC->AddToRenderSet();
			//}
			//bFlash = !bFlash;
		}

		// Cutoff?
		if(bCutoff)
		{
			pPE->SetContinuous(false);
		}

		// On to the Next 1
		emitterIter++;
	}

	// Cooldown
	if(m_fCooldown > 0.0f)
	{
		m_fCooldown -= fDT;
	}
}
void CEffectComponent::Update(float fDT)
{
	EffectMap::iterator effectIter = m_cEffectMap.begin();
	while(effectIter != m_cEffectMap.end())
	{
		TEffect* pEffect = &(*effectIter).second;
		pEffect->Update(fDT);

		effectIter++;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Callbacks
////////////////////////////////////////////////////////////////////////////////
void CEffectComponent::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component
	CEffectComponent* pEC = (CEffectComponent*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Call Handler
	pEC->Update(fDT);
}

// Add Emitter
void CEffectComponent::AddEmitter(EEmitterCompType eType, CParticleEmitter* pEmitter,
								  CRenderComponent* pRenderComp, CFrame* pParentFrame,
								  EParticleEmitterType eEmitterType)
{
	//pEmitter->SetOn(true);
	pEmitter->SetParentFrame(pParentFrame);
	pEmitter->SetType(eEmitterType);

	// Look for existing Effect
	EffectMap::iterator effectIter = m_cEffectMap.find(eType);
	if(effectIter != m_cEffectMap.end())
	{
		(*effectIter).second.m_cEmitterRenderComps.insert(make_pair(pEmitter, pRenderComp));
		//m_cEffectMap[eType].m_cEmitterRenderComps.insert(make_pair(pEmitter, pRenderComp));
	}
	else
	{
		m_cEffectMap[eType].m_cEmitterRenderComps.insert(make_pair(pEmitter, pRenderComp));
	}
	//m_cEmitterRenderComps.insert(make_pair(pEmitter, pRenderComp));
}

// Switch Emiiter On/Off
void TEffect::SwitchOnOffEmitters(bool bOn)
{
	// Check for Cooldown
	if(bOn && m_fCooldown > 0.0f)
	{
		return;
	}

	m_bOn = bOn;

	// Turn on all Emitters
	EmitterRenderCompMap::iterator emitterIter;
	emitterIter = m_cEmitterRenderComps.begin();
	while(emitterIter != m_cEmitterRenderComps.end())
	{
		// Get a Pointer to the Emitter
		CParticleEmitter* pPE = (*emitterIter).first;

		pPE->SetOn(bOn);

		emitterIter++;
	}
}
void CEffectComponent::SwitchOnOffEmitters(EEmitterCompType eType, bool bOn)
{
	m_cEffectMap[eType].SwitchOnOffEmitters(bOn);
}

// Set On Target
void TEffect::SetOnTarget(bool bOnTarget)
{
	// Attach all Emitters to their Parent Frame
	EmitterRenderCompMap::iterator emitterIter;
	emitterIter = m_cEmitterRenderComps.begin();
	while(emitterIter != m_cEmitterRenderComps.end())
	{
		// Get a Pointer to the Emitter
		CParticleEmitter* pPE = (*emitterIter).first;

		pPE->SetIsOnTarget(bOnTarget);

		emitterIter++;
	}
}
void CEffectComponent::SetOnTarget(EEmitterCompType eType, bool bOnTarget)
{
	m_cEffectMap[eType].SetOnTarget(bOnTarget);
}

// Timer
void TEffect::SetETimer(float fTimer)
{
	m_fTimer = fTimer;
}
void CEffectComponent::SetEffectTimer(EEmitterCompType eType, float fTimer)
{
	m_cEffectMap[eType].SetETimer(fTimer);
}

// Set Continuous
void TEffect::SetContinuous(bool bContinuous)
{
	// Tell all emitter to change continuosity
	EmitterRenderCompMap::iterator emitterIter;
	emitterIter = m_cEmitterRenderComps.begin();
	while(emitterIter != m_cEmitterRenderComps.end())
	{
		// Get a Pointer to the Emitter
		CParticleEmitter* pPE = (*emitterIter).first;

		pPE->SetContinuous(bContinuous);

		emitterIter++;
	}
}
void CEffectComponent::SetContinuous(EEmitterCompType eType, bool bContinuous)
{
	m_cEffectMap[eType].SetContinuous(bContinuous);
}

// Set Parent Frame
void TEffect::SetParentFrame(CFrame* pFrame)
{
	// Tell all emitter to change parent frame
	EmitterRenderCompMap::iterator emitterIter;
	emitterIter = m_cEmitterRenderComps.begin();
	while(emitterIter != m_cEmitterRenderComps.end())
	{
		// Get a Pointer to the Emitter
		CParticleEmitter* pPE = (*emitterIter).first;

		pPE->SetParentFrame(pFrame);

		emitterIter++;
	}
}
void CEffectComponent::SetParentFrame(EEmitterCompType eType, CFrame* pFrame)
{
	m_cEffectMap[eType].SetParentFrame(pFrame);
}

// Change Context
void TEffect::ChangeContext(EParticleEmitterType eEmitterType, unsigned int uRenderContextIdx)
{
	// Get Context
	DXRenderContext* pContext = DXRenderContextManager::GetInstance()->
		GetContext((ERenderContext)uRenderContextIdx);

	// Tell all emitter to change continuosity
	EmitterRenderCompMap::iterator emitterIter;
	emitterIter = m_cEmitterRenderComps.begin();
	while(emitterIter != m_cEmitterRenderComps.end())
	{
		// Get a Pointer to the Emitter
		CParticleEmitter* pPE = (*emitterIter).first;
		CRenderComponent* pRC = (*emitterIter).second;

		if(pPE->GetType() == eEmitterType)
		{
			pRC->GetRenderShape()->SetRenderContext(pContext);
		}

		emitterIter++;
	}
}
void CEffectComponent::ChangeContext(EEmitterCompType eCompType, EParticleEmitterType eEmitterType, unsigned int uRenderContextIdx)
{
	m_cEffectMap[eCompType].ChangeContext(eEmitterType, uRenderContextIdx);
}

// On/Off
void CEffectComponent::ActivateCallback(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	// Get the Effect Comp
	//CEffectComponent* pEC = (CEffectComponent*)pComp;
	//pEC->m_bOn = true;
}
void CEffectComponent::DeactivateCallback(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	// Get the Effect Comp
	//CEffectComponent* pEC = (CEffectComponent*)pComp;
	//pEC->m_bOn = false;
}

void CEffectComponent::ShutdownCallback(IEvent* pEvent, IComponent* pComp)
{
	CEffectComponent* pEC = (CEffectComponent*)pComp;
	pEC->Shutdown();
}
void CEffectComponent::Shutdown(void)
{
	EffectMap::iterator effectIter = m_cEffectMap.begin();
	while(effectIter != m_cEffectMap.end())
	{
		TEffect* pEffect = &(*effectIter).second;
		pEffect->Shutdown();//SwitchOnOffEmitters(false);

		effectIter++;
	}

	
}

// Effect
void TEffect::Shutdown(void)
{
	CObjectManager* pOM = CObjectManager::GetInstance();

	// Tell all emitter to shutdown
	//EmitterRenderCompMap::iterator emitterIter;
	//emitterIter = m_cEmitterRenderComps.begin();
	while(!m_cEmitterRenderComps.empty())
	{
		// Get a Pointer to the Emitter
		//CParticleEmitter* pPE = (*emitterIter).first;
		CRenderComponent* pRC = (*m_cEmitterRenderComps.begin()).second;

		if(pRC->GetParent() && pRC->GetParent()->GetID())
		{
			pOM->DestroyObject(pRC->GetParent());
			CEventManager::GetInstance()->UnregisterEventAll(pRC);
			//pRC->GetParent()->RemoveComponent(pRC);
		}

		m_cEmitterRenderComps.erase(m_cEmitterRenderComps.begin());

		//emitterIter++;
	}
}

// Win State
void CEffectComponent::WinStateCallback(IEvent* pEvent, IComponent* pComp)
{
	CEffectComponent* pEC = (CEffectComponent*)pComp;
	pEC->WinState();
}
void CEffectComponent::WinState(void)
{
	// Tell all emitter to turn off except victory ones
	EffectMap::iterator effectIter = m_cEffectMap.begin();
	while(effectIter != m_cEffectMap.end())
	{
		EEmitterCompType eType = (*effectIter).first;
		TEffect* pEffect = &(*effectIter).second;

		if(eType != EC_TYPE_CONFETTI && eType != EC_TYPE_FIREWORK_TRAIL && eType != EC_TYPE_FIREWORK_BURST
			&& eType != EC_TYPE_HELD_ITEM && eType != EC_TYPE_SCIENTIST_GLOW)
		{
			pEffect->SwitchOnOffEmitters(false);
		}

		effectIter++;
	}
}
