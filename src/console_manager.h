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

class lua_manager {
public:
  lua_manager();
  ~lua_manager();

  /**
   * Initializes the Console Manager and Lua
   */
  void initialize();

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
  void display_console();

  /**
   * Hides the console
   */
  void hide_console();

  /**
   * Draws the console to the screen
   */
  void draw_console();

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
  void execute(const std::string& lua_code);

  /**
   * Executed when the Enter key is pressed from the console.
   * Executes the CurrentLine as Lua Code
   *
   * @param[in] lua_code The line of code to execute
   */
  void parse(const std::string& lua_code);

  /**
   * If the console is currently displayed, it hides, and vice versa
   */
  static void toggle_console();

  /**
   * Checks for input
   */
  static void update();

private:
  std::uint32_t _visible_line_count;
  const std::uint32_t _max_line_count;

  std::string _current_line;
  std::string _console_buffer;

  bool _is_active;

  const std::string _script_root_directory;

  lua_State* _lua;
};

} // namespace scd
