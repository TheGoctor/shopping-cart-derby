/**
 * @file event_manager.h
 *
 * @author Mac Reichelt
 *
 * @brief Handles events sent between components for the entire game.
 */

#pragma once

#include <string>

#include "enums.h"
#include "events/ievent.h"

namespace scd {
class event;
class base_component;
using event_id = std::uint32_t;

class event_manager {
public:
  using event_callback = std::function<void(const event&)>;

  event_id register_event(const std::string& event_name,
                          event_callback callback);

  void register_event(event_id id, event_callback callback);

  void unregister_event(event_id id, event_callback callback);

  void post(std::unique_ptr<event>&& event, event_priority priority);

  void process();

  void clear();

private:
  // Struct needed to correctly insert new events into the event set
  struct cmp {
    bool operator()(const std::unique_ptr<event>& lhs,
                    const std::unique_ptr<event>& rhs) {
      return lhs->_priority > rhs->_priority;
    }
  };

  scd::multimap<event_id, event_callback, heap::event> _listeners;

  scd::priority_queue<std::unique_ptr<event>, cmp, heap::event> _event_list;
};

} // namespace scd
