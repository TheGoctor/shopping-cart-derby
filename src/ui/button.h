#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "enums.h"
#include "rendering/texture_manager.h"

struct lua_State;

namespace scd {
class event;
}

namespace scd::ui {

class button : public scd::base_component {
public:
  button(scd::object& owner);

  static int CreateButtonComponent(lua_State* lua);
  static button* CreateButtonComponent(scd::object& owner,
                                       const std::string& function_name,
                                       const std::string& on_rollover_function_name,
                                       const std::string& sprite_texture_name,
                                       int position_x, int position_y,
                                       bool start_selected, int game_state,
                                       int texture_depth);

  static int SetNextButtonComponent(lua_State* lua);
  static void SetNextButtonComponent(button* me, button* down, button* up,
                                     button* left, button* right);

  void init(const std::string& button_texture_name, int texture_depth);
  void load(const std::string& button_texture_name, int texture_depth, int position_x,
            int position_y);
  void reinit_values();
  void activate();
  void deactivate();
  void unshow();

  void set_screen_position(int position_x, int position_y);

  int associated_state() const { return _associated_state; }

  scd::components::sprite* sprite_component() const {
    return _display_component_button;
  }

  /**
   * Calls all of the functions that need to be called every frame.
   *
   * @param cSender the sender of the event
   * @param cEvent Actually a CUpdateEvent that holds the frame's delta time
   */
  static void update(event* cEvent, scd::base_component* cCenter);

  // This is to fix the menu creeper bug - This means in update we'll never
  // have buttons taht work
  static void gameplay_update(event* cEvent, scd::base_component* cCenter);

  static void up_pressed(event* cEvent, scd::base_component* cCenter);
  static void down_pressed(event* cEvent, scd::base_component* cCenter);
  static void left_pressed(event* cEvent, scd::base_component* cCenter);
  static void right_pressed(event* cEvent, scd::base_component* cCenter);
  static void select_pressed(event* cEvent, scd::base_component* cCenter);
  static void invalid_selection(event* cEvent, scd::base_component* cCenter);
  static void button_state_enabled(event* cEvent,
                                   scd::base_component* cCenter);
  static void button_state_disabled(event* cEvent,
                                    scd::base_component* cCenter);
  static void button_state_init(event* cEvent, scd::base_component* cCenter);
  static void button_state_load(event* cEvent, scd::base_component* cCenter);

protected:
  game_state _associated_state;

  scd::button* _select_down;
  scd::button* _select_up;
  scd::button* _select_left;
  scd::button* _select_right;

  std::string _selection_function;
  std::string _on_scroll_over_function;

  scd::object* _display_object_button;
  scd::components::sprite* _display_component_button;
  TSpriteData _sprite_data_button;

  scd::object* _display_object_selection;
  scd::components::sprite* _display_component_selection;
  TSpriteData _sprite_data_selection;

  float _time_since_selection_entry;
  float _input_cooldown;
  bool _selected;
  bool _selected_start_value;
};

} // namespace scd::ui
