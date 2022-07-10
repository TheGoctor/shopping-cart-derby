#include "object_manager.h"

#include "core/id_generator.h"
#include "core/object.h"
//#include "events/events.h"

#include <cassert>

namespace scd {

std::shared_ptr<object> object_manager::create(
    const std::string& name,
    const scd::vector3& position,
    float heading,
    object* parent) {
  id_generator::id_t id = _id_gen.get_id(name);

  // Don't create an object without a unique name
  assert(by_name(name) == nullptr);

  std::shared_ptr<object> obj = std::make_shared<object>(id);

  obj->local_position(position);
  obj->rotate(world_up, heading);

  // Set Transform Frame
  if (parent != nullptr) {
    bind_objects(*parent, *obj);
  }

  _objects.emplace(id, obj);

  return obj.get();
}

void object_manager::destroy(const std::shared_ptr<object>& obj) {
  // Send an event to notify all component managers to destroy the components
  // associated with this object
  // events::object::send("DestroyObject", obj, PRIORITY_IMMEDIATE);
  if (obj) {
    _objects.erase(_objects.find(obj->id()));
  }
}

void object_manager::bind_objects(object& parent, object& child) {
  parent.add_child(child);
}

object* object_manager::by_name(const std::string& name) {
  // use the passed in name to find the object with the same name
  id_generator::id_t name_id = _id_gen.get_id(name);

  auto obj = _objects.find(name_id);
  if (obj != _objects.end()) {
    return obj->second;
  }

  // Didn't find object with specified name
  return nullptr;
}
} // namespace scd
