#pragma once

#include "enums.h"

#include <string>

struct lua_State;

namespace scd::event {
void send(
    const std::string& event_name,
    base_component* sender,
    void* data,
    event_priority priority);

struct update_state {
  const float _delta_time;

  update_state(float delta_time)
      : _delta_time(delta_time) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      float delta_time,
      event_priority priority = event_priority::update);
};

struct state_change {
  const game_state _new_state;

  state_change(game_state new_state)
      : _new_state(new_state) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      game_state new_state,
      event_priority priority = event_priority::input);
};

struct render {
  object& _object;

  render(object& object)
      : _object(object) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& object,
      event_priority priority = event_priority::render);
};

struct ram {
  object& _rammer;
  object& _rammee;

  ram(object& rammer, object& rammee)
      : _rammer(rammer)
      , _rammee(rammee) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& rammer,
      object& rammee,
      event_priority priority = event_priority::normal);
};

struct object_event {
  object& _object;

  object_event(object& object)
      : _object(object) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& pObj,
      event_priority priority = event_priority::NORMAL);
};

struct input {
  object& _player;
  const float _amount;

  input(object& player, float amount)
      : _player(player)
      , _amount(amount) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& player,
      float amount,
      event_priority priority = event_priority::input);
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
struct held_item_collected {
  object& _held_item;
  object& _collector;

  held_item_collected(object& held_item, object& collector)
      : _held_item(held_item)
      , _collector(collector) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& held_item,
      object& collector,
      event_priority priority = event_priority::normal);
};

struct heading {
  const vector3 _heading;

  heading(const vector3& heading)
      : _heading(heading) {}

  static int create(lua_State* lua); // takes (x,y,z)

  static void send(
      const std::string& event_name,
      base_component* sender,
      const vector3& heading,
      event_priority priority = event_priority::normal);
};

struct goal_item {
  const goal_item_type _type;
  scd::goal_item& _item;

  goal_item(goal_item_type item_type, scd::goal_item& item)
      : _type(item_type)
      , _item(item) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      goal_item_type item_type,
      scd::goal_item& item,
      event_priority priority = event_priority::normal);
};

struct weight_class {
  const cart_weight _weight;
  object& _object;

  weight_class(cart_weight weight, object& object)
      : _weight(weight)
      , _object(object) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& object_to_change,
      cart_weight weight,
      event_priority priority = event_priority::normal);
};

struct goal_item_collected {
  object& _goal_item;
  object& _collector;

  goal_item_collected(object& goal_item, object& collector)
      : _goal_item(goal_item)
      , _collector(collector) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& goal_item,
      object& collector,
      event_priority priority = event_priority::normal);
};

struct impact {
  object& _collider; // object to rebound
  object& _collidee;

  const vector3 _normal;
  const vector3 _collision_point;

  impact(
      object& collider,
      object& collidee,
      const vector3& normal,
      const vector3& collision_point)
      : _collider(collider)
      , _collidee(collidee)
      , _normal(normal)
      , _collision_point(collision_point) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& collider,
      object& collidee,
      const vector3& normal,
      event_priority priority = event_priority::normal,
      const vector3& collision_point = {0.0f, 0.0f, 0.0f});
};

struct status_effect {
  object& _object;
  const float _duration;

  status_effect(object& object, float duration)
      : _object(object)
      , _duration(duration) {}

  static int create(lua_State* lua);

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& object_to_affect,
      float duration,
      event_priority priority = event_priority::normal);
};

struct float_data {
  const float _value;

  float_data(float value)
      : _value(value) {}

  static void send(
      const std::string& event_name,
      base_component* sender,
      float value,
      event_priority priority = event_priority::normal);
};

struct spawn_pickup_item {
  const vector3 _vector0;
  const vector3 _vector1;
  const int _item_id;

  spawn_pickup_item(const vector3& vector0, const vector3& vector1, int item_id)
      : _vector0(vector0)
      , _vector1(vector1)
      , _item_id(item_id) {}

  void send(
      const std::string& event_name,
      base_component* sender,
      const vector3& vector0,
      const vector3& vector1,
      int item_id,
      event_priority priority = event_priority::normal);
};

struct pickup_item_collected {
  object& _pickup_item;
  object& _collector;

  const goal_item_type _item_type;

  pickup_item_type(
      object& pickup_item,
      object& collector,
      goal_item_type item_type = goal_item_type::none)
      : m_pcPickupItem(pPickupItem)
      , m_pcCollector(pCollector)
      , m_eItemType(eType) {}

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& goal_item,
      object& collector,
      goal_item_type item_type,
      event_priority priority = event_priority::normal);
};

struct pickup_item {
  object& _pickup_item;
  const goal_item_type _item_type;

  pickup_item(
      object& pickup_item,
      goal_item_type item_type = goal_item_type::none)
      : _pickup_item(pickup_item)
      , _item_type(item_type) {}

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& goal_item,
      goal_item_type item_type,
      event_priority priority = event_priority::normal);
};

struct two_int {
  const std::int32_t _value0;
  const std::int32_t _value1;

  two_int(int value0, int value1)
      : _value0(value0)
      , _value1(value1) {}

  static void send(
      const std::string& event_name,
      base_component* sender,
      int value0,
      int value1,
      event_priority priority = event_priority::normal);
};

struct mouse {
  const std::int32_t _position_x;
  const std::int32_t _position_y;

  mouse(std::int32_t position_x, std::int32_t position_y)
      : _position_x(position_x)
      , _position_y(position_y) {}

  static void send(
      const std::string& event_name,
      base_component* sender,
      std::int32_t position_x,
      std::int32_t position_y,
      event_priority priority = event_priority::input);
};

int send_lua_event(lua_State*);

struct console {
  const char _key_pressed;

  console(char key_pressed)
      : _key_pressed(key_pressed) {}

  static void send(
      const std::string& event_name,
      base_component* sender,
      char key,
      event_priority priority = event_priority::input);
};

struct speed {
  object& _object;
  const float _speed;

  speed(object& object, float speed)
      : _object(object)
      , _speed(speed) {}

  static void send(
      const std::string& event_name,
      base_component* sender,
      object& object,
      float speed,
      event_priority priority = event_priority::normal);
};

struct held_item_spawned {
  scd::held_item& _held_item;

  held_item_spawned(scd::held_item& held_item)
      : _held_item(held_item) {}

  void send(
      const std::string& event_name,
      base_component* sender,
      scd::held_item& held_item,
      event_priority priority = event_priority::normal);
};

struct string_data {
  std::string _value;

  string_data(const std::string& value)
      : _value(value) {}

  static void send(
      const std::string& event_name,
      base_component* sender,
      const string& value,
      event_priority priority = event_priority::normal);
};

} // namespace scd::events
