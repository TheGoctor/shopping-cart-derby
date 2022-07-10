/**
 * @file camera_manager.h
 *
 * @author Joseph Leybovich
 *
 * @brief A manager for cameras
 */
#pragma once

#include "core/camera.h"

#include <memory>

namespace scd {
class camera_manager {
private:
  std::unique_ptr<camera> _camera = nullptr;

public:
  /**
   * Initializes the Camera and builds the perspective and registers for events
   *
   * @param[in] screen_width The width of the screen in pixels.
   * @param[in] screen_height The height of the screen in pixels.
  */
  camera_manager(int screen_width, int screen_height);

  const camera* camera() const { return _camera.get(); }

  void on_attach_to_camera(const event&);
  void on_move_camera(const event&);
  void on_update(const event&);
  void on_attach_to_win_state(const event&);
  void on_attach_to_lose_state(const event&);

  static int attach_camera_to_player(lua_State* pLua);
};
} // namespace scd
