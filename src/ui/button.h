#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "enums.h"
#include "rendering/texture_manager.h"

namespace scd::components {
class IEvent;

class button : public scd::base_component {
public:
  button(scd::object* owner);

  static int CreateButtonComponent(lua_State* lua);
  static button* CreateButtonComponent(scd::object* owner,
                                       std::string function_name,
                                       std::string on_rollover_function_name,
                                       std::string sprite_texture_name,
                                       int position_x, int position_y,
                                       bool start_selected, int game_state,
                                       int texture_depth);

  static int SetNextButtonComponent(lua_State* lua);
  static void SetNextButtonComponent(button* me, button* down, button* up,
                                     button* left, button* right);

  void init(std::string button_texture_name, int texture_depth);
  void load(std::string button_texture_name, int texture_depth, int position_x,
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
  static void update(IEvent* cEvent, scd::base_component* cCenter);

  // This is to fix the menu creeper bug - This means in update we'll never
  // have buttons taht work
  static void gameplay_update(IEvent* cEvent, scd::base_component* cCenter);

  static void up_pressed(IEvent* cEvent, scd::base_component* cCenter);
  static void down_pressed(IEvent* cEvent, scd::base_component* cCenter);
  static void left_pressed(IEvent* cEvent, scd::base_component* cCenter);
  static void right_pressed(IEvent* cEvent, scd::base_component* cCenter);
  static void select_pressed(IEvent* cEvent, scd::base_component* cCenter);
  static void invalid_selection(IEvent* cEvent, scd::base_component* cCenter);
  static void button_state_enabled(IEvent* cEvent,
                                   scd::base_component* cCenter);
  static void button_state_disabled(IEvent* cEvent,
                                    scd::base_component* cCenter);
  static void button_state_init(IEvent* cEvent, scd::base_component* cCenter);
  static void button_state_load(IEvent* cEvent, scd::base_component* cCenter);

protected:
  EGameState _associated_state;

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

} // namespace scd::components
