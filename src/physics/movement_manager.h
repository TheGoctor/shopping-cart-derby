#pragma once

#include "core/containers.h"

namespace scd {

enum class cart_weight { none, light, medium, heavy };

enum class turn_direction { left = -1, none, right, drift_waiting_for_turn };

class object;
class event;
class base_component;
class movement;

enum class acceleration_behavior {
  error = -1,
  accelerate,
  brake,
  coast,
  go_right,
  go_left,
  drifting,
  max
};

class movement_manager {
  scd::map<unsigned int, movement*> _components;

  movement_manager();
  ~movement_manager(){};
  movement_manager(const movement_manager&){};
  movement_manager& operator=(const movement_manager&);

public:
  static movement_manager& get() {
    static movement_manager _manager;
    return _manager;
  }
  static int create_component(lua_State* pLua);
  static movement* create_component(object& owner);
  static void on_shutdown(event*, base_component*);

  static int set_cart_weight(lua_State* pLua);

  bool is_player_invincible(object& player);
  float player_speed(object& player);
  movement* get_component(object& owner);
};

} // namespace scd
