#pragma once

#include "rendering/dx_util.h"

// vertex (position, color) structure
struct VERTEX_POSCOLOR
{
	D3DXVECTOR3	position;	// 12 Bytes (3floats * 4bytes)
	D3DCOLOR	color;		// 4  Bytes (ARGB)
};

// vertex (position, color, texture) structure
struct VERTEX_POS3_COL4_TEX2
{
	D3DXVECTOR3	position;	// 12 Bytes (3floats * 4bytes)
	D3DCOLOR	color;		// 4  Bytes (ARGB)
	D3DXVECTOR2	uv;			// 8  Bytes (2floats * 4bytes)
};

// vertex (position, normal, texture2D) structure
struct VERTEX_POS3_NORM3_TEX2
{
	D3DXVECTOR3	position;	// 12 Bytes (3floats * 4bytes)
	D3DXVECTOR3	normal;		// 12 Bytes (3floats * 4bytes)
	D3DXVECTOR2	uv;			// 8  Bytes (2floats * 4bytes)
};

// vertex (position, texture3D) structure
struct VERTEX_POS3_TEX3
{
	D3DXVECTOR3	position;	// 12 Bytes (3floats * 4bytes)
	float		u,v,w;		// 12 Bytes (3floats * 4bytes)
};
