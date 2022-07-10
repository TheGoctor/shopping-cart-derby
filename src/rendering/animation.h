////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimation.h
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates Animation Data and Functionality
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace scd {

using frame_map = scd::map<unsigned int, scd::transform, HEAPID_RENDER>;

struct key_frame {
  float _key_time;       // Time of Frame
  scd::transform _frame; // The Position of the Bone in the Frame

  key_frame();
};

using bone_id = std::uint32_t;

struct bone {
  std::string name;                  // The Name of the Bone
  bone_id id        = 0;             // The Index of the Bone
  bone_id parent_id = 0;             // The Parent's Index
  scd::vector<key_frame> key_frames; // The Key Frames that apply to the Bone
  scd::vector<bone_id> child_ids;    // The Indices of the Children

  bone() = default;
  ~bone();
};

// Bone Influence
struct bone_influence {
  bone_id bone_id = 0;    // The Index of the Bone
  float weight    = 0.0f; // The Amount of Influence applied from the Bone
};

class animation {
public:
  animation();
  ~animation();

  void add_bone(const bone& bone);

  const scd::vector<bone>& bones() const { return _bones; }
  std::size_t bone_count() const { return _bones.size(); }

  float duration() const { return _duration; }
  void set_duration(float duration) { _duration = duration; }

  bool is_looping() const { return _is_looping; }
  void set_looping(bool loop) { _is_looping = loop; }

private:
  // Let Interpolator Access Private Data
  friend class Interpolator;

  float _duration;          // The Time it takes to go through the Animation
  scd::vector<bone> _bones; // List of Bones

  bool _is_looping;
};

class interpolator {
  friend class CAnimationBlender;

  animation* _animation;  // The Current Animation
  dx_mesh* _mesh;         // The Mesh to Apply Skinning To (Must be a Copy)
  frame_map _bone_frames; // List of Bone Frames
  float _current_time;    // The Current Time into the Animation

  // Interpolation used for Skinning
  scd::transform interpolate(const scd::transform& a, const scd::transform& b,
                             float lamda);

  // NON-FUNCTIONAL
  scd::transform interpolate_with_scale(const scd::transform& a,
                                        const scd::transform& b, float lamda);

public:
  // Constructor
  interpolator();

  // Destructor
  ~interpolator();

  // Connect the Frames
  void build_heirarchy();

  // Process and Interpolate
  void process();

  // Accessors
  animation* animation() const { return _animation; }
  const frame_map& current_bone_frames() const { return _bone_frames; }

  void set_animation(animation* anim);
  void set_mesh(dx_mesh* mesh);

  void add_time(float seconds) { _current_time += seconds; }
  void set_time(float seconds) { _current_time = seconds; }
};

} // namespace scd
