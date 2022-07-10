////////////////////////////////////////////////////////////////////////////////
//	File			:	animate.cpp
//	Date			:	5/9/11
//	Mod. Date		:	5/9/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object Animate
////////////////////////////////////////////////////////////////////////////////
#include "animate.h"

#include "core/object.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "rendering/dx_mesh.h"
#include "rendering/model_manager.h"

#include <iostream>

namespace scd {
// Constructor
animate::animate(CObject* pParent, DXMesh* pMesh)
    : m_pcParent(pParent)
    , m_pMesh(pMesh)
    , m_cInterpolator()
    , m_cSteerInterp()
    , m_pDefaultAnim(NULL)
    , m_bOn(true)
    , m_bSlowed(false)
    , m_bSteering(false)
    , m_bPrevSteer(false) {
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
  event_manager.register_event(szEventName, this, PauseUpdateCallback);
  szEventName = "Update";
  szEventName += CONSOLE_STATE;
  event_manager.register_event(szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += PAUSE_OPTIONS_STATE;
  event_manager.register_event(szEventName, this, PauseUpdateCallback);
  szEventName = "Update";
  szEventName += PAUSE_KEYBINDS_STATE;
  event_manager.register_event(szEventName, this, PauseUpdateCallback);

  // Reset
  szEventName = "InitObjects"; // first run
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, ActivateCallback);
  szEventName =
      "DisableObjects"; // someone pushed state on top of us (e.g. pause)
  szEventName += GAMEPLAY_STATE;
  // pEM->RegisterEvent(szEventName, this, DeactivateCallback);
  szEventName =
      "EnableObjects"; // state on top of us popped (e.g. pause resumed game)
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, ActivateCallback);
}

animate::~animate() { MMDEL(m_pMesh); }

// Handlers
void animate::Update(float fDT) {
  // If the Component is Off Do Not Update
  if (m_bOn == false || GetIsActive() == false)
    return;

  // If the Interpolator does not have a Valid Animation set it
  if (m_cInterpolator.GetAnimation() == NULL
      || (m_bSteering == false && m_bPrevSteer)) {
    int nDefaultID = CIDGen::GetInstance()->GetID("Default");
    unsigned int nNoID = CIDGen::GetInstance()->GetID("");
    AnimMap::iterator pAnimIter = m_cAnimationList.find(m_cCurrAnim.first);

    // Animation is not the default animation so we need to see if we are
    // playing the pre-anim or the loop anim
    if (pAnimIter != m_cAnimationList.end() && pAnimIter->second.second != nNoID
        && m_nCurrAnim == pAnimIter->second.first) {
      // We were playing the pre-anim so switch to loop
      m_nCurrAnim = pAnimIter->second.second;
    } else {
      // Currently playing the Default Animation
      pAnimIter = m_cAnimationList.find(nDefaultID);
      m_nCurrAnim = pAnimIter->second.first;
    }

    if (m_bSteering == false && m_bPrevSteer && !m_bSlowed) {
      // Currently playing the Default Animation
      pAnimIter = m_cAnimationList.find(nDefaultID);
      m_nCurrAnim = pAnimIter->second.first;
    }

    CAnimation* pAnim = ModelManager::GetInstance()->GetAnim(m_nCurrAnim);
    m_cInterpolator.SetAnimation(pAnim);
  }

  // Interpolate Between Frame
  m_cInterpolator.AddTime(fDT);
  m_cInterpolator.Process();

  // Skin the Mesh
  SkinMesh();

  // HACK: Add to set
  SendRenderEvent("AddToSet", this, this->m_pcParent, PRIORITY_UPDATE);
  m_bPrevSteer = m_bSteering;
  m_bSteering = false;
  m_bSlowed = false;
}

////////////////////////////////////////////////////////////////////////////////
// Callbacks
////////////////////////////////////////////////////////////////////////////////

// Update
void animate::UpdateCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Component
  animate* pAC = (animate*)pComp;

  // Get Data from Event
  TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
  float fDT = pUpdateEvent->m_fDeltaTime;

  // Call Handler
  pAC->Update(fDT);
}

// Switch Animation
void animate::HandleAnimationSwitchCallback(
    IEvent* pEvent,
    IComponent* pComp) {
  // Get the Component
  animate* pAC = (animate*)pComp;

  // Get the Parent's ID
  TInputEvent* pIE = (TInputEvent*)pEvent->GetData();

  //// So the correct player plays the right animation
  if (pAC->m_pcParent != pIE->m_pPlayer
      && ((char*)pAC->m_pcParent->GetID())[6]
             != ((char*)pIE->m_pPlayer->GetID())[6])
    return;

  // Get Event ID
  unsigned int uEventID = pEvent->GetEventID();

  unsigned int nSlowed = CIDGen::GetInstance()->GetID("SlowAnim");
  if (uEventID == nSlowed)
    pAC->m_bSlowed = true;
  else if (pAC->m_bSlowed)
    return; // Keep Trudging

  // Check to see if it's a steering animation
  unsigned int nSteerLeft = CIDGen::GetInstance()->GetID("SteerLeftAnim");
  unsigned int nSteerRight = CIDGen::GetInstance()->GetID("SteerRightAnim");
  if (uEventID == nSteerLeft || uEventID == nSteerRight) {
    if (pAC->m_bSlowed)
      return; // Don't steer if you're slowed
    else
      pAC->m_bSteering = true;
  }

  // Old Code
  //////////////////////////////////////////////////////////////////////////
  // Find the Animation to Switch to
  AnimMap::iterator animIter;
  animIter = pAC->m_cAnimationList.find(uEventID);
  if (pAC->m_cAnimationList.end() != animIter) {
    // We found a Match
    pair<unsigned int, unsigned int> pAnimIDs = animIter->second;
    pAC->m_cCurrAnim = *animIter;

    // Get the Animation
    CAnimation* pAnim = NULL;

    if (pAC->m_nCurrAnim == pAnimIDs.second
        || (pAC->m_nCurrAnim == pAnimIDs.first
            && pAC->m_cInterpolator.GetAnimation() == NULL)) {
      // cout << "Switching to secondary animation from HandleEvent\n";
      pAC->m_nCurrAnim = pAnimIDs.second;
      // cout << "Secondary Animation is " << (char*)pAC->m_nCurrAnim << endl;
    } else {
      // cout << "Using Primary Animation from HandleEvent\n";
      pAC->m_nCurrAnim = pAnimIDs.first;
      // cout << "Primary Animation is " << (char*)pAC->m_nCurrAnim << endl;
    }

    // Set the Animation
    pAnim = ModelManager::GetInstance()->GetAnim(pAC->m_nCurrAnim);
    pAC->m_cInterpolator.SetAnimation(pAnim);
    pAC->m_pCurrentAnimation = pAnim;
  }

  //////////////////////////////////////////////////////////////////////////
}

// Apply Skinning to thte Verts of the Mesh
void animate::SkinMesh(void) {
  // Get Original Vert Data
  TMeshVertexInfo* vertInfo =
      ModelManager::GetInstance()->GetCloneVerts(m_pMesh->GetName());

  // Get Bones
  // vector<TBindBone, CRenderAllocator<TBindBone>>;
  CDynArray<TBindBone>& pBindBones = m_pMesh->GetBones();

  // Get Frames
  FrameMap& pCurrFrames = m_cInterpolator.GetCurrentBoneFrames();

  // Get Influences
  InfluCont& pWeights = m_pMesh->GetWeights();

  // Create Verts
  VERTEX_POS3_NORM3_TEX2* verts =
      new VERTEX_POS3_NORM3_TEX2[m_pMesh->GetVertCount()];

  // Skin
  for (unsigned int nVert = 0; nVert < m_pMesh->GetVertCount(); ++nVert) {
    // Keep track of Final Position and Original Vert Position
    D3DXVECTOR3 vFinalPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 bindVert = vertInfo->m_vVertices[nVert];

    unsigned int nNumIf = pWeights[nVert].size();
    unsigned int nBoneIdx = 0;
    for (unsigned int nInf = 0; nInf < nNumIf; ++nInf) {
      // Inverse Bind Pose at Current Frame
      nBoneIdx = pWeights[nVert][nInf].m_nBoneIndex;
      scd::transform bindMat = pBindBones[nBoneIdx].m_cFrame.GetWorldMatrix();
      D3DXMatrixInverse(&bindMat, NULL, &bindMat);

      // Get the Interpolated Matrix
      scd::transform interMat = pCurrFrames[nBoneIdx].GetWorldMatrix();

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
    // memcpy(&verts[nVert], &vFinalPos, sizeof(D3DXVECTOR3));
  }

  // Reset Vert Buffer
  m_pMesh->ResetVertBuffer(
      verts, m_pMesh->GetVertCount(), sizeof(VERTEX_POS3_NORM3_TEX2));

  // Clean up Verts
  delete[] verts;
}

// Add to Map
void animate::AddAnimation(
    string szEventName,
    string szPreAnimationName,
    string szLoopAnimationName) {
  // Get the IDs
  CIDGen* pIDGen = CIDGen::GetInstance();
  unsigned int nEventID = pIDGen->GetID(szEventName);
  unsigned int nPreAnimID = pIDGen->GetID(szPreAnimationName);
  unsigned int nLoopAnimID = pIDGen->GetID(szLoopAnimationName);
  // Register for the event
  event_manager.register_event(nEventID, this, HandleAnimationSwitchCallback);

  // Insert into Map (This Overwrites if there is a Data Collision)
  m_cAnimationList[nEventID] = make_pair(nPreAnimID, nLoopAnimID);

  // Call this to load all the animation for this character as they're created
  CAnimation* pPreAnim =
      ModelManager::GetInstance()->GetAnim(szPreAnimationName);
  CAnimation* pLoopAnim =
      ModelManager::GetInstance()->GetAnim(szLoopAnimationName);
  pPreAnim->SetLooping(false);
  if (pLoopAnim)
    pLoopAnim->SetLooping(true);

  // Set Default Animation
  if (szEventName == "Default") {
    this->GetInterpolator().SetAnimation(pPreAnim);
    m_pDefaultAnim = pPreAnim;
  }
}

// On/Off
void animate::ActivateCallback(IEvent*, IComponent* pComp) {
  // Get the Effect Comp
  animate* pAC = (animate*)pComp;
  pAC->m_bOn = true;
}
void animate::DeactivateCallback(IEvent*, IComponent* pComp) {
  // Get the Effect Comp
  animate* pAC = (animate*)pComp;
  pAC->m_bOn = false;
}

void animate::PauseUpdateCallback(IEvent*, IComponent* pComp) {
  // Get the Effect Comp
  animate* pAC = (animate*)pComp;

  // return if it's not active
  if (pAC->m_bOn == false || pAC->GetIsActive() == false)
    return;

  SendRenderEvent("AddToSet", pAC, pAC->m_pcParent, PRIORITY_UPDATE);
}
} // namespace scd
