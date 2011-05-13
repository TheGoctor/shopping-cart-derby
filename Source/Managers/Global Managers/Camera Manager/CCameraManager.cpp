///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCameraManager.cpp"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/12/11
//
//	Last Changed	:	05/09/11
//
//	Changed By		:	HN
//
//  Purpose			:	A manager for cameras
///////////////////////////////////////////////////////////////////////////////
#include "../../../Managers/Global Managers/Event Manager/CEventManager.h"
#include "../../../../Source/CObject.h"
#include "../../../Managers/Global Managers/Event Manager/EventStructs.h"
using namespace EventStructs;

#include "CCameraManager.h"

// Initalize
void CCameraManager::Init(int nScreenWidth, int nScreenHeight)
{
	// Setup camera.
	m_pCam = CCamera::GetInstance();
	m_pCam->BuildPerspective(D3DX_PI * 0.5f, (float)nScreenWidth/(float)nScreenHeight,
							1.0f, 100.0f);
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("AttachToCamera", (IComponent*)GetInstance(), AttachToCamCallback);
	pEM->RegisterEvent("UpdateState", (IComponent*)GetInstance(), UpdateCallback);
}

// Attach To Camera
void CCameraManager::AttachToCamCallback(IEvent* e, IComponent*)
{
	TObjectEvent* eObj = static_cast<TObjectEvent*>(e->GetData());

	GetInstance()->m_pCam->m_Target.SetParent(eObj->m_pcObj->GetTransform());
	GetInstance()->m_pCam->SetTarget(&GetInstance()->m_pCam->m_Target);
	GetInstance()->m_pCam->SetFrameParent(eObj->m_pcObj->GetTransform());
	GetInstance()->m_pCam->GetFrame().TranslateFrame(D3DXVECTOR3(0.0f, 4.0f, -2.0f));
	GetInstance()->m_pCam->GetTarget()->TranslateFrame(D3DXVECTOR3(0.0f, 0.0f, 5.0f));
}

void CCameraManager::UpdateCallback(IEvent* e, IComponent*)
{
	// Get the Values for the Event
	TUpdateStateEvent* pEvent = static_cast<TUpdateStateEvent*>(e->GetData());
	float fDt = pEvent->m_fDeltaTime;

	//// Update Camera
	GetInstance()->m_pCam->Update(fDt);
}