#ifndef __scd_components_scroller_h__
#define __scd_components_scroller_h__

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "component.h"

#include "Enums.h"
#include "object.h"
#include "Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"

namespace scd::components {
class button;

class IEvent;

class scroller : public scd::base_component {
public:
  scroller(scd::object* owner);

  static int create(lua_State* pLua);
  static scroller* create(button* button, float xVel, float yVel, float xSize,
                          float ySize);

  void set_position_x(float fX) { _position.x = fX; }
  void set_position_y(float fY) { _position.y = fY; }

  static void update(IEvent* cEvent, scd::base_component* cCenter);

  static void on_init(IEvent* cEvent, scd::base_component* cCenter);
  static void on_enable(IEvent* cEvent, scd::base_component* cCenter);
  static void on_disable(IEvent* cEvent, scd::base_component* cCenter);

private:
  button* _button;

  scd::vector2 _position;
  scd::vector2 _velocity;
  scd::vector2 _size;

  scd::vector2 _start_position;
};
} // namespace scd::components

#endif // __scd_components_scroller_h__
