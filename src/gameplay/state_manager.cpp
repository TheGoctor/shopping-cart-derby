#include "CStateManager.h"
#include "../Event Manager/CEventManager.h"
#include "../Event Manager/CIDGen.h"
#include "../Event Manager/EventStructs.h"
using namespace EventStructs;

CStateManager::CStateManager(void) : m_pEM(NULL), m_bChangingState(false)
{
}

CStateManager::~CStateManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// Init()	:	Initializes the State Manager Object
//
// Mod. Date		:	4/4/11
// Mod. Initials	:	JL
////////////////////////////////////////////////////////////////////////////////
void CStateManager::Init(void)
{
	m_pEM = CEventManager::GetInstance();
	m_pEM->RegisterEvent("UpdateState", (IComponent*)GetInstance(), Update);
	m_pEM->RegisterEvent("RenderState", (IComponent*)GetInstance(), Render);
	m_pEM->RegisterEvent("Shutdown", (IComponent*)GetInstance(), Shutdown);

	// Handle pushing the pause state when "Menu" input is pressed (should only happen in gameplay state)
	m_pEM->RegisterEvent("Menu", (IComponent*)GetInstance(), PushPausedState);
	//m_pEM->RegisterEvent("PauseGame", (IComponent*)GetInstance(), PushState); // gotta have the right data for the push
	m_pEM->RegisterEvent("Back", (IComponent*)GetInstance(), PopState); // menu back

	m_pEM->RegisterEvent("StateChange", (IComponent*)GetInstance(), ChangeState);

	// Added for Console to work properly
	m_pEM->RegisterEvent("PushState", (IComponent*)GetInstance(), PushState);
	m_pEM->RegisterEvent("PopState", (IComponent*)GetInstance(), PopState);
	
	m_pEM->RegisterEvent("FocusLost", (IComponent*)GetInstance(), LoseFocus);
	
	

	// Start in Intro
	
#ifdef _DEBUG
	ChangeState(MAIN_MENU_STATE);
#else
	ChangeState(INTRO_STATE);
#endif
}

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
void CStateManager::PushState(EGameState eGameState)
{
	// if we had a state
	if(!m_cStateStack.empty())
	{
		// DISABLE the objects
		PostDisableObjectsEvent(m_cStateStack.top());
	}

	// Push on State
	m_cStateStack.push(eGameState);

	// Send Events
	// INIT the objects
	PostInitObjectsEvent(eGameState);
	PostInputChangeEvent(eGameState);
	
	// HACK: Enable the objects too (until everyone handles init)
	//PostEnableObjectsEvent(eGameState);

}

////////////////////////////////////////////////////////////////////////////////
// PopState()	:	Pops the top state off the stack
//
// Returns	:	void
//
// Mod. Date		:	4/4/11
// Mod. Initials	:	JL
////////////////////////////////////////////////////////////////////////////////
void CStateManager::PopState(void)
{
	// so we don't try to pop to or from an empty stack
	// (Unless its in the middle of a change state)
	if( !m_bChangingState && m_cStateStack.size() <= 1)
	{
		return;
	}


	EGameState eGameState = m_cStateStack.top();

	// DISABLE objects in the previous state
	PostDisableObjectsEvent(eGameState); // send a disable for things that don't need to unload such as buttons
	PostUnloadObjectsEvent(eGameState); // send shutdownobjects event

	// Pop off top state
	m_cStateStack.pop();

	// if we didn't pop everything off the stack just now
	if(!m_cStateStack.empty())
	{
		eGameState = m_cStateStack.top(); // update the local gamestate var

		// Send InputStateChange for the stack underneath
		PostInputChangeEvent(eGameState);

		// ENABLE the stuff underneath
		PostEnableObjectsEvent(eGameState);
	}
}

////////////////////////////////////////////////////////////////////////////////
// ChangeState()	:	Pops the states off the stack and pushes the passed in 
//						state
//
// Returns	:	void
//
// Mod. Date		:	4/4/11
// Mod. Initials	:	JL
////////////////////////////////////////////////////////////////////////////////
void CStateManager::ChangeState(EGameState eGameState)
{
	// so we can pop the last state
	GetInstance()->m_bChangingState = true;

	// Pop All States
	ClearAllStates();

	// Push passed in state
	PushState(eGameState);

	// so pop wont pop the last state anymore
	GetInstance()->m_bChangingState = false;
}

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
void CStateManager::Update(float )
{
}

////////////////////////////////////////////////////////////////////////////////
// Render()	:	Sends Render[State]Object events
//
// Returns	:	void
//
// Mod. Date		:	4/4/11
// Mod. Initials	:	JL
////////////////////////////////////////////////////////////////////////////////
void CStateManager::Render(void)
{
}

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
void CStateManager::Shutdown(void)
{
	ClearAllStates();
}

////////////////////////////////////////////////////////////////////////////////
// ClearAllStates()	:	Pops all the active states off the stack
//
// Returns	:	void
//
// Mod. Date		:	4/4/11
// Mod. Initials	:	JL
////////////////////////////////////////////////////////////////////////////////
void CStateManager::ClearAllStates(void)
{
	// Pop off all states
	m_bChangingState = true; // so we can pop ALL the states off
	while(!m_cStateStack.empty())
	{
		PopState();
	}
	m_bChangingState = false;
	//// Send Disable All Objects
	//PostDisableObjectsEvent(MAX_STATE);
}

void CStateManager::Update(IEvent* pcEvent, IComponent* )
{
	// so we don't try to act on an empty stack
	if(GetInstance()->m_cStateStack.empty())
	{
		return;
	}
	
	float fDT = ((TUpdateStateEvent*)pcEvent->GetData())->m_fDeltaTime;
	string szEvent = "Update";
	szEvent += (char)GetInstance()->m_cStateStack.top();
	SendUpdateEvent(szEvent, (IComponent*)GetInstance(), fDT);
}

void CStateManager::Render(IEvent* , IComponent* )
{
}

void CStateManager::Shutdown(IEvent* , IComponent* )
{
	GetInstance()->Shutdown();
}

void CStateManager::PushState(IEvent* pcEvent, IComponent* )
{
	TStateEvent* pcState = (TStateEvent*)pcEvent->GetData();
	GetInstance()->PushState(pcState->m_eToState);
}

void CStateManager::PopState(IEvent* , IComponent* )
{
	GetInstance()->PopState();
}

void CStateManager::ChangeState(IEvent* pcEvent, IComponent* )
{
	TStateEvent* pcState = (TStateEvent*)pcEvent->GetData();

	GetInstance()->ChangeState(pcState->m_eToState);
}

void CStateManager::PostInputChangeEvent(EGameState eGameState)
{
	// Send InputStateChange
	SendStateEvent("InputStateChange", (IComponent*)GetInstance(), eGameState, PRIORITY_IMMEDIATE);
	
}

void CStateManager::PostDisableObjectsEvent(EGameState eGameState)
{
	string szEvent = "DisableObjects";
	szEvent += (char)eGameState;
	// Send DisableObjects[State] Event
	SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void CStateManager::PostEnableObjectsEvent(EGameState eGameState)
{
	string szEvent = "EnableObjects";
	szEvent += (char)eGameState;
	
	// Send EnableObjects[State] Event
	SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void CStateManager::PostInitObjectsEvent(EGameState eGameState)
{
	string szEvent = "InitObjects";
	szEvent += (char)eGameState;
	
	SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void CStateManager::PostLoadObjectsEvent(EGameState eGameState)
{
	string szEvent = "LoadObjects";
	szEvent += (char)eGameState;
	
	SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void CStateManager::PostUnloadObjectsEvent(EGameState eGameState)
{
	string szEvent = "ShutdownObjects";
	szEvent += (char)eGameState;
	
	SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}


void CStateManager::PushPausedState(IEvent* , IComponent* )
{
	//TStateEvent* pcState = (TStateEvent*)pcEvent->GetData();

	GetInstance()->PushState(PAUSE_STATE);
}

int CStateManager::PushState(lua_State* pLua)
{
	EGameState eState = (EGameState)lua_tointeger(pLua, -1);
	lua_pop(pLua, 1);
	CStateManager::GetInstance()->PushState(eState);

	return 0;
}

int CStateManager::StateChange(lua_State* pLua)
{
	EGameState eState = (EGameState)lua_tointeger(pLua, -1);
	lua_pop(pLua, 1);
	CStateManager::GetInstance()->ChangeState(eState);

	return 0;
}

int CStateManager::Back(lua_State* /*pLua*/)
{
	CStateManager::GetInstance()->PopState();

	return 0;
}



void CStateManager::LoseFocus(IEvent* pcEvent, IComponent* pcSender)
{
	// if the stack isn't empty
	if( ! GetInstance()->m_cStateStack.empty())
	{
		// check the top stack
		EGameState eState = GetInstance()->m_cStateStack.top();

		// if we're in the gameplay state
		if(eState == GAMEPLAY_STATE)
		{
			// pause it for that bro who alt tabbed
			GetInstance()->PushState(PAUSE_STATE);
		}
	}
}
