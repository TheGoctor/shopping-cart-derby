////////////////////////////////////////////////////////////////////////////////
//	File			:	CParticleEmitter.h
//	Date			:	5/17/11
//	Mod. Date		:	5/17/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Particle Emitter
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CPARTICLEEMITTER_H_
#define _CPARTICLEEMITTER_H_

// Includes
#include <string>
//#include <vector>
using namespace std;
#include "dxutil.h"
#include "DXMesh.h"
#include "..\Memory Manager\CDynArray.h"

// Foward Declares
class CFrame;

// Enums
enum EParticleEmitterSpawnAreaType { PE_SAT_MIN		= 0,
									 PE_SAT_RECT	= 1,
									 PE_SAT_ELLIPSE = 2,
									 PE_SAT_MAX };

enum EParticleEmitterType { PE_TYPE_MIN,					// 0
							PE_TYPE_BOOST_GLOW,				// 2
							PE_TYPE_BOOST_LIGHTNING,		// 3
							PE_TYPE_BOOST_CLOUD,			// 4
							PE_TYPE_KRACK_SMALL,			// 5
							PE_TYPE_KRACK_MID,			// 5
							PE_TYPE_KRACK_BIG,			// 5
							PE_TYPE_KRACK_POPUP,			// 5
							PE_TYPE_TURKEY_SNOWFLAKE_TRAIL_A,			// 5
							PE_TYPE_TURKEY_SNOWFLAKE_BURST_A,			// 5
							PE_TYPE_TURKEY_SNOWFLAKE_TRAIL_B,			// 5
							PE_TYPE_TURKEY_SNOWFLAKE_BURST_B,			// 5
							PE_TYPE_TURKEY_SNOWFLAKE_BURST_C,			// 5
							PE_TYPE_TURKEY_PUFF,			// 5
							PE_TYPE_BANANA_FLOOR_SPLAT,		// 6
							PE_TYPE_BANANA_SHLOOP_POPUP,	// 7
							PE_TYPE_BANANA_DRIP,		// 8
							PE_TYPE_PEANUT_SQUISH_POPUP,	// 10
							PE_TYPE_GOAL_ITEM_GLOW_GROW,	// 11
							PE_TYPE_GOAL_ITEM_GLOW_SHRINK,	// 12
							PE_TYPE_GOAL_ITEM_STARS,		// 13
							PE_TYPE_GOAL_ITEM_SHINE,		// 14
							PE_TYPE_GOAL_ITEM_ICON,			// 15

							PE_TYPE_CRASH_SMALL,		// 17
							PE_TYPE_CRASH_MID,		// 18
							PE_TYPE_CRASH_BIG,		// 19
							PE_TYPE_CRASH_POPUP,			// 20

							PE_TYPE_BOLT_BURST,
							PE_TYPE_SPARK_BURST,

							PE_TYPE_BUBBLE_TRAIL,			// 20
							PE_TYPE_SOUP_SPARKLE,

							PE_TYPE_JAM_TRAIL_UP,
							PE_TYPE_JAM_TRAIL_DOWN,
							PE_TYPE_JAM_HIT_BLOB,			// 25
							PE_TYPE_JAM_HIT_POPUP,			// 26
							PE_TYPE_JAM_BURST,

							PE_TYPE_DONUT_PUFF,				// 27
							
							PE_TYPE_PIE_TRAIL_A,		// 28
							PE_TYPE_PIE_TRAIL_B,		// 28
							PE_TYPE_PIE_SPLAT_POPUP,		// 28
							
							PE_TYPE_FIRE_WORK_TRAIL,		// 29
							PE_TYPE_FIRE_WORK_BURST,		// 29
							PE_TYPE_CONFETTI_STRIP,		// 30
							PE_TYPE_CONFETTI_STAR,		// 31

							PE_TYPE_CART_DUST,		// 32
							PE_TYPE_CART_MUD,		// 32

							PE_TYPE_CAUTION_SIGN,		// 33
							PE_TYPE_MAX };

// Defines
#define NUM_QUAD_VERTS (5)
#define NUM_QUAD_IDXS  (6)
#define DEG_TO_RAD ((float)(D3DX_PI / 180.0f))

// Foward Declares
class CParticleEmitter;

////////////////////////////////////////////////////////////////////////////////
// Structures
////////////////////////////////////////////////////////////////////////////////

// Particle
struct TParticle
{
	scd::transform	m_mWorldMatrix;
	float		m_fAge;			
	float		m_fLifeTime;
	D3DXVECTOR3	m_vPosition;
	D3DXVECTOR2 m_vScale;
	D3DXVECTOR3 m_vRotationLocal;
	D3DXVECTOR3 m_vRotationWorld;
	D3DXVECTOR3 m_vVelocity;
	D3DXVECTOR3 m_vAcceleration;
	float		m_fGravityForce;
	scd::vector4	m_tColor;
	float		m_fAlpha;
	float		m_fRed;
	float		m_fGreen;
	float		m_fBlue;
	int			m_nCurrentFrame;

	// Constructor
	TParticle(void);
};

// Quad
struct TQuad
{
	CDynArray<VERTEX_POS3_COL4_TEX2> m_cVerts;

	// Create
	TQuad* CreateQuad(TParticle* pParticle, CParticleEmitter* pEmitter);

	// Default Constructor
	TQuad(void);
};

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

// Particle Emitter
class CParticleEmitter : public DXMesh
{
private:

	// Emitter Properties
	string  m_szTextureName;
	int		m_nMaxParticles;
	float	m_fMinLifeTime;
	float	m_fMaxLifeTime;
	bool	m_bContinuous;
	bool	m_bBillboard;
	bool	m_bOn;
	CFrame* m_cParentFrame;
	bool	m_bOnTarget;
	bool	m_bSetFrame;
	int     m_nCurrFrame;
	EParticleEmitterType m_eType;

	// Spawn Values
	EParticleEmitterSpawnAreaType m_eSpawnAreaType;
	D3DXVECTOR3 				  m_vSpawnPosition;
	D3DXVECTOR3 				  m_vSpawnRectMin;
	D3DXVECTOR3 				  m_vSpawnRectMax;
	D3DXVECTOR3 				  m_vSpawnEllipseRadius;
	D3DXVECTOR3 				  m_vSpawnEllipseHole;

	// Scale
	D3DXVECTOR2	m_vScaleInitial;
	D3DXVECTOR2	m_vScaleRate;

	// Velocity
	D3DXVECTOR3 m_vVelocityInitial;
	D3DXVECTOR3 m_vVelocityRate;

	// Acceleration
	D3DXVECTOR3 m_vAccelerationInitial;
	D3DXVECTOR3 m_vAccelerationRate;

	// Rotation
	D3DXVECTOR3 m_vRotationLocalInitial;
	D3DXVECTOR3 m_vRotationLocalRate;
	D3DXVECTOR3 m_vRotationWorldInitial;
	D3DXVECTOR3 m_vRotationWorldRate;

	// Gravity
	bool		m_bApplyGravity;
	D3DXVECTOR3 m_vGravityPosition;
	float		m_fGravityInitial;
	float		m_fGravityRate;

	// Color
	bool m_bRandomColor;
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
	int   m_nNumFrames;
	int   m_nNumRows;
	int	  m_nNumCols;
	float m_fAnimSwitchTime;

	// Lists
	CDynArray<TParticle> m_cParticles;
	CDynArray<TQuad>	  m_cQuads;

public:

	// Constructor
	CParticleEmitter(void);

	// Assignment
	CParticleEmitter& operator=(CParticleEmitter &ref);

	// Update
	void UpdateEmitter(float fDT);
	void UpdateVertInfo(int p, TQuad* pQuad);

	// Reset Particles
	void ResetParticle(TParticle* pPart);
	void ResetEmitter(void);

	// Spawn Functions
	D3DXVECTOR3 SpawnInRect();
	D3DXVECTOR3 SpawnInEllipse();

	// Random Helpers
	D3DXVECTOR3 GetRandomVec3(D3DXVECTOR3 vMin, D3DXVECTOR3 vMax);

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

		// Vert Properties
		int	GetNumVerts(void)   { return m_nMaxParticles * NUM_QUAD_VERTS; }
		int	GetNumIndices(void) { return m_nMaxParticles * NUM_QUAD_IDXS;  }

		// Emitter Propteries
		string  GetTextureName(void)  { return m_szTextureName; }
		int		GetMaxParticles(void) { return m_nMaxParticles; }
		float	GetMinLifeTime(void)  { return m_fMinLifeTime;  }
		float	GetMaxLifeTime(void)  { return m_fMaxLifeTime;  }
		bool	IsContinuous(void)	  { return m_bContinuous;	}
		bool	IsBillboarded(void)	  { return m_bBillboard;	}
		bool	IsOn(void)			  { return m_bOn;			}
		bool	IsOnTarget(void)	  { return m_bOnTarget;		}
		CFrame* GetFrame(void)		  { return m_cParentFrame;  }
		EParticleEmitterType GetType(void) { return m_eType;	}

		// Spawn Type
		EParticleEmitterSpawnAreaType GetSpawnAreaType(void) 
			{ return m_eSpawnAreaType; }
		D3DXVECTOR3 GetSpawnPosition(void)		 { return m_vSpawnPosition;		 }
		D3DXVECTOR3 GetSpawnRectMin(void)		 { return m_vSpawnRectMin;		 }
		D3DXVECTOR3 GetSpawnRectMax(void)		 { return m_vSpawnRectMax;		 }
		D3DXVECTOR3 GetSpawnEllipseRadius(void)  { return m_vSpawnEllipseRadius; }
		D3DXVECTOR3 GetSpawnEllipseHole(void)	 { return m_vSpawnEllipseHole;	 }

		// Scale
		D3DXVECTOR2 GetInitialScale(void) { return m_vScaleInitial; }
		D3DXVECTOR2 GetScaleRate(void)	  { return m_vScaleRate;	}

		// Velocity
		D3DXVECTOR3 GetInitialVelocity(void) { return m_vVelocityInitial; }
		D3DXVECTOR3 GetVelocityRate(void)	 { return m_vVelocityRate;	  }

		// Acceleration
		D3DXVECTOR3 GetInitialAcceleration(void) { return m_vAccelerationInitial; }
		D3DXVECTOR3 GetAccelerationRate(void)	 { return m_vAccelerationRate;	  }

		// Rotation
		D3DXVECTOR3 GetInitialRotationLocal(void) { return m_vRotationLocalInitial; }
		D3DXVECTOR3 GetRotationLocalRate(void)	  { return m_vRotationLocalRate;	}
		D3DXVECTOR3 GetInitialRotationWorld(void) { return m_vRotationWorldInitial; }
		D3DXVECTOR3 GetRotationWorldRate(void)	  { return m_vRotationWorldRate;	}

		// Gravity
		bool		IsGravityApplied(void)	 { return m_bApplyGravity;	  }
		D3DXVECTOR3 GetGravityPosition(void) { return m_vGravityPosition; }
		float		GetInitialGravity(void)	 { return m_fGravityInitial;  }
		float		GetGravityRate(void)	 { return m_fGravityRate;	  }

		// Color
		bool	  IsColorRandom(void)	{ return m_bRandomColor;  }
		BYTE	 GetInitialAlpha(void)	{ return m_cAlphaInitial; }
		BYTE	 GetInitialRed(void)	{ return m_cRedInitial;	  }
		BYTE	 GetInitialGreen(void)	{ return m_cGreenInitial; }
		BYTE	 GetInitialBlue(void)	{ return m_cBlueInitial;  }
		float	 GetAlphaRate(void)		{ return m_fAlphaRate;    }
		float	 GetRedRate(void)		{ return m_fRedRate;      }
		float	 GetGreenRate(void)		{ return m_fGreenRate;    }
		float	 GetBlueRate(void)		{ return m_fBlueRate;     }

		// Blend Modes
		int GetSourceBlendMode(void)	  { return m_nSourceBlend; }
		int GetDestinationBlendMode(void) { return m_nDestBlend;   }

		// Animation
		int GetNumFrames(void)		  { return m_nNumFrames;	  }
		int GetNumRows(void)		  { return m_nNumRows;		  }
		int GetNumCols(void)		  { return m_nNumCols;		  }
		float GetAnimSwitchTime(void) { return m_fAnimSwitchTime; }

///////////////////////////////////////////////////////////////////////////////
// Mutators
///////////////////////////////////////////////////////////////////////////////

		// Emitter Properties
		void SetTextureName(string szTextureName) { m_szTextureName = szTextureName; }
		void SetMaxParticles(int nMaxParticles)   
		{ 
			m_nMaxParticles = nMaxParticles; 
			m_cParticles.resize(m_nMaxParticles);
			m_cQuads.resize(m_nMaxParticles);
			//m_pVertInfo = MMNEW(TMeshVertexInfo);
			m_tVertInfo.m_vVertices.resize(m_nMaxParticles * NUM_QUAD_VERTS);
			//m_tVertInfo.m_vNormals.resize(m_nMaxParticles * NUM_QUAD_VERTS);
			m_tVertInfo.m_vColors.resize(m_nMaxParticles * NUM_QUAD_VERTS);
			m_tVertInfo.m_vUV.resize(m_nMaxParticles * NUM_QUAD_VERTS);
			m_tVertInfo.m_vIndices.resize(m_nMaxParticles * NUM_QUAD_IDXS);
		}
		void SetMinLifeTime(float fMinLifeTime)	  { m_fMinLifeTime  = fMinLifeTime;  }
		void SetMaxLifeTime(float fMaxLifeTime)   { m_fMaxLifeTime  = fMaxLifeTime;  }
		void SetContinuous(bool bContinuous)	  { m_bContinuous   = bContinuous;   }
		void SetBillboard(bool bBillboard)		  { m_bBillboard    = bBillboard;	 }
		void SetOn(bool bOn)					  
		{
			if(bOn && !m_bOn)
			{
				ResetEmitter();
			}
			m_bOn = bOn;
		}
		void SetParentFrame(CFrame* pFrame)		  { m_cParentFrame  = pFrame;		 }
		void SetIsOnTarget(bool bOnTarget)		  { m_bOnTarget		= bOnTarget;	 }
		void SetType(EParticleEmitterType eType)  { m_eType			= eType;		 }

		// Spawn Type
		void SetSpawnAreaType(EParticleEmitterSpawnAreaType eSpawnAreaType) 
			{ m_eSpawnAreaType		= eSpawnAreaType;	   }
		void SetSpawnPosition(D3DXVECTOR3 vSpawnPosition) 
			{ m_vSpawnPosition		= vSpawnPosition;	   }
		void SetSpawnRectMin(D3DXVECTOR3 vSpawnRectMin) 
			{ m_vSpawnRectMin		= vSpawnRectMin;	   }
		void SetSpawnRectMax(D3DXVECTOR3 vSpawnRectMax) 
			{ m_vSpawnRectMax		= vSpawnRectMax;	   }
		void SetSpawnEllipseRadius(D3DXVECTOR3 vSpawnEllipseRadius) 
			{ m_vSpawnEllipseRadius = vSpawnEllipseRadius; }
		void SetSpawnEllipseHole(D3DXVECTOR3 vSpawnEllipseHole) 
			{ m_vSpawnEllipseHole	= vSpawnEllipseHole;   }

		// Scale
		void SetInitialScale(D3DXVECTOR2 vScaleInitial) { m_vScaleInitial = vScaleInitial; }
		void SetScaleRate(D3DXVECTOR2 vScaleRate)		{ m_vScaleRate	  = vScaleRate;	   }

		// Velocity
		void SetInitialVelocity(D3DXVECTOR3 vVelocityInitial) { m_vVelocityInitial = vVelocityInitial; }
		void SetVelocityRate(D3DXVECTOR3 vVelocityRate)		  { m_vVelocityRate	   = vVelocityRate;	   }

		// Acceleration
		void SetInitialAcceleration(D3DXVECTOR3 vAccelerationInitial) { m_vAccelerationInitial = vAccelerationInitial; }
		void SetAccelerationRate(D3DXVECTOR3 vAccelerationRate)		  { m_vAccelerationRate	   = vAccelerationRate;	   }

		// Rotation
		void SetInitialRotationLocal(D3DXVECTOR3 vRotationLocalInitial) { m_vRotationLocalInitial = vRotationLocalInitial; }
		void SetRotationLocalRate(D3DXVECTOR3 vRotationLocalRate)		{ m_vRotationLocalRate	  = vRotationLocalRate;	   }
		void SetInitialRotationWorld(D3DXVECTOR3 vRotationWorldInitial) { m_vRotationWorldInitial = vRotationWorldInitial; }
		void SetRotationWorldRate(D3DXVECTOR3 vRotationWorldRate)		{ m_vRotationWorldRate	  = vRotationWorldRate;	   }

		// Gravity
		void SetApplyGravity(bool bApplyGravity)			  { m_bApplyGravity	   = bApplyGravity;	   }
		void SetGravityPosition(D3DXVECTOR3 vGravityPosition) { m_vGravityPosition = vGravityPosition; }
		void SetInitialGravity(float fGravityInitial)		  { m_fGravityInitial  = fGravityInitial;  }
		void SetGravityRate(float fGravityRate)				  { m_fGravityRate	   = fGravityRate;	   }

		// Color
		void SetRandomColor(bool bRandomColor)		  { m_bRandomColor	= bRandomColor;	 }
		void SetInitialAlpha(BYTE cAlpha)			  { m_cAlphaInitial = cAlpha;		 }
		void SetInitialRed(BYTE cRed)				  { m_cRedInitial   = cRed;			 }
		void SetInitialGreen(BYTE cGreen)			  { m_cGreenInitial = cGreen;		 }
		void SetInitialBlue(BYTE cBlue)				  { m_cBlueInitial  = cBlue;		 }
		void SetAlphaRate(float fAlphaRate)			  { m_fAlphaRate    = fAlphaRate;	 }
		void SetRedRate(float fRedRate)				  { m_fRedRate      = fRedRate;		 }
		void SetGreenRate(float fGreenRate)			  { m_fGreenRate    = fGreenRate;	 }
		void SetBlueRate(float fBlueRate)			  { m_fBlueRate     = fBlueRate;	 }

		// Blend Modes
		void SetSourceBlend(int nSrcBlend)		 { m_nSourceBlend = nSrcBlend; }
		void SetDestinationBlend(int nDestBlend) { m_nDestBlend = nDestBlend;  }

		// Animation
		void SetNumFrames(int nNumFrames)				   { m_nNumFrames	   = nNumFrames;	  }
		void SetNumRows(int nNumRows)					   { m_nNumRows		   = nNumRows;		  }
		void SetNumCols(int nNumCols)					   { m_nNumCols		   = nNumCols;		  }
		void SetAnimationSwitchTime(float fAnimSwitchTime) { m_fAnimSwitchTime = fAnimSwitchTime; }
		void SetFrame(int nCurrFrame)
		{
			m_bSetFrame = true;
			m_nCurrFrame = nCurrFrame;
			//for (int p = 0; p < m_nMaxParticles; p++)
			//{
			//	// Get Pointer to Particle and Quad
			//	TParticle* pPart = &m_cParticles[p];
			//	pPart->m_nCurrentFrame = nCurrFrame;
			//}
		}
};
#endif // _CPARTICLEEMITTER_H_