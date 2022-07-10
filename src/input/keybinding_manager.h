// TODO: File Header
#ifndef _CKEYBINDSMANAGER_H_
#define _CKEYBINDSMANAGER_H_

#include <list>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Memory Manager\scd::allocator.h"
#include "..\\Rendering Managers\\Texture Managers\\CBitmapFont.h"
#include "../Memory Manager/CEventAllocator.h"

class IEvent;
class IComponent;

#define NUM_REBINDABLE_KEYS 10

struct TKeybindInfo
{
	int nKey; // cmd_
	CBitmapFontComp* pFont;
};

class CKeybindsManager
{
	float		m_fTimer;

	TKeybindInfo m_tKeyInfo[NUM_REBINDABLE_KEYS]; // 10 hardcoded to the number of mappable keys we have
	BYTE m_tDefaultKeyInfo[NUM_REBINDABLE_KEYS]; // keys for set to default button
	int m_tDefaultControllerButtonInfo[NUM_REBINDABLE_KEYS]; // keys for set to default button

	D3DXVECTOR2 m_vMainMenuLocation[NUM_REBINDABLE_KEYS];
	D3DXVECTOR2 m_vPauseMenuLocation[NUM_REBINDABLE_KEYS];

	bool m_bCatchingInput;
	int m_nIndexToChange;

	CBitmapFontComp* m_pDirectionsFont;

	bool m_bReadyForControllerCheck;

	list<unsigned int, CEventAllocator<unsigned int>> m_lUnbindableKeys;


public:
	CKeybindsManager();

	static CKeybindsManager* GetInstance()
	{
		static CKeybindsManager CKeybindsManager;
		return &CKeybindsManager;
	}

	void Init();

	static void HandleUpdate(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateEnter(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateExit(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateInit(IEvent* cEvent, IComponent* cCenter);
	static void HandleStateInitPause(IEvent* cEvent, IComponent* cCenter);
	static int SetKeyCallback(lua_State* pLua);
	static int SetBindDefaultCallback(lua_State* pLua);
};


#endif //_CKeybindsManager_H_