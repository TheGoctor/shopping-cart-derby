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
#include "CConsoleManager.h"

#include "..\..\..\CObject.h"
#include "..\Event Manager\CEventManager.h"
#include "..\Event Manager\EventStructs.h"
using namespace EventStructs;
#include "..\Object Manager\CObjectManager.h"
#include "..\Rendering Managers\Direct3DManager.h"

#define MAX_LINES 20

CConsoleManager::CConsoleManager() : m_bActive(false)
{
}

CConsoleManager::~CConsoleManager()
{
}

CConsoleManager* CConsoleManager::GetInstance()
{
	static CConsoleManager cConsoleManager;
	return &cConsoleManager;
}

bool CConsoleManager::Initialize()
{	
	m_szToDraw = "";
	m_nNumLines  = 0;
	//m_nMaxLines = MAX_LINES;

	string szState = "Update";
	szState += CONSOLE_STATE;
	CEventManager::GetInstance()->RegisterEvent("InputStateChange", (IComponent*)GetInstance(), ToggleConsole);

	CEventManager::GetInstance()->RegisterEvent("KeyPressed", (IComponent*)GetInstance(), Update);

	return true;
}

void CConsoleManager::Shutdown()
{
}

void CConsoleManager::ToggleConsole(IEvent* pEvent, IComponent* pComp)
{
	CConsoleManager* pDebug = GetInstance();

	if(((TStateEvent*)pEvent->GetData())->m_eToState != CONSOLE_STATE)
	{
		pDebug->HideConsole();
	}
	else
	{
		pDebug->DisplayConsole();
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

	Direct3DManager* pD3D = Direct3DManager::GetInstance();

	// So the text starts at the bottom of the screen and works its way up
	int nTop = 750 - ((m_nNumLines+1) * 32);
	RECT fontRec = {0, nTop, 500, 750};

	pD3D->GetFont()->DrawText(0, m_szToDraw.c_str(), -1, &fontRec, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}

void CConsoleManager::Print(string szTextToPrint)
{
}

void CConsoleManager::Update(IEvent* pEvent, IComponent* pComp)
{
	CConsoleManager* pDebug = (CConsoleManager*)pComp;
	int ch = ((TInputEvent*)pEvent->GetData())->m_nPlayer;

	if(ch)
	{
		pDebug->m_szToDraw += ch;

		if(ch == '\n')
		{
			// Parse and Execute CurrLine
			pDebug->Parse(pDebug->m_szCurrLine);

			pDebug->m_szCurrLine.clear();
			pDebug->m_szCurrLine;
			++pDebug->m_nNumLines;
		}
		else
		{
			pDebug->m_szCurrLine += ch;
		}

	}

	if(pDebug->m_nNumLines > MAX_LINES)
	{
		pDebug->m_szToDraw.erase(0, pDebug->m_szToDraw.find('\n')+1);
		--pDebug->m_nNumLines;
	}
}

void CConsoleManager::Parse(string szCommand)
{
	// Parse the string for any possible commands
}