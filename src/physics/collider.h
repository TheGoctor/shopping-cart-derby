/************************************************************************
 * Filename:			CCollideable.h
 * Mod. Date:		07/25/2011
 * Mod. Initials:	RN
 * Author:			Raymond W. Nieves
 * Purpose:			Encapsulates all collision code within a Behavior Object
 ************************************************************************/
#pragma once

#include "core/base_component.h"
#include "enums.h"
#include "physics/math_types.h"

#include <array>

namespace scd {
class scd::object;
class CBVHNode;

class collider : public scd::base_component {
public:
  // tunneling members
  bool _next_frame_tunneling; // true if the next frame may have tunneling

  scd::vector3 _prev_position;
  scd::vector3 _prev_normal; // previous position, and the normal of
                             // the previous frame's collision

  scd::object* _tunneled_object; // object we may have tunneled through

  collider(object& owner)
    : base_component(owner) {}

  collider(object& owner, int type)
    : base_component(owner)
    , _bv_type(type) {}

  ~collider() = default;

  // initialize function
  void init();

  bool is_static() const;
  unsigned int flags() const;
  CBVHNode* bvh_node() const;
  int bv_type() const;
  geometry::aabb aabb() const;
  geometry::box box() const;
  geometry::sphere sphere() const;
  geometry::plane plane() const;
  geometry::line line() const;

  bool was_checked() const;
  unsigned int obj_type() const;
  bool is_reactor() const;

  void set_static(bool bstatic);
  void set_flags(unsigned int nflag);
  void set_was_checked(bool bchecked);
  void set_bvh_node(CBVHNode* cNode);
  void set_bv_type(int type);
  void set_aabb(const geometry::aabb& taabb);
  void set_box(const geometry::box& tobb);
  void set_sphere(const geometry::sphere& tsphere);
  void set_plane(const geometry::plane& tplane);
  void set_line(const geometry::line& tLine);
  void set_obj_type(unsigned int ntype);
  void set_is_reactor(bool breact);

private:
  int _bv_type;              // type of bounding volume
  bool _was_checked = false; // if the object was checked on the current frame
  bool _is_static;           // true if the object's position is static
  bool _is_reactor; // true if the object causes a collision reaction or if
                    // objects pass through it
  unsigned int _object_type; // object type

  CBVHNode* _bvh_node; // bvh node where this object is located
};

} // namespace scd
