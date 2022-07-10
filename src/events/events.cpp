/**
 * @file events.cpp
 *
 * @author Joseph Leybovich
 *
 * @brief Groups event structures with send functions.
 */
#include "events.h"

#include "console_manager.h"
#include "events/event_manager.h"
#include "events/ievent.h"

#include <cassert>

namespace scd::events {

void send(const std::string& event_name, base_component* sender, void* data,
          event_priority priority) {
  assert(sender != nullptr);

  event* _event
    = MMNEWEVENT(event(id_gen::get().get_id(event_name), sender, data));
  event_manager::get().post(_event, priority);
}

int update::create(lua_State* lua) {
  float delta_time = static_cast<float>(lua_tonumber(lua, -1));
  lua_pop(lua, 1);

  lua_pushlightuserdata(lua, MMNEWEVENT(update(delta_time)));

  return 1;
}

void update::send(const std::string& event_name, base_component* sender,
                  float delta_time, event_priority priority) {
  update* _update = MMNEWEVENT(update(delta_time));
  send(event_name, sender, _update, priority);
}

int EventStructs::CreateStateEvent(lua_State* pLua) {
  EGameState eTo = (EGameState)((int)lua_tonumber(pLua, -1));
  lua_pop(pLua, 1);

  lua_pushlightuserdata(pLua, MMNEWEVENT(TStateEvent(eTo)));

  return 1;
}

void EventStructs::SendStateEvent(string szEventName, IComponent* pSender,
                                  EGameState eTo, EEventPriority) {
  TStateEvent* ptStateData = MMNEWEVENT(TStateEvent(eTo));
  SendIEvent(szEventName, pSender, ptStateData, PRIORITY_IMMEDIATE);
}

int EventStructs::CreateRenderEvent(lua_State* pLua) {
  CObject* pParent = (CObject*)lua_topointer(pLua, -1);
  lua_pop(pLua, 1);

  lua_pushlightuserdata(pLua, MMNEWEVENT(TRenderEvent(pParent)));

  return 1;
}

void EventStructs::SendRenderEvent(string szEventName, IComponent* pSender,
                                   CObject* pParent, EEventPriority ePriority) {
  TRenderEvent* ptRenderData = MMNEWEVENT(TRenderEvent(pParent));
  SendIEvent(szEventName, pSender, ptRenderData, ePriority);
}

int EventStructs::CreateRamEvent(lua_State* pLua) {
  CObject* pRammer = (CObject*)lua_topointer(pLua, -2);
  CObject* pRammee = (CObject*)lua_topointer(pLua, -1);
  lua_pop(pLua, 2);

  lua_pushlightuserdata(pLua, MMNEWEVENT(TRamEvent(pRammer, pRammee)));

  return 1;
}

void EventStructs::SendRamEvent(string szEventName, IComponent* pSender,
                                CObject* pRammer, CObject* pRammee,
                                EEventPriority ePriority) {
  TRamEvent* ptRamData = MMNEWEVENT(TRamEvent(pRammer, pRammee));
  SendIEvent(szEventName, pSender, ptRamData, ePriority);
}

int EventStructs::CreateObjectEvent(lua_State* pLua) {
  CObject* pObj = (CObject*)lua_topointer(pLua, -1);
  lua_pop(pLua, 1);

  lua_pushlightuserdata(pLua, MMNEWEVENT(TObjectEvent(pObj)));

  return 1;
}

void EventStructs::SendObjectEvent(string szEventName, IComponent* pSender,
                                   CObject* pObj, EEventPriority ePriority) {
  TObjectEvent* ptObjData = MMNEWEVENT(TObjectEvent(pObj));
  SendIEvent(szEventName, pSender, ptObjData, ePriority);
}

int EventStructs::CreateInputEvent(lua_State* pLua) {
  CObject* pPlayer = (CObject*)lua_topointer(pLua, -2);
  float fAmount    = (float)lua_tonumber(pLua, -1);
  lua_pop(pLua, 2);

  lua_pushlightuserdata(pLua, MMNEWEVENT(TInputEvent(pPlayer, fAmount)));

  return 1;
}

void EventStructs::SendInputEvent(string szEventName, IComponent* pSender,
                                  CObject* pPlayer, float fAmount,
                                  EEventPriority) {
  TInputEvent* ptInputData = MMNEWEVENT(TInputEvent(pPlayer, fAmount));
  SendIEvent(szEventName, pSender, ptInputData, PRIORITY_INPUT);
}

int EventStructs::SendHeldItemCollectedEvent(lua_State* pLua) {
  string szEventName = lua_tostring(pLua, -3);
  string szItemName  = lua_tostring(pLua, -2);
  string szCollector = lua_tostring(pLua, -1);

  // SendHeldItemCollectedEvent(szEventName, (IComponent*)&Debug,

  lua_pop(pLua, 3);
  return 0;
}

void EventStructs::SendHeldItemCollectedEvent(string szEventName,
                                              IComponent* pSender,
                                              CObject* pcHeldItem,
                                              CObject* pcCollector,
                                              EEventPriority ePriority) {
  THeldItemCollected* ptItemData
    = MMNEWEVENT(THeldItemCollected(pcHeldItem, pcCollector));
  SendIEvent(szEventName, pSender, ptItemData, ePriority);
}

int EventStructs::CreateHeadingEvent(lua_State* pLua) {
  D3DXVECTOR3 vHeading;
  vHeading.x = (float)lua_tonumber(pLua, -3);
  vHeading.x = (float)lua_tonumber(pLua, -2);
  vHeading.x = (float)lua_tonumber(pLua, -1);
  lua_pop(pLua, 3);

  lua_pushlightuserdata(pLua, MMNEWEVENT(THeadingEvent(vHeading)));

  return 1;
}

void EventStructs::SendHeadingEvent(string szEventName, IComponent* pSender,
                                    D3DXVECTOR3 vHeading,
                                    EEventPriority ePriority) {
  THeadingEvent* ptHeadingData = MMNEWEVENT(THeadingEvent(vHeading));
  SendIEvent(szEventName, pSender, ptHeadingData, ePriority);
}

int EventStructs::CreateGoalItemEvent(lua_State* pLua) {
  EGoalItemType eType = (EGoalItemType)lua_tointeger(pLua, -2);
  CGoalItems* pGIC    = (CGoalItems*)lua_touserdata(pLua, -1);
  lua_pop(pLua, 2);

  lua_pushlightuserdata(pLua, MMNEWEVENT(TGoalItemEvent(eType, pGIC)));

  return 1;
}

void EventStructs::SendGoalItemEvent(string szEventName, IComponent* pSender,
                                     EGoalItemType eGoalItemType,
                                     CGoalItems* pGoalItem,
                                     EEventPriority ePriority) {
  TGoalItemEvent* ptGoalData
    = MMNEWEVENT(TGoalItemEvent(eGoalItemType, pGoalItem));
  SendIEvent(szEventName, pSender, ptGoalData, ePriority);
}

int EventStructs::CreateWeightClassEvent(lua_State* pLua) {
  ECartWeight eWeightClass = (ECartWeight)((int)lua_tonumber(pLua, -2));
  CObject* pObj            = (CObject*)(lua_touserdata(pLua, -1));
  lua_pop(pLua, 2);

  lua_pushlightuserdata(pLua,
                        MMNEWEVENT(TWeightClassEvent(eWeightClass, pObj)));

  return 1;
}

void EventStructs::SendWeightClassEvent(string szEventName, IComponent* pSender,
                                        CObject* pObjectToChange,
                                        ECartWeight eWeightClass,
                                        EEventPriority ePriority) {
  TWeightClassEvent* pWtData
    = MMNEWEVENT(TWeightClassEvent(eWeightClass, pObjectToChange));
  SendIEvent(szEventName, pSender, pWtData, ePriority);
}

int EventStructs::CreateGoalItemCollectedEvent(lua_State* pLua) {
  CObject* pGoalItem  = (CObject*)((int)lua_touserdata(pLua, -2));
  CObject* pCollector = (CObject*)((int)lua_touserdata(pLua, -1));
  lua_pop(pLua, 2);

  lua_pushlightuserdata(
    pLua, MMNEWEVENT(TGoalItemCollectedEvent(pGoalItem, pCollector)));

  return 1;
}

void EventStructs::SendGoalItemCollectedEvent(string szEventName,
                                              IComponent* pSender,
                                              CObject* pGoalItem,
                                              CObject* pCollector,
                                              EEventPriority ePriority) {
  TGoalItemCollectedEvent* ptGoalData
    = MMNEWEVENT(TGoalItemCollectedEvent(pGoalItem, pCollector));
  SendIEvent(szEventName, pSender, ptGoalData, ePriority);
}
int EventStructs::CreateImpactEvent(lua_State*) {
  //	CObject* pCollider = (CObject*)((int)lua_touserdata(pLua, -2));
  //	D3DXVECTOR3 vReflect = (D3DXVECTOR3)((D3DXVECTOR3)lua_touserdata(pLua,
  //-1)); 	lua_pop(pLua, 2); 	lua_pushlightuserdata(pLua,
  // MMNEWEVENT(TImpactEvent(pCollider, vReflect)));
  return -1; // shouldn't be called in lua anyway, so please don't call it in
             // lua
}
void EventStructs::SendImpactEvent(string szEventName, IComponent* pSender,
                                   CObject* pCollider, CObject* pCollidedwith,
                                   D3DXVECTOR3 vNormal,
                                   EEventPriority ePriority,
                                   D3DXVECTOR3 vColPt) {
  TImpactEvent* ptImpactData
    = MMNEWEVENT(TImpactEvent(pCollider, pCollidedwith, vNormal, vColPt));
  SendIEvent(szEventName, pSender, ptImpactData, ePriority);
}
// void EventStructs::SendImpactEvent(string szEventName, IComponent* pSender,
//								   CObject* pCollider, CObject* pCollidedwith, EEventPriority
// ePriority /* = PRIORITY_NORMAL */)
//{
//	TImpactEvent* ptImpactData = MMNEWEVENT( TImpactEvent(pCollider,
// pCollidedwith) ); 	SendIEvent(szEventName, pSender, ptImpactData, ePriority);
//}
void EventStructs::SendStatusEffectEvent(string szEventName,
                                         IComponent* pSender,
                                         CObject* pObjectToAffect,
                                         float fDuration,
                                         EEventPriority ePriority) {
  TStatusEffectEvent* ptStatusData
    = MMNEWEVENT(TStatusEffectEvent(pObjectToAffect, fDuration));
  SendIEvent(szEventName, pSender, ptStatusData, ePriority);
}

int EventStructs::CreateStatusEffectEvent(lua_State* pLua) {
  CObject* pObject = (CObject*)lua_touserdata(pLua, -2);
  float fDuration  = (float)lua_tonumber(pLua, -1);
  lua_pop(pLua, 2);

  lua_pushlightuserdata(pLua,
                        MMNEWEVENT(TStatusEffectEvent(pObject, fDuration)));

  return 1;
}

void EventStructs::SendFloatEvent(string szEventName, IComponent* pSender,
                                  float fValue, EEventPriority ePriority) {
  TFloatEvent* ptFloatData = MMNEWEVENT(TFloatEvent(fValue));
  SendIEvent(szEventName, pSender, ptFloatData, ePriority);
}

// void EventStructs::SendCharacterEvent(string szEventName, IComponent*
// pSender, 									 ECharacter echaracter, EEventPriority ePriority)
//{
//	TCharacterEvent* ptCharacterData = MMNEWEVENT(
//		TCharacterEvent(echaracter));
//	SendIEvent(szEventName, pSender, ptCharacterData, ePriority);
//}

void EventStructs::SendSpawnPickupItemEvent(string szEventName,
                                            IComponent* pSender, D3DXVECTOR3 v1,
                                            D3DXVECTOR3 v2, int nID,
                                            EEventPriority ePriority) {
  TSpawnPickupItemEvent* ptSendData
    = MMNEWEVENT(TSpawnPickupItemEvent(v1, v2, nID));
  SendIEvent(szEventName, pSender, ptSendData, ePriority);
}

void EventStructs::SendPickupItemCollectedEvent(string szEventName,
                                                IComponent* pSender,
                                                CObject* pPickupItem,
                                                CObject* pCollector,
                                                EGoalItemType eType = NO_ITEM,
                                                EEventPriority ePriority) {
  TPickupItemCollectedEvent* ptItemData
    = MMNEWEVENT(TPickupItemCollectedEvent(pPickupItem, pCollector, eType));
  SendIEvent(szEventName, pSender, ptItemData, ePriority);
}

void EventStructs::SendPickupItemEvent(string szEventName, IComponent* pSender,
                                       CObject* pGoalItem, EGoalItemType eType,
                                       EEventPriority ePriority) {
  TPickupItemEvent* ptItemData = MMNEWEVENT(TPickupItemEvent(pGoalItem, eType));
  SendIEvent(szEventName, pSender, ptItemData, ePriority);
}

void EventStructs::SendTwoIntEvent(string szEventName, IComponent* pSender,
                                   int n1, int n2, EEventPriority ePriority) {
  TTwoIntEvent* ptItemData = MMNEWEVENT(TTwoIntEvent(n1, n2));
  SendIEvent(szEventName, pSender, ptItemData, ePriority);
}

void EventStructs::SendMouseEvent(string szEventName, IComponent* pSender,
                                  LONG nX, LONG nY, EEventPriority ePriority) {
  TMouseEvent* ptMouseData = MMNEWEVENT(TMouseEvent(nX, nY));
  SendIEvent(szEventName, pSender, ptMouseData, ePriority);
}

int EventStructs::SendLuaEvent(lua_State* pLua) {
  string szEvent = lua_tostring(pLua, -1);
  lua_pop(pLua, 1);
  SendIEvent(szEvent, (IComponent*)&Debug, NULL, PRIORITY_NORMAL);

  return 0;
}

void EventStructs::SendConsoleEvent(string szEventName, IComponent* pSender,
                                    char chKey, EEventPriority ePriority) {
  TConsoleEvent* ptConsoleData = MMNEWEVENT(TConsoleEvent(chKey));
  SendIEvent(szEventName, pSender, ptConsoleData, ePriority);
}

void EventStructs::SendSpeedEvent(string szEventName, IComponent* pSender,
                                  CObject* pObject, float fSpeed,
                                  EEventPriority ePriority) {
  TSpeedEvent* ptSpeedData = MMNEWEVENT(TSpeedEvent(pObject, fSpeed));
  SendIEvent(szEventName, pSender, ptSpeedData, ePriority);
}

void EventStructs::SendHeldItemSpawnedEvent(string szEventName,
                                            IComponent* pSender,
                                            CHeldItemComponent* pHeldItem,
                                            EEventPriority ePriority) {
  THeldItemSpawned* ptSpawnedData = MMNEWEVENT(THeldItemSpawned(pHeldItem));
  SendIEvent(szEventName, pSender, ptSpawnedData, ePriority);
}

void EventStructs::SendStringEvent(string szEventName, IComponent* pSender,
                                   string& pString, EEventPriority ePriority) {
  TStringEvent* ptString = MMNEWEVENT(TStringEvent(pString));
  SendIEvent(szEventName, pSender, ptString, ePriority);
}

void EventStructs::SendStringEvent(string szEventName, IComponent* pSender,
                                   char* pString, EEventPriority ePriority) {
  TStringEvent* ptString = MMNEWEVENT(TStringEvent(pString));
  SendIEvent(szEventName, pSender, ptString, ePriority);
}

} // namespace scd::events
