////////////////////////////////////////////////////////////////////////////////
//	File			:	DXMesh.h
//	Date			:	5/10/11
//	Mod. Date		:	5/10/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Mesh
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CDXMESH_H_
#define _CDXMESH_H_

// Includes
#include "Direct3DManager.h"
#include "CAnimation.h"

struct TBindBone
{
	string				 m_szBoneName;
	unsigned int		 m_nBoneIdx;
	unsigned int		 m_nParentBoneIdx;
	int					 m_nNumKeyFrames;
	CFrame				 m_cFrame;
	int					 m_nNumChildren;
	vector<unsigned int> m_cChildrenIdxs;
};

struct TMeshVertexInfo
{
	vector<D3DXVECTOR3>					m_vVertices;
	vector<D3DXVECTOR3>					m_vNormals;
	vector<D3DXVECTOR2>					m_vUV;
	//vector<DWORD>						m_vIndices;
	//vector<std::vector<TBoneInfluence>> m_vWeights;
	//vector<TBone>						m_vJoints;
};	

class DXMesh
{
	IDirect3DVertexBuffer9		 * m_pVertBuffer;
	IDirect3DIndexBuffer9		 * m_pIndexBuffer;
	IDirect3DVertexDeclaration9  * m_pVertDecl;
	unsigned int			 m_iVertCount;
	unsigned int			 m_iIndexCount;
	unsigned int			 m_iVertSize;
	D3DPRIMITIVETYPE		 m_Mode;
	bool					 m_bIndexed;

	TMeshVertexInfo*		 m_pVertInfo;
	
	// Animation Data
	
	// Bone Weights
	vector<vector<TBoneInfluence>>	m_cWeights;

	// Bone Hierarchy
	vector<TBindBone>					m_cJoints;



public:
	DXMesh();
	~DXMesh();

	char					 m_szName[64];

	void CreateVertexBuffer(const void *pVertArray, 
		unsigned int iVertCount, IDirect3DVertexDeclaration9 * pVertDecl,
		unsigned int iVertSize, D3DPRIMITIVETYPE mode);

	void CreateVertexNormalTexture2DBuffer(const VERTEX_POS3_NORM3_TEX2 *pVertArray, unsigned int iVertCount);
	void CreateVertexTexture3DBuffer(const VERTEX_POS3_TEX3 *pVertArray, unsigned int iVertCount);

	void CreateIndexBuffer(const DWORD *iIndices, 
		unsigned int iIndexCount);

	IDirect3DVertexBuffer9		 * GetVertBuffer(void) const 
	{ 
		return m_pVertBuffer;  
	}
	IDirect3DIndexBuffer9		 * GetIndexBuffer(void) const 
	{ 
		return m_pIndexBuffer; 
	}
	IDirect3DVertexDeclaration9  * GetVertDecl(void) const 
	{ 
		return m_pVertDecl;	   
	}
	const unsigned int GetVertCount(void) const 
	{ 
		return m_iVertCount;  
	}
	const unsigned int GetIndexCount(void) const 
	{ 
		return m_iIndexCount; 
	}
	const unsigned int GetVertSize(void) const 
	{ 
		return m_iVertSize; 
	}
	const D3DPRIMITIVETYPE GetMode(void) const 
	{ 
		return m_Mode; 
	}
	char* GetName() 
	{
		return m_szName;
	}
	const unsigned int GetTriangleCount(void) const
	{ 
		if(true == m_bIndexed)
			return m_iIndexCount / 3;
		else
			return m_iVertCount / 3;
	}
	const bool IsIndexed(void) const 
	{ 
		return m_bIndexed; 
	}

	//string GetName(void)
	//{
	//	return m_szName;
	//}

	void SetMode(D3DPRIMITIVETYPE mode) 
	{ 
		m_Mode = mode; 
	}

	void SetWeight(int nIdx, TBoneInfluence bi)
	{
		m_cWeights[nIdx].push_back(bi);
	}


	///*void SetName(string szName) 
	//{
	//	m_szName = szName;
	//}*/

	void AddBoneToHiearchy(TBindBone tBone)
	{
		m_cJoints.push_back(tBone);
	}

	vector<TBindBone>& GetBones(void)
	{
		return m_cJoints;
	}

	vector<vector<TBoneInfluence>>& GetWeights(void)
	{
		return m_cWeights;
	}

	void SetVertexInfo(TMeshVertexInfo* pMeshInfo)
	{
		int nVertCount = pMeshInfo->m_vVertices.size();
		VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[nVertCount];

		for(int nVert = 0; nVert < nVertCount; ++nVert)
		{
			verts[nVert].position.x = pMeshInfo->m_vVertices[nVert].x;
			verts[nVert].position.y = pMeshInfo->m_vVertices[nVert].y;
			verts[nVert].position.z = pMeshInfo->m_vVertices[nVert].z;

			verts[nVert].normal = pMeshInfo->m_vNormals[nVert];

			verts[nVert].uv.x = pMeshInfo->m_vUV[nVert].x;
			verts[nVert].uv.y = pMeshInfo->m_vUV[nVert].y;
		}

		CreateVertexBuffer(verts, nVertCount, Direct3DManager::GetInstance()->GetVertNormTex2DDecl(), 
			sizeof(VERTEX_POS3_NORM3_TEX2), GetMode());

		m_pVertInfo = pMeshInfo;

		delete [] verts;
	}

	void BuildHiearchy(void);
};

#endif