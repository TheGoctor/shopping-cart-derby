#include "race_starter.h"

scd::race_starter::race_starter(scd::object& owner)
  : base_component(owner) {}

void scd::race_starter::update(float dt) {
  // increment timer

  // if hud element change is needed
  // change hud element
  // play sound

  // render proper hud element

  // if timer is at point to start race
  // change input state to game

  // if timer is at total end
  // stop rendering hud stuff and clean up all that
  // remove us from the object
  // obj.removecomp(comp*)
  // MMDEL(CStartOfRaceComponent, this); // clean up my memory
}
