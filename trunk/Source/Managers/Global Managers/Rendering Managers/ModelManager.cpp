#include "ModelManager.h"
#include "Direct3DManager.h"
#include "..\\Event Manager\\CIDGen.h"
#include <iostream>
#include <fstream>
using namespace std;

// Don't use for arrays and only use when fin is your fstream
#define READ(var) (fin.read((char*)&var, sizeof(var)))

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
	//// Map of Meshs (Key = ID of MeshName, Data = DXMesh)
	//map<unsigned int, DXMesh*>::iterator meshIter = m_cMeshTemplates.begin();
	//while(meshIter != m_cMeshTemplates.end())
	//{
	//	MMDEL(DXMesh, meshIter->second);

	//	meshIter++;
	//}

	//// Map of Mesh Vertex Data (Key = ID of MeshName, Data = Vertex Data)
	//map<unsigned int, TMeshVertexInfo*> m_cVertexTemplates;

	//// Map of Animations (Key = ID of AnimName, Data = Animation)
	//map<unsigned int, CAnimation*> m_cAnimationTemplates;


	//// Map of Clone Meshs (Key = Idx, Data = Clone Animation)
	//map<unsigned int, DXMesh*> m_cCloneAnimations;

	//// Map of Mesh Vertex Data (Key = ID of MeshName, Data = Vertex Data)
	//map<unsigned int, TMeshVertexInfo*> m_cCloneVertex;

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

int ModelManager::LoadModelWithBones(char* szFileName)
{
	//read String Size
	//read Name of Model
	//read Loop size
	//read Vertices
	//read Normals
	//read Text COORDS
	//read Triangles
	//unsigned int num = 0;
	//char buffer[128];
	//memset(buffer, 0, 128);

	// Get Singletons
	Direct3DManager* pD3DMan = Direct3DManager::GetInstance();
	CIDGen* pIDGen = CIDGen::GetInstance();

	fstream fin(szFileName, ios_base::in | ios_base::binary);

	if(fin.is_open())
	{
		D3DXVECTOR3 *pos_buffer;
		D3DXVECTOR3 *norm_buffer;
		D3DXVECTOR2	*uv_buffer;
		DWORD		*indicies;

		// Create Mesh
		DXMesh* pMesh = MMNEW(DXMesh) DXMesh();
		TMeshVertexInfo* pVertInfo = MMNEW(TMeshVertexInfo) TMeshVertexInfo();

		// Get Mesh name
		char szMeshName[64];
		unsigned int nNameLen = 0;
		fin.read((char*)&nNameLen, sizeof(nNameLen));
		fin.read(szMeshName, nNameLen);

		strcpy(pMesh->m_szName,szMeshName);
		// Copy szMeshName to m_pMeshBuffer[currIndex].m_szName
		//memcpy(m_pMeshBuffer[currIndex].m_szName, szMeshName, 64);

		// Get Vert Count
		unsigned int nVertCount;
		READ(nVertCount);

		// Get Poly Count
		unsigned int nPolyCount;
		READ(nPolyCount);

		// Get Texture Data ?

		// Set up Vertex Buffers
		pos_buffer = new D3DXVECTOR3[nVertCount];
		norm_buffer = new D3DXVECTOR3[nVertCount];
		uv_buffer = new D3DXVECTOR2[nVertCount];

		// Get Vertex Data
		pMesh->GetWeights().resize(nVertCount);
		for(unsigned int nVert = 0; nVert < nVertCount; nVert++)
		{
			// Position Data
			READ(pos_buffer[nVert].x);
			READ(pos_buffer[nVert].y);
			READ(pos_buffer[nVert].z);

			// Normal Data
			READ(norm_buffer[nVert].x);
			READ(norm_buffer[nVert].y);
			READ(norm_buffer[nVert].z);

			// UV Data
			READ(uv_buffer[nVert].x);
			READ(uv_buffer[nVert].y);

			// Influence Data
			// Get NumInfluences
			unsigned int nNumInfluences;
			READ(nNumInfluences);
			pMesh->GetWeights()[nVert].resize(nNumInfluences);

			for(unsigned int nInfl = 0; nInfl < nNumInfluences; nInfl++)
			{
				TBoneInfluence bi;

				// Get Joint Index
				unsigned int nJointIndx;
				READ(nJointIndx);
				bi.m_nBoneIndex = nJointIndx;

				// vert weight
				float fVertWeight;
				READ(fVertWeight);
				bi.m_fWeight = fVertWeight;

				//pVertInfo->m_vWeights.push_back(bi);
				pMesh->SetWeight(nVert, bi);
			}
		}
		VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[nVertCount];

		for(unsigned int i = 0; i < nVertCount; ++i)
		{
			verts[i].position.x = pos_buffer[i].x;
			verts[i].position.y = pos_buffer[i].y;
			verts[i].position.z = -pos_buffer[i].z;

			pVertInfo->m_vVertices.push_back(verts[i].position);

			verts[i].normal = norm_buffer[i];

			pVertInfo->m_vNormals.push_back(verts[i].normal);

			verts[i].uv.x = uv_buffer[i].x;
			verts[i].uv.y = -uv_buffer[i].y;

			pVertInfo->m_vUV.push_back(verts[i].uv);
		}

		// Create Vertex Buffer
		/*pMesh->CreateVertexBuffer(verts, nVertCount,
			pD3DMan->GetVertNormTex2DDecl(), sizeof(VERTEX_POS3_NORM3_TEX2),
			pMesh->GetMode());*/



		// Get Polys
		indicies = new DWORD[nPolyCount*3];
		fin.read((char*)indicies, sizeof(DWORD)*nPolyCount*3);

		//pVertInfo->m_vIndices.resize(nPolyCount*3);
		//memcpy(pVertInfo->m_vIndices, indicies, sizeof(DWORD)*nPolyCount*3);
		pMesh->SetVertexInfo(pVertInfo);

		// Create Index Buffer
		pMesh->CreateIndexBuffer(indicies, nVertCount*3);
		

		// Get Bone Count
		unsigned int nBoneCount;
		READ(nBoneCount);

		// Get Bone Data
		for(unsigned int nBone = 0; nBone < nBoneCount; nBone++)
		{
			// Create Bone
			TBindBone tBone;

			// Get Bone Name
			unsigned int nBoneNameLen;
			READ(nBoneNameLen);
			char nBoneName[64];
			fin.read(nBoneName, nBoneNameLen);
			tBone.m_szBoneName = nBoneName;

			// Get Current Bone
			unsigned int nCurrBone;
			READ(nCurrBone);
			tBone.m_nBoneIdx = nCurrBone;

			// Get Parent Index
			unsigned int nParentIndx;
			READ(nParentIndx);
			tBone.m_nParentBoneIdx = nParentIndx;

			// Get Local Matrix
			D3DXMATRIX mLocalMatrix;
			// Local X Axis
			READ(mLocalMatrix._11);
			READ(mLocalMatrix._12);
			READ(mLocalMatrix._13);
			READ(mLocalMatrix._14);
			
			// Local Y Axis
			READ(mLocalMatrix._21);
			READ(mLocalMatrix._22);
			READ(mLocalMatrix._23);
			READ(mLocalMatrix._24);
			
			// Local Z Axis
			READ(mLocalMatrix._31);
			READ(mLocalMatrix._32);
			READ(mLocalMatrix._33);
			READ(mLocalMatrix._34);
			
			// Local W Axis
			READ(mLocalMatrix._41);
			READ(mLocalMatrix._42);
			READ(mLocalMatrix._43);
			READ(mLocalMatrix._44);

			// Create Frame
			CFrame cFrame;
			cFrame.GetLocalMatrix() = mLocalMatrix;
			tBone.m_cFrame = cFrame;

			// Get Child Count
			unsigned int nChildCount;
			READ(nChildCount);
			tBone.m_nNumChildren = nChildCount;

			// Get Children
			for(unsigned int nChild = 0; nChild < nChildCount; nChild++)
			{
				// Get Child Index (Joint Index)
				unsigned int nChildIndx;
				READ(nChildIndx);
				tBone.m_cChildrenIdxs.push_back(nChildIndx);
				//tBone.m_cChildrenIdxs[nChild] = nChildIndx;

				// Get Child Name Length
				unsigned int nChildNameLen;
				READ(nChildNameLen);

				// Get Child Name
				char szChildName[64];
				fin.read(szChildName, nChildNameLen);

				int x = 0;
			}

			//pVertInfo->m_vJoints.push_back(tBone);
			pMesh->AddBoneToHiearchy(tBone);
		}

		fin.close();
		fin.clear();

		// Build Hierarchy
		pMesh->BuildHiearchy();


		// Add to Map
		m_cMeshTemplates.insert(make_pair(pIDGen->GetID(szMeshName), pMesh));
		m_cVertexTemplates.insert(make_pair(pIDGen->GetID(szMeshName), pVertInfo));

		// Clean up
		delete [] pos_buffer;
		delete [] norm_buffer;
		delete [] uv_buffer;
		delete [] verts;
		delete [] indicies;

		//MMDEL(DXMesh, pMesh);
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

		//switch(largestSide)
		//{
		//case 0:
		//	//			m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.z *= -1;
		//	m_pAABBBuffer[currAABBIndex].aabb.cBoxMax -= D3DXVECTOR3(0,0,zLen*2.0f);
		//	m_pAABBBuffer[currAABBIndex].aabb.cBoxMin -= D3DXVECTOR3(0,0,zLen*2.0f);
		//	break;
		//case 1:
		//	m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.y *= -1;
		//	break;
		//case 2:
		//	m_pAABBBuffer[currAABBIndex].aabb.cBoxMax.z *= -1;
		//	break;
		//};

		delete [] pos_buffer;
	}
	else
		return -1;

	currAABBIndex++;
	return currAABBIndex - 1;
}

//TAABB ModelManager::CreateAABB(D3DXVECTOR3* pPosInfo)
//{
//
//}

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

int ModelManager::LoadAnimFile(char* szFileName)
{
	// Get Singletons
	CIDGen* pIDGen = CIDGen::GetInstance();

	// Open a FileStream
	fstream fin(szFileName, ios_base::in | ios_base::binary);

	if(fin.is_open())
	{
		// Get Anim Name
		char szAnimName[64];
		int nNameLen = 0;
		READ(nNameLen);
		fin.read(szAnimName, nNameLen);

		// Get Duration
		float fDuration;
		READ(fDuration);

		// Get Number of Bones
		int nNumBones;
		READ(nNumBones);

		// Create Anim Struct
		CAnimation* pAnimData = MMNEW(CAnimation) CAnimation();
		pAnimData->SetDuration(fDuration);
		pAnimData->SetNumBones(nNumBones);

		// Create Bones
		//pAnimData->m_pBones = MMNEWARRAY(TBone, nNumBones, HEAPID_RENDER);

		// Get Bone Info
		for(int bone = 0; bone < nNumBones; ++bone)
		{
			// Create Bone
			TBone tBone;

			// Get Bone Idx
			int nBoneIdx;
			READ(nBoneIdx);

			// Get Number of KeyFrames
			int nNumKeyFrames;
			READ(nNumKeyFrames);

			// Update Bones
			tBone.m_nBoneIdx = nBoneIdx;
			tBone.m_nNumKeyFrames = nNumKeyFrames;

			// Get KeyFrame Info
			for(int nFrame = 0; nFrame < nNumKeyFrames; ++nFrame)
			{
				// Get KeyFrame Idx
				TKeyFrame tKeyFrame;
				
				// Get Key Time
				float fKeyTime;
				READ(fKeyTime);
				tKeyFrame.m_fKeyTime = fKeyTime;

				// Get Local Matrix
				D3DXMATRIX mLocalMatrix;
				D3DXMATRIX mWorldMatrix;
				fin.read((char*)&mLocalMatrix, sizeof(mLocalMatrix));
				fin.read((char*)&mWorldMatrix, sizeof(mWorldMatrix));

				// Update KeyFrame
				tKeyFrame.m_cFrame.GetLocalMatrix() = mLocalMatrix;
				tBone.m_cKeyFrames.push_back(tKeyFrame);
			}

			// Add Bone
			pAnimData->AddBone(tBone);
		}

		// Calc Tween Times
		pAnimData->SetupTweenTimes();

		// Add to Map
		m_cAnimationTemplates.insert(make_pair(pIDGen->GetID(szAnimName),
			pAnimData));

		// Close and Clear the FileStream
		SAFE_CLOSE(fin);

		//MMDEL(CAnimation, pAnimData);
	}

	return -1;
}

// Animate Bones
void ModelManager::SetUpBoneMesh(void)
{
	// Get Singletons
	Direct3DManager* pD3DMan = Direct3DManager::GetInstance();

	// Bone Info
	//for(int i = 0; i < 10; ++i)
	//{
	//	// set up the weights
	//	if( i == 0 || i == 8 || i == 2 || i == 4 || i == 6 ) //  i know this is stupid
	//	{
	//		TBoneInfluence inf;
	//		inf.m_fWeight = 1.0f;
	//		inf.m_nBoneIndex = 0;
	//		m_pBoneMesh->SetWeight(i, inf);
	//	}
	//	else
	//	{
	//		TBoneInfluence inf;
	//		inf.m_fWeight = 1.0f;
	//		inf.m_nBoneIndex = 1;
	//		m_pBoneMesh->SetWeight(i, inf);
	//	}
	//}
}

DXMesh* ModelManager::GetAnimMesh(string szMeshName)
{
	return (m_cMeshTemplates.find(CIDGen::GetInstance()->GetID(szMeshName)))->second;
}

CAnimation* ModelManager::GetAnim(string szAnimName)
{
	return (m_cAnimationTemplates.find(CIDGen::GetInstance()->GetID(szAnimName)))->second;
}

DXMesh* ModelManager::GetCloneMesh(string szMeshName)
{
	// Create a Clone
	DXMesh* pCloneMesh = MMNEW(DXMesh) DXMesh();

	// Find Original Mesh
	DXMesh* pOriginalMesh = (m_cMeshTemplates.find(CIDGen::GetInstance()->GetID(szMeshName)))->second;

	// Clone
	*pCloneMesh = *pOriginalMesh;

	// Return the Clone
	return pOriginalMesh;
}

TMeshVertexInfo* ModelManager::GetCloneVerts(string szMeshName)
{
	// Create a Clone
	//TMeshVertexInfo pCloneVerts;// = MMNEW(TMeshVertexInfo) TMeshVertexInfo();

	// Find Original Verts
	TMeshVertexInfo* pOriginalVerts = (m_cVertexTemplates.find(CIDGen::GetInstance()->GetID(szMeshName)))->second;

	// Clone
	//pCloneVerts = *pOriginalVerts;

	// Return the Clone
	return pOriginalVerts;
}


#undef READ