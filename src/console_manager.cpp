#include "console_manager.h"

#include <iostream>

#include "core/object.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "rendering/direct3d_manager.h"

// Register With Lua
#include "components/ai_manager.h"
#include "components/animation_manager.h"
#include "components/inventory_manager.h"
#include "components/level_manager.h"
#include "components/rendering/animated_donut.h"
#include "components/rendering/effects/invulnerable_effect.h"
#include "components/rendering/effects/peanut_butter_effect.h"
#include "components/rendering/scrolling_texture.h"
#include "components/rendering/shadow.h"
#include "components/rendering/skid_marks.h"
#include "core/camera_manager.h"
#include "core/object_manager.h"
#include "input/keybinding_manager.h"
#include "physics/collision_manager.h"
#include "physics/movement_manager.h"
#include "rendering/hud_manager.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "ui/button.h"
#include "ui/scroller.h"
#include "ui/slider.h"

scd::lua_manager::lua_manager()
  : _is_active(false)
  , _visible_line_count(0)
  , _max_line_count(40)
  , _current_line()
  , _console_buffer()
  , _script_root_directory("Source/Scripts/")
  , _lua(lua_open()) {
  // Initialize Lua
  luaL_openlibs(_lua);

  std::string state = "Update";
  state += CONSOLE_STATE;
  CEventManager::GetInstance()->RegisterEvent(
    "Shutdown", (IComponent*)this, Shutdown);
  CEventManager::GetInstance()->RegisterEvent(
    "InputStateChange", (IComponent*)this, ToggleConsole);
  CEventManager::GetInstance()->RegisterEvent(
    "KeyPressed", (IComponent*)this, Update);

  register_c_functions();

  load_lua_file("luatest.lua");

  lua_getglobal(_lua, "InitValues");
  lua_pcall(_lua, 0, 0, 0);
}

scd::lua_manager::~lua_manager() {
  lua_close(_lua);
  _lua = nullptr;
}

void scd::lua_manager::register_c_functions() {
  lua_register(_lua, "CreateObj", &CObjectManager::CreateObject);
  lua_register(_lua,
               "CreateMovementComponent",
               &CMovementManager::CreateMovementComponent);
  lua_register(_lua, "CreateRenderComp", &Renderer::CreateRenderComp);
  lua_register(_lua, "CreateSpriteComp", &CTextureManager::CreateSpriteComp);
  lua_register(_lua, "CreateAIComponent", &CAIManager::CreateAIComponent);
  lua_register(_lua,
               "CreateCollideableComponent",
               &CCollisionManager::CreateCollideableComponent);
  lua_register(
    _lua, "CreateButtonComponent", &CButtonComponent::CreateButtonComponent);
  lua_register(
    _lua, "CreateSliderComponent", &CSliderComponent::CreateSliderComponent);
  lua_register(_lua, "SetSliderValue", &CSliderComponent::SetSliderValue);
  lua_register(_lua,
               "CreateScrollerComponent",
               &CScrollerComponent::CreateScrollerComponent);
  lua_register(
    _lua, "SetNextButtonComponent", &CButtonComponent::SetNextButtonComponent);
  lua_register(
    _lua, "CreateInventoryComp", &CInventoryManager::CreateInventoryComp);
  lua_register(_lua, "CreateAnimComp", &CAnimateManager::CreateAnimComp);
  lua_register(_lua, "BindObjects", &CObjectManager::BindObjects);
  lua_register(_lua, "print", &scd::lua_manager::Print);

  lua_register(
    _lua, "CreateUpdateStateEvent", &EventStructs::CreateUpdateStateEvent);
  lua_register(_lua, "CreateStateEvent", &EventStructs::CreateStateEvent);
  lua_register(_lua, "CreateRenderEvent", &EventStructs::CreateRenderEvent);
  lua_register(_lua, "CreateRamEvent", &EventStructs::CreateRamEvent);
  lua_register(_lua, "CreateObjectEvent", &EventStructs::CreateObjectEvent);
  lua_register(_lua, "CreateInputEvent", &EventStructs::CreateInputEvent);
  lua_register(_lua, "CreateHeadingEvent", &EventStructs::CreateHeadingEvent);
  lua_register(_lua, "CreateGoalItemEvent", &EventStructs::CreateGoalItemEvent);
  lua_register(
    _lua, "CreateWeightClassEvent", &EventStructs::CreateWeightClassEvent);
  lua_register(_lua,
               "CreateGoalItemCollectedEvent",
               &EventStructs::CreateGoalItemCollectedEvent);
  lua_register(
    _lua, "CreateStatusEffectEvent", &EventStructs::CreateStatusEffectEvent);
  lua_register(_lua, "PushState", &CStateManager::PushState);
  lua_register(_lua, "StateChange", &CStateManager::StateChange);
  lua_register(_lua, "Back", &CStateManager::Back);
  lua_register(_lua, "SendLuaEvent", &EventStructs::SendLuaEvent);
  lua_register(_lua, "SetCartWeight", &CMovementManager::SetCartWeight);
  lua_register(_lua, "SetCharacterPicked", &CHUDManager::SetCharacterPicked);
  lua_register(
    _lua, "CreateShadowComponent", &CShadowComp::CreateShadowComponent);
  lua_register(_lua, "Collisions", &CLevelManager::ToggleCollisionVision);
  lua_register(_lua, "Geometry", &CLevelManager::ToggleGeometryVision);
  lua_register(_lua,
               "CreateInvulnerableVFXComponent",
               &CInvulnerableVFX::CreateInvulnerableVFXComponent);
  lua_register(
    _lua, "CreateSkidMarksComponent", &CSkidMarks::CreateSkidMarksComponent);
  lua_register(
    _lua, "CreatePBVFXComp", &CPeanutButterVFXComp::CreatePBVFXComponent);
  lua_register(_lua,
               "CreateScrollingUVComponent",
               &CScrollingUVComp::CreateScrollingUVComponent);

  lua_register(_lua, "SetKey", &CKeybindsManager::SetKeyCallback);
  lua_register(
    _lua, "SetDefaultKeys", &CKeybindsManager::SetBindDefaultCallback);
  lua_register(_lua, "LuaGetObjectByName", &CObjectManager::GetObjectByName);
  lua_register(_lua,
               "CreateAnimatedDonutComp",
               &CAnimatedDonutComp::CreateAnimatedDonutComponent);
  lua_register(_lua, "AttachCameraTo", &CCameraManager::AttachCamToPlayer);
  lua_register(_lua, "SetGoalTri", &CAIManager::SetGoalTri);
}

bool scd::lua_manager::load_lua_file(const std::string& file_name) {
  std::string file_path = _script_root_directory + file_name;

  if (luaL_dofile(_lua, file_path.c_str())) {
    const char* error = lua_tostring(_lua, -1);
    MessageBoxA(0, error, file_path.c_str(), MB_OK);
    lua_pop(_lua, 1);
    return false;
  }

  return true;
}

void scd::lua_manager::toggle_console() {
  if (((TStateEvent*)pEvent->GetData())->m_eToState != CONSOLE_STATE) {
    hide_console();
  } else {
    display_console();
  }
}

void scd::lua_manager::display_console() {
  _is_active = true;
}

void scd::lua_manager::hide_console() {
  _is_active = false;
}

void scd::lua_manager::draw_console() {
  if (!_is_active)
    return;

  // So the text starts at the bottom of the screen and works its way up
  int nTop     = 750 - ((_visible_line_count + 1) * 16);

  Direct3DManager::GetInstance()->GetFont()->DrawText(
    0,
    _console_buffer,
    -1,
    {0, nTop, 500, 750},
    0,
    {1.0f, 1.0f, 1.0f, 1.0f});
}

void scd::lua_manager::dump_stack() {
  int i;
  int top = lua_gettop(_lua);
  for (i = 1; i <= top; ++i) {
    int t = lua_type(_lua, i);
    switch (t) {
    case LUA_TSTRING: {
      printf("%s", lua_tostring(_lua, i));
      break;
    }
    case LUA_TBOOLEAN: {
      printf(lua_toboolean(_lua, i) ? "true" : "false");
      break;
    }
    case LUA_TNUMBER: {
      printf("%g", lua_tonumber(_lua, i));
      break;
    }
    default: {
      printf("%s", lua_typename(_lua, t));
      break;
    }
    };
    printf(" ");
  }
  printf("\n");
}

int scd::lua_manager::print(lua_State* pLua) {
  const char* message = lua_tostring(pLua, -1);
  lua_pop(pLua, 1);

  print(message);

  return 0;
}

void scd::lua_manager::print(const std::string& message) {
  _console_buffer += message;
  if (_console_buffer.last() != '\n')
    _console_buffer += '\n';

  if (++_visible_line_count > MAX_LINES) {
    _console_buffer.erase(0, _console_buffer.find('\n') + 1);
    --_visible_line_count;
  }

  std::cout << message.c_str() << std::endl;
}

void scd::lua_manager::execute(const std::string& lua_code) {
  luaL_dostring(_lua, lua_code.c_str());
}

void scd::lua_manager::update() {
  char ch = ((TConsoleEvent*)pEvent->GetData())->m_chKeyPressed;

  if (ch) {
    if (ch == '\b' && _current_line.size() > 0) {
      _current_line.erase(_current_line.end() - 1);
      _console_buffer.erase(_console_buffer.end() - 1);
      return;
    } else {
      _console_buffer += ch;
    }

    if (ch == '\n') {
      // Parse and Execute CurrLine
      execute(_current_line);

      _current_line.clear();
      _current_line;
      ++_visible_line_count;
    } else {
      _current_line += ch;
    }
  }

  if (_visible_line_count > MAX_LINES) {
    _console_buffer.erase(0, _console_buffer.find('\n') + 1);
    --_visible_line_count;
  }
}

void scd::lua_manager::parse(const std::string& command) {
  call_lua_function(command);
}
