/**
 * @file base_component.h
 *
 * @brief Base class for all component classes so they may be added to
 * the Object Class.
 *
 * @author Mac Reichelt
 *
 * @copyright (c) 2016 Mac Reichelt
 */

#pragma once

#include <memory>

namespace scd {
class object;

class base_component : public std::enable_shared_from_this<base_component> {
public:
  using pointer      = std::shared_ptr<base_component>;
  using weak_pointer = std::weak_ptr<base_component>;

  virtual ~base_component() = default;

  bool is_active() const { return _is_active; }
  void activate() { _is_active = true; }
  void deactivate() { _is_active = false; }

  const scd::object& owner() const { return _owner; }

protected:
  explicit base_component(scd::object& owner)
    : _owner{ owner }
    , _is_active{ true }
  {
  }

protected:
  scd::object& _owner;
  bool _is_active;
};
} // namespace scd
