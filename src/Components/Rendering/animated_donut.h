////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimatedDonutComp.h
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Animated Donut
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "components/component_manager.h"
#include "components/rendering/animate.h"
#include "components/rendering/effects/effect.h"
#include "core/base_component.h"
#include "core/object.h"
#include "events/events.h"

namespace scd {
///////////////////////////////////////////////////////////////////////////////
// Animated Donut Component Class
///////////////////////////////////////////////////////////////////////////////
class animated_donut : public base_component {
public:
  ///////////////////////////////////////////////////////////////////////////
  // Function: “Init”
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: This Functions is used to Initalize the Component by
  //			Registering for Events, Getting and Transforming an
  //			Animated Donut Object and Creating a Puff Effect.
  ///////////////////////////////////////////////////////////////////////////
  void init(void);

  ///////////////////////////////////////////////////////////////////////////
  // Constructor
  ///////////////////////////////////////////////////////////////////////////
  animated_donut(object& owner)
    : base_component(owner) {
    // Add to Auto Manager
    manager()->add(this);

    // Add Component to Parent
    owner->add_component(this);

    // Initalize
    init();
  }

  ///////////////////////////////////////////////////////////////////////////
  // Destructor
  ///////////////////////////////////////////////////////////////////////////
  ~animated_donut() {
    // Remove Component from the Manager
    manager()->remove(this);
  }

  ///////////////////////////////////////////////////////////////////////////
  // Factory Funcs
  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: “CreateAnimatedDonutComponent”
  //
  // Return: int - 0 to LUA
  //
  // Parameters: lua_State* ptLUA - LUA State of the Call
  //
  // Purpose: This Functions is used to Create an Animated Donut
  //			Component and Attach it to an Object through LUA.
  ///////////////////////////////////////////////////////////////////////
  static int create(lua_State* ptLUA);

  ///////////////////////////////////////////////////////////////////////
  // Function: “CreateAnimatedDonutComponent”
  //
  // Return: CAnimatedDonutComp* - A New Animated Donut Component
  //								 connected to the Passed in Object
  //
  // Parameters: CObject* pcParent - The Parent Animation Object
  //
  // Purpose: This Functions is used to Create an Animated Donut
  //			 Component and Attach it to an Object through C++.
  ///////////////////////////////////////////////////////////////////////
  static animated_donut* create(object& owner) {
    return MMNEW(animated_donut(owner));
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Event Callbacks
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////
  // Function: “UpdateCallback”
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Updating the Passed in
  //			Component's Donut Effects and Check for end of Duration
  //			to Switch Effects Off.
  ///////////////////////////////////////////////////////////////////////////
  static void on_update(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Function: “UseDonutCallback”
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle activating the Passed in
  //			Component's Donut Effects on the Player that Used it.
  ///////////////////////////////////////////////////////////////////////////
  static void on_use_donut(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Function: “DespawnDonutCallback”
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent 	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle deactivating the Passed in
  //			Component's Donut Effects on the Player that Used it.
  ///////////////////////////////////////////////////////////////////////////
  static void on_despawn_donut(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Function: “WinInitCallback”
  //
  // Return: void
  //
  // Parameters: IEvent*			 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Turn off the Passed in Component's
  //			Effects on Enter of Win State.
  ///////////////////////////////////////////////////////////////////////////
  static void on_win_init(IEvent*, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Function: “ShutdownGameplayCallback”
  //
  // Return: void
  //
  // Parameters: IEvent*			 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Unregister the Passed in Component
  //			for Events on Gameplay State Exit.
  ///////////////////////////////////////////////////////////////////////////
  static void on_shutdown_gameplay(IEvent*, IComponent* pComp);

  static component_manager<animated_donut>* manager() {
    static component_manager<animated_donut> _manager;

    return &_manager;
  }

private:
  animate* _animate    = nullptr; // Render Comp
  effect* _puff_effect = nullptr; // Effect Comp
  float _duration      = 0.0f;    // Duration

  ///////////////////////////////////////////////////////////////////////////
  // Event Handlers
  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: “Update”
  //
  // Return: void
  //
  // Parameters: float fDT - The time since the last Frame
  //
  // Purpose: This Functions is used to Handle Updating Donut Effects
  //			and Check for end of Duration to Switch Effects Off.
  ///////////////////////////////////////////////////////////////////////
  void update(float delta_time);

  ///////////////////////////////////////////////////////////////////////
  // Function: “UseDonut”
  //
  // Return: void
  //
  // Parameters: TStatusEffectEvent* ptSEEvent - Data relating to the
  //			   Status Effect Event
  //
  // Purpose: This Functions is used to Handle activating the Donut
  //			Effects on the Player that Used it.
  ///////////////////////////////////////////////////////////////////////
  void use_donut(TStatusEffectEvent* ptSEEvent);

  ///////////////////////////////////////////////////////////////////////
  // Function: “DespawnDonut”
  //
  // Return: void
  //
  // Parameters: TStatusEffectEvent* ptSEEvent - Data relating to the
  //			   Status Effect Event
  //
  // Purpose: This Functions is used to Handle deactivating the Donut
  //			Effects on the Player that Used it.
  ///////////////////////////////////////////////////////////////////////
  void despawn_donut(TStatusEffectEvent* ptSEEvent);

  ///////////////////////////////////////////////////////////////////////////
  // Helper Funcs
  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: “CheckForPlayerMatch”
  //
  // Return: bool - Whether the Parent Animation Object is associated
  //				  with the Passed in Object
  //
  // Parameters: CObject* pOtherObj - The Object to Check against
  //
  // Purpose: This Functions is used to Check Whether the Parent
  //			Animation Object is Associated with the Passed in Object.
  ///////////////////////////////////////////////////////////////////////
  bool check_for_player_match(object* other_object);
};

} // namespace scd
