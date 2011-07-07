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

CBitmapFont::CBitmapFont(int nImageID, int nRectWidth, int nCharHeight,
						 int nNumCols, char chStartChar) : m_pTM(NULL)
{
	m_pTM = CTextureManager::GetInstance();
	SetImageID(nImageID);
	SetCharHeight(nCharHeight);
	SetNumColumns(nNumCols);
	SetStartChar(chStartChar);

	m_nRectWidth = nRectWidth;
}

CBitmapFont::~CBitmapFont(void)
{
}

void CBitmapFont::DrawString(const char* szString, int nPosX, int nPosY, 
							 float fScale, DWORD dwColor)
{
	int originX = nPosX;
	int len = strlen(szString);
	//int nCharWidth = 0;

	for (int i = 0; i<len; i++)
	{
		char ch = szString[i];

		int nID = ch - m_cStartChar;
		
		if (ch == ' ')
		{
			nPosX += (int)(m_nCharWidth[nID] * fScale);
			continue;
		}
		
		else if (ch == '\n')
		{
			nPosX = originX;
			nPosY += (int)(m_nCharHeight * fScale);
			continue;
		}

		RECT rLetter = CellAlgorithm(nID);
		m_pTM->Draw(GetImageID(), nPosX, nPosY, fScale, fScale, &rLetter, 0.0f, 0.0f, 0.0f, dwColor);
		nPosX += (int)(m_nCharWidth[nID] * fScale);
	}
}

void CBitmapFont::LoadFont(const char *szFilename, const char* szFileWidths)
{
	// set the image id of this bitmap font
	SetImageID(m_pTM->LoadTexture(szFilename));

	if(szFileWidths == NULL)
		return;

	// Load a binary file
	ifstream fin(szFileWidths, ios_base::binary | ios_base::in);
	if(fin.is_open())
	{
		fin.read((char*)m_nCharWidth, sizeof(int) * 128);
		fin.close();
		fin.clear();
	}
	else
	{
		// Binary file doesn't exist, so make one from a text file
		// read in the character widths into the array
		ifstream fin("Resource\\BitmapFont.txt");
		if (fin.is_open())
		{
			unsigned int i = 0;
			char buffer[5];
			while(!fin.eof())
			{
				fin.getline((char*)buffer, 5);
				m_nCharWidth[i] = atoi((const char*)buffer);
				++i;
			}
			fin.close();
			fin.clear();
		}
		ofstream out(szFileWidths, ios_base::binary | ios_base::out);
		if(out.is_open())
		{
			out.write((char*)m_nCharWidth, sizeof(int)*128);
			out.close();
			out.clear();
		}
	}
}

void CBitmapFont::UnLoadFont()
{
	m_pTM->UnloadTexture(GetImageID());
	memset(m_nCharWidth, 0, sizeof(int)*128);
}

RECT CBitmapFont::CellAlgorithm(int nID)
{
	RECT rCell;
	
	// pick out the proper letter from the image
	rCell.left = ((nID % m_nNumCols) * m_nRectWidth) + 
		((m_nRectWidth - m_nCharWidth[nID])/2);
	rCell.top = (nID / m_nNumCols) * m_nCharHeight;
	rCell.right = rCell.left + m_nCharWidth[nID];
	rCell.bottom = rCell.top + m_nCharHeight;

	return rCell;
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
