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

  /**
   * @brief Clears state stack and pushes the new state.
   *
   * @param state The new state.
   */
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

  /**
   * @brief Pops all actives states off the state stack.
   */
  void clear();

  // Helper Funcs
  void PostEnableObjectsEvent(game_state state);
  void PostDisableObjectsEvent(game_state state);
  void PostInitObjectsEvent(game_state state);
  void PostLoadObjectsEvent(game_state state);
  void PostUnloadObjectsEvent(game_state state);
  void PostInputChangeEvent(game_state state);

private:
  // The stack which contains the active states
  std::stack<game_state, scd::deque<game_state>> _state_stack;

  std::weak_ptr<class event_manager> _event_manager;

  std::mutex _state_mutex;
};

} // namespace scd
