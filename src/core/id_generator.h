#pragma once

#include "core/containers.h"

#include <string>

#include <cstdint>

namespace scd {
class id_generator {
public:
  using id_t = std::size_t;

  /**
   * @brief Get a unique id for the given string.
   *
   * @param str The string to convert into an ID.
   * @return id_t A unique id for the string.
   */
  id_t get_id(const std::string& str);

private:
  scd::map<std::string, id_t> _registry;
};

} // namespace scd
