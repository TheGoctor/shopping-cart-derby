// TODO: File Header
#ifndef _CMOVEMENTMANAGER_H_
#define _CMOVEMENTMANAGER_H_

#include <map>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Global Managers\Memory Manager\CAllocator.h"


class CObject;
class IEvent;
class IComponent;
class CMovement;

enum EAccelerateBehavior
{
	ACC_ERR = -1, ACCELERATE, BRAKE, COAST, GORIGHT, GOLEFT, ACC_MAX
};

class CMovementManager
{
	std::map<unsigned int, CMovement*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CMovement*>>> m_cMovements;

	CMovementManager();
	~CMovementManager() {};
	CMovementManager(const CMovementManager&) {};
	CMovementManager& operator=(const CMovementManager&);

public:
	static CMovementManager* GetInstance()
	{
		static CMovementManager m_Instance;
		return &m_Instance;
	}
	static int CreateMovementComponent(lua_State* pLua);
	static CMovement* CreateMovementComponent(CObject* pObj);

	// Callback Functions
	static void HandleInputAccelerate(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputBrake(IEvent* cEvent, IComponent* cCenter);
	static void HandleInput(IEvent* cEvent, IComponent* cCenter);
	static void Update(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputLeft(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputRight(IEvent* cEvent, IComponent* cCenter);
	static void HandleBoost(IEvent* cEvent, IComponent* cCenter);
	static void HandleCollision(IEvent* cEvent, IComponent* cCenter);

	
	
	void AccelerateCart();
	void BrakeCart();
	void GoLeft();
	void GoRight();
	void SendUpdates(IEvent* cEvent);

};


#endif //_CMOVEMENTMANAGER_H__H_