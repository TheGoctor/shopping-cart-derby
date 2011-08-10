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
#ifndef _CCAMERACOMPONENT_H_
#define _CCAMERACOMPONENT_H_

#include "Camera.h"
#include "..\Console Manager\CConsoleManager.h"

//class CCamera;
class CCameraManager
{
private:
	CCamera* m_pCam;
	/////////////////
	// Constructor //
	/////////////////
	CCameraManager()
	{
		m_pCam = NULL;
	}

	/////////////////////
	// Trilogy of Evil //
	/////////////////////
	~CCameraManager()
	{
		MMDEL( m_pCam );
	}
	CCameraManager(const CCameraManager&)
	{
	}
	CCameraManager& operator=(const CCameraManager&)
	{
	}

public:

	// Singleton Instance
	static CCameraManager* GetInstance(void) 
	{ 
		static CCameraManager cCameraManager; 
		return &cCameraManager; 
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
	void Init(int nScreenWidth, int nScreenHeight);

	// Accessors
	inline CCamera* GetCam(void)
	{
		return m_pCam;
	}

	// Function Callbacks
	static void AttachToCamCallback(IEvent* e, IComponent* comp);
	static void MoveCameraCallback(IEvent* e, IComponent* comp);
	static void UpdateCallback(IEvent* e, IComponent* comp);
	static void AttachToWinStateCallback(IEvent* e, IComponent* comp);
	static void AttachToLoseStateCallback(IEvent* e, IComponent* comp);
	static int CCameraManager::AttachCamToPlayer(lua_State* pLua);
};

#endif // _CCAMERACOMPONENT_H_