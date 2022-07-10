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

#pragma once

#include <string>

#include "core/base_component.h"

struct lua_State;

namespace scd {

// For convenience
#define Debug CConsoleManager::GetInstance()

class console_manager {
private:
  unsigned int
    m_nNumLines; // The current number of lines present in the console window

  std::string
    _current_line; // String of text representing the current line of text
  std::string
    _console_buffer; // String of text representing all the text in the console

  bool _is_active; // Whether the console is currently active

  lua_State* m_pLua;

  // Constructor
  console_manager();

  ~console_manager();

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
  static console_manager& get();

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
  void initialize();

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
  static void shutdown(event*, base_component*);

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
  void register_c_functions();

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
  bool load_lua_file(const std::string& file_path);

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
  void display();

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
  void hide();

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
  void draw();

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
  void dump_stack();

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
  static int print(lua_State* pLua);
  void print(const std::string& line);

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
  void call_lua_function(const std::string& lua_code);

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
  void parse(const std::string& lua_code);

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
  static void toggle_console(event*, base_component*);

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
  static void update(event*, base_component*);
};

} // namespace scd
