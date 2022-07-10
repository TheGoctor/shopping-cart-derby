////////////////////////////////////////////////////////////////////////////////
//	File			:	DXMesh.h
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Mesh,
//						Acts as Base Class for other 3D Rendersbles
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/containers.h"
#include "physics/math_types.h"
#include "rendering/animation.h"
#include "rendering/direct3d_manager.h"

#include <string>

namespace scd {
// BonedMesh Data
struct bind_bone {
  std::string _name;
  unsigned int _id        = 0;
  unsigned int _parent_id = 0;
  scd::transform _frame;
  scd::vector<unsigned int> _child_ids;
};

// Vertex Info
struct mesh_vertex_info {
  scd::vector<scd::vector3> _vertices;
  scd::vector<scd::vector3> _normals;
  scd::vector<scd::vector4> _colors;
  scd::vector<scd::vector2> _tex_coords;
  scd::vector<std::uint32_t> _indices;
};

using influence_container = scd::vector<scd::vector<bone_influence>>;

// Mesh
class dx_mesh {
  // D3DX Info
  IDirect3DVertexBuffer9* m_pVertBuffer;    // Vertex Buffer
  IDirect3DIndexBuffer9* m_pIndexBuffer;    // Index Buffer
  IDirect3DVertexDeclaration9* m_pVertDecl; // Vertex Declaration
  D3DPRIMITIVETYPE m_Mode;                  // Primitive Type

  // Vert Info
  unsigned int m_uVertCount;  // Number of Vertices
  unsigned int m_uIndexCount; // Number of Indices
  unsigned int m_uVertSize;   // Size of Vertex
  bool m_bIndexed;            // Is Indexed?
  bool m_bCreateBuffer;       // Should the Buffers be Created?

  bool m_bFlashingY; // Is the Mesh Flashing Yellow?
  bool m_bFlashingW; // Is the Mesh Flashing White?
  bool m_bFlashingB; // Is the Mesh Flashing Blue?

  float m_fFlashRate; // the percent to flash

protected:
  mesh_vertex_info _vertex_info; // Raw Vertex Information
private:
  // Animation Data

  // Bone Weights
  influence_container m_cWeights;

  // Bone Hierarchy
  unsigned int m_uNumBones;
  scd::vector<bind_bone> m_cJoints;

public:
  // Default Constructor
  dx_mesh();

  // Destructor
  ~dx_mesh();

  // Mesh Name
  std::string _name;

  const mesh_vertex_info& vertex_info() const { return _vertex_info; }

  // Assignment Operator
  dx_mesh& operator=(const dx_mesh& rhs);

  // Create Buffers
  void CreateVertexBuffer(const void* pVertArray, unsigned int uVertCount,
                          IDirect3DVertexDeclaration9* pVertDecl,
                          unsigned int uVertSize, D3DPRIMITIVETYPE mode);
  void
    CreateVertexNormalTexture2DBuffer(const VERTEX_POS3_NORM3_TEX2* pVertArray,
                                      unsigned int iVertCount);
  void CreateVertexColorTexture2DBuffer(const VERTEX_POS3_COL4_TEX2* pVertArray,
                                        unsigned int iVertCount);
  // void CreateVertexTexture3DBuffer(const VERTEX_POS3_TEX3 *pVertArray,
  // unsigned int uVertCount);
  void CreateIndexBuffer(const DWORD* dwpIndices, unsigned int uIndexCount);

  // Accessors
  IDirect3DVertexBuffer9* GetVertBuffer(void) const { return m_pVertBuffer; }
  IDirect3DIndexBuffer9* GetIndexBuffer(void) const { return m_pIndexBuffer; }
  IDirect3DVertexDeclaration9* GetVertDecl(void) const { return m_pVertDecl; }

  const unsigned int GetVertCount(void) const { return m_uVertCount; }
  const unsigned int GetIndexCount(void) const { return m_uIndexCount; }
  const unsigned int GetVertSize(void) const { return m_uVertSize; }
  const D3DPRIMITIVETYPE GetMode(void) const { return m_Mode; }
  char* GetName() { return m_szName; }
  const std::string& name() const { return _name; }

  const unsigned int GetTriangleCount(void) const {
    if (m_Mode == D3DPT_TRIANGLELIST) {
      if (true == m_bIndexed)
        return m_uIndexCount / 3;
      else
        return m_uVertCount / 3;
    } else if (m_Mode == D3DPT_TRIANGLESTRIP) {
      if (true == m_bIndexed)
        return m_uIndexCount - 2;
      else
        return m_uVertCount - 2;
    } else {
      if (true == m_bIndexed)
        return m_uIndexCount / 3;
      else
        return m_uVertCount / 3;
    }
  }
  const bool IsIndexed(void) const { return m_bIndexed; }
  const bool IsFlashingYellow(void) const { return m_bFlashingY; }
  const bool IsFlashingWhite(void) const { return m_bFlashingW; }
  const bool IsFlashingBlue(void) const { return m_bFlashingB; }
  const float GetFlashRate(void) const { return m_fFlashRate; }

  // vector<TBindBone, CRenderAllocator<TBindBone>>
  CDynArray<TBindBone>& GetBones(void) { return m_cJoints; }
  InfluCont& GetWeights(void) { return m_cWeights; }

  // Mutators
  void SetMode(D3DPRIMITIVETYPE mode) { m_Mode = mode; }
  void SetWeight(int nIdx, TBoneInfluence bi) {
    m_cWeights[nIdx].push_back(bi);
  }
  void SizeWeightsToTwo(int nIdx) {
    while (m_cWeights[nIdx].size() != 2) {
      TBoneInfluence bi;
      bi.m_fWeight    = 0.0f;
      bi.m_nBoneIndex = 0;
      m_cWeights[nIdx].push_back(bi);
    }
  }

  void AddBoneToHiearchy(TBindBone& tBone) { m_cJoints.push_back(tBone); }
  void SetIndexedTexturedVertexInfo(
    TMeshVertexInfo* pMeshInfo); //,// bool createVB = false);
  void SetIndexedColoredTexturedVertexInfo(
    TMeshVertexInfo* pMeshInfo); //,// bool createVB = false);
  void SetColoredTexturedVertexInfo(
    TMeshVertexInfo* pMeshInfo); //,// bool createVB = false);

  void SetFlashingYellow(bool bFlashingY) { m_bFlashingY = bFlashingY; }
  void SetFlashingWhite(bool bFlashingW) { m_bFlashingW = bFlashingW; }
  void SetFlashingBlue(bool bFlashingB) { m_bFlashingB = bFlashingB; }
  void SetFlashRate(float fFlashRate) { m_fFlashRate = fFlashRate; }

  // Reset Buffers
  void ResetVertBuffer(const void* pVertArray, int nVertCount,
                       unsigned int uVertSize);
  void ResetIndexBuffer(const DWORD* dwpIdxArray, unsigned int uIdxCount);

  // Build Bone Hiearchy
  void BuildHiearchy(void);

  ///////////////////////////////////////////////////////////////////////////////
  // CALLBACKS
  ///////////////////////////////////////////////////////////////////////////////

  // Lost Device
  void LostDevice(void);
  static void LostDeviceCallback(IEvent*, IComponent* pComp);
};
} // namespace scd
