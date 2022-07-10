/*******************************************************************************
 * Filename:  		CDepartment.cpp
 * Date:      		05/01/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		Departments will hold a set amount of Goal Items.
                                        These Goal Items will be assoicated to
 the Department in terms of type (i.e. milk in the dairy department).

                                        A Department will list for a
 SpawnGoalItem event and will spawn a random Goal Item within itself.
 ******************************************************************************/
#include "department.h"

#include "components/level/goal_item.h"
#include "components/spawning_manager.h"
#include "core/object_manager.h"
#include "events/events.h"
#include "physics/physics_manager.h"
#include "rendering/renderer.h"

namespace scd {
CDepartment::CDepartment(scd::object* CObj)
    : m_pParent(CObj) {}

CDepartment::~CDepartment() {}

void CDepartment::Init(EDepartment eDepartment) {
  // Setup department
  m_eType = eDepartment;
  m_bUsed = false;
}

void CDepartment::AcknowledgeDepartments(int nID) {
  string szID;
  char chCounter[32];
  scd::vector3 cPos;

  // Setup ID for obj
  _itoa_s(nID, chCounter, 32, 10);
  szID = "GoalItem ";
  szID += chCounter;

  // Create Object
  scd::object* pObj = scd::objectManager::GetInstance()->CreateObject(szID);

  // Create Goal Item
  CGoalItems* pGoalItems =
      CSpawningManager::GetInstance()->CreateGoalItemComponent(
          pObj, (EGoalItemType)(nID));

  // Insert Goal Item into Department's map
  map<int,
      TGoalItemSpawnLocation,
      less<unsigned int>,
      scd::allocator<pair<int, TGoalItemSpawnLocation>>>::iterator pSpawnIter =
      m_cGoalItemSpawnLocations.begin();

  while (pSpawnIter != m_cGoalItemSpawnLocations.end()) {
    pSpawnIter->second.m_cGoalItem = pGoalItems;
    ++pSpawnIter;
  }

  // Set Render/Collision
  Renderer::GetInstance()->CreateRenderComp(
      pObj,
      CSpawningManager::GetInstance()->GetGoalItem(m_eType),
      CSpawningManager::GetInstance()->GetGoalItemRenderContext(nID),
      2);

  TSphere tsphere;
  tsphere.cPosition =
      pGoalItems->GetParent()->GetTransform()->GetWorldPosition();
  tsphere.fRadius = 1.0f;
  CCollideable* pCollideable =
      CCollisionManager::GetInstance()->CreateCollideableComponent(
          pObj, false, false, OBJGITEM);
  pCollideable->SetBVType(BSPHERE);
  pCollideable->SetSphere(tsphere);

  // Send the GoalItemInit event
  SendGoalItemEvent("GoalItemInit", this, pGoalItems->GetType(), pGoalItems);
}

void CDepartment::Despawn() {
  // Department is no longer being used
  m_bUsed = false;

  map<int,
      TGoalItemSpawnLocation,
      less<unsigned int>,
      scd::allocator<pair<int, TGoalItemSpawnLocation>>>::iterator pcIter =
      m_cGoalItemSpawnLocations.begin();

  while (pcIter != m_cGoalItemSpawnLocations.end()) {
    // Set Goal Item to despawn
    pcIter->second.m_cGoalItem->Despawn();

    ++pcIter;
  }
}

void CDepartment::SpawnGoalItem() {
  int nLocation = rand() % m_cGoalItemSpawnLocations.size();

  map<int,
      TGoalItemSpawnLocation,
      less<unsigned int>,
      scd::allocator<pair<int, TGoalItemSpawnLocation>>>::iterator pcIter =
      m_cGoalItemSpawnLocations.find(nLocation);

  m_bUsed = true;

  pcIter->second.m_cGoalItem->Init(pcIter->second.m_cGoalItem->GetType());

  scd::transform& myMatress =
      pcIter->second.m_cGoalItem->GetParent()->GetTransform()->GetLocalMatrix();
  myMatress._41 = pcIter->second.m_cPos.x;
  myMatress._42 = pcIter->second.m_cPos.y;
  myMatress._43 = pcIter->second.m_cPos.z;

  pcIter->second.m_cGoalItem->Spawn();
}
} // namespace scd
