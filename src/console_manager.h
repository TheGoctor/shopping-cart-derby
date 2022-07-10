/**
 * @file console_manager.h
 *
 * @author Mac Reichelt
 *
 * @description Class displays a debugging console as text over the game screen.
 * Console can be used to display debugging information on different parts of
 * the screen. Also, users can type commands into the console to change data
 * in the game and in lua files.
 */

#pragma once

#include <string>

#include "core/base_component.h"

struct lua_State;

namespace scd {

// For convenience
#define Debug CConsoleManager::GetInstance()

class console_manager {
private:
  unsigned int _visible_line_count;

  std::string _current_line;
  std::string _console_buffer;

  bool _is_active;

  lua_State* _lua;

  console_manager();
  ~console_manager();

public:
  /**
   * Gets a singleton instance of the Console Manager
   *
   * @return Reference to the singleton
   */
  static console_manager& get();

  /**
   * Initializes the Console Manager and Lua
   */
  void initialize();

  /**
   * Closes and shuts down Lua
   *
   * @param[in] event Not used in this function
   * @param[in] component Not used in this function
   */
  static void shutdown(event*, base_component*);

  /**
   * Registers all the C++ functions that lua needs
   */
  void register_c_functions();

  /**
   * Loads the specified lua file into lua's memory and executes it
   *
   * @param[in] file_path	The path to the Lua file
   *
   * @return Whether the lua file successfully loaded.
   */
  bool load_lua_file(const std::string& file_path);

  /**
   * Displays the Console
   */
  void display();

  /**
   * Hides the console
   */
  void hide();

  /**
   * Draws the console to the screen
   */
  void draw();

  /**
   * Displays the contents of the Lua Stack to the Windows Console Screen and
   * the in-game console screen.
   */
  void dump_stack();

  /**
   * Prints the passed in text to the Windows Console and the In-Game Console
   *
   * @param[in] line The text to be displayed
   */
  void print(const std::string& line);
  static int print(lua_State* pLua);

  /**
   * Executes the passed in string as if it were Lua code
   *
   * @param[in] lua_code The line of code to be executed in Lua
   */
  void call_lua_function(const std::string& lua_code);

  /**
   * Executed when the Enter key is pressed from the console.
   * Executes the CurrentLine as Lua Code
   *
   * @param[in] lua_code The line of code to execute
   */
  void parse(const std::string& lua_code);

  /**
   * If the console is currently displayed, it hides, and vice versa
   *
   * @param[in] event Not used.
   * @param[in] component Not used.
   */
  static void toggle_console(event*, base_component*);

  /**
   * Checks for input
   *
   * @param[in] event Not used.
   * @param[in] component Not used.
   */
  static void update(event*, base_component*);
};

} // namespace scd
