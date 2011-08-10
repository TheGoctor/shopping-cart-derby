////////////////////////////////////////////////////////////////////////////////
//	File			:	CInvulnerableVFX.cpp
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Chiken Soup VFX
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CInvulnerableVFX.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CEffectManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\DXMesh.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"

// Defines
#define INV_FLASH_RATE	   (4.0f)
#define BOOST_ANIME_SWITCH (0.1f)

// Initalize
void CInvulnerableVFX::Init(void)
{
	// Create Effects
	CEffectManager* pEffMan = CEffectManager::GetInstance();
	m_pEffect = pEffMan->CreateInvulnerableEffectComponent(m_pParentObj);

	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	
	// Invulnerable
	pEM->RegisterEvent("Invulnerable", this, InvulnerableCallback);

	// Boost
	pEM->RegisterEvent("UseBoost", this, BoostCallback);

	// Update
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	// Wining?
	szEventName = "Update";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, this, WinStateInitCallback);

	// Destroy Obj
	pEM->RegisterEvent("DestroyObject", this, DestroyObjectCallback);
}

// Factory
int CInvulnerableVFX::CreateInvulnerableVFXComponent(lua_State* pLUA)
{
	// Get Data from Lua
	CObject* pObj = (CObject*)lua_topointer(pLUA, -2);
	DXMesh* pMesh = (DXMesh*)lua_topointer(pLUA,  -1);

	// Pop that Solda Boi!
	lua_pop(pLUA, 2);

	// Create the Component
	CreateInvulnerableVFXComponent(pObj, pMesh);

	// Do not Return anythin to LUA
	return 0;
}

// Helper Funcs
bool CInvulnerableVFX::CheckForPlayerMatch(CObject* pOtherObj)
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

// Invulnerable
void CInvulnerableVFX::InvulnerableCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Data from Event
	CInvulnerableVFX* pIVFXComp = (CInvulnerableVFX*)pComp;
	TStatusEffectEvent* pData = (TStatusEffectEvent*)pEvent->GetData();

	// Call Handler
	pIVFXComp->Invulnerable(pData);
}
void CInvulnerableVFX::Invulnerable(TStatusEffectEvent* pcObjEvent)
{
	if(CheckForPlayerMatch(pcObjEvent->m_pObject))
	{
		// Turn On
		m_bActive = true;
		m_pMesh->SetFlashingYellow(true);
		m_pEffect->SetContinuous(EC_TYPE_CHICKEN_INV, true);
		m_pEffect->SwitchOnOffEmitters(EC_TYPE_CHICKEN_INV, true);
		m_fInvDuration = pcObjEvent->m_fDuration;
	}
}

// Boost
void CInvulnerableVFX::BoostCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Data from Event
	CInvulnerableVFX* pIVFXComp = (CInvulnerableVFX*)pComp;
	TStatusEffectEvent* pData = (TStatusEffectEvent*)pEvent->GetData();

	// Call Handler
	pIVFXComp->Boost(pData);
}
void CInvulnerableVFX::Boost(TStatusEffectEvent* pcObjEvent)
{
	if(CheckForPlayerMatch(pcObjEvent->m_pObject))
	{
		// Turn On
		//m_bActive = true;
		m_pMesh->SetFlashingBlue(true);
		m_fBoostDuration = pcObjEvent->m_fDuration;
	}
}

// Update
void CInvulnerableVFX::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CInvulnerableVFX* pIVFXComp = (CInvulnerableVFX*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Update Component
	pIVFXComp->Update(fDT);
}
void CInvulnerableVFX::Update(float fDT)
{
	// Boost
	if(m_fBoostDuration > 0.0f)
	{
		m_fBoostDuration  -= fDT;

		if(m_fBoostDuration > 0.0f)
		{
			m_fBoostAnimTimer += fDT;

			if(m_fBoostAnimTimer > BOOST_ANIME_SWITCH)
			{
				if(m_pMesh->IsFlashingBlue())
				{
					m_pMesh->SetFlashingBlue(false);
					m_pMesh->SetFlashingWhite(true);
				}
				else
				{
					m_pMesh->SetFlashingBlue(true);
					m_pMesh->SetFlashingWhite(false);
				}

				m_fBoostAnimTimer = 0.0f;
			}
		}
		else
		{
			m_fBoostDuration = 0.0f;
			m_pMesh->SetFlashingBlue(false);
			m_pMesh->SetFlashingWhite(false);
		}
	}
	else
	{
		m_pMesh->SetFlashingBlue(false);
		m_pMesh->SetFlashingWhite(false);
	}

	// Inv
	if(m_bActive == false)
		return;

	m_fInvDuration -= fDT;

	if(m_fInvDuration > 0.0f)
	{
		float fFlashRate = m_pMesh->GetFlashRate();

		if(m_fDir == 1.0f)
			fFlashRate += fDT * INV_FLASH_RATE;
		else
			fFlashRate -= fDT * INV_FLASH_RATE;

		if(fFlashRate > 1.0f)
		{
			fFlashRate = 1.0f;
			m_fDir *= -1.0f;
		}
		else if(fFlashRate < 0.3f)
		{
			fFlashRate = 0.3f;
			m_fDir *= -1.0f;
		}

		m_pMesh->SetFlashRate(fFlashRate);
	}
	else
	{
		m_fInvDuration = 0.0f;
		m_bActive = false;
		m_pMesh->SetFlashingYellow(false);
		m_pEffect->SetContinuous(EC_TYPE_CHICKEN_INV, false);
	}
}

// Win State
void CInvulnerableVFX::WinStateInitCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CInvulnerableVFX* pIVFXComp = (CInvulnerableVFX*)pComp;
	pIVFXComp->TurnOff();
}
void CInvulnerableVFX::TurnOff(void)
{
	m_pMesh->SetFlashingYellow(false);
	m_pMesh->SetFlashingBlue(false);
	m_pMesh->SetFlashingWhite(false);
}

// Destroy Object
void CInvulnerableVFX::DestroyObjectCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CInvulnerableVFX* pIVFXComp = (CInvulnerableVFX*)pComp;
	TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();
	
	if(pIVFXComp->GetParent()->GetID() == pData->m_pcObj->GetID())
	{
		CEventManager::GetInstance()->UnregisterEventAll(pComp);
	}
}