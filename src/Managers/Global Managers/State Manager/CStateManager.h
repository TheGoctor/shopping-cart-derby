////////////////////////////////////////////////////////////////////////////////
//	File			:	CStateManager.h
//	Date			:	4/4/11
//	Mod. Date		:	4/4/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	The CStateManager module controls the transitions of 
//						states in our game by means of a state stack. 
////////////////////////////////////////////////////////////////////////////////
#ifndef _CSTATEMANAGER_H_
#define _CSTATEMANAGER_H_

#include <stack>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "../Memory Manager/CAllocator.h"
#include "../../../Enums.h"

class IEvent;
class IComponent;
class CEventManager;

class CStateManager
{
	std::stack<EGameState, deque<EGameState, CAllocator<EGameState>>> m_cStateStack; // The stack which contains the active states
	CEventManager* m_pEM;

	bool		m_bChangingState;

	// Constructors, Destructors
	CStateManager(void);
	~CStateManager(void);
	CStateManager(const CStateManager&);
	CStateManager& operator=(const CStateManager&);

	////////////////////////////////////////////////////////////////////////////////
	// PushState()	:	Pushes the passed in state
	//
	// Ins		:	EGameState	- The state to push
	//
	// Returns	:	void
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void PushState(EGameState eGameState);

	////////////////////////////////////////////////////////////////////////////////
	// PopState()	:	Pops the top state off the stack
	//
	// Returns	:	void
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void PopState(void);

	////////////////////////////////////////////////////////////////////////////////
	// ChangeState()	:	Pops the states off the stack and pushes the passed in 
	//						state
	//
	// Returns	:	void
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void ChangeState(EGameState eGameState);

	////////////////////////////////////////////////////////////////////////////////
	// Update()	:	Sends Update[State]Object events
	//
	// Ins		:	float	- Delta time
	// 
	// Returns	:	void
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void Update(float fDT);

	////////////////////////////////////////////////////////////////////////////////
	// Render()	:	Sends Render[State]Object events
	//
	// Returns	:	void
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void Render(void);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Function:	“Shutdown”
	//
	// Return:		void
	//
	// Parameters:	void
	//
	// Purpose:		This function is called when shutting the game down. This function shuts down all the singletons
	//				and cleans up any memory that has been allocated.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Shutdown(void);

	////////////////////////////////////////////////////////////////////////////////
	// ClearAllStates()	:	Pops all the active states off the stack
	//
	// Returns	:	void
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void ClearAllStates(void);

	// Helper Funcs
	void PostEnableObjectsEvent(EGameState eGameState);
	void PostDisableObjectsEvent(EGameState eGameState);
	void PostInitObjectsEvent(EGameState eGameState);
	void PostLoadObjectsEvent(EGameState eGameState);
	void PostUnloadObjectsEvent(EGameState eGameState);
	void PostInputChangeEvent(EGameState eGameState);

public:
	
	////////////////////////////////////////////////////////////////////////////////
	// GetInstance()	:	Returns the Singleton
	//
	// Returns	:	static CStateManager*
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	static CStateManager* GetInstance(void) 
	{ 
		static CStateManager cStateManager;
		return &cStateManager; 
	} 
	
	////////////////////////////////////////////////////////////////////////////////
	// Init()	:	Initializes the State Manager Object
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void Init(void);

	// Callbacks
	static void Update(IEvent* pcEvent, IComponent* pcSender);
	static void Render(IEvent* pcEvent, IComponent* pcSender);
	static void Shutdown(IEvent* pcEvent, IComponent* pcSender);
	static void PushState(IEvent* pcEvent, IComponent* pcSender);
	static void PopState(IEvent* pcEvent, IComponent* pcSender);
	static void ChangeState(IEvent* pcEvent, IComponent* pcSender);
	static void ChangeStateGameplay(IEvent* pcEvent, IComponent* pcSender);
	static void PushPausedState(IEvent* pcEvent, IComponent* pcSender);
	
	
	static void LoseFocus(IEvent* pcEvent, IComponent* pcSender);
	
	static int PushState(lua_State* pLua);
	static int StateChange(lua_State* pLua);
	static int Back(lua_State* pLua);

};

#endif