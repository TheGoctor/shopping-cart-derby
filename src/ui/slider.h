#pragma once

#include "ui/button.h"

struct lua_State;

namespace scd {
  class event;
}

namespace scd::ui {

class slider : public scd::button {
private:
  scd::vector3 _start_position;
  scd::vector3 _end_position;

  int _slider_for;

  float _slider_value;
  float _state_entered_slider_value;

public:
  slider(scd::object& owner);

  static int create(lua_State* pLua);
  static slider* create(scd::object& owner, const std::string& event_name,
                        const std::string& sprite_texture_name, int left, int top,
                        int right, int bottom, int texture_depth,
                        bool start_selected, int slider_for, game_state state);

  void init(const std::string& button_texture_name, int texture_depth);
  void reinit();
  void activate();
  void deactivate();
  void hide();

  void set_screen_position(int left, int top, int right, int bottom);

  void calculate_and_send_float_event();
  void move_slider_one_tick(int direction);
  void set_slider_to_value(float amount, bool send_value = true);
  static int set_slider_value(lua_State* pLua);

  static void on_select_pressed(event* cEvent, scd::base_component* cCenter);
  static void on_slide_left(event* cEvent, scd::base_component* cCenter);
  static void on_slide_right(event* cEvent, scd::base_component* cCenter);
  static void on_slider_state_init(event* cEvent, scd::base_component* cCenter);
  static void on_slider_cancel_pause(event* cEvent, scd::base_component* cCenter);
  static void on_slider_cancel_main(event* cEvent, scd::base_component* cCenter);

  void slider_cancel(int eState);
};
} // namespace scd::ui
