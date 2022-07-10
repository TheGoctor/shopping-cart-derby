// FILE HEADER
#ifndef _CSCROLLERCOMPONENT_H_
#define _CSCROLLERCOMPONENT_H_

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\..\IComponent.h"

// For TSpriteData
#include "..\..\Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"
#include "..\..\CObject.h"
#include "..\..\Enums.h"

class CButtonComponent;

class IEvent;
//class CSpriteComponent;

class CScrollerComponent : public IComponent
{
protected:
	CObject*	m_pObject;
	CButtonComponent* m_pButton;
	
	float m_yPos;
	float m_xPos;
	float m_xVel;
	float m_yVel;
	float m_xSize;
	float m_ySize;

	float m_xStartPos;
	float m_yStartPos;


public:

	///////////////////////////////////////////////////////////////////////////
	//	Name:			CScrollerComponent
	//	Parameters:		none
	//	Return:			void
	//	Description:	Subscribes to relevant events.
	///////////////////////////////////////////////////////////////////////////
	CScrollerComponent(CObject* pObj);

	static int CreateScrollerComponent(lua_State* pLua);
	static CScrollerComponent* CreateScrollerComponent(CButtonComponent* pButton, float xVel, float yVel,
																		float xSize, float ySize);


	void SetX(float fX) 
	{
		m_xPos = fX;
	}
	void SetY(float fY)
	{
		m_yPos = fY;
	}
	///////////////////////////
	//
	//	Callback Functions
	//
	///////////////////////////
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			Update
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CUpdateEvent
	//											//Holds the frame's delta time
	//	Return:			void
	//	Description:	Calls all of the functions that need to 
	//					 be called every frame. This function is 
	//					 called by the event system. 
	///////////////////////////////////////////////////////////////////////////
	static void Update(IEvent* cEvent, IComponent* cCenter);
	
	static void ScrollerStateDisable(IEvent* cEvent, IComponent* cCenter);
	static void ScrollerStateInit(IEvent* cEvent, IComponent* cCenter);
	static void ScrollerStateEnable(IEvent* cEvent, IComponent* cCenter);
	
};

#endif