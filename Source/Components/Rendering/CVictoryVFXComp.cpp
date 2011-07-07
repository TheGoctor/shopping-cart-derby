////////////////////////////////////////////////////////////////////////////////
//	File			:	CVictoryVFXComp.cpp
//	Date			:	6/10/11
//	Mod. Date		:	6/10/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Victory VFX
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CVictoryVFXComp.h"
#include "..\\..\\Managers\\Global Managers\\Object Manager\\CObjectManager.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CEffectManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include "..\\..\\Managers\\Global Managers\\Sound Manager\\CWwiseSoundManager.h"
// Defines
#define FIREWORK_RISE_RATE (D3DXVECTOR3(0.0f, 3.0f, 0.0f))
#define FIREWORK_Y_CAP     (3.5f)

// Initalize
void CVictoryVFXComp::Init(void)
{
	// Get Singletons
	CEventManager* pEM = CEventManager::GetInstance();
	CEffectManager* pEffMan = CEffectManager::GetInstance();

	// Create Effects
	for(int firework = 0; firework < MAX_FIREWORKS; ++firework)
	{
		m_tFireworks[firework].Init(m_pParentObj);
	}
	//m_tFirework.Init(m_pParentObj);
	//m_tFirework2.Init(m_pParentObj);
	
	m_pConfettiEffect = pEffMan->CreateConfettiEffectComponent(m_pParentObj);
	//m_pConfettiEffect->SwitchOnOffEmitters(EC_TYPE_CONFETTI, true);

	// Register for Events
	string szEventName = "Update";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	szEventName = "HumanWinEffect";
	pEM->RegisterEvent(szEventName, this, PlayerWonCallback);

	szEventName = "ShutdownObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, ShutdownCallback);

	// Destroy Obj
	//pEM->RegisterEvent("DestroyObject", this, DestroyObjectCallback);
}

// Firework
void TFirework::Init(CObject* pParent)
{
	// Get Singletons
	static int nCount = 0;
	CObjectManager* pOM = CObjectManager::GetInstance();
	CEffectManager* pEffMan = CEffectManager::GetInstance();

	// Create Effect Obj
	string szObjName = (char*)pParent->GetID();
	szObjName += "FireworkEmitter";
	szObjName += nCount;
	m_pEmitterObj = pOM->CreateObject(szObjName);
	pOM->BindObjects(pParent, m_pEmitterObj);

	// Set Random Pos
	D3DXVECTOR3 vVec;
	vVec.x = RAND_FLOAT(-2.0f, 2.0f);
	vVec.y = 0.3f;
	vVec.z = RAND_FLOAT(-2.0f, 2.0f);
	m_pEmitterObj->GetTransform()->TranslateFrame(vVec);

	// Y Cap
	m_fYCap = RAND_FLOAT(2.0f, FIREWORK_Y_CAP);

	//m_pEmitterObj->GetTransform()->TranslateFrame(D3DXVECTOR3(-1.0f, 0.3f, 1.0f));

	m_pEffect = pEffMan->CreateFireworkEffectComponent(m_pEmitterObj);
	nCount++;
	//m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_TRAIL, true);
}

// Shutdown
void CVictoryVFXComp::Shutdown(void)
{
	CObjectManager* pOM = CObjectManager::GetInstance();
	CEventManager::GetInstance()->UnregisterEventAll(this);

	for(int firework = 0; firework < MAX_FIREWORKS; ++firework)
	{
		pOM->DestroyObject(m_tFireworks[firework].m_pEmitterObj);
	}
}

// Helper Funcs
void TFirework::ResetFirework(void)
{
	// Get the Frame
	CFrame* pEmitterFrame = m_pEmitterObj->GetTransform();

	// Set Pos Back
	D3DXVECTOR3 vVec = pEmitterFrame->GetLocalPosition();
	vVec *= -1.0f;
	pEmitterFrame->TranslateFrame(vVec);

	// Set Random Pos
	vVec.x = RAND_FLOAT(-2.0f, 2.0f);
	vVec.y = 0.3f;
	vVec.z = RAND_FLOAT(-2.0f, 2.0f);
	pEmitterFrame->TranslateFrame(vVec);

	// Find a Random Firwork Context
	ERenderContext eRenderConxtext;
	switch(rand() % 4)
	{
	case 0:
		eRenderConxtext = RC_FIREWORK_BLUE;		
		break;
	case 1:
		eRenderConxtext = RC_FIREWORK_GREEN;		
		break;
	case 2:
		eRenderConxtext = RC_FIREWORK_BLACK;		
		break;
	case 3:
		eRenderConxtext = RC_FIREWORK_ORANGE;		
		break;
	default:
		eRenderConxtext = RC_FIREWORK_BLUE;
		break;
	}
	m_pEffect->ChangeContext(EC_TYPE_FIREWORK_TRAIL, PE_TYPE_FIRE_WORK_TRAIL, eRenderConxtext);
	m_pEffect->ChangeContext(EC_TYPE_FIREWORK_BURST, PE_TYPE_FIRE_WORK_BURST, eRenderConxtext);

	m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_TRAIL, true);
	m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_BURST, false);
	m_fTimer = 0.0f;
	m_bRising = true;
	m_fYCap = RAND_FLOAT(2.0f, FIREWORK_Y_CAP);
}

// Update
void CVictoryVFXComp::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CVictoryVFXComp* pVVFXomp = (CVictoryVFXComp*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Update Component
	pVVFXomp->Update(fDT);
}
void CVictoryVFXComp::Update(float fDT)
{
	if(m_bActive == false)
		return;

	for(int firework = 0; firework < MAX_FIREWORKS; ++firework)
	{
		m_tFireworks[firework].Update(fDT);
	}
	//m_tFirework.Update(fDT);
	//m_tFirework2.Update(fDT);
}

// Firework
void TFirework::Update(float fDT)
{
	// Should we Reset the Fireworks
	if(m_fTimer > 0.0f)
	{
		m_fTimer -= fDT;
		if(m_fTimer < 0.0f)
		{
			ResetFirework();
		}
	}

	// Get the Frame
	CFrame* pEmitterFrame = m_pEmitterObj->GetTransform();

	if(m_bRising)
	{
		// Reached Apex
		if(pEmitterFrame->GetWorldPosition().y >= m_fYCap)
		{
			m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_TRAIL, false);
			m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_BURST, true);
			CWwiseSoundManager::GetInstance()->PlayTheSound(WINSTATE_EXPLOSION, GLOBAL_ID);
			m_fTimer = RAND_FLOAT(0.75f, 1.25f);
			m_bRising = false;
		}
		else
		{
			// Move the Emitter up up
			pEmitterFrame->TranslateFrame(FIREWORK_RISE_RATE * fDT);
		}
	}
}

void CVictoryVFXComp::PlayerWonCallback(IEvent* pEvent, IComponent* pComp)
{
	CVictoryVFXComp* pVFX = (CVictoryVFXComp*)pComp;

	pVFX->m_pConfettiEffect->SwitchOnOffEmitters(EC_TYPE_CONFETTI, true);
	
	for(int firework = 0; firework < MAX_FIREWORKS; ++firework)
	{
		pVFX->m_tFireworks[firework].ResetFirework();
	}
	//pVFX->m_tFirework.ResetFirework();
	//pVFX->m_tFirework2.ResetFirework();

	pVFX->m_bActive = true;
}

void CVictoryVFXComp::ShutdownCallback(IEvent* pEvent, IComponent* pComp)
{
	CVictoryVFXComp* pVFX = (CVictoryVFXComp*)pComp;

	pVFX->m_pConfettiEffect->SwitchOnOffEmitters(EC_TYPE_CONFETTI, false);

	for(int firework = 0; firework < MAX_FIREWORKS; ++firework)
	{
		pVFX->m_tFireworks[firework].m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_TRAIL, false);
		pVFX->m_tFireworks[firework].m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_BURST, false);
	}
	//pVFX->m_tFirework.m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_TRAIL, false);
	//pVFX->m_tFirework.m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_BURST, false);
	//pVFX->m_tFirework2.m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_TRAIL, false);
	//pVFX->m_tFirework2.m_pEffect->SwitchOnOffEmitters(EC_TYPE_FIREWORK_BURST, false);

	pVFX->m_bActive = false;
}

void CVictoryVFXComp::DestroyObjectCallback(IEvent* pEvent, IComponent* pComp)
{
	CVictoryVFXComp* pVFX = (CVictoryVFXComp*)pComp;
	TObjectEvent* pObjEvent = (TObjectEvent*)pEvent->GetData();

	if(pVFX->GetParent()->GetID() == pObjEvent->m_pcObj->GetID())
	{
		pVFX->Shutdown();
	}
}