/*
 * @file object.h
 *
 * @brief Fundamental game object.
 *
 * @author Mac Reichelt
 *
 * @copyright (c) 2016 Mac Reichelt
 */
#pragma once

#include "core/containers.h"
#include "physics/math_types.h"

#include <string>

namespace scd {
class component;

class object {
public:
  using id_t = std::uint32_t;

  object(const std::string& name)
      : _name(name) {}

  std::string name() const { return _name; }
  const scd::transform& pose() const { return _pose; }
  id_t id() const { return _id; }

  /** Adds a component to the Object.
   *
   * Objects cannot have more than one instance of a single type of
   * component e.g. One object having two render components.
   *
   * @param component The Component to add.
   */
  template <class T, class... Args>
  void add_component<T>(Args&&... args) {
    _components.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
  }

  /** Removes a component from the Object.
   *
   * This function does not clean up the component's memory or remove it from
   * its manager.
   *
   * @param component The Component to remove.
   */
  void remove_component(const std::shared_ptr<scd::component>& component) {
    _components.remove(component);
  }

private:
  scd::list<std::shared_ptr<scd::component>> _components;

  const std::string _name;

  // The ID of the object. ID is generated from the name of the object
  const id_t _id;

private:
  scd::vector3 _last_world_position;
  scd::transform _local_pose;
  scd::transform _world_pose;

  scd::object* _parent = nullptr;
  scd::list<scd::object*> _children;

  bool _dirty = false;

public:
  void add_child(scd::object& child) {
    if (child._parent == this) {
      // nothing to do
      return;
    }

    // detach from old parent
    if (child._parent != nullptr) {
      child._parent->remove_child(child);
    }

    _children.push_back(&child);
    child._parent = this;
    child.dirty();
  }

  scd::object* remove_child(scd::object& child) {
    _children.erase(&child);
    child._parent = nullptr;
    child.dirty();
  }

  // sets all the children of a frame to "DIRTY"
  void dirty() {
    _dirty = true;

    for (auto& child : _children) {
      child->dirty();
    }
  }

  const scd::transform& local_pose() const { return _local_pose; }

  const scd::transform& world_pose() const {
    if (_dirty) {
      if (_parent != nullptr) {
        _world_pose = _parent->world_pose() * _local_pose;
      } else {
        _world_pose = _local_pose;
      }

      // mark clean
      _dirty = false;
    }

    return _world_pose;
  }

  void rotate(const scd::vector3& axis, float angle) {
    scd::transform rotation = math::matrix_axis_angle(axis, angle);
    _local_pose = math::matrix_multiply(_local_pose, rotation);
    update_world_pose();
  }

  void scale(const scd::vector3& scale) {
    scd::transform scale_matrix =
        math::matrix_scale(math::matrix_identity(), scale);
    _local_pose = math::matrix_multiply(_local_pose, scale_matrix);
    update_world_pose();
  }

  void translate(const scd::vector3& translation) {
    _last_position = world_position();
    _local_pose = math::matrix_translate(_local_pose, translation);
    update_world_pose();
  }

  const scd::vector3& last_world_position() const { return _last_position; }
  const scd::vector3& world_position() const { return world_pose().p; }

  const scd::vector3& local_position() const { return _local_pose.p; }

  void local_position(const scd::vector3f& position) {
    _local_pose.p = position;
  }
};
} // namespace scd
