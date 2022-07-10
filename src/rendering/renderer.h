#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <windows.h>
#define LEAN_AND_MEAN

#include <map>

#include "..\\Memory Manager\\scd::allocator.h"
#include "DXRenderContext.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class CEventManager;
class RenderContext;
class Direct3DManager;
class CCameraManager;
class ModelManager;
class DXRenderContextManager;
class DXMesh;
class CRenderComponent;
class CHUDManager;
class IEvent;
class IComponent;
class CObject;
class RenderNode;

class Renderer
{
	Direct3DManager*		m_pD3D;				// A pointer to the Direct3D Manager
	ModelManager*			m_pMM;				// A pointer to the Model Manager
	DXRenderContextManager* m_pRCM;
	CHUDManager*			m_pHUD;
	CEventManager*			m_pEM;
	CCameraManager*			m_pCM;

	std::map<unsigned int, CRenderComponent*, less<unsigned int>,
		scd::allocator<pair<unsigned int, CRenderComponent*>>> m_cRenderComps;

public:
	Renderer(void);
	~Renderer(void);
	
	void Init(HWND hWnd, int nScreenWidth, int nScreenHeight, bool bIsWindowed);

	void LoadModels(void);

	static Renderer* GetInstance(void) 
	{ 
		static Renderer cRenderer; 
		return &cRenderer; 
	}

	static void Render(RenderSet &set);

	static void AddToRenderSet(IEvent*, IComponent*);
	static void DestroyObject(IEvent*, IComponent*);

	void RenderScene(void);
	
	static int CreateRenderComp(lua_State* pLua);
	static CRenderComponent* CreateRenderComp(CObject* pParent, int nModelID,
								   int nRenderContextIdx, int nRenderFuncIdx);
	static CRenderComponent* CreateRenderComp(CObject* pParent, DXMesh* pMesh,
								   int nRenderContextIdx, int nRenderFuncIdx);

	void Shutdown(void);

	// Callbacks
	static void RenderCallback(IEvent* e, IComponent* comp);
	static void ShutdownCallback(IEvent* e, IComponent* comp);
	static void DestroyComponent(IEvent*, IComponent*);
};

#endif	// _RENDERER_H_