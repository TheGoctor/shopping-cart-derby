// FILE HEADER
#ifndef _CBUTTONCOMPONENT_H_
#define _CBUTTONCOMPONENT_H_

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

class IEvent;
//class CSpriteComponent;


class CButtonComponent : public IComponent
{
private:
	CObject*			m_pObject; // parent of component
	
	CButtonComponent*	m_pNext;
	CButtonComponent*	m_pPrevious;
	string				m_szSelectionEvent;

	CObject*			m_pDisplayObjectButton;
	CSpriteComponent*	m_pDisplayComponentButton;
	TSpriteData			m_tSpriteDataButton;

	CObject*			m_pDisplayObjectSelection;
	CSpriteComponent*	m_pDisplayComponentSelection;
	TSpriteData			m_tSpriteDataSelection;

	static CObject*			m_pDisplayObjectBackground;
	static CSpriteComponent*	m_pDisplayComponentBackground;
	static TSpriteData			m_tSpriteDataBackground;

	float				m_fTimeSinceSelectionEntry;
	float				m_fInputCooldown;
	bool				m_bSelected;

public:


	///////////////////////////////////////////////////////////////////////////
	//	Name:			CButtonComponent
	//	Parameters:		none
	//	Return:			void
	//	Description:	Subscribes to relevant events.
	///////////////////////////////////////////////////////////////////////////
	CButtonComponent(CObject* pObj);

	static int CreateButtonComponent(lua_State* pLua);
	static CButtonComponent* CreateButtonComponent(CObject* pObj, string szEventName, string szSpriteTextureName, int nPosX, int nPosY, bool bStartSelected);

	static int SetNextButtonComponent(lua_State* pLua);
	static void SetNextButtonComponent(CButtonComponent* pCurrent, CButtonComponent* pNext);

	
	void Init(string szButtonTexName);
	void Activate();
	void Deactivate();
	void Unshow();

	void SetScreenPosition(int nPosX, int nPosY);


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
	static void PreviousPressed(IEvent* cEvent, IComponent* cCenter);
	static void NextPressed(IEvent* cEvent, IComponent* cCenter);
	static void SelectPressed(IEvent* cEvent, IComponent* cCenter);
	
};

#endif