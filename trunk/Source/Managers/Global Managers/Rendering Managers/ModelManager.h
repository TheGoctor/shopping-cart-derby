#ifndef _CMODELMANAGER_H_
#define _CMODELMANAGER_H_

#include "DXMesh.h"
#include "..\\..\\..\\Components\\Collision\\CCollideable.h"

class ModelManager
{
	struct NamedAABB
	{
		char	m_szName[64];
		TAABB	aabb;
	};

	static ModelManager m_pInstance;
	ModelManager();
	ModelManager(ModelManager &ref);
	ModelManager &operator=(ModelManager &ref);

	DXMesh m_pMeshBuffer[50];
	NamedAABB m_pAABBBuffer[50];

	unsigned int currIndex;
	unsigned int currAABBIndex;

public:

	static ModelManager* GetInstance(void);
	~ModelManager();

	DXMesh* GetMesh(unsigned int index) 
	{ 
		return &m_pMeshBuffer[index]; 
	}

	int LoadModel(char* szFileName);
	int LoadAABB(char* szMeshName);

	int GetMeshIndexByName(const char* szMeshName, bool bIsCollision = false);
	bool GetAABBByNameWithOffset(char* szMeshName,
										   D3DXVECTOR3 trans,
										   D3DXMATRIX rot,
										   TAABB& outAABB, TAABB& outLocalAABB);
	//TAABB CreateAABB(D3DXVECTOR3* pPosInfo);

	DXMesh* CreateCubeFromAABB(TAABB aabb);

};

#endif	// _CMODELMANAGER_H_