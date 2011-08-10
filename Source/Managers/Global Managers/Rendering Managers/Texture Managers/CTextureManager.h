////////////////////////////////////////////////////////
//  File			:	"CTextureManager.h"
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
#ifndef _CTEXTUREMANAGER_H_
#define _CTEXTUREMANAGER_H_

#include <map>
#include <set>
// The include files for Direct3D9
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\\..\\..\\..\\Components\\Rendering\\CSpriteComponent.h"
#include "..\\..\\Memory Manager\\CAllocator.h"
#include "CBitmapFont.h"

// Foward Declares
class CObject;
class IEvent;
class CBitmapFontComp;

class CTextureManager
{
private:
	// All the data we need to contain a texture.
	typedef struct _TEXTURE
	{
		char				filename[_MAX_PATH];	// The filename of this texture.
		int					ref;					// The number of times this texture has be loaded.
		LPDIRECT3DTEXTURE9	texture;				// A pointer to the texture.
		int					Width;					// The width of the texture.
		int					Height;					// The Height of the texture.

		// Default constructor.
		_TEXTURE()
		{
			filename[0] = '\0';
			ref = 0;
			texture = NULL;
			Width = 0;
			Height = 0;
		}
	} TEXTURE, *LPTEXTURE;

	//Constructor
	CTextureManager(void);
	//Copy constructor
	CTextureManager(CTextureManager &ref);
	//Assignment operator
	CTextureManager &operator=(CTextureManager &ref);


	std::map<unsigned int, TEXTURE, less<unsigned int>,
		CAllocator<pair<unsigned int, TEXTURE>>>	m_Textures;			// A list of all the loaded textures.
	LPDIRECT3DDEVICE9				m_lpDevice;			// A pointer to the Direct3D device.
	LPD3DXSPRITE					m_lpSprite;			// A pointer to the sprite interface.
	static CTextureManager			m_Instance;			// An instance to this class.

	// Components
	struct TSpriteCompare
	{
		bool operator()(CSpriteComponent* lhs, CSpriteComponent* rhs)
		{
			if(lhs->GetSpriteData().m_nZ == rhs->GetSpriteData().m_nZ)
			{
				return lhs < rhs;
			}
			// Greater than operator
			return lhs->GetSpriteData().m_nZ < rhs->GetSpriteData().m_nZ;
		}
	};

	std::set<CSpriteComponent*, TSpriteCompare,
		CAllocator<CSpriteComponent*>>	m_cSpriteComps;	// A list of all the Sprite Components

	std::set<CBitmapFontComp*, less<CBitmapFontComp*>,
		CAllocator<CBitmapFontComp*>>	m_cBitmapFontComps;	// A list of all the Sprite Components

public:
	
	//Destructor
	~CTextureManager(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "GetInstance"
	//
	// Return: static CTextureManger* - returns an instance of the manager
	//
	// Parameters: void
	//
	//	Purpose:	Gets an instance to this class.
	///////////////////////////////////////////////////////////////////
	static CTextureManager *GetInstance(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "InitTextureManager"
	//
	// Return: bool - true if everything was successful
	//				flase if something didn't initialize correctly
	//
	// Parameters: LPDIRECT3DDEVICE9 - the handle to the d3d device
	//			LPD3DXSPRITE - the handle to the sprite device		
	//
	//	Purpose:	Initializes the texture manager.
	///////////////////////////////////////////////////////////////////
	bool InitTextureManager(LPDIRECT3DDEVICE9 _lpDevice, LPD3DXSPRITE _lpSprite);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "ShutdownTextureManager"
	//
	// Return: void
	//
	// Parameters: void
	//
	//	Purpose:	Unloads all the loaded textures and 
	//				releases references to sprite and d3d devices.
	///////////////////////////////////////////////////////////////////
	void ShutdownTextureManager(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "LoadTexture"
	//
	// Return: int - the id to the texture
	//
	// Parameters: - const char* - the string to the file
	//				DWORD - color key of the texture file
	//
	//	Purpose:	To load a texture from a file. 
	//  NOTE:		Image dimensions must be a power of 2 (i.e. 256x64).
	//				Supports .bmp, .dds, .dib, .hdr, .jpg, .pfm, .png, 
	//				.ppm, and .tga files. 
	///////////////////////////////////////////////////////////////////
	int LoadTexture(const char* _szFilename, DWORD _dwColorkey = 0);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "UnloadTexture"
	//
	// Return: void
	//
	// Parameters: int - the texture id to unload
	//
	//	Purpose:	Releases a reference to a given texture. When the
	//				reference to the texture is zero, the texture is
	//				released from memory.
	///////////////////////////////////////////////////////////////////
	void UnloadTexture(int _nID);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "GetTextureWidth"
	//
	// Return: int - the width of the texture
	//
	// Parameters: int - the texture id to get width
	//
	//	Purpose:	Gets the width of a specified texture.
	///////////////////////////////////////////////////////////////////
	int GetTextureWidth(int _nID);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "GetTextureHeight"
	//
	// Return: int - the height of the texture
	//
	// Parameters: int - the texture id
	//
	//	Purpose:	Gets the height of a specified texture.
	///////////////////////////////////////////////////////////////////
	int GetTextureHeight(int _nID);

	//	Purpose:	Draws a texture to the screen.
	//	NOTE:	Drawing a section of an image will only work properly if 
	//			that image is a power of 2!
	///////////////////////////////////////////////////////////////////
	bool Draw(int _nID, int _nX, int _nY, float _fScaleX = 1.0f, float _fScaleY = 1.0f,
		RECT* _pSection = NULL, float _fRotCenterX = 0.0f, float _fRotCenterY = 0.0f, 
		float _fRotation = 0.0f, DWORD _dwColor = 0xFFFFFFFF);


	// Component Functions
	static int CreateSpriteComp(lua_State* pLua);
	static CSpriteComponent* CreateSpriteComp(CObject* pParent,
		TSpriteData tSpriteData, bool bActive);

	static CBitmapFontComp* CreateBitmapFontComp(CObject* pParent,
		string szWord, CBitmapFont cFont, int nX, int nY,
		float fScale, DWORD dwColor, bool bActive);

	void RenderSprites(void);

	//Event callback functions
	static void RenderSpritesCallback(IEvent* e, IComponent* comp);
	static void ShutdownCallback(IEvent* e, IComponent* comp);
};
#endif // _CTEXTUREMANAGER_H_