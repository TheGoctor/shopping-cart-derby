#include "jam.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/held_item_manager.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"

namespace scd::component {
jam::jam(scd::object& owner)
    : base_component(owner) {
  JAM_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  std::string szEvent = "Update";
  szEvent += GAMEPLAY_STATE;
  CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);
  // CEventManager::GetInstance()->RegisterEvent("UseJam", this, ReInit);
  // register for events
}

std::shared_ptr<jam> jam::create(scd::object& owner) {
  return owner.create_component<jam>();
}
// call backs
void jam::on_update(float dt) {
  if (!is_spawned()) {
    return;
  }

  _time_remaining -= dt;

  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      JAM_ID,
      owner().world_position(),
      0.35f);

  if (time_remaining() <= 0.0f) {
    despawn();
    return;
  }
}
void jam::on_use() {
  // TStatusEffectEvent* eEvent = (TStatusEffectEvent*)cEvent->GetData();
  // m_fTimeLeft = m_fDuration;
  JAM_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      JAM_ID,
      owner().world_position(),
      0.35f);
  // SendStatusEffectEvent("JamEffect",pComp, eEvent->m_pObject,
  // pComp->m_fDuration);
  CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_JAM_USE, JAM_ID);
  // pComp->SetIsSpawned(true);
}
void jam::despawn() {
  CWwiseSoundManager::GetInstance()->UnregisterObject(JAM_ID);
  _is_spawned = false;
  owner().local_position({300, 300, 300});
}
} // namespace scd::component
