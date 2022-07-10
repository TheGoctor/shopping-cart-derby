#include "turkey.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/held_item_manager.h"
#include "core/object.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "physics/physics_manager.h"
#include "rendering/effect_manager.h"

namespace scd::components {
turkey(object& owner)
    : base_component(owner) {
  TURKEY_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();

  // TODO: subscribe to events
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);

  CEventManager::GetInstance()->RegisterEvent(
      "HeldItemInWorldCollision", this, EnvironmentCollision);

  CEventManager::GetInstance()->RegisterEvent(
      "HeldItemInWorldPlayerCollision", this, PlayerCollision);

  CEventManager::GetInstance()->RegisterEvent(
      "TurkeyDestroyedByItem", this, ItemCollision);

  szEventName = "Update";
  szEventName += PAUSE_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += CONSOLE_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += PAUSE_OPTIONS_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += PAUSE_KEYBINDS_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += QUIT_CONFIRMATION_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += IN_GAME_HOW_TO_PLAY_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);

  SendObjectEvent("TurkeyCreated", this, _owner, PRIORITY_IMMEDIATE);
}

std::shared_ptr<turkey> turkey::create(object& owner, const vector3& direction) {
  auto comp = owner.create_component<turkey>();

  comp->_id = -1;
  comp->_move_direction = direction;
  D3DXVec3Normalize(&comp->_move_direction, &comp->_move_direction);

  return comp;
}

void turkey::ReInit() {
  m_fTimeLeft = m_fDuration;
  m_bSpawned = true;
  TURKEY_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  TSphere tsphere;
  //	tsphere.cPosition = scd::vector3(0,5,0);
  tsphere.cPosition = _owner.world_position();
  tsphere.fRadius = .5f; // TODO: Is this the radius we want?
  m_pCollidableComponent->SetBVType(BSPHERE);
  m_pCollidableComponent->SetSphere(tsphere);

  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      TURKEY_ID, _owner.world_position(), 0.5f);
  CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_TURKEY_USE, TURKEY_ID);

  // let effect stuff know we're fired
  SendObjectEvent("TurkeyEffect", this, m_pObject, PRIORITY_IMMEDIATE);
}

void turkey::despawn() {
  m_bSpawned = false;
  _owner.local_position({300, 300, 300});

  CWwiseSoundManager::GetInstance()->PlayTheSound(
      ITEM_TURKEY_TRAVEL_STOP, TURKEY_ID);
  CWwiseSoundManager::GetInstance()->UnregisterObject(TURKEY_ID);

  // instead, just set the sphere's radius to 0
  TSphere tsphere{};
  tsphere.cPosition = _owner.world_position();
  tsphere.fRadius = 0.0f;

  SendObjectEvent("TurkeyDespawned", this, _owner);
}

void turkey::on_update(float dt) {
  // if w'ere not spawned
  if (!is_spawned()) {
    // no need to update
    return;
  }

  _time_remaining -= dt;

  if (_time_remaining <= 0.0f) {
    despawn();
    return;
  }

  // TODO: Play effect stuff here (frost, etc)

  // translate the object
  _owner.translate(_move_direction * dt * _speed);

  _owner.rotate({0.0f, 1.0f, 0.0f}, dt * 5.0f);

  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      TURKEY_ID, _owner.world_position(), 0.25f);

  // tell it to render
  SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_IMMEDIATE);
}

// Collision on world
void turkey::EnvironmentCollision(IEvent* cEvent) {
  TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

  // collider is the item, collided is the world geometry
  if (_owner == tEvent->m_pcCollider && is_spawned()) {
    scd::vector3 MoveDir = _owner.local_position();
    D3DXVec3Normalize(&MoveDir, &MoveDir);

    ColNormal = tEvent->m_vNormal;
    D3DXVec3Normalize(&ColNormal, &ColNormal);

    if (ColNormal.x > 0.0f || ColNormal.x <= 0.0f && ColNormal.z == 0.0f) {
      _move_direction.x *= -1.0f;
    } else if (
        ColNormal.z > 0.0f || ColNormal.z < 0.0f && ColNormal.x == 0.0f) {
      _move_direction.z *= -1.0f;
    } else {
      // R = -2*(V dot N)*N - V reflection formula
      scd::vector3 ref =
          -2 * (D3DXVec3Dot(&_move_direction, &ColNormal)) * ColNormal
          - _move_direction;
      D3DXVec3Normalize(&_move_direction, &ref);
    }

    CWwiseSoundManager::GetInstance()->PlayTheSound(
        ITEM_TURKEY_EN_IMPACT, TURKEY_ID); // HACK: not global id
  }
}

// collision on player
void turkey::on_player_collision(IEvent* cEvent) {
  TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

  // collider is cart, collided is item
  if (_owner == tEvent->m_pCollidedWith && _is_spawned) {
    // send stun effect event to player
    SendStatusEffectEvent("Stun", this, tEvent->m_pcCollider, _stun_duration);

    // play sound
    CWwiseSoundManager::GetInstance()->PlayTheSound(
        ITEM_TURKEY_IMPACT, TURKEY_ID);

    CWwiseSoundManager::GetInstance()->PlayTheSound(STATUS_STUN, TURKEY_ID);

    // destroy me
    despawn();

    // tell who we rammed to drop an item
    SendRamEvent("PlayerRammed", this, _owner, tEvent->m_pcCollider);
  }
}

void turkey::on_item_collision(IEvent* cEvent) {
  TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();
  if (_owner == tEvent->m_pcCollider && _is_spawned) {
    // HACK: not global id
    CWwiseSoundManager::GetInstance()->PlayTheSound(
        ITEM_TURKEY_IMPACT, TURKEY_ID);

    // effect stuff
    scd::vector3 p1, p2, trans;
    p2 = _collider->GetSphere().cPosition;
    p1 = CCollisionManager::GetInstance()
             ->GetTurkeyDestroyObject()
             ->GetTransform()
             ->GetWorldPosition();

    trans = p2 - p1; // vector to the turkeys position

    CCollisionManager::GetInstance()
        ->GetTurkeyDestroyObject()
        ->GetTransform()
        ->TranslateFrame(trans);

    CEffectComponent* pEC = CEffectManager::GetInstance()->CreateStunComponent(
        CCollisionManager::GetInstance()->GetTurkeyDestroyObject());

    pEC->SetDeadTimer(EC_TYPE_TURKEY_STUN, 0.0f);
    pEC->SwitchOnOffEmitters(EC_TYPE_TURKEY_STUN, true);

    despawn();
  }
}

void turkey::on_pause_update(float dt) {
  if (is_spawned()) {
    SendRenderEvent("AddToSet", this, _owner, PRIORITY_UPDATE);
  }
}

} // namespace scd::components
