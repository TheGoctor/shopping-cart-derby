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


enum ECartWeight
{
	NOWEIGHT, LWEIGHT, MWEIGHT, HWEIGHT
};

enum ETurnDirection
{
	TURNLEFT = -1, NOTURN, TURNRIGHT, DRIFTWAITINGFORTURN
};


class CObject;
class IEvent;
class IComponent;
class CMovement;

enum EAccelerateBehavior
{
	ACC_ERR = -1, ACCELERATE, BRAKE, COAST, GORIGHT, GOLEFT, DRIFTING, ACC_MAX
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
		static CMovementManager cMovementManager;
		return &cMovementManager;
	}
	static int CreateMovementComponent(lua_State* pLua);
	static CMovement* CreateMovementComponent(CObject* pObj);
	static void Shutdown(IEvent*, IComponent*);

	static int SetCartWeight(lua_State* pLua);

	bool PlayerIsInvincible(CObject* pPlayer);
	float GetPlayerSpeed(CObject* pPlayer);
	CMovement* GetMovementComponent(CObject* pOwner);

};

#endif //_CMOVEMENTMANAGER_H__H_