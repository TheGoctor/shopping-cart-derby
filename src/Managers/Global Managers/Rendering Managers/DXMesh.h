////////////////////////////////////////////////////////////////////////////////
//	File			:	DXMesh.h
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Mesh,
//						Acts as Base Class for other 3D Rendersbles
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _DXMESH_H_
#define _DXMESH_H_

// Includes
#include <vector>
using std::vector;
#include "Direct3DManager.h"
#include "CAnimation.h"
#include "..\Memory Manager\CDynArray.h"

// BonedMesh Data
struct TBindBone
{
	string				 m_szBoneName;
	unsigned int		 m_nBoneIdx;
	unsigned int		 m_nParentBoneIdx;
	CFrame				 m_cFrame;
	int					 m_nNumChildren;
	CDynArray<unsigned int> m_cChildrenIdxs;

	TBindBone() : m_nBoneIdx(0), m_nParentBoneIdx(0), m_nNumChildren(0) {}
};

// Vertex Info
struct TMeshVertexInfo
{
	//vector<D3DXVECTOR3, CRenderAllocator<D3DXVECTOR3>>;
	CDynArray<D3DXVECTOR3> m_vVertices;
	//vector<D3DXVECTOR3, CRenderAllocator<D3DXVECTOR3>>;
	CDynArray<D3DXVECTOR3> m_vNormals;
	//vector<D3DCOLOR, CRenderAllocator<D3DCOLOR>>;
	CDynArray<D3DCOLOR> m_vColors;
	//vector<D3DXVECTOR2, CRenderAllocator<D3DXVECTOR2>>;
	CDynArray<D3DXVECTOR2> m_vUV;
	//vector<DWORD, CRenderAllocator<DWORD>>;
	CDynArray<DWORD> m_vIndices;
	//vector<std::vector<TBoneInfluence>> m_vWeights;
	//vector<TBone>						m_vJoints;

	// Assignment Operator
	TMeshVertexInfo& operator=(TMeshVertexInfo& rhs);
};	

//typedef vector<vector<TBoneInfluence, CRenderAllocator<TBoneInfluence>>, 
//	CRenderAllocator<vector<TBoneInfluence, CRenderAllocator<TBoneInfluence>>>>;
typedef CDynArray<CDynArray<TBoneInfluence>> InfluCont;

// Mesh
class DXMesh
{
	// D3DX Info
	IDirect3DVertexBuffer9*		 m_pVertBuffer;		// Vertex Buffer
	IDirect3DIndexBuffer9*		 m_pIndexBuffer;	// Index Buffer
	IDirect3DVertexDeclaration9* m_pVertDecl;		// Vertex Declaration
	D3DPRIMITIVETYPE			 m_Mode;			// Primitive Type

	// Vert Info
	unsigned int			 m_uVertCount;		// Number of Vertices
	unsigned int			 m_uIndexCount;		// Number of Indices
	unsigned int			 m_uVertSize;		// Size of Vertex
	bool					 m_bIndexed;		// Is Indexed?
	bool					 m_bCreateBuffer;	// Should the Buffers be Created?

	bool					 m_bFlashingY;		// Is the Mesh Flashing Yellow?
	bool					 m_bFlashingW;		// Is the Mesh Flashing White?
	bool					 m_bFlashingB;		// Is the Mesh Flashing Blue?

	float					 m_fFlashRate;		// the percent to flash

protected:
	TMeshVertexInfo			 m_tVertInfo;		// Raw Vertex Information
private:
	
	// Animation Data
	
	// Bone Weights
	InfluCont		m_cWeights;

	// Bone Hierarchy
	unsigned int						m_uNumBones;
	//vector<TBindBone, CRenderAllocator<TBindBone>>;
	CDynArray<TBindBone> m_cJoints;

public:

	// Default Constructor
	DXMesh();

	// Destructor
	~DXMesh();

	// Mesh Name
	char					 m_szName[64];

	TMeshVertexInfo& GetVertInfo() { return m_tVertInfo; }

	// Assignment Operator
	DXMesh& operator=(DXMesh& rhs);

	// Create Buffers
	void CreateVertexBuffer(const void *pVertArray, 
		unsigned int uVertCount, IDirect3DVertexDeclaration9 * pVertDecl,
		unsigned int uVertSize, D3DPRIMITIVETYPE mode);
	void CreateVertexNormalTexture2DBuffer(const VERTEX_POS3_NORM3_TEX2 *pVertArray, unsigned int iVertCount);
	void CreateVertexColorTexture2DBuffer(const VERTEX_POS3_COL4_TEX2 *pVertArray, unsigned int iVertCount);
	//void CreateVertexTexture3DBuffer(const VERTEX_POS3_TEX3 *pVertArray, unsigned int uVertCount);
	void CreateIndexBuffer(const DWORD *dwpIndices, unsigned int uIndexCount);

	// Accessors
	IDirect3DVertexBuffer9		 * GetVertBuffer(void)  const { return m_pVertBuffer;  }
	IDirect3DIndexBuffer9		 * GetIndexBuffer(void) const {	return m_pIndexBuffer; }
	IDirect3DVertexDeclaration9  * GetVertDecl(void)	const { return m_pVertDecl;	   }
	
	const unsigned int GetVertCount(void)  const { return m_uVertCount;  }
	const unsigned int GetIndexCount(void) const { return m_uIndexCount; }
	const unsigned int GetVertSize(void)   const { return m_uVertSize;   }
	const D3DPRIMITIVETYPE GetMode(void)   const { return m_Mode;		 }
	char* GetName()		   { return m_szName;		  }
	string GetNameString() { return string(m_szName); }
	
	const unsigned int GetTriangleCount(void) const
	{ 
		if(m_Mode == D3DPT_TRIANGLELIST)
		{
			if(true == m_bIndexed)
				return m_uIndexCount / 3;
			else
				return m_uVertCount / 3;
		}
		else if(m_Mode == D3DPT_TRIANGLESTRIP)
		{
			if(true == m_bIndexed)
				return m_uIndexCount - 2;
			else
				return m_uVertCount - 2;
		}
		else
		{
			if(true == m_bIndexed)
				return m_uIndexCount / 3;
			else
				return m_uVertCount / 3;
		}
	}
	const bool IsIndexed(void) const  { return m_bIndexed; }
	const bool IsFlashingYellow(void) const  { return m_bFlashingY; }
	const bool IsFlashingWhite(void) const  { return m_bFlashingW; }
	const bool IsFlashingBlue(void) const  { return m_bFlashingB; }
	const float GetFlashRate(void) const { return m_fFlashRate; }

	//vector<TBindBone, CRenderAllocator<TBindBone>>
	CDynArray<TBindBone>& GetBones(void)				 { return m_cJoints;  }
	InfluCont& GetWeights(void) { return m_cWeights; }

	// Mutators
	void SetMode(D3DPRIMITIVETYPE mode)			{ m_Mode = mode;				  }
	void SetWeight(int nIdx, TBoneInfluence bi)
	{ 
		m_cWeights[nIdx].push_back(bi);
	}
	void SizeWeightsToTwo(int nIdx)
	{
		while(m_cWeights[nIdx].size() != 2)
		{
			TBoneInfluence bi;
			bi.m_fWeight = 0.0f;
			bi.m_nBoneIndex = 0;
			m_cWeights[nIdx].push_back(bi);
		}
	}

	void AddBoneToHiearchy(TBindBone& tBone)		{ m_cJoints.push_back(tBone);     }
	void SetIndexedTexturedVertexInfo(TMeshVertexInfo* pMeshInfo);//,// bool createVB = false);
	void SetIndexedColoredTexturedVertexInfo(TMeshVertexInfo* pMeshInfo);//,// bool createVB = false);
	void SetColoredTexturedVertexInfo(TMeshVertexInfo* pMeshInfo);//,// bool createVB = false);
	
	void SetFlashingYellow(bool bFlashingY) { m_bFlashingY = bFlashingY; }
	void SetFlashingWhite(bool bFlashingW) { m_bFlashingW = bFlashingW; }
	void SetFlashingBlue(bool bFlashingB) { m_bFlashingB = bFlashingB; }
	void SetFlashRate(float fFlashRate) { m_fFlashRate = fFlashRate; }

	// Reset Buffers
	void ResetVertBuffer(const void *pVertArray, int nVertCount, unsigned int uVertSize);
	void ResetIndexBuffer(const DWORD *dwpIdxArray, unsigned int uIdxCount);

	// Build Bone Hiearchy
	void BuildHiearchy(void);

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS
///////////////////////////////////////////////////////////////////////////////

	// Lost Device
	void LostDevice(void);
	static void LostDeviceCallback(IEvent*, IComponent* pComp);
};

#endif // _DXMESH_H_