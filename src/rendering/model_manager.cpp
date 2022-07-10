#include "ModelManager.h"
#include "Direct3DManager.h"
#include "..\\Event Manager\\CEventManager.h"
#include "..\\Event Manager\\CIDGen.h"
#include <iostream>
#include <fstream>
using namespace std;

// Don't use for arrays and only use when fin is your fstream
#define READ(var) (fin.read((char*)&var, sizeof(var)))

ModelManager* ModelManager::GetInstance()
{
	static ModelManager cModelManager;
	return &cModelManager;
}

ModelManager::ModelManager()
{
	currIndex = 0;
}

void ModelManager::Init(void)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("Shutdown", (IComponent*)GetInstance(), ShutdownCallback);
}

// Shutdown
void ModelManager::ShutdownCallback(IEvent*, IComponent*)
{
	GetInstance()->Shutdown();
}
void ModelManager::Shutdown(void)
{
	// Meshs
	map<unsigned int, DXMesh*, less<unsigned int>, CRenderAllocator<pair
		<unsigned int, DXMesh*>>>::iterator meshIter = m_cMeshTemplates.begin();
	while(meshIter != m_cMeshTemplates.end())
	{
		MMDEL(meshIter->second);

		meshIter++;
	}
	m_cMeshTemplates.clear();

	// Vertex Data
	map<unsigned int, TMeshVertexInfo*, less<unsigned int>, CRenderAllocator
		<pair<unsigned int, TMeshVertexInfo*>>>::iterator vertIter 
		= m_cVertexTemplates.begin();
	while(vertIter != m_cVertexTemplates.end())
	{
		MMDEL(vertIter->second);

		vertIter++;
	}
	m_cVertexTemplates.clear();
	
	// Animations
	map<unsigned int, CAnimation*, less<unsigned int>, CRenderAllocator
		<pair<unsigned int, CAnimation*>>>::iterator animIter 
		= m_cAnimationTemplates.begin();
	while(animIter != m_cAnimationTemplates.end())
	{
		MMDEL(animIter->second);

		animIter++;
	}
	m_cAnimationTemplates.clear();

	// Clone Meshs
	list<DXMesh*, CRenderAllocator<DXMesh*>>::iterator cloneIter 
		= m_cClonedBonedMeshs.begin();
	while(cloneIter != m_cClonedBonedMeshs.end())
	{
		MMDEL( (*cloneIter) );

		cloneIter++;
	}
	m_cClonedBonedMeshs.clear();
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
		D3DXVECTOR2 *uv_buffer;
		DWORD  *indicies;

		


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


		TMeshVertexInfo* pVertInfo;
		bool bStoringVerts = (strcmp(m_pMeshBuffer[currIndex].m_szName, "FFP_3D_CheckoutCollision_FIN") == 0);
		if( bStoringVerts )
		{
			pVertInfo = MMNEW(TMeshVertexInfo);
		}

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
			verts[i].position.z = pos_buffer[i].z;

			verts[i].normal = norm_buffer[i];
			verts[i].uv.x = uv_buffer[i].x;
			verts[i].uv.y = -uv_buffer[i].y;

			if( bStoringVerts )
			{
				pVertInfo->m_vUV.push_back(verts[i].uv);
				pVertInfo->m_vVertices.push_back(verts[i].position);
				pVertInfo->m_vNormals.push_back(verts[i].normal);
			}


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

		if( bStoringVerts )
		{
			m_cVertexTemplates.insert(make_pair(CIDGen::GetInstance()->GetID(m_pMeshBuffer[currIndex].m_szName), pVertInfo));
		}


		delete [] pos_buffer;
		delete [] norm_buffer;
		delete [] uv_buffer;
		delete [] verts;
		delete [] indicies;

		fin.close();
		fin.clear();
	}
	else
	{
		MessageBoxA(NULL, szFileName, "Failed to load static mesh", MB_OK);
		return -1;
	}

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
	//Direct3DManager* pD3DMan = Direct3DManager::GetInstance();
	CIDGen* pIDGen = CIDGen::GetInstance();

	fstream fin(szFileName, ios_base::in | ios_base::binary);

	if(fin.is_open())
	{
		D3DXVECTOR3 *pos_buffer;
		D3DXVECTOR3 *norm_buffer;
		D3DXVECTOR2	*uv_buffer;
		DWORD		*indicies;

		// Create Mesh
		DXMesh* pMesh = MMNEW(DXMesh);
		TMeshVertexInfo* pVertInfo = MMNEW(TMeshVertexInfo);

		// Get Mesh name
		char szMeshName[64];
		unsigned int nNameLen = 0;
		fin.read((char*)&nNameLen, sizeof(nNameLen));
		fin.read(szMeshName, nNameLen);

		strcpy_s(pMesh->m_szName,szMeshName);
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
			//pMesh->GetWeights()[nVert].resize(2);

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

				// If its 0 Weigh trow it out
				if(bi.m_fWeight == 0.0f)
					continue;

				//pVertInfo->m_vWeights.push_back(bi);
				pMesh->SetWeight(nVert, bi);
			}

			//pMesh->SizeWeightsToTwo(nVert);
		}
		VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[nVertCount];

		for(unsigned int i = 0; i < nVertCount; ++i)
		{
			verts[i].position.x = pos_buffer[i].x;
			verts[i].position.y = pos_buffer[i].y;
			verts[i].position.z = pos_buffer[i].z;

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

		for(unsigned int idx = 0; idx < nPolyCount*3; ++idx)
		{
			pVertInfo->m_vIndices.push_back(indicies[idx]);
		}

		//pVertInfo->m_vIndices.resize(nPolyCount*3);
		//memcpy(pVertInfo->m_vIndices, indicies, sizeof(DWORD)*nPolyCount*3);
		pMesh->SetIndexedTexturedVertexInfo(pVertInfo);//, true);

		// Create Index Buffer
		//pMesh->CreateIndexBuffer(indicies, nPolyCount*3);
		

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
			scd::transform mLocalMatrix;
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

			//mLocalMatrix._31 *= -1;
			//mLocalMatrix._32 *= -1;
			//mLocalMatrix._33 *= -1;
			////mLocalMatrix._34 *= -1;

			
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
			//tBone.m_cChildrenIdxs.resize(nChildCount);
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

				//int x = 0;
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
	{
		MessageBoxA(NULL, szFileName, "Failed to load animated mesh", MB_OK);
		return -1;
	}


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
	{
		MessageBoxA(NULL, szMeshName, "Failed to load AABB", MB_OK);
		return -1;
	}

	currAABBIndex++;
	return currAABBIndex - 1;
}

//TAABB ModelManager::CreateAABB(D3DXVECTOR3* pPosInfo)
//{
//
//}

int ModelManager::GetMeshIndexByName(const char* szMeshName, bool bIsCollision, bool stripNumber, char** szStripedName)
{
	char buffer[64];
	memset(buffer, 0, 64);
	int namelength = strlen(szMeshName);
	int offset = 0;

	if(stripNumber)
	{
		// Strip the Numbers from the End of the Mesh Name
		while(isdigit(szMeshName[namelength - offset]) || 
			szMeshName[namelength - offset] == '\0')
		{
			++offset;
		}
	}

	memcpy(buffer, szMeshName, namelength - offset+1);
	buffer[namelength - offset + 1] = '\0';

	// Return Striped Name
	/*if(szStripedName)
	{
		*szStripedName = buffer;
	}*/

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
										   scd::transform rot,
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
	VERTEX_POSCOLOR *verts = MMNEWARRAYEX(VERTEX_POSCOLOR, 8, HEAPID_RENDER);
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
			verts[uVert].color = scd::vector4(255, 0, 0, 255);
		}

		// Create Vert Buffer
		rendercube->CreateVertexBuffer(verts, 8, pD3DMan->GetVertColorDecl(),
			sizeof(VERTEX_POSCOLOR), D3DPT_TRIANGLELIST);

		MMDELARRAYEX(verts, HEAPID_RENDER);
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
		CAnimation* pAnimData = MMNEW(CAnimation);
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
				scd::transform mLocalMatrix;
				scd::transform mWorldMatrix;
				fin.read((char*)&mLocalMatrix, sizeof(mLocalMatrix));

				//mLocalMatrix._31 *= -1;
				//mLocalMatrix._32 *= -1;
				//mLocalMatrix._33 *= -1;
				////mLocalMatrix._34 *= -1;

				fin.read((char*)&mWorldMatrix, sizeof(mWorldMatrix));

				// Update KeyFrame
				tKeyFrame.m_cFrame.GetLocalMatrix() = mLocalMatrix;
				tBone.m_cKeyFrames.push_back(tKeyFrame);
			}

			// Add Bone
			pAnimData->AddBone(tBone);
		}

		// Calc Tween Times
		//pAnimData->SetupTweenTimes();

		// Add to Map
		m_cAnimationTemplates.insert(make_pair(pIDGen->GetID(szAnimName),
			pAnimData));

		// Close and Clear the FileStream
		SAFE_CLOSE(fin);

		//MMDEL(CAnimation, pAnimData);

		return 1; // SUCCESS!
	}

	if(szFileName[20] != 0)
		MessageBoxA(NULL, szFileName, "Failed to load animation file", MB_OK);
	return 0; // Error
}

DXMesh* ModelManager::GetAnimMesh(string szMeshName)
{
	//LoadModelWithBones
	map<unsigned int, DXMesh*, less<unsigned int>, CRenderAllocator
		<pair<unsigned int, DXMesh*>>>::iterator pMeshIter;
	int nMeshID = CIDGen::GetInstance()->GetID(szMeshName);
	pMeshIter = m_cMeshTemplates.find(nMeshID);
	if(pMeshIter != m_cMeshTemplates.end())
		return pMeshIter->second;

	// Mesh is not already loaded, try to load it
	string szMeshPath = "Resource/Character Models/";
	szMeshPath += szMeshName;
	szMeshPath += ".mesh";
	if(LoadModelWithBones((char*)szMeshPath.c_str()) != -1)
		return m_cMeshTemplates.find(nMeshID)->second;
	else
		return NULL; // Mesh File does not exist
}

CAnimation* ModelManager::GetAnim(string szAnimName)
{
	map<unsigned int, CAnimation*, less<unsigned int>, CRenderAllocator
		<pair<unsigned int, CAnimation*>>>::iterator pAnimIter;
	int nAnimID = CIDGen::GetInstance()->GetID(szAnimName);
	pAnimIter = m_cAnimationTemplates.find(nAnimID);
	if(pAnimIter != m_cAnimationTemplates.end())
		return pAnimIter->second;

	// Animation is not already loaded, try to load it
	string szAnimPath = "Resource/Animations/";
	szAnimPath += szAnimName;
	if(LoadAnimFile((char*)szAnimPath.c_str()))
		return m_cAnimationTemplates.find(nAnimID)->second;
	else
		return NULL; // Anim File does not exist
	
}

CAnimation* ModelManager::GetAnim(unsigned int nAnimID)
{
	map<unsigned int, CAnimation*, less<unsigned int>, CRenderAllocator
		<pair<unsigned int, CAnimation*>>>::iterator pAnimIter;
	pAnimIter = m_cAnimationTemplates.find(nAnimID);
	if(pAnimIter != m_cAnimationTemplates.end())
		return pAnimIter->second;

	// Animation is not already loaded, try to load it
	string szAnimPath = "Resource/Animations/";
	szAnimPath += (char*)nAnimID;
	if(LoadAnimFile((char*)szAnimPath.c_str()))
		return m_cAnimationTemplates.find(nAnimID)->second;
	else
		return NULL; // Anim File does not exist
}

DXMesh* ModelManager::GetCloneMesh(string szMeshName)
{
	// Find Original Mesh
	DXMesh* pOriginalMesh = GetAnimMesh(szMeshName);

	if(pOriginalMesh == NULL)
		return NULL;

	// Create a Clone
	DXMesh* pCloneMesh = MMNEW(DXMesh);


	// Find Original Verts
	TMeshVertexInfo* pOriginalVerts = (m_cVertexTemplates.find(CIDGen::GetInstance()->GetID(szMeshName)))->second;

	// Verts
	pCloneMesh->SetIndexedTexturedVertexInfo(pOriginalVerts);//, true);

	// Mesh Name
	int nMeshNameLen = szMeshName.size();
	for(int nChar = 0; nChar < nMeshNameLen+1; ++nChar)
	{
		pCloneMesh->m_szName[nChar] = szMeshName[nChar];
	}

	// Bones
	int nNumBones = pOriginalMesh->GetBones().size();
	for(int bone = 0; bone < nNumBones; ++bone)
	{
		TBindBone tBone;
		tBone.m_nBoneIdx = pOriginalMesh->GetBones()[bone].m_nBoneIdx;
		tBone.m_cFrame.CloneLocal(pOriginalMesh->GetBones()[bone].m_cFrame);
		tBone.m_nParentBoneIdx = pOriginalMesh->GetBones()[bone].m_nParentBoneIdx;
		
		int nStrLen = pOriginalMesh->GetBones()[bone].m_szBoneName.size();
		tBone.m_szBoneName.resize(nStrLen+1);
		for(int nChar = 0; nChar < nStrLen+1; ++nChar)
		{
			tBone.m_szBoneName[nChar] = pOriginalMesh->GetBones()[bone].m_szBoneName[nChar];
		}
		
		tBone.m_nNumChildren = pOriginalMesh->GetBones()[bone].m_nNumChildren;
		tBone.m_cChildrenIdxs.resize(tBone.m_nNumChildren);
		for(int child = 0; child < tBone.m_nNumChildren; ++child)
		{
			tBone.m_cChildrenIdxs[child] = pOriginalMesh->GetBones()[bone].m_cChildrenIdxs[child];
		}

		pCloneMesh->AddBoneToHiearchy(tBone);
	}

	pCloneMesh->BuildHiearchy();

	// Weights
	int nNumWeights = pOriginalMesh->GetWeights().size();
	pCloneMesh->GetWeights().resize(nNumWeights);
	for(int weight = 0; weight < nNumWeights; ++weight)
	{
		int nNumInf = pOriginalMesh->GetWeights()[weight].size();
		pCloneMesh->GetWeights()[weight].resize(nNumInf);
		for(int nInf = 0; nInf < nNumInf; ++nInf)
		{
			TBoneInfluence bi;
			bi.m_nBoneIndex = pOriginalMesh->GetWeights()[weight][nInf].m_nBoneIndex;
			bi.m_fWeight = pOriginalMesh->GetWeights()[weight][nInf].m_fWeight;

			pCloneMesh->SetWeight(weight, bi);
		}
	}

	// Clone
	//m_cClonedBonedMeshs.push_back(pCloneMesh);
	//*pCloneMesh = *pOriginalMesh;

	// Return the Clone
	return pCloneMesh;
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