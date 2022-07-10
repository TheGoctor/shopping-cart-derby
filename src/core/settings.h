#pragma once

#include <string>

namespace scd {
class settings {
public:
  settings() = default;
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
  bool _is_fullscreen{ false };
  std::string _log_dir{};
  int _window_width{ 1280 };
  int _window_height{ 720 };
  float _window_aspect_ratio{ static_cast<float>(_window_width) / _window_height };
};
} // namespace scd
