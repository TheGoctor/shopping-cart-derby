#include "state_manager.h"

#include <string>

scd::state_manager::state_manager() {
  // Start in Intro
#ifdef _DEBUG
  change_state(game_state::main_menu);
#else
  change_state(game_state::intro);
#endif
}

scd::state_manager::~state_manager() {
  clear();
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
void scd::state_manager::push(game_state state) {
  // if we had a state
  if (!_state_stack.empty()) {
    // DISABLE the objects
    PostDisableObjectsEvent(_state_stack.top());
  }

  // Push on State
  _state_stack.push(state);

  // Send Events
  // INIT the objects
  PostInitObjectsEvent(state);
  PostInputChangeEvent(state);

  // HACK: Enable the objects too (until everyone handles init)
  // PostEnableObjectsEvent(eGameState);
}

void scd::state_manager::pop() {
  // so we don't try to pop to or from an empty stack
  // (Unless its in the middle of a change state)
  if (!_is_changing_state && !_state_stack.empty()) {
    return;
  }

  game_state state = _state_stack.top();

  // DISABLE objects in the previous state
  PostDisableObjectsEvent(state); // send a disable for things that don't
                                  // need to unload such as buttons
  PostUnloadObjectsEvent(state);  // send shutdownobjects event

  // Pop off top state
  _state_stack.pop();

  // if we didn't pop everything off the stack just now
  if (!_state_stack.empty()) {
    state = _state_stack.top(); // update the local gamestate var

    // Send InputStateChange for the stack underneath
    PostInputChangeEvent(state);

    // ENABLE the stuff underneath
    PostEnableObjectsEvent(state);
  }
}

void scd::state_manager::change_state(game_state state) {
  // so we can pop the last state
  _is_changing_state = true;

  // Pop All States
  clear();

  // Push passed in state
  push(state);

  // so pop wont pop the last state anymore
  _is_changing_state = false;
}

void scd::state_manager::clear() {
  // Pop off all states
  _is_changing_state = true; // so we can pop ALL the states off
  while (!_state_stack.empty()) {
    pop_state();
  }
  _is_changing_state = false;
  //// Send Disable All Objects
  // PostDisableObjectsEvent(MAX_STATE);
}

void scd::state_manager::PostInputChangeEvent(game_state state) {
  // Send InputStateChange
  SendStateEvent("InputStateChange",
                 (IComponent*)GetInstance(),
                 eGameState,
                 PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostDisableObjectsEvent(game_state eGameState) {
  std::string szEvent = "DisableObjects";
  szEvent += (char)eGameState;
  // Send DisableObjects[State] Event
  SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostEnableObjectsEvent(game_state eGameState) {
  string szEvent = "EnableObjects";
  szEvent += (char)eGameState;

  // Send EnableObjects[State] Event
  SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostInitObjectsEvent(game_state eGameState) {
  string szEvent = "InitObjects";
  szEvent += (char)eGameState;

  SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostLoadObjectsEvent(game_state eGameState) {
  string szEvent = "LoadObjects";
  szEvent += (char)eGameState;

  SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostUnloadObjectsEvent(game_state eGameState) {
  string szEvent = "ShutdownObjects";
  szEvent += (char)eGameState;

  SendIEvent(szEvent, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PushPausedState(IEvent*, IComponent*) {
  // TStateEvent* pcState = (TStateEvent*)pcEvent->GetData();

  PushState(PAUSE_STATE);
}

void scd::state_manager::LoseFocus(IEvent* pcEvent, IComponent* pcSender) {
  // if the stack isn't empty
  if (!_state_stack.empty()) {
    // check the top stack
    EGameState eState = _state_stack.top();

    // if we're in the gameplay state
    if (eState == GAMEPLAY_STATE) {
      // pause it for that bro who alt tabbed
      push_state(PAUSE_STATE);
    }
  }
}
