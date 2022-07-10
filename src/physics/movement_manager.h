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

#include "Global Managers\Memory Manager\scd::allocator.h"


enum ECartWeight
{
	NOWEIGHT, LWEIGHT, MWEIGHT, HWEIGHT
};

enum ETurnDirection
{
	TURNLEFT = -1, NOTURN, TURNRIGHT, DRIFTWAITINGFORTURN
};


class scd::object;
class IEvent;
class scd::base_component;
class CMovement;

enum EAccelerateBehavior
{
	ACC_ERR = -1, ACCELERATE, BRAKE, COAST, GORIGHT, GOLEFT, DRIFTING, ACC_MAX
};

class CMovementManager
{
	std::map<unsigned int, CMovement*, less<unsigned int>, 
		scd::allocator<pair<unsigned int, CMovement*>>> m_cMovements;

	CMovementManager();
	~CMovementManager() {};
	CMovementManager(const CMovementManager&) {};
	CMovementManager& operator=(const CMovementManager&);

public:
	static CMovementManager* GetInstance()
	{
		static CMovementManager cMovementManager;
		return &cMovementManager;
	}
	static int CreateMovementComponent(lua_State* pLua);
	static CMovement* CreateMovementComponent(scd::object* pObj);
	static void Shutdown(IEvent*, scd::base_component*);

	static int SetCartWeight(lua_State* pLua);

	bool PlayerIsInvincible(scd::object* pPlayer);
	float GetPlayerSpeed(scd::object* pPlayer);
	CMovement* GetMovementComponent(scd::object* pOwner);

};

#endif //_CMOVEMENTMANAGER_H__H_