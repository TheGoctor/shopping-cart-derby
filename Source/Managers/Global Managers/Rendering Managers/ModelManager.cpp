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
		//VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[num];
		VERTEX_POSCOLOR *verts = new VERTEX_POSCOLOR[num];
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
			
			verts[i].color = D3DCOLOR_ARGB(255,rand()%255,rand()%255,rand()%255);
		}

		this->m_pMeshBuffer[currIndex].CreateVertexBuffer(verts, num, 
			Direct3DManager::GetInstance()->GetVertColorDecl(),
			sizeof(VERTEX_POSCOLOR),
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

int ModelManager::GetMeshIndexByName(char* szMeshName)
{
	char buffer[40];
	memset(buffer, 0, 40);
	int namelength = strlen(szMeshName);
	int offset = 0;
	while(isdigit(szMeshName[namelength - offset]) || 
		szMeshName[namelength - offset] == '\0')
	{
		++offset;
	}
	memcpy(buffer, szMeshName, namelength - offset+1);
	buffer[namelength - offset + 1] = '\0';

	for(unsigned int i = 0; i < currIndex; ++i)
	{
		if(strcmp(buffer, m_pMeshBuffer[i].GetName()) == 0)
		{
			return i;
		}
	}
	return -1;
}
