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
#include "core/id_generator.h"
#include "physics/math_types.h"

#include <memory>
#include <string>

namespace scd {
class component;

class object : public std::enable_shared_from_this<object> {
public:
  object(id_generator::id_t id)
      : _id(id) {}

  id_generator::id_t id() const { return _id; }

  /** Adds a component to the Object.
   *
   * Objects cannot have more than one instance of a single type of
   * component e.g. One object having two render components.
   *
   * @param component The Component to add.
   */
  template <class T>
  void add_component(const std::shared_ptr<T>& component) {
    // TODO: use sfinae?
    static_assert(
        std::is_base_of<scd::base_component, T>::value,
        "Type must be a component!");

    // TODO: check that there isn't already a component of this type.
    _components.emplace_back(component);
  }

  template <class T, typename... Args>
  std::shared_ptr<T> create_component(Args&&... args) {
    auto component = std::make_shared<T>(std::forward<Args>(args)...);
    _components.emplace_back(component);
    return component;
  }

  /** Removes a component from the Object.
   *
   * This function does not clean up the component's memory or remove it from
   * its manager.
   *
   * @param component The Component to remove.
   */
  void remove_component(const std::shared_ptr<scd::base_component>& component) {
    _components.remove(component);
  }

private:
  scd::list<std::shared_ptr<scd::base_component>> _components;

  const id_generator::id_t _id;

  scd::vector3 _last_world_position;
  scd::transform _local_pose;
  scd::transform _world_pose;

  std::weak_ptr<scd::object> _parent;
  scd::list<std::shared_ptr<scd::object>> _children;

  bool _dirty = false;

public:
  void add_child(scd::object& child) {
    auto parent = child._parent.lock();
    if (parent == shared_from_this()) {
      // nothing to do
      return;
    }

    // detach from old parent
    if (parent) {
      parent->remove_child(child);
    }

    _children.push_back(child.shared_from_this());
    child._parent = weak_from_this();
    child.dirty();
  }

  scd::object* remove_child(scd::object& child) {
    _children.erase(child.shared_from_this());
    child._parent.reset();
    child.dirty();
    return &child;
  }

  // sets all the children of a frame to "DIRTY"
  void dirty() {
    _dirty = true;

    for (auto& child : _children) {
      child->dirty();
    }
  }

  const scd::transform& local_pose() const { return _local_pose; }

  const scd::transform& world_pose() {
    if (_dirty) {
      if (auto parent = _parent.lock()) {
        _world_pose = parent->world_pose() * _local_pose;
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
    world_pose();
  }

  void scale(const scd::vector3& scale) {
    scd::transform scale_matrix =
        math::matrix_scale(math::matrix_identity(), scale);
    _local_pose = math::matrix_multiply(_local_pose, scale_matrix);
    world_pose();
  }

  void translate(const scd::vector3& translation) {
    _last_world_position = world_position();
    _local_pose = math::matrix_translate(_local_pose, translation);
    world_pose();
  }

  const scd::vector3& last_world_position() const {
    return _last_world_position;
  }
  const scd::vector3& world_position() { return world_pose().p; }

  const scd::vector3& local_position() const { return _local_pose.p; }

  void local_position(const scd::vector3& position) {
    _local_pose.p = position;
  }
};
} // namespace scd
