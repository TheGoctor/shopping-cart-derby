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
#include "..\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"

class CStartOfRaceComponent;
class CObject;
class IEvent;
class IComponent;
class CSpriteComponent;
class DXRenderContext;
class CWwiseSoundManager;

class CStartOfRaceManager
{
	float		m_fTotalDuration;
	float		m_fCurrentTimerValue;
	bool		m_bEnableInput;
	bool		m_bHasPlayedSound;
	bool		m_bBikeStarted;
	bool		m_bWelcomeSound;
	CWwiseSoundManager* pSound;

	CSpriteComponent*	m_pDirectionsComp;
	bool				m_bWaitingForInput;

	DXRenderContext*	m_pStartingLightContex;

public:
	CStartOfRaceManager();

	void StartBikeSounds();
	static CStartOfRaceManager* GetInstance()
	{
		static CStartOfRaceManager cStartOfRaceManager;
		return &cStartOfRaceManager;
	}

	static void HandleUpdate(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateEnter(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateExit(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateInit(IEvent* cEvent, IComponent* cCenter);
	static void EnterPressed(IEvent* cEvent, IComponent* cCenter);
};


#endif //_CSTARTOFRACEMANAGER_H_