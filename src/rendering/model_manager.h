#ifndef _CMODELMANAGER_H_
#define _CMODELMANAGER_H_

#include <map>
#include <list>

#include "DXMesh.h"
#include "CAnimation.h"
#include "..\\..\\..\\CFrame.h"
#include "..\\..\\..\\Components\\Collision\\CCollideable.h"
#include "..\\Memory Manager\\CRenderAllocator.h"

class ModelManager
{
	struct NamedAABB
	{
		char	m_szName[64];
		TAABB	aabb;
	};

	ModelManager();
	ModelManager(ModelManager &ref);
	ModelManager &operator=(ModelManager &ref);

	// Map of Meshs (Key = ID of MeshName, Data = DXMesh)
	map<unsigned int, DXMesh*, less<unsigned int>, 
		CRenderAllocator<pair<unsigned int, DXMesh*>>> m_cMeshTemplates;

	// Map of Mesh Vertex Data (Key = ID of MeshName, Data = Vertex Data)
	map<unsigned int, TMeshVertexInfo*, less<unsigned int>,
		CRenderAllocator<pair<unsigned int, TMeshVertexInfo*>>> m_cVertexTemplates;

	// Map of Animations (Key = ID of AnimName, Data = Animation)
	map<unsigned int, CAnimation*, less<unsigned int>,
		CRenderAllocator<pair<unsigned int, CAnimation*>>> m_cAnimationTemplates;

	// Map of Clone Meshs (Key = Idx, Data = Clone Animation)
	list<DXMesh*, CRenderAllocator<DXMesh*>> m_cClonedBonedMeshs;

	unsigned int currIndex;
	unsigned int currAABBIndex;

	DXMesh* m_pBoneMesh;

	DXMesh m_pMeshBuffer[150];
	NamedAABB m_pAABBBuffer[75];

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

	int GetMeshIndexByName(const char* szMeshName, bool bIsCollision = false, bool stripNumber = true, char** szStripedName = NULL);
	bool GetAABBByNameWithOffset(char* szMeshName,
									D3DXVECTOR3 trans,
									scd::transform rot,
									TAABB& outAABB, TAABB& outLocalAABB);

	DXMesh* GetAnimMesh(string szMeshName);
	CAnimation* GetAnim(string szAnimName);
	CAnimation* GetAnim(unsigned int uAnimeID);

	//TAABB CreateAABB(D3DXVECTOR3* pPosInfo);

	// Collision Rect
	DXMesh* CreateCubeFromAABB(TAABB aabb);

	// Clones
	DXMesh* GetCloneMesh(string szMeshName);
	TMeshVertexInfo* GetCloneVerts(string szMeshName);

	// Init
	void Init(void);

	// Shutdown
	static void ShutdownCallback(IEvent*, IComponent*);
	void Shutdown(void);
};

#endif	// _CMODELMANAGER_H_