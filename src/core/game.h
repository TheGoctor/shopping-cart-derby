/**
 * @file Game.h
 *
 * @brief Creates an instance of The Game.
 *
 * @author Mac Reichelt
 *
 * @copyright (c) 2016 Mac Reichelt
 */

#pragma once

#include <Magnum/Platform/Sdl2Application.h>

#include <chrono>

#include <cstdint>

namespace scd {
class settings;

class game : public Magnum::Platform::Application {
public:
  // explicit game(int argc, char** argv);
  explicit game(const Arguments& arguments);

  ~game();

  std::uint32_t window_width() const { return _window_width; }
  std::uint32_t window_height() const { return _window_height; }
  bool is_fullscreen() const { return _fullscreen; }
  float fps() const { return _fps; }

  void set_window_width(std::uint32_t width) { _window_width = width; }
  void set_window_height(std::uint32_t height) { _window_width = height; }
  void set_fullscreen(bool is_fullscreen) { _fullscreen = is_fullscreen; }

  int main();

  void drawEvent() override;

private:
  std::chrono::high_resolution_clock _timer;

  float _frame_time;
  int _frame_count;
  float _fps;
  float _delta_time;
  std::uint32_t _prev_time;

  // Managers
  CEventManager* m_pEM; // A pointer to the Event Manager
  Renderer* m_pRenderer;
  CWwiseSoundManager* m_pSound;

  std::uint32_t _window_width;  // The width of the window in pixels
  std::uint32_t _window_height; // The height of the window in pixels
  bool _fullscreen;             // Whether the game is fullscreen or windowed
};
} // namespace scd
