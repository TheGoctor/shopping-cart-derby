// TODO: File Header
#ifndef _CSTARTOFRACEMANAGER_H_
#define _CSTARTOFRACEMANAGER_H_

#include <list>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Global Managers\Memory Manager\CAllocator.h"

class CStartOfRaceComponent;
class CObject;
class IEvent;
class IComponent;
class CSpriteComponent;

class CStartOfRaceManager
{
	float		m_fTotalDuration;
	float		m_fCurrentTimerValue;
	bool		m_bEnableInput;

	CObject*			m_pDisplay0;
	CObject*			m_pDisplay1;
	CObject*			m_pDisplay2;
	CObject*			m_pDisplay3;
	CSpriteComponent*	m_pDisplayComponent0;
	CSpriteComponent*	m_pDisplayComponent1;
	CSpriteComponent*	m_pDisplayComponent2;
	CSpriteComponent*	m_pDisplayComponent3;

public:
	CStartOfRaceManager();

	static CStartOfRaceManager* GetInstance()
	{
		static CStartOfRaceManager cStartOfRaceManager;
		return &cStartOfRaceManager;
	}

	static void HandleUpdate(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateEnter(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateExit(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateInit(IEvent* cEvent, IComponent* cCenter);
};


#endif //_CSTARTOFRACEMANAGER_H_