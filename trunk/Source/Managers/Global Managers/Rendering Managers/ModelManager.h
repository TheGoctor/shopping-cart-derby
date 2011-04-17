#ifndef _CMODELMANAGER_H_
#define _CMODELMANAGER_H_

#include "DXMesh.h"

class ModelManager
{
	static ModelManager m_pInstance;
	ModelManager();
	ModelManager(ModelManager &ref);
	ModelManager &operator=(ModelManager &ref);

	DXMesh m_pMeshBuffer[50];
	unsigned int currIndex;

public:

	static ModelManager* GetInstance(void);
	~ModelManager();

	DXMesh* GetMesh(unsigned int index) 
	{ 
		return &m_pMeshBuffer[index]; 
	}

	int LoadModel(char* szFileName);

	int GetMeshIndexByName(char* szMeshName);

};

#endif	// _CMODELMANAGER_H_