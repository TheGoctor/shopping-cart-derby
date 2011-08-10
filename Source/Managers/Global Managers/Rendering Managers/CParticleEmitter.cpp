////////////////////////////////////////////////////////////////////////////////
//	File			:	CParticleEmitter.h
//	Date			:	5/17/11
//	Mod. Date		:	5/17/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Particle Emitter
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CParticleEmitter.h"
#include "Direct3DManager.h"
#include "..\Camera Manager\CCameraManager.h"
#include "..\..\..\CFrame.h"

// Defines
#define ROTATION_CO (0.70710678118654752440084436210485f)

// Constructor
TParticle::TParticle(void) : m_fAge(0.0f), m_fLifeTime(0.0f),
	m_vPosition(0.0f, 0.0f, 0.0f), m_vScale(1.0f, 1.0f),
	m_vRotationLocal(0.0f, 0.0f, 0.0f), m_vRotationWorld(0.0f, 0.0f, 0.0f),
	m_vVelocity(0.0f, 0.0f, 0.0f), m_vAcceleration(0.0f, 0.0f, 0.0f),
	m_fGravityForce(0.0f), m_tColor(0.0f, 0.0f, 0.0f, 1.0f), m_fAlpha(1.0f),
	m_fRed(0.0f), m_fGreen(0.0f), m_fBlue(0.0f), m_nCurrentFrame(0)
{
	D3DXMatrixIdentity(&m_mWorldMatrix);
}

// Default Constructor
TQuad::TQuad(void)
{
	m_cVerts.resize(NUM_QUAD_VERTS);
}

// Create Quad from Particle
TQuad* TQuad::CreateQuad(TParticle* pParticle, CParticleEmitter* pEmitter)
{
	// Animation
	float fCellWidth  = 1.0f / (float)pEmitter->GetNumCols();
    float fCellHeight = 1.0f / (float)pEmitter->GetNumRows();
	float fUL = (pParticle->m_nCurrentFrame % pEmitter->GetNumCols()) * fCellWidth;
    float fUR = fUL + fCellWidth;
    float fUT = (pParticle->m_nCurrentFrame / pEmitter->GetNumCols()) * fCellHeight;
    float fUB = fUT + fCellHeight;

	// Setup Verts
	D3DXMATRIX viewMat = CCameraManager::GetInstance()->GetCam()->GetViewMatrix();
	//m_pVerts = new VERTEX_POS3_NORM3_TEX2[NUM_QUAD_VERTS];
	if(pEmitter->IsBillboarded())
	{
		// Find Right Vector
		D3DXVECTOR3 tran = D3DXVECTOR3(viewMat._11, viewMat._21, viewMat._31);
		//D3DXVec3Normalize(&tran, &tran);
		D3DXVECTOR3 rightVec = tran * pParticle->m_vScale.x;

		// Find Up Vector
		tran = D3DXVECTOR3(viewMat._12, viewMat._22, viewMat._32);
		//D3DXVec3Normalize(&tran, &tran);
		D3DXVECTOR3 upVec = tran * pParticle->m_vScale.y;

		// Use Axis to Create Quad
		m_cVerts[0].position = upVec + rightVec;
		m_cVerts[1].position = (-upVec) - rightVec;
		m_cVerts[2].position = (-rightVec) + upVec;
		m_cVerts[3].position = rightVec - upVec;
		m_cVerts[4].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		// Use Scale to Create Quad
		m_cVerts[0].position = D3DXVECTOR3(pParticle->m_vScale.x, pParticle->m_vScale.y, 0.0f);
		m_cVerts[1].position = D3DXVECTOR3(-pParticle->m_vScale.x, -pParticle->m_vScale.y, 0.0f);
		m_cVerts[2].position = D3DXVECTOR3(-pParticle->m_vScale.x, pParticle->m_vScale.y, 0.0f);
		m_cVerts[3].position = D3DXVECTOR3(pParticle->m_vScale.x, -pParticle->m_vScale.y, 0.0f);
		m_cVerts[4].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// Rotations
	D3DXMATRIX mRot;
	D3DXMatrixIdentity(&mRot);
	D3DXMATRIX mTran;
	D3DXMatrixIdentity(&mTran);

	D3DXVECTOR3 framPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	if(pEmitter->IsOnTarget() && pEmitter->GetFrame())
	{
		framPos = pEmitter->GetFrame()->GetWorldPosition();
	}

	D3DXMatrixTranslation(&pParticle->m_mWorldMatrix, pParticle->m_vPosition.x,
		pParticle->m_vPosition.y, pParticle->m_vPosition.z);

	// Find Local Rotation Matrix
	D3DXVECTOR3 radRotLoc = pParticle->m_vRotationLocal * DEG_TO_RAD;
	D3DXMatrixRotationYawPitchRoll(&mRot, radRotLoc.y, radRotLoc.x, radRotLoc.z);

	// Find World Rotation Matrix
	D3DXMATRIX mRotW;
	D3DXMatrixIdentity(&mRotW);
	D3DXVECTOR3 radRotWorld = pParticle->m_vRotationWorld * DEG_TO_RAD;
	D3DXMatrixRotationYawPitchRoll(&mRotW, radRotWorld.y, radRotWorld.x, radRotWorld.z);

	// Translate
	m_cVerts[4].position = pParticle->m_vPosition;
	for (int vert = 0; vert < NUM_QUAD_VERTS - 1; ++vert)
	{
		// Apply Local Rotation and Translation
		
		if(pEmitter->IsBillboarded() == false)
			D3DXVec3TransformCoord(&m_cVerts[vert].position, &m_cVerts[vert].position, &mRot);
		m_cVerts[vert].position += pParticle->m_vPosition + framPos;

		// Apply World Rotation
		//D3DXVec3TransformCoord(&m_cVerts[vert].position, &m_cVerts[vert].position, &mRotW);
		
		// Apply Color
		m_cVerts[vert].color = pParticle->m_tColor;
	}

	// Find X and Y Frames
	/*float fXFrame = fCellWidth  * fUL;
	float fYFrame = fCellHeight * fUT;*/

	// Apply UVs
	if( (m_cVerts[0].position.x - m_cVerts[2].position.x) > 0.0f )
	{
		if(pEmitter->GetNumFrames() > 1 || pEmitter->IsBillboarded() == false)
		{
			m_cVerts[0].uv = D3DXVECTOR2(fUR, fUT);
			m_cVerts[1].uv = D3DXVECTOR2(fUL, fUB);
			m_cVerts[2].uv = D3DXVECTOR2(fUL, fUT);
			m_cVerts[3].uv = D3DXVECTOR2(fUR, fUB);
		}
		else
		{
			/*Out.uv.x = TEXCOORDRADIUS * cos(DegreeToRadian(angle)) + .5;
			Out.uv.y = TEXCOORDRADIUS * sin(DegreeToRadian(angle)) + .5;*/

			m_cVerts[0].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (315.0f + pParticle->m_vRotationLocal.z) ) + .5f;
			m_cVerts[0].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (315.0f + pParticle->m_vRotationLocal.z) ) + .5f;

			m_cVerts[1].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (135.0f + pParticle->m_vRotationLocal.z)) + .5f;
			m_cVerts[1].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (135.0f + pParticle->m_vRotationLocal.z)) + .5f;

			m_cVerts[2].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (225.0f + pParticle->m_vRotationLocal.z)) + .5f;
			m_cVerts[2].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (225.0f + pParticle->m_vRotationLocal.z)) + .5f;

			m_cVerts[3].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (45.0f + pParticle->m_vRotationLocal.z)) + .5f;
			m_cVerts[3].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (45.0f + pParticle->m_vRotationLocal.z)) + .5f;
		}
		m_cVerts[4].uv = D3DXVECTOR2(fUR / 2, fUB / 2);
	}
	else
	{
		if(pEmitter->GetNumFrames() > 1 || pEmitter->IsBillboarded() == false)
		{
			m_cVerts[0].uv = D3DXVECTOR2(fUL, fUT);
			m_cVerts[1].uv = D3DXVECTOR2(fUR, fUB);
			m_cVerts[2].uv = D3DXVECTOR2(fUR, fUT);
			m_cVerts[3].uv = D3DXVECTOR2(fUL, fUB);
		}
		else
		{
			m_cVerts[0].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (225.0f + pParticle->m_vRotationLocal.z)) + .5f;
			m_cVerts[0].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (225.0f + pParticle->m_vRotationLocal.z)) + .5f;

			m_cVerts[1].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (45.0f + pParticle->m_vRotationLocal.z)) + .5f;
			m_cVerts[1].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (45.0f + pParticle->m_vRotationLocal.z)) + .5f;

			m_cVerts[2].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (315.0f + pParticle->m_vRotationLocal.z) ) + .5f;
			m_cVerts[2].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (315.0f + pParticle->m_vRotationLocal.z) ) + .5f;

			m_cVerts[3].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (135.0f + pParticle->m_vRotationLocal.z)) + .5f;
			m_cVerts[3].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (135.0f + pParticle->m_vRotationLocal.z)) + .5f;
		}
		m_cVerts[4].uv = D3DXVECTOR2(fUR / 2, fUB / 2);

	}

	D3DXVECTOR3 vView = D3DXVECTOR3(viewMat._31, viewMat._32, viewMat._33);
	D3DXVECTOR3 vFront = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	if(0.0f < D3DXVec3Dot(&vView, &vFront))
	{
		if( (m_cVerts[0].position.x - m_cVerts[2].position.x) < 0.0f )
		{
			if(pEmitter->GetNumFrames() > 1 || pEmitter->IsBillboarded() == false)
			{
				m_cVerts[0].uv = D3DXVECTOR2(fUR, fUT);
				m_cVerts[1].uv = D3DXVECTOR2(fUL, fUB);
				m_cVerts[2].uv = D3DXVECTOR2(fUL, fUT);
				m_cVerts[3].uv = D3DXVECTOR2(fUR, fUB);
			}
			else
			{
				m_cVerts[0].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (315.0f + pParticle->m_vRotationLocal.z )) + .5f;
				m_cVerts[0].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (315.0f + pParticle->m_vRotationLocal.z )) + .5f;

				m_cVerts[1].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (135.0f + pParticle->m_vRotationLocal.z)) + .5f;
				m_cVerts[1].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (135.0f + pParticle->m_vRotationLocal.z)) + .5f;

				m_cVerts[2].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (225.0f + pParticle->m_vRotationLocal.z)) + .5f;
				m_cVerts[2].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (225.0f + pParticle->m_vRotationLocal.z)) + .5f;

				m_cVerts[3].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (45.0f + pParticle->m_vRotationLocal.z)) + .5f;
				m_cVerts[3].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (45.0f + pParticle->m_vRotationLocal.z)) + .5f;
			}

			m_cVerts[4].uv = D3DXVECTOR2(fUR / 2, fUB / 2);
		}
		else
		{
			if(pEmitter->GetNumFrames() > 1 || pEmitter->IsBillboarded() == false)
			{
				m_cVerts[0].uv = D3DXVECTOR2(fUL, fUT);
				m_cVerts[1].uv = D3DXVECTOR2(fUR, fUB);
				m_cVerts[2].uv = D3DXVECTOR2(fUR, fUT);
				m_cVerts[3].uv = D3DXVECTOR2(fUL, fUB);
			}
			else
			{
				m_cVerts[0].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (225.0f + pParticle->m_vRotationLocal.z)) + .5f;
				m_cVerts[0].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (225.0f + pParticle->m_vRotationLocal.z)) + .5f;

				m_cVerts[1].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (45.0f + pParticle->m_vRotationLocal.z)) + .5f;
				m_cVerts[1].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (45.0f + pParticle->m_vRotationLocal.z)) + .5f;

				m_cVerts[2].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (315.0f + pParticle->m_vRotationLocal.z) ) + .5f;
				m_cVerts[2].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (315.0f + pParticle->m_vRotationLocal.z) ) + .5f;

				m_cVerts[3].uv.x = ROTATION_CO * cos(DEG_TO_RAD * (135.0f + pParticle->m_vRotationLocal.z)) + .5f;
				m_cVerts[3].uv.y = ROTATION_CO * sin(DEG_TO_RAD * (135.0f + pParticle->m_vRotationLocal.z)) + .5f;
			}

			m_cVerts[4].uv = D3DXVECTOR2(fUR / 2, fUB / 2);
		}

	}

	return this;
}

// Constructor
CParticleEmitter::CParticleEmitter(void) : DXMesh(), m_nMaxParticles(0),
	m_fMinLifeTime(0.0f), m_fMaxLifeTime(0.0f), m_eType(PE_TYPE_MIN),
	m_bContinuous(false), m_bBillboard(false), m_bOn(false),// m_bCreateBuffer(true),
	m_eSpawnAreaType(PE_SAT_RECT), m_vSpawnPosition(0.0f, 0.0f, 0.0f),
	m_vSpawnRectMin(0.0f, 0.0f, 0.0f), m_vSpawnRectMax(0.0f, 0.0f, 0.0f),
	m_vSpawnEllipseRadius(0.0f, 0.0f, 0.0f), m_vSpawnEllipseHole(0.0f, 0.0f, 0.0f),
	m_vScaleInitial(1.0f, 1.0f), m_vScaleRate(0.0f, 0.0f),
	m_vVelocityInitial(0.0f, 0.0f, 0.0f), m_vVelocityRate(0.0f, 0.0f, 0.0f),
	m_vAccelerationInitial(0.0f, 0.0f, 0.0f), m_vAccelerationRate(0.0f, 0.0f, 0.0f),
	m_vRotationLocalInitial(0.0f, 0.0f, 0.0f), m_vRotationLocalRate(0.0f, 0.0f, 0.0f),
	m_vRotationWorldInitial(0.0f, 0.0f, 0.0f), m_vRotationWorldRate(0.0f, 0.0f, 0.0f),
	m_bApplyGravity(false), m_vGravityPosition(0.0f, 0.0f, 0.0f),
	m_fGravityInitial(0.0f), m_fGravityRate(0.0f), m_bRandomColor(false),
	m_nSourceBlend(2), m_nDestBlend(2),
	m_cAlphaInitial(0), m_cRedInitial(0), m_cGreenInitial(0), m_cBlueInitial(0),
	m_fAlphaRate(0.0f), m_fRedRate(0.0f), m_fGreenRate(0.0f), m_fBlueRate(0.0f),
	m_nNumFrames(1), m_nNumRows(1), m_nNumCols(1), m_fAnimSwitchTime(0.0f), m_cParentFrame(NULL),
	m_bOnTarget(false), m_bSetFrame(false), m_nCurrFrame(0)
{
	//m_tColorInitial = D3DCOLOR_ARGB(0, 0, 0, 0);
}

// Update
void CParticleEmitter::UpdateEmitter(float fDT)
{
	/*if(m_bOn)
		return;*/

	// Keep Track of Dead Particles
	int nDeadParts = 0;

	// Update Position
	//D3DXVECTOR3 vFramePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//if(pFrame)
	//	vFramePos = pFrame->GetWorldPosition();

	// Loop through Particles
	for (int p = 0; p < m_nMaxParticles; p++)
    {
		// Get Pointer to Particle and Quad
		TParticle* pPart = &m_cParticles[p];
		TQuad*	   pQuad = &m_cQuads[p];

		// Age
		pPart->m_fAge += fDT;

		// Is it dead?
		if (pPart->m_fAge >= pPart->m_fLifeTime)
		{
			if(m_bContinuous)
			{
				// Recycle
				ResetParticle(pPart);
			}
			else
			{
				// Kill Particle
				D3DXVECTOR2 vScale = D3DXVECTOR2(0.0f, 0.0f);
				pPart->m_vScale = vScale;
				pQuad = pQuad->CreateQuad(pPart, this);

				// Update Vert Info
				UpdateVertInfo(p, pQuad);
				
				// Inc Dead Count
				++nDeadParts;
				continue;
			}
		}

		// Animation
		if(pPart->m_fAge >= (m_fAnimSwitchTime * (float)(pPart->m_nCurrentFrame + 1)) 
			&& m_bSetFrame == false)
		{
			if(pPart->m_nCurrentFrame < m_nNumFrames-1)
				++pPart->m_nCurrentFrame;
		}
		else if(m_bSetFrame == true)
		{
			pPart->m_nCurrentFrame = m_nCurrFrame;
		}

		// Apply Gravity
		if (m_bApplyGravity)
		{
			// Find Gravity Position
			D3DXVECTOR3 vGravWPos = m_vGravityPosition;
			if(!m_bOnTarget && m_cParentFrame)
			{
				vGravWPos += m_cParentFrame->GetWorldPosition();
			}

			// Find the Distance
			D3DXVECTOR3 vDist = vGravWPos - pPart->m_vPosition;
			D3DXVec3Normalize(&vDist, &vDist);

			// Gravity Force
			pPart->m_fGravityForce += (m_fGravityRate * fDT);

			// Apply Gravity
			D3DXVECTOR3 vGravForce = vDist * pPart->m_fGravityForce;
			pPart->m_vVelocity += (vGravForce * fDT);
		}

		// Acceleration
		pPart->m_vAcceleration += (m_vAccelerationRate * fDT);

		// Velocity
		pPart->m_vVelocity += (m_vVelocityRate * fDT);
        pPart->m_vVelocity += (pPart->m_vAcceleration * fDT);

		// Position
		pPart->m_vPosition += (pPart->m_vVelocity * fDT);

		// Scale
		pPart->m_vScale += (m_vScaleRate * fDT);

		// Rotation
		pPart->m_vRotationLocal += (m_vRotationLocalRate * fDT);
		pPart->m_vRotationWorld += (m_vRotationWorldRate * fDT);

		// Color
		pPart->m_fAlpha += m_fAlphaRate * fDT;
		if (pPart->m_fAlpha < 0) pPart->m_fAlpha = 0;
			else if (pPart->m_fAlpha > 255) pPart->m_fAlpha = 255;

		if(m_bRandomColor == false)
		{
			pPart->m_fRed += m_fRedRate * fDT;
			pPart->m_fGreen += m_fGreenRate * fDT;
			pPart->m_fBlue += m_fBlueRate * fDT;

			if (pPart->m_fRed < 0) pPart->m_fRed = 0;
			else if (pPart->m_fRed > 255) pPart->m_fRed = 255;

			if (pPart->m_fGreen < 0) pPart->m_fGreen = 0;
			else if (pPart->m_fGreen > 255) pPart->m_fGreen = 255;

			if (pPart->m_fBlue < 0) pPart->m_fBlue = 0;
			else if (pPart->m_fBlue > 255) pPart->m_fBlue = 255;
		}

		pPart->m_tColor = D3DCOLOR_ARGB((DWORD)pPart->m_fAlpha,
			(DWORD)pPart->m_fRed,
			(DWORD)pPart->m_fGreen,
			(DWORD)pPart->m_fBlue);

		/*pPart->m_tColor = D3DCOLOR_ARGB(127,
			(DWORD)pPart->m_fRed,
			(DWORD)pPart->m_fGreen,
			(DWORD)pPart->m_fBlue);*/

		// Quad
		pQuad = pQuad->CreateQuad(pPart, this);
	
		// Update Vert Info
		UpdateVertInfo(p, pQuad);
	}

	// Shut off Burst Effects
	if(nDeadParts == m_nMaxParticles)
	{
		// Shut Off Emitter
		m_bOn = false;
		//ResetEmitter();
	}

	// Update Mesh
	SetIndexedColoredTexturedVertexInfo(&m_tVertInfo);//, m_bCreateBuffer);
	//m_bCreateBuffer = false;
}

// Update Vert Info
void CParticleEmitter::UpdateVertInfo(int p, TQuad* pQuad)
{
	// Position
	m_tVertInfo.m_vVertices[p * NUM_QUAD_VERTS]	 = pQuad->m_cVerts[0].position;
	m_tVertInfo.m_vVertices[p * NUM_QUAD_VERTS + 1] = pQuad->m_cVerts[1].position;
	m_tVertInfo.m_vVertices[p * NUM_QUAD_VERTS + 2] = pQuad->m_cVerts[2].position;
	m_tVertInfo.m_vVertices[p * NUM_QUAD_VERTS + 3] = pQuad->m_cVerts[3].position;

	// Color
	m_tVertInfo.m_vColors[p * NUM_QUAD_VERTS]	 = pQuad->m_cVerts[0].color;
	m_tVertInfo.m_vColors[p * NUM_QUAD_VERTS + 1] = pQuad->m_cVerts[1].color; 
	m_tVertInfo.m_vColors[p * NUM_QUAD_VERTS + 2] = pQuad->m_cVerts[2].color;
	m_tVertInfo.m_vColors[p * NUM_QUAD_VERTS + 3] = pQuad->m_cVerts[3].color;

	// UV
	m_tVertInfo.m_vUV[p * NUM_QUAD_VERTS]	   = pQuad->m_cVerts[0].uv;
	m_tVertInfo.m_vUV[p * NUM_QUAD_VERTS + 1] = pQuad->m_cVerts[1].uv;
	m_tVertInfo.m_vUV[p * NUM_QUAD_VERTS + 2] = pQuad->m_cVerts[2].uv;
	m_tVertInfo.m_vUV[p * NUM_QUAD_VERTS + 3] = pQuad->m_cVerts[3].uv;

	// Indices
	m_tVertInfo.m_vIndices[p * NUM_QUAD_IDXS]	   = p * NUM_QUAD_VERTS;
	m_tVertInfo.m_vIndices[p * NUM_QUAD_IDXS + 1] = p * NUM_QUAD_VERTS + 1;
	m_tVertInfo.m_vIndices[p * NUM_QUAD_IDXS + 2] = p * NUM_QUAD_VERTS + 2;
	m_tVertInfo.m_vIndices[p * NUM_QUAD_IDXS + 3] = p * NUM_QUAD_VERTS;
	m_tVertInfo.m_vIndices[p * NUM_QUAD_IDXS + 4] = p * NUM_QUAD_VERTS + 3;
	m_tVertInfo.m_vIndices[p * NUM_QUAD_IDXS + 5] = p * NUM_QUAD_VERTS + 1;
}

// Reset Particle
void CParticleEmitter::ResetParticle(TParticle* pPart)
{
	// Position
	switch(m_eSpawnAreaType)
	{
	case PE_SAT_RECT:
		pPart->m_vPosition = SpawnInRect();
		break;
	case PE_SAT_ELLIPSE:
		pPart->m_vPosition = SpawnInEllipse();
		break;
	default:
		break;
	}

	// Life
	pPart->m_fAge = 0.0f;
	pPart->m_fLifeTime = RAND_FLOAT(m_fMinLifeTime, m_fMaxLifeTime);

	// Transforms
	pPart->m_vRotationLocal = m_vRotationLocalInitial;
	pPart->m_vRotationWorld = m_vRotationWorldInitial;
	pPart->m_vAcceleration  = m_vAccelerationInitial;
	pPart->m_vVelocity		= m_vVelocityInitial;
	pPart->m_vScale			= m_vScaleInitial;
	pPart->m_fGravityForce	= m_fGravityInitial;

	// Color
	pPart->m_fAlpha = m_cAlphaInitial;
	if(m_bRandomColor)
	{
		pPart->m_fRed   = RAND_FLOAT(0, 255);
		pPart->m_fGreen = RAND_FLOAT(0, 255);
		pPart->m_fBlue  = RAND_FLOAT(0, 255);
	}
	else
	{
		pPart->m_fRed   = m_cRedInitial;
		pPart->m_fGreen = m_cGreenInitial;
		pPart->m_fBlue  = m_cBlueInitial;
	}

	// Animation
	pPart->m_nCurrentFrame = 0;
}

// Reset Emitter
void CParticleEmitter::ResetEmitter(void)
{
	// Reset All Particles
	for (int p = 0; p < m_nMaxParticles; ++p)
    {
		TParticle* pPart = &m_cParticles[p];
		TQuad* pQuad	 = &m_cQuads[p];

		ResetParticle(pPart);
		pQuad = pQuad->CreateQuad(pPart, this);
	}
}

// Spawn Functions
D3DXVECTOR3 CParticleEmitter::SpawnInRect()
{
	D3DXVECTOR3 vWPos = m_vSpawnPosition;

	if(!m_bOnTarget && m_cParentFrame)
	{
		vWPos += m_cParentFrame->GetWorldPosition();
	}

	return GetRandomVec3(vWPos - m_vSpawnRectMin, vWPos + m_vSpawnRectMax);
}

D3DXVECTOR3 CParticleEmitter::SpawnInEllipse()
{
	float fXAngle = RAND_FLOAT(0.0f, 90.0f);
	float fZAngle = RAND_FLOAT(0.0f, 90.0f);

	D3DXVECTOR3 vWPos = m_vSpawnPosition;

	if(!m_bOnTarget && m_cParentFrame)
	{
		vWPos += m_cParentFrame->GetWorldPosition();
	}

	D3DXVECTOR3 vRange = GetRandomVec3(m_vSpawnEllipseHole, m_vSpawnEllipseRadius);

	D3DXVECTOR3 vPos = D3DXVECTOR3(vWPos.x + (vRange.x * cos(fXAngle)),
		vWPos.y + (vRange.y * sin(fXAngle)),
		vWPos.z + (vRange.z * cos(fZAngle)));

	return vPos;
}

// Random Helpers
D3DXVECTOR3 CParticleEmitter::GetRandomVec3(D3DXVECTOR3 vMin, D3DXVECTOR3 vMax)
{
	D3DXVECTOR3 vNewVec;
	vNewVec.x = RAND_FLOAT(vMin.x, vMax.x);
	vNewVec.y = RAND_FLOAT(vMin.y, vMax.y);
	vNewVec.z = RAND_FLOAT(vMin.z, vMax.z);
	return vNewVec;
}

// Assignment
CParticleEmitter& CParticleEmitter::operator=(CParticleEmitter &ref)
{
	// Mesh Name
	char cChar = 'a';
	int count = 0;
	while(cChar != '\0')
	{
		this->m_szName[count] = ref.m_szName[count];
		cChar = ref.m_szName[count];
		++count;
	}

	// Properties
	this->SetTextureName(ref.GetTextureName());
	this->SetMaxParticles(ref.GetMaxParticles());
	this->SetMinLifeTime(ref.GetMinLifeTime());
	this->SetMaxLifeTime(ref.GetMaxLifeTime());
	this->SetContinuous(ref.IsContinuous());
	this->SetBillboard(ref.IsBillboarded());

	// Spawn Values
	this->SetSpawnAreaType(ref.GetSpawnAreaType());
	this->SetSpawnPosition(ref.GetSpawnPosition());
	this->SetSpawnRectMin(ref.GetSpawnRectMin());
	this->SetSpawnRectMax(ref.GetSpawnRectMax());
	this->SetSpawnEllipseRadius(ref.GetSpawnEllipseRadius());
	this->SetSpawnEllipseHole(ref.GetSpawnEllipseHole());

	// Scale
	this->SetInitialScale(ref.GetInitialScale());
	this->SetScaleRate(ref.GetScaleRate());
	
	// Velocity
	this->SetInitialVelocity(ref.GetInitialVelocity());
	this->SetVelocityRate(ref.GetVelocityRate());

	// Acceleration
	this->SetInitialAcceleration(ref.GetInitialAcceleration());
	this->SetAccelerationRate(ref.GetAccelerationRate());

	// Rotation
	this->SetInitialRotationLocal(ref.GetInitialRotationLocal());
	this->SetRotationLocalRate(ref.GetRotationLocalRate());
	this->SetInitialRotationWorld(ref.GetInitialRotationWorld());
	this->SetRotationWorldRate(ref.GetRotationWorldRate());

	// Gravity
	this->SetApplyGravity(ref.IsGravityApplied());
	this->SetGravityPosition(ref.GetGravityPosition());
	this->SetInitialGravity(ref.GetInitialGravity());
	this->SetGravityRate(ref.GetGravityRate());

	// Color
	this->SetRandomColor(ref.IsColorRandom());
	this->SetInitialAlpha(ref.GetInitialAlpha());
	this->SetInitialRed(ref.GetInitialRed());
	this->SetInitialGreen(ref.GetInitialGreen());
	this->SetInitialBlue(ref.GetInitialBlue());
	this->SetAlphaRate(ref.GetAlphaRate());
	this->SetRedRate(ref.GetRedRate());
	this->SetGreenRate(ref.GetGreenRate());
	this->SetBlueRate(ref.GetBlueRate());

	// Blend Modes
	this->SetSourceBlend(ref.GetSourceBlendMode());
	this->SetDestinationBlend(ref.GetDestinationBlendMode());

	// Animation
	this->SetNumFrames(ref.GetNumFrames());
	this->SetNumRows(ref.GetNumRows());
	this->SetNumCols(ref.GetNumCols());
	this->SetAnimationSwitchTime(ref.GetAnimSwitchTime());

	// Update to Create Buffers
	//this->SetOn(true);
	this->UpdateEmitter(0.0f);

	return *this;
}