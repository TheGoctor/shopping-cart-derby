#include "state_manager.h"

#include <string>

scd::state_manager::state_manager() {
  change_state(game_state::intro);
}

scd::state_manager::~state_manager() {
  clear();
}

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
  if (!_state_mutex.try_lock() && !_state_stack.empty()) {
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
  auto lock = std::lock_guard<std::mutex>(_state_mutex);

  // Pop All States
  clear();

  // Push passed in state
  push(state);
}

void scd::state_manager::clear() {
  // Pop off all states
  auto lock = std::lock_guard<std::mutex>(_state_mutex);
  while (!_state_stack.empty()) {
    pop();
  }
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
  std::string event_name = "DisableObjects";
  event_name += (char)eGameState;
  // Send DisableObjects[State] Event
  SendIEvent(event_name, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostEnableObjectsEvent(game_state eGameState) {
  std::string event_name = "EnableObjects";
  event_name += (char)eGameState;

  // Send EnableObjects[State] Event
  SendIEvent(event_name, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostInitObjectsEvent(game_state eGameState) {
  std::string event_name = "InitObjects";
  event_name += (char)eGameState;

  SendIEvent(event_name, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostLoadObjectsEvent(game_state eGameState) {
  std::string event_name = "LoadObjects";
  event_name += (char)eGameState;

  SendIEvent(event_name, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}

void scd::state_manager::PostUnloadObjectsEvent(game_state eGameState) {
  std::string event_name = "ShutdownObjects";
  event_name += (char)eGameState;

  SendIEvent(event_name, (IComponent*)GetInstance(), NULL, PRIORITY_IMMEDIATE);
}
