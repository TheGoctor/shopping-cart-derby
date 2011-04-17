#include "DXRenderContextManager.h"

DXRenderContextManager DXRenderContextManager::m_pInstance;


DXRenderContextManager::DXRenderContextManager(void)
{
}

DXRenderContextManager::~DXRenderContextManager(void)
{
}
DXRenderContextManager *DXRenderContextManager::GetInstance()
{
	return &m_pInstance;
}

void DXRenderContextManager::Initialize()
{
	BuildRenderContexts();
}

void DXRenderContextManager::BuildRenderContexts()
{
	// Flat
	m_cContexts[RC_FLAT].LoadShader("Resource/Shaders/basic.fx");
	m_cContexts[RC_FLAT].RenderFunc = DXRenderContext::FlatRenderContextFunc;
	m_cContexts[RC_FLAT].CreateRenderSet();
}

void DXRenderContextManager::RenderContexts()
{
	for(unsigned int uRCIdx = 0; uRCIdx < RC_MAX; ++uRCIdx)
	{
		m_cContexts[uRCIdx].RenderProcess();
	}
}

// Context Accessors
DXRenderContext* DXRenderContextManager::GetContext(ERenderContext eContextIdx)
{
	return &m_cContexts[eContextIdx];
}
