/**
 * @file camera_manager.cpp
 *
 * @author Joseph Leybovich
 *
 * @brief A manager for cameras
 */
#include "camera_manager.h"

namespace scd {
void camera_manager::camera_manager(int screen_width, int screen_height) {
  // Setup camera.
  object* camera = object_manager::get().create("Camera0");

  _camera = std::make_unique<camera>();

  // BuildPerspective(field of view, aspect ratio, near plane, far plane)
  _camera->build_perspective(
    D3DX_PI / 3.0f, (float)nScreenWidth / (float)nScreenHeight, 1.0f, 200.0f);

  event_manager& event_manager = event_manager::get();
  event_manager.register_event("AttachToCamera",
                               std::bind(&on_attach_to_camera, this));
  event_manager.register_event("UpdateState", std::bind(&on_update, this));
  event_manager.register_event("AttachToCameraWin",
                               std::bind(&on_attach_to_win_state, this));
  event_manager.register_event("AttachToCameraLose",
                               std::bind(&on_attach_to_lose_state, this));
}

int camera_manager::attach_camera_to_player(lua_State* pLua) {
  std::string object_name = lua_tostring(pLua, -1);
  lua_pop(pLua, 1);

  object* object = object_manager::get().find_by_name(object_name);

  if (object == nullptr) {
    Debug.Print("Invalid Object Name");
    return 0;
  }

  _camera->GetTarget1().SetParent(pObj->GetTransform());
  _camera->SetTarget(&_camera->GetTarget1());
  _camera->SetFrameParent(pObj->GetTransform());

  return 0;
}

void camera_manager::on_attach_to_camera(const event& e) {
  object_event* event_data = dynamic_cast<object_event*>(&e.data());

  D3DXMatrixIdentity(&_camera->GetFrame().GetLocalMatrix());
  _camera->GetFrame().SetParent(NULL);

  _camera->GetTarget1().SetParent(eObj->m_pcObj->GetTransform());
  D3DXMatrixIdentity(&_camera->GetTarget1().GetLocalMatrix());

  _camera->SetTarget(&_camera->GetTarget1());
  _camera->SetFrameParent(eObj->m_pcObj->GetTransform());
  _camera->GetFrame().TranslateFrame(
    D3DXVECTOR3(0.0f, 4.5f, -4.0f));
  _camera->GetTarget()->TranslateFrame(
    D3DXVECTOR3(0.0f, 1.0f, 5.0f));
}

void camera_manager::on_update(const event& e) {
  update_state_event* event_data = dynamic_cast<update_state_event*>(&e.data());

  _camera->update(event_data->delta_time);
}

void camera_manager::on_attach_to_win_state(const event& e) {
  object_event* event_data = dynamic_cast<object_event*>(&e.data());

  D3DXMatrixIdentity(&_camera->GetFrame().GetLocalMatrix());
  _camera->GetFrame().SetParent(NULL);

  _camera->GetTarget1().SetParent(eObj->m_pcObj->GetTransform());
  D3DXMatrixIdentity(&_camera->GetTarget1().GetLocalMatrix());

  _camera->SetTarget(&_camera->GetTarget1());
  _camera->SetFrameParent(eObj->m_pcObj->GetTransform());
  _camera->GetFrame().TranslateFrame(
    D3DXVECTOR3(0.0f, 2.0f, 4.0f));
  _camera->GetTarget()->TranslateFrame(
    D3DXVECTOR3(0.0f, 1.0f, -2.0f));
}

void camera_manager::on_attach_to_lose_state(const event& e) {
  object_event* event_data = dynamic_cast<object_event*>(&e.data());

  D3DXMatrixIdentity(&_camera->GetFrame().GetLocalMatrix());
  _camera->GetFrame().SetParent(NULL);

  _camera->GetTarget1().SetParent(eObj->m_pcObj->GetTransform());
  D3DXMatrixIdentity(&_camera->GetTarget1().GetLocalMatrix());

  _camera->SetTarget(&_camera->GetTarget1());
  _camera->SetFrameParent(eObj->m_pcObj->GetTransform());
  _camera->GetFrame().TranslateFrame(
    D3DXVECTOR3(0.0f, 2.0f, 4.0f));
  _camera->GetTarget()->TranslateFrame(
    D3DXVECTOR3(0.0f, 1.0f, -2.0f));
}
} // namespace scd
