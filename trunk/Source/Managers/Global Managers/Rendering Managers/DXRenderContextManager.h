#ifndef _DXRENDERCONTEXTMANAGER_H_
#define _DXRENDERCONTEXTMANAGER_H_

#include "DXRenderContext.h"

enum ERenderContext { RC_MIN = -1, 
RC_FLAT, RC_CART, RC_CEILING, 
RC_ENDCAP, RC_FLOOR, RC_FSD_C_L, 
RC_FSD_C_M, RC_FSD_C_S, RC_FSD_S_L, 
RC_FSD_S_M, RC_FSD_S_S, RC_FRONT_DOOR,
RC_GLASS_COUNTER, RC_GLASS_FREEZER, RC_HALF_WALL, 
RC_LOBSTER, RC_OPFR_L, RC_OPFR_R, 
RC_PRODUCE_L, RC_PRODUCE_R, RC_REGISTER, 
RC_SHELF_S, RC_SHELF_T, RC_WALL, RC_MAX };

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