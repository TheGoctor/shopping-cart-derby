///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCameraManager.cpp"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/12/11
//
//	Last Changed	:	07/26/11
//
//	Changed By		:	HN
//
//  Purpose			:	A manager for cameras
///////////////////////////////////////////////////////////////////////////////
#include "../Event Manager/CEventManager.h"
#include "../Event Manager/EventStructs.h"
#include "../Object Manager/CObjectManager.h"
#include "../../Component Managers/CCollisionManager.h"
#include "../../../../Source/CObject.h"
using namespace EventStructs;

#include "CCameraManager.h"

// Initalize
void CCameraManager::Init(int nScreenWidth, int nScreenHeight)
{
	// Setup camera.
	CObject* pCam = CObjectManager::GetInstance()->CreateObject("Camera0");

	m_pCam = MMNEW(CCamera);

	//BuildPerspective(field of view, aspect ratio, near plane, far plane)
	m_pCam->BuildPerspective(D3DX_PI/3.0f, (float)nScreenWidth/(float)nScreenHeight,
							1.0f, 200.0f);
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("AttachToCamera", (IComponent*)GetInstance(), AttachToCamCallback);
	pEM->RegisterEvent("UpdateState", (IComponent*)GetInstance(), UpdateCallback);
	pEM->RegisterEvent("AttachToCameraWin", (IComponent*)GetInstance(), AttachToWinStateCallback);
	pEM->RegisterEvent("AttachToCameraLose",  (IComponent*)GetInstance(), AttachToLoseStateCallback);
}

// Attach To Camera

int CCameraManager::AttachCamToPlayer(lua_State* pLua)
{
	string szObjName = lua_tostring(pLua, -1);
	lua_pop(pLua, 1);

	CObject* pObj = CObjectManager::GetInstance()->GetObjectByName(szObjName);

	if(pObj == NULL)
	{
		Debug.Print("Invalid Object Name");
		return 0;
	}

	GetInstance()->m_pCam->GetTarget1().SetParent(pObj->GetTransform());
	GetInstance()->m_pCam->SetTarget(&GetInstance()->m_pCam->GetTarget1());
	GetInstance()->m_pCam->SetFrameParent(pObj->GetTransform());

	return 0;
}

void CCameraManager::AttachToCamCallback(IEvent* e, IComponent*)
{
	TObjectEvent* eObj = static_cast<TObjectEvent*>(e->GetData());

	D3DXMatrixIdentity(&GetInstance()->m_pCam->GetFrame().GetLocalMatrix());
	GetInstance()->m_pCam->GetFrame().SetParent(NULL);

	GetInstance()->m_pCam->GetTarget1().SetParent(eObj->m_pcObj->GetTransform());
	D3DXMatrixIdentity(&GetInstance()->m_pCam->GetTarget1().GetLocalMatrix());

	GetInstance()->m_pCam->SetTarget(&GetInstance()->m_pCam->GetTarget1());
	GetInstance()->m_pCam->SetFrameParent(eObj->m_pcObj->GetTransform());
	GetInstance()->m_pCam->GetFrame().TranslateFrame(D3DXVECTOR3(0.0f, 4.5f, -4.0f));
	GetInstance()->m_pCam->GetTarget()->TranslateFrame(D3DXVECTOR3(0.0f, 1.0f, 5.0f));
}

void CCameraManager::UpdateCallback(IEvent* e, IComponent*)
{
	// Get the Values for the Event
	TUpdateStateEvent* pEvent = static_cast<TUpdateStateEvent*>(e->GetData());
	float fDt = pEvent->m_fDeltaTime;

	//// Update Camera
	GetInstance()->m_pCam->Update(fDt);
}

void CCameraManager::AttachToWinStateCallback(IEvent* e, IComponent* comp)
{
	TObjectEvent* eObj = static_cast<TObjectEvent*>(e->GetData());

	D3DXMatrixIdentity(&GetInstance()->m_pCam->GetFrame().GetLocalMatrix());
	GetInstance()->m_pCam->GetFrame().SetParent(NULL);

	GetInstance()->m_pCam->GetTarget1().SetParent(eObj->m_pcObj->GetTransform());
	D3DXMatrixIdentity(&GetInstance()->m_pCam->GetTarget1().GetLocalMatrix());
	
	GetInstance()->m_pCam->SetTarget(&GetInstance()->m_pCam->GetTarget1());
	GetInstance()->m_pCam->SetFrameParent(eObj->m_pcObj->GetTransform());
	GetInstance()->m_pCam->GetFrame().TranslateFrame(D3DXVECTOR3(0.0f, 2.0f, 4.0f));
	GetInstance()->m_pCam->GetTarget()->TranslateFrame(D3DXVECTOR3(0.0f, 1.0f, -2.0f));
}

void CCameraManager::AttachToLoseStateCallback(IEvent* e, IComponent* comp)
{
	
	TObjectEvent* eObj = static_cast<TObjectEvent*>(e->GetData());

	D3DXMatrixIdentity(&GetInstance()->m_pCam->GetFrame().GetLocalMatrix());
	GetInstance()->m_pCam->GetFrame().SetParent(NULL);

	GetInstance()->m_pCam->GetTarget1().SetParent(eObj->m_pcObj->GetTransform());
	D3DXMatrixIdentity(&GetInstance()->m_pCam->GetTarget1().GetLocalMatrix());

	GetInstance()->m_pCam->SetTarget(&GetInstance()->m_pCam->GetTarget1());
	GetInstance()->m_pCam->SetFrameParent(eObj->m_pcObj->GetTransform());
	GetInstance()->m_pCam->GetFrame().TranslateFrame(D3DXVECTOR3(0.0f, 2.0f, 4.0f));
	GetInstance()->m_pCam->GetTarget()->TranslateFrame(D3DXVECTOR3(0.0f, 1.0f, -2.0f));
}