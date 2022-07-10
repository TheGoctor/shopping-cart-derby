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
    float radius;
  };
  // Plane (not so much a BV, but it will represent a collideable surface such
  // as a wall)
  struct plane {
    scd::vector3 normal;
    float distance;
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
} // namespace geometry
} // namespace scd

#endif // __scd_math_defs_h__
