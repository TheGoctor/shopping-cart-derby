////////////////////////////////////////////////////////////////////////////////
//	File			:	CShadowComp.h
//	Date			:	5/19/11
//	Mod. Date		:	5/19/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object have a Shadow
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "components/component_manager.h"
#include "core/base_component.h"
#include "core/object.h"
#include "enums.h"

namespace scd {
// Foward Declares
class CObject;
class DXMesh;
class CRenderComponent;

// Shadow Component
class CShadowComp : scd::base_component {
private:
  // Shadow Obj
  CObject* m_pShadowObj;

  // Type
  ECharacterSet m_eType;

  // Render Comp
  CRenderComponent* m_pRenComp;

  // Helper Funcs
  DXMesh* GetShadowMeshByCharacterSet(ECharacterSet eType);

public:
  // Get Auto-Manager
  static CComponentManager<CShadowComp>* GetManager(void) {
    // Auto-Manager
    static CComponentManager<CShadowComp> m_cManager;

    return &m_cManager;
  }

  // Constructor
  CShadowComp(CObject* pParent, ECharacterSet eType)
      : m_pParent(pParent)
      , m_eType(eType)
      , m_pRenComp(NULL)
      , m_pShadowObj(NULL) {
    // Add to Auto Manager
    GetManager()->Add(this);

    // Add Component to Parent
    pParent->AddComponent(this);

    // Initalize
    Init();
  }

  // Initalize
  void Init(void);

  // Destructor
  ~CShadowComp(void) {
    Shutdown();
    GetManager()->Remove(this);
  }

  // Shutdown
  void Shutdown(void);

  // Factory
  static int CreateShadowComponent(lua_State* pLua);
  static CShadowComp*
  CreateShadowComponent(CObject* pParent, ECharacterSet eType) {
    return MMNEW(CShadowComp(pParent, eType));
  }

  // Accessors
  CObject* GetParent(void) { return m_pParent; }
  ECharacterSet GetCharacterSetType(void) { return m_eType; }

  // Mutators
  void SetCharaterSetType(ECharacterSet eType) { m_eType = eType; }

  // Callbacks
  static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
  void Update(void);

  // Shutdown
  static void ShutdownCallback(IEvent* pEvent, IComponent* pComp);
};
} // namespace scd
