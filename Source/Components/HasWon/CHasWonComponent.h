// FILE HEADER
#ifndef _CHASWONCOMPONENT_H_
#define _CHASWONCOMPONENT_H_

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


class CObject;
class IEvent;
class CSpriteComponent;


class CHasWonComponent : public IComponent
{
private:
	CObject*			m_pObject; // parent of component
	
	bool				m_bDisplayingWinScreen;

	CObject*			m_pDisplay0;
	CSpriteComponent*	m_pDisplayComponent0;
	TSpriteData			m_tSpriteData0;
	
	CObject*			m_pDisplay1;
	CSpriteComponent*	m_pDisplayComponent1;
	TSpriteData			m_tSpriteData1;

	int					m_nMenuSelection;
	float				m_fTimeSinceLastInput;
public:


	///////////////////////////////////////////////////////////////////////////
	//	Name:			CHasWonComponent
	//	Parameters:		none
	//	Return:			void
	//	Description:	Subscribes to relevant events.
	///////////////////////////////////////////////////////////////////////////
	CHasWonComponent(CObject* pObj);

	static int CreateHasWonComponent(lua_State* pLua);
	static CHasWonComponent* CreateHasWonComponent(CObject* pObj);


	
	void Init();


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
	static void GameWon(IEvent* cEvent, IComponent* cCenter);
	
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
	static void LeftPressed(IEvent* cEvent, IComponent* cCenter);
	static void RightPressed(IEvent* cEvent, IComponent* cCenter);
	static void SelectPressed(IEvent* cEvent, IComponent* cCenter);
	
};

#endif