////////////////////////////////////////////////////////
//  File			:	"CBitmapFont.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//	Team			:	Falcon Fish Productions
//
//  Date Created	:	4/02/2011
//
//  Purpose			:	Handles the Rendering of Bitmap Fonts
//
////////////////////////////////////////////////////////
#include "CBitmapFont.h"
#include "CTextureManager.h"
#include "..\\..\\..\\..\\CObject.h"

#include <fstream>
using std::ifstream;
#include <string>
using std::string;

CBitmapFont::CBitmapFont(void) : m_pTM(NULL)
{
	m_pTM = CTextureManager::GetInstance();
	SetImageID(-1);
	SetCharHeight(0);
	SetRectWidth(0);
	SetNumColumns(0);
	SetStartChar(' ');
}

CBitmapFont::~CBitmapFont(void)
{
}

CBitmapFont* CBitmapFont::GetInstance()
{
	static CBitmapFont instance;
	return &instance;
}

void CBitmapFont::DrawString(const char* szString, int nPosX, int nPosY, 
							 float fScale, DWORD dwColor)
{
	int originX = nPosX;
	int len = strlen(szString);
	int nCharWidth = 0;

	for (int i = 0; i<len; i++)
	{
		char ch = szString[i];
		nCharWidth = GetWidth(ch);
		
		if (ch == ' ')
		{
			nPosX += (int)(nCharWidth * fScale);
			continue;
		}
		
		else if (ch == '\n')
		{
			nPosX = originX;
			nPosY += (int)(m_nCharHeight * fScale);
			continue;
		}

		int id = ch - m_cStartChar;
		RECT rLetter = CellAlgorithm(id);
		m_pTM->Draw(GetImageID(), nPosX, nPosY, fScale, fScale, &rLetter, 0.0f, 0.0f, 0.0f, dwColor);
		nPosX += (int)(nCharWidth * fScale);
	}
}

void CBitmapFont::LoadFont(const char *szFilename, const char* szFileWidths)
{
	// set the image id of this bitmap font
	SetImageID(m_pTM->LoadTexture(szFilename));

	// read in the character widths into the array
	ifstream fin(szFileWidths);
	if (fin.is_open())
	{
		unsigned int i = 0;
		char buffer[5];
		while(!fin.eof())
		{
			fin.getline((char*)buffer, 5);
			m_nWidths[i] = atoi((const char*)buffer);
			++i;
		}
		fin.close();
	}
}

void CBitmapFont::UnLoadFont()
{
	m_pTM->UnloadTexture(GetImageID());
	for (unsigned int i=0; i<128; ++i)
		m_nWidths[i] = 0;
}

RECT CBitmapFont::CellAlgorithm(int nID)
{
	RECT rCell;
	
	// pick out the proper letter from the image
	rCell.left = (nID % m_nNumCols) * m_nRectWidth;
	rCell.top = (nID / m_nNumCols) * m_nCharHeight;
	rCell.right = rCell.left + m_nRectWidth;
	rCell.bottom = rCell.top + m_nCharHeight;

	return rCell;
}

int CBitmapFont::GetWidth(char cLetter)
{
	return m_nWidths[cLetter - GetStartChar()];
}


// COMPONENT
CBitmapFontComp::CBitmapFontComp(CObject* pParent, CBitmapFont cFont,
								 string szWord, int nX, int nY, float fScale,
								 DWORD dwColor) :
	m_pTM(NULL), m_pcParent(pParent), m_szWord(szWord),
	m_cFont(cFont), m_bActive(false), m_nX(nX), m_nY(nY), m_fScale(fScale),
	m_dwColor(dwColor)
{
	m_pTM = CTextureManager::GetInstance();
}

// Draw
void CBitmapFontComp::DrawWord(void)
{
	// Check for NULL
	if(m_pTM)
	{
		// Draw the Sprite
		m_cFont.DrawString(m_szWord.c_str(), m_nX, m_nY, m_fScale, m_dwColor);
	}
}
