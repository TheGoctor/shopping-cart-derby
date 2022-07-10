////////////////////////////////////////////////////////
//  File			:	"CBitmapFont.h"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	4/02/2011
//
//	Last Changed	:	07/24/11
//
//	Changed By		:	HN
//
//  Purpose			:	Handles the Rendering of Bitmap Fonts
//
////////////////////////////////////////////////////////
#pragma once

#include "core/base_component.h"

#include <string>

#define NUM_CHARS 128;

namespace scd {
class CTextureManager;
class object;

class bitmap_font {
public:
  bitmap_font(int nImageID = -1, int nRectWidth = 16, int nCharHeight = 16,
              int nNumCols = 16, char chStartChar = ' ');

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
  void draw_string(const char* szString, int nPosX, int nPosY,
                   float fScale  = 1.0f,
                   DWORD dwColor = D3DCOLOR_XRGB(255, 255, 255));

  // needs the file path and the file that contains the character widths
  void load_font(const char* szFilename, const char* szFileWidths);

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

  CTextureManager* m_pTM;

  // private so only we can call it
  RECT CellAlgorithm(int nID);
  int width(char cLetter) const;
};

class font_component : public base_component {
private:
  CTextureManager* m_pTM; // A Pointer to the Texture Manager
  std::string m_szWord;
  bitmap_font m_cFont;

  int m_nX;
  int m_nY;

  float m_fScale;
  DWORD m_dwColor;

public:
  font_component(object* pParent, bitmap_font cFont, std::string szWord, int nX,
                  int nY, float fScale, DWORD dwColor);

  void draw_word(void);

  // Mutator
  void set_word(const std::string& szWord) { m_szWord = szWord; }

  void set_y(int nY) { m_nY = nY; }

  void set_x(int nX) { m_nX = nX; }

  void SetColor(DWORD nColor) { m_dwColor = nColor; }
};

} // namespace scd
