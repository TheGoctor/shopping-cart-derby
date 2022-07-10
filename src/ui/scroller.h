#pragma once

#include "core/base_component.h"
#include "physics/math_types.h"

struct lua_State;

namespace scd {
class event;
}

namespace scd::ui {
class button;

class scroller : public scd::base_component {
public:
  scroller(scd::object& owner);

  static int create(lua_State* pLua);
  static scroller* create(button* button, const vector2& velocity, const vector2& size);

  void set_position_x(float fX) { _position.x = fX; }
  void set_position_y(float fY) { _position.y = fY; }

  static void update(event* cEvent, scd::base_component* cCenter);

  static void on_init(event* cEvent, scd::base_component* cCenter);
  static void on_enable(event* cEvent, scd::base_component* cCenter);
  static void on_disable(event* cEvent, scd::base_component* cCenter);

private:
  button* _button;

  scd::vector2 _position;
  scd::vector2 _velocity;
  scd::vector2 _size;

  scd::vector2 _start_position;
};
} // namespace scd::ui
