////////////////////////////////////////////////////////////////////////////////
//	File			:	CJamHitEffectComp.h
//	Date			:	7/27/11
//	Mod. Date		:	7/27/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Jam Hit Effect
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "components/component_manager.h"
#include "components/rendering/effects/effect.h"
#include "core/base_component.h"
#include "core/object.h"

namespace scd {
class CJamHitEffectComp : public scd::base_component {
private:
  ///////////////////////////////////////////////////////////////////////////
  // Data Members
  ///////////////////////////////////////////////////////////////////////////

  CObject* m_pParentObj;          // Parent Object
  CObject* m_pDownEffect;         // Down Effect Obj
  CEffectComponent* m_pDownVFX;   // Jam Down Trail Effect
  CEffectComponent* m_pDownBVFX;  // Jam Down Meteor Effect
  CEffectComponent* m_pTargetVFX; // Jam Down Target Effect
  float m_fStartTimer;            // Effect Start Timer
  float m_fInvDuration;           // Invulnerable Duration
  bool m_bActive;                 // Active Flag
  bool m_bDead;                   // Dead Flag

  ///////////////////////////////////////////////////////////////////////////
  // Event Handlers
  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: �Update�
  //
  // Return: void
  //
  // Parameters: float fDT - The time since the last Frame
  //
  // Purpose: This Function is used to Handle Updating Jam Hit Effects
  //			and Check for end of Duration to Switch Effects Off.
  ///////////////////////////////////////////////////////////////////////
  void Update(float fDT);

  ///////////////////////////////////////////////////////////////////////
  // Function: �Jam�
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: This Functions is used to Handle activating the Jam Hit
  //			Effects on the Player that did not use it.
  ///////////////////////////////////////////////////////////////////////
  void Jam(void);

public:
  ///////////////////////////////////////////////////////////////////////////
  // Function: �Init�
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: This Functions is used to Initalize the Component by
  //			Registering for Events, Getting and Transforming an
  //			Jam Hit Effect Object and Creating Sub-Effect Objects
  // and 			Render Components as well as Binding them to the Parent
  // Player.
  ///////////////////////////////////////////////////////////////////////////
  void Init(void);

  ///////////////////////////////////////////////////////////////////////////
  // Constructor
  ///////////////////////////////////////////////////////////////////////////
  CJamHitEffectComp(CObject* pParentPlayerObj)
      : m_pParentObj(pParentPlayerObj)
      , m_pDownEffect(NULL)
      , m_pDownVFX(NULL)
      , m_pTargetVFX(NULL)
      , m_fStartTimer(0.0f)
      , m_fInvDuration(0.0f)
      , m_bActive(false)
      , m_bDead(false) {
    // Add to Auto Manager
    GetManager()->Add(this);

    // Add Component to Parent
    m_pParentObj->AddComponent(this);

    // Initalize
    Init();
  }

  ///////////////////////////////////////////////////////////////////////////
  // Destructor
  ///////////////////////////////////////////////////////////////////////////
  ~CJamHitEffectComp(void) {
    // Remove from Manager
    GetManager()->Remove(this);
  }

  ///////////////////////////////////////////////////////////////////////////
  // Factory Funcs
  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: �CreateJamHitVFXComponent�
  //
  // Return: CJamHitEffectComp* - A New Jam Hit Effect Component
  //								connected to the Passed
  //in Object
  //
  // Parameters: CObject* pPlayerParentObj - The Parent Player Object
  //
  // Purpose: This Functions is used to Create a Jam Hit Effect
  //			Component and Attach it to an Object through C++.
  ///////////////////////////////////////////////////////////////////////
  static CJamHitEffectComp*
  CreateJamHitVFXComponent(CObject* pPlayerParentObj) {
    return MMNEW(CJamHitEffectComp(pPlayerParentObj));
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Event Callbacks
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////
  // Function: �UpdateCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Updating the Passed in
  //			Component's Jam Hit Effects and Check for end of
  // Effects.
  ///////////////////////////////////////////////////////////////////////////
  static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////
  // Function: �JamCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle activating the passed in
  //			Jam Hit	Effects Component on the Player that did not use
  // it.
  ///////////////////////////////////////////////////////////////////////
  static void JamCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////
  // Function: �InvulnerableCallback�
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: This Functions is used to Handle deactivating the Jam Hit
  //			Effects on the Player that just became Invulnerable.
  ///////////////////////////////////////////////////////////////////////
  static void InvulnerableCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Function: �ShutdownGameplayCallback�
  //
  // Return: void
  //
  // Parameters: IEvent*			 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to deactivate the Passed in Component's
  //			Jam Hit Effects.
  ///////////////////////////////////////////////////////////////////////////
  static void ShutdownGameplayCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Accessors
  ///////////////////////////////////////////////////////////////////////////

  // Parent Object
  CObject* GetParent(void) { return m_pParentObj; }

  // Active Flag
  bool IsActive(void) { return m_bActive; }

  // Auto-Manager
  static CComponentManager<CJamHitEffectComp>* GetManager(void) {
    // Auto-Manager
    static CComponentManager<CJamHitEffectComp> m_cManager;

    return &m_cManager;
  }
};
} // namespace scd
