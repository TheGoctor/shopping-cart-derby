////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimatedDonutComp.cpp
//	Date			:	6/10/11
//	Mod. Date		:	6/10/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Animated Donut
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CAnimatedDonutComp.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CEffectManager.h"
#include "..\\..\\Managers\\Component Managers\\CAnimateManager.h"

// Defines
#define DONUT_PUFF_LIFESPAN  (0.1f)

// Initalize
void CAnimatedDonutComp::Init(void)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("DonutEffect", this, UseDonutCallback);
	pEM->RegisterEvent("DonutDespawn", this, DespawnDonutCallback);

	string szEventName = "ShutdownObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, ShutdownCallBack);

	// Wining?
	szEventName = "InitObjects";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, (IComponent*)this, WinInitCallback);

	// Update
	szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	// 
	//string szObjName = "DonutComponent";
	//szObjName += nDonutsCreated + '0';

	//// Get Animate Component
	m_pAnimComp = CAnimateManager::GetInstance()->GetComponent(m_pParentObj->GetID());
	m_pParentObj->GetTransform()->ScaleFrame(1.25f, 1.25f, 1.25f);
	m_pParentObj->GetTransform()->TranslateFrame(D3DXVECTOR3(0.0f, 0.0f, -0.75f));
	m_pAnimComp->SetOn(false);
	m_pAnimComp->SetIsActive(false);

	m_pPuffEffect = CEffectManager::GetInstance()->CreateDonutUseComponent(m_pParentObj);
}

// Shutdown
void CAnimatedDonutComp::ShutdownCallBack(IEvent*, IComponent* pComp)
{
	CAnimatedDonutComp* pDonut = (CAnimatedDonutComp*)pComp;
	pDonut->Shutdown();
}

void CAnimatedDonutComp::Shutdown(void)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();

	//if(m_pParentObj->GetID())
	//	m_pParentObj->RemoveComponent(this);

	pEM->UnregisterEventAll(this);
}

// Factory
int CAnimatedDonutComp::CreateAnimatedDonutComponent(lua_State* pLUA)
{
	// Get Data from Lua
	CObject* pObj = (CObject*)lua_topointer(pLUA, -1);

	// Pop that Solda Boi!
	lua_pop(pLUA, 1);

	// Create the Component
	CreateAnimatedDonutComponent(pObj);

	// Do not Return anythin to LUA
	return 0;
}

// Helper Funcs
bool CAnimatedDonutComp::CheckForPlayerMatch(CObject* pOtherObj)
{
	// Get Object Names
	char* szEventObjName = (char*)pOtherObj->GetID();
	char* szCompObjName  = (char*)m_pParentObj->GetID();

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

// Callbacks

// Use
void CAnimatedDonutComp::UseDonutCallback(IEvent* pEvent, IComponent* pComp)
{
	CAnimatedDonutComp* pADC = (CAnimatedDonutComp*)pComp;
	TStatusEffectEvent* pData = (TStatusEffectEvent*)pEvent->GetData();

	pADC->UseDonut(pData);
}
void CAnimatedDonutComp::UseDonut(TStatusEffectEvent* pcObjEvent)
{
	if(CheckForPlayerMatch(pcObjEvent->m_pObject))
	{
		m_pAnimComp->SetOn(true);
		m_pAnimComp->SetIsActive(true);

		if(m_fDuration <= 0.0f)
		{
			m_pPuffEffect->SwitchOnOffEmitters(EC_TYPE_DONUT_USE, true);
			m_pPuffEffect->SetLifespan(EC_TYPE_DONUT_USE, DONUT_PUFF_LIFESPAN);
		}

		m_fDuration = pcObjEvent->m_fDuration;

	}
}

// Despawn
void CAnimatedDonutComp::DespawnDonutCallback(IEvent* pEvent, IComponent* pComp)
{
	CAnimatedDonutComp* pADC = (CAnimatedDonutComp*)pComp;
	TStatusEffectEvent* pData = (TStatusEffectEvent*)pEvent->GetData();

	pADC->DespawnDonut(pData);
}
void CAnimatedDonutComp::DespawnDonut(TStatusEffectEvent* pcObjEvent)
{
	if(CheckForPlayerMatch(pcObjEvent->m_pObject))
	{
		m_pAnimComp->SetOn(false);
		m_pAnimComp->SetIsActive(false);
		m_pPuffEffect->SwitchOnOffEmitters(EC_TYPE_DONUT_USE, true);
		m_pPuffEffect->SetLifespan(EC_TYPE_DONUT_USE, DONUT_PUFF_LIFESPAN);
		m_fDuration = 0.0f;
	}
}

void CAnimatedDonutComp::WinInitCallback(IEvent*, IComponent* pComp)
{
	CAnimatedDonutComp* pDC = (CAnimatedDonutComp*)pComp;
		
	pDC->m_pAnimComp->SetOn(false);
	pDC->m_pAnimComp->SetIsActive(false);
}

void CAnimatedDonutComp::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CAnimatedDonutComp* pDonutComp = (CAnimatedDonutComp*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	if(pDonutComp->m_fDuration > 0.0f)
	{
		pDonutComp->m_fDuration -= fDT;

		if(pDonutComp->m_fDuration <= 0.0f)
		{
			pDonutComp->m_pAnimComp->SetOn(false);
			pDonutComp->m_pAnimComp->SetIsActive(false);
			pDonutComp->m_pPuffEffect->SwitchOnOffEmitters(EC_TYPE_DONUT_USE, true);
			pDonutComp->m_pPuffEffect->SetLifespan(EC_TYPE_DONUT_USE, DONUT_PUFF_LIFESPAN);
			pDonutComp->m_fDuration = 0.0f;
		}
	}
}