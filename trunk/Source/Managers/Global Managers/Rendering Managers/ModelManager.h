#ifndef _CMODELMANAGER_H_
#define _CMODELMANAGER_H_

#include <map>

#include "DXMesh.h"
#include "CAnimation.h"
#include "..\\..\\..\\CFrame.h"
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



	// Map of Meshs (Key = ID of MeshName, Data = DXMesh)
	map<unsigned int, DXMesh*> m_cMeshTemplates;

	// Map of Mesh Vertex Data (Key = ID of MeshName, Data = Vertex Data)
	map<unsigned int, TMeshVertexInfo*> m_cVertexTemplates;

	// Map of Animations (Key = ID of AnimName, Data = Animation)
	map<unsigned int, CAnimation*> m_cAnimationTemplates;


	// Map of Clone Meshs (Key = Idx, Data = Clone Animation)
	map<unsigned int, DXMesh*> m_cCloneAnimations;

	// Map of Mesh Vertex Data (Key = ID of MeshName, Data = Vertex Data)
	//map<unsigned int, TMeshVertexInfo*> m_cCloneVertex;

	unsigned int currIndex;
	unsigned int currAABBIndex;

	DXMesh* m_pBoneMesh;

	DXMesh m_pMeshBuffer[50];
	NamedAABB m_pAABBBuffer[50];

public:

	static ModelManager* GetInstance(void);
	~ModelManager();

	DXMesh* GetMesh(unsigned int index) 
	{ 
		return &m_pMeshBuffer[index]; 
	}

	int LoadModel(char* szFileName);
	int LoadModelWithBones(char* szFileName);
	int LoadAABB(char* szMeshName);

	int LoadAnimFile(char* szFileName);

	int GetMeshIndexByName(const char* szMeshName, bool bIsCollision = false);
	bool GetAABBByNameWithOffset(char* szMeshName, D3DXVECTOR3 offset,
		TAABB& outAABB, TAABB& outLocalAABB);

	DXMesh* GetAnimMesh(string szMeshName);
	CAnimation* GetAnim(string szAnimName);

	//TAABB CreateAABB(D3DXVECTOR3* pPosInfo);

	// Collision Rect
	DXMesh* CreateCubeFromAABB(TAABB aabb);

	// Animate Bones
	void SetUpBoneMesh(void);

	// Clones
	DXMesh* GetCloneMesh(string szMeshName);
	TMeshVertexInfo* GetCloneVerts(string szMeshName);
};

#endif	// _CMODELMANAGER_H_