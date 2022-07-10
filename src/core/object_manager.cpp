#include "object_manager.h"

#include "core/object.h"
#include "events/events.h"

namespace scd {

int object_manager::create(lua_State* lua) {
  // Get the Obj Name from LUA
  std::string name = lua_tostring(lua, -5);
  float position_x = static_cast<float>(lua_tonumber(lua, -4));
  float position_y = static_cast<float>(lua_tonumber(lua, -3));
  float position_z = static_cast<float>(lua_tonumber(lua, -2));
  float heading    = static_cast<float>(lua_tonumber(lua, -1));

  // Take the Data off the LUA Stack
  lua_pop(lua, 5);

  // Create the Obj
  object* object = create(name, {position_x, position_y, position_z}, heading);

  // Push the Obj onto the LUA Stack
  lua_pushlightuserdata(lua, object);

  // 1 Return Value(s)
  return 1;
}

object* object_manager::create(const std::string& name,
                               const scd::vector3& position, float heading,
                               object* parent) {
  std::shared_ptr<object> obj = std::make_shared<object>(name);

  obj->set_local_position(position);
  obj->rotate(_world_up, heading);

  // Set Transform Frame
  if (parent != nullptr) {
    bind_objects(parent, obj);
  }

  _objects.emplace_back(obj);

  return obj.get();
}

void object_manager::destroy(object* obj) {
  // Send an event to notify all component managers to destroy the components
  // associated with this object
  events::object::send(
    "DestroyObject", (IComponent*)GetInstance(), pObj, PRIORITY_IMMEDIATE);

  _objects.remove(obj);
}

int object_manager::bind_objects(lua_State* lua) {
  object* parent = (object*)lua_topointer(lua, -2);
  object* child  = (object*)lua_topointer(lua, -1);

  bind_objects(parent, child);

  lua_pop(lua, 2);

  return 0;
}

void object_manager::bind_objects(object* parent, object* child) {
  parent->add_child(*child);
}

int object_manager::by_name(lua_State* lua) {
  // Get the name from Lua
  std::string name = lua_tostring(lua, -1);
  lua_pop(lua, 1);

  object* obj = by_name(name);

  // Pass the object pointer back to Lua
  lua_pushlightuserdata(lua, static_cast<void*> obj);

  return 1;
}

object* object_manager::by_name(const std::string& name) {
  // use the passed in name to find the object with the same name
  unsigned int name_id = id_gen::get().get_id(name);

  for (const auto& obj : _objects) {
    if (obj->id() == name_id)
      return obj.get();
  }

  // Didn't find object with specified name
  return nullptr;
}
} // namespace scd
