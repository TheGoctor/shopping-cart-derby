#include "race_start_manager.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/race_starter.h"
#include "core/object.h"
#include "core/object_manager.h"
#include "events/events.h"
#include "input/input_manager.h"
#include "memory/memory_manager.h"
#include "rendering/dx_render_context_manager.h"
#include "rendering/hud_manager.h"
#include "rendering/texture_manager.h"

scd::race_start_manager::race_start_manager()
  : _current_timer_value(0.0f)
  , _total_duration(3.0f)
  , _enable_input(true)
  , _bike_started(false)
  , _starting_light_context(nullptr)
  , _waiting_for_input(false)
  , _has_played_sound{false}
  , _welcome_sound{false}
  , _sound{scd::audio_manager::get()}
  , _starting_light_context{scd::dx_render_context_manager::get().get_context(RC_STARTLIGHT)}
  {
  std::string szEventName = "Update";
  szEventName += (char)GAMEPLAY_STATE;
  event_manager::get().register_event(
    szEventName, std::bind(&on_update, this);

  szEventName = "InitObjects";
  szEventName += (char)GAMEPLAY_STATE;
  event_manager::get().register_event(
    szEventName, std::bind(&on_state_init, this));

  event_manager::get().register_event(
    "GameplayEnterPressed", std::bind(&on_keypress_enter, this));

  TSpriteData tListData{};

  tListData.m_nTexture = texture_manager::get().load_texture(
    "Resource\\HUD\\HowToPlay\\T_Start_How_To_Play_D.png");
  tListData.m_fScaleX      = 1.0f;
  tListData.m_fScaleY      = 1.0f;
  tListData.m_dwColor      = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);

  // Get Inital Sprite Data
  scd::object* obj = object_manager::get().create_object("DirectionsOverlay"); // TODO: Clean this up?

  m_pDirectionsComp = texture_manager::get().create_sprite(obj, tListData, false);
}

void scd::race_start_manager::on_update(float dt) {
  if (_waiting_for_input) {
    return;
  }

  if (!_welcome_sound) {
    pSound->PlayTheSound(WELCOME_TO, GLOBAL_ID);

    for (unsigned int playernum = 0; playernum < 4; ++playernum) {
      pSound->SetRPMValueForSound(
        0, CHUDManager::GetInstance()->GetPlayerCharID(playernum));
    }

    pSound->PlayCartSounds();
    _welcome_sound = true;
  }

  // if we're in the countdown
  if (_current_timer_value < 5.0f) {
    _current_timer_value += dt;
    _enable_input = true;

    if (_current_timer_value < 4.0f) {
      // change input state to intro (aka disable input)
      SendIEvent("DisableMovement", this, nullptr, PRIORITY_NORMAL);
    }

    if (_current_timer_value > 4.0f) // 3rd light
    {
      // check/change hud element
      // play sound
      m_pStartingLightContex->SetTexIdx(5);

      if (m_bEnableInput) {
        // re-enable the input
        SendIEvent("EnableMovement", this, nullptr, PRIORITY_NORMAL);

        m_bEnableInput = false;

        SendIEvent("RaceStarted", this, nullptr, PRIORITY_NORMAL);
      }
    } else if (_current_timer_value > 3.0f) // 2nd light
    {
      m_pStartingLightContex->SetTexIdx(4);
    } else if (_current_timer_value > 2.0f) // 1st light
    {
      m_pStartingLightContex->SetTexIdx(3);
    } else if (_current_timer_value > 1.0f) // 1st light
    {
      m_pStartingLightContex->SetTexIdx(2);

      if (!_has_played_sound) {
        pSound->PlayTheSound(START_COUNTDOWN, GLOBAL_ID);
        _has_played_sound = true;
      }
    }
  }
}

void scd::race_start_manager::on_state_init() {
  m_fCurrentTimerValue = -5.0f;
  m_bEnableInput       = true;
  m_bHasPlayedSound    = false;
  m_bWelcomeSound      = false;
  m_bBikeStarted       = false;
  m_pStartingLightContex->SetTexIdx(1);
  m_pDirectionsComp->SetActive(true);
  m_bWaitingForInput = true;

  // change input state to intro (aka disable input)
  SendIEvent("DisableMovement", this, nullptr, PRIORITY_NORMAL);
}

void scd::race_start_manager::on_keypress_enter() {
  m_bWaitingForInput = false;

  // Unshow the directions overlay
  m_pDirectionsComp->SetActive(false);
  SendIEvent("OverlayUnshow", this, nullptr, PRIORITY_NORMAL);
}
