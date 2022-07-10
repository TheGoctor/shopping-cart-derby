#include "Settings.h"

#include <tclap/CmdLine.h>

#include <iostream>

scd::settings::settings()
  : _is_fullscreen(false)
  , _log_dir("./logs")
  , _window_aspect_ratio(16.f / 9.f)
  , _window_height(720)
  , _window_width(1280) {}

scd::settings::settings(int argc, char** argv) {
  this->load_cli(argc, argv);
}

bool scd::settings::load_cli(int argc, char** argv) {
  try {
    TCLAP::CmdLine cmd("Command Description Message", ' ', "0.0");

    TCLAP::SwitchArg is_fullscreen_arg("f", "fullscreen", "Fullscreen");

    TCLAP::ValueArg<std::string> log_dir_arg(
      "l", "logdir", "Log Directory", false, "./logs", "string");
    cmd.add(log_dir_arg);

    TCLAP::ValueArg<float> window_aspect_ratio(
      "a", "aspect-ratio", "Window Aspect Ratio", false, 16.f / 9.f, "float");
    cmd.add(window_aspect_ratio);

    TCLAP::ValueArg<int> window_height_arg(
      "h", "height", "Window Height", false, 720, "int");
    cmd.add(window_height_arg);

    TCLAP::ValueArg<int> window_width_arg(
      "w", "width", "Window Width", false, 1280, "int");
    cmd.add(window_width_arg);

    cmd.parse(argc, argv);

    if (is_fullscreen_arg.isSet())
      _is_fullscreen = is_fullscreen_arg.getValue();

    if (log_dir_arg.isSet())
      _log_dir = log_dir_arg.getValue();

    if (window_aspect_ratio.isSet())
      _window_aspect_ratio = window_aspect_ratio.getValue();

    if (window_height_arg.isSet())
      _window_height = window_height_arg.getValue();

    if (window_width_arg.isSet())
      _window_width = window_width_arg.getValue();

    _window_height = _window_width / _window_aspect_ratio;
  } catch (TCLAP::ArgException& e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId()
              << std::endl;

    return false;
  }

  return true;
}
