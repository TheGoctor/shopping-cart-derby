#ifndef _CDXMESH_H_
#define _CDXMESH_H_

#include "Direct3DManager.h"

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

	void SetMode(D3DPRIMITIVETYPE mode) 
	{ 
		m_Mode = mode; 
	}
};

#endif