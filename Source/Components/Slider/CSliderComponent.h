// FILE HEADER
#ifndef _CSLIDERCOMPONENT_H_
#define _CSLIDERCOMPONENT_H_

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

#include "..\Button\CButtonComponent.h"

class IEvent;
//class CSpriteComponent;

class CSliderComponent : public CButtonComponent
{
private:
	
	D3DXVECTOR3		m_vStartPosition;
	D3DXVECTOR3		m_vEndPosition;

public:
	///////////////////////////////////////////////////////////////////////////
	//	Name:			CButtonComponent
	//	Parameters:		none
	//	Return:			void
	//	Description:	Subscribes to relevant events.
	///////////////////////////////////////////////////////////////////////////
	CSliderComponent(CObject* pObj);

	static int CreateSliderComponent(lua_State* pLua);
	static CSliderComponent* CreateSliderComponent(CObject* pObj, string szEventName, string szSpriteTextureName, 
														  int nStartX, int nSartY, int nEndX, int nEndY, int nTextureDepth,
														  bool bStartSelected, int eGameState);

	
	void Init(string szButtonTexName, int nTextureDepth);
	void ReInitValues();
	void Activate();
	void Deactivate();
	void Unshow();

	void SetScreenPosition(int nPosX, int nPosY, int nEndX, int nEndY);

	void CalculateAndSendFloatEvent();
	void MoveSliderOneTick(int nDirection);
	void SetSliderToValue(float fAmount);
	static int SetSliderValue(lua_State* pLua);

	///////////////////////////
	//
	//	Callback Functions
	//
	///////////////////////////
	
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			GameWon
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CUpdateEvent
	//											//Holds the frame's delta time
	//	Return:			void
	//	Description:	Calls all of the functions that need to 
	//					 be called every frame. This function is 
	//					 called by the event system. 
	///////////////////////////////////////////////////////////////////////////
	
	static void SelectPressed(IEvent* cEvent, IComponent* cCenter);
	static void SlideLeft(IEvent* cEvent, IComponent* cCenter);
	static void SlideRight(IEvent* cEvent, IComponent* cCenter);
	static void SliderStateInit(IEvent* cEvent, IComponent* cCenter);
	
};

#endif