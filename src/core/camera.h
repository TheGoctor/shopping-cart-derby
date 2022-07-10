/**
 * @file camera.h
 *
 * @author Huy Nguyen
 *
 * @brief Camera class that controls the camera movement and view
 */

#pragma once

#include "core/containers.h"
#include "physics/collider.h"
#include "physics/math_types.h"

namespace scd {
class camera {
public:
  camera();
  camera(const camera&);
  camera& operator=(const camera&);
  ~camera();

  const scd::transform& projection_matrix() const { return _projection_matrix; }
  const scd::transform& view_projection_matrix() const {
    return _view_projection_matrix;
  }
  scd::transform* target_ptr() const { return _target_ptr; }
  const scd::transform& frame() const { return _frame; }
  const scd::transform& target() const { return _target; }
  const scd::vector3& eye_position() const { return _eye_position; }
  const scd::vector3& look_at_position() const { return _look_at_position; }
  const scd::vector3& view_position() const;
  const scd::transform& view_matrix() const;

  void set_target(const scd::transform& target) { _target_ptr = &target; }
  void set_frame_parent(CFrame* frame) { m_frame.SetParent(frame); }

  /**
   * Builds the projection matrix for the camera
   *
   * @param[in] field_of_view the field of view value
   * @param[in] aspect the Aspect of the camera
   * @param[in] near_z the near plane
   * @param[in] far_z the far plane
   */
  void build_perspective(float field_of_view, float aspect, float near_z,
                         float far_z);

  /**
   * Updates the camera's position and does the look at
   *
   * @param[in] delta_time The time in seconds since the last update.
   */
  void update(float delta_time);

private:
  scd::queue<scd::transform> _matrices;

  scd::transform _view_matrix;
  scd::transform _projection_matrix;
  scd::transform _view_projection_matrix;
  scd::transform _world;
  scd::vector3 _postion;
  scd::vector3 _eye_position;
  scd::vector3 _look_at_position;
  scd::object* _frustum;
  scd::collider* _frustum_bounds;
  scd::geometry::sphere _camera_sphere;
  scd::vector3 _collision_transform;
  scd::transform _frame;
  scd::transform* _target_ptr;
  scd::transform _target;
  static scd::vector3 _world_up;
};
} // namespace scd
