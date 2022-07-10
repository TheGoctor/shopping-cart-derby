/**
 * @file ai_component.h

 * @author Jesse A. Stanciu

 * @brief This is the AI's main class which is responsible for changing the AI's
 * state based on weights. The AI will have three main states: Defensive,
 * Collective, and Aggressive. Each state will dertermine how the AI behaves in
 * each situation.
 */

#pragma once

#include "components/ai_manager.h"
#include "core/base_component.h"
#include "core/containers.h"
#include "core/object.h"
#include "physics/math_types.h"

namespace scd {
class dx_mesh;
class renderable;

class ai_component : public scd::base_component {
private:
  enum class state { NOT_INIT = -1, COLLECTIVE = 0, AGGRESSIVE, DEFENSIVE };

  // For rendering triangles in the nav mesh
  dx_mesh* _mesh;
  renderable* _renderable;
  bool _shutdown;

  /*****************************************************************
   * StartDraw()	Sets up anything needed to begin drawing traingles
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      06/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void start_draw();

  /*****************************************************************
   * EndDraw()	Passes all buffers to be drawn
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      06/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void end_draw();

  /*****************************************************************
   * DrawTriangle()	Passes information for the triangle to be
   *					drawn and a color to be drawn in
   *
   * Ins:				scd::vector3 / TTriangle* / TTri*
   *					scd::vector4 cColor
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      06/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void draw_triangle(const scd::vector3& vertex0, const scd::vector3& vertex1,
                     const scd::vector3& vertex2, const scd::vector4& color);
  void draw_triangle(TTriangle* tTri, const scd::vector4& cColor);
  void draw_triangle(TTri* tTri, const scd::vector4& cColor);

  /*****************************************************************
   * DrawTriangle()	Passes information for a line to be
   *					drawn and a color to be drawn in
   *
   * Ins:				scd::vector3 vStart, vEnd
   *					scd::vector4 cColor
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      06/06/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void draw_line(const scd::vector3& start, const scd::vector3& end,
                 const scd::vector4& color);

  // Pathplanning stuff
  int _current_triangle_id;
  int _target_triangle_id;

  // Steering Information
  scd::vector3 _target_position;

  int _controller_id;
  //

  // Needed Logic
  TRay m_tTargetPlayerLine;
  float _weight[3];
  float _timer;
  scd::object* _target_player;
  bool _target_item;
  bool _random_item;

  float _speed;
  float _ramming_speed;
  float _acceleration;
  float _blind_timer;
  float _jam_timer;
  float _use_item_timer;
  float _turkey_to_player;

  /*****************************************************************
   * Use()				Uses the passed in type if it has one
   *
   * Ins:				EHeldItemType
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      05/11/11
   * Mod. Initials:	      JS
   *****************************************************************/
  bool Use(EHeldItemType);

  /*****************************************************************
   * GetFirstPlace()	Returns the ID of who is in first place
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:			int
   *
   * Mod. Date:		      07/18/11
   * Mod. Initials:	      JS
   *****************************************************************/
  static int GetFirstPlace();

  /*****************************************************************
   * GetMyPlace()	Returns the numerial place that the agent is
   *				currently in
   *
   * Ins:
   *
   * Outs:
   *
   * Returns:			int
   *
   * Mod. Date:		      07/18/11
   * Mod. Initials:	      JS
   *****************************************************************/
  int GetMyPlace();

  /*****************************************************************
   * UpdateCollective()	Updates agent logic with a collective bias
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
  void UpdateCollective();

  /*****************************************************************
   * UpdateAggressive()	Updates agent logic with a aggressive bias
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
  void UpdateAggressive();

  /*****************************************************************
   * UpdateDefensive()	Updates agent logic with a defensive bias
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
  void UpdateDefensive();

  /*****************************************************************
   * RemoveGoalItemFromLevel()	Removes the past in Goal Item type
   *							from the GoalItemsInLevel map
   *
   * Ins:				EGoalItemType
   *
   * Outs:
   *
   * Returns:
   *
   * Mod. Date:		      07/21/11
   * Mod. Initials:	      JS
   *****************************************************************/
  void RemoveGoalItemFromLevel(EGoalItemType eGoalItemType);

  /*****************************************************************
   * TGoalItems	Holds the item's id and weight. Weight will
   *				determine how likely the AI is to go for it.
   *				The higher the weight, the more likely the AI
   *				will go for the item.
   *****************************************************************/
  struct TGoalItem {
    scd::vector3 m_cPos;
    EGoalItemType m_nGoalItem;
    float m_pfWeight;
  };

  struct held_item {
    scd::vector3 _position;
    EHeldItemType m_nHeldItem;
    int m_nID;
    bool m_bPowerup;
  };

  /*****************************************************************
   * TKnowledge	Holds all knowledge that the agent will have
   *
   * TPlayer		Has a char for amount of goal item the the
   *				opponent has collected and a list for
   *				goal items that it has SEEN the opponent collect
   *****************************************************************/
  struct player {
    scd::map<unsigned int, EGoalItemType> _goal_items;

    unsigned int _aggression;

    // The opponent's object's ID
    int _player;
    int _item;
    char _goal_item_count;
    bool _target;
    bool _has_item;
    float _current_speed;
  };

  /**
   * Returns a pointer to the object who owns the passed in ID
   *
   * @param nID
   */
  scd::object& GetOpponent(int);

  /**
   * Returns the position of the object who owns the passed in ID
   *
   * @param nID
   *
   * @return position of opponent
   */
  scd::vector3 GetOpponentPos(int);

  /**
   * Finds a nearby held item of either a power up or attack item
   *
   * @param bPowerup true if you want to find a power up, false if you want an
   * attack item
   *
   * @param[out] out_item The found item
   *
   * @returns whether an item was found
   */
  bool FindNearbyHeldItem(bool bPowerup, THeldItem&);

  /**
   * Finds the closest opponent to the agent and returns a pointer to their
   * object
   *
   * @return found player
   */
  scd::object* GetClosestOpponent();

  /**
   * Determines if the type of a item is a power up or attack item
   *
   * @param id
   *
   * @return true if powerup, false if attack item
   */
  bool IsPowerup(int);

  /**
   * Returns the squared distance between two points
   *
   * @param a
   * @param b
   *
   * @return squared distance between two points
   */
  float GetDistance(const scd::vector3& a, const scd::vector3& b) const;

  /**
   * Goes through logic and uses any Held Items that the agent deems apporiate
   */
  void UseHeldItem();

  /**
   * Makes the agent use a psudeo-random Held Item
   */
  void PickRandomHeldItem();

  /**
   * Adds up all the weight components and changes states if necessary
   */
  void EvaluateStateWeights();

  /**
   * Ranks the currently spawned Goal Items in a weight based system to\
   * determine which to go for.
   */
  void EvaluateGoalItemWeights();

  /**
   * Ranks each opponent based either on collection weights, aggressive weights,
   * or defensive weights, depending on the agent's current state.
   */
  void EvaluateOpponentWeights();

  /**
   * Ranks each Held Item based on distance and state of the agent
   */
  void EvaluateHeldItemWeights();

public:
  struct TKnowledge {
    TPlayer m_cOpponents[3];
    int m_nOpponentID;

    scd::map<unsigned int, TGoalItem> _goal_items_in_level;
    scd::map<unsigned int, THeldItem> _held_items_in_level;
    scd::map<unsigned int, EGoalItemType> _needed_goal_items;
    scd::map<unsigned int, EGoalItemType> _dropped_items;

    EHeldItemType m_peMyHeldItems[2];

    unsigned int m_nDistanceFromGoal;

  } m_tKnowledge;

  scd::vector3 m_vGoalPosition;
  scd::object* m_pObject;
  EStates m_eCurrentState;
  int nAnimation;
  bool bDrawTris;
  bool bLogical;
  float fTurnSpeed;
  float fThreshold[2];
  triangle* tNextTri;
  triangle* m_pMiniGoalTri;

  /**
   * Sets a bool on whether the agent should recieve random held items or not
   *
   * Ins:				EGoalItemType
   */
  void SetUseRandomItems(bool bRandomItems) { _random_item = bRandomItems; }

  /**
   * Creates an instace of CAscd::base_component
   *
   * @param object Parent of the component
   */
  ai_component(scd::object& object);

  /**
   * Destructor
   */
  ~ai_component();

  /**
   * Inits the component
   */
  void Init();

  // Events

  /**
   * Adds the agent's opponents to its list of opponents
   *
   * @param object The opponent.
   */
  void SetupOpponents(const scd::object& opponent);

  /**
   * Is called when a Goal Item is knocked out of a players cart
   */
  static void ItemDropped(IEvent*, scd::base_component*);

  /**
   * Is called when a Goal Item is created from CSpawningManager
   */
  static void GoalItemInit(IEvent*, scd::base_component*);

  /**
   * Reinits the AI after multiple playthroughs
   */
  static void InitCB(IEvent*, scd::base_component*);

  /**
   * Called when the gameplay state is exited and the agent should shutdown
   */
  static void Shutdown(IEvent*, scd::base_component*);

  /**
   * Called every frame updating values for each players' speed
   */
  static void UpdateSpeeds(IEvent*, scd::base_component*);

  /**
   * Called when a new Held Item is spawned
   */
  static void HeldItemSpawned(IEvent*, scd::base_component*);

  /**
   * Called when a Held Item is collected by a player
   */
  static void HeldItemCollected(IEvent*, scd::base_component*);

  /**
   * Called at the beginning of the game to set the ram speed of the agent
   */
  static void SetRamSpeed(IEvent* pEvent, scd::base_component* pComp);

  /**
   * Called when a pick up item (knocked out Goal Item) is collected
   */
  static void PickupItemCollected(IEvent*, scd::base_component*);

  /**
   * Called when a Held Item is stolen through the use of donuts
   */
  static void ItemStealing(IEvent*, scd::base_component*);

  /**
   * Called when a new player is created
   */
  static void PlayerCreated(IEvent*, scd::base_component*);

  /**
   * Event sent through Lua to toggle the rendering of the nav mesh
   */
  static void ToggleTris(IEvent*, scd::base_component*);

  /**
   * Event sent through Lua to toggle certain aspects of the AI
   */
  static void ToggleAI(IEvent*, scd::base_component*);

  /**
   * Event sent through Lua to toggle if the agent runs logic
   */
  static void ToggleLogic(IEvent*, scd::base_component*);

  /**
   * Called when the game starts to set all the turn speeds of the players
   */
  static void ShowTurnSpeed(IEvent*, scd::base_component*);

  /**
   * Called when the game starts to set all the acceleration of the players
   */
  static void ShowAcceleration(IEvent*, scd::base_component*);

  /**
   * Called when a player is hit by a pie
   */
  static void Blind(IEvent*, scd::base_component*);

  /**
   * Called when a player is hit by jam
   */
  static void Jammed(IEvent*, scd::base_component*);

  /**
   * An event that tells the agent that a Goal Item has been collected
   */
  static void GoalItemCollected(IEvent*, scd::base_component*);

  /**
   * An event that tells the agent that a Goal Item has spawned
   */
  static void GoalItemSpawned(IEvent*, scd::base_component*);

  /**
   * An event that tells the agent that a Goal Item has spawned
   */
  static void GoalItemDespawned(IEvent*, scd::base_component*);

  /**
   * Update calls any function that needs calling
   */
  static void Update(IEvent*, scd::base_component*);

  /**
   * Called when the gameplay state is paused to render the game world.
   */
  static void PauseUpdateCallback(IEvent*, scd::base_component* pComp);

  /**
   * Creates an instance of an object to render a triangle of the nav mesh.
   *
   * @return component of the triangle
   */
  CRenderComponent* CreateMeshTriangle();

  /**
   * Sets the controller number of the agent
   *
   * @param controller The controller number.
   */
  void SetControllerNumber(int controller) { m_nControllerNumber = controller; }
};

} // namespace scd
