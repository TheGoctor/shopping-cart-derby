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

class CCamera;

class CCameraManager
{
private:
	CCamera* m_pCam;
	
	/////////////////
	// Constructor //
	/////////////////
	CCameraManager()
	{
	}

	/////////////////////
	// Trilogy of Evil //
	/////////////////////
	~CCameraManager()
	{
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
		static CCameraManager instance; 
		return &instance; 
	}

	// Initalize
	void Init(int nScreenWidth, int nScreenHeight);

	// Accessors
	inline CCamera* GetCam(void)
	{
		return m_pCam;
	}

	// Callbacks
	//static void ShutdownCallback(IEvent* e, IComponent* comp);
	static void AttachToCamCallback(IEvent* e, IComponent* comp);
	static void MoveCameraCallback(IEvent* e, IComponent* comp);
	static void UpdateCallback(IEvent* e, IComponent* comp);
};

#endif // _CCAMERACOMPONENT_H_