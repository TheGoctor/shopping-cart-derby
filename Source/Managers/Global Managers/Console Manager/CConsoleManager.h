////////////////////////////////////////////////////////////////////////////////
//	File			:	CConsoleManager.h
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

#ifndef _CCONSOLEMANAGER_H_
#define _CCONSOLEMANAGER_H_

#include <string>
#include <queue>
using std::string;
using std::deque;

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Memory Manager\CAllocator.h"
#include "..\..\..\IComponent.h"

class IEvent;
class CBitmapFontComp;

class CConsoleManager
{
private:
	//unsigned int m_nUpdateID;

	//unsigned int m_nMaxLines;
	unsigned int m_nNumLines;

	string m_szCurrLine;
	string m_szToDraw;

	bool m_bActive;

	// Constructor
	CConsoleManager();

	// Trilogy of Evil
	~CConsoleManager();
	CConsoleManager(const CConsoleManager&);
	CConsoleManager& operator=(const CConsoleManager&);

public:
	static CConsoleManager* GetInstance();

	bool Initialize();
	void Shutdown();

	void DisplayConsole();
	void HideConsole();

	void DrawConsole();

	void Print(string szTextToPrint);

	void Parse(string szCommand);

	static void ToggleConsole(IEvent*, IComponent*);
	static void Update(IEvent*, IComponent*);
};

typedef CConsoleManager Debug;

#endif