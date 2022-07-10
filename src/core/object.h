/*
 * @file Object.h
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

  object(std::string name)
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

  scd::transform _pose;

  const std::string _name;

  // The ID of the object. ID is generated from the name of the object
  id_t _id;

private:
  scd::vector3 _last_world_position;
  scd::transform _local_pose;
  scd::transform _world_pose;

  scd::object* _parent = nullptr;
  std::list<scd::object*> _children;

  std::uint32_t _flags = 0;

public:
  enum { DIRTY = 1 };

  void add_child(scd::object& child) {
    if (child._parent != nullptr) {
      if (child._parent != this) {
        child._parent->remove_child(child);
        _children.push_back(&child);
        child._parent = this;
      }
    } else {
      _children.push_back(&child);
      child._parent = this;
      child.update_world_pose();
    }
  }

  scd::object* remove_child(scd::object& child) {
    scd::object* temp_frame = nullptr;
    for (auto& _child : _children) {
      if (child == _child) {
        temp_frame = _child;
        _children.erase(_child);
        temp_frame->_parent = nullptr;
        break;
      }
    }
    return temp_frame;
  }

  // sets all the children of a frame to "DIRTY"
  void SetChildFlags() {
    for (auto& child : _children) {
      child->SetChildFlags();
    }
  }

  void update_world_pose(void) {
    _last_world_position
      = scd::vector3(world_pose()._41, world_pose()._42, world_pose()._43);
    if (_flags != DIRTY) {
      _flags = DIRTY;

      for (auto& child : _children) {
        child->update_world_pose();
      }
    }
  }

  const scd::transform& local_pose() const { return _local_pose; }

  void set_world_pose(const scd::transform& world_pose) {
    _world_pose = world_pose;
  }

  const scd::transform& world_pose() const {
    if (_flags == DIRTY) {
      if (_parent != nullptr) {
        D3DXMatrixMultiply(&_world_pose, &_local_pose, &_parent->world_pose());
      } else {
        _world_pose = _local_pose;
      }
    }
    return _world_pose;
  }
  void rotate(const scd::vector3& axis, float angle) {
    scd::transform rotMat;
    D3DXMatrixRotationAxis(&rotMat, &vRotAxis, fAngle);
    D3DXMatrixMultiply(&m_mLocalMatrix, &rotMat, &m_mLocalMatrix);
    update_world_pose();
  }
  void scale(float x, float y, float z) {
    scd::transform scaleMat;
    D3DXMatrixScaling(&scaleMat, x, y, z);
    D3DXMatrixMultiply(&_local_pose, &_local_pose, &scaleMat);
    update_world_pose();
  }

  scd::vector3 last_world_position() const { return _last_position; }
  scd::vector3 world_position() {
    return scd::vector3(world_pose()._41, world_pose()._42, world_pose()._43);
  }

  scd::vector3 get_local_position() const {
    return scd::vector3(local_pose()._41, local_pose()._42, local_pose()._43);
  }

  void set_local_position(const scd::vector3& position) {
    _local_pose.p = position;
  }

  void translate_frame(const scd::vector3& translation) {
    _last_position.x = world_pose()._41;
    _last_position.y = world_pose()._42;
    _last_position.z = world_pose()._43;
    scd::transform translateMat;
    D3DXMatrixTranslation(
      &translateMat, translation.x, translation.y, translation.z);
    D3DXMatrixMultiply(&_local_pose, &_local_pose, &translateMat);
    update_world_pose();
  }
};
} // namespace scd
