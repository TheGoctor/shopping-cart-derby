#include "held_item_manager.h"

#include "components/rendering/renderable.h"
#include "components/spawning_manager.h"
#include "core/object.h"
#include "core/object_manager.h"
#include "events/events.h"
#include "physics/physics_manager.h"
#include "rendering/dx_render_context.h"
#include "rendering/dx_render_context_manager.h"
#include "rendering/model_manager.h"
#include "rendering/renderer.h"

#define NUM_TURKEYS (10)
#define NUM_DONUTS (4)
#define NUM_BOOSTS (4)
#define NUM_BANANAS (8)
#define NUM_CREAMPIE (6)

namespace scd {
held_item_manager::held_item_manager() {
  nDonutsCreated = 0;
  nBananasCreated = 0;
  nEnergyDrinksCreated = 0;
  nPiesCreated = 0;
  nDonutsCreated = 0;
  nJamsCreated = 0;
  nPeanutButtersCreated = 0;
  nTurkeysCreated = 0;

  event_manager.register_event(
      "UsePieForward", (scd::base_component*)this, UsePieCallback);
  event_manager.register_event(
      "UseBananaForward", (scd::base_component*)this, UseBananaCallback);
  event_manager.register_event(
      "UseTurkeyForward", (scd::base_component*)this, UseTurkeyCallback);
  event_manager.register_event(
      "UseBoost", (scd::base_component*)this, UseBoostCallback);
  event_manager.register_event(
      "UsePeanutButterForward",
      (scd::base_component*)this,
      UsePeanutButterCallback);
  event_manager.register_event(
      "UseSoup", (scd::base_component*)this, UseSoupCallback);
  event_manager.register_event(
      "UseDonut", (scd::base_component*)this, UseDonutCallback);

  string szEventName = "ShutdownObjects";
  szEventName += GAMEPLAY_STATE;
  event_manager.register_event(
      szEventName, (scd::base_component*)this, ShutdownCallback);
}

void held_item_manager::on_use_turkey(IEvent* cEvent) {
  TStatusEffectEvent* tEvent = (TStatusEffectEvent*)cEvent->GetData();
  held_item_manager* me = (held_item_manager*)cCenter;

  // set its pos to the user's pos
  scd::vector3 vMove(
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._31,
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._32,
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._33);
  D3DXVec3Normalize(&vMove, &vMove);

  scd::vector3 vPosToSpawnAt =
      tEvent->m_pObject->GetTransform()->GetWorldPosition() + vMove * 2.0f;

  CTurkeyComponent* comp = me->GetATurkey(vPosToSpawnAt);

  // reinit its values
  comp->ReInit();

  comp->SetPosition(vPosToSpawnAt);
  // set its direction to obj's heading
  comp->SetDirection(vMove);

  CCollisionManager* pCM = CCollisionManager::GetInstance();
  TSphere objsphere, parentsphere;
  objsphere.fRadius = parentsphere.fRadius =
      pCM->GetNonStaticByID(comp->GetParent()->GetID())->GetSphere().fRadius;
  objsphere.cPosition =
      pCM->GetNonStaticByID(comp->GetParent()->GetID())->GetSphere().cPosition;

  parentsphere.cPosition = objsphere.cPosition - vMove * 1.5f;
  scd::vector3 vectoitem = parentsphere.cPosition - objsphere.cPosition;
  pCM->GetNonStaticByID(comp->GetParent()->GetID())->SetSphere(parentsphere);
  if (pCM->TestIndividualCollisionsSphere(
          pCM->GetNonStaticByID(comp->GetParent()->GetID()))) {
    pCM->GetNonStaticByID(comp->GetParent()->GetID())->SetSphere(objsphere);
    SendStatusEffectEvent(
        "Stun", comp, tEvent->m_pObject, comp->GetStunDuration());
    SendRamEvent(
        "PlayerRammed",
        comp,
        comp->GetParent(),
        tEvent->m_pObject,
        PRIORITY_IMMEDIATE);
    //		SendStatusEffectEvent("Blind", comp, tEvent->m_pObject,
    // BLIND_TIME);
    comp->Despawn();
  } else {
    pCM->GetNonStaticByID(comp->GetParent()->GetID())->SetSphere(objsphere);
  }
}
void held_item_manager::UsePieCallback(
    IEvent* cEvent,
    scd::base_component* cCenter) {
  TStatusEffectEvent* tEvent = (TStatusEffectEvent*)cEvent->GetData();
  held_item_manager* pMe = (held_item_manager*)cCenter;

  scd::vector3 vMove(
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._31,
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._32,
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._33);
  D3DXVec3Normalize(&vMove, &vMove);
  scd::vector3 vSpawnPos =
      tEvent->m_pObject->GetTransform()->GetWorldPosition() + vMove * 2.0f;

  CPie* pComp = pMe->GetAPie(vSpawnPos);

  pComp->ReInit();
  //	tEvent->m_pObject->
  pComp->SetPosition(vSpawnPos);
  pComp->SetDirection(vMove);

  CCollisionManager* pCM = CCollisionManager::GetInstance();
  TSphere objsphere, parentsphere;
  objsphere.fRadius = parentsphere.fRadius =
      pCM->GetNonStaticByID(pComp->GetParent()->GetID())->GetSphere().fRadius;
  objsphere.cPosition =
      pCM->GetNonStaticByID(pComp->GetParent()->GetID())->GetSphere().cPosition;

  parentsphere.cPosition = objsphere.cPosition - vMove * 1.6f;
  scd::vector3 vectoitem = parentsphere.cPosition - objsphere.cPosition;
  pCM->GetNonStaticByID(pComp->GetParent()->GetID())->SetSphere(parentsphere);
  if (pCM->TestIndividualCollisionsSphere(
          pCM->GetNonStaticByID(pComp->GetParent()->GetID()))) {
    pCM->GetNonStaticByID(pComp->GetParent()->GetID())->SetSphere(objsphere);
    SendStatusEffectEvent("Blind", pComp, tEvent->m_pObject, BLIND_TIME);
    pComp->Despawn();
  } else {
    pCM->GetNonStaticByID(pComp->GetParent()->GetID())->SetSphere(objsphere);
  }
}
void held_item_manager::UseBananaCallback(
    IEvent* cEvent,
    scd::base_component* cCenter) {
  TStatusEffectEvent* tEvent = (TStatusEffectEvent*)cEvent->GetData();
  held_item_manager* pMe = (held_item_manager*)cCenter;
  CBanana* pComp = pMe->GetABanana();
  // reinit this banan
  pComp->ReInit();
  scd::vector3 vMove(
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._31,
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._32,
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._33);
  D3DXVec3Normalize(&vMove, &vMove);
  scd::vector3 vSpawnPos =
      tEvent->m_pObject->GetTransform()->GetWorldPosition() - vMove * 1.7f;
  pComp->SetPosition(vSpawnPos);
}

void held_item_manager::UsePeanutButterCallback(
    IEvent* cEvent,
    scd::base_component* cCenter) {
  TStatusEffectEvent* tEvent = (TStatusEffectEvent*)cEvent->GetData();
  held_item_manager* pMe = (held_item_manager*)cCenter;
  CPeanutButter* pComp = pMe->GetAPeanutButter();

  pComp->ReInit();
  scd::vector3 vMove(
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._31,
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._32,
      tEvent->m_pObject->GetTransform()->GetWorldMatrix()._33);
  scd::vector3 vSpawnPos =
      tEvent->m_pObject->GetTransform()->GetWorldPosition() - vMove * 3.5f;
  pComp->SetPosition(vSpawnPos);
}

void held_item_manager::UseBoostCallback(
    IEvent* cEvent,
    scd::base_component* cCenter) {
  // TStatusEffectEvent* tEvent = (TStatusEffectEvent*)cEvent->GetData();
  held_item_manager* pMe = (held_item_manager*)cCenter;

  /*CEnergyDrink* comp =*/pMe->GetAnEnergyDrink()->ReInit(cEvent, cCenter);

  // reinit its values
  // comp->ReInit(cEvent, cCenter);
}

void held_item_manager::UseSoupCallback(
    IEvent* cEvent,
    scd::base_component* cCenter) {
  TStatusEffectEvent* tEvent = (TStatusEffectEvent*)cEvent->GetData();
  held_item_manager* pMe = (held_item_manager*)cCenter;

  CChickenSoupComponent* pComp = pMe->GetAChickenSoup();

  // call this before ReInit() so that the attached object can be sent properly
  // in the invulnerable event
  pComp->SetAttachedObject(tEvent->m_pObject);

  pComp->ReInit();
}

void held_item_manager::UseDonutCallback(
    IEvent* cEvent,
    scd::base_component* cCenter) {
  TStatusEffectEvent* tEvent = (TStatusEffectEvent*)cEvent->GetData();
  held_item_manager* pMe = (held_item_manager*)cCenter;

  CDonut* pComp = pMe->GetADonut();

  // call this before ReInit() so that the attached object can be sent properly
  // in the invulnerable event
  pComp->SetAttachedObject(tEvent->m_pObject);

  pComp->ReInit();
}

void held_item_manager::UseRayJamCallback(
    IEvent* cEvent,
    scd::base_component* cCenter) {
  // TStatusEffectEvent* tEvent = (TStatusEffectEvent*)cEvent->GetData();
  held_item_manager* pMe = (held_item_manager*)cCenter;
  /*CRayJam* pComp =*/pMe->GetARayJam()->ReInit(cEvent, cCenter);
  // pComp->ReInit(cEvent, cCenter);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CTurkeyComponent* held_item_manager::GetATurkey(scd::vector3 vPos) {
  list<CTurkeyComponent*, scd::allocator<CTurkeyComponent*>>::iterator iter;
  iter = m_lTurkeyComps.begin();

  while (iter != m_lTurkeyComps.end()) {
    // if we're not spawned
    if (!(*iter)->Spawned()) {
      // break so we can grab it down below
      break;
    }
    iter++;
  }

  // if we DID find an unused item in the list (aka "down below")
  if (iter != m_lTurkeyComps.end()) {
    // get that item and return it
    CTurkeyComponent* pComp = *iter;
    return pComp;
  }

  // got here because we didn't find an unused item in the list

  // create a new turkey
  scd::vector3 vObjPos(vPos.x, vPos.y, vPos.z);
  string szObjName = "TurkeyComponent";
  szObjName += (char)(nTurkeysCreated + '0');
  nTurkeysCreated++;

  scd::object* pObj = scd::objectManager::CreateObject(szObjName);

  // create this component
  CTurkeyComponent* pComponent =
      CTurkeyComponent::CreateTurkeyComponent(pObj, vObjPos);

  // add Render component
  /*CRenderComponent* pRenderComp = */ Renderer::CreateRenderComp(
      pObj,
      ModelManager::GetInstance()->GetMeshIndexByName("TurkeyShape", false),
      RC_TURKEY,
      2); // 2 for textured\

  // Add a collision component
  TSphere tsphere;
  tsphere.cPosition = vObjPos;
  tsphere.fRadius = 0.5f; // TODO: Is this the radius we want?
  CCollideable* pCol =
      CCollisionManager::GetInstance()->CreateCollideableComponent(
          pObj, false, true, OBJTURKEY);
  //		pObj, false, true, OBJTURKEY, false);
  pCol->SetBVType(BSPHERE);
  pCol->SetSphere(tsphere);

  // store the collidable component
  pComponent->SetCollidableComponent(pCol);

  pObj->GetTransform()->ScaleFrame(2.8f, 2.8f, 2.8f);

  // add to the list
  m_lTurkeyComps.push_back(pComponent);

  return pComponent;
}

CBanana* held_item_manager::GetABanana() {
  list<CBanana*, scd::allocator<CBanana*>>::iterator iter;
  iter = m_lBananaComps.begin();

  while (iter != m_lBananaComps.end()) {
    // if we're not spawned
    if (!(*iter)->GetIsSpawned()) {
      // break so we can grab it down below
      break;
    }
    iter++;
  }

  // if we DID find an unused item in the list (aka "down below")
  if (iter != m_lBananaComps.end()) {
    // get that item and return it
    CBanana* pComp = *iter;
    return pComp;
  }

  // got here because we didn't find an unused item in the list

  // create a new banana
  scd::vector3 vObjPos(0.0f, 0.0f, 0.0f);
  string szObjName = "BananaComponent";
  szObjName += (char)(nBananasCreated + '0');
  nBananasCreated++;

  scd::object* pObj = scd::objectManager::CreateObject(szObjName);

  // create this component
  CBanana* pComponent = CBanana::CreateBananaComponent(pObj);

  // add Render component
  /*CRenderComponent* pRenderComp = */ Renderer::CreateRenderComp(
      pObj,
      CSpawningManager::GetInstance()->GetHeldItemMeshID(BANANA),
      CSpawningManager::GetInstance()->GetHeldItemRenderCompID(BANANA),
      2); // 2 for textured\

  // Add a collision component
  TSphere tsphere;
  tsphere.cPosition = vObjPos;
  tsphere.fRadius = .5f; // TODO: Is this the radius we want?
  CCollideable* pCol =
      CCollisionManager::GetInstance()->CreateCollideableComponent(
          pObj, false, true, OBJBANANA);
  pCol->SetBVType(BSPHERE);
  pCol->SetSphere(tsphere);

  pObj->GetTransform()->ScaleFrame(2.0f, 2.0f, 2.0f);

  // add to the list
  m_lBananaComps.push_back(pComponent);

  return pComponent;
}

CChickenSoupComponent* held_item_manager::GetAChickenSoup() {
  list<CChickenSoupComponent*, scd::allocator<CChickenSoupComponent*>>::iterator
      iter;
  iter = m_lChickenSoupComps.begin();

  while (iter != m_lChickenSoupComps.end()) {
    // if we're not spawned
    if (!(*iter)->GetIsSpawned()) {
      // break so we can grab it down below
      break;
    }
    iter++;
  }

  // if we DID find an unused item in the list (aka "down below")
  if (iter != m_lChickenSoupComps.end()) {
    // get that item and return it
    CChickenSoupComponent* pComp = *iter;
    return pComp;
  }

  // got here because we didn't find an unused item in the list

  // create a new soup
  scd::vector3 vObjPos(0.0f, 0.0f, 0.0f);
  string szObjName = "SoupComponent";
  szObjName += (char)(nSoupsCreated + '0');
  nSoupsCreated++;

  scd::object* pObj = scd::objectManager::CreateObject(szObjName);

  // create this component
  CChickenSoupComponent* pComponent =
      CChickenSoupComponent::CreateChickenSoupComponent(pObj);

  // int meshindex =
  //	ModelManager::GetInstance()->GetMeshIndexByName("BubbleShape1", false,
  // false);

  //// add Render component
  ///*CRenderComponent* pRenderComp = */Renderer::CreateRenderComp(pObj,
  //	meshindex,
  //	RC_CS_BUB2,
  //	RF_INDEXED_VERT_TEX2); // 2 for textured\

  // HACK MAKE A GIANT PIE FOR THIS MESH
  // pObj->GetTransform()->ScaleFrame(4.0f, 4.0f, 4.0f);

  // add to the list
  m_lChickenSoupComps.push_back(pComponent);

  return pComponent;
}

CEnergyDrink* held_item_manager::GetAnEnergyDrink() {
  list<CEnergyDrink*, scd::allocator<CEnergyDrink*>>::iterator iter;
  iter = m_lEnergyDrinkComps.begin();

  while (iter != m_lEnergyDrinkComps.end()) {
    // if we're not spawned
    if (!(*iter)->GetIsSpawned()) {
      // break so we can grab it down below
      break;
    }
    iter++;
  }

  // if we DID find an unused item in the list (aka "down below")
  if (iter != m_lEnergyDrinkComps.end()) {
    // get that item and return it
    CEnergyDrink* pComp = *iter;
    return pComp;
  }

  // got here because we didn't find an unused item in the list

  // create a new soup
  scd::vector3 vObjPos(0.0f, 0.0f, 0.0f);
  string szObjName = "EnergyDrinkComponent";
  szObjName += (char)(nEnergyDrinksCreated + '0');
  nEnergyDrinksCreated++;

  scd::object* pObj = scd::objectManager::CreateObject(szObjName);

  // create this component
  CEnergyDrink* pComponent = CEnergyDrink::CreateEnergyDrinkComponent(pObj);

  // add to the list
  m_lEnergyDrinkComps.push_back(pComponent);

  return pComponent;
}

CPie* held_item_manager::GetAPie(scd::vector3 vPos) {
  list<CPie*, scd::allocator<CPie*>>::iterator iter;
  iter = m_lPieComps.begin();

  while (iter != m_lPieComps.end()) {
    // if we're not spawned
    if (!(*iter)->GetIsSpawned()) {
      // break so we can grab it down below
      break;
    }
    iter++;
  }

  // if we DID find an unused item in the list (aka "down below")
  if (iter != m_lPieComps.end()) {
    // get that item and return it
    CPie* pComp = *iter;
    return pComp;
  }

  // got here because we didn't find an unused item in the list

  // create a new soup
  scd::vector3 vObjPos(vPos.x, vPos.y, vPos.z);
  string szObjName = "PieComponent";
  szObjName += (char)(nPiesCreated + '0');
  nPiesCreated++;

  scd::object* pObj = scd::objectManager::CreateObject(szObjName);

  // create this component
  CPie* pComponent =
      CPie::CreatePieComponent(pObj, scd::vector3(0.0f, 0.0f, 0.0f));

  // Add a collision component
  TSphere tsphere;
  tsphere.cPosition = vObjPos;
  tsphere.fRadius = 0.5f; // TODO: Is this the radius we want?
  CCollideable* pCol =
      CCollisionManager::GetInstance()->CreateCollideableComponent(
          pObj, false, true, OBJPIE);
  pCol->SetBVType(BSPHERE);
  pCol->SetSphere(tsphere);

  // add Render component
  /*CRenderComponent* pRenderComp =*/Renderer::CreateRenderComp(
      pObj,
      CSpawningManager::GetInstance()->GetHeldItemMeshID(PIE),
      CSpawningManager::GetInstance()->GetHeldItemRenderCompID(PIE),
      2); // 2 for textured

  pObj->GetTransform()->ScaleFrame(2.0f, 2.0f, 2.0f);

  // add to the list
  m_lPieComps.push_back(pComponent);

  return pComponent;
}

CPeanutButter* held_item_manager::GetAPeanutButter() {
  list<CPeanutButter*, scd::allocator<CPeanutButter*>>::iterator iter;
  iter = m_lPeanutButterComps.begin();

  while (iter != m_lPeanutButterComps.end()) {
    // if we're not spawned
    if (!(*iter)->GetIsSpawned()) {
      // break so we can grab it down below
      break;
    }
    iter++;
  }

  // if we DID find an unused item in the list (aka "down below")
  if (iter != m_lPeanutButterComps.end()) {
    // get that item and return it
    CPeanutButter* pComp = *iter;
    return pComp;
  }

  // got here because we didn't find an unused item in the list

  // create a new soup
  scd::vector3 vObjPos(0.0f, 0.0f, 0.0f);
  string szObjName = "PeanutButterComponent";
  szObjName += (char)(nPeanutButtersCreated + '0');
  nPeanutButtersCreated++;

  scd::object* pObj = scd::objectManager::CreateObject(szObjName);

  // create this component
  CPeanutButter* pComponent = CPeanutButter::CreatePeanutButterComponent(pObj);

  float fScale = 3.0f; // scale of the model

  // Add a collision component
  TSphere tsphere;
  tsphere.cPosition = vObjPos;
  tsphere.fRadius = fScale * .7f; // TODO: Is this the radius we want?
  CCollideable* pCol =
      CCollisionManager::GetInstance()->CreateCollideableComponent(
          pObj, false, false, OBJPBUTTER);
  pCol->SetBVType(BSPHERE);
  pCol->SetSphere(tsphere);
  pComponent->SetAOE(pCol);

  // add Render component
  /*CRenderComponent* pRenderComp =*/Renderer::CreateRenderComp(
      pObj,
      ModelManager::GetInstance()->GetMeshIndexByName(
          "FFP_3D_PeanutPile_FINShape", false),
      RC_PEANUTPILE,
      2); // 2 for textured\

  pObj->GetTransform()->ScaleFrame(
      fScale,
      fScale,
      fScale); // scale the scale so it's not quite so far out from the mesh

  // add to the list
  m_lPeanutButterComps.push_back(pComponent);

  return pComponent;
}

CDonut* held_item_manager::GetADonut() {
  list<CDonut*, scd::allocator<CDonut*>>::iterator iter;
  iter = m_lDonutComps.begin();

  while (iter != m_lDonutComps.end()) {
    // if we're not spawned
    if (!(*iter)->GetIsSpawned()) {
      // break so we can grab it down below
      break;
    }
    iter++;
  }

  // if we DID find an unused item in the list (aka "down below")
  if (iter != m_lDonutComps.end()) {
    // get that item and return it
    CDonut* pComp = *iter;
    return pComp;
  }

  // got here because we didn't find an unused item in the list

  // create a new soup
  scd::vector3 vObjPos(0.0f, 0.0f, 0.0f);
  string szObjName = "DonutComponent";
  szObjName += (char)(nDonutsCreated + '0');
  nDonutsCreated++;

  scd::object* pObj = scd::objectManager::CreateObject(szObjName);

  // create this component
  CDonut* pComponent = CDonut::CreateDonutComponent(pObj);

  // add Animate and Render component
  /*string szFuncName = "CreateDonutAnimateComp(\"";
  szFuncName += szObjName;
  szFuncName += "\")";
  Debug.CallLuaFunc(szFuncName);*/

  /*CRenderComponent* pRenderComp = Renderer::CreateRenderComp(pObj,
          CSpawningManager::GetInstance()->m_nHeldItemMeshIDs[DONUT],
          CSpawningManager::GetInstance()->m_nHeldItemRenderCompIDs[DONUT],
          2); */// 2 for textured\

  // make the donut HUGE!!!
  // pObj->GetTransform()->ScaleFrame(10.0f, 10.0f, 10.0f);

  // add to the list
  m_lDonutComps.push_back(pComponent);

  return pComponent;
}

CRayJam* held_item_manager::GetARayJam() {
  list<CRayJam*, scd::allocator<CRayJam*>>::iterator iter;
  iter = m_lJamComps.begin();

  while (iter != m_lJamComps.end()) {
    // if we're not spawned
    if (!(*iter)->GetIsSpawned()) {
      // break so we can grab it down below
      break;
    }
    iter++;
  }

  // if we DID find an unused item in the list (aka "down below")
  if (iter != m_lJamComps.end()) {
    // get that item and return it
    CRayJam* pComp = *iter;
    return pComp;
  }

  // got here because we didn't find an unused item in the list

  // create a new soup
  scd::vector3 vObjPos(0.0f, 0.0f, 0.0f);
  string szObjName = "RayJamComponent";
  szObjName += (char)(nJamsCreated + '0');
  nJamsCreated++;

  scd::object* pObj = scd::objectManager::CreateObject(szObjName);

  // create this component
  CRayJam* pComponent = CRayJam::CreateRayJamComponent(pObj);

  // add to the list
  m_lJamComps.push_back(pComponent);

  return pComponent;
}

void held_item_manager::ShutdownCallback(
    IEvent* /*cEvent*/,
    scd::base_component* /*cCenter*/) {
  held_item_manager* me = GetInstance();

  // despawn all the items in the level
  list<CTurkeyComponent*, scd::allocator<CTurkeyComponent*>>::iterator
      turkeyIter;
  turkeyIter = me->m_lTurkeyComps.begin();

  while (turkeyIter != me->m_lTurkeyComps.end()) {
    (*turkeyIter)->Despawn();
    turkeyIter++;
  }

  list<CDonut*, scd::allocator<CDonut*>>::iterator donutIter;
  donutIter = me->m_lDonutComps.begin();

  while (donutIter != me->m_lDonutComps.end()) {
    (*donutIter)->Despawn();
    donutIter++;
  }

  list<CEnergyDrink*, scd::allocator<CEnergyDrink*>>::iterator EnergyDrinkIter;
  EnergyDrinkIter = me->m_lEnergyDrinkComps.begin();

  while (EnergyDrinkIter != me->m_lEnergyDrinkComps.end()) {
    (*EnergyDrinkIter)->Despawn();
    EnergyDrinkIter++;
  }

  list<CBanana*, scd::allocator<CBanana*>>::iterator BananaIter;
  BananaIter = me->m_lBananaComps.begin();

  while (BananaIter != me->m_lBananaComps.end()) {
    (*BananaIter)->Despawn();
    BananaIter++;
  }

  list<CChickenSoupComponent*, scd::allocator<CChickenSoupComponent*>>::iterator
      ChickenSoupIter;
  ChickenSoupIter = me->m_lChickenSoupComps.begin();

  while (ChickenSoupIter != me->m_lChickenSoupComps.end()) {
    (*ChickenSoupIter)->Despawn();
    ChickenSoupIter++;
  }

  list<CPie*, scd::allocator<CPie*>>::iterator PieIter;
  PieIter = me->m_lPieComps.begin();

  while (PieIter != me->m_lPieComps.end()) {
    (*PieIter)->Despawn();
    PieIter++;
  }

  list<CRayJam*, scd::allocator<CRayJam*>>::iterator RayJamIter;
  RayJamIter = me->m_lJamComps.begin();

  while (RayJamIter != me->m_lJamComps.end()) {
    (*RayJamIter)->Despawn();
    RayJamIter++;
  }

  list<CPeanutButter*, scd::allocator<CPeanutButter*>>::iterator
      PeanutButterIter;
  PeanutButterIter = me->m_lPeanutButterComps.begin();

  while (PeanutButterIter != me->m_lPeanutButterComps.end()) {
    (*PeanutButterIter)->Despawn();
    PeanutButterIter++;
  }
}
} // namespace scd
