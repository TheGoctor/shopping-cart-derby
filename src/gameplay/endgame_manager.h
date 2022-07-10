////////////////////////////////////////////////////////////////////////////////
//	File			:	CEndgameManager.h
//	Date			:	6/17/11
//	Mod. Date		:	6/17/11
//	Mod. Initials	:	JL, HN, MS
//	Author			:
//	Purpose			:	Encapsulates the Endgame Senarios
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>

// Includes

namespace scd {

class CSpriteComponent;
class IComponent;
class IEvent;
class CObject;

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
  // mainly for when character unlock message pops up
  std::shared_ptr<class bitmap_font> _font;

  std::array<std::int32_t, 2> _font_position;

  // for winning the game only once per play
  bool _game_won;

  // Flags and Timers
  bool m_bExitingGameplay; // For Fading
  bool m_bHumanWon;        // For Human player WINNING
  bool m_bSoundPlayed;
  bool m_bLoseFade;
  bool m_bCameraMoved;
  bool m_bVictoryPlayed;
  float m_fCameraTime;
  float m_fFadeScreenAlpha;
  float m_fWinLoseAlpha;
  float m_fLoseFadeTimer;
  float m_fCameraMoveTimer;

  // Sprite Components
  CSpriteComponent* m_pFadeScreenComp;
  CSpriteComponent* m_pWinLoseComp;
  CSpriteComponent* m_pMainMenuComp;

  CSpriteComponent* m_pFinishFlag;

  CObject* m_pWinnerObject;
  CObject* m_pLoserObject1;
  CObject* m_pLoserObject2;
  CObject* m_pLoserObject3;
  CObject* m_pPlayerObject;

  // Helper funcs
  void ResetSprites(void);
};

} // namespace scd
