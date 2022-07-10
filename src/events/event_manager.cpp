#include "event_manager.h"

#include "core/base_component.h"
#include "core/id_generator.h"
#include "core/object.h"
#include "events/ievent.h"

#include <assert.h>

namespace scd {

event_id event_manager::register_event(const std::string& event_name,
                                       event_callback callback) {
  // Convert string to int
  event_id id = id_generator::get().get_id(event_name);

  // Register the callback
  register_event(id, callback);

  return id;
}

void event_manager::register_event(event_id id, event_callback callback) {
  _listeners.emplace(id, callback);
}

void event_manager::unregister_event(event_id id, event_callback callback) {
  auto range = _listeners.equal_range(id);

  for (auto iter = range.first; iter != range.second; ++iter) {
    if (iter->second == callback) {
      iter = _listeners.erase(iter);
      break;
    }
  }
}

void event_manager::post(std::unique_ptr<event>&& event,
                         event_priority priority) {
  if (priority != event_priority::immediate) {
    event->set_priority(priority);
    _event_list.push(event);
  } else {
    auto range = _listeners.equal_range(event->id());

    for (auto iter = range.first; iter != range.second; ++iter) {
      iter->second(*event);
    }

    event.reset();
  }
}

void event_manager::clear() {
  _event_list.clear();
}

void event_manager::process() {
  auto events = std::move(_event_list);

  while (!events.empty()) {
    const auto& top = events.top();

    auto range = _listeners.equal_range(top->id());

    for (auto iter = range.first; iter != range.second; ++iter) {
      iter->second(*event);
    }

    events.pop();
  }
}

} // namespace scd
