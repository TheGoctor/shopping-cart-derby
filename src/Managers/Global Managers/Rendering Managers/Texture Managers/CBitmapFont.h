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
#ifndef _CBITMAPFONT_H_
#define _CBITMAPFONT_H_

#include <windows.h>
#define LEAN_AND_MEAN

#include <string>
using std::string;

#include"..\\dxutil.h"

#include"..\\..\\..\\..\\IComponent.h"

#define NUM_CHARS 128;

class CTextureManager;
class CObject;

class CBitmapFont
{
private:
	// so we can draw to the screen
	//CSGD_TextureManager*	m_pTM;

	int		m_nImageID;
	int		m_nCharWidth[128]; // an array of widths for kernning
	int		m_nCharHeight;
	int		m_nNumCols;
	char	m_cStartChar;

	int		m_nRectWidth;

	CTextureManager* m_pTM;

	// private so only we can call it
	RECT CellAlgorithm(int nID);
	int GetWidth(char cLetter);
	
public:
		
	CBitmapFont(int nImageID = -1, int nRectWidth = 16,
		int nCharHeight = 16, int nNumCols = 16, char chStartChar = ' ');
	//copy constructor
	CBitmapFont(const CBitmapFont& bf)
	{
		m_pTM = bf.m_pTM;

		m_nImageID = bf.m_nImageID;
		memcpy(m_nCharWidth, bf.m_nCharWidth, sizeof(int)*128);
		m_nCharHeight = bf.m_nCharHeight;
		m_nNumCols = bf.m_nNumCols;
		m_cStartChar = bf.m_cStartChar;
		m_nRectWidth = bf.m_nRectWidth;
	}
	//Assignment operator
	CBitmapFont& operator=(const CBitmapFont&){return *this;}
	//Destructor
	~CBitmapFont();

	//Accessors
	int GetImageID(){return m_nImageID;}
	int GetCharHeight(){return m_nCharHeight;}
	int GetNumColumns(){return m_nNumCols;}
	char GetStartChar(){return m_cStartChar;}

	//Mutators
	void SetImageID(int nImageID){m_nImageID = nImageID;}
	void SetCharHeight(int nCharHeight){m_nCharHeight = nCharHeight;}
	void SetNumColumns(int nNumCol){m_nNumCols = nNumCol;}
	void SetStartChar(char cStartChar){m_cStartChar = cStartChar;}

	// Draws a specified string to a location with scale, rotation and color of choice
	void DrawString(const char* szString, int nPosX, int nPosY,
				float fScale = 1.0f, DWORD dwColor = D3DCOLOR_XRGB(255,255,255));
	// needs the file path and the file that contains the character widths
	void LoadFont(const char* szFilename, const char* szFileWidths);
	// unloads the image and sets the width array to 0;
	void UnLoadFont();
};

class CBitmapFontComp : public IComponent
{
private:
	
	CTextureManager*	m_pTM;		   // A Pointer to the Texture Manager
	CObject*			m_pcParent;	   // The Game Object this Comp us associated with
	string				m_szWord;
	CBitmapFont			m_cFont;
	bool				m_bActive;	   // A bool stating if the Sprite is Rendering
	int m_nX;
	int m_nY;
	float m_fScale;
	DWORD m_dwColor;

	CBitmapFontComp(const CBitmapFontComp& /*bf*/)
	{
	}

public:

	CBitmapFontComp(CObject* pParent, CBitmapFont cFont, string szWord,
		int nX, int nY, float fScale, DWORD dwColor);

	~CBitmapFontComp(void)
	{
	}

	void DrawWord(void);

	// Accessors
	inline bool IsActive(void)
	{
		return m_bActive;
	}

	// Mutator
	inline void SetWord(string szWord)
	{
		m_szWord = szWord;
	}

	inline void SetY(int nY)
	{
		m_nY = nY;
	}

	inline void SetActive(bool bActive)
	{
		m_bActive = bActive;
	}
	inline void	SetX(int nX)
	{
		m_nX = nX;
	}

	inline void SetColor(DWORD nColor)
	{
		m_dwColor = nColor;
	}

};

#endif