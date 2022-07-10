////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateComponent.h
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object Animate
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/base_component.h"
#include "core/containers.h"
#include "physics/math_types.h"
#include "rendering/animation.h"

#include <cstdint>

namespace scd {

class animate : public base_component {
private:
  friend class CAnimState;

  // Animation
  animation* _animation;

  using event_id     = std::uint32_t;
  using animation_id = std::uint32_t;

  using animation_map = scd::map<event_id, animation_id>;

  animation_map _animations;

  // Mesh
  dx_mesh* m_pMesh;

  // Interpolator
  interpolator _interpolator;
  interpolator _steer_interpolator;

  // Callback Handlers
  void update(float delta_time);

  // Handle to previous animation for a "default" animation
  animation* _default_animation;

  animation_id _current_animation_id;
  animation_map::iterator _current_animation_iter;
  animation* _current_animation;

  // On
  bool _is_enabled;

  // Blended Frames
  scd::vector<scd::transform> _blended_frames;
  scd::vector<VERTEX_POS3_NORM3_TEX2> _skin_verts;

  bool _is_slowed;
  bool _is_steering;
  bool _was_steering;

public:
  // Constructor
  animate(scd::object& owner, scd::dx_mesh* pMesh);
  ~animate();

  // Skinning Algo
  void skin_mesh();

  // Callbacks
  static void on_update(IEvent* pEvent, base_component* component);
  static void on_animation_switch(IEvent* pEvent, base_component* component);
  static void on_activate(IEvent* pEvent, base_component* component);
  static void on_deactivate(IEvent* pEvent, base_component* component);
  static void on_pause(IEvent* pEvent, base_component* component);

  void enable() { _is_enabled = true; }
  void disable() { _is_enabled = false; }
  bool is_enabled() const { return _is_enabled; }

  // Add to Map
  void add_animation(const std::string& event_name,
                     const std::string& pre_animation_name,
                     const std::string& loop_animation_name);

  const interpolator& interpolator() const { return _interpolator; }
};

} // namespace scd
