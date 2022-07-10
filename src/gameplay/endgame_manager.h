#pragma once

#include "core/containers.h"

#include <array>
#include <memory>

#include <cstdint>

namespace scd {

class endgame_manager {
public:
  endgame_manager();
  ~endgame_manager();

  // Callbacks
  void on_gameplay_update();
  void on_gameplay_init();

  void on_game_win();

  void on_win_init();
  void on_win_update();
  void on_win_state_exit();

  void on_enter_pressed();

private:
  static constexpr float fade_alpha_rate{ 0.5f };
  static constexpr float lose_fade_time{ 3.0f };

  // mainly for when character unlock message pops up
  std::shared_ptr<class bitmap_font> _font{ nullptr };

  std::array<std::int32_t, 2> _font_position{0, 0};

  // for winning the game only once per play
  bool _game_won = false;

  // Flags and Timers
  bool m_bExitingGameplay  = false; // For Fading
  bool m_bHumanWon         = false; // For Human player WINNING
  bool m_bSoundPlayed      = false;
  bool m_bLoseFade         = false;
  bool m_bCameraMoved      = false;
  bool m_bVictoryPlayed    = false;
  float m_fCameraTime      = 0.0f;
  float m_fFadeScreenAlpha = 0.0f;
  float m_fWinLoseAlpha    = 0.0f;
  float m_fLoseFadeTimer   = 0.0f;
  float m_fCameraMoveTimer = 0.0f;

  // Sprite Components
  std::shared_ptr<component::sprite> _fade_screen = nullptr;
  std::shared_ptr<component::sprite> _win_lose    = nullptr;
  std::shared_ptr<component::sprite> _main_menu   = nullptr;
  std::shared_ptr<component::sprite> _finish_flag = nullptr;

  scd::vector<std::shared_ptr<scd::object>> _player_rankings;

  // Helper funcs
  void reset_sprites();
};

} // namespace scd
