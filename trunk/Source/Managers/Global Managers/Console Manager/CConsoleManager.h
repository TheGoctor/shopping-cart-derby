////////////////////////////////////////////////////////////////////////////////
//	File			:	CConsoleManager.h
//	Date			:	5/3/11
//	Mod. Date		:	7/26/11
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

// For convenience
#define Debug CConsoleManager::GetInstance()

class CConsoleManager
{
private:
	unsigned int m_nNumLines;	// The current number of lines present in the console window

	string m_szCurrLine;		// String of text representing the current line of text
	string m_szToDraw;			// String of text representing all the text in the console 

	bool m_bActive;				// Whether the console is currently active

	// Lua State Pointer
	lua_State* m_pLua;

	// Constructor
	CConsoleManager();

	// Trilogy of Evil
	~CConsoleManager();
	CConsoleManager(const CConsoleManager&);
	CConsoleManager& operator=(const CConsoleManager&);

public:
////////////////////////////////////////////////////////////////////////////////
// GetInstance()	:	Gets a singleton instance of the Console Manager
//
// Ins				:	void
//
// Outs				:	void
//
// Returns			:	CConsoleManager&	-	Reference to the singleton
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	static CConsoleManager& GetInstance();

////////////////////////////////////////////////////////////////////////////////
// Initialize()		:	Initializes the Console Manager and Lua
//
// Ins				:	void
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void Initialize();
	
////////////////////////////////////////////////////////////////////////////////
// Shutdown()		:	Closes and shuts down Lua
//
// Ins				:	IEvent*		-	Not used in this function
//						IComponent*	-	Not used in this function
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	static void Shutdown(IEvent*, IComponent*);

////////////////////////////////////////////////////////////////////////////////
// RegisterCFunctions()		:	Registers all the C++ functions that lua needs
//
// Ins				:	void
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void RegisterCFunctions();
	
////////////////////////////////////////////////////////////////////////////////
// LoadLuaFile()	:	Loads the specified lua file into lua's memory and
//						and executes it
//
// Ins				:	string szFileName	-	The name of the Lua file
//
// Outs				:	void
//
// Returns			:	bool	-	True if the Lua file loaded, false if not
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	bool LoadLuaFile(string szFileName);

////////////////////////////////////////////////////////////////////////////////
// DisplayConsole()	:	Displays the Console
//
// Ins				:	void
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void DisplayConsole();
	
////////////////////////////////////////////////////////////////////////////////
// HideConsole()	:	Hides the console
//
// Ins				:	void
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void HideConsole();

////////////////////////////////////////////////////////////////////////////////
// DrawConsole()	:	Draws the console to the screen
//
// Ins				:	void
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void DrawConsole();

////////////////////////////////////////////////////////////////////////////////
// StackDump()		:	Displays the contents of the Lua Stack to the Windows
//						Console Screen and the in-game console screen
//
// Ins				:	void
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void StackDump();

////////////////////////////////////////////////////////////////////////////////
// Print()			:	Prints the passed in text to the Windows Console and the
//						In-Game Console
//
// Ins				:	string szTextToPrint	-	The text to be displayed
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	static int Print(lua_State* pLua);
	void Print(string szTextToPrint);

////////////////////////////////////////////////////////////////////////////////
// CallLuaFunc()	:	Executes the passed in string as if it were Lua code
//
// Ins				:	string szFunc	-	The line of code to be executed in
//											Lua
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void CallLuaFunc(string szFunc);

////////////////////////////////////////////////////////////////////////////////
// Parse()			:	Executed when the Enter key is pressed from the console.
//						Executes the CurrentLine as Lua Code
//
// Ins				:	string szCommand	-	The line of code to execute
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	void Parse(string szCommand);

////////////////////////////////////////////////////////////////////////////////
// ToggleConsole()	:	If the console is currently displayed, it hides, and
//						vice versa
//
// Ins				:	IEvent*		-	Not used
//						IComponent*	-	Not used
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	static void ToggleConsole(IEvent*, IComponent*);
	
////////////////////////////////////////////////////////////////////////////////
// Update()			:	Checks for input
//
// Ins				:	IEvent*		- Not used
//						IComponent*	- Not used
//
// Outs				:	void
//
// Returns			:	void
//
// Mod. Date		:	3/29/11
// Mod. Initials	:	MR
////////////////////////////////////////////////////////////////////////////////
	static void Update(IEvent*, IComponent*);
};

#endif