// TODO: File Header
#ifndef _CSTEERINGMANAGER_H_
#define _CSTEERINGMANAGER_H_

#include <list>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Global Managers\Memory Manager\CAllocator.h"

class CSteering;
class CObject;
class IEvent;

enum ECartWeight
{
	NOWEIGHT, LWEIGHT, MWEIGHT, HWEIGHT
};

enum ETurnDirection
{
	TURNLEFT = -1, NOTURN, TURNRIGHT, DRIFTWAITINGFORTURN
};

class CSteeringManager
{
public:
	CSteeringManager();

	static CSteeringManager* GetInstance()
	{
		static CSteeringManager me;
		return &me;
	}

	
	static void HandleUpdate(IEvent* cEvent, IComponent* cCenter);	
	static void HandleInputSteerLeft(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputSteerRight(IEvent* cEvent, IComponent* cCenter);

	void Update(float fDt);
	void SteerLeft();
	void SteerRight();
	
};


#endif //_CSTEERINGMANAGER_H_