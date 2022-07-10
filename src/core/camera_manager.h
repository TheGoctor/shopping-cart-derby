///////////////////////////////////////////////////////////////////////////////
//  File			:	"CCameraManager.h"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/12/11
//
//	Last Changed	:	04/12/11
//
//  Purpose			:	A manager for cameras
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "core/camera.h"

namespace scd {
class camera_manager {
private:
  camera* _camera = nullptr;

  camera_manager() = default;

public:
  // Singleton Instance
  static camera_manager& get() {
    static camera_manager _manager;
    return _manager;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Function: "Init"
  //
  // Return: void
  //
  // Parameters: int - the screen width
  //			int - the screen height
  //
  // Purpose:  Initializes the Camera and builds the prospective and registers
  //    for events
  ////////////////////////////////////////////////////////////////////////////////
  void init(int nScreenWidth, int nScreenHeight);

  // Accessors
  const camera* camera() const { return _camera; }

  // Function Callbacks
  static void AttachToCamCallback(IEvent* e, IComponent* comp);
  static void MoveCameraCallback(IEvent* e, IComponent* comp);
  static void UpdateCallback(IEvent* e, IComponent* comp);
  static void AttachToWinStateCallback(IEvent* e, IComponent* comp);
  static void AttachToLoseStateCallback(IEvent* e, IComponent* comp);
  static int AttachCamToPlayer(lua_State* pLua);
};
} // namespace scd
