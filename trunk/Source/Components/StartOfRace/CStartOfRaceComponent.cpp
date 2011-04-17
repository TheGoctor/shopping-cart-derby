#include "CStartOfRaceComponent.h"


#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CHeadingEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CUpdateStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CInputEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CWeightClassEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CIDGen.h"
#include "..\..\CObject.h"
#include "..\Movement\CMovement.h"

#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"



CStartOfRaceComponent::CStartOfRaceComponent(CObject* pObj) : m_pObject(pObj)
{
	
}


void CStartOfRaceComponent::Init()
{

		//HACK AGAIN: Don't access directly. Should be handled by event above
		//CInputManager::InputStateChange(pInputEvent, NULL);
		// ENDH

}


///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CStartOfRaceComponent::Update(float fDt)
{

	

	// increment timer

	// if hud element change is needed
		// change hud element
		// play sound

	// render proper hud element

	// if timer is at point to start race
		// change input state to game

	// if timer is at total end
		// stop rendering hud stuff and clean up all that
		// remove us from the object
		// obj.removecomp(comp*)
		//MMDEL(CStartOfRaceComponent, this); // clean up my memory
}



