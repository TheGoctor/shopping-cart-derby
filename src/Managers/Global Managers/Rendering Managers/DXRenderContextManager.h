#ifndef _DXRENDERCONTEXTMANAGER_H_
#define _DXRENDERCONTEXTMANAGER_H_

#include "DXRenderContext.h"
#include "..\\..\\..\\Enums.h"


class DXRenderContextManager
{
private:
	// Singleton data
	DXRenderContextManager();
	DXRenderContextManager(const DXRenderContextManager &) 
	{
	}
	DXRenderContextManager &operator=(const DXRenderContextManager &) 
	{
	}

	///////////////////////////////////////////////
	// Render contexts
	DXRenderContext m_cContexts[RC_MAX];

	// Map of Contexts (Key = ID of Texture Name, Data = DXRenderContext)
	map<unsigned int, DXRenderContext*, less<unsigned int>,
		CAllocator<pair<unsigned int, DXRenderContext*>>> m_cDynamicContexts;

	void BuildRenderContexts();
	typedef map<unsigned int, ID3DXEffect*, less<unsigned int>, 
		CRenderAllocator<pair<unsigned int, ID3DXEffect*>>> ShaderMap;
	ShaderMap m_cShaderFiles;

	ID3DXEffect* LoadShader(const char *szFXFile);

public:
	///////////////////////////////////////////////
	// Singleton public methods

	// Gets a pointer to the instance
	static DXRenderContextManager *GetInstance();

	~DXRenderContextManager();
	///////////////////////////////////////////////
	// Standard methods
	
	void Initialize();
	void RenderContexts();

	// Init
	void Init(void);

	// Shutdown
	static void ShutdownCallback(IEvent*, IComponent*);
	void Shutdown(void);

	// Factory
	DXRenderContext* CreateRenderContext(string szTexFile = "default.jpg",
		string szEffectFile = "Texture2D.fx",
		void (*pRenderFunc)(RenderNode &node) = DXRenderContext::TexturedRenderContextFunc);

	///////////////////////////////////////////////
	// Render context accessors
	DXRenderContext* GetContext(ERenderContext eContextIdx);
};

#endif	// _DXRENDERCONTEXTMANAGER_H_