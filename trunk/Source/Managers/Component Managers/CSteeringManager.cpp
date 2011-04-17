#include "CMovementManager.h"

#include "..\Global Managers\Memory Manager\CMemoryManager.h"

#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CInputEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CHeadingEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CUpdateStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CWeightClassEvent.h"

#include "..\..\Components\Movement\CMovement.h"

#include "..\..\CObject.h"

CSteeringManager::CSteeringManager()
{
	
}


void CSteeringManager::HandleUpdate(IEvent* cEvent, IComponent* cCenter)
{
	
}
	
void CSteeringManager::HandleInputSteerLeft(IEvent* cEvent, IComponent* cCenter)
{
}

void CSteeringManager::HandleInputSteerRight(IEvent* cEvent, IComponent* cCenter)
{
}
	

void CSteeringManager::Update(float fDt)
{
	
}

void CSteeringManager::SteerLeft()
{
	
}

void CSteeringManager::SteerRight()
{

}