////////////////////////////////////////////////////////////////////////////////
//	File			:	CConsoleManager.cpp
//	Date			:	5/3/11
//	Mod. Date		:	5/3/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Class displays a debugging console as text over the game
//						screen. Console can be used to display debugging 
//						information on different parts of the screen. Also, 
//						users can type commands into the console to change data
//						in the game and in lua files.
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
using std::cout;

#include "CConsoleManager.h"

#include "..\..\..\CObject.h"
#include "..\Event Manager\CEventManager.h"
#include "..\Event Manager\EventStructs.h"
using namespace EventStructs;
#include "..\Rendering Managers\Direct3DManager.h"

// Register With Lua
#include "..\Object Manager\CObjectManager.h"
#include "..\..\Component Managers\CMovementManager.h"
#include "..\Rendering Managers\Renderer.h"
#include "..\Rendering Managers\Texture Managers\CTextureManager.h"
#include "..\Rendering Managers\Texture Managers\CHUDManager.h"
#include "..\Keybind Manager\CKeybindsManager.h"
#include "..\..\Component Managers\CAIManager.h"
#include "..\..\Component Managers\CCollisionManager.h"
#include "..\..\..\Components\Button\CButtonComponent.h"
#include "..\..\..\Components\Slider\CSliderComponent.h"
#include "..\..\..\Components\Scroller\CScrollerComponent.h"
#include "..\..\..\Components\Rendering\CShadowComp.h"
#include "..\..\..\Components\Rendering\CInvulnerableVFX.h"
#include "..\..\..\Components\Rendering\CAnimatedDonutComp.h"
#include "..\..\..\Components\Rendering\CSkidMarks.h"
#include "..\..\..\Components\Rendering\CPeanutButterVFXComp.h"
#include "..\..\..\Components\Rendering\CScrollingUVComp.h"
#include "..\..\Component Managers\CInventoryManager.h"
#include "..\..\Component Managers\CAnimateManager.h"
#include "..\..\Component Managers\CLevelManager.h"
#include "..\Camera Manager\CCameraManager.h"

#define MAX_LINES 40

CConsoleManager::CConsoleManager() : m_bActive(false), m_pLua(lua_open())
{
	// Initialize Lua
	luaL_openlibs(m_pLua);

	m_szToDraw = "";
	m_nNumLines  = 0;
}

CConsoleManager::~CConsoleManager()
{
}

CConsoleManager& CConsoleManager::GetInstance()
{
	static CConsoleManager cConsoleManager;
	return cConsoleManager;
}

void CConsoleManager::Initialize()
{	
	string szState = "Update";
	szState += CONSOLE_STATE;
	CEventManager::GetInstance()->RegisterEvent("Shutdown", (IComponent*)this, Shutdown);
	CEventManager::GetInstance()->RegisterEvent("InputStateChange", (IComponent*)this, ToggleConsole);
	CEventManager::GetInstance()->RegisterEvent("KeyPressed", (IComponent*)this, Update);

	RegisterCFunctions();
	
	LoadLuaFile("luatest.lua");

	lua_getglobal(m_pLua, "InitValues");
	lua_pcall(m_pLua, 0, 0, 0);
}

void CConsoleManager::Shutdown(IEvent*, IComponent*)
{
	lua_close(GetInstance().m_pLua);
}

void CConsoleManager::RegisterCFunctions()
{
	lua_register(m_pLua, "CreateObj", 
		&CObjectManager::CreateObject);
	lua_register(m_pLua, "CreateMovementComponent", 
		&CMovementManager::CreateMovementComponent);
	lua_register(m_pLua, "CreateRenderComp",
		&Renderer::CreateRenderComp);
	lua_register(m_pLua, "CreateSpriteComp",
		&CTextureManager::CreateSpriteComp);
	lua_register(m_pLua, "CreateAIComponent",
		&CAIManager::CreateAIComponent);
	lua_register(m_pLua, "CreateCollideableComponent",
		&CCollisionManager::CreateCollideableComponent);
	lua_register(m_pLua, "CreateButtonComponent",
		&CButtonComponent::CreateButtonComponent);
	lua_register(m_pLua, "CreateSliderComponent",
		&CSliderComponent::CreateSliderComponent);
	lua_register(m_pLua, "SetSliderValue",
		&CSliderComponent::SetSliderValue);
	lua_register(m_pLua, "CreateScrollerComponent",
		&CScrollerComponent::CreateScrollerComponent);
	lua_register(m_pLua, "SetNextButtonComponent",
		&CButtonComponent::SetNextButtonComponent);
	lua_register(m_pLua, "CreateInventoryComp",
		&CInventoryManager::CreateInventoryComp);
	lua_register(m_pLua, "CreateAnimComp",
		&CAnimateManager::CreateAnimComp);
	lua_register(m_pLua, "BindObjects",
		&CObjectManager::BindObjects);
	lua_register(m_pLua, "print",
		&CConsoleManager::Print);

	lua_register(m_pLua, "CreateUpdateStateEvent",
		&EventStructs::CreateUpdateStateEvent);
	lua_register(m_pLua, "CreateStateEvent",
		&EventStructs::CreateStateEvent);
	lua_register(m_pLua, "CreateRenderEvent",
		&EventStructs::CreateRenderEvent);
	lua_register(m_pLua, "CreateRamEvent",
		&EventStructs::CreateRamEvent);
	lua_register(m_pLua, "CreateObjectEvent",
		&EventStructs::CreateObjectEvent);
	lua_register(m_pLua, "CreateInputEvent",
		&EventStructs::CreateInputEvent);
	lua_register(m_pLua, "CreateHeadingEvent",
		&EventStructs::CreateHeadingEvent);
	lua_register(m_pLua, "CreateGoalItemEvent",
		&EventStructs::CreateGoalItemEvent);
	lua_register(m_pLua, "CreateWeightClassEvent",
		&EventStructs::CreateWeightClassEvent);
	lua_register(m_pLua, "CreateGoalItemCollectedEvent",
		&EventStructs::CreateGoalItemCollectedEvent);
	lua_register(m_pLua, "CreateStatusEffectEvent",
		&EventStructs::CreateStatusEffectEvent);
	lua_register(m_pLua, "PushState",
		&CStateManager::PushState);
	lua_register(m_pLua, "StateChange",
		&CStateManager::StateChange);
	lua_register(m_pLua, "Back",
		&CStateManager::Back);
	lua_register(m_pLua, "SendLuaEvent",
		&EventStructs::SendLuaEvent);
	lua_register(m_pLua, "SetCartWeight",
		&CMovementManager::SetCartWeight);
	lua_register(m_pLua, "SetCharacterPicked",
		&CHUDManager::SetCharacterPicked);
	lua_register(m_pLua, "CreateShadowComponent",
		&CShadowComp::CreateShadowComponent);
	lua_register(m_pLua, "Collisions",
		&CLevelManager::ToggleCollisionVision);
	lua_register(m_pLua, "Geometry",
		&CLevelManager::ToggleGeometryVision);
	lua_register(m_pLua, "CreateInvulnerableVFXComponent",
		&CInvulnerableVFX::CreateInvulnerableVFXComponent);
	lua_register(m_pLua, "CreateSkidMarksComponent",
		&CSkidMarks::CreateSkidMarksComponent);
	lua_register(m_pLua, "CreatePBVFXComp",
		&CPeanutButterVFXComp::CreatePBVFXComponent);
	lua_register(m_pLua, "CreateScrollingUVComponent",
		&CScrollingUVComp::CreateScrollingUVComponent);

	lua_register(m_pLua, "SetKey",
		&CKeybindsManager::SetKeyCallback);
	lua_register(m_pLua, "SetDefaultKeys",
		&CKeybindsManager::SetBindDefaultCallback);
	lua_register(m_pLua, "LuaGetObjectByName",
		&CObjectManager::GetObjectByName);
	lua_register(m_pLua, "CreateAnimatedDonutComp",
		&CAnimatedDonutComp::CreateAnimatedDonutComponent);
	lua_register(m_pLua, "AttachCameraTo",
		&CCameraManager::AttachCamToPlayer);
	lua_register(m_pLua, "SetGoalTri",
		&CAIManager::SetGoalTri);
}

bool CConsoleManager::LoadLuaFile(string szFileName)
{
	string szFilePath = "Source/Scripts/";
	szFilePath += szFileName;

	if(luaL_dofile(m_pLua, szFilePath.c_str()))
	{
		const char* error = lua_tostring(m_pLua, -1);
		MessageBoxA(0, error, szFileName.c_str(), MB_OK);
		lua_pop(m_pLua, 1);
		return false;
	}
	return true;
}

void CConsoleManager::ToggleConsole(IEvent* pEvent, IComponent*)
{
	CConsoleManager& pDebug = GetInstance();

	if(((TStateEvent*)pEvent->GetData())->m_eToState != CONSOLE_STATE)
	{
		pDebug.HideConsole();
	}
	else
	{
		pDebug.DisplayConsole();
	}
}

void CConsoleManager::DisplayConsole()
{
	m_bActive = true;
}

void CConsoleManager::HideConsole()
{
	m_bActive = false;
}

void CConsoleManager::DrawConsole()
{
	if(m_bActive == false)
		return;

	// So the text starts at the bottom of the screen and works its way up
	int nTop = 750 - ((m_nNumLines+1) * 16);
	RECT fontRec = {0, nTop, 500, 750};

	Direct3DManager::GetInstance()->GetFont()->DrawText(0, m_szToDraw.c_str(),
		-1, &fontRec, 0, scd::vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void CConsoleManager::StackDump()
{
	int i;
	int top = lua_gettop(m_pLua);
	for(i=1; i<=top; i++)
	{
		int t = lua_type(m_pLua, i);
		switch(t)
		{
		case LUA_TSTRING:
			{
				printf("%s", lua_tostring(m_pLua, i));
				break;
			}
		case LUA_TBOOLEAN:
			{
				printf(lua_toboolean(m_pLua, i) ? "true" : "false");
				break;
			}
		case LUA_TNUMBER:
			{
				printf("%g", lua_tonumber(m_pLua, i));
				break;
			}
		default:
			{
				printf("%s", lua_typename(m_pLua, t));
				break;
			}
		};
		printf(" ");
	}
	printf("\n");
}

int CConsoleManager::Print(lua_State* pLua)
{
	const char* szString = lua_tostring(pLua, -1);
	lua_pop(pLua, 1);

	Debug.Print(szString);

	return 0;
}

void CConsoleManager::Print(string szTextToPrint)
{
	m_szToDraw += szTextToPrint;
	if(*(m_szToDraw.end()-1) != '\n')
		m_szToDraw += '\n';
	
	if(++m_nNumLines > MAX_LINES)
	{
		m_szToDraw.erase(0, m_szToDraw.find('\n')+1);
		--m_nNumLines;
	}

	cout << szTextToPrint.c_str() << endl;
}

void CConsoleManager::CallLuaFunc(string szFunc)
{
	luaL_dostring(m_pLua, szFunc.c_str());
}

void CConsoleManager::Update(IEvent* pEvent, IComponent*)
{
	CConsoleManager& pDebug = GetInstance();
	char ch = ((TConsoleEvent*)pEvent->GetData())->m_chKeyPressed;

	if(ch)
	{
		if(ch == '\b' && pDebug.m_szCurrLine.size() > 0)
		{
			pDebug.m_szCurrLine.erase(pDebug.m_szCurrLine.end()-1);
			pDebug.m_szToDraw.erase(pDebug.m_szToDraw.end()-1);
			return;
		}
		else
		{
			pDebug.m_szToDraw += ch;
		}

		if(ch == '\n')
		{
			// Parse and Execute CurrLine
			pDebug.Parse(pDebug.m_szCurrLine);

			pDebug.m_szCurrLine.clear();
			pDebug.m_szCurrLine;
			++pDebug.m_nNumLines;
		}
		else
		{
			pDebug.m_szCurrLine += ch;
		}

	}

	if(pDebug.m_nNumLines > MAX_LINES)
	{
		pDebug.m_szToDraw.erase(0, pDebug.m_szToDraw.find('\n')+1);
		--pDebug.m_nNumLines;
	}
}

void CConsoleManager::Parse(string szCommand)
{
	// Parse the string for any possible commands (Run it through Lua)
	CallLuaFunc(szCommand);
}