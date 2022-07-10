#include "id_generator.h"

scd::id_generator::id_t scd::id_generator::get_id(const std::string& name) {
  // See if the name is already defined
  auto found = _registry.find(name);

  // Return ID if defined
  if (found != _registry.end()) {
    return found->second;
  }

  // Add string to register if not defined
  _registry.emplace(name, 0);
  id_t id = static_cast<id_t>(_registry.find(name)->first.c_str());
  _registry[name] = id;

  // Return address of string in map as ID (so that you may cast the ID as a
  // char* to see the string!)
  return id;
}
