////////////////////////////////////////////////////////////////////////////////
//	File			:	CParticleEmitter.h
//	Date			:	5/17/11
//	Mod. Date		:	5/17/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a
// Particle Emitter
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/containers.h"
#include "rendering/dx_mesh.h"
#include "rendering/dx_util.h"

#include <array>
#include <string>

// Defines
#define NUM_QUAD_VERTS (5)
#define NUM_QUAD_IDXS (6)
#define DEG_TO_RAD ((float)(D3DX_PI / 180.0f))

namespace scd {

struct particle {
  scd::transform m_mWorldMatrix;
  float m_fAge;
  float m_fLifeTime;
  scd::vector3 m_vPosition;
  D3DXVECTOR2 m_vScale;
  scd::vector3 m_vRotationLocal;
  scd::vector3 m_vRotationWorld;
  scd::vector3 m_vVelocity;
  scd::vector3 m_vAcceleration;
  float m_fGravityForce;
  scd::vector4 m_tColor;
  float m_fAlpha;
  float m_fRed;
  float m_fGreen;
  float m_fBlue;
  int m_nCurrentFrame;

  particle();
};

// Quad
struct quad {
  std::array<VERTEX_POS3_COL4_TEX2, 4> _vertices;

  static std::shared_ptr<quad>
  create(particle& particle, particle_emitter& emitter);

  quad();
};

class particle_emitter : public DXMesh {
public:
  enum class spawn_area_type { rectangle, ellipse };

  enum emitter_type {
    boost_glow,
    boost_lightning,
    boost_cloud,
    krack_small,
    krack_mid,
    krack_big,
    krack_popup,
    turkey_snowflake_trail_a,
    turkey_snowflake_burst_a,
    turkey_snowflake_trail_b,
    turkey_snowflake_burst_b,
    turkey_snowflake_burst_c,
    turkey_puff,
    banana_floor_splat,
    banana_shloop_popup,
    banana_drip,
    peanut_squish_popup,
    goal_item_glow_grow,
    goal_item_glow_shrink,
    goal_item_stars,
    goal_item_shine,
    goal_item_icon,

    crash_small,
    crash_mid,
    crash_big,
    crash_popup,

    bolt_burst,
    spark_burst,

    bubble_trail,
    soup_sparkle,

    jam_trail_up,
    jam_trail_down,
    jam_hit_blob,
    jam_hit_popup,
    jam_burst,

    donut_puff,

    pie_trail_a,
    pie_trail_b,
    pie_splat_popup,

    fire_work_trail,
    fire_work_burst,
    confetti_strip,
    confetti_star,

    cart_dust,
    cart_mud,

    caution_sign,
    max
  };

private:
  std::string _texture_name;
  std::size_t _max_particles;
  float _min_lifetime;
  float _max_lifetime;
  bool _is_continuous;
  bool _is_billboard;
  bool _is_enabled;
  CFrame* m_cParentFrame;
  bool _is_on_target;
  bool _is_frame_set;
  int _current_frame;
  emitter_type _type;

  // Spawn Values
  spawn_area_type _spawn_area_type;
  scd::vector3 _spawn_position;
  scd::vector3 _spawn_rectangle_min;
  scd::vector3 _spawn_rectangle_max;
  scd::vector3 _spawn_ellipse_radius;
  scd::vector3 _spawn_ellipse_hole;

  // Scale
  scd::vector2 _initial_scale;
  scd::vector2 _scale_rate;

  // Velocity
  scd::vector3 _initial_velocity;
  scd::vector3 _velocity_rate;

  // Acceleration
  scd::vector3 _initial_acceleration;
  scd::vector3 _acceleration_rate;

  // Rotation
  scd::vector3 _initial_local_rotation;
  scd::vector3 _local_rotation_rate;
  scd::vector3 _initial_world_rotation;
  scd::vector3 _world_rotation_rate;

  // Gravity
  bool _apply_gravity;
  scd::vector3 _gravity_position;
  float _initial_gravity;
  float _gravity_rate;

  // Color
  bool _random_color;
  BYTE m_cAlphaInitial;
  BYTE m_cRedInitial;
  BYTE m_cGreenInitial;
  BYTE m_cBlueInitial;
  float m_fAlphaRate;
  float m_fRedRate;
  float m_fGreenRate;
  float m_fBlueRate;

  // Blend Modes
  int m_nSourceBlend;
  int m_nDestBlend;

  // Animation
  int m_nNumFrames;
  int m_nNumRows;
  int m_nNumCols;
  float m_fAnimSwitchTime;

  scd::vector<particle> _particles;
  scd::vector<quad> _quads;

public:
  // Constructor
  particle_emitter();

  // Assignment
  CParticleEmitter& operator=(CParticleEmitter& ref);

  // Update
  void UpdateEmitter(float fDT);
  void UpdateVertInfo(int p, quad* pQuad);

  // Reset Particles
  void Reseparticle(particle* pPart);
  void ResetEmitter(void);

  // Spawn Functions
  scd::vector3 SpawnInRect();
  scd::vector3 SpawnInEllipse();

  // Random Helpers
  scd::vector3 GetRandomVec3(scd::vector3 vMin, scd::vector3 vMax);

  ///////////////////////////////////////////////////////////////////////////////
  // Accessors
  ///////////////////////////////////////////////////////////////////////////////

  // Vert Properties
  int GetNumVerts(void) { return m_nMaxParticles * NUM_QUAD_VERTS; }
  int GetNumIndices(void) { return m_nMaxParticles * NUM_QUAD_IDXS; }

  // Emitter Propteries
  string GetTextureName(void) { return m_szTextureName; }
  int GetMaxParticles(void) { return m_nMaxParticles; }
  float GetMinLifeTime(void) { return m_fMinLifeTime; }
  float GetMaxLifeTime(void) { return m_fMaxLifeTime; }
  bool IsContinuous(void) { return m_bContinuous; }
  bool IsBillboarded(void) { return m_bBillboard; }
  bool IsOn(void) { return m_bOn; }
  bool IsOnTarget(void) { return m_bOnTarget; }
  CFrame* GetFrame(void) { return m_cParentFrame; }
  EParticleEmitterType GetType(void) { return m_eType; }

  // Spawn Type
  EParticleEmitterSpawnAreaType GetSpawnAreaType(void) {
    return m_eSpawnAreaType;
  }
  scd::vector3 GetSpawnPosition(void) { return m_vSpawnPosition; }
  scd::vector3 GetSpawnRectMin(void) { return m_vSpawnRectMin; }
  scd::vector3 GetSpawnRectMax(void) { return m_vSpawnRectMax; }
  scd::vector3 GetSpawnEllipseRadius(void) { return m_vSpawnEllipseRadius; }
  scd::vector3 GetSpawnEllipseHole(void) { return m_vSpawnEllipseHole; }

  // Scale
  D3DXVECTOR2 GetInitialScale(void) { return m_vScaleInitial; }
  D3DXVECTOR2 GetScaleRate(void) { return m_vScaleRate; }

  // Velocity
  scd::vector3 GetInitialVelocity(void) { return m_vVelocityInitial; }
  scd::vector3 GetVelocityRate(void) { return m_vVelocityRate; }

  // Acceleration
  scd::vector3 GetInitialAcceleration(void) { return m_vAccelerationInitial; }
  scd::vector3 GetAccelerationRate(void) { return m_vAccelerationRate; }

  // Rotation
  scd::vector3 GetInitialRotationLocal(void) { return m_vRotationLocalInitial; }
  scd::vector3 GetRotationLocalRate(void) { return m_vRotationLocalRate; }
  scd::vector3 GetInitialRotationWorld(void) { return m_vRotationWorldInitial; }
  scd::vector3 GetRotationWorldRate(void) { return m_vRotationWorldRate; }

  // Gravity
  bool IsGravityApplied(void) { return m_bApplyGravity; }
  scd::vector3 GetGravityPosition(void) { return m_vGravityPosition; }
  float GetInitialGravity(void) { return m_fGravityInitial; }
  float GetGravityRate(void) { return m_fGravityRate; }

  // Color
  bool IsColorRandom(void) { return m_bRandomColor; }
  BYTE GetInitialAlpha(void) { return m_cAlphaInitial; }
  BYTE GetInitialRed(void) { return m_cRedInitial; }
  BYTE GetInitialGreen(void) { return m_cGreenInitial; }
  BYTE GetInitialBlue(void) { return m_cBlueInitial; }
  float GetAlphaRate(void) { return m_fAlphaRate; }
  float GetRedRate(void) { return m_fRedRate; }
  float GetGreenRate(void) { return m_fGreenRate; }
  float GetBlueRate(void) { return m_fBlueRate; }

  // Blend Modes
  int GetSourceBlendMode(void) { return m_nSourceBlend; }
  int GetDestinationBlendMode(void) { return m_nDestBlend; }

  // Animation
  int GetNumFrames(void) { return m_nNumFrames; }
  int GetNumRows(void) { return m_nNumRows; }
  int GetNumCols(void) { return m_nNumCols; }
  float GetAnimSwitchTime(void) { return m_fAnimSwitchTime; }

  ///////////////////////////////////////////////////////////////////////////////
  // Mutators
  ///////////////////////////////////////////////////////////////////////////////

  // Emitter Properties
  void SetTextureName(string szTextureName) { m_szTextureName = szTextureName; }
  void SetMaxParticles(int nMaxParticles) {
    m_nMaxParticles = nMaxParticles;
    m_cParticles.resize(m_nMaxParticles);
    m_cQuads.resize(m_nMaxParticles);
    // m_pVertInfo = MMNEW(TMeshVertexInfo);
    m_tVertInfo.m_vVertices.resize(m_nMaxParticles * NUM_QUAD_VERTS);
    // m_tVertInfo.m_vNormals.resize(m_nMaxParticles * NUM_QUAD_VERTS);
    m_tVertInfo.m_vColors.resize(m_nMaxParticles * NUM_QUAD_VERTS);
    m_tVertInfo.m_vUV.resize(m_nMaxParticles * NUM_QUAD_VERTS);
    m_tVertInfo.m_vIndices.resize(m_nMaxParticles * NUM_QUAD_IDXS);
  }
  void SetMinLifeTime(float fMinLifeTime) { m_fMinLifeTime = fMinLifeTime; }
  void SetMaxLifeTime(float fMaxLifeTime) { m_fMaxLifeTime = fMaxLifeTime; }
  void SetContinuous(bool bContinuous) { m_bContinuous = bContinuous; }
  void SetBillboard(bool bBillboard) { m_bBillboard = bBillboard; }
  void SetOn(bool bOn) {
    if (bOn && !m_bOn) {
      ResetEmitter();
    }
    m_bOn = bOn;
  }
  void SetParentFrame(CFrame* pFrame) { m_cParentFrame = pFrame; }
  void SetIsOnTarget(bool bOnTarget) { m_bOnTarget = bOnTarget; }
  void SetType(EParticleEmitterType eType) { m_eType = eType; }

  // Spawn Type
  void SetSpawnAreaType(EParticleEmitterSpawnAreaType eSpawnAreaType) {
    m_eSpawnAreaType = eSpawnAreaType;
  }
  void SetSpawnPosition(scd::vector3 vSpawnPosition) {
    m_vSpawnPosition = vSpawnPosition;
  }
  void SetSpawnRectMin(scd::vector3 vSpawnRectMin) {
    m_vSpawnRectMin = vSpawnRectMin;
  }
  void SetSpawnRectMax(scd::vector3 vSpawnRectMax) {
    m_vSpawnRectMax = vSpawnRectMax;
  }
  void SetSpawnEllipseRadius(scd::vector3 vSpawnEllipseRadius) {
    m_vSpawnEllipseRadius = vSpawnEllipseRadius;
  }
  void SetSpawnEllipseHole(scd::vector3 vSpawnEllipseHole) {
    m_vSpawnEllipseHole = vSpawnEllipseHole;
  }

  // Scale
  void SetInitialScale(D3DXVECTOR2 vScaleInitial) {
    m_vScaleInitial = vScaleInitial;
  }
  void SetScaleRate(D3DXVECTOR2 vScaleRate) { m_vScaleRate = vScaleRate; }

  // Velocity
  void SetInitialVelocity(scd::vector3 vVelocityInitial) {
    m_vVelocityInitial = vVelocityInitial;
  }
  void SetVelocityRate(scd::vector3 vVelocityRate) {
    m_vVelocityRate = vVelocityRate;
  }

  // Acceleration
  void SetInitialAcceleration(scd::vector3 vAccelerationInitial) {
    m_vAccelerationInitial = vAccelerationInitial;
  }
  void SetAccelerationRate(scd::vector3 vAccelerationRate) {
    m_vAccelerationRate = vAccelerationRate;
  }

  // Rotation
  void SetInitialRotationLocal(scd::vector3 vRotationLocalInitial) {
    m_vRotationLocalInitial = vRotationLocalInitial;
  }
  void SetRotationLocalRate(scd::vector3 vRotationLocalRate) {
    m_vRotationLocalRate = vRotationLocalRate;
  }
  void SetInitialRotationWorld(scd::vector3 vRotationWorldInitial) {
    m_vRotationWorldInitial = vRotationWorldInitial;
  }
  void SetRotationWorldRate(scd::vector3 vRotationWorldRate) {
    m_vRotationWorldRate = vRotationWorldRate;
  }

  // Gravity
  void SetApplyGravity(bool bApplyGravity) { m_bApplyGravity = bApplyGravity; }
  void SetGravityPosition(scd::vector3 vGravityPosition) {
    m_vGravityPosition = vGravityPosition;
  }
  void SetInitialGravity(float fGravityInitial) {
    m_fGravityInitial = fGravityInitial;
  }
  void SetGravityRate(float fGravityRate) { m_fGravityRate = fGravityRate; }

  // Color
  void SetRandomColor(bool bRandomColor) { m_bRandomColor = bRandomColor; }
  void SetInitialAlpha(BYTE cAlpha) { m_cAlphaInitial = cAlpha; }
  void SetInitialRed(BYTE cRed) { m_cRedInitial = cRed; }
  void SetInitialGreen(BYTE cGreen) { m_cGreenInitial = cGreen; }
  void SetInitialBlue(BYTE cBlue) { m_cBlueInitial = cBlue; }
  void SetAlphaRate(float fAlphaRate) { m_fAlphaRate = fAlphaRate; }
  void SetRedRate(float fRedRate) { m_fRedRate = fRedRate; }
  void SetGreenRate(float fGreenRate) { m_fGreenRate = fGreenRate; }
  void SetBlueRate(float fBlueRate) { m_fBlueRate = fBlueRate; }

  // Blend Modes
  void SetSourceBlend(int nSrcBlend) { m_nSourceBlend = nSrcBlend; }
  void SetDestinationBlend(int nDestBlend) { m_nDestBlend = nDestBlend; }

  // Animation
  void SetNumFrames(int nNumFrames) { m_nNumFrames = nNumFrames; }
  void SetNumRows(int nNumRows) { m_nNumRows = nNumRows; }
  void SetNumCols(int nNumCols) { m_nNumCols = nNumCols; }
  void SetAnimationSwitchTime(float fAnimSwitchTime) {
    m_fAnimSwitchTime = fAnimSwitchTime;
  }
  void SetFrame(int nCurrFrame) {
    m_bSetFrame = true;
    m_nCurrFrame = nCurrFrame;
    // for (int p = 0; p < m_nMaxParticles; p++)
    //{
    //	// Get Pointer to Particle and Quad
    //	particle* pPart = &m_cParticles[p];
    //	pPart->m_nCurrentFrame = nCurrFrame;
    //}
  }
};

} // namespace scd
