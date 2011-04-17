#ifndef _DXRENDERCONTEXTMANAGER_H_
#define _DXRENDERCONTEXTMANAGER_H_

#include "DXRenderContext.h"

enum ERenderContext { RC_MIN = -1, RC_FLAT, RC_MAX };

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
	static DXRenderContextManager m_pInstance;

	///////////////////////////////////////////////
	// Render contexts
	DXRenderContext m_cContexts[RC_MAX];

	void BuildRenderContexts();

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

	///////////////////////////////////////////////
	// Render context accessors
	DXRenderContext* GetContext(ERenderContext eContextIdx);
};

#endif	// _DXRENDERCONTEXTMANAGER_H_