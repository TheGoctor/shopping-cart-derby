/*******************************************************************************
 * Filename:  		CAIManager.h
 * Date:      		04/04/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This is the manager for the
        CAscd::base_component components. Will hold a pointer
        to each of the CAscd::base_component objects and
        any static information they need.
 ******************************************************************************/

#pragma once

#include "core/containers.h"
#include "physics/math_types.h"
#include "physics/physics_manager.h"

#include <array>
#include <string>

namespace scd {

struct nav_mesh_triangle {
  scd::map<int, nav_mesh_triangle*> connections;
  std::array<geometry::line, 3> edges;
  scd::vector3 position;
  std::array<int, 3> indexes;

  bool IsConnected(const nav_mesh_triangle& rhs) const {
    scd::vector3 a, b, c, x, y, z;
    int connections = 0;

    a = edges[0].start;
    b = edges[0].end;
    c = edges[1].end;
    x = rhs.edges[0].start;
    y = rhs.edges[0].end;
    z = rhs.edges[1].end;

    if ((a == x) || (a == y) || (a == z)) {
      ++connections;
    }
    if ((b == x) || (b == y) || (b == z)) {
      ++connections;
    }
    if ((c == x) || (c == y) || (c == z)) {
      ++connections;
    }
    if (connections > 1) {
      return true;
    }

    return false;
  }
};

struct planner_node {
  planner_node* _parent = nullptr;
  nav_mesh_triangle* _triangle = nullptr;

  float _given_cost     = 0.0f;
  float _heuristic_cost = 0.0f;
  float _final_cost     = 0.0f;

  planner_node() = default;

  planner_node(planner_node* parent, nav_mesh_triangle* triangle,
               float given_cost,
               float heuristic_cost)
    : _parent(parent)
    , _triangle(triangle)
    , _given_cost(given_cost)
    , _heuristic_cost(heuristic_cost) {
    _final_cost = _heuristic_cost + _given_cost;
  }

  bool operator<(const planner_node& rhs) const {
    return this->_final_cost < rhs._final_cost;
  }
};

class ai_manager {
  friend class scd::base_component;

  /////////////////
  // Constructor //
  /////////////////
  ai_manager();

  struct node {
    std::string name;
    scd::transform local_pose;
    scd::transform world_pose;
  };

  // Pathfinding stuffs
  scd::set<planner_node*> _open;
  scd::map<nav_mesh_triangle*, planner_node*> _created;

  char* _lookup;

  scd::map<int, nav_mesh_triangle*> _triangles;

  scd::object* _nav_mesh;

  bool _show_info;
  std::string _info;

  /*****************************************************************
   * LoadNavMesh()	Loads a nav mesh
   *
   * Ins:				string szFileName
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      06/07/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void load_nav_mesh(const std::string& file_path);

  /*****************************************************************
   * IsConnected()	Checks to see if two TTri's are connected
   *
   * Ins:				const TTri& rhs
   *
   * Outs:
   *
   * Returns:			True - Connected
   *
   * Mod. Date:		      06/07/11
   * Mod. Initials:	      JS
   *****************************************************************/
  bool is_connected(const nav_mesh_triangle& lhs,
                    const nav_mesh_triangle& rhs) const;

  /*****************************************************************
   * CreateConnections()	Preforms logic to either read in connections
   *						or create the connections file
   *
   * Ins:				string szFileName
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      06/07/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void create_connections(const std::string& file_path);

  /*****************************************************************
   * CreateLookUpTable()	Preforms logic to either read in a look up
   *						table or create the file
   *
   * Ins:				const TTri& rhs
   *
   * Outs:				string szFileName
   *
   * Returns:
   *
   * Mod. Date:		      06/07/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void create_lookup_table(const std::string& file_path);

  /*****************************************************************
   * Pathplan()	Preforms A* pathfinding from triangle 1 to
   *				triangle 2
   *
   * Ins:				const int Tri1, const int Tri2
   *
   * Outs:
   *
   * Returns:			int - Edge to go through from tri 1
   *
   * Mod. Date:		      06/07/11
   * Mod. Initials:	      JS
   *****************************************************************/
  int path_plan(const int, const int);

  /*****************************************************************
   * LinesEqual()	Checks to see if two lines are the same line
   *
   * Ins:			const TLine&, const TLine&
   *
   * Outs:
   *
   * Returns:		True - Same Line
   *
   * Mod. Date:		      06/07/11
   * Mod. Initials:	      JS
   *****************************************************************/
  bool lines_equal(const geometry::line& lhs, geometry::line& rhs);

  /*****************************************************************
   * LookUpCellAlgorithm()	Takes in two ints for a 2D array and
   *						returns the next triangle to go to.
   *
   * Ins:			const int, const int
   *
   * Outs:
   *
   * Returns:		unsigned int - index of next triangle
   *
   * Mod. Date:		      06/07/11
   * Mod. Initials:	      JS
   *****************************************************************/
  inline unsigned int look_up_cell_algorithm(const int x, const int y) {
    // x = Current, y = Target
    return y * _triangles.size() + x;
  }
  // /Pathfinding stuffs

protected:
  std::array<scd::vector3, 9> _dept_positions;

  scd::list<EGoalItemType> _initialized_goal_items;

  // Will call allocate to create
  // the amount of AI agents needed
  scd::map<unsigned int, scd::base_component*> _components;
  scd::map<unsigned int, scd::object*> _spawnable_goal_items;
  scd::map<unsigned int, scd::object*> _players;

  /*****************************************************************
   * GetDistance()	Returns SQUARED distance between two points
   *
   * Ins:			const scd::vector3&, const scd::vector3&
   *
   * Outs:
   *
   * Returns:		float - distance
   *
   * Mod. Date:		      06/07/11
   * Mod. Initials:	      JS
   *****************************************************************/
  inline float get_distance(const scd::vector3& cPos1,
                            const scd::vector3& cPos2) const {
    float x = cPos2.x - cPos1.x;
    float z = cPos2.z - cPos1.z;
    return ((x * x) + (z * z));
  }

public:
  /*****************************************************************
   * Init()	Registers for events and loads any needed file
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      04/04/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void init();

  /*****************************************************************
   * GetAIInfo()	Recieves each agent and their state
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      07/13/11
   * Mod. Initials:	      JS
   *****************************************************************/
  std::string& get_ai_info();

  /*****************************************************************
   * Creates an AI component
   *
   * Ins:					scd::object* - Parent, bool UseRandomItems
   *
   * Outs:
   *
   * Returns:				CAscd::base_component*
   *
   * Mod. Date:		      04/04/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static scd::base_component* create_component(scd::object* owner, bool);
  static int create_component(lua_State* pLua);

  /*****************************************************************
   * GoalItemInit()	Saves the inited goal item into a list
   *
   * Ins:				IEvent* -
   *					scd::base_component* - TGoalItemEvent*
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      06/27/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static void on_goal_item_init(IEvent*, scd::base_component*);

  /*****************************************************************
   * ToggleInfo()	Toggles if the szInfo string returns actual
   *				info or a null string
   *
   * Ins:			IEvent* -
   *				scd::base_component* -
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      07/12/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static void on_toggle_info(IEvent*, scd::base_component*);

  /*****************************************************************
   * LoadLevel()	Loads the NavMesh, creates connections between
   *				triangles, and then generates the look
   *				up table
   *
   * Ins:			IEvent* -
   *				scd::base_component* - TStringEvent
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      04/04/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static void on_load_level(IEvent*, scd::base_component*);

  /*****************************************************************
   * SetGoalTri()	Sets the traingle the specified agent is
   *				to go to
   *
   * Ins:			pLua - string: SetAIGoalTri(#agent, #tri)
   *
   * Outs:
   *
   * Returns:		int - random int because this is a lua function
   *
   * Mod. Date:		      06/14/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static int set_goal_triangle(lua_State* pLua);

  /*****************************************************************
   * Update()	Update calls any function that needs calling
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      04/04/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static void on_update(IEvent*, scd::base_component*);

  /*****************************************************************
   * Shutdown()	Shuts down the component
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      04/04/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static void on_shutdown(IEvent*, scd::base_component*);

  /*****************************************************************
   * SetupOpponents()	Calls SetupOpponents() on each agent
   *					so they know who their opponents are (ID wise)
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      05/01/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static void on_setup_opponents(IEvent* piEvent,
                                 scd::base_component* piComponent);
};
} // namespace scd
