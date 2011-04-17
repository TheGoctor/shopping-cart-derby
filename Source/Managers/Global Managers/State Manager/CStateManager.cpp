#include "CStateManager.h"
#include "../Event Manager/CEventManager.h"
#include "../Event Manager/CStateEvent.h"
#include "../Event Manager/CUpdateStateEvent.h"
#include "../Event Manager/CIDGen.h"

CStateManager::CStateManager(void) : m_pEM(NULL)
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
	m_pEM->RegisterEvent("UpdateState", NULL, Update);
	m_pEM->RegisterEvent("RenderState", NULL, Render);
	m_pEM->RegisterEvent("Shutdown", NULL, Shutdown);

	m_pEM->RegisterEvent("PauseGame", NULL, PushState);
	m_pEM->RegisterEvent("BackPressed", NULL, PopState);

	m_pEM->RegisterEvent("StateChange", NULL, ChangeState);

	m_pEM->RegisterEvent("StateChangeGameplay", NULL, ChangeStateGameplay);

	// Start in Intro
	ChangeState(MAIN_MENU_STATE);
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
	// Push on State
	m_cStateStack.push(eGameState);

	// Send Events
	PostEnableObjectsEvent(eGameState);
	PostInputChangeEvent(eGameState);
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
	EGameState eGameState = m_cStateStack.top();

	// Send Disable Event
	PostDisableObjectsEvent(eGameState);

	// Pop off top state
	m_cStateStack.pop();

	// Send InputStateChange
	PostInputChangeEvent(eGameState);
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
	// Pop All States
	ClearAllStates();

	// Push passed in state
	PushState(eGameState);
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
void CStateManager::Update(float fDT)
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
	while(false == m_cStateStack.empty())
	{
		PopState();
	}
	//// Send Disable All Objects
	//PostDisableObjectsEvent(MAX_STATE);
}

void CStateManager::Update(IEvent* pcEvent, IComponent* pcSender)
{
	CUpdateStateEvent* pcState = (CUpdateStateEvent*)pcEvent;
	string szEvent = "Update";
	szEvent += GetInstance()->m_cStateStack.top();
	
	CUpdateStateEvent* pUpdateEvent = MMNEWEVENT(CUpdateStateEvent) 
		CUpdateStateEvent(CIDGen::GetInstance()->GetID(szEvent), 
		NULL, pcState->GetDeltaTime());
	GetInstance()->m_pEM->PostEvent(pUpdateEvent, PRIORITY_UPDATE);
	//GetInstance()->Update(pcState->GetDeltaTime());
}

void CStateManager::Render(IEvent* pcEvent, IComponent* pcSender)
{
	//GetInstance()->Render();
}

void CStateManager::Shutdown(IEvent* pcEvent, IComponent* pcSender)
{
	GetInstance()->Shutdown();
}

void CStateManager::PushState(IEvent* pcEvent, IComponent* pcSender)
{
	CStateEvent* pcState = (CStateEvent*)pcEvent;
	GetInstance()->PushState(pcState->GetToState());
	
}

void CStateManager::PopState(IEvent* pcEvent, IComponent* pcSender)
{
	EGameState fromState = GetInstance()->m_cStateStack.top();
	GetInstance()->PopState();

}

void CStateManager::ChangeState(IEvent* pcEvent, IComponent* pcSender)
{
	CStateEvent* pcState = (CStateEvent*)pcEvent;
	GetInstance()->ChangeState(pcState->GetToState());
}

void CStateManager::PostEnableObjectsEvent(EGameState eGameState)
{
	string szEvent = "EnableObjects";
	szEvent += eGameState;
	// Send EnableObjects[State] Event
	IEvent* pObjectEvent = MMNEWEVENT(IEvent) IEvent(CIDGen::GetInstance()->
		GetID(szEvent), NULL);

	m_pEM->PostEvent(pObjectEvent, PRIORITY_NORMAL);
}

void CStateManager::PostInputChangeEvent(EGameState eGameState)
{
	string szEvent = "InputStateChange";
	//szEvent += eGameState;
	// Send InputStateChange
	CStateEvent* pInputEvent = MMNEWEVENT(CStateEvent) CStateEvent(CIDGen::GetInstance()->
		GetID(szEvent), NULL, eGameState, eGameState);

	m_pEM->PostEvent(pInputEvent, PRIORITY_INPUT);
}

void CStateManager::PostDisableObjectsEvent(EGameState eGameState)
{
	string szEvent = "DisableObjects";
	szEvent += eGameState;
	// Send DisableObjects[State] Event
	IEvent* pObjectEvent = MMNEWEVENT(IEvent) IEvent(CIDGen::GetInstance()->
		GetID(szEvent), NULL);

	m_pEM->PostEvent(pObjectEvent, PRIORITY_NORMAL);
}

void CStateManager::ChangeStateGameplay(IEvent* pcEvent, IComponent* pcSender)
{
	GetInstance()->ChangeState(GAMEPLAY_STATE);
}
