// FILE HEADER
#ifndef _CSTARTOFRACECOMPONENT_H_
#define _CSTARTOFRACECOMPONENT_H_

#include <D3dx9math.h>

#include "..\..\IComponent.h"
#include "..\..\Managers\Component Managers\CStartOfRaceManager.h"

class CObject;
class IEvent;

class CStartOfRaceComponent : public IComponent
{
private:
	CObject*		m_pObject; // parent of component

	// DATA MEMBERS GO HERE LOL

public:


	///////////////////////////////////////////////////////////////////////////
	//	Name:			CStartOfRaceComponent
	//	Parameters:		none
	//	Return:			void
	//	Description:	Subscribes to relevant events.
	///////////////////////////////////////////////////////////////////////////
	CStartOfRaceComponent(CObject* pObj);


	///////////////////////////////////////////////////////////////////////////
	//	Name:			Init
	//	Parameters:		none
	//	Return:			void
	//	Description:	Calls all of the functions that need to 
	//					 be called every frame. This function is 
	//					 called by the event system. 
	///////////////////////////////////////////////////////////////////////////
	void Init();


	///////////////////////////////////////////////////////////////////////////
	//	Name:			Update
	//	Parameters:		float	fDt		- Time since last frame
	//	Return:			void
	//	Description:	Calls all of the functions that need to 
	//					 be called every frame. This function is 
	//					 called by the manager. 
	///////////////////////////////////////////////////////////////////////////
	void Update(float fDt);
};

#endif