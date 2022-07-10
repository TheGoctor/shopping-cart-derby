#pragma once

#include "core/base_component.h"
#include "physics/math_types.h"

#include <array>
#include <string>

#define NUM_CHARS 128;

namespace scd {
class object;

struct color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

class bitmap_font {
public:
  bitmap_font(int image_id = -1, int rect_width = 16, int char_height = 16,
              int num_columns = 16, char start_char = ' ');

  ~bitmap_font();

  int image_id() const { return m_nImageID; }
  int char_height() const { return m_nCharHeight; }
  int num_columns() const { return m_nNumCols; }
  char start_char() const { return m_cStartChar; }

  void set_image_id(int nImageID) { m_nImageID = nImageID; }
  void set_char_height(int nCharHeight) { m_nCharHeight = nCharHeight; }
  void set_num_columns(int nNumCol) { m_nNumCols = nNumCol; }
  void set_start_char(char cStartChar) { m_cStartChar = cStartChar; }

  // Draws a specified string to a location with scale, rotation and color of
  // choice
  void draw_string(const char* string, int position_x, int position_y,
                   float scale  = 1.0f,
                   color color = {255, 255, 255, 255});

  // needs the file path and the file that contains the character widths
  void load_font(const char* file_name, const char* file_widths);

  // unloads the image and sets the width array to 0;
  void unload_font();

private:
  // so we can draw to the screen
  // CSGD_TextureManager*	m_pTM;

  int m_nImageID;
  int m_nCharWidth[128]; // an array of widths for kernning
  int m_nCharHeight;
  int m_nNumCols;
  char m_cStartChar;

  int m_nRectWidth;

  class texture_manager* m_pTM;

  // private so only we can call it
  geometry::rectangle CellAlgorithm(int nID);
  int width(char cLetter) const;
};

class font_component : public base_component {
private:
  std::weak_ptr<class texture_manager> _texture_manager;
  std::string _word;
  bitmap_font _font;

  std::array<int, 2> _position;

  float _scale;
  color _color;

public:
  font_component(object& parent, bitmap_font font);

  void draw_word();

  void set_word(const std::string& word) { _word = word; }

  void set_position(const std::array<int, 2>& position) { _position = position; }

  void set_color(color color) { _color = color; }

  void set_scale(float scale) { _scale = scale; }
};

} // namespace scd
