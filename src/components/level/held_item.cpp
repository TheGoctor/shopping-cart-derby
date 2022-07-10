/*******************************************************************************
 * Filename:  		CHeldItemComponent.cpp
 * Date:      		05/01/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This will represent the Held Item in the world.
          When created, Init should be called and a EHeldItemType
          should be given.
 ******************************************************************************/
#include "held_item.h"

#include "components/rendering/renderable.h"
#include "components/spawning_manager.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "rendering/dx_render_context_manager.h"

namespace scd {

CHeldItemComponent::CHeldItemComponent(scd::object* pObj)
    : m_pParent(pObj) {
  string szEvent = "Update";
  szEvent += GAMEPLAY_STATE;
  event_manager.register_event(szEvent, this, Update);

  szEvent = "Update";
  szEvent += PAUSE_STATE;
  event_manager.register_event(szEvent, this, PauseUpdateCallback);

  szEvent = "Update";
  szEvent += CONSOLE_STATE;
  event_manager.register_event(szEvent, this, PauseUpdateCallback);

  szEvent = "Update";
  szEvent += PAUSE_OPTIONS_STATE;
  event_manager.register_event(szEvent, this, PauseUpdateCallback);

  szEvent = "Update";
  szEvent += PAUSE_KEYBINDS_STATE;
  event_manager.register_event(szEvent, this, PauseUpdateCallback);

  szEvent = "Update";
  szEvent += QUIT_CONFIRMATION_STATE;
  event_manager.register_event(szEvent, this, PauseUpdateCallback);

  szEvent = "Update";
  szEvent += IN_GAME_HOW_TO_PLAY_STATE;
  event_manager.register_event(szEvent, this, PauseUpdateCallback);
  szEvent = "Update";
  szEvent += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
  event_manager.register_event(szEvent, this, PauseUpdateCallback);
}

CHeldItemComponent::CHeldItemComponent() {}

CHeldItemComponent::~CHeldItemComponent() {}

void CHeldItemComponent::Init() {
  m_bSpawned = false;
  m_fSpawnTimer = 1;
  m_eType = NO_HELD_ITEM;

  event_manager.register_event("HeldItemCollision", this, HeldItemCollision);
  event_manager.register_event("HeldItemCollected", this, HeldItemCollected);
}

void CHeldItemComponent::Spawn() {
  m_bSpawned = true;
  m_eType = (EHeldItemType)(rand() % MAX_HELD_ITEMS);

  list<TSpawnLocation, scd::allocator<TSpawnLocation>>::iterator pIter =
      CSpawningManager::GetInstance()->m_cHeldItemLocations.begin();

  // get a random number to figure out which location to spawn at (arbitrary
  // number)
  int nRandNumber =
      rand() % 64 + 1; // since the number of endcaps is ~32, we aproximate the
                       // range as double so we dont get skewed results
  bool bFoundAnEmptySpawnPoint = false;

  // get the last unused spot of a random number of endcaps
  while (pIter != CSpawningManager::GetInstance()->m_cHeldItemLocations.end()) {
    // if the place is unused
    if ((*pIter).m_bUsed == false) {
      // use this location
      m_tSpawnLocation = &(*pIter);
      bFoundAnEmptySpawnPoint = true;
    }

    pIter++;
    // if we're at the end, loop around
    if (pIter == CSpawningManager::GetInstance()->m_cHeldItemLocations.end()) {
      pIter = CSpawningManager::GetInstance()->m_cHeldItemLocations.begin();
    }

    // decrement our counter
    nRandNumber--;

    // if that's the last spawn point we need to check
    if (nRandNumber == 0) {
      break;
    }
  }

  // if we didn't find an empty spawn point
  if (!bFoundAnEmptySpawnPoint) {
    // return out of here
    return;
  }

  m_tSpawnLocation->m_bUsed = true;

  SendHeldItemSpawnedEvent("HeldItemSpawned", this, this);
  SendObjectEvent("BlueLightSpecialSpawned", this, m_tSpawnLocation->m_cEndCap);

  D3DXMatrixIdentity(&m_pParent->GetTransform()->GetLocalMatrix());

  // offset to the center of the endcap
  scd::transform transToCenterOfEndcap;
  D3DXMatrixTranslation(&transToCenterOfEndcap, -.5f, 0.0f, .45f);
  m_pParent->GetTransform()->GetLocalMatrix() =
      m_pParent->GetTransform()->GetLocalMatrix() * transToCenterOfEndcap;

  // mult by the translation and rotation matrix
  m_pParent->GetTransform()->GetLocalMatrix() =
      m_pParent->GetTransform()->GetLocalMatrix()
      * m_tSpawnLocation->m_tRotation;

  // get the context and set this mesh and context to the proper thing
  DXRenderContext* pContext = DXRenderContextManager::GetInstance()->GetContext(
      (ERenderContext)CSpawningManager::GetInstance()
          ->m_nHeldItemRenderCompIDs[m_eType]);
  m_pRenderComponent->SetMesh(
      CSpawningManager::GetInstance()->m_nHeldItemMeshIDs[m_eType], pContext);
}

void CHeldItemComponent::Despawn() {
  m_bSpawned = false;
  m_fSpawnTimer = 1.0f; // time to not be spawned until respawn

  // if we have a spawn location
  if (m_tSpawnLocation) {
    // unuse it and reset it
    m_tSpawnLocation->m_bUsed = false;
    m_tSpawnLocation = NULL;
  }
  m_pParent->GetTransform()->TranslateFrame(scd::vector3(100, 100, 100));
}

void CHeldItemComponent::Update(
    IEvent* piEvent,
    scd::base_component* piComponent) {
  // Get component
  CHeldItemComponent* pcHeldItem = (CHeldItemComponent*)piComponent;

  // Get the fDT
  float fDT = ((TUpdateStateEvent*)piEvent->GetData())->m_fDeltaTime;

  // Update the spawn timer
  pcHeldItem->m_fSpawnTimer -= fDT;

  // If spawned, update
  if (pcHeldItem->m_bSpawned) {
    // if we don't actually have a spawn location (and we're spawned)
    if (pcHeldItem->m_tSpawnLocation == NULL) {
      // we're probably at the origin so let's despawn so it's not ugly
      pcHeldItem->Despawn();
    }

    SendRenderEvent(
        "AddToSet", pcHeldItem, pcHeldItem->m_pParent, PRIORITY_IMMEDIATE);
    pcHeldItem->m_pParent->GetTransform()->GetLocalMatrix()._42 =
        sin(pcHeldItem->m_fSpawnTimer * 2.5f) * .2f + 1.3f;
    pcHeldItem->m_pParent->GetTransform()->RotateFrame(
        scd::vector3(0, 1, 0), fDT * 1.5f);

    return;
  }

  if (pcHeldItem->m_fSpawnTimer < 0.0f) {
    pcHeldItem->Spawn();
  }
}

void CHeldItemComponent::HeldItemCollision(
    IEvent* iEvent,
    scd::base_component* comp) {
  THeldItemCollected* tEvent = (THeldItemCollected*)iEvent->GetData();
  CHeldItemComponent* pComp = (CHeldItemComponent*)comp;

  scd::object* pHeldItem = tEvent->m_pcHeldItem;
  scd::object* pCollector = tEvent->m_pcCollector;

  if (pHeldItem == pComp->m_pParent) {
    // if we have valid endcaps and location
    if (pComp->m_tSpawnLocation && pComp->m_tSpawnLocation->m_cEndCap) {
      SendObjectEvent(
          "BlueLightSpecialDespawned",
          pComp,
          pComp->m_tSpawnLocation->m_cEndCap);
    }
    // Tell everyone else
    SendGoalItemCollectedEvent(
        "HeldItemCollected", pComp, pHeldItem, pCollector, PRIORITY_IMMEDIATE);
  }
}

void CHeldItemComponent::HeldItemCollected(
    IEvent* iEvent,
    scd::base_component* comp) {
  THeldItemCollected* tEvent = (THeldItemCollected*)iEvent->GetData();
  CHeldItemComponent* pComp = (CHeldItemComponent*)comp;

  scd::object* pHeldItem = tEvent->m_pcHeldItem;

  if (pHeldItem == pComp->m_pParent) {
    pComp->Despawn();
  }
}

void CHeldItemComponent::PauseUpdateCallback(
    IEvent*,
    scd::base_component* pComp) {
  // Get the Effect Comp
  CHeldItemComponent* comp = (CHeldItemComponent*)pComp;

  if (comp->m_bSpawned) {
    SendRenderEvent("AddToSet", comp, comp->m_pParent, PRIORITY_UPDATE);
  }
}
} // namespace scd
