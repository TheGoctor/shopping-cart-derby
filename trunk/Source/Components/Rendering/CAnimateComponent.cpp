////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateComponent.cpp
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object Animate
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
using std::cout;

// Includes
#include "CAnimateComponent.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\DXMesh.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\ModelManager.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include"..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
#include "..\\..\\CObject.h"
using namespace EventStructs;

// Constructor
CAnimateComponent::CAnimateComponent(CObject* pParent, DXMesh* pMesh,
									 Interpolator cInterpolator)
	: m_pcParent(pParent), m_pMesh(pMesh), m_cInterpolator(cInterpolator),
	m_pDefaultAnim(NULL), m_bOn(true)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();

	// Update
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	szEventName = "Update";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	szEventName = "Update";
	szEventName += PAUSE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);
	szEventName = "Update";
	szEventName += CONSOLE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);

	szEventName = "Update";
	szEventName += PAUSE_OPTIONS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);
	szEventName = "Update";
	szEventName += PAUSE_KEYBINDS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);


	// Reset
	szEventName = "InitObjects"; // first run
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, ActivateCallback);
	szEventName = "DisableObjects"; // someone pushed state on top of us (e.g. pause)
	szEventName += GAMEPLAY_STATE;
	//pEM->RegisterEvent(szEventName, this, DeactivateCallback);
	szEventName = "EnableObjects"; // state on top of us popped (e.g. pause resumed game)
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, ActivateCallback);
}

// Handlers
void CAnimateComponent::Update(float fDT)
{
	// If the Component is Off Do Not Update
	if(m_bOn == false || GetIsActive() == false)
		return;

	// If the Interpolator does not have a Valid Animation set it
	if(m_cInterpolator.GetAnimation() == NULL)
	{
		int nDefaultID = CIDGen::GetInstance()->GetID("Default");
		unsigned int nNoID = CIDGen::GetInstance()->GetID("");
		AnimMap::iterator pAnimIter = m_cAnimationList.find(m_cCurrAnim.first);
		if(pAnimIter != m_cAnimationList.end() && pAnimIter->second.second != nNoID && 
			m_nCurrAnim == pAnimIter->second.first)
		{
			// Animation is not the default animation so we need to see if we are playing
			// the pre-anim or the loop anim

				// We were playing the pre-anim so switch to loop
				m_nCurrAnim = pAnimIter->second.second;
				//cout << "Switching to secondary animation from Update\n";
		}
		else
		{

			//if(m_nCurrAnim != 0)
			//	cout << "Current Animation is " << (char*)m_nCurrAnim << endl;
			//else
			//	cout << "No Current Animation\n";

			//cout << "Switching Animation to Default\n";

			// Currently playing the Default Animation
			pAnimIter = m_cAnimationList.find(nDefaultID);
			m_nCurrAnim = pAnimIter->second.first;

		}

		CAnimation* pAnim = ModelManager::GetInstance()->GetAnim(m_nCurrAnim);
		m_cInterpolator.SetAnimation(pAnim);
	}

	//if(m_nCurrAnim)
	//	cout << endl << "Current Animation " << (char*)m_nCurrAnim <<endl << endl;

	// Interpolate Between Frame
	m_cInterpolator.AddTime(fDT);
	m_cInterpolator.Process();

	//if(m_cInterpolator.GetAnimation() == NULL)
	//	cout << "Current Animation is now NULL\n";

	// Skin the Mesh
	SkinMesh();

	// HACK: Add to set
	SendRenderEvent("AddToSet", this, this->m_pcParent, PRIORITY_UPDATE);
}

////////////////////////////////////////////////////////////////////////////////
// Callbacks
////////////////////////////////////////////////////////////////////////////////

// Update
void CAnimateComponent::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component
	CAnimateComponent* pAC = (CAnimateComponent*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Call Handler
	pAC->Update(fDT);
}

// Switch Animation
void CAnimateComponent::HandleAnimationSwitchCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Component
	CAnimateComponent* pAC = (CAnimateComponent*)pComp;

	// Get the Parent's ID
	TInputEvent *pIE = (TInputEvent*)pEvent->GetData();
	//char* pParentName = (char*)pAC->m_pcParent->GetID();

	//// So the correct player plays the right animation
	//if(pParentName[6] != pIE->m_nPlayer + '0')
	//	return;
	if(pAC->m_pcParent != pIE->m_pPlayer && ((char*)pAC->m_pcParent->GetID())[6]
		!= ((char*)pIE->m_pPlayer->GetID())[6])
		return;

	// Get Event ID
	unsigned int uEventID = pEvent->GetEventID();

	// Old Code
	//////////////////////////////////////////////////////////////////////////
	// Find the Animation to Switch to
	AnimMap::iterator animIter;
	animIter = pAC->m_cAnimationList.find(uEventID);
	if(pAC->m_cAnimationList.end() != animIter)
	{
		// We found a Match
		pair<unsigned int, unsigned int> pAnimIDs = animIter->second;
		pAC->m_cCurrAnim = *animIter;

		// Get the Animation
		CAnimation* pAnim = NULL;

		if(pAC->m_nCurrAnim == pAnimIDs.second || (pAC->m_nCurrAnim == pAnimIDs.first && pAC->m_cInterpolator.GetAnimation() == NULL))
		{
			//cout << "Switching to secondary animation from HandleEvent\n";
			pAC->m_nCurrAnim = pAnimIDs.second;
			//cout << "Secondary Animation is " << (char*)pAC->m_nCurrAnim << endl;
		}
		else
		{
			//cout << "Using Primary Animation from HandleEvent\n";
			pAC->m_nCurrAnim = pAnimIDs.first;
			//cout << "Primary Animation is " << (char*)pAC->m_nCurrAnim << endl;
		}
		// Set the Animation
		pAnim = ModelManager::GetInstance()->GetInstance()->GetAnim(pAC->m_nCurrAnim);
		pAC->m_cInterpolator.SetAnimation(pAnim);
		pAC->m_pCurrentAnimation = pAC->m_cInterpolator.GetAnimation();
	}

	//////////////////////////////////////////////////////////////////////////

	// New Code
	//////////////////////////////////////////////////////////////////////////

	//	// Switch from pre animation to loop animation when pre animation finishes
	//	if(NULL == pAC->m_cInterpolator.GetAnimation() &&
	//		pAC->m_pCurrentAnimation == pPreAnim)
	//	{
	//		pAC->m_cInterpolator.SetAnimation(pLoopAnim);
	//		m_nCurrAnim = pAnimIDs.second;
	//	}
	//	else
	//	{
	//		m_nCurrAnim = pAnimIDs.first;
	//	}

	//	// Loop the loop animation
	//	if(pAC->m_pCurrentAnimation == pLoopAnim)
	//	{
	//		pAC->m_cInterpolator.SetAnimation(pLoopAnim);
	//		m_nCurrAnim = pAnimIDs.second;
	//	}
	//	else
	//	{
	//		m_nCurrAnim = pAnimIDs.first;
	//	}

	//////////////////////////////////////////////////////////////////////////
}

// Apply Skinning to thte Verts of the Mesh
void CAnimateComponent::SkinMesh(void)
{
	// Get Original Vert Data
	TMeshVertexInfo* vertInfo = ModelManager::GetInstance()->GetCloneVerts(m_pMesh->GetName());

	// Get Bones
	vector<TBindBone>& pBindBones = m_pMesh->GetBones();

	// Get Frames
	FrameMap& pCurrFrames = m_cInterpolator.GetCurrentBoneFrames();

	// Get Influences
	vector<vector<TBoneInfluence>>& pWeights = m_pMesh->GetWeights();

	// Create Verts
	VERTEX_POS3_NORM3_TEX2 *verts = new VERTEX_POS3_NORM3_TEX2[m_pMesh->GetVertCount()];

	// Skin
	for(unsigned int nVert = 0; nVert < m_pMesh->GetVertCount(); ++nVert)
	{
		// Keep track of Final Position and Original Vert Position
		D3DXVECTOR3 vFinalPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 bindVert = vertInfo->m_vVertices[nVert];

		int nNumIf = pWeights[nVert].size();
		int nBoneIdx = 0;
		for(int nInf = 0; nInf < nNumIf; ++nInf)
		{
			// Inverse Bind Pose at Current Frame
			nBoneIdx = pWeights[nVert][nInf].m_nBoneIndex;
			D3DXMATRIX bindMat = pBindBones[nBoneIdx].m_cFrame.GetWorldMatrix();
			D3DXMatrixInverse(&bindMat, NULL, &bindMat);

			// Get the Interpolated Matrix
			D3DXMATRIX interMat = pCurrFrames[nBoneIdx].GetWorldMatrix();

			// Apply both Matrices
			D3DXVECTOR3 vNewPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			D3DXVec3TransformCoord(&vNewPos, &bindVert, &bindMat);
			D3DXVec3TransformCoord(&vNewPos, &vNewPos, &interMat);

			// Apply Weight
			vNewPos *= pWeights[nVert][nInf].m_fWeight;

			// Add to Final Position
			vFinalPos += vNewPos;
		}

		// Set Final Position
		verts[nVert].position.x = vFinalPos.x;
		verts[nVert].position.y = vFinalPos.y;
		verts[nVert].position.z = vFinalPos.z;
		
		// Copy over UV Data
		verts[nVert].uv.x = vertInfo->m_vUV[nVert].x;
		verts[nVert].uv.y = vertInfo->m_vUV[nVert].y;
		//memcpy(&verts[nVert], &vFinalPos, sizeof(D3DXVECTOR3));
	}

	// Reset Vert Buffer
	m_pMesh->ResetVertBuffer(verts, m_pMesh->GetVertCount(), sizeof(VERTEX_POS3_NORM3_TEX2));

	// Clean up Verts
	delete [] verts;
}

// Add to Map
void CAnimateComponent::AddAnimation(string szEventName, string szPreAnimationName, string szLoopAnimationName)
{
	// Get the IDs
	CIDGen* pIDGen = CIDGen::GetInstance();
	unsigned int nEventID = pIDGen->GetID(szEventName);
	unsigned int nPreAnimID  = pIDGen->GetID(szPreAnimationName);
	unsigned int nLoopAnimID = pIDGen->GetID(szLoopAnimationName);
	// Register for the event
	CEventManager::GetInstance()->RegisterEvent(nEventID, this, HandleAnimationSwitchCallback);

	// Insert into Map (This Overwrites if there is a Data Collision)
	m_cAnimationList[nEventID] = make_pair(nPreAnimID, nLoopAnimID);

	// Call this to load all the animation for this character as they're created
	CAnimation* pPreAnim = ModelManager::GetInstance()->GetAnim(szPreAnimationName);
	/*CAnimation* pLoopAnim =*/ ModelManager::GetInstance()->GetAnim(szLoopAnimationName);

	// Set Default Animation
	if(szEventName == "Default")
	{
		this->GetInterpolator().SetAnimation(pPreAnim);
		m_pDefaultAnim = pPreAnim;
	}
}

// On/Off
void CAnimateComponent::ActivateCallback(IEvent*, IComponent* pComp)
{
	// Get the Effect Comp
	CAnimateComponent* pAC = (CAnimateComponent*)pComp;
	pAC->m_bOn = true;
}
void CAnimateComponent::DeactivateCallback(IEvent*, IComponent* pComp)
{
	// Get the Effect Comp
	CAnimateComponent* pAC = (CAnimateComponent*)pComp;
	pAC->m_bOn = false;
}


void CAnimateComponent::PauseUpdateCallback(IEvent*, IComponent* pComp)
{
	// Get the Effect Comp
	CAnimateComponent* pAC = (CAnimateComponent*)pComp;

	// return if it's not active
	if(pAC->m_bOn == false || pAC->GetIsActive() == false)
		return;

	
	SendRenderEvent("AddToSet", pAC, pAC->m_pcParent, PRIORITY_UPDATE);
	
}


