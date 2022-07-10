///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCamera.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	04/07/2011
//
//  Last Changed	:	07/26/2011
//
//	Changed By		:	HN
//
//  Purpose			:	Camera class that controls the camera movement and view
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "core/containers.h"
#include "physics/collider.h"
#include "physics/math_types.h"

namespace scd {
class camera {
private:
  scd::queue<scd::transform> matrices;

  scd::transform _view_matrix;
  scd::transform _projection_matrix;
  scd::transform _view_projection_matrix;
  scd::transform _world;
  scd::vector3 _postion;
  scd::vector3 _eye_position;
  scd::vector3 _look_at_position;
  scd::object* _frustum;
  collider* _frustum_bounds;
  TSphere _camera_sphere;
  scd::vector3 _collision_transform;
  scd::transform _frame;
  scd::transform* _target_ptr;
  scd::transform _target;
  static scd::vector3 s_vWorldUp;

public:
  // Constructor
  camera(void);
  // Copy constructor
  camera(const camera&);
  // Assignment operator
  camera& operator=(const camera&);
  // Destructor
  ~camera(void);

  ///////////////////////////////////////////////
  // Accessors
  ////////////////////////////////////////////
  scd::transform& GetProjectionMatrix() { return this->_projection_matrix; }
  const scd::transform& GetViewProjectionMatrix() {
    return this->_view_projection_matrix;
  }
  scd::transform* target() const { return m_pTarget; }
  scd::transform& frame() const { return m_frame; }
  scd::transform& target1() const { return m_Target; }
  scd::vector3 GetEyePos() const { return m_vEyePos; }
  scd::vector3 GetLookAtPos() const { return m_vLookAtPos; }
  const scd::vector3& GetViewPosition();
  scd::transform& GetViewMatrix();
  /////////////////////////////////////////////////////
  // Mutators/
  ////////////////////////////////////////////////////
  void SetTarget(CFrame* pTarget) { m_pTarget = pTarget; }
  void SetFrameParent(CFrame* frame) { m_frame.SetParent(frame); }
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "BuildPerspective"
  //
  // Return: void
  //
  // Parameters: float -	the field of view value
  //			float	-	the Aspect of the camera
  //			float	-	the near plane
  //			float	-	the far plane
  //
  // Purpose: Builds the projection matrix for the camera
  //
  ////////////////////////////////////////////////////////////////////////////////
  void BuildPerspective(float fFieldOfView, float fAspect, float fZNear,
                        float fZFar);
  ////////////////////////////////////////////////////////////////////////////////
  // Function: "Update"
  //
  // Return: void
  //
  // Parameters: float - delta time
  //
  // Purpose:  updates the cameras position and does the look at
  //
  ////////////////////////////////////////////////////////////////////////////////
  void Update(float fDeltaTime);
};
} // namespace scd
