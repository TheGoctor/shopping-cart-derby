////////////////////////////////////////////////////////////////////////////////
//	File			:	CStateManager.h
//	Date			:	4/4/11
//	Mod. Date		:	4/4/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	The CStateManager module controls the transitions of
//						states in our game by means of a state stack.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "core/containers.h"

#include <memory>
#include <mutex>
#include <stack>

namespace scd {

class state_manager {
public:
  enum class game_state {
    intro,
    main_menu,
  };

public:
  state_manager();
  ~state_manager();

  /**
   * @brief Pushes a new state onto the stack.
   *
   * @param state The new state.
   */
  void push(game_state state);

  /**
   * @brief Pops the current state off the stack.
   */
  void pop();

  ////////////////////////////////////////////////////////////////////////////////
  // ChangeState()	:	Pops the states off the stack and pushes the passed in
  //						state
  //
  // Returns	:	void
  //
  // Mod. Date		:	4/4/11
  // Mod. Initials	:	JL
  ////////////////////////////////////////////////////////////////////////////////
  void change_state(game_state state);

  /**
   * @brief Sends update events for the current state.
   *
   * @param dt Time since the last update in seconds.
   */
  void update(float dt);

  /**
   * @brief Sends render events for the current state.
   */
  void render();

  ////////////////////////////////////////////////////////////////////////////////
  // ClearAllStates()	:	Pops all the active states off the stack
  //
  // Returns	:	void
  //
  // Mod. Date		:	4/4/11
  // Mod. Initials	:	JL
  ////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Pops all actives states off the state stack.
   */
  void clear();

  // Helper Funcs
  void PostEnableObjectsEvent(game_state eGameState);
  void PostDisableObjectsEvent(game_state eGameState);
  void PostInitObjectsEvent(game_state eGameState);
  void PostLoadObjectsEvent(game_state eGameState);
  void PostUnloadObjectsEvent(game_state eGameState);
  void PostInputChangeEvent(game_state eGameState);

private:
  // The stack which contains the active states
  std::stack<game_state, scd::deque<game_state>> _state_stack;

  std::weak_ptr<class event_manager> _event_manager;

  std::atomic_bool _is_changing_state;
};

} // namespace scd
