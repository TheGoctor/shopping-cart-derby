////////////////////////////////////////////////////////////////////////////////
//	File			:	IEvent.h
//	Date			:	3/31/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Encapsulates the data for an event such as, the event ID
//						and the parameter. CEventManager will mainly use this
//						class but classes that include CEventManager will also
//						be able to use it.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

namespace scd {

class base_component;

class event {
private:
  // The ID used to identify this event
  unsigned int _event_id;

  // The component sending the event
  base_component* _sender;

  // When the event gets processed
  event_priority _priority;

  // Pointer to a structure containing additional data
  std::unique_ptr<void*> _data;

  friend class event_manager;

protected:
  void set_priority(event_priority priority) { _priority = priority; }

public:
  event(unsigned int event_id, base_component& sender,
        std::unique_ptr<void*>&& data = nullptr) {
    _event_id = event_id;
    _sender   = &sender;
    _data     = std::move(data);
  }

  // Accessors
  unsigned int event_id() const { return _event_id; }

  base_component* sender() const { return _sender; }

  void* data() const { return _data.get(); }
};

} // namespace scd
