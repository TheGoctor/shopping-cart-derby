#pragma once

#include "core/base_component.h"
#include "core/id_generator.h"
#include "physics/math_types.h"

#include <cstdint>

namespace scd {

struct sprite_data {
  id_generator::id_t texture_id; // The Texture ID of the Sprite
  std::int32_t x;                // The X Position of the Sprite in Screen Space
  std::int32_t y;                // The Y Position of the Sprite in Screen Space
  std::int32_t z;                // The Z Position of the Sprite in Screen Space
  vector2 scale;                 // The Scale of the Sprite
  vector2 rotation_center;       // The Center point for Sprite in the X
  real rotation_angle;           // The amount of rotation applied to the sprite
  std::uint32_t color;           // The color to render the Sprite in
  geometry::rectangle rectangle; // The rectangle section of the Sprite to Draw
};

namespace component {
class sprite : public base_component {
public:
  // Constructor
  sprite(object& owner, sprite_data data);

  void draw(class texture_manager& texture_manager);

  sprite_data data() const { return _data; }
  void data(sprite_data data) { _data = data; }

  bool is_rendering() const { return _is_rendering; }
  void is_rendering(bool is_rendering) { _is_rendering = is_rendering; }

  // Sort
  bool operator<(const sprite& rhs) { return _data.z < rhs._data.z; }

private:
  sprite_data _data{};
  bool _is_rendering{false};
};
} // namespace component

} // namespace scd
