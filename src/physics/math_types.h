#ifndef __scd_math_defs_h__
#define __scd_math_defs_h__

#include <foundation/PxQuat.h>
#include <foundation/PxTransform.h>
#include <foundation/PxVec2.h>
#include <foundation/PxVec3.h>
#include <foundation/PxVec4.h>

#include <array>

namespace scd {
typedef physx::PxQuat quaternion;
typedef physx::PxTransform transform;
typedef physx::PxVec2 vector2;
typedef physx::PxVec3 vector3;
typedef physx::PxVec4 vector4;

namespace geometry {
// structures for various bounding volumes
// triangle
struct triangle {
  std::array<scd::vector3f, 3> vertices;
  scd::vector3f tNorm;
};

// Axis-aligned Bounding Box
struct aabb {
  scd::vector3f min;
  scd::vector3f max;
};

// Oriented Bounding Box
struct box {
  scd::transform pose;
  scd::vector3f half_extents; // positive halfwidth extents along each axis
};

// Sphere BV
struct sphere {
  scd::vector3f position;
  float radius;
};
// Plane (not so much a BV, but it will represent a collideable surface such
// as a wall)
struct plane {
  scd::vector3f normal;
  float distance;
};
// Line
struct line {
  scd::vector3f start;
  scd::vector3f end;
};

struct ray {
  scd::vector3f point;
  scd::vector3f normal;
};

struct rectangle {
  int left;
  int top;
  int right;
  int bottom;
};
} // namespace geometry

namespace math {
transform matrix_identity() { return {}; }

transform matrix_multiply(const transform& left, const transform& right) {
  return left * right;
}

transform matrix_axis_angle(const vector3& axis, float angle) { return {}; }

transform matrix_scale(const transform& pose, const vector3& scale) {
  return pose * scale;
}

transform matrix_translate(const transform& pose, const vector3& translation) {
  return pose;
}
} // namespace math
} // namespace scd

#endif // __scd_math_defs_h__
