////////////////////////////////////////////////////////
//  File			:	"CTextureManager.cpp"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	04/05/11
//
//	Last Changed	:	07/24/11
//
//	Changed By		:	HN			
//
//  Purpose			:	Wrapper class for manageing Textures for Direct3D
////////////////////////////////////////////////////////

#include <assert.h>		// code in assertions gets compiled out in Release mode
#include <string>

#include "CTextureManager.h"
#include "..\\..\\..\\..\\CObject.h"

#include "../../../../Managers/Global Managers/Event Manager/CEventManager.h"
#include "../../../../Managers/Global Managers/Event Manager/EventStructs.h"
using namespace EventStructs;
#include "../../../../Managers/Global Managers/Memory Manager/scd::allocator.h"

//	For macros
//	Macro to safely Release COM Objects.
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			if (p) { p->Release(); p = NULL; }
#endif

//	Macro to safely delete pointers.
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			if (p) { delete p; p = NULL; }
#endif

//	Macro to safely delete array pointers.
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	if (p) { delete [] p; p = NULL; }
#endif

//	Macro for reporting Wrapper errors
#ifndef DXERROR
#define DXERROR(a)	{ OutputDebugString(a); OutputDebugString("\n"); return false; }
#endif

#pragma warning (disable : 4996)

CTextureManager::CTextureManager(void)
{
	m_lpDevice = NULL;
	m_lpSprite = NULL;
}

CTextureManager::~CTextureManager(void)
{
}

CTextureManager *CTextureManager::GetInstance(void)
{
	static CTextureManager cTextureManager;
	return &cTextureManager;
}

bool CTextureManager::InitTextureManager(LPDIRECT3DDEVICE9 _lpDevice, LPD3DXSPRITE _lpSprite)
{
	m_lpDevice = _lpDevice;
	m_lpDevice->AddRef();

	m_lpSprite = _lpSprite;
	m_lpSprite->AddRef();

	// Register for Events
	CEventManager::GetInstance()->RegisterEvent("RenderSprites",
		(IComponent*)GetInstance(), RenderSpritesCallback);

	CEventManager::GetInstance()->RegisterEvent("Shutdown",
		(IComponent*)GetInstance(), ShutdownCallback);

	return (m_lpDevice && m_lpSprite);
}

void CTextureManager::ShutdownTextureManager(void)
{
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		// Remove ref.
		m_Textures[i].ref = 0;

		// Release the texture if it's not being used.
		SAFE_RELEASE(m_Textures[i].texture);
		m_Textures[i].filename[0] = '\0';
	}

	// Clear the list of all loaded textures.
	m_Textures.clear();

	// Components
	set<CSpriteComponent*, TSpriteCompare,
		scd::allocator<CSpriteComponent*>>::iterator cIter;
	cIter = m_cSpriteComps.begin();
	while(cIter != m_cSpriteComps.end())
	{
		CSpriteComponent* pSC = *cIter;
		if(pSC)
		{
			MMDEL(pSC);
		}

		cIter++;
	}

	set<CBitmapFontComp*, less<CBitmapFontComp*>,
		scd::allocator<CBitmapFontComp*>>::iterator cBitIter;
	cBitIter = m_cBitmapFontComps.begin();
	while(cBitIter != m_cBitmapFontComps.end())
	{
		CBitmapFontComp* pBC = *cBitIter;
		if(pBC)
		{
			MMDEL(pBC);
		}

		cBitIter++;
	}

	//	Release our references to the sprite interface and d3d device
	SAFE_RELEASE(m_lpSprite);
	SAFE_RELEASE(m_lpDevice);
}

int CTextureManager::LoadTexture(const char* _szFilename, DWORD _dwColorkey)
{
	//	Make sure the filename is valid.
	if (!_szFilename)	return -1;

	// Make sure the texture isn't already loaded.
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		// compare strings without caring about upper or lowercase.
		if (stricmp(_szFilename, m_Textures[i].filename) == 0)	// 0 means they are equal.
		{
			m_Textures[i].ref++; // add a reference to this texture.
			return i;	// return the index.
		}
	}

	// Look for an open spot.
	int nID = -1;
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		if (m_Textures[i].ref == 0)
		{
			nID = i;
			break;
		}
	}

	// if we didn't find an open spot, load it in a new one
	if (nID == -1)
	{
		// A temp texture object.
		TEXTURE loaded;

		// Copy the filename of the loaded texture.
		strcpy(loaded.filename, _szFilename);

		// Load the texture from the given file.
		HRESULT hr = 0;
		if (FAILED(hr = D3DXCreateTextureFromFileEx(m_lpDevice, _szFilename, 0, 0, D3DX_DEFAULT, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
			D3DX_DEFAULT, _dwColorkey, 0, 0, &loaded.texture)))
		{
			// Failed.
			char szBuffer[256] = {0};
			sprintf(szBuffer, "Failed to Create Texture - %s", _szFilename); 
			MessageBox(0, szBuffer, "TextureManager Error", MB_OK);
			return -1;
		}

		// AddRef.
		loaded.ref = 1;

		// Get surface description (to find Width/Height of the texture)
		D3DSURFACE_DESC d3dSurfDesc;
		ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));

		loaded.texture->GetLevelDesc(0, &d3dSurfDesc);

		// Remember the Width and Height
		loaded.Width	= d3dSurfDesc.Width;
		loaded.Height	= d3dSurfDesc.Height;

		// Put the texture into the list.
		int nIdx = m_Textures.size();
		m_Textures[nIdx] = loaded;

		// Return the nID of the texture.
		return nIdx;
	}
	// we found an open spot
	else
	{
		// Make sure the texture has been released.
		SAFE_RELEASE(m_Textures[nID].texture);

		// Copy the filename of the loaded texture.
		strcpy(m_Textures[nID].filename, _szFilename);

		// Load the texture from the given file.
		HRESULT hr = 0;
		if (FAILED(hr = D3DXCreateTextureFromFileEx(m_lpDevice, _szFilename, 0, 0, D3DX_DEFAULT, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
			D3DX_DEFAULT, _dwColorkey, 0, 0, &m_Textures[nID].texture)))
		{
			// Failed.
			char szBuffer[256] = {0};
			sprintf(szBuffer, "Failed to Create Texture - %s", _szFilename); 
			MessageBox(0, szBuffer, "TextureManager Error", MB_OK);
			return -1;
		}

		// Get surface description (to find Width/Height of the texture)
		D3DSURFACE_DESC d3dSurfDesc;
		ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));

		m_Textures[nID].texture->GetLevelDesc(0, &d3dSurfDesc);

		// Remember the Width and Height
		m_Textures[nID].Width	= d3dSurfDesc.Width;
		m_Textures[nID].Height	= d3dSurfDesc.Height;

		// AddRef
		m_Textures[nID].ref = 1;

		// Return the nID of the texture.
		return nID;
	}
}

void CTextureManager::UnloadTexture(int _nID)
{
	// Make sure the nID is in range.
	assert(_nID > -1 && _nID < (int)m_Textures.size() && "nID is out of range");
	
	// Remove ref.
	m_Textures[_nID].ref--;

	// Release the texture if it's not being used.
	if (m_Textures[_nID].ref <= 0)
	{
		// Do a lazy delete and leave this spot empty
		SAFE_RELEASE(m_Textures[_nID].texture);
		m_Textures[_nID].filename[0] = '\0';
		m_Textures[_nID].ref = 0;
	}
}

int CTextureManager::GetTextureWidth(int _nID)
{
	// Make sure the nID is in range.
	assert(_nID > -1 && _nID < (int)m_Textures.size() && "nID is out of range");

	return m_Textures[_nID].Width;
}

int CTextureManager::GetTextureHeight(int _nID)
{
	// Make sure the nID is in range.
	assert(_nID > -1 && _nID < (int)m_Textures.size() && "nID is out of range");

	return m_Textures[_nID].Height;
}

bool CTextureManager::Draw(int _nID, int _nX, int _nY, float _fScaleX, float _fScaleY,
	RECT* _pSection, float _fRotCenterX, float _fRotCenterY, float _fRotation, DWORD _dwColor)
{
	// Make sure the nID is in range.
	assert(_nID > -1 && _nID < (int)m_Textures.size() && "nID is out of range");

	// Make sure that the texture is valid
	assert(m_Textures[_nID].texture != NULL && "Attempting to draw released texture id");

	// Make sure the sprite was created and we have a valid texture.
	if (!m_lpSprite)
		return false;

	scd::transform scale;
	scd::transform rotation;
	scd::transform translate;
	scd::transform combined;

	// Initialize the Combined matrix.
	D3DXMatrixIdentity(&combined);

	// Scale the sprite.
	D3DXMatrixScaling(&scale, _fScaleX, _fScaleY, 1.0f);
	combined *= scale;

	// Rotate the sprite.
	D3DXMatrixTranslation(&translate, -_fRotCenterX * _fScaleX, -_fRotCenterY * _fScaleY, 0.0f);
	combined *= translate;
	D3DXMatrixRotationZ(&rotation, _fRotation);
	combined *= rotation;
	D3DXMatrixTranslation(&translate, _fRotCenterX * _fScaleX, _fRotCenterY * _fScaleY, 0.0f);
	combined *= translate;

	// Translate the sprite
	D3DXMatrixTranslation(&translate, (float)_nX, (float)_nY, 0.0f);
	combined *= translate;

	// Apply the transform.
	m_lpSprite->SetTransform(&combined);

	m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND);

		// Draw the sprite.
		if (FAILED(m_lpSprite->Draw(m_Textures[_nID].texture, _pSection, NULL, NULL, _dwColor)))
			DXERROR("Failed to draw the texture.");

	m_lpSprite->End();

	// Move the world back to identity.
	D3DXMatrixIdentity(&combined);
	m_lpSprite->SetTransform(&combined);

	// success.
	return true;
}

// Component Functions
int CTextureManager::CreateSpriteComp(lua_State* pLua)
{
	// Get Object and Values from LUA
	CObject* pObj = (CObject*)lua_topointer(pLua, -19);
	
	TSpriteData tSpriteData;
	tSpriteData.m_nTexture = GetInstance()->LoadTexture(
		(const char*)lua_tostring(pLua, -18));
	tSpriteData.m_nX			= (int)lua_tonumber(pLua, -17);
	tSpriteData.m_nY			= (int)lua_tonumber(pLua, -16);
	tSpriteData.m_nZ			= (int)lua_tonumber(pLua, -15);
	tSpriteData.m_fScaleX		= (float)lua_tonumber(pLua, -14);
	tSpriteData.m_fScaleY		= (float)lua_tonumber(pLua, -13);
	tSpriteData.m_fRotCenterX	= (float)lua_tonumber(pLua, -12);
	tSpriteData.m_fRotCenterY	= (float)lua_tonumber(pLua, -11);
	tSpriteData.m_fRot			= (float)lua_tonumber(pLua, -10);
	
	float fR = (float)lua_tonumber(pLua, -9);
	float fG = (float)lua_tonumber(pLua, -8);
	float fB = (float)lua_tonumber(pLua, -7);
	float fA = (float)lua_tonumber(pLua, -6);
	tSpriteData.m_dwColor = scd::vector4(fR, fG, fB, fA);
		
	tSpriteData.m_tRect.left   = (LONG)lua_tonumber(pLua, -5);
	tSpriteData.m_tRect.top	   = (LONG)lua_tonumber(pLua, -4);
	tSpriteData.m_tRect.right  = (LONG)lua_tonumber(pLua, -3);
	tSpriteData.m_tRect.bottom = (LONG)lua_tonumber(pLua, -2);

	bool bActive = (0 != lua_toboolean(pLua, -1)); // To properly cast int to bool

	// Create Component
	/*CSpriteComponent* pSpriteComp = */CreateSpriteComp(pObj, tSpriteData, bActive);

	// End LUA
	lua_pop(pLua, 19);
	return 0;
}

CSpriteComponent* CTextureManager::CreateSpriteComp(CObject* pParent,
		TSpriteData tSpriteData, bool bActive)
{
	// Create Component
	CSpriteComponent *pComp = MMNEW(CSpriteComponent(pParent,
		tSpriteData));	

	pComp->SetActive(bActive);

	// Insert Component
	GetInstance()->m_cSpriteComps.insert(pComp);

	// Add to Parent
	pParent->AddComponent(pComp);

	// Return Component
	return pComp;
}

CBitmapFontComp* CTextureManager::CreateBitmapFontComp(CObject* pParent,
		string szWord, CBitmapFont cFont, int nX, int nY,
		float fScale, DWORD dwColor, bool bActive)
{
	// Create Component
	CBitmapFontComp *pComp = MMNEW(CBitmapFontComp(pParent, cFont, szWord, nX, nY, fScale, dwColor));	

	pComp->SetActive(bActive);

	// Insert Component
	GetInstance()->m_cBitmapFontComps.insert(pComp);

	// Add to Parent
	if(pParent)
	{
		pParent->AddComponent(pComp);
	}

	// Return Component
	return pComp;
}


// Render
void CTextureManager::RenderSpritesCallback(IEvent*, IComponent*)
{
	GetInstance()->RenderSprites();
}
void CTextureManager::RenderSprites(void)
{
	// Components
	set<CSpriteComponent*, TSpriteCompare,
		scd::allocator<CSpriteComponent*>>::iterator cIter;
	cIter = m_cSpriteComps.begin();
	while(cIter != m_cSpriteComps.end())
	{
		CSpriteComponent* pSC = *cIter;
		if(pSC && pSC->IsActive() && pSC->GetSpriteData().m_nZ < 100)
		{
			pSC->DrawSprite();
		}

		cIter++;
	}

	set<CBitmapFontComp*, less<CBitmapFontComp*>,
		scd::allocator<CBitmapFontComp*>>::iterator cBitIter;
	cBitIter = m_cBitmapFontComps.begin();
	while(cBitIter != m_cBitmapFontComps.end())
	{
		CBitmapFontComp* pBC = *cBitIter;
		if(pBC && pBC->IsActive())
		{
			pBC->DrawWord();
		}

		cBitIter++;
	}

	cIter = m_cSpriteComps.begin();
	while(cIter != m_cSpriteComps.end())
	{
		CSpriteComponent* pSC = *cIter;
		if(pSC && pSC->IsActive() && pSC->GetSpriteData().m_nZ >= 100)
		{
			pSC->DrawSprite();
		}

		cIter++;
	}
}

// Shutdown
void CTextureManager::ShutdownCallback(IEvent*, IComponent*)
{
	GetInstance()->ShutdownTextureManager();
}

#pragma warning (default : 4996)