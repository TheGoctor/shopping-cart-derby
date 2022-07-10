/*
 * @file Settings.h
 *
 * @brief Contains and loads settings from various sources.
 *
 * @author Mac Reichelt
 *
 * @copyright (c) 2016 Mac Reichelt
 */
#ifndef __scd_settings_h__
#define __scd_settings_h__

#include <string>

namespace scd {
class settings {
public:
  settings();
  settings(int argc, char** argv);

  bool load_cli(int argc, char** argv);
  bool load_ini(const char* ini_file_path);
  bool load_lua(const char* lua_file_path);

  bool is_fullscreen() const { return _is_fullscreen; }
  std::string get_log_dir() const { return _log_dir; }
  float get_aspect_ratio() const { return _window_aspect_ratio; }
  int get_window_height() const { return _window_height; }
  int get_window_width() const { return _window_width; }

private:
  bool _is_fullscreen;
  std::string _log_dir;
  float _window_aspect_ratio;
  int _window_height;
  int _window_width;
};
} // namespace scd

#endif // __scd_settings_h__
