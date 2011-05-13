////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateComponent.h
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object Animate
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _CANIMATECOMPONENT_H_
#define _CANIMATECOMPONENT_H_

// Includes
#include "..\\..\\CObject.h"

class CAnimateComponent
{
private:

	// Parent Object
	CObject* m_pcParent;

	// 

public:

	// Constructor
	CAnimateComponent(CObject* pParent);

};
#endif // _CANIMATECOMPONENT_H_