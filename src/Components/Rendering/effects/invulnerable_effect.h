////////////////////////////////////////////////////////////////////////////////
//	File			:	CInvulnerableVFX.h
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Chiken Soup VFX
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "components/component_manager.h"
#include "components/rendering/effects/effect.h"
#include "core/base_component.h"
#include "core/object.h"
#include "events/events.h"

namespace scd {
class DXMesh;

///////////////////////////////////////////////////////////////////////////////
// Invulnerable VFX Component Class
///////////////////////////////////////////////////////////////////////////////
class CInvulnerableVFX : public scd::base_component {
private:
  ///////////////////////////////////////////////////////////////////////////
  // Data Members
  ///////////////////////////////////////////////////////////////////////////

  CObject* m_pParentObj;       // Parent Object
  DXMesh* m_pMesh;             // Mesh
  CEffectComponent* m_pEffect; // Effect
  float m_fDir;                // Direction
  float m_fInvDuration;        // InvulnerableDuration
  float m_fBoostDuration;      // Boost Duration
  float m_fBoostAnimTimer;     // Boost Animation Timer
  bool m_bActive;              // Active Flag

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
  // Purpose: This Functions is used to Handle Updating Invulnerable and
  //			Boost Effects and Check for end of Duration to Switch
  //			Effects Off.
  ///////////////////////////////////////////////////////////////////////
  void Update(float fDT);

  ///////////////////////////////////////////////////////////////////////
  // Function: �Invulnerable�
  //
  // Return: void
  //
  // Parameters: TStatusEffectEvent* pcObjEvent - Event Data with Duration
  //
  // Purpose: This Functions is used to Handle Activating Invulnerable
  //			Effects.
  ///////////////////////////////////////////////////////////////////////
  void Invulnerable(TStatusEffectEvent* pcObjEvent);

  ///////////////////////////////////////////////////////////////////////
  // Function: �Boost�
  //
  // Return: void
  //
  // Parameters: TStatusEffectEvent* pcObjEvent - Event Data with Duration
  //
  // Purpose: This Functions is used to Handle Activating Boost
  //			Effects.
  ///////////////////////////////////////////////////////////////////////
  void Boost(TStatusEffectEvent* pcObjEvent);

  ///////////////////////////////////////////////////////////////////////
  // Function: �TurnOff�
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: This Functions is used to Handle Deactivating Invulnerable
  //			and Boost Effects.
  ///////////////////////////////////////////////////////////////////////
  void TurnOff(void);

  ///////////////////////////////////////////////////////////////////////////
  // Helper Funcs
  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: �CheckForPlayerMatch�
  //
  // Return: bool - Whether the Parent Animation Object is associated
  //				  with the Passed in Object
  //
  // Parameters: CObject* pOtherObj - The Object to Check against
  //
  // Purpose: This Functions is used to Check Whether the Parent
  //			Animation Object is Associated with the Passed in
  //Object.
  ///////////////////////////////////////////////////////////////////////
  bool CheckForPlayerMatch(CObject* pOtherObj);

public:
  ///////////////////////////////////////////////////////////////////////////
  // Function: �Init�
  //
  // Return: void
  //
  // Parameters: void
  //
  // Purpose: This Function is used to Initalize the Component by
  //			Registering for Events and Creating a Bubble Effect.
  ///////////////////////////////////////////////////////////////////////////
  void Init(void);

  ///////////////////////////////////////////////////////////////////////////
  // Constructor
  ///////////////////////////////////////////////////////////////////////////
  CInvulnerableVFX(CObject* pParent, DXMesh* pMesh)
      : m_pParentObj(pParent)
      , m_pMesh(pMesh)
      , m_bActive(false)
      , m_fDir(1.0f)
      , m_fInvDuration(0.0f)
      , m_fBoostDuration(0.0f)
      , m_fBoostAnimTimer(0.0f) {
    // Add to Auto Manager
    GetManager()->Add(this);

    // Add Component to Parent
    pParent->AddComponent(this);

    // Initalize
    Init();
  }

  ///////////////////////////////////////////////////////////////////////////
  // Destructor
  ///////////////////////////////////////////////////////////////////////////
  ~CInvulnerableVFX(void) {
    // Remove from Manager
    GetManager()->Remove(this);
  }

  ///////////////////////////////////////////////////////////////////////////
  // Factory Funcs
  ///////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: �CreateInvulnerableVFXComponent�
  //
  // Return: int - 0 to LUA
  //
  // Parameters: lua_State* ptLUA - LUA State of the Call
  //
  // Purpose: This Functions is used to Create an Invulnerable VFX
  //			Component and Attach it to an Object through LUA.
  ///////////////////////////////////////////////////////////////////////
  static int CreateInvulnerableVFXComponent(lua_State* pLUA);

  ///////////////////////////////////////////////////////////////////////
  // Function: �CreateInvulnerableVFXComponent�
  //
  // Return: CInvulnerableVFX* - A New Invulnerable VFX Component
  //							   connected to the Passed in
  //Object
  //
  // Parameters: CObject* pParent - The Parent Animation Object
  //			   DXMesh* pMesh	- The Parent Mesh
  //
  // Purpose: This Functions is used to Create an Invulnerable VFX
  //			Component and Attach it to an Object through C++.
  ///////////////////////////////////////////////////////////////////////
  static CInvulnerableVFX*
  CreateInvulnerableVFXComponent(CObject* pParent, DXMesh* pMesh) {
    return MMNEW(CInvulnerableVFX(pParent, pMesh));
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Event Callbacks
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // Function: �InvulnerableCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Activating the Passed in
  //			Component's Invulnerable Effects.
  ///////////////////////////////////////////////////////////////////////
  static void InvulnerableCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////
  // Function: �BoostCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Activating the Passed in
  //			Component's Boost Effects.
  ///////////////////////////////////////////////////////////////////////
  static void BoostCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////
  // Function: �Update�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Updating the Passed in
  //			Component's Invulnerable and Boost Effects and Check for
  //			end of Duration to Switch Effects Off.
  ///////////////////////////////////////////////////////////////////////
  static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////
  // Function: �WinStateInitCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Deactivating the Passed in
  //			Component's Invulnerable and Boost Effects on Enter of
  //Win 			State.
  ///////////////////////////////////////////////////////////////////////
  static void WinStateInitCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////
  // Function: �DestroyObjectCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Function is used to Handle Unregistering for Events
  //			when the Parent Object is Destroyed.
  ///////////////////////////////////////////////////////////////////////
  static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

  ///////////////////////////////////////////////////////////////////////////
  // Accessors
  ///////////////////////////////////////////////////////////////////////////

  // Parent Object
  CObject* GetParent(void) { return m_pParentObj; }

  // Active Flag
  bool IsActive(void) { return m_bActive; }

  // Auto-Manager
  static CComponentManager<CInvulnerableVFX>* GetManager(void) {
    // Auto-Manager
    static CComponentManager<CInvulnerableVFX> m_cManager;

    return &m_cManager;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Mutators
  ///////////////////////////////////////////////////////////////////////////

  // Active Flag
  void SetActive(bool bActive) { m_bActive = bActive; }
};

} // namespace scd
