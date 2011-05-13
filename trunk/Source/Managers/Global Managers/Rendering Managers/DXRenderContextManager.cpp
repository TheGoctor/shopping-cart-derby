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
	// Flat- VERT3 / COLOR3  ---/// NOT TO BE USED \\\---
	m_cContexts[RC_FLAT].LoadShader("Resource/Shaders/basic.fx");
	m_cContexts[RC_FLAT].RenderFunc = DXRenderContext::FlatRenderContextFunc;
	m_cContexts[RC_FLAT].CreateRenderSet();

	m_cContexts[RC_CART].AddTexture("Tex", "Resource/Textures/T_cart.png");
	m_cContexts[RC_CART].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CART].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CART].CreateRenderSet();

	m_cContexts[RC_CEILING].AddTexture("Tex", "Resource/Textures/T_Ceiling_D.png");
	m_cContexts[RC_CEILING].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CEILING].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CEILING].CreateRenderSet();

	m_cContexts[RC_ENDCAP].AddTexture("Tex", "Resource/Textures/T_Endcap_D.png");
	m_cContexts[RC_ENDCAP].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_ENDCAP].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_ENDCAP].CreateRenderSet();
	
	m_cContexts[RC_FLOOR].AddTexture("Tex", "Resource/Textures/T_Floor_D.png");
	m_cContexts[RC_FLOOR].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR].CreateRenderSet();
	
	m_cContexts[RC_FSD_C_L].AddTexture("Tex", "Resource/Textures/T_FreedisplayCircleLarge_D.png");
	m_cContexts[RC_FSD_C_L].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSD_C_L].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSD_C_L].CreateRenderSet();

	m_cContexts[RC_FSD_C_M].AddTexture("Tex", "Resource/Textures/T_FreedisplayCircleMedium_D.png");
	m_cContexts[RC_FSD_C_M].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSD_C_M].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSD_C_M].CreateRenderSet();

	m_cContexts[RC_FSD_C_S].AddTexture("Tex", "Resource/Textures/T_FreestandingCircleSmall_D.png");
	m_cContexts[RC_FSD_C_S].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSD_C_S].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSD_C_S].CreateRenderSet();

	m_cContexts[RC_FSD_S_L].AddTexture("Tex", "Resource/Textures/T_FreestandingSquare_Large_D.png");
	m_cContexts[RC_FSD_S_L].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSD_S_L].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSD_S_L].CreateRenderSet();

	m_cContexts[RC_FSD_S_M].AddTexture("Tex", "Resource/Textures/T_FreestandingSquare_Medium_D.png");
	m_cContexts[RC_FSD_S_M].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSD_S_M].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSD_S_M].CreateRenderSet();

	m_cContexts[RC_FSD_S_S].AddTexture("Tex", "Resource/Textures/T_FreestandingSquareSmall_D.png");
	m_cContexts[RC_FSD_S_S].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSD_S_S].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSD_S_S].CreateRenderSet();

	m_cContexts[RC_FRONT_DOOR].AddTexture("Tex", "Resource/Textures/T_Front_Door_D.png");
	m_cContexts[RC_FRONT_DOOR].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FRONT_DOOR].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FRONT_DOOR].CreateRenderSet();

	m_cContexts[RC_GLASS_COUNTER].AddTexture("Tex", "Resource/Textures/T_GlassCounter_D.png");
	m_cContexts[RC_GLASS_COUNTER].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_GLASS_COUNTER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_GLASS_COUNTER].CreateRenderSet();

	m_cContexts[RC_GLASS_FREEZER].AddTexture("Tex", "Resource/Textures/T_Freezer_D.png");
	m_cContexts[RC_GLASS_FREEZER].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_GLASS_FREEZER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_GLASS_FREEZER].CreateRenderSet();

	m_cContexts[RC_HALF_WALL].AddTexture("Tex", "Resource/Textures/T_Half_Wall_D.png");
	m_cContexts[RC_HALF_WALL].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_HALF_WALL].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_HALF_WALL].CreateRenderSet();

	m_cContexts[RC_LOBSTER].AddTexture("Tex", "Resource/Textures/T_LobsterTank_D.png");
	m_cContexts[RC_LOBSTER].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_LOBSTER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_LOBSTER].CreateRenderSet();

	m_cContexts[RC_OPFR_L].AddTexture("Tex", "Resource/Textures/T_OpenFreezerLeft_D.png");
	m_cContexts[RC_OPFR_L].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_OPFR_L].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_OPFR_L].CreateRenderSet();

	m_cContexts[RC_OPFR_R].AddTexture("Tex", "Resource/Textures/T_OpenFreezerRight_D.png");
	m_cContexts[RC_OPFR_R].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_OPFR_R].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_OPFR_R].CreateRenderSet();

	m_cContexts[RC_PRODUCE_L].AddTexture("Tex", "Resource/Textures/T_ProduceLeft_D.png");
	m_cContexts[RC_PRODUCE_L].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PRODUCE_L].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PRODUCE_L].CreateRenderSet();

	m_cContexts[RC_PRODUCE_R].AddTexture("Tex", "Resource/Textures/T_ProduceRight_D.png");
	m_cContexts[RC_PRODUCE_R].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PRODUCE_R].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PRODUCE_R].CreateRenderSet();

	m_cContexts[RC_REGISTER].AddTexture("Tex", "Resource/Textures/T_Register_D.png");
	m_cContexts[RC_REGISTER].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_REGISTER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_REGISTER].CreateRenderSet();

	m_cContexts[RC_SHELF_S].AddTexture("Tex", "Resource/Textures/T_Short Self_D.png");
	m_cContexts[RC_SHELF_S].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_SHELF_S].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SHELF_S].CreateRenderSet();

	m_cContexts[RC_SHELF_T].AddTexture("Tex", "Resource/Textures/T_Tall_Shelf_D.png");
	m_cContexts[RC_SHELF_T].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_SHELF_T].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SHELF_T].CreateRenderSet();

	m_cContexts[RC_WALL].AddTexture("Tex", "Resource/Textures/T_Outer_Walls_D.png");
	m_cContexts[RC_WALL].LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_WALL].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_WALL].CreateRenderSet();

	m_cContexts[RC_BONES].LoadShader("Resource/Shaders/basic.fx");
	m_cContexts[RC_BONES].RenderFunc = DXRenderContext::FlatRenderContextFunc;
	m_cContexts[RC_BONES].CreateRenderSet();
}

void DXRenderContextManager::RenderContexts()
{
	for(unsigned int uRCIdx = 0; uRCIdx < RC_MAX; ++uRCIdx)
	{
		m_cContexts[uRCIdx].RenderProcess();
	}
	//m_cContexts[RC_BONES].RenderProcess();
}

// Context Accessors
DXRenderContext* DXRenderContextManager::GetContext(ERenderContext eContextIdx)
{
	return &m_cContexts[eContextIdx];
}
