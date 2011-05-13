////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateComponent.cpp
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object Animate
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CAnimateComponent.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\DXMesh.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\ModelManager.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
#include "..\\..\\CObject.h"
using namespace EventStructs;

// Constructor
CAnimateComponent::CAnimateComponent(CObject* pParent, DXMesh* pMesh, CAnimation* pAnimation,
									 Interpolator cInterpolator)
	: m_pcParent(pParent), m_pMesh(pMesh), m_pAnimation(pAnimation), m_cInterpolator(cInterpolator)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();

	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);
}

// Handlers
void CAnimateComponent::Update(float fDT)
{
	// Interpolate Between Frame
	m_cInterpolator.AddTime(fDT);
	m_cInterpolator.Process();

	// Skin the Mesh
	SkinMesh();
}

// Callbacks
void CAnimateComponent::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component
	CAnimateComponent* pAC = (CAnimateComponent*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Call Handler
	pAC->Update(fDT);
}

void CAnimateComponent::SkinMesh(void)
{
	TMeshVertexInfo* vertInfo = ModelManager::GetInstance()->GetCloneVerts(m_pMesh->GetName());

	vector<TBindBone>& pBindBones = m_pMesh->GetBones();


	vector<CFrame>& pCurrFrames = m_cInterpolator.GetCurrentBoneFrames();

	vector<vector<TBoneInfluence>>& pWeights = m_pMesh->GetWeights();

	IDirect3DVertexBuffer9* pBuffer = m_pMesh->GetVertBuffer();

	VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[m_pMesh->GetVertCount()];

	//pBuffer->Lock(0, sizeof(VERTEX_POS3_NORM3_TEX2) * m_pMesh->GetVertCount(), (void**)&verts, 0);

	for(unsigned int nVert = 0; nVert < m_pMesh->GetVertCount(); ++nVert)
	{
		D3DXVECTOR3 vFinalPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 bindVert = vertInfo->m_vVertices[nVert];

		int nNumIf = pWeights[nVert].size();
		int nBoneIdx = 0;
		for(unsigned int nInf = 0; nInf < nNumIf; ++nInf)
		{
			nBoneIdx = pWeights[nVert][nInf].m_nBoneIndex;
			D3DXMATRIX bindMat = pBindBones[nBoneIdx].m_cFrame.GetWorldMatrix();
			D3DXMatrixInverse(&bindMat, NULL, &bindMat);

			D3DXMATRIX interMat = m_cInterpolator.GetCurrentBoneFrames()[nBoneIdx].GetWorldMatrix();

			D3DXVECTOR3 vNewPos;
			D3DXVec3TransformCoord(&vNewPos, &bindVert, &bindMat);
			D3DXVec3TransformCoord(&vNewPos, &bindVert, &interMat);

			vNewPos *= pWeights[nVert][nInf].m_fWeight;

			vFinalPos += vNewPos;
		}

		verts[nVert].position = vFinalPos;
		//memcpy(&verts[nVert], &vFinalPos, sizeof(D3DXVECTOR3));
	}

	m_pMesh->CreateVertexBuffer(verts, m_pMesh->GetVertCount(), Direct3DManager::GetInstance()->GetVertNormTex2DDecl(), 
			sizeof(VERTEX_POS3_NORM3_TEX2), m_pMesh->GetMode());

	//pBuffer->Unlock();
}