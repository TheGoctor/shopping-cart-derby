////////////////////////////////////////////////////////////////////////////////
//	File			:	CScrollingUVComp.h
//	Date			:	7/12/11
//	Mod. Date		:	7/12/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets Meshes Scroll UVs
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "components/component_manager.h"
#include "core/base_component.h"
#include "core/object.h"
#include "rendering/dx_mesh.h"

namespace scd::component {
// Scolling UVs
class CScrollingUVComp : public scd::base_component {
private:
  // Character Set Type
  ECharacterSet m_eCharacterSetType;

  // Mesh
  DXMesh* m_pMesh;

  // Modifiers
  float m_fSpeed;
  float m_fTopSpeed;
  float m_fMinSpeed;

  // Handlers
  void Update(float fDT);
  void UpdateSciCart(float fDT);
  void UpdateStoryTimeCart(float fDT);
  void UpdateEndGameCart(float fDT);

  // Helper Funcs
  static bool CheckForPlayerMatch(CObject* pParentObj, CObject* pOtherObj);

public:
  // Get Auto-Manager
  static CComponentManager<CScrollingUVComp>* GetManager(void) {
    // Auto-Manager
    static CComponentManager<CScrollingUVComp> m_cManager;

    return &m_cManager;
  }

  // Constructor
  CScrollingUVComp(CObject* pParent, DXMesh* pMesh, ECharacterSet eType)
      : m_pParentObj(pParent)
      , m_pMesh(pMesh)
      , m_eCharacterSetType(eType)
      , m_fSpeed(1.0f)
      , m_fTopSpeed(10.0f)
      , m_fMinSpeed(1.0f)

  {
    // Add to Auto Manager
    GetManager()->Add(this);

    // Add Component to Parent
    pParent->AddComponent(this);

    // Initalize
    Init();
  }

  // Initalize
  void Init(void);

  // Shutdown
  void Shutdown(void);

  // Destructor
  ~CScrollingUVComp(void) {
    // Remove from Manager
    GetManager()->Remove(this);
  }

  // Factory
  static int CreateScrollingUVComponent(lua_State* pLua);
  static CScrollingUVComp* CreateScrollingUVComponent(
      CObject* pParent,
      DXMesh* pMesh,
      ECharacterSet eType) {
    return MMNEW(CScrollingUVComp(pParent, pMesh, eType));
  }

  // Callbacks
  static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
  static void AccelerateCallback(IEvent* pEvent, IComponent* pComp);
  static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

  // Accessors
  CObject* GetParent(void) { return m_pParentObj; }
};
} // namespace scd::component
