#include "sprite.h"

#include "rendering/texture_manager.h"

scd::component::sprite::sprite(object& owner, sprite_data data)
    : base_component(owner)
    , _data(data) {}

void scd::component::sprite::draw(texture_manager& texture_manager) {
  texture_manager.draw(_data);
}
