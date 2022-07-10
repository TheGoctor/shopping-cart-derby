////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateManager.h
//	Date			:	5/12/11
//	Mod. Date		:	5/12/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Factory for Animate Components
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/containers.h"

struct lua_State;

namespace scd {
class animate;
class object;
class dx_mesh;
class base_component;
class event;

// Animate Manager
class animation_manager {
public:
  // Singleton
  static animation_manager* get() {
    static animation_manager pcAnimateMan;
    return &pcAnimateMan;
  }

  // Factory
  static int create_component(lua_State* pLua);
  static animate* create_component(scd::object* owner, dx_mesh* mesh);

  // Init
  void init(void);

  // Shutdown
  static void on_shutdown(event*, scd::base_component*);
  void shutdown();

  // Disable
  static void on_disable(event*, scd::base_component*);
  void disable();

  // Get Comp By ID
  animate* get_component(unsigned int uID) {
    auto found = _components.find(uID);
    if (found != _components.end())
      return found->second;
    else
      return nullptr;
  }

private:
  animation_manager();
  ~animation_manager();

  // Map of Animate Components
  scd::map<unsigned int, animate*> _components;
};

} // namespace scd
