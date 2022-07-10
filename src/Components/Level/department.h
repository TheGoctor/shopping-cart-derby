/*******************************************************************************
 * Filename:  		CDepartment.h
 * Date:      		05/01/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		Departments will hold a set amount of Goal Items.
          These Goal Items will be assoicated to the Department
          in terms of type (i.e. milk in the dairy department).

          A Department will list for a SpawnGoalItem event and will
          spawn a random Goal Item within itself.
 ******************************************************************************/
#pragma once

#include "core/base_component.h"
#include "core/containers.h"
#include "physics/math_types.h"

namespace scd {
class event;
class CGoalItems;

struct TGoalItemSpawnLocation {
  scd::vector3 m_cPos;
  CGoalItems* m_cGoalItem;
};

class department : public scd::base_component {
  scd::object* m_pParent;
  EDepartment m_eType;
  bool m_bUsed;

public:
  scd::map<int, TGoalItemSpawnLocation> _goal_item_spawn_locations;

  /*****************************************************************
   * CDepartment()	Creates an instance of Department
   *
   * Ins:			scd::object* - Parent
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      05/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  department(scd::object&);

  /*****************************************************************
  ~CDepartment()	Destroys the instance
  *
  * Ins:
  *
  * Outs:
  *
  * Returns:
  *
  * Mod. Date:		      05/06/11
  * Mod. Initials:	      JS
  *****************************************************************/
  ~department();

  /*****************************************************************
   * Init()	Sets up any data needed for the department.
   *
   * Ins:		EDepartment - The Department type (Dairy/Deli)
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      05/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void init(EDepartment eDepartment);

  /*****************************************************************
   * AcknowledgeDepartments()	Tells the Department that it will be
   *							used this game. Creates the Goal
   *							Items and pushes them into its map
   *							of Goal Item locations. Sends the
   *							GoalItemInit event.
   *
   * Ins:						nID - EGoalItemType to spawn
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      05/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void acknowledge_departments(int);

  /*****************************************************************
   * Despawn()	Tells the Department to despawn any Goal Item spawned
   *			in it and then set its used bool to false.
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      05/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void despawn();

  /*****************************************************************
   * SpawnGoalItem()	Picks a psudeo random location to spawn a
   *					Goal Item. Then places the Goal Item at
   *					that location.
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      05/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void spawn_goal_item();

  /*****************************************************************
   * Deactivate()	Opposite of AcknowledgeDepartment. Sets the
   *				Department to not be used this game.
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      05/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  inline void deactivate() {
    // Setting this to true makes it so
    // that the Spawning Manager ignores it
    // while picking a Department to spawn
    // Goal Items in
    m_bUsed = true;
  }

  inline bool is_spawned() const { return m_bUsed; }
};

} // namespace scd
