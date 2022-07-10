////////////////////////////////////////////////////////////////////////////////
//	File: CComponentManager.h"
//
//	Author: Mac Reichelt (MR)
//
//	Description: This class automatically manages components that don't already
//				 have a manager and cleans them up appropriately. Components
//				 are automatically added to the manager and removed when created
//				 and destroyed. The manager is automatically instantiated when
//				 the first component is created.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/base_component.h"
#include "core/containers.h"
#include "core/object.h"
#include "events/event_manager.h"

namespace scd {

template <
  typename component_t,
  typename = std::enable_if<std::is_base_of_v<scd::base_component, component_t>>>
class component_manager {
public:
  component_manager() {
    _shutdown_event_id = CEventManager::GetInstance()->RegisterEvent(
      "Shutdown", reinterpret_cast<scd::base_component*>(this), shutdown);
  }

  ~component_manager() {
    CEventManager::GetInstance()->UnregisterEvent(
      _shutdown_event_id, reinterpret_cast<scd::base_component*>(this));
    shutdown(nullptr, reinterpret_cast<scd::base_component*>(this));
  }

  static void shutdown(IEvent*, scd::base_component* pComp) {
    component_manager* pMe = (component_manager*)pComp;
    for (auto& component : pMe->_components) {
      MMDEL(component);
    }
    pMe->_components.clear();
  }

  void add(component_t* obj) { _components.push_back(obj); }

  void remove(component_t* obj) { _components.remove(obj); }
  void remove(unsigned int object_id) { remove(find(object_id)); }

  component_t* find(unsigned int owner_id) {
    for (const auto& component : _components) {
      if (component->owner()->id() == object_id)
        return component;
    }

    return nullptr;
  }
  component_t* find(const scd::object& owner) {
    for (const auto& component : _components) {
      if (component->owner() == owner)
        return component;
    }

    return nullptr;
  }

private:
  scd::list<component_t*> _components;
  unsigned int _shutdown_event_id;
};

} // namespace scd
