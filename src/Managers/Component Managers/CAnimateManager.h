////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateManager.h
//	Date			:	5/12/11
//	Mod. Date		:	5/12/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Factory for Animate Components
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _CANIMATEMANAGER_H_
#define _CANIMATEMANAGER_H_

// Includes
#include <map>
using namespace std;
#include "..\\Global Managers\\Memory Manager\\CAllocator.h"

// GOC & LUA!
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// Foward Declares
class CAnimateComponent;
class DXMesh;
class CAnimation;
class CObject;
class IEvent;
class IComponent;

// Animate Manager
class CAnimateManager
{
private:

	// Singleton
	CAnimateManager(void);
	~CAnimateManager(void);

	// Map of Animate Components
	map<unsigned int, CAnimateComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAnimateComponent*>>> m_cAnimateComps;

public:

	// Singleton
	static CAnimateManager* GetInstance(void) 
	{ 
		static CAnimateManager pcAnimateMan; 
		return &pcAnimateMan; 
	}

	// Factory
	static int CreateAnimComp(lua_State* pLua);
	static CAnimateComponent* CreateAnimateComp(CObject* pParent, DXMesh* pMesh);

	// Init
	void Init(void);

	// Shutdown
	static void ShutdownCallback(IEvent*, IComponent*);
	void Shutdown(void);

	// Disable
	static void DisableCallback(IEvent*, IComponent*);
	void Disable();

	// Get Comp By ID
	CAnimateComponent* GetComponent(unsigned int uID)
	{
		map<unsigned int, CAnimateComponent*, less<unsigned int>,
			CAllocator<pair<unsigned int, CAnimateComponent*>>>::iterator compIter =
				m_cAnimateComps.find(uID);
		if(compIter == m_cAnimateComps.end())
			return NULL;
		else
			return compIter->second;
	}
};

#endif // _CANIMATEMANAGER_H_