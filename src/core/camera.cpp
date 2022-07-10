/**
 * @file camera.cpp
 *
 * @author Huy Nguyen
 *
 * @brief Camera class that controls the camera movement and view
 */

#include "camera.h"
#include "core/object_manager.h"

namespace scd {
scd::vector3 scd::camera::_world_up(0.0f, 1.0f, 0.0f);

scd::transform& camera::view_matrix() const {
  D3DXMatrixInverse(&_view_matrix, 0, &_view_matrix);
  _view_matrix._11 *= -1.0f;
  _view_matrix._12 *= -1.0f;
  _view_matrix._13 *= -1.0f;
  _view_matrix._14 *= -1.0f;
  D3DXMatrixInverse(&_view_matrix, 0, &_view_matrix);
  return _view_matrix;
}

void camera::update(float delta_time) {
  float time = 0.0f;
  time += delta_time;

  scd::transform camera_matrix;

  _eye_position = _frame.p;

  _look_at_position = _target_ptr->p;

  physics_manager::get().TestObjAgainstWall(
      _eye_position, (_look_at_position - _eye_position));

  D3DXMatrixLookAtLH(&cameraMatrix, &m_vEyePos, &m_vLookAtPos, &s_vWorldUp);

  _matrices.push(camera_matrix);

  if (_matrices.size() >= m_iQueueSize) {
    _view_matrix = _matrices.front();
    _matrices.pop();
  }

  D3DXMatrixMultiply(
      &_view_projection_matrix, &view_matrix(), &projection_matrix());
}

void camera::build_perspective(
    float field_of_view,
    float aspect_ratio,
    float near_z,
    float far_z) {
  D3DXMatrixPerspectiveFovLH(
      &_projection_matrix, field_of_view, aspect_ratio, near_z, far_z);
}

const scd::vector3& camera::view_position() {
  _position = _view_matrix.p;
  return _position;
}
} // namespace scd
