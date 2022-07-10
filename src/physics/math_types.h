#pragma once

#include <physx/foundation/PxQuat.h>
#include <physx/foundation/PxTransform.h>
#include <physx/foundation/PxVec2.h>
#include <physx/foundation/PxVec3.h>
#include <physx/foundation/PxVec4.h>

#include <array>

namespace scd {
typedef physx::PxQuat quaternion;
typedef physx::PxTransform transform;
typedef physx::PxVec2 vector2;
typedef physx::PxVec3 vector3;
typedef physx::PxVec4 vector4;
typedef float real;

static constexpr vector3 world_up{0, 0, 1};
static constexpr vector3 world_forward{0, 1, 0};
static constexpr vector3 world_right{1, 0, 0};

namespace geometry {
// structures for various bounding volumes
// triangle
struct triangle {
  std::array<scd::vector3, 3> vertices;
  scd::vector3 tNorm;
};

// Axis-aligned Bounding Box
struct aabb {
  scd::vector3 min;
  scd::vector3 max;
};

// Oriented Bounding Box
struct box {
  scd::transform pose;
  scd::vector3 half_extents; // positive halfwidth extents along each axis
};

// Sphere BV
struct sphere {
  scd::vector3 position;
  scd::real radius;
};
// Plane (not so much a BV, but it will represent a collideable surface such
// as a wall)
struct plane {
  scd::vector3 normal;
  scd::real distance;
};
// Line
struct line {
  scd::vector3 start;
  scd::vector3 end;
};

struct ray {
  scd::vector3 point;
  scd::vector3 normal;
};

struct rectangle {
  int left;
  int top;
  int right;
  int bottom;
};
} // namespace geometry

namespace math {
transform matrix_identity() { return transform(physx::PxIDENTITY); }

transform matrix_multiply(const transform& left, const transform& right) {
  return left * right;
}

transform matrix_axis_angle(const vector3& axis, float radians) {
  // axis must be normalized
  assert(axis.isNormalized());

  // not required, but helps make sure angle is radians
  assert(angle >= -physx::PxPi && angle <= physx::PxPi);

  return quaternion(angle, axis);
}

transform matrix_scale(const transform& pose, const vector3& scale) {
  return pose * scale;
}

transform matrix_translate(const transform& pose, const vector3& translation) {
  return pose;
}
} // namespace math
} // namespace scd
