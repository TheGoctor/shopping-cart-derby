#pragma once

#include "core/base_component.h"

namespace scd {
class race_starter : public scd::base_component
{
public:
    race_starter(scd::object& owner);

    void update(float dt);
};

} // namespace scd
