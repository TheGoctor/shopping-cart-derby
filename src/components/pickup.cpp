#include "pickup.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/pickup_manager.h"
#include "core/object.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"

scd::pickup::pickup(scd::object& owner)
  : base_component(owner)
  , m_bSpawned(false) {}

// Lua stuff
// CreatePickupItemComponent(pObj, flyDir.x, flyDir.y, flyDir.z)
int scd::pickup::create(lua_State* pLua) {
  scd::object* pObj = (scd::object*)lua_topointer(pLua, -4);
  scd::vector3 vDir;
  vDir.x = (float)lua_tonumber(pLua, -3);
  vDir.y = (float)lua_tonumber(pLua, -2);
  vDir.z = (float)lua_tonumber(pLua, -1);

  /*CPickupItemComponent* pPickupItemComponent =*/create(pObj, vDir);

  lua_pop(pLua, 4);
  return 0;
}

scd::pickup* scd::pickup::create(scd::object& obj, scd::vector3 vDirection) {
  pickup* comp = MMNEW(pickup(pObj));

  comp->Init();
  comp->m_vDirectionToMove = vDirection;

  D3DXVec3Normalize(&comp->m_vDirectionToMove, &comp->m_vDirectionToMove);

  pObj->AddComponent(comp);
  pickup_manager::get().AddComponent(comp);

  return comp;
}
//	//Lua Stuff

void scd::pickup::EventInit() {
  // listens for these events
  string szEvent = "Update";
  szEvent += GAMEPLAY_STATE;
  event_manager.register_event(szEvent, this, Update);

  szEvent = "Update";
  szEvent += PAUSE_STATE;
  event_manager.register_event(
    szEvent, this, PauseUpdateCallback);

  szEvent = "Update";
  szEvent += CONSOLE_STATE;
  event_manager.register_event(
    szEvent, this, PauseUpdateCallback);

  szEvent = "Update";
  szEvent += PAUSE_OPTIONS_STATE;
  event_manager.register_event(
    szEvent, this, PauseUpdateCallback);
  szEvent = "Update";
  szEvent += PAUSE_KEYBINDS_STATE;
  event_manager.register_event(
    szEvent, this, PauseUpdateCallback);
  szEvent = "Update";
  szEvent += QUIT_CONFIRMATION_STATE;
  event_manager.register_event(
    szEvent, this, PauseUpdateCallback);
  szEvent = "Update";
  szEvent += IN_GAME_HOW_TO_PLAY_STATE;
  event_manager.register_event(
    szEvent, this, PauseUpdateCallback);
  szEvent = "Update";
  szEvent += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
  event_manager.register_event(
    szEvent, this, PauseUpdateCallback);

  event_manager.register_event(
    "PickupItemCollision", this, PickupItemCollisionCallback);
}

void scd::pickup::Init() {
  m_fLifetime     = 15.0f;
  m_fTimeLeft     = 15.0f;
  m_fFlyOutSpeed  = 8.0f;
  m_fTimeToFly    = 1.0f;
  m_fMaxArcHeight = 4.0f;
  m_bSpawned      = true;
  m_bRendering    = true;
  // m_nItemType = -1;
  m_nItemType = NO_ITEM;
}

void scd::pickup::on_update(float dt) {
  if (!m_bSpawned) {
    return;
  }

  m_fTimeLeft -= fDt;
  if (m_fTimeLeft <= 0.0f) {
    // TODO: destroy me sire!
    DestroyPickupItem(this);
    return;
  } else if (!ready_to_collide()) // if we're still flying
  {
    owner.GetTransform()->GetLocalMatrix()._41
      += comp->m_vDirectionToMove.x * fDt * comp->m_fFlyOutSpeed;
    comp->m_pObject->GetTransform()->GetLocalMatrix()._43
      += comp->m_vDirectionToMove.z * fDt * comp->m_fFlyOutSpeed;

    // set arc height
    comp->m_pObject->GetTransform()->GetLocalMatrix()._42
      = comp->GetArcYValue();

  } else // we're ready to be picked up
  {
    // oscillate on the Y axis (bounce up and down)
    comp->m_pObject->GetTransform()->GetLocalMatrix()._42
      = 1.0f + .5f * sin(comp->m_fTimeLeft);
  }

  // flash code
  if (comp->m_bRendering) {
    // HACK: BVH will handle this once implemented
    SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_NORMAL);
  }
  if (comp->m_fTimeLeft < 5.0f) {
    comp->m_bRendering = !comp->m_bRendering;
  }
}

void scd::pickup::DestroyPickupItem(CPickupItemComponent* toDestroy) {
  pickup_manager::get().despawn(toDestroy);
}

void scd::pickup::on_collision() {
  // if we were the ones collected
  if (tEvent->m_pcGoalItem == owner && ready_to_collide()) {
    // tell the collector they collected a goal item
    SendPickupItemCollectedEvent("PickupItemCollected",
                                 this,
                                 owner(),
                                 tEvent->m_pcCollector,
                                 (EGoalItemType)pComp->m_nItemType);

    // destroy us
    DestroyPickupItem(pComp);
  }
}

void scd::pickup::on_pause() {
  if (m_bSpawned) {
    SendRenderEvent("AddToSet", comp, owner, PRIORITY_UPDATE);
  }
}
