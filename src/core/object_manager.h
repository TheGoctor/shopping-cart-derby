/**
 * @file object_manager.h
 *
 * @author Joseph Leybovich
 *
 * @brief This class is primarily used to load game objects upon entering a game
 * state, and disabling game objects upon exiting a state.
 */

#pragma once

#include "core/containers.h"

#include <memory>

namespace scd {
class object_manager {
  // A list of all the game objects currently in the world
  scd::list<std::shared_ptr<object>> _objects;

  object_manager() = default;
  ~object_manager() = default;

  object_manager(const object_manager&) = delete;
  object_manager& operator=(const object_manager&) = delete;

public:
  /**
   * The factory function used to create objects. All created objects must have
   * a unique name and must be created using this function.
   *
   * @param[in] name The name of the object. Must be unique.
   * @param[in] position the object's starting location.
   * @param[in] heading The object's starting rotation around Y axis
   * @param[in] parent The object's frame's parent.
   *
   * @return Pointer to the newly created object
   */
  object* create(const std::string& name, const scd::vector3& position,
                 float heading = 0.0f, object* parent = nullptr);
  static int create(lua_State* lua);

  /**
   * Tells the component managers to destroy their components attached to this
   * object, unregisters the object for all events, and deletes the object.
   *
   * @param[in] object The pointer to the object to be destroyed
   */
  static void destroy(object* object);

  /**
   * Binds the frames of the two passed in objects so the child object and
   * parent objects are bound appropriately in their frame hierarchies.
   *
   * @param[in] parent Pointer to the parent object
   * @param[in] child Pointer to the child object
   */
  static int bind_objects(lua_State* lua);
  void bind_objects(object* parent, object* child);

  /**
   * Searches for an object with the given name. If no object is found, the
   * function returns nullptr.
   *
   * @param[in] name The name of the object.
   *
   * @return Pointer to the object with the passed in name. NULL if object
   * wasn't found.
   */
  object* by_name(const std::string& name);
  static int by_name(lua_State* lua);
};

} // namespace scd
