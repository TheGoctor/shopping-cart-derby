#pragma once

#include "core/containers.h"
#include "memory/allocator.h"
#include "rendering/texture_manager.h"

namespace scd {

class race_start_manager {
public:
  race_start_manager();

  void start_bike_sounds();

  void on_update();
  void on_state_init();
  void on_keypress_enter();

private:
  float _total_duration;
  float _current_timer_value;
  bool _enable_input;
  bool _has_played_sound;
  bool _bike_started;
  bool _welcome_sound;
  CWwiseSoundManager* pSound;

  CSpriteComponent* m_pDirectionsComp;
  bool _waiting_for_input;

  DXRenderContext* m_pStartingLightContex;
};

} // namespace scd
