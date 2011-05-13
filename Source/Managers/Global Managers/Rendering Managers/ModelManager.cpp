#include "ModelManager.h"
#include "Direct3DManager.h"
#include <iostream>
#include <fstream>
using namespace std;

ModelManager ModelManager::m_pInstance;

ModelManager* ModelManager::GetInstance()
{
	return &m_pInstance;
}

ModelManager::ModelManager()
{
	currIndex = 0;
}

ModelManager::~ModelManager()
{


}
int ModelManager::LoadModel(char* szFileName)
{
	//read String Size
	//read Name of Model
	//read Loop size
	//read Vertices
	//read Normals
	//read Text COORDS
	//read Triangles
	unsigned int num = 0;
	char buffer[128];


	memset(buffer, 0, 128);

	fstream fin(szFileName, ios_base::in | ios_base::binary);

	if(fin.is_open())
	{
		D3DXVECTOR3 *pos_buffer;
		D3DXVECTOR3 *norm_buffer;
		D3DXVECTOR2	*uv_buffer;
		DWORD		*indicies;

		// strlen
		fin.read((char*)&num, 4);
		// name
		fin.read((char*)&m_pMeshBuffer[currIndex].m_szName, num);
		// loop size
		fin.read((char*)&num, 4);
		VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[num];
		//VERTEX_POSCOLOR *verts = new VERTEX_POSCOLOR[num];
		pos_buffer = new D3DXVECTOR3[num];
		norm_buffer = new D3DXVECTOR3[num];
		uv_buffer = new D3DXVECTOR2[num];
		// vertices
		for(unsigned int i = 0; i < num; ++i)
		{
			fin.read((char*)&pos_buffer[i].x, sizeof(float));
			fin.read((char*)&pos_buffer[i].y, sizeof(float));
			fin.read((char*)&pos_buffer[i].z, sizeof(float));

		}
		for(unsigned int i = 0; i < num; ++i)
		{
			fin.read((char*)&norm_buffer[i].x, sizeof(float));
			fin.read((char*)&norm_buffer[i].y, sizeof(float));
			fin.read((char*)&norm_buffer[i].z, sizeof(float));

		}
		for(unsigned int i = 0; i < num; ++i)
		{
			fin.read((char*)&uv_buffer[i].x, sizeof(float));
			fin.read((char*)&uv_buffer[i].y, sizeof(float));
		}
		for(unsigned int i = 0; i < num; ++i)
		{
			verts[i].position.x = pos_buffer[i].x;
			verts[i].position.y = pos_buffer[i].y;
			verts[i].position.z = -pos_buffer[i].z;

			verts[i].normal = norm_buffer[i];
			verts[i].uv.x = uv_buffer[i].x;
			verts[i].uv.y = -uv_buffer[i].y;

			//verts[i].color = D3DCOLOR_ARGB(255,rand()%255,rand()%255,rand()%255);
		}

		this->m_pMeshBuffer[currIndex].CreateVertexBuffer(verts, num, 
			Direct3DManager::GetInstance()->GetVertNormTex2DDecl(),
			sizeof(VERTEX_POS3_NORM3_TEX2),
			this->m_pMeshBuffer[currIndex].GetMode());

		fin.read((char*)&num, 4);
		indicies = new DWORD[num*3];
		for(unsigned int i = 0; i < num*3; i+=3)
		{
			fin.read((char*)&indicies[i], 4);
			fin.read((char*)&indicies[i+1], 4);
			fin.read((char*)&indicies[i+2], 4);
		}

		this->m_pMeshBuffer[currIndex].CreateIndexBuffer(indicies, num*3);

		delete [] pos_buffer;
		delete [] norm_buffer;
		delete [] uv_buffer;
		delete [] verts;
		delete [] indicies;

		fin.close();
		fin.clear();
	}
	else
		return -1;


	currIndex++;
	return currIndex-1;
}

int ModelManager::LoadAABB(char* szMeshName)
{
	unsigned int num = 0;
	char buffer[128];

	memset(buffer, 0, 128);

	fstream fin(szMeshName, ios_base::in | ios_base::binary);

	if(fin.is_open())
	{
		D3DXVECTOR3 *pos_buffer;

		// strlen
		fin.read((char*)&num, 4);
		// name
		fin.read((char*)&m_pAABBBuffer[currAABBIndex].m_szName, num);
		// loop size
		fin.read((char*)&num, 4);

		pos_buffer = new D3DXVECTOR3[num];

		// vertices
		for(unsigned int i = 0; i < num; ++i)
		{
			fin.read((char*)&pos_buffer[i].x, sizeof(float));
			fin.read((char*)&pos_buffer[i].y, sizeof(float));
			fin.read((char*)&pos_buffer[i].z, sizeof(float));
		}

		fin.close();
		fin.clear();

		for(unsigned int i = 0; i < num; ++i)
		{
			if(pos_buffer[i].x > m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.x)
				m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.x = pos_buffer[i].x;

			if(pos_buffer[i].y > m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.y)
				m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.y = pos_buffer[i].y;

			if(pos_buffer[i].z > m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.z)
				m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.z = pos_buffer[i].z;

			if(pos_buffer[i].x < m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.x)
				m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.x = pos_buffer[i].x;

			if(pos_buffer[i].y < m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.y)
				m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.y = pos_buffer[i].y;

			if(pos_buffer[i].z < m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.z)
				m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.z = pos_buffer[i].z;
		}

		// TODO: find the largest side and invert the coord of that axis
		//ex. v z is longest side
		m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.z *= -1;
		m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.z *= -1;
		int largestSide;
		int xLen, yLen, zLen;
		xLen = (int)(m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.x - m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.x);
		yLen = (int)(m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.y - m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.y);
		zLen = (int)(m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.z - m_pAABBBuffer[currAABBIndex].aabb.cBoxMin.z);
		if(xLen >= yLen && xLen >= zLen)
			largestSide = 0;
		if(yLen >= zLen && yLen >= xLen)
			largestSide = 1;
		if(zLen >= xLen && zLen >= yLen)
			largestSide = 2;

		delete [] pos_buffer;
	}
	else
		return -1;

	currAABBIndex++;
	return currAABBIndex - 1;
}

int ModelManager::GetMeshIndexByName(const char* szMeshName, bool bIsCollision)
{
	// Strip the Numbers from the End of the Mesh Name
	char buffer[64];
	memset(buffer, 0, 64);
	int namelength = strlen(szMeshName);
	int offset = 0;
	while(isdigit(szMeshName[namelength - offset]) || 
		szMeshName[namelength - offset] == '\0')
	{
		++offset;
	}
	memcpy(buffer, szMeshName, namelength - offset+1);
	buffer[namelength - offset + 1] = '\0';

	if(bIsCollision)
	{
		memcpy(&buffer[namelength - offset + 1], "_Collision\0", 11);
	}

	// Look for a Match
	for(unsigned int i = 0; i < currIndex; ++i)
	{
		if(strcmp(buffer, m_pMeshBuffer[i].GetName()) == 0)
		{
			return i;
		}
	}
	return -1;
}

bool ModelManager::GetAABBByNameWithOffset(char* szMeshName,
										   D3DXVECTOR3 trans,
										   D3DXMATRIX rot,
										   TAABB& outAABB, TAABB& outLocalAABB)
{
	char buffer[64];

	int namelength = strlen(szMeshName);
	int offset = 0;
	while(isdigit(szMeshName[namelength - offset]) || 
		szMeshName[namelength - offset] == '\0')
	{
		++offset;
	}
	memcpy(buffer, szMeshName, namelength - offset+1);
	buffer[namelength - offset+1] = '\0';
	string search(buffer);
	search += "_Collision";

	for(unsigned int i = 0; i < currAABBIndex; ++i)
	{
		if(strcmp(search.c_str(), m_pAABBBuffer[i].m_szName) == 0)
		{
			// Local Pos
			D3DXVECTOR4 rotmin(m_pAABBBuffer[i].aabb.cBoxMin, 0.0f);
			D3DXVECTOR4 rotmax(m_pAABBBuffer[i].aabb.cBoxMax, 0.0f);

			// Rotate min and max
			D3DXVec4Transform(&rotmin, &rotmin, &rot);
			D3DXVec4Transform(&rotmax, &rotmax, &rot);
			
			// Find new min and max
			D3DXVECTOR3 newmax(max(rotmin.x, rotmax.x),
								max(rotmin.y, rotmax.y),
								max(rotmin.z, rotmax.z) );

			D3DXVECTOR3 newmin(min(rotmin.x, rotmax.x),
								min(rotmin.y, rotmax.y),
								min(rotmin.z, rotmax.z) );

			// save for rendering
			outLocalAABB.cBoxMax = m_pAABBBuffer[i].aabb.cBoxMax;
			outLocalAABB.cBoxMin = m_pAABBBuffer[i].aabb.cBoxMin;

			// translate new min and max into worldspace for collision
			outAABB.cBoxMax = newmax + trans;
			outAABB.cBoxMin = newmin + trans;
			return true;
		}
	}
	return false;
}

DXMesh* ModelManager::CreateCubeFromAABB(TAABB aabb)
{
	// Get Singletons
	Direct3DManager* pD3DMan = Direct3DManager::GetInstance();

	DXMesh *rendercube = MMNEW(DXMesh);

	// Create Verts
		
		// Position
		VERTEX_POSCOLOR *verts = MMNEWARRAYEX(VERTEX_POSCOLOR,8,HEAPID_RENDER);
		verts[0].position = aabb.cBoxMin;
		verts[1].position = D3DXVECTOR3(aabb.cBoxMin.x, aabb.cBoxMax.y, aabb.cBoxMin.z);
		verts[2].position = D3DXVECTOR3(aabb.cBoxMax.x, aabb.cBoxMax.y, aabb.cBoxMin.z);
		verts[3].position = D3DXVECTOR3(aabb.cBoxMax.x, aabb.cBoxMin.y, aabb.cBoxMin.z);
		verts[4].position = D3DXVECTOR3(aabb.cBoxMin.x, aabb.cBoxMin.y, aabb.cBoxMax.z);
		verts[5].position = D3DXVECTOR3(aabb.cBoxMin.x, aabb.cBoxMax.y, aabb.cBoxMax.z);
		verts[6].position = aabb.cBoxMax;
		verts[7].position = D3DXVECTOR3(aabb.cBoxMax.x, aabb.cBoxMin.y, aabb.cBoxMax.z);

		// Color
		for(size_t uVert = 0; uVert < 8; ++uVert)
		{
			verts[uVert].color = D3DXCOLOR(255, 0, 0, 255);
		}

		// Create Vert Buffer
		rendercube->CreateVertexBuffer(verts, 8, pD3DMan->GetVertColorDecl(),
			sizeof(VERTEX_POSCOLOR), D3DPT_TRIANGLELIST);

	// Create Indices
		
		const int nNumCubeIdxs = 36;
		DWORD nCudeIdxs[nNumCubeIdxs] = 
		{
			// Front
			0, 1, 2,
			0, 2, 3,

			// Back
			7, 6, 5,
			7, 5, 4,

			// Left
			4, 5, 1,
			4, 1, 0,

			// Right
			6, 3, 2,
			6, 7, 3,

			// Top
			1, 5, 6,
			1, 6, 2,

			// Bottom
			3, 4, 0,
			3, 7, 4
		};

		// Create Idx Buffer
		rendercube->CreateIndexBuffer(nCudeIdxs, nNumCubeIdxs);

	return rendercube;
}