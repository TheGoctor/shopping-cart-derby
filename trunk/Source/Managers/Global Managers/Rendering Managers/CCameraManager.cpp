///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCameraManager.cpp"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/12/11
//
//	Last Changed	:	04/12/11
//
//  Purpose			:	A manager for cameras
///////////////////////////////////////////////////////////////////////////////

#include "../../../Managers/Global Managers/Event Manager/CEventManager.h"
#include "../../../Managers/Global Managers/Event Manager/CMoveCameraEvent.h"
#include "../../../Managers/Global Managers/Event Manager/CUpdateStateEvent.h"

#include "Camera.h"
#include "CCameraManager.h"

// Initalize
void CCameraManager::Init(int nScreenWidth, int nScreenHeight)
{
	m_pCam = CCamera::GetInstance();
	// Setup camera.
	//m_pCam->SetViewPosition(0.0f, 6.0f, -10.0f);
	m_pCam->BuildPerspective(D3DX_PI/3.0f, (float)nScreenWidth / (float)nScreenHeight, 1.0f, 1000.0f);
	
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("AttachToCamera",
		NULL, AttachToCamCallback);

	pEM->RegisterEvent("MoveCamera",
	NULL, MoveCameraCallback);

	pEM->RegisterEvent("UpdateState",
		NULL, UpdateCallback);
}

// Attach To Camera
void CCameraManager::AttachToCamCallback(IEvent* e, IComponent* comp)
{
	// Get the Values for the Event
	CMoveCameraEvent* pcCamEvent = static_cast<CMoveCameraEvent*>(e);
	D3DXVECTOR3 vPos = pcCamEvent->GetPosition();
	
	GetInstance()->m_pCam->LookAtTarget(D3DXVECTOR3(vPos.x, 4.0f, vPos.z-8.0f),
		D3DXVECTOR3(vPos.x, vPos.y, vPos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	//GetInstance()->m_pCam->SetOffsetDistance(5.0f);
}

// Move Camera
void CCameraManager::MoveCameraCallback(IEvent* e, IComponent* comp)
{
	// Get the Values for the Event
	CMoveCameraEvent* pcCamEvent = static_cast<CMoveCameraEvent*>(e);
	D3DXVECTOR3 vPos = pcCamEvent->GetPosition();
	float fVel = pcCamEvent->GetSpeed();
	float fTurnRate = pcCamEvent->GetTurnRate();
	
	float turn = fTurnRate*3400.0f; // constant is to match
							//turn rate with turn rate here

	// Then move the camera based on where the cart has moved to.
	// When the cart is moving backwards rotations are inverted to match
	// the direction of travel. Consequently the camera's rotation needs to be
	// inverted as well.
	if (fTurnRate != 0.0f)
	{
		GetInstance()->m_pCam->Rotate(turn, 0.0f);
	}
	
	GetInstance()->m_pCam->LookAtTarget(vPos);

	
}

void CCameraManager::UpdateCallback(IEvent* e, IComponent* comp)
{
	// Get the Values for the Event
	CUpdateStateEvent* pEvent = static_cast<CUpdateStateEvent*>(e);
	float fDt = pEvent->GetDeltaTime();

	//// Update Camera
	GetInstance()->m_pCam->Update(fDt);
}