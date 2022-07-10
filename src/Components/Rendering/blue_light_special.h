///////////////////////////////////////////////////////////////////////////////
//	File			:	CBlueLightSpecialComp.h
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Blue Light Special for Held Items
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "components/component_manager.h"
#include "components/rendering/renderable.h"
#include "core/base_component.h"
#include "core/object.h"

namespace scd {

class blue_light_special : public base_component {
private:
  object* _blue_light_object         = nullptr; // Blue Light Obj
  renderable* _blue_light_renderable = nullptr; // BLS Render Comp
  renderable* _end_cap_renderable    = nullptr; // Endcap Render Comp

  ///////////////////////////////////////////////////////////////////////
  // Function: “Update”
  //
  // Return: void
  //
  // Parameters: float fDT - The time since the last Frame
  //
  // Purpose: This Functions is used to Handle Updating BLS Effect by
  //			Translating and Rotating the BLS Obj
  ///////////////////////////////////////////////////////////////////////
  void update(float delta_time);

public:
  ///////////////////////////////////////////////////////////////////////////
  // Function: “Init”
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: This Functions is used to Initalize the Component by
  //			Registering for Events, Getting and Transforming a BLS Object
  //			with a Render Comp and Binding it to the Parent	Endcap Object.
  ///////////////////////////////////////////////////////////////////////////
  void init();

  ///////////////////////////////////////////////////////////////////////////
  // Constructor
  ///////////////////////////////////////////////////////////////////////////
  blue_light_special(CRenderComponent* pRC)
    : base_component(pRC->owner())
    , _end_cap_renderable(pRC)

  {
    manager().add(this);
    _owner->add_component(this);

    init();
  }

  ///////////////////////////////////////////////////////////////////////////
  // Destructor
  ///////////////////////////////////////////////////////////////////////////
  ~blue_light_special(void) { manager().remove(this); }

  ///////////////////////////////////////////////////////////////////////////
  // Factory Funcs
  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: “CreateBlueLightSpecialComponent”
  //
  // Return: CBlueLightSpecialComp* - A New Blue Light Special Component
  //									connected to the Passed in Object
  //
  // Parameters: CRenderComponent* pcRC - The Parent Remder Component of
  //										the associated Endcap Object
  //
  // Purpose: This Functions is used to Create an Blue Light Special
  //			Component and Attach it to an Object through C++.
  ///////////////////////////////////////////////////////////////////////
  static blue_light_special* create(renderable* pcRC) {
    return MMNEW(blue_light_special(pcRC));
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
  //			Component's BLS Effect by Translating and Rotating the BLS Obj
  ///////////////////////////////////////////////////////////////////////////
  static void on_update(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Function: “ActivateCallback”
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Activating of the Passed in
  //			Component's BLS Effect.
  ///////////////////////////////////////////////////////////////////////////
  static void on_activate(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Function: “DeactivateCallback”
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Deactivating of the Passed in
  //			Component's BLS Effect.
  ///////////////////////////////////////////////////////////////////////////
  static void on_deactivate(IEvent* pEvent, IComponent* pComp);

  static component_manager<blue_light_special>& manager() {
    static component_manager<blue_light_special> _manager;
    return _manager;
  }
};

} // namespace scd
