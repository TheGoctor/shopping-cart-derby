/***********************************************
 * Filename:  		ai_component.cpp
 * Date:      		04/04/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This is the AI's main class
      which is responsable for changing
      the AI's state based on weights.
          The AI will have three main
      states: Defensive, Collective, and
      Aggressive. Each state will dertermine
      how the AI behaves in each situation.
 ************************************************/
#include "ai_component.h"

#include "components/ai_manager.h"
#include "components/inventory.h"
#include "components/inventory_manager.h"
#include "components/level/goal_item.h"
#include "components/level/held_item.h"
#include "components/pickup_manager.h"
#include "components/rendering/renderable.h"
#include "components/spawning_manager.h"
#include "core/base_component.h"
#include "core/object.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "input/input_manager.h"
#include "physics/physics_manager.h"
#include "rendering/dx_mesh.h"
#include "rendering/renderer.h"

#define MIN_SPEED (7)
#define USE_ITEM_TIMER (rand() % 26 + 25)
#define TURKEY_TO_PLAYER (20.0f)

namespace {
float Magnitude(scd::vector3 pt) {
  return sqrtf((pt.x * pt.x) + (pt.y * pt.y) + (pt.z * pt.z));
}

float AngleBetweenVectors(scd::vector3 pt1, scd::vector3 pt2) {
  float fDotProduct = D3DXVec3Dot(&pt1, &pt2);

  float fMagnitude = Magnitude(pt1) * Magnitude(pt2);

  if (fMagnitude == 0.0f) {
    return 0.0f;
  }

  float fAngle = acos(fDotProduct / fMagnitude);

  if (_isnan(fAngle)) {
    return 0.0f;
  }

  return fAngle;
}

float DegreesToRadians(const float fDegrees) {
  return fDegrees * (3.14159265f / 180.0f);
}
} // anonymous namespace

namespace scd {
// Hack cuz we don't have
// time to fix it
// Yay!!
int nNumber = 0;
ai_component::ai_component(scd::object* pObj)
    : m_pObject(pObj)
    , m_pMesh(NULL)
    , m_pRendComp(NULL)
    , m_bShutdown(false) {}

ai_component::~ai_component() {
  // Delete all memory being used by this component

  while (m_tKnowledge.m_cGoalItemsInLevel.empty() == false) {
    map<unsigned int,
        TGoalItem,
        less<unsigned int>,
        scd::allocator<pair<unsigned int, TGoalItem>>>::iterator pIter =
        m_tKnowledge.m_cGoalItemsInLevel.begin();

    MyDelete<TGoalItem>(&pIter->second, HEAPID_GENERAL);

    m_tKnowledge.m_cGoalItemsInLevel.erase(
        m_tKnowledge.m_cGoalItemsInLevel.begin());
  }

  while (m_tKnowledge.m_cNeededGoalItems.empty() == false) {
    map<unsigned int,
        EGoalItemType,
        less<unsigned int>,
        scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator pIter =
        m_tKnowledge.m_cNeededGoalItems.begin();

    MyDelete<EGoalItemType>(&pIter->second, HEAPID_GENERAL);

    m_tKnowledge.m_cNeededGoalItems.erase(
        m_tKnowledge.m_cNeededGoalItems.begin());
  }

  for (unsigned nIndex = 0; nIndex < 3; ++nIndex) {
    while (m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.empty() == false) {
      map<unsigned int,
          EGoalItemType,
          less<unsigned int>,
          scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator pIter =
          m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin();

      MyDelete<EGoalItemType>(&pIter->second, HEAPID_GENERAL);

      m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.erase(
          m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin());
    }
  }
}

void ai_component::InitCB(IEvent*, scd::base_component* pComp) {
  // Reinit all agents

  ai_component* cAIComp = (ai_component*)pComp;

  cAIComp->m_pfWeight[COLLECTIVE] = 0;
  cAIComp->m_pfWeight[AGGRESSIVE] = 0;
  cAIComp->m_pfWeight[DEFENSIVE] = 0;

  cAIComp->m_eCurrentState = NOT_INIT;

  cAIComp->m_tKnowledge.m_cGoalItemsInLevel.clear();
  cAIComp->m_tKnowledge.m_peMyHeldItems[0] = NO_HELD_ITEM;
  cAIComp->m_tKnowledge.m_peMyHeldItems[1] = NO_HELD_ITEM;
  cAIComp->m_tKnowledge.m_cNeededGoalItems.clear();
  cAIComp->m_tKnowledge.m_cHeldItemsInLevel.clear();
  cAIComp->m_tKnowledge.m_nDistanceFromGoal = 0;
  cAIComp->m_tKnowledge.m_nOpponentID = 0;

  list<EGoalItemType, scd::allocator<EGoalItemType>>::iterator pItemIter =
      CAIManager::GetInstance()->m_cInitedGoalItems.begin();

  while (pItemIter != CAIManager::GetInstance()->m_cInitedGoalItems.end()) {
    cAIComp->m_tKnowledge.m_cNeededGoalItems.insert(
        make_pair(unsigned int(*pItemIter), *pItemIter));
    ++pItemIter;
  }

  for (unsigned nPlayers = 0; nPlayers < 3; ++nPlayers) {
    cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_bHasItem = false;
    cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_bTarget = false;
    cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.clear();
    cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_chAmtGoalItems = 0;
    cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_nItem = -1;
    cAIComp->m_tKnowledge.m_cOpponents[nPlayers].m_nAggression = 0;
  }

  cAIComp->m_fTimer = 1.0f;

  cAIComp->m_vTargetPosition = scd::vector3(0, 0, -65535);

  cAIComp->m_nControllerNumber = ++nNumber;

  cAIComp->m_nTargetTriangle = -1;
  cAIComp->m_nCurrentTriangle = -1;

  cAIComp->m_tTargetPlayerLine.cRayNormal = scd::vector3(65535, 65535, 65535);

  cAIComp->CreateMeshTriangle();
  cAIComp->bDrawTris = false;
  cAIComp->bLogical = true;

  cAIComp->m_pMiniGoalTri = NULL;
  cAIComp->fThreshold[0] = 0.5f;
  cAIComp->fThreshold[1] = 0.8f;

  cAIComp->m_fUseItemTimer = static_cast<float>(USE_ITEM_TIMER);
  cAIComp->m_fTurkeyToPlayer = TURKEY_TO_PLAYER;

  // Alert anyone who is waiting on AI
  // that AI is now up and running
  SendIEvent("AIInited", cAIComp, NULL, PRIORITY_RENDER);
}

void ai_component::Init() {
  // Register for all events needed

  CEventManager* pEM = CEventManager::GetInstance();

  string szEvent = "Update";
  string szEvent2 = "InitObjects";
  string szEvent3 = "ShutdownObjects";
  szEvent += GAMEPLAY_STATE;
  szEvent2 += GAMEPLAY_STATE;
  szEvent3 += GAMEPLAY_STATE;

  pEM->RegisterEvent("PlayerCharacterSet", this, PlayerCreated);
  pEM->RegisterEvent(szEvent, this, Update);
  pEM->RegisterEvent(szEvent2, this, InitCB);
  pEM->RegisterEvent(szEvent3, this, Shutdown);
  pEM->RegisterEvent("GoalItemCollected", this, GoalItemCollected);
  pEM->RegisterEvent("PickupItemCollected", this, PickupItemCollected);
  pEM->RegisterEvent("HeldItemSpawned", this, HeldItemSpawned);
  pEM->RegisterEvent("HeldItemCollected", this, HeldItemCollected);
  pEM->RegisterEvent("GoalItemSpawned", this, GoalItemSpawned);
  pEM->RegisterEvent("GoalItemDespawned", this, GoalItemDespawned);
  pEM->RegisterEvent("SpeedValue", this, UpdateSpeeds);
  pEM->RegisterEvent("ItemDropped", this, ItemDropped);
  pEM->RegisterEvent("ToggleAI", this, ToggleAI);
  pEM->RegisterEvent("ToggleTris", this, ToggleTris);
  pEM->RegisterEvent("ShowRamSpeed", this, SetRamSpeed);
  pEM->RegisterEvent("ShowTurnSpeed", this, ShowTurnSpeed);
  pEM->RegisterEvent("ShowAcceleration", this, ShowAcceleration);
  pEM->RegisterEvent("StealItem", this, ItemStealing);
  pEM->RegisterEvent("Blind", this, Blind);
  pEM->RegisterEvent("UseJam", this, Jammed);

  string szEventName = "Update";
  szEventName += PAUSE_STATE;
  pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += CONSOLE_STATE;
  pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += PAUSE_KEYBINDS_STATE;
  pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += PAUSE_OPTIONS_STATE;
  pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += QUIT_CONFIRMATION_STATE;
  pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += IN_GAME_HOW_TO_PLAY_STATE;
  pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);
  szEventName = "Update";
  szEventName += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
  pEM->RegisterEvent(szEvent, this, PauseUpdateCallback);
}

void ai_component::PlayerCreated(IEvent* pEvent, scd::base_component* pComp) {
  // Function is used for ToggleInfo event
  // Allows us to display the name of each
  // agent to the screen

  TTwoIntEvent* pTwoInts = (TTwoIntEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  if (pcAI->m_nControllerNumber != pTwoInts->m_nInt1) {
    // Not us
    return;
  }

  pcAI->nAnimation = pTwoInts->m_nInt2;
}

void ai_component::Shutdown(IEvent*, scd::base_component* pComp) {
  // Shutdown this agent

  ai_component* pcAI = (ai_component*)pComp;

  if (pcAI->m_bShutdown)
    return; // Don't shutdown the same thing twice

  map<unsigned int,
      TGoalItem,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, TGoalItem>>>::iterator
      pGoalItemLevelIter;
  while (pcAI->m_tKnowledge.m_cGoalItemsInLevel.empty() == false) {
    pGoalItemLevelIter = pcAI->m_tKnowledge.m_cGoalItemsInLevel.begin();

    MyDelete<TGoalItem>(&pGoalItemLevelIter->second, HEAPID_GENERAL);

    pcAI->m_tKnowledge.m_cGoalItemsInLevel.erase(
        pcAI->m_tKnowledge.m_cGoalItemsInLevel.begin());
  }

  while (pcAI->m_tKnowledge.m_cNeededGoalItems.empty() == false) {
    map<unsigned int,
        EGoalItemType,
        less<unsigned int>,
        scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator pIter =
        pcAI->m_tKnowledge.m_cNeededGoalItems.begin();

    MyDelete<EGoalItemType>(&pIter->second, HEAPID_GENERAL);

    pcAI->m_tKnowledge.m_cNeededGoalItems.erase(
        pcAI->m_tKnowledge.m_cNeededGoalItems.begin());
  }

  for (unsigned nIndex = 0; nIndex < 3; ++nIndex) {
    map<unsigned int,
        EGoalItemType,
        less<unsigned int>,
        scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator pIter;
    while (pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.empty()
           == false) {
      pIter = pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin();

      MyDelete<EGoalItemType>(&pIter->second, HEAPID_GENERAL);

      pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.erase(
          pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin());
    }
  }

  map<unsigned int,
      THeldItem,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, THeldItem>>>::iterator
      pHeldItemsLevelIter;
  while (pcAI->m_tKnowledge.m_cHeldItemsInLevel.empty() == false) {
    pHeldItemsLevelIter = pcAI->m_tKnowledge.m_cHeldItemsInLevel.begin();

    MyDelete<THeldItem>(&pHeldItemsLevelIter->second, HEAPID_GENERAL);

    pcAI->m_tKnowledge.m_cHeldItemsInLevel.erase(
        pcAI->m_tKnowledge.m_cHeldItemsInLevel.begin());
  }

  map<unsigned int,
      EGoalItemType,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator
      pDroppedItemsIter;
  while (pcAI->m_tKnowledge.m_cDroppedItems.empty() == false) {
    pDroppedItemsIter = pcAI->m_tKnowledge.m_cDroppedItems.begin();

    MyDelete<EGoalItemType>(&pDroppedItemsIter->second, HEAPID_GENERAL);

    pcAI->m_tKnowledge.m_cDroppedItems.erase(
        pcAI->m_tKnowledge.m_cDroppedItems.begin());
  }

  MMDEL(pcAI->m_pMesh);
  MMDEL(pcAI->m_pRendComp);
  pcAI->m_bShutdown = true;
}

// Events Start
void ai_component::ItemStealing(IEvent* pEvent, scd::base_component* pComp) {
  // A donut was used and someone was hit

  TImpactEvent* pImpactEvent = (TImpactEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  if (pcAI->m_pObject->GetID() == pImpactEvent->m_pcCollider->GetID()) {
    // We're the ones who is stealing an item
    CInventoryComponent* pCompToStealFrom =
        CInventoryManager::GetInstance()->GetInventoryComponent(
            pImpactEvent->m_pCollidedWith);

    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        if (pcAI->m_tKnowledge.m_peMyHeldItems[i] == NO_HELD_ITEM) {
          if (pCompToStealFrom->GetHeldItem(j) != NO_HELD_ITEM) {
            pcAI->m_tKnowledge.m_peMyHeldItems[i] =
                pCompToStealFrom->GetHeldItem(j);
          }
        }
      }
    }
  } else if (
      pcAI->m_pObject->GetID() == pImpactEvent->m_pCollidedWith->GetID()) {
    // We're losing the item
    for (int i = 0; i < 2; ++i) {
      if (pcAI->m_tKnowledge.m_peMyHeldItems[i] != NO_HELD_ITEM) {
        pcAI->m_tKnowledge.m_peMyHeldItems[i] = NO_HELD_ITEM;
        return;
      }
    }
  }
}

void ai_component::ItemDropped(
    IEvent* piEvent,
    scd::base_component* piComponent) {
  // A Goal Item was knocked out of someone's cart

  ai_component* pcAI = (ai_component*)piComponent;
  TGoalItemCollectedEvent* pcGoalItem =
      (TGoalItemCollectedEvent*)piEvent->GetData();

  // Get the type
  unsigned int pType = (unsigned int)pcGoalItem->m_pcGoalItem;

  // Insert the type into the DroppedItems map
  pcAI->m_tKnowledge.m_cDroppedItems.insert(
      make_pair(pType, static_cast<EGoalItemType>(pType)));

  if (pcAI->m_pObject->GetID() == pcGoalItem->m_pcCollector->GetID()) {
    // If we dropped the item
    pcAI->m_tKnowledge.m_cNeededGoalItems.insert(
        make_pair(pType, static_cast<EGoalItemType>(pType)));
  } else // If an opponent dropped the item
  {
    // Find which opponent
    int nIndex = -1;
    int i;
    for (i = 0; i < 3; ++i) {
      if (pcAI->m_tKnowledge.m_cOpponents[i].m_nPlayer
          == pcGoalItem->m_pcCollector->GetID()) {
        nIndex = i;
        break;
      }
    }

    // Decrease how many Goal Items they have
    map<unsigned int,
        EGoalItemType,
        less<unsigned int>,
        scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator ItemIter =
        pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.find(pType);

    // Remove the goal item from their collected goal item map
    pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.erase(ItemIter);

    // Adjust the AmtGoalItems
    pcAI->m_tKnowledge.m_cOpponents[nIndex].m_chAmtGoalItems =
        pcAI->m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.size();
  }
}

void ai_component::SetupOpponents(scd::object* pcObject) {
  // If not me
  if (m_pObject->GetID() != pcObject->GetID()) {
    // Remember this player
    m_tKnowledge.m_cOpponents[m_tKnowledge.m_nOpponentID].m_nPlayer =
        pcObject->GetID();
    ++m_tKnowledge.m_nOpponentID;
  }
}

void ai_component::ToggleAI(IEvent*, scd::base_component* pComp) {
  // Toggles if the agent does certain things
  pComp->ToggleIsActive();
}

void ai_component::ToggleTris(IEvent*, scd::base_component* pComp) {
  // Toggles the rendering of the nav mesh
  ai_component* pcAI = (ai_component*)pComp;
  pcAI->bDrawTris = !pcAI->bDrawTris;
}

bool ai_component::Use(EHeldItemType eType) {
  // Loop through and use the passed in item
  // if it is contained in our inventory
  for (int nItem = 0; nItem < 2; ++nItem) {
    if (m_tKnowledge.m_peMyHeldItems[nItem] == eType) {
      if (nItem == 0)
        SendInputEvent("UseHeldItem1", this, m_pObject, 1.0f);
      else
        SendInputEvent("UseHeldItem2", this, m_pObject, 1.0f);

      m_tKnowledge.m_peMyHeldItems[nItem] = NO_HELD_ITEM;
      return true;
    }
  }

  return false;
}

CRenderComponent* ai_component::CreateMeshTriangle() {
  string szObjName = "meshTri";
  scd::object* pObj =
      scd::objectManager::GetInstance()->CreateObject(szObjName);

  // Store both
  m_pMesh = MMNEW(DXMesh);
  m_pRendComp = Renderer::GetInstance()->CreateRenderComp(
      pObj, m_pMesh, RC_FLAT, RF_VERT);

  return m_pRendComp;
}

void ai_component::StartDraw() {
  if (bDrawTris == false) {
    return;
  }

  TMeshVertexInfo& pVertInfo = m_pMesh->GetVertInfo();
  // Add verts to mesh to be drawn
  pVertInfo.m_vVertices.clear();
  pVertInfo.m_vColors.clear();
}

void ai_component::EndDraw() {
  if (bDrawTris == false) {
    return;
  }

  TMeshVertexInfo& pVertInfo = m_pMesh->GetVertInfo();
  // Render all lines and tris... and maybe spheres or w/e
  int count = pVertInfo.m_vVertices.size();
  for (int v = 0; v < count; v++) {
    pVertInfo.m_vVertices[v].y = 0.1f;
  }
  m_pMesh->SetColoredTexturedVertexInfo(&pVertInfo);
  m_pRendComp->AddToRenderSet();
}

void ai_component::DrawTriangle(
    scd::vector3 vVert0,
    scd::vector3 vVert1,
    scd::vector3 vVert2,
    scd::vector4 cColor) {
  if (bDrawTris == false) {
    return;
  }

  TMeshVertexInfo& pVertInfo = m_pMesh->GetVertInfo();
  D3DCOLOR color = cColor;

  pVertInfo.m_vVertices.push_back(vVert0);
  pVertInfo.m_vColors.push_back(color);
  pVertInfo.m_vUV.push_back(D3DXVECTOR2(0.0f, 0.0f));

  pVertInfo.m_vVertices.push_back(vVert1);
  pVertInfo.m_vColors.push_back(color);
  pVertInfo.m_vUV.push_back(D3DXVECTOR2(0.0f, 0.0f));

  pVertInfo.m_vVertices.push_back(vVert2);
  pVertInfo.m_vColors.push_back(color);
  pVertInfo.m_vUV.push_back(D3DXVECTOR2(0.0f, 0.0f));
}

void ai_component::DrawTriangle(TTri* tTri, scd::vector4 cColor) {
  DrawTriangle(
      tTri->tEdges[0].cLineStart,
      tTri->tEdges[1].cLineStart,
      tTri->tEdges[2].cLineStart,
      cColor);
}

void ai_component::DrawTriangle(TTriangle* tTri, scd::vector4 cColor) {
  DrawTriangle(tTri->tVert0, tTri->tVert1, tTri->tVert2, cColor);
}

void ai_component::DrawLine(
    scd::vector3 vStart,
    scd::vector3 vEnd,
    scd::vector4 cColor) {
  DrawTriangle(vStart, vEnd, vEnd, cColor);
}

void ai_component::Update(IEvent* piEvent, scd::base_component* piComponent) {
  // nNumber is a HACK
  nNumber = 0;
  ai_component* pcAI = (ai_component*)piComponent;
  CAIManager* cAIManager = CAIManager::GetInstance();
  CCollisionManager* cCollision = CCollisionManager::GetInstance();
  TUpdateStateEvent* pcUpdate = (TUpdateStateEvent*)piEvent->GetData();

  SendRenderEvent("AddToSet", pcAI, pcAI->m_pObject, PRIORITY_IMMEDIATE);

  if (CInputManager::GetInstance()->GetState() == GAMEPLAY_STATE) {
    if (pcAI->GetIsActive()) {
      pcAI->m_fBlindTimer -= pcUpdate->m_fDeltaTime;
      pcAI->m_fJamTimer -= pcUpdate->m_fDeltaTime;
      pcAI->m_fTimer += pcUpdate->m_fDeltaTime;

      if (pcAI->m_bRandomItems == true) {
        pcAI->m_fUseItemTimer -= pcUpdate->m_fDeltaTime;
        pcAI->m_fTurkeyToPlayer -= pcUpdate->m_fDeltaTime;
      }
    }

    // Get agent's position offset by the heading
    scd::vector3 Heading;
    Heading.x = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._31;
    Heading.y = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._32;
    Heading.z = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._33;
    scd::vector3 myPosition =
        pcAI->m_pObject->GetTransform()->GetWorldPosition();
    myPosition += Heading * 0.5f;

    // Go to checkout behavior
    if (pcAI->m_tKnowledge.m_cNeededGoalItems.size() == 0) {
      pcAI->m_eCurrentState = NOT_INIT;
      pcAI->m_vGoalPosition = scd::vector3(0, 0, 68);
    } else {
      if (pcAI->m_fJamTimer < 0) {
        // If not jammed update quickly
        if (pcAI->m_fTimer > 0.4f) {
          pcAI->m_fTimer = 0.0f;
          pcAI->EvaluateStateWeights();
        }
      } else {
        // We're jammed so update slower
        if (pcAI->m_fTimer > 4.0f) {
          pcAI->m_fTimer = 0.0f;
          pcAI->EvaluateStateWeights();
        }
      }
    }

    pcAI->StartDraw();

    // Declare/Initiliaze
    int nHeadingEdge = -1;
    int nAStarEdge = -1;
    int nMiniGoalTriID = -1;
    bool bCurFound = false;
    bool bTarFound = false;
    TTriangle tri;
    scd::vector3 ToGoal;
    scd::vector3 GoalPosition;

    // Find GoalPosition, CurrentTriangle and TargetTriangle
    map<int, TTri*, less<int>, scd::allocator<pair<int, TTri*>>>::iterator
        pFindIter = cAIManager->m_cTris.begin();
    tri.tNorm = scd::vector3(0, 1, 0);
    while (pFindIter != cAIManager->m_cTris.end()) {
      if (bCurFound == true && bTarFound == true) {
        break;
      }

      if (cAIManager->GetDistance(
              pFindIter->second->tPos, pcAI->m_vGoalPosition)
          < 144) {
        // Finding TargetTriangle
        tri.tVert0 = pFindIter->second->tEdges[0].cLineStart;
        tri.tVert1 = pFindIter->second->tEdges[1].cLineStart;
        tri.tVert2 = pFindIter->second->tEdges[2].cLineStart;

        if (cCollision->PointInTriangle(tri, pcAI->m_vGoalPosition) == true) {
          pcAI->DrawTriangle(
              tri.tVert0, tri.tVert1, tri.tVert2, scd::vector4(255, 0, 0, 255));
          pcAI->DrawLine(
              myPosition,
              pFindIter->second->tPos,
              scd::vector4(0, 0, 255, 255));
          pcAI->m_nTargetTriangle = pFindIter->first;

          // Set goal for later
          GoalPosition = pFindIter->second->tPos;
          bTarFound = true;
        }
      }

      if (cAIManager->GetDistance(pFindIter->second->tPos, myPosition) < 144) {
        // Finding CurrentTriangle
        tri.tVert0 = pFindIter->second->tEdges[0].cLineStart;
        tri.tVert1 = pFindIter->second->tEdges[1].cLineStart;
        tri.tVert2 = pFindIter->second->tEdges[2].cLineStart;

        if (cCollision->PointInTriangle(tri, myPosition) == true) {
          pcAI->DrawTriangle(
              tri.tVert0,
              tri.tVert1,
              tri.tVert2,
              scd::vector4(126, 126, 126, 255));
          pcAI->m_nCurrentTriangle = pFindIter->first;
          bCurFound = true;
        }
      }

      ++pFindIter;
    }

    if (bTarFound == false) {
      // We didn't find a new target...
      // Find the triangle closest to the goal position
      pFindIter = cAIManager->m_cTris.begin();
      TTri* tClosestTri;
      int nTriNumber;
      float fDist = FLT_MAX;
      float fTempDist;
      while (pFindIter != cAIManager->m_cTris.end()) {
        fTempDist = cAIManager->GetDistance(
            pFindIter->second->tPos, pcAI->m_vGoalPosition);

        if (fTempDist < fDist) {
          // We found a closer triangle than the last one
          fDist = fTempDist;
          tClosestTri = pFindIter->second;
          nTriNumber = pFindIter->first;
        }

        ++pFindIter;
      }

      // We found the closest triangle so
      // set it as our target
      pcAI->DrawTriangle(
          tClosestTri->tEdges[0].cLineStart,
          tClosestTri->tEdges[1].cLineStart,
          tClosestTri->tEdges[2].cLineStart,
          scd::vector4(255, 0, 0, 255));

      pcAI->DrawLine(
          myPosition, tClosestTri->tPos, scd::vector4(0, 0, 255, 255));
      pcAI->m_nTargetTriangle = nTriNumber;
      GoalPosition = tClosestTri->tPos;
    }

    if (bCurFound == false) {
      // We aren't in a triangle
      // So lets just go back to the last
      // triangle we were in
      if (pcAI->m_nCurrentTriangle == -1) {
        pcAI->m_nCurrentTriangle = rand() % cAIManager->m_cTris.size();
      }

      pFindIter = cAIManager->m_cTris.find(pcAI->m_nCurrentTriangle);
      tri.tVert0 = pFindIter->second->tEdges[0].cLineStart;
      tri.tVert1 = pFindIter->second->tEdges[1].cLineStart;
      tri.tVert2 = pFindIter->second->tEdges[2].cLineStart;

      scd::transform& myMatress =
          pcAI->m_pObject->GetTransform()->GetLocalMatrix();
      myMatress._41 = pFindIter->second->tPos.x - Heading.x;
      myMatress._42 = pFindIter->second->tPos.y - Heading.y;
      myMatress._43 = pFindIter->second->tPos.z - Heading.z;
    }

    // Find which edge the agent should be trying to go through
    TTri* pTri = cAIManager->m_cTris.find(pcAI->m_nCurrentTriangle)->second;
    map<int, TTri*, less<int>, scd::allocator<pair<int, TTri*>>>::iterator
        pCurTriIter = pTri->m_cConnections.begin();

    // The edge that we're suppose to go through according to A*
    nAStarEdge = cAIManager->m_nLookup[cAIManager->LookUpCellAlgorithm(
        pcAI->m_nCurrentTriangle, pcAI->m_nTargetTriangle)];

    int nNextTriID = -1;
    // Find next triangle
    while (pCurTriIter != pTri->m_cConnections.end()) {
      for (int i = 0; i < 3; ++i) {
        // Get the next triangle based on A* edge
        if (cAIManager->LinesEqual(
                pTri->tEdges[nAStarEdge], pCurTriIter->second->tEdges[i])) {
          pcAI->tNextTri = pCurTriIter->second;
          nNextTriID = pCurTriIter->first;

          // To easily get out of the while loop
          // without using a goto
          pCurTriIter = pTri->m_cConnections.end();
          --pCurTriIter;
          //
          break;
        }
      }

      ++pCurTriIter;
    }
#pragma endregion

    if (nNextTriID != -1) {
      // Find entire path within distance
      float fDistanceOfPath;
      if (pcAI->m_fBlindTimer < 0) {
        // We aren't pied so do pathing reguraly
        fDistanceOfPath = (pcAI->m_fSpeed * pcAI->m_fSpeed) * 0.35f;
      } else {
        // We are pied so create a longer path which
        // will be less accurate
        fDistanceOfPath = (pcAI->m_fSpeed * pcAI->m_fSpeed) * 2.5f;
      }

      // Get the next tri
      map<int, TTri*, less<int>, scd::allocator<pair<int, TTri*>>>::iterator
          pTemp = cAIManager->m_cTris.find(nNextTriID);

      do {
        if (pTemp->first == pcAI->m_nTargetTriangle) {
          // We found our Goal
          pcAI->m_pMiniGoalTri = pTemp->second;
          break;
        }

        pcAI->DrawTriangle(
            pTemp->second->tEdges[0].cLineStart,
            pTemp->second->tEdges[1].cLineStart,
            pTemp->second->tEdges[2].cLineStart,
            scd::vector4(126, 126, 126, 255));

        // Find which edge the AI should try to
        // be exiting out of
        nAStarEdge = cAIManager->m_nLookup[cAIManager->LookUpCellAlgorithm(
            pTemp->first, pcAI->m_nTargetTriangle)];

        map<int, TTri*, less<int>, scd::allocator<pair<int, TTri*>>>::iterator
            pConnectionIter = pTemp->second->m_cConnections.begin();

        // Loop through all connections and look for
        // the next triangle in the path
        while (pConnectionIter != pTemp->second->m_cConnections.end()) {
          // For each edge
          for (int nEdge = 0; nEdge < 3; ++nEdge) {
            // If the pConnectionIter's current edge is
            // equal to the current triangle's AStarEdge
            if (cAIManager->LinesEqual(
                    pTemp->second->tEdges[nAStarEdge],
                    pConnectionIter->second->tEdges[nEdge])) {
              // Set the MiniGoalTri to pTemp
              pcAI->DrawLine(
                  pTemp->second->tPos,
                  pConnectionIter->second->tPos,
                  scd::vector4(255, 255, 0, 255));
              pcAI->m_pMiniGoalTri = pTemp->second;
              nMiniGoalTriID = pTemp->first;

              // Set pTemp to pConnectionIter
              pTemp = cAIManager->m_cTris.find(pConnectionIter->first);

              // To easily get out of the while
              // loop without using a goto
              pConnectionIter = pTemp->second->m_cConnections.end();
              --pConnectionIter;
              break;
              //
            }
          }
          ++pConnectionIter;
        }
      } while (fDistanceOfPath
               > pcAI->GetDistance(myPosition, pTemp->second->tPos));
      pcAI->DrawLine(
          myPosition, pcAI->m_pMiniGoalTri->tPos, scd::vector4(0, 255, 0, 255));
      pcAI->DrawLine(
          myPosition, pTemp->second->tPos, scd::vector4(126, 0, 126, 255));
    }

    if (pcAI->m_pMiniGoalTri == NULL) {
      // Don't do more processing
      // inless you want to crash
      return;
    }

    // Find which edge the heading collides with
    pFindIter = cAIManager->m_cTris.find(pcAI->m_nCurrentTriangle);
    TRay HeadingRay;
    HeadingRay.cRayStart = myPosition;
    HeadingRay.cRayNormal = Heading * 20.0f;
    for (int nEdge = 0; nEdge < 3; ++nEdge) {
      if (cCollision->RayToLineIntersection(
              HeadingRay, pFindIter->second->tEdges[nEdge])
          == true) {
        nHeadingEdge = nEdge;
        break;
      }
    }

    // Error checking
    if ((nHeadingEdge == -1)) {
      pcAI->DrawLine(
          myPosition, Heading + myPosition, scd::vector4(0, 255, 255, 255));
      pcAI->EndDraw();
      return;
    }

    // Move
    nAStarEdge = cAIManager->m_nLookup[cAIManager->LookUpCellAlgorithm(
        pcAI->m_nCurrentTriangle, pcAI->m_nTargetTriangle)];

    scd::vector3 ToGoalNorm;
    ToGoal = pcAI->m_pMiniGoalTri->tPos - myPosition;
    if (nMiniGoalTriID == -1) {
      // Steer towards goal
      ToGoal = pcAI->m_vGoalPosition - myPosition;

      scd::vector3 cRtVec, cRtVecNorm;
      cRtVec.x = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._11;
      cRtVec.y = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._12;
      cRtVec.z = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._13;

      // It's a left vecotr at the moment
      // so we need to switch it to right
      cRtVec *= -1;

      // Find out if we need to turn left or right
      // by dot product from right vector and goal vector
      D3DXVec3Normalize(&cRtVecNorm, &cRtVec);
      D3DXVec3Normalize(&ToGoalNorm, &ToGoal);

      float fDot = D3DXVec3Dot(&ToGoalNorm, &cRtVecNorm);

      if (fDot > 0.0f) {
        if (pcAI->GetIsActive()) {
          SendInputEvent("SteerRight", pcAI, pcAI->m_pObject, 1.0f);
        }
      } else {
        if (pcAI->GetIsActive()) {
          SendInputEvent("SteerLeft", pcAI, pcAI->m_pObject, 1.0f);
        }
      }

      if (pcAI->GetIsActive()) {
        SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, 1.0f);
      }
    } else if (nHeadingEdge == nAStarEdge) {
      // Accelerate by some value
      D3DXVec3Normalize(&ToGoalNorm, &ToGoal);
      float fd = D3DXVec3Dot(&Heading, &ToGoalNorm);

      // Get move thresholds
      float fMove1 =
          1 - (D3DXToDegree(pcAI->fTurnSpeed) * pcAI->fThreshold[0]) / 90.0f;
      float fMove2 =
          1 - (D3DXToDegree(pcAI->fTurnSpeed) * pcAI->fThreshold[1]) / 90.0f;

      if (fd > fMove1) {
        // We're going good so we can speed up
        if (pcAI->GetIsActive()) {
          SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, fd);
        }
      } else if ((fd > fMove2) && (pcAI->m_fSpeed > MIN_SPEED)) {
        // We're going to need to turn soon
        // so slow down some
        if (pcAI->GetIsActive()) {
          SendInputEvent("Decelerate", pcAI, pcAI->m_pObject, 1 - fd);
        }
      } else {
        // We need to slow down now, but
        // not too much
        if (pcAI->GetIsActive()) {
          if (pcAI->m_fSpeed > MIN_SPEED) {
            SendInputEvent("Decelerate", pcAI, pcAI->m_pObject, 1.0f);
          } else {
            SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, 0.2f);
          }
        }
      }
    } else {
      // Steer Toward Next Tri
      if (pcAI->m_fSpeed > MIN_SPEED) {
        if (pcAI->GetIsActive()) {
          SendInputEvent("Decelerate", pcAI, pcAI->m_pObject, 1.0f);
        }
      } else {
        if (pcAI->GetIsActive()) {
          SendInputEvent("Accelerate", pcAI, pcAI->m_pObject, 1.0f);
        }
      }

      scd::vector3 cRtVec, cRtVecNorm;
      cRtVec.x = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._11;
      cRtVec.y = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._12;
      cRtVec.z = pcAI->m_pObject->GetTransform()->GetWorldMatrix()._13;
      cRtVec *= -1;
      D3DXVec3Normalize(&cRtVecNorm, &cRtVec);
      D3DXVec3Normalize(&ToGoalNorm, &ToGoal);

      float fDot = D3DXVec3Dot(&ToGoalNorm, &cRtVecNorm);

      if (fDot > 0.0f) {
        if (pcAI->GetIsActive()) {
          SendInputEvent("SteerRight", pcAI, pcAI->m_pObject, 1.0f);
        }
      } else {
        if (pcAI->GetIsActive()) {
          SendInputEvent("SteerLeft", pcAI, pcAI->m_pObject, 1.0f);
        }
      }
    }

    pcAI->DrawLine(
        myPosition, Heading + myPosition, scd::vector4(0, 255, 255, 255));

    pcAI->EndDraw();
    pcAI->UseHeldItem();
  }
#pragma endregion
}

// void ai_component::Update()
//{
//	// This horrible thing is how you
//	// call a member function pointer
//	// that calls a member function
//	//((*this).*(m_pfUpdateState))();
//}

void ai_component::UpdateCollective() { EvaluateGoalItemWeights(); }

void ai_component::UpdateAggressive() {
  bool bPassive = false;
  EvaluateOpponentWeights();

  float fDist = GetDistance(
      m_pTargetPlayer->GetTransform()->GetWorldPosition(),
      m_pObject->GetTransform()->GetWorldPosition());

  if (fDist < 225) {
    if ((sqrtf(fDist) / m_fSpeed * m_fAcceleration + m_fSpeed)
        < m_fRammingSpeed) {
      // We aren't going fast enough to ram
      // so be passive
      bPassive = true;
    }
  }

  if (bPassive == true) {
    // Don't be aggressive
    m_bTargetItem = false;
    EvaluateGoalItemWeights();
    EvaluateHeldItemWeights();

    if (m_bTargetItem == false) {
      // We didn't find a held item to get,
      // nor a goal item
      map<int, TTri*, less<int>, scd::allocator<pair<int, TTri*>>>::iterator
          pIter;

      // Set goal to random triangle
      pIter = CAIManager::GetInstance()->m_cTris.find(
          rand() % CAIManager::GetInstance()->m_cTris.size());

      m_vGoalPosition = pIter->second->tPos;
    }
  }
}

void ai_component::UpdateDefensive() {
  EvaluateGoalItemWeights();
  EvaluateHeldItemWeights();
}

scd::object* ai_component::GetOpponent(int nPlayerID) {
  map<unsigned int,
      scd::object*,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, scd::object*>>>::iterator OpponentIter =
      CAIManager::GetInstance()->m_cPlayers.find(nPlayerID);

  return OpponentIter->second;
}

scd::vector3 ai_component::GetOpponentPos(int nPlayerID) {
  return CAIManager::GetInstance()
      ->m_cPlayers.find(nPlayerID)
      ->second->GetTransform()
      ->GetWorldPosition();
}

scd::object* ai_component::GetClosestOpponent() {
  map<unsigned int,
      scd::object*,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, scd::object*>>>::iterator OpponentIter =
      CAIManager::GetInstance()->m_cPlayers.begin();

  scd::object* cOpponents[3];
  float fDist[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
  unsigned index = 0;

  while (OpponentIter != CAIManager::GetInstance()->m_cPlayers.end()) {
    // Find positions of all opponents
    if (OpponentIter->second->GetID() != m_pObject->GetID()) {
      cOpponents[index] = OpponentIter->second;
      fDist[index] = GetDistance(
          m_pObject->GetTransform()->GetWorldPosition(),
          OpponentIter->second->GetTransform()->GetWorldPosition());
      ++index;
    }

    ++OpponentIter;
  }

  if ((fDist[0] <= fDist[1]) && (fDist[0] <= fDist[2])) {
    return cOpponents[0];
  } else if (fDist[1] <= fDist[2]) {
    return cOpponents[1];
  } else {
    return cOpponents[2];
  }
}

void ai_component::GoalItemInit(
    IEvent* piEvent,
    scd::base_component* piComponent) {
  // We're getting the Goal Items for this level
  // save them so we know what to go for
  ai_component* pcAI = (ai_component*)piComponent;
  TGoalItemEvent* pcGoalItem = (TGoalItemEvent*)piEvent->GetData();

  pcAI->m_tKnowledge.m_cNeededGoalItems.insert(make_pair(
      unsigned int(pcGoalItem->m_eGoalItemType), pcGoalItem->m_eGoalItemType));
}

void ai_component::GoalItemCollected(
    IEvent* piEvent,
    scd::base_component* piComponent) {
  ai_component* pcAI = (ai_component*)piComponent;
  TGoalItemCollectedEvent* pcCollected =
      (TGoalItemCollectedEvent*)piEvent->GetData();

  EGoalItemType eType = CSpawningManager::GetInstance()->GetGoalItemType(
      pcCollected->m_pcGoalItem);

  // Look for which player found it
  // if Our ID == Collector's ID
  if (pcAI->m_pObject->GetID() != pcCollected->m_pcCollector->GetID()) {
    // We didn't collect it
    // For each opponent...
    for (unsigned nPlayer = 0; nPlayer < 3; ++nPlayer) {
      // If we found the opponent who collected the item
      if (pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_nPlayer
          == pcCollected->m_pcCollector->GetID()) {
        // Add the Goal Item into their known
        // collected Goal Items
        pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.insert(
            make_pair(unsigned int(eType), eType));

        // Increment the amount of Goal Items we know they have
        pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_chAmtGoalItems =
            pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.size();
      }
    }
  } else // This AI collected the Goal Item
  {
    // Remove from our needed map
    map<unsigned int,
        EGoalItemType,
        less<unsigned int>,
        scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator
        cNeededIter = pcAI->m_tKnowledge.m_cNeededGoalItems.find(eType);

    if (cNeededIter != pcAI->m_tKnowledge.m_cNeededGoalItems.end()) {
      pcAI->m_tKnowledge.m_cNeededGoalItems.erase(cNeededIter);
    }
  }

  // Remove Goal Item from spawned list
  pcAI->RemoveGoalItemFromLevel(eType);
}

void ai_component::GoalItemSpawned(
    IEvent* piEvent,
    scd::base_component* piComponent) {
  ai_component* pcAI = (ai_component*)piComponent;
  TGoalItemEvent* pcGoalItem = (TGoalItemEvent*)piEvent->GetData();

  TGoalItem tGoalItem;
  tGoalItem.m_nGoalItem = pcGoalItem->m_eGoalItemType;
  tGoalItem.m_pfWeight = 0;
  tGoalItem.m_cPos.x = pcGoalItem->m_pcGoalItem->GetParent()
                           ->GetTransform()
                           ->GetWorldMatrix()
                           ._41;
  tGoalItem.m_cPos.y = pcGoalItem->m_pcGoalItem->GetParent()
                           ->GetTransform()
                           ->GetWorldMatrix()
                           ._42;
  tGoalItem.m_cPos.z = pcGoalItem->m_pcGoalItem->GetParent()
                           ->GetTransform()
                           ->GetWorldMatrix()
                           ._43;
  pcAI->m_tKnowledge.m_cGoalItemsInLevel.insert(
      make_pair(unsigned int(tGoalItem.m_nGoalItem), tGoalItem));
}

void ai_component::GoalItemDespawned(
    IEvent* piEvent,
    scd::base_component* piComponent) {
  ai_component* pcAI = (ai_component*)piComponent;
  TGoalItemEvent* pcGoalItem = (TGoalItemEvent*)piEvent->GetData();

  pcAI->RemoveGoalItemFromLevel(pcGoalItem->m_eGoalItemType);
}

void ai_component::RemoveGoalItemFromLevel(EGoalItemType eGoalItemType) {
  map<unsigned int,
      TGoalItem,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, TGoalItem>>>::iterator cGoalItemIter =
      m_tKnowledge.m_cGoalItemsInLevel.find(eGoalItemType);

  if (cGoalItemIter != m_tKnowledge.m_cGoalItemsInLevel.end()) {
    m_tKnowledge.m_cGoalItemsInLevel.erase(cGoalItemIter);
  }
}

void ai_component::EvaluateStateWeights() {
  if (bLogical == false) {
    return;
  }

  bool bOpponentInFirst = false;

  m_pfWeight[COLLECTIVE] = 0;
  m_pfWeight[AGGRESSIVE] = 0;
  m_pfWeight[DEFENSIVE] = 0;

  // Iterator for Goal Items in the level
  map<unsigned int,
      TGoalItem,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, TGoalItem>>>::iterator ItemIter;

  // Iterator for any needed Goal Item
  map<unsigned int,
      EGoalItemType,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator NeededIter;

  // Iterator for any Goal Item the opponent has
  map<unsigned int,
      EGoalItemType,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator OpponentIter;

  // Assign the Goal Item Level iter
  NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

  // Increase the collective weight per
  // Goal Item needed
  map<unsigned int,
      TGoalItem,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, TGoalItem>>>::iterator TempItemIter;
  while (NeededIter != m_tKnowledge.m_cNeededGoalItems.end()) {
    TempItemIter = m_tKnowledge.m_cGoalItemsInLevel.find(NeededIter->first);

    if (TempItemIter != m_tKnowledge.m_cGoalItemsInLevel.end()) {
      m_pfWeight[COLLECTIVE] += 0.2f;
    }

    ++NeededIter;
  }

  // Get an iterator for all the dropped
  // items in the level
  map<unsigned int,
      EGoalItemType,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator
      pDroppedIter = m_tKnowledge.m_cDroppedItems.begin();

  // Increase the collective weight
  // for all dropped items in the level
  // that are needed
  NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

  map<unsigned int,
      EGoalItemType,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator
      TempDroppedIter;
  while (NeededIter != m_tKnowledge.m_cNeededGoalItems.end()) {
    TempDroppedIter = m_tKnowledge.m_cDroppedItems.find(NeededIter->first);
    if (TempDroppedIter != m_tKnowledge.m_cDroppedItems.end()) {
      m_pfWeight[COLLECTIVE] += 0.25f;
    }
    ++NeededIter;
  }

  // Aggressive Weight

  // Loop through each opponent and look to see if they have
  // a Goal Item that we need and increase aggressive weight
  // if they do
  for (unsigned nPlayers = 0; nPlayers < 3; ++nPlayers) {
    // Reset our knowledge of the
    // current player
    m_tKnowledge.m_cOpponents[nPlayers].m_bHasItem = false;

    if (m_tKnowledge.m_cOpponents[nPlayers].m_chAmtGoalItems == 8) {
      bOpponentInFirst = true;
    }

    // Set iterator for opponent's Goal Items
    OpponentIter = m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.begin();

    // Search through their list of Goal Items
    while (OpponentIter
           != m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.end()) {
      NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

      // Search through our needed list with
      // with what they have
      while (NeededIter != m_tKnowledge.m_cNeededGoalItems.end()) {
        // Increase aggressive weight if they
        // have an item we want
        if (OpponentIter->second == NeededIter->second) {
          m_pfWeight[AGGRESSIVE] += 0.10f;
          m_tKnowledge.m_cOpponents[nPlayers].m_bHasItem = true;
          m_tKnowledge.m_cOpponents[nPlayers].m_nItem = NeededIter->second;
        }

        NeededIter++;
      }

      OpponentIter++;
    }
  }

  // Adjust weights based on place
  int nPlace = GetMyPlace();

  switch (nPlace) {
  case 1: {
    // In first so don't put yourself in danger
    m_pfWeight[AGGRESSIVE] -= 0.2f;
    break;
  }
  case 2: {
    // In second...
    if (bOpponentInFirst == true) {
      // opponent has 8 items
      if ((m_pfWeight[COLLECTIVE] > 0.0f)) {
        // Try and collect all 8 and win
        m_pfWeight[AGGRESSIVE] -= 1.0f;
        m_pfWeight[DEFENSIVE] -= 1.0f;
        m_pfWeight[COLLECTIVE] += 1.0f;
      } else {
        // KILL THEM!
        m_pfWeight[AGGRESSIVE] += 1.0f;
      }
    } else {
      // just be a little defensive since
      // there's no goal items to collect
      if (m_pfWeight[COLLECTIVE] == 0.0f) {
        m_pfWeight[DEFENSIVE] += 0.2f;
      }
    }
    break;
  }
  case 3: {
    // nothing...
    break;
  }
  case 4: {
    // In last!
    int nFirstPlaceID =
        CAIManager::GetInstance()->m_cPlayers.find(GetFirstPlace())->first;

    int nFirstPlaceIndex = 0;
    for (; nFirstPlaceIndex < 3; ++nFirstPlaceIndex) {
      // Find who's in first
      if (nFirstPlaceID
          == m_tKnowledge.m_cOpponents[nFirstPlaceIndex].m_nPlayer) {
        break;
      }
    }

    int nItemDifference;
    nItemDifference =
        m_tKnowledge.m_cOpponents[nFirstPlaceIndex].m_chAmtGoalItems
        - (8 - m_tKnowledge.m_cNeededGoalItems.size());

    if (nItemDifference > 4) {
      // Screw it...
      m_pfWeight[AGGRESSIVE] += 0.55f;
    } else if (nItemDifference > 3) {
      // We should attack them if there
      // isn't a lot to collect
      m_pfWeight[AGGRESSIVE] += 0.25f;
    } else if (nItemDifference > 2) {
      // We might want to hit them a bit
      m_pfWeight[AGGRESSIVE] += 0.15f;
    }

    m_pfWeight[AGGRESSIVE] += 0.15f;

    break;
  }
  }

  // Increase defensive weight if
  // collective or aggressive are low
  if (m_pfWeight[COLLECTIVE] < 0.20f) {
    m_pfWeight[DEFENSIVE] += 0.30f;
  }
  if (m_pfWeight[AGGRESSIVE] <= 0.15f) {
    m_pfWeight[DEFENSIVE] += 0.15f;
  }

  // Get New State
  if ((m_pfWeight[DEFENSIVE] > m_pfWeight[COLLECTIVE])
      && (m_pfWeight[DEFENSIVE] > m_pfWeight[AGGRESSIVE])) {
    m_eCurrentState = DEFENSIVE;
    UpdateDefensive();
  } else if (m_pfWeight[COLLECTIVE] >= m_pfWeight[AGGRESSIVE]) {
    m_eCurrentState = COLLECTIVE;
    UpdateCollective();
  } else {
    m_eCurrentState = AGGRESSIVE;
    UpdateAggressive();
  }
}

void ai_component::EvaluateHeldItemWeights() {
  if ((m_tKnowledge.m_peMyHeldItems[0] != NO_HELD_ITEM)
      && (m_tKnowledge.m_peMyHeldItems[1] != NO_HELD_ITEM)) {
    // Either we're already going towards
    // a Held Item or we can't hold any more
    return;
  }

  THeldItem tHeldItem;
  bool bPowerup;

  if (m_pfWeight[COLLECTIVE] > m_pfWeight[AGGRESSIVE]) {
    // Make it more defensive
    bPowerup = true;
  } else {
    // Make it more aggressive
    bPowerup = false;
  }

  if (FindNearbyHeldItem(bPowerup, tHeldItem) == true) {
    // We found a held item
    m_bTargetItem = true;
    m_vGoalPosition = tHeldItem.m_cPos;
  }
}

void ai_component::EvaluateGoalItemWeights() {
  map<unsigned int,
      TGoalItem,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, TGoalItem>>>::iterator ItemIter;
  map<unsigned int,
      EGoalItemType,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator NeededIter;

  scd::vector3 VecTo;
  float fMyDist(0);
  float fDist(0);

  // See if there's a dropped item nearby
  // that we need
  map<unsigned int,
      EGoalItemType,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator DroppedIter;
  DroppedIter = m_tKnowledge.m_cDroppedItems.begin();

  CPickupItemComponent* pPickup;
  while (DroppedIter != m_tKnowledge.m_cDroppedItems.end()) {
    pPickup =
        CPickupItemManager::GetInstance()->GetPickupComp(DroppedIter->second);

    if (pPickup == NULL) {
      ++DroppedIter;
      continue;
    }

    if (GetDistance(
            pPickup->GetObject()->GetTransform()->GetWorldPosition(),
            m_pObject->GetTransform()->GetWorldPosition())
        <= 225) {
      // We found a goal
      m_vGoalPosition =
          pPickup->GetObject()->GetTransform()->GetWorldPosition();
      return;
    }

    ++DroppedIter;
  }

  ItemIter = m_tKnowledge.m_cGoalItemsInLevel.begin();
  TGoalItem pcTarget;
  pcTarget.m_nGoalItem = NO_ITEM;
  pcTarget.m_pfWeight = FLT_MAX;

  // Loop through each Goal Item which the AI knows is in the level.
  // {
  //		Loop through all Goal Items the AI currently needs and
  //		checks to see if any of the Goal Items in the level matches
  //		any of the Goal Items the AI needs.
  // }
  while (ItemIter != m_tKnowledge.m_cGoalItemsInLevel.end()) {
    NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

    while (NeededIter != m_tKnowledge.m_cNeededGoalItems.end()) {
      if (NeededIter->second == ItemIter->second.m_nGoalItem) {
        // This will take the distance from each goal item
        // the agent is and make it a value between 0 - 1,
        // where the closer the item is, the higher the
        // value is. This is then added to the item's weight
        scd::vector3 cMe;
        scd::vector3 cHeading, cHeadingNorm;
        scd::vector3 cToGoal, cToGoalNorm;
        cMe.x = m_pObject->GetTransform()->GetWorldMatrix()._41;
        cMe.y = m_pObject->GetTransform()->GetWorldMatrix()._42;
        cMe.z = m_pObject->GetTransform()->GetWorldMatrix()._43;
        cHeading.x = m_pObject->GetTransform()->GetWorldMatrix()._31;
        cHeading.y = m_pObject->GetTransform()->GetWorldMatrix()._32;
        cHeading.z = m_pObject->GetTransform()->GetWorldMatrix()._33;

        cToGoal = ItemIter->second.m_cPos - cMe;
        D3DXVec3Normalize(&cHeadingNorm, &cHeading);
        D3DXVec3Normalize(&cToGoalNorm, &cToGoal);

        fMyDist = GetDistance(ItemIter->second.m_cPos, cMe);

        float MyWeight = (1 - (fMyDist / MAXDIST)) + m_fSpeed
                         + D3DXVec3Dot(&cHeadingNorm, &cToGoalNorm);
        ItemIter->second.m_pfWeight = fMyDist;

        // This will get the closest opponent to you
        // and find out their weight and then will subtract
        // from each of the item's weights
        scd::object* pOpponent = GetClosestOpponent();

        scd::vector3 OpponentPos;
        scd::vector3 OpponentHeading;

        // If the current player is not me
        if (pOpponent->GetID() != m_pObject->GetID()) {
          OpponentPos.x = pOpponent->GetTransform()->GetWorldMatrix()._41;
          OpponentPos.y = pOpponent->GetTransform()->GetWorldMatrix()._42;
          OpponentPos.z = pOpponent->GetTransform()->GetWorldMatrix()._43;

          OpponentHeading.x = pOpponent->GetTransform()->GetLocalMatrix()._31;
          OpponentHeading.y = pOpponent->GetTransform()->GetLocalMatrix()._32;
          OpponentHeading.z = pOpponent->GetTransform()->GetLocalMatrix()._33;

          VecTo = ItemIter->second.m_cPos - OpponentPos;
          fDist = GetDistance(ItemIter->second.m_cPos, OpponentPos);

          scd::vector3 OpponentHeadingNorm, VecToNorm;
          D3DXVec3Normalize(&OpponentHeadingNorm, &OpponentHeading);
          D3DXVec3Normalize(&VecToNorm, &VecTo);

          // Find opponent in knowledge
          int nIndex;
          for (nIndex = 0;; ++nIndex) {
            if (m_tKnowledge.m_cOpponents[nIndex].m_nPlayer
                == pOpponent->GetID()) {
              break;
            }
          }

          float OpponentWeight =
              (1 - (fDist / MAXDIST)
               + m_tKnowledge.m_cOpponents[nIndex].m_fCurrentSpeed)
              + D3DXVec3Dot(&OpponentHeadingNorm, &VecToNorm);

          ItemIter->second.m_pfWeight -= OpponentWeight;

          if (ItemIter->second.m_pfWeight < 0.0f) {
            ItemIter->second.m_pfWeight = 0.0f;
          }
        }

        if (pcTarget.m_pfWeight > ItemIter->second.m_pfWeight) {
          memcpy(&pcTarget, &ItemIter->second, sizeof(pcTarget));
        }
      }

      NeededIter++;
    }

    ItemIter++;
  }

  if ((pcTarget.m_pfWeight > 0) && (pcTarget.m_nGoalItem != NO_ITEM)) {
    m_bTargetItem = true;
    m_vGoalPosition = pcTarget.m_cPos;
  }
}

void ai_component::EvaluateOpponentWeights() {
  if (GetMyPlace() == 4) {
    // We're in last place so just auto target the
    // guy in first place
    m_pTargetPlayer =
        CAIManager::GetInstance()->m_cPlayers.find(GetFirstPlace())->second;
    m_vGoalPosition = m_pTargetPlayer->GetTransform()->GetWorldPosition();

    return;
  }

  scd::object* pTempTargetPlayer;
  m_pTargetPlayer = NULL;

  for (unsigned CurOpponent = 0;
       CurOpponent < CAIManager::GetInstance()->m_cPlayers.size();
       ++CurOpponent) {
    if (m_tKnowledge.m_cOpponents[CurOpponent].m_chAmtGoalItems == 8) {
      // This guy has 8 items... KILL HIM!
      m_pTargetPlayer =
          GetOpponent(m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
      break;
    }

    if (m_tKnowledge.m_cOpponents[CurOpponent].m_bHasItem == true) {
      // Find a player who has our
      if (m_pTargetPlayer == NULL) {
        m_pTargetPlayer =
            GetOpponent(m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
      } else {
        pTempTargetPlayer =
            GetOpponent(m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);

        // If pTempTargetPlayer is < pTargetPlayer
        if (GetDistance(
                m_pTargetPlayer->GetTransform()->GetWorldPosition(),
                m_pObject->GetTransform()->GetWorldPosition())
            > GetDistance(
                  pTempTargetPlayer->GetTransform()->GetWorldPosition(),
                  m_pObject->GetTransform()->GetWorldPosition())) {
          m_pTargetPlayer =
              GetOpponent(m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
        }
      }
    }
  }

  if (m_pTargetPlayer == NULL) {
    // We don't have a target so just get whoever is closer
    m_pTargetPlayer = GetClosestOpponent();
  }

  if (m_pTargetPlayer != NULL) {
    m_vGoalPosition = m_pTargetPlayer->GetTransform()->GetWorldPosition();
  }
}

float ai_component::GetDistance(
    const scd::vector3& cPos1,
    const scd::vector3& cPos2) const {
  // return SQUARED distance
  float x = cPos2.x - cPos1.x;
  float z = cPos2.z - cPos1.z;
  return ((x * x) + (z * z));
}

void ai_component::UpdateSpeeds(IEvent* pEvent, scd::base_component* pComp) {
  TSpeedEvent* pSpeed = (TSpeedEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  if (pcAI->m_pObject->GetID() == pSpeed->m_pObject->GetID()) {
    // Set our speed
    pcAI->m_fSpeed = pSpeed->m_fSpeed;
    return;
  }

  for (int nIndex = 0; nIndex < 3; ++nIndex) {
    // Find the guy in our known opponents
    if (pcAI->m_tKnowledge.m_cOpponents[nIndex].m_nPlayer
        == pSpeed->m_pObject->GetID()) {
      // set their speed
      pcAI->m_tKnowledge.m_cOpponents[nIndex].m_fCurrentSpeed =
          pSpeed->m_fSpeed;

      return;
    }
  }
}

bool ai_component::IsPowerup(int eType) { return eType <= MAX_ATTACK_ITEMS; }

void ai_component::HeldItemSpawned(IEvent* pEvent, scd::base_component* pComp) {
  THeldItemSpawned* pHeldItem = (THeldItemSpawned*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  THeldItem tHeldItem;
  tHeldItem.m_cPos =
      pHeldItem->m_pHeldItem->GetParent()->GetTransform()->GetWorldPosition();
  tHeldItem.m_nHeldItem = pHeldItem->m_pHeldItem->GetType();
  tHeldItem.m_bPowerup = pcAI->IsPowerup(pHeldItem->m_pHeldItem->GetType());
  tHeldItem.m_nID = pHeldItem->m_pHeldItem->GetParent()->GetID();
  pcAI->m_tKnowledge.m_cHeldItemsInLevel.insert(
      make_pair(tHeldItem.m_nID, tHeldItem));
}

void ai_component::HeldItemCollected(
    IEvent* pEvent,
    scd::base_component* pComp) {
  THeldItemCollected* pHeldItem = (THeldItemCollected*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  if (pcAI->m_tKnowledge.m_cHeldItemsInLevel.size() == 0) {
    // No Held Items in level
    return;
  }

  // Find the iterator
  map<unsigned int,
      THeldItem,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, THeldItem>>>::iterator pHeldItemIter =
      pcAI->m_tKnowledge.m_cHeldItemsInLevel.find(
          pHeldItem->m_pcHeldItem->GetID());

  if (pHeldItemIter == pcAI->m_tKnowledge.m_cHeldItemsInLevel.end()) {
    // Error
    return;
  }

  if (pHeldItem->m_pcCollector->GetID() == pcAI->m_pObject->GetID()) {
    // If us
    for (int nItem = 0; nItem < 2; ++nItem) {
      if (pcAI->m_tKnowledge.m_peMyHeldItems[nItem] == NO_HELD_ITEM) {
        pcAI->m_tKnowledge.m_peMyHeldItems[nItem] =
            pHeldItemIter->second.m_nHeldItem;
        break;
      }
    }
  }

  if (pHeldItemIter != pcAI->m_tKnowledge.m_cHeldItemsInLevel.end()) {
    pcAI->m_tKnowledge.m_cHeldItemsInLevel.erase(pHeldItemIter);
  }
}

bool ai_component::FindNearbyHeldItem(bool bPowerup, THeldItem& tHeldItem) {
  // Find the closest HeldItem to the AI
  // that is of the correct type

  if (m_tKnowledge.m_cHeldItemsInLevel.size() == 0) {
    return false;
  }

  // This will be our starting
  // distance. It will be increased
  // until we find an item, or it
  // reaches the size of X
  float fIncrement = 50.0f;
  float fDist = fIncrement;

  map<unsigned int,
      THeldItem,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, THeldItem>>>::iterator pHeldItemIter;

  while (fDist < 3600) {
    pHeldItemIter = m_tKnowledge.m_cHeldItemsInLevel.begin();

    while (pHeldItemIter != m_tKnowledge.m_cHeldItemsInLevel.end()) {
      if (GetDistance(
              pHeldItemIter->second.m_cPos,
              m_pObject->GetTransform()->GetWorldPosition())
          <= fDist) {
        if (IsPowerup(pHeldItemIter->second.m_nHeldItem) == bPowerup) {
          if (m_eCurrentState == AGGRESSIVE) {
            if ((pHeldItemIter->second.m_nHeldItem == TURKEY)
                || (pHeldItemIter->second.m_nHeldItem == SOUP)
                || (pHeldItemIter->second.m_nHeldItem == DONUT)) {
              tHeldItem = pHeldItemIter->second;
              return true;
            }
          } else {
            tHeldItem = pHeldItemIter->second;
            return true;
          }
        }
      }

      ++pHeldItemIter;
    }
    fDist += fIncrement;
  }

  return false;
}

void ai_component::UseHeldItem() {
  // Use random held item if timer is up
  if (m_fUseItemTimer < 0.0f) {
    m_fUseItemTimer = static_cast<float>(USE_ITEM_TIMER);
    PickRandomHeldItem();
  }

  // Check to see if the agent has any held items
  // if( (m_tKnowledge.m_peMyHeldItems[0] == NO_HELD_ITEM) &&
  //	(m_tKnowledge.m_peMyHeldItems[1] == NO_HELD_ITEM) )
  //{
  //	return;
  //}

  map<unsigned int,
      scd::object*,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, scd::object*>>>::iterator OpponentIter =
      CAIManager::GetInstance()->m_cPlayers.begin();

  scd::object* pOpponents[3];
  int index = 0;

  // Get a pointer to each opponent
  while ((OpponentIter != CAIManager::GetInstance()->m_cPlayers.end())) {
    if (OpponentIter->second->GetID() != m_pObject->GetID()) {
      pOpponents[index] = OpponentIter->second;
      ++index;
    }

    ++OpponentIter;
  }

  scd::vector3 MyHeading;
  scd::vector3 OpponentHeading;
  scd::vector3 MyHeadingNorm;
  scd::vector3 OpponentHeadingNorm;
  scd::vector3 ToPlayerPos;
  scd::vector3 ToPlayerNorm;
  scd::vector3 ToAgent;
  float fDot;

  // Get agent's heading
  MyHeading.x = m_pObject->GetTransform()->GetWorldMatrix()._31;
  MyHeading.y = m_pObject->GetTransform()->GetWorldMatrix()._32;
  MyHeading.z = m_pObject->GetTransform()->GetWorldMatrix()._33;
  D3DXVec3Normalize(&MyHeadingNorm, &MyHeading);

  // Use anything that we don't
  // need an opponent around for
  Use(JAM);

  // Loop through each opponent and see
  // if we should use an item that needs
  // los or distance requirements
  for (size_t i = 0; i < (CAIManager::GetInstance()->m_cPlayers.size() - 1);
       ++i) {
    // Get opponent's heading
    OpponentHeading.x = pOpponents[i]->GetTransform()->GetWorldMatrix()._31;
    OpponentHeading.y = pOpponents[i]->GetTransform()->GetWorldMatrix()._32;
    OpponentHeading.z = pOpponents[i]->GetTransform()->GetWorldMatrix()._33;
    D3DXVec3Normalize(&OpponentHeadingNorm, &OpponentHeading);

    // Check Turkey and Pie

    // Get vector from agent's heading to opponent
    m_tTargetPlayerLine.cRayNormal =
        pOpponents[i]->GetTransform()->GetWorldPosition()
        - (MyHeading + m_pObject->GetTransform()->GetWorldPosition());

    // Check the distance
    if (GetDistance(
            m_tTargetPlayerLine.cRayStart, m_tTargetPlayerLine.cRayNormal)
        > 225) {
      // Target is too far away;
      continue;
    }

    // Normalize and dot product
    D3DXVec3Normalize(&ToPlayerPos, &m_tTargetPlayerLine.cRayNormal);
    fDot = D3DXVec3Dot(&MyHeadingNorm, &ToPlayerPos);

    // Check to see if they're infront
    if (fDot > 0.95f) {
      if (Use(TURKEY) == true) {
        continue;
      } else if (
          pOpponents[i]->GetID() == CIDGen::GetInstance()->GetID("Player0")) {
        if (m_fTurkeyToPlayer < 0.0f) {
          if ((rand() % 101) <= 10) {
            SendStatusEffectEvent("UseTurkeyForward", this, m_pObject, 1.0f);
            m_fTurkeyToPlayer = TURKEY_TO_PLAYER;
            continue;
          } else {
            m_fTurkeyToPlayer = 3.0f;
          }
        }
      }

      if (Use(PIE) == true) {
        continue;
      }
    }

    // Check Soup

    // Get vector from agent to opponent
    ToPlayerPos =
        m_pObject->GetTransform()->GetWorldPosition()
        - (OpponentHeading + pOpponents[i]->GetTransform()->GetWorldPosition());

    // Normalize and dot product
    D3DXVec3Normalize(&ToPlayerNorm, &ToPlayerPos);
    fDot = D3DXVec3Dot(&OpponentHeadingNorm, &ToPlayerNorm);

    if (fDot > 0.95f) {
      Use(SOUP);
    }

    // Use dropped items since someone
    // is nearby
    Use(BANANA);
    Use(PEANUT_BUTTER);

    // Use Donut when about to ram
    if (m_eCurrentState == AGGRESSIVE) {
      Use(DONUT);
      Use(ENERGY_DRINK);
    }
  }
}

void ai_component::SetRamSpeed(IEvent* pEvent, scd::base_component* pComp) {
  TSpeedEvent* pSpeedEvent = (TSpeedEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  if (pSpeedEvent->m_pObject->GetID() == pcAI->m_pObject->GetID()) {
    pcAI->m_fRammingSpeed = pSpeedEvent->m_fSpeed;
  }
}

void ai_component::ShowTurnSpeed(IEvent* pEvent, scd::base_component* pComp) {
  TSpeedEvent* pSpeedEvent = (TSpeedEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  if (pSpeedEvent->m_pObject->GetID() == pcAI->m_pObject->GetID()) {
    pcAI->fTurnSpeed = pSpeedEvent->m_fSpeed;
  }
}

void ai_component::ShowAcceleration(
    IEvent* pEvent,
    scd::base_component* pComp) {
  TSpeedEvent* pSpeedEvent = (TSpeedEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  if (pSpeedEvent->m_pObject->GetID() == pcAI->m_pObject->GetID()) {
    pcAI->m_fAcceleration = pSpeedEvent->m_fSpeed;
  }
}

void ai_component::Blind(IEvent* pEvent, scd::base_component* pComp) {
  TStatusEffectEvent* pStatusEffect = (TStatusEffectEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  pcAI->m_fBlindTimer = 5.0F;
}

void ai_component::Jammed(IEvent* pEvent, scd::base_component* pComp) {
  TStatusEffectEvent* pStatusEffect = (TStatusEffectEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  if (pcAI->m_pObject->GetID() != pStatusEffect->m_pObject->GetID()) {
    pcAI->m_fJamTimer = 10.0f;
  }
}

void ai_component::PickupItemCollected(
    IEvent* pEvent,
    scd::base_component* pComp) {
  TPickupItemCollectedEvent* pPickup =
      (TPickupItemCollectedEvent*)pEvent->GetData();
  ai_component* pcAI = (ai_component*)pComp;

  EGoalItemType eType = pPickup->m_eItemType;

  // Look for which player found it
  // if Our ID == Collector's ID
  if (pcAI->m_pObject->GetID() != pPickup->m_pcCollector->GetID()) {
    // We didn't collect it
    // For each opponent...
    for (unsigned nPlayer = 0; nPlayer < 3; ++nPlayer) {
      // If we found the opponent who collected the item
      if (pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_nPlayer
          == pPickup->m_pcCollector->GetID()) {
        // Add the Goal Item into their known
        // collected Goal Items
        pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.insert(
            make_pair(unsigned int(eType), eType));

        // Increment the amount of Goal Items we know they have
        pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_chAmtGoalItems =
            pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.size();
      }
    }
  } else // This AI collected the Goal Item
  {
    // Remove from our needed list
    map<unsigned int,
        EGoalItemType,
        less<unsigned int>,
        scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator
        cNeededIter =
            pcAI->m_tKnowledge.m_cNeededGoalItems.find(unsigned int(eType));

    if (cNeededIter != pcAI->m_tKnowledge.m_cNeededGoalItems.end()) {
      pcAI->m_tKnowledge.m_cNeededGoalItems.erase(cNeededIter);
    }
  }

  // Remove Dropped Item from dropped list
  map<unsigned int,
      EGoalItemType,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, EGoalItemType>>>::iterator
      cDroppedIter = pcAI->m_tKnowledge.m_cDroppedItems.find(eType);

  if (cDroppedIter != pcAI->m_tKnowledge.m_cDroppedItems.end()) {
    pcAI->m_tKnowledge.m_cDroppedItems.erase(cDroppedIter);
  }
}

void ai_component::PauseUpdateCallback(IEvent*, scd::base_component* pComp) {
  // Get the Effect Comp
  ai_component* comp = (ai_component*)pComp;
  SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_UPDATE);
}

int ai_component::GetFirstPlace() {
  CAIManager* pAIManager = CAIManager::GetInstance();

  map<unsigned int,
      ai_component*,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, ai_component*>>>::iterator pIter;
  pIter = pAIManager->m_cAscd::base_components.begin();

  // Check human player
  int nPlayerID = CIDGen::GetInstance()->GetID("Player0");
  int i = 0;
  for (; i < 3; ++i) {
    if (pIter->second->m_tKnowledge.m_cOpponents[i].m_nPlayer == nPlayerID) {
      break;
    }
  }

  // Check other agents
  int nID = pIter->second->m_tKnowledge.m_cOpponents[i].m_nPlayer;
  int nFirstPlaceAmtItems =
      pIter->second->m_tKnowledge.m_cOpponents[i].m_chAmtGoalItems;
  int nTempItems = -1;

  while (pIter != pAIManager->m_cAscd::base_components.end()) {
    nTempItems = 8 - pIter->second->m_tKnowledge.m_cNeededGoalItems.size();

    if (nFirstPlaceAmtItems < nTempItems) {
      nID = pIter->second->m_pObject->GetID();
      nFirstPlaceAmtItems = nTempItems;
    }

    ++pIter;
  }

  return nID;
}

int ai_component::GetMyPlace() {
  CAIManager* pAIManager = CAIManager::GetInstance();

  map<unsigned int,
      ai_component*,
      less<unsigned int>,
      scd::allocator<pair<unsigned int, ai_component*>>>::iterator pIter;
  pIter = pAIManager->m_cAscd::base_components.begin();

  // Auto set to first place
  int nPlace = 1;

  // Check human player
  int nPlayerID = CIDGen::GetInstance()->GetID("Player0");
  int i = 0;
  for (; i < 3; ++i) {
    if (pIter->second->m_tKnowledge.m_cOpponents[i].m_nPlayer == nPlayerID) {
      break;
    }
  }

  if ((8 - m_tKnowledge.m_cNeededGoalItems.size())
      <= static_cast<unsigned int>(
             pIter->second->m_tKnowledge.m_cOpponents[i].m_chAmtGoalItems)) {
    ++nPlace;
  }

  while (pIter != pAIManager->m_cAscd::base_components.end()) {
    if (pIter->second->m_pObject->GetID() == m_pObject->GetID()) {
      // Don't check myself
      ++pIter;
      continue;
    }

    if (pIter->second->m_tKnowledge.m_cNeededGoalItems.size()
        <= m_tKnowledge.m_cNeededGoalItems.size()) {
      // This person has more Goal Items than I do
      ++nPlace;
    }

    ++pIter;
  }

  return nPlace;
}

void ai_component::PickRandomHeldItem() {
  switch (rand() % 11) {
  case 0:
  case 6: {
    SendStatusEffectEvent("UseBananaForward", this, this->m_pObject, 1.0);
    break;
  }
  case 1:
  case 7: {
    SendStatusEffectEvent("UsePeanutButterForward", this, this->m_pObject, 1.0);
    break;
  }
  case 2:
  case 8: {
    SendStatusEffectEvent("UseSoup", this, this->m_pObject, 1.0);
    break;
  }
  case 3: {
    SendStatusEffectEvent("UseJam", this, this->m_pObject, 1.0);
    break;
  }
  case 4:
  case 9: {
    SendStatusEffectEvent("UseDonut", this, this->m_pObject, 1.0);
    break;
  }
  case 5:
  case 10: {
    SendStatusEffectEvent("UseTurkeyForward", this, this->m_pObject, 1.0);
    break;
  }
  }
}
} // namespace scd
