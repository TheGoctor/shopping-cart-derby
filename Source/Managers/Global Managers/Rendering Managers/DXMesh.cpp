////////////////////////////////////////////////////////////////////////////////
//	File			:	DXMesh.cpp
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Mesh,
//						Acts as Base Class for other 3D Rendersbles
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "DXMesh.h"

// Vertex Info Assignment Operator
TMeshVertexInfo& TMeshVertexInfo::operator=(TMeshVertexInfo& rhs)
{
	// Self Check
	if(this == &rhs)
		return *this;

	// Clear our Vectors, if we have something already
	this->m_vVertices.resize(rhs.m_vVertices.size());
	this->m_vColors.resize(rhs.m_vColors.size());
	this->m_vNormals.resize(rhs.m_vNormals.size());
	this->m_vUV.resize(rhs.m_vUV.size());
	this->m_vIndices.resize(rhs.m_vIndices.size());

	// Copy over Vertex Values
	unsigned int uNumVerts = rhs.m_vVertices.size();
	for(unsigned int uVert = 0; uVert < uNumVerts; ++uVert)
	{
		this->m_vVertices.push_back(rhs.m_vVertices[uVert]);
		this->m_vNormals.push_back(rhs.m_vNormals[uVert]);
		this->m_vUV.push_back(rhs.m_vUV[uVert]);
	}

	// Copy over Color
	unsigned int uNumColors = rhs.m_vColors.size();
	for(unsigned int uVert = 0; uVert < uNumColors; ++uVert)
	{
		this->m_vColors.push_back(rhs.m_vColors[uVert]);
	}

	// Copy over Index Values
	unsigned int uNumIdxs = rhs.m_vIndices.size();
	for(unsigned int uIdx = 0; uIdx < uNumIdxs; ++uIdx)
	{
		this->m_vIndices.push_back(rhs.m_vIndices[uIdx]);
	}

	// Return Self
	return *this;
}

// Mesh Assignment Operator
DXMesh& DXMesh::operator=(DXMesh& rhs)
{
	// Self Check
	if(this == &rhs)
		return *this;

	// Set Vertex Information
	this->SetIndexedTexturedVertexInfo(&rhs.m_tVertInfo);//, true);

	// Set Weights
	this->m_cWeights.resize(rhs.m_cWeights.size());
	for(unsigned int weight = 0; weight < this->m_cWeights.size(); ++weight)
	{
		this->m_cWeights[weight].resize(rhs.m_cWeights[weight].size());
		for(unsigned int inf = 0; inf < this->m_cWeights[weight].size(); ++inf)
		{
			this->m_cWeights[weight][inf] = rhs.m_cWeights[weight][inf];
		}
	}

	// Set Bones
	m_uNumBones = rhs.m_cJoints.size();
	this->m_cJoints.resize(m_uNumBones);
	for(unsigned int bone = 0; m_uNumBones; ++bone)
	{
		this->m_cJoints[bone] = rhs.m_cJoints[bone];

		this->m_cJoints[bone].m_cChildrenIdxs.resize(rhs.m_cJoints[bone].m_cChildrenIdxs.size());
		for(unsigned int child = 0; child < this->m_cJoints[bone].m_cChildrenIdxs.size(); ++child)
		{
			this->m_cJoints[bone].m_cChildrenIdxs[child] = rhs.m_cJoints[bone].m_cChildrenIdxs[child];
		}
	}

	// Build Connections
	BuildHiearchy();

	return *this;
}

// Constructor
DXMesh::DXMesh(void) 
		: m_pVertBuffer(NULL), m_pIndexBuffer(NULL), m_pVertDecl(NULL), //m_pVertInfo(NULL),
		  m_uVertCount(0), m_uIndexCount(0), m_uVertSize(0), m_Mode(D3DPT_TRIANGLELIST),
		  m_bIndexed(false), m_bCreateBuffer(true), 
		  m_bFlashingY(false), m_bFlashingW(false), m_bFlashingB(false), m_uNumBones(0),
		  m_fFlashRate(0.0f)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("LostDevice", (IComponent*)this, LostDeviceCallback);
}

// Destructor
DXMesh::~DXMesh(void)
{
	// Clean up Buffers
	SAFE_RELEASE(m_pVertBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	
	/*if(m_pVertInfo)
		MMDEL(m_pVertInfo);*/

	// Clean up Bones
	int nNumBones = m_cJoints.size();
	for(int bone = 0; bone < nNumBones; ++bone)
	{
		m_cJoints[bone].m_cChildrenIdxs.clear();
	}
	if(nNumBones > 0)
		m_cJoints.clear();

	// Clean up Weights
	int nNumWeights = m_cWeights.size();
	for(int weight = 0; weight < nNumWeights; ++weight)
	{
		m_cWeights[weight].clear();
	}
	if(nNumWeights > 0)
		m_cWeights.clear();
}

// Create Vertex Position and Color Buffer
void DXMesh::CreateVertexBuffer(const void *pVertArray, unsigned int uVertCount,
								IDirect3DVertexDeclaration9 * pVertDecl,
								unsigned int uVertSize, D3DPRIMITIVETYPE mode)
{
	// Get Singletons
	Direct3DManager * pD3DMan = Direct3DManager::GetInstance();

	void * buffer;

	// Set Values
	m_uVertCount = uVertCount;
	m_pVertDecl  = pVertDecl;
	m_uVertSize  = uVertSize;
	m_Mode       = mode;

	// Reset Buffer
	SAFE_RELEASE(m_pVertBuffer);

	// Reset Vert Info
	m_bCreateBuffer = false;

	// Fill it Out
	unsigned int uBufferSize = uVertSize * uVertCount;
	pD3DMan->GetDirect3DDevice()->CreateVertexBuffer(uBufferSize, 0, 0, D3DPOOL_MANAGED, &m_pVertBuffer, 0);
	m_pVertBuffer->Lock(0, 0, &buffer, 0);
	memcpy(buffer, pVertArray, uBufferSize);
	m_pVertBuffer->Unlock();
}

void DXMesh::CreateVertexNormalTexture2DBuffer(const VERTEX_POS3_NORM3_TEX2 *pVertArray, unsigned int iVertCount)
{
	CreateVertexBuffer(pVertArray, iVertCount,
		Direct3DManager::GetInstance()->GetVertNormTex2DDecl(), sizeof(VERTEX_POS3_NORM3_TEX2), m_Mode);
}

void DXMesh::CreateVertexColorTexture2DBuffer(const VERTEX_POS3_COL4_TEX2 *pVertArray, unsigned int iVertCount)
{
	CreateVertexBuffer(pVertArray, iVertCount,
		Direct3DManager::GetInstance()->GetVertColTex2DDecl(), sizeof(VERTEX_POS3_COL4_TEX2), m_Mode);
}

//void DXMesh::CreateVertexTexture3DBuffer(const VERTEX_POS3_TEX3 *pVertArray, unsigned int uVertCount)
//{
//	CreateVertexBuffer(pVertArray, uVertCount,
//		Direct3DManager::GetInstance()->GetVertTex3DDecl(), sizeof(VERTEX_POS3_TEX3), m_Mode);
//}

// Create Index Buffer
void DXMesh::CreateIndexBuffer(const DWORD *dwpIndices, unsigned int uIndexCount)
{
	// Get Singletons
	Direct3DManager * pD3DMan = Direct3DManager::GetInstance();

	void* buffer;

	// Update Values
	m_uIndexCount = uIndexCount;
	m_bIndexed	  = true;

	// Reset Buffer
	SAFE_RELEASE(m_pIndexBuffer);

	// Fill it Out
	unsigned int uBufferSize = sizeof(DWORD) * uIndexCount;
	pD3DMan->GetDirect3DDevice()->CreateIndexBuffer(uBufferSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, 0);
	m_pIndexBuffer->Lock(0, 0, &buffer, 0);
	memcpy(buffer, dwpIndices, uBufferSize);
	m_pIndexBuffer->Unlock();
}

// Reset Buffers
void DXMesh::ResetVertBuffer(const void *pVertArray, int nVertCount, unsigned int uVertSize)
{
	// Create Buffer
	void* buffer;

	// Find Buffer Size
	unsigned int uBufferSize = uVertSize * nVertCount;
	m_uVertCount = nVertCount;

	// Reset Buffer
	//if(m_pVertBuffer)
	//{
		m_pVertBuffer->Lock(0, 0, &buffer, 0);
		memcpy(buffer, pVertArray, uBufferSize);
		m_pVertBuffer->Unlock();
	//}
}
void DXMesh::ResetIndexBuffer(const DWORD *dwpIdxArray, unsigned int uIdxCount)
{
	// Create Buffer
	void* buffer;

	// Find Buffer Size
	unsigned int uBufferSize = sizeof(DWORD) * uIdxCount;
	m_uIndexCount = uIdxCount;

	// Reset Buffer
	//if(m_pIndexBuffer)
	//{
		m_pIndexBuffer->Lock(0, 0, &buffer, 0);
		memcpy(buffer, dwpIdxArray, uBufferSize);
		m_pIndexBuffer->Unlock();
	//}
}

// Build Bone Connections
void DXMesh::BuildHiearchy(void)
{
	m_uNumBones = m_cJoints.size();
	for(unsigned bone = 0; bone < m_uNumBones; ++bone)
	{
		for(int child = 0; child < m_cJoints[bone].m_nNumChildren; ++child)
		{
			m_cJoints[bone].m_cFrame.AddChildFrame(
				&m_cJoints[m_cJoints[bone].m_cChildrenIdxs[child]].m_cFrame);
		}
	}
	
	// Update to Connect the Frame
	m_cJoints[0].m_cFrame.Update();
}

// Set Vertex Info
void DXMesh::SetIndexedTexturedVertexInfo(TMeshVertexInfo* pMeshInfo)//, bool createVB)
{
	int nVertCount = pMeshInfo->m_vVertices.size();
	VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[nVertCount];

	int nNumIdxs = pMeshInfo->m_vIndices.size();
	DWORD *indices = new DWORD[nNumIdxs];

	// Verts
	for(int nVert = 0; nVert < nVertCount; ++nVert)
	{
		verts[nVert].position = pMeshInfo->m_vVertices[nVert];
		verts[nVert].normal = pMeshInfo->m_vNormals[nVert];
		verts[nVert].uv = pMeshInfo->m_vUV[nVert];
	}

	// Indices
	for(int idx = 0; idx < nNumIdxs; ++idx)
	{
		indices[idx] = pMeshInfo->m_vIndices[idx];
	}

	if(m_bCreateBuffer)
	{
		// Create Vert Buffer
		CreateVertexNormalTexture2DBuffer(verts, nVertCount);

		// Create Index Buffer
		CreateIndexBuffer(indices, nNumIdxs);
	}
	else
	{
		ResetVertBuffer(verts, nVertCount, sizeof(VERTEX_POS3_NORM3_TEX2));
		ResetIndexBuffer(indices, nNumIdxs);
	}
	
	// Copy Vertex Info
	m_tVertInfo = *pMeshInfo;

	// Clean up
	delete [] verts;
	delete [] indices;
}

void DXMesh::SetIndexedColoredTexturedVertexInfo(TMeshVertexInfo* pMeshInfo)
{
	int nVertCount = pMeshInfo->m_vVertices.size();
	VERTEX_POS3_COL4_TEX2 *verts = new VERTEX_POS3_COL4_TEX2[nVertCount];

	int nNumIdxs = pMeshInfo->m_vIndices.size();
	DWORD *indices = new DWORD[nNumIdxs];

	// Verts
	for(int nVert = 0; nVert < nVertCount; ++nVert)
	{
		verts[nVert].position = pMeshInfo->m_vVertices[nVert];
		verts[nVert].color = pMeshInfo->m_vColors[nVert];
		verts[nVert].uv = pMeshInfo->m_vUV[nVert];
	}

	// Indices
	for(int idx = 0; idx < nNumIdxs; ++idx)
	{
		indices[idx] = pMeshInfo->m_vIndices[idx];
	}

	if(m_bCreateBuffer)
	{
		// Create Vert Buffer
		CreateVertexColorTexture2DBuffer(verts, nVertCount);

		// Create Index Buffer
		CreateIndexBuffer(indices, nNumIdxs);
	}
	else
	{
		ResetVertBuffer(verts, nVertCount, sizeof(VERTEX_POS3_COL4_TEX2));
		ResetIndexBuffer(indices, nNumIdxs);
	}
	
	// Copy Vertex Info
	m_tVertInfo = *pMeshInfo;

	// Clean up
	delete [] verts;
	delete [] indices;
}

void DXMesh::SetColoredTexturedVertexInfo(TMeshVertexInfo* pMeshInfo)
{
	int nVertCount = pMeshInfo->m_vVertices.size();
	VERTEX_POS3_COL4_TEX2 *verts = new VERTEX_POS3_COL4_TEX2[nVertCount];

	//unsigned int nNumIdxs = pMeshInfo->m_vIndices.size();

	// Verts
	for(int nVert = 0; nVert < nVertCount; ++nVert)
	{
		verts[nVert].position = pMeshInfo->m_vVertices[nVert];
		verts[nVert].color = pMeshInfo->m_vColors[nVert];
		verts[nVert].uv = pMeshInfo->m_vUV[nVert];
	}

	//if(m_bCreateBuffer)
	//{
		// Create Vert Buffer
		CreateVertexColorTexture2DBuffer(verts, nVertCount);
	//}
	//else
	//{
		//ResetVertBuffer(verts, nVertCount, sizeof(VERTEX_POS3_COL4_TEX2));
	//}
	
	// Copy Vertex Info
	m_tVertInfo = *pMeshInfo;

	// Clean up
	delete [] verts;
}


///////////////////////////////////////////////////////////////////////////////
// CALLBACKS
///////////////////////////////////////////////////////////////////////////////

// Lost Device
void DXMesh::LostDeviceCallback(IEvent*, IComponent* /*pComp*/)
{
	// Get Mesh
	//DXMesh* pMesh = (DXMesh*)pComp;

	// Call Lost Device on Component
	//pMesh->LostDevice();
}
void DXMesh::LostDevice(void)
{
	//// Get Singletons
	//Direct3DManager* pD3D = Direct3DManager::GetInstance();

	//// Create Vert Buffer
	//CreateVertexBuffer(NULL, GetVertCount(), pD3D->GetVertNormTex2DDecl(),
	//	sizeof(VERTEX_POS3_NORM3_TEX2), GetMode());

	//// Create Index Buffer
	//CreateIndexBuffer(NULL, GetIndexCount());

	m_bCreateBuffer = true;
	//SetIndexedTexturedVertexInfo(&this->m_tVertInfo);
}