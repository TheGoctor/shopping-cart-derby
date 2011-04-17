#include "DXMesh.h"

DXMesh::DXMesh(void) 
		: m_pVertBuffer(NULL), m_pIndexBuffer(NULL), m_pVertDecl(NULL),
		  m_iVertCount(0), m_iIndexCount(0), m_iVertSize(0), m_Mode(D3DPT_TRIANGLELIST),
		  m_bIndexed(false) 
{
}

DXMesh::~DXMesh(void)
{
	SAFE_RELEASE(m_pVertBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

// Create Vertex Position and Color Buffer
void DXMesh::CreateVertexBuffer(const void *pVertArray, unsigned int iVertCount,
								IDirect3DVertexDeclaration9 * pVertDecl,
								unsigned int iVertSize, D3DPRIMITIVETYPE mode)
{
	// Get Singletons
	Direct3DManager * pD3DMan = Direct3DManager::GetInstance();

	void * buffer;

	// Set Values
	m_iVertCount = iVertCount;
	m_pVertDecl = pVertDecl;
	m_iVertSize = iVertSize;
	m_Mode = mode;

	// Reset Buffer
	SAFE_RELEASE(m_pVertBuffer);

	// Fill it Out
	unsigned int iBufferSize = iVertSize * iVertCount;
	pD3DMan->GetDirect3DDevice()->CreateVertexBuffer(iBufferSize, 0, 0, D3DPOOL_DEFAULT, &m_pVertBuffer, 0);
	m_pVertBuffer->Lock(0, 0, &buffer, 0);
	memcpy(buffer, pVertArray, iBufferSize);
	m_pVertBuffer->Unlock();
}

void DXMesh::CreateVertexNormalTexture2DBuffer(const VERTEX_POS3_NORM3_TEX2 *pVertArray, unsigned int iVertCount)
{
	CreateVertexBuffer(pVertArray, iVertCount,
		Direct3DManager::GetInstance()->GetVertNormTex2DDecl(), sizeof(VERTEX_POS3_NORM3_TEX2), D3DPT_TRIANGLELIST);
}

void DXMesh::CreateVertexTexture3DBuffer(const VERTEX_POS3_TEX3 *pVertArray, unsigned int iVertCount)
{
	CreateVertexBuffer(pVertArray, iVertCount,
		Direct3DManager::GetInstance()->GetVertTex3DDecl(), sizeof(VERTEX_POS3_TEX3), D3DPT_TRIANGLELIST);
}

// Create Index Buffer
void DXMesh::CreateIndexBuffer(const DWORD *iIndices, unsigned int iIndexCount)
{
	// Get Singletons
	Direct3DManager * pD3DMan = Direct3DManager::GetInstance();

	void * buffer;

	// Update Values
	m_iIndexCount = iIndexCount;
	m_bIndexed = true;

	// Reset Buffer
	SAFE_RELEASE(m_pIndexBuffer);

	// Fill it Out
	unsigned int iBufferSize = sizeof(DWORD) * iIndexCount;
	pD3DMan->GetDirect3DDevice()->CreateIndexBuffer(iBufferSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, 0);
	m_pIndexBuffer->Lock(0, 0, &buffer, 0);
	memcpy(buffer, iIndices, iBufferSize);
	m_pIndexBuffer->Unlock();
}