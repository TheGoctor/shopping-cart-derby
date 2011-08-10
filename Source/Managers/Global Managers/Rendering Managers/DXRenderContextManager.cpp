#include "DXRenderContextManager.h"


DXRenderContextManager::DXRenderContextManager(void)
{
}

DXRenderContextManager::~DXRenderContextManager(void)
{
}
DXRenderContextManager *DXRenderContextManager::GetInstance()
{
	static DXRenderContextManager m_cDXRenderContextManager;
	return &m_cDXRenderContextManager;
}

void DXRenderContextManager::Initialize()
{
	BuildRenderContexts();
}

void DXRenderContextManager::ShutdownCallback(IEvent*, IComponent*)
{
	DXRenderContextManager* pMe = GetInstance();
	while(pMe->m_cShaderFiles.empty() == false)
	{
		SAFE_RELEASE(pMe->m_cShaderFiles.begin()->second);
		pMe->m_cShaderFiles.erase(pMe->m_cShaderFiles.begin());
	}
}

ID3DXEffect* DXRenderContextManager::LoadShader(const char *szFXFile)
{
	// Get Singletons
	Direct3DManager * pD3DMan = Direct3DManager::GetInstance();

	// Check for Vaild File Names
	if(NULL == szFXFile)
	{
		// Error
		MessageBoxA(0, szFXFile, "Trying to Load a Non-Existant Shader File", MB_OK);
		return NULL; // Return Failure
	}

	unsigned int nFileID = CIDGen::GetInstance()->GetID(szFXFile);
	ShaderMap::iterator pIter = m_cShaderFiles.find(nFileID);

	if(pIter != m_cShaderFiles.end())
	{
		return pIter->second;
	}
	else
	{
		// Create Shader
		ID3DXBuffer *errors;
		ID3DXEffect* pEffect = NULL;
		D3DXCreateEffectFromFile(pD3DMan->GetDirect3DDevice(), szFXFile, 0, 0, D3DXSHADER_DEBUG, 0, &pEffect, &errors);
		if (errors)
		{
			MessageBox(0, (LPCSTR)errors->GetBufferPointer(),0,0);
			return NULL;
		}
		else
		{
			m_cShaderFiles[nFileID] = pEffect;
			return pEffect;
		}
	}
}

void DXRenderContextManager::BuildRenderContexts()
{
	// Flat- VERT3 / COLOR3  ---/// NOT TO BE USED \\\---
	m_cContexts[RC_FLAT].m_pShaderEffect = LoadShader("Resource/Shaders/basic.fx");
	m_cContexts[RC_FLAT].RenderFunc = DXRenderContext::FlatRenderContextFunc;
	m_cContexts[RC_FLAT].CreateRenderSet();

	////////////////////////////////////////////////////////////////////////////////////////
	// Environment /////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	m_cContexts[RC_ABCART].AddTexture("Tex", "Resource/Textures/T_AbandonedCart_D.png");
	m_cContexts[RC_ABCART].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_ABCART].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_ABCART].CreateRenderSet();

	m_cContexts[RC_ACDUCT].AddTexture("Tex", "Resource/Textures/T_ACDuct_D.png");
	m_cContexts[RC_ACDUCT].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_ACDUCT].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_ACDUCT].CreateRenderSet();

	m_cContexts[RC_BAKERYSIGN].AddTexture("Tex", "Resource/Textures/T_BakerySign_D.png");
	m_cContexts[RC_BAKERYSIGN].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BAKERYSIGN].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BAKERYSIGN].CreateRenderSet();

	m_cContexts[RC_BCA].AddTexture("Tex", "Resource/Textures/T_BasicCutouts_D.png");
	m_cContexts[RC_BCA].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BCA].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BCA].CreateRenderSet();

	m_cContexts[RC_BCB].AddTexture("Tex", "Resource/Textures/T_BasicCutouts_D.png");
	m_cContexts[RC_BCB].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BCB].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BCB].CreateRenderSet();

	m_cContexts[RC_BCC].AddTexture("Tex", "Resource/Textures/T_BasicCutouts_D.png");
	m_cContexts[RC_BCC].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BCC].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BCC].CreateRenderSet();

	m_cContexts[RC_BOYSROOM].AddTexture("Tex", "Resource/Textures/T_BoysRestroom_D.png");
	m_cContexts[RC_BOYSROOM].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BOYSROOM].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BOYSROOM].CreateRenderSet();

	m_cContexts[RC_BBWINDOW].AddTexture("Tex", "Resource/Textures/T_ButcherBakeryWindow_D.png");
	m_cContexts[RC_BBWINDOW].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BBWINDOW].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BBWINDOW].CreateRenderSet();

	m_cContexts[RC_BUTCHERSIGN].AddTexture("Tex", "Resource/Textures/T_ButcherSign_D.png");
	m_cContexts[RC_BUTCHERSIGN].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BUTCHERSIGN].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BUTCHERSIGN].CreateRenderSet();

	m_cContexts[RC_BOXES].AddTexture("Tex", "Resource/Textures/T_CardBoardBoxes_D.png");
	m_cContexts[RC_BOXES].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BOXES].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BOXES].CreateRenderSet();

	m_cContexts[RC_CEILING0].AddTexture("Tex", "Resource/Textures/T_CeilingSet0_D.png");
	m_cContexts[RC_CEILING0].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CEILING0].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CEILING0].CreateRenderSet();

	m_cContexts[RC_CEILING1].AddTexture("Tex", "Resource/Textures/T_CeilingSet1_D.png");
	m_cContexts[RC_CEILING1].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CEILING1].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CEILING1].CreateRenderSet();

	m_cContexts[RC_DAIRY].AddTexture("Tex", "Resource/Textures/T_DairyItems_D.png");
	m_cContexts[RC_DAIRY].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_DAIRY].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_DAIRY].CreateRenderSet();

	m_cContexts[RC_DELISIGN].AddTexture("Tex", "Resource/Textures/T_DeliSign_D.png");
	m_cContexts[RC_DELISIGN].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_DELISIGN].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_DELISIGN].CreateRenderSet();

	m_cContexts[RC_DELIWINDOW].AddTexture("Tex", "Resource/Textures/T_DeliWindow_D.png");
	m_cContexts[RC_DELIWINDOW].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_DELIWINDOW].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_DELIWINDOW].CreateRenderSet();

	m_cContexts[RC_EMPDOOR].AddTexture("Tex", "Resource/Textures/T_EmployeeDoor_D.png");
	m_cContexts[RC_EMPDOOR].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_EMPDOOR].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_EMPDOOR].CreateRenderSet();

	m_cContexts[RC_EOTM].AddTexture("Tex", "Resource/Textures/T_EmployeeOfTheMonth_D.png");
	m_cContexts[RC_EOTM].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_EOTM].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_EOTM].CreateRenderSet();

	m_cContexts[RC_ENDCAP].AddTexture("Tex", "Resource/Textures/T_Endcap_2.png");
	m_cContexts[RC_ENDCAP].AddTexture("Tex2", "Resource/Textures/T_Endcap_1.png");
	m_cContexts[RC_ENDCAP].m_pShaderEffect = LoadShader("Resource/Shaders/MultiTexture.fx");
	m_cContexts[RC_ENDCAP].RenderFunc = DXRenderContext::TextureChangeContextFunc;
	m_cContexts[RC_ENDCAP].CreateRenderSet();

	m_cContexts[RC_EXITSIGN].AddTexture("Tex", "Resource/Textures/T_ExitSign_D.png");
	m_cContexts[RC_EXITSIGN].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_EXITSIGN].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_EXITSIGN].CreateRenderSet();

	m_cContexts[RC_FLOOR0].AddTexture("Tex", "Resource/Textures/T_FloorSet0_D.png");
	m_cContexts[RC_FLOOR0].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR0].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR0].CreateRenderSet();

	m_cContexts[RC_FLOOR1].AddTexture("Tex", "Resource/Textures/T_FloorSet1_D.png");
	m_cContexts[RC_FLOOR1].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR1].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR1].CreateRenderSet();

	m_cContexts[RC_FLOOR2].AddTexture("Tex", "Resource/Textures/T_FloorSet2_D.png");
	m_cContexts[RC_FLOOR2].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR2].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR2].CreateRenderSet();

	m_cContexts[RC_FLOOR3].AddTexture("Tex", "Resource/Textures/T_FloorSet3_D.png");
	m_cContexts[RC_FLOOR3].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR3].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR3].CreateRenderSet();

	m_cContexts[RC_FLOOR4].AddTexture("Tex", "Resource/Textures/T_FloorSet4_D.png");
	m_cContexts[RC_FLOOR4].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR4].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR4].CreateRenderSet();

	m_cContexts[RC_FLOOR5].AddTexture("Tex", "Resource/Textures/T_FloorSet5_D.png");
	m_cContexts[RC_FLOOR5].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR5].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR5].CreateRenderSet();

	m_cContexts[RC_FLOOR6].AddTexture("Tex", "Resource/Textures/T_FloorSet6_D.png");
	m_cContexts[RC_FLOOR6].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR6].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR6].CreateRenderSet();

	m_cContexts[RC_FLOOR7].AddTexture("Tex", "Resource/Textures/T_FloorSet7_D.png");
	m_cContexts[RC_FLOOR7].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR7].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR7].CreateRenderSet();

	m_cContexts[RC_FLOOR8].AddTexture("Tex", "Resource/Textures/T_FloorSet8_D.png");
	m_cContexts[RC_FLOOR8].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR8].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR8].CreateRenderSet();

	m_cContexts[RC_FLOOR9].AddTexture("Tex", "Resource/Textures/T_FloorSet9_D.png");
	m_cContexts[RC_FLOOR9].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOOR9].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOOR9].CreateRenderSet();

	m_cContexts[RC_FLOORTRIM].AddTexture("Tex", "Resource/Textures/T_FloorTrim_D.png");
	m_cContexts[RC_FLOORTRIM].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FLOORTRIM].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FLOORTRIM].CreateRenderSet();

	m_cContexts[RC_FRONTDOOR].AddTexture("Tex", "Resource/Textures/T_FrontDoor_D.png");
	m_cContexts[RC_FRONTDOOR].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FRONTDOOR].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FRONTDOOR].CreateRenderSet();

	m_cContexts[RC_FSDCL].AddTexture("Tex", "Resource/Textures/T_FSDCircleL_D.png");
	m_cContexts[RC_FSDCL].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSDCL].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSDCL].CreateRenderSet();

	m_cContexts[RC_FSDCM].AddTexture("Tex", "Resource/Textures/T_FSDCircleM_D.png");
	m_cContexts[RC_FSDCM].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSDCM].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSDCM].CreateRenderSet();

	m_cContexts[RC_FSDCS].AddTexture("Tex", "Resource/Textures/T_FSDCircleS_D.png");
	m_cContexts[RC_FSDCS].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSDCS].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSDCS].CreateRenderSet();

	m_cContexts[RC_FSDSL].AddTexture("Tex", "Resource/Textures/T_FSDSquareL_D.png");
	m_cContexts[RC_FSDSL].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSDSL].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSDSL].CreateRenderSet();

	m_cContexts[RC_FSDSM].AddTexture("Tex", "Resource/Textures/T_FSDSquareM_D.png");
	m_cContexts[RC_FSDSM].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSDSM].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSDSM].CreateRenderSet();

	m_cContexts[RC_FSDSS].AddTexture("Tex", "Resource/Textures/T_FSDSquareS_D.png");
	m_cContexts[RC_FSDSS].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_FSDSS].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSDSS].CreateRenderSet();

	m_cContexts[RC_GIRLSROOM].AddTexture("Tex", "Resource/Textures/T_GirlsRestroom_D.png");
	m_cContexts[RC_GIRLSROOM].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_GIRLSROOM].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_GIRLSROOM].CreateRenderSet();

	m_cContexts[RC_GLASSCOUNTER].AddTexture("Tex", "Resource/Textures/T_GlassCounter_D.png");
	m_cContexts[RC_GLASSCOUNTER].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_GLASSCOUNTER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_GLASSCOUNTER].CreateRenderSet();

	m_cContexts[RC_GLASSFREEZER].AddTexture("Tex", "Resource/Textures/T_GlassFreezer_D.png");
	m_cContexts[RC_GLASSFREEZER].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_GLASSFREEZER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_GLASSFREEZER].CreateRenderSet();

	m_cContexts[RC_HALFWALL].AddTexture("Tex", "Resource/Textures/T_HalfWall_D.png");
	m_cContexts[RC_HALFWALL].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_HALFWALL].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_HALFWALL].CreateRenderSet();

	m_cContexts[RC_HELPDESK].AddTexture("Tex", "Resource/Textures/T_HelpDesk_D.png");
	m_cContexts[RC_HELPDESK].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_HELPDESK].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_HELPDESK].CreateRenderSet();

	m_cContexts[RC_LIGHTS].AddTexture("Tex", "Resource/Textures/T_LightFixtures_D.png");
	m_cContexts[RC_LIGHTS].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_LIGHTS].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_LIGHTS].CreateRenderSet();

	m_cContexts[RC_LOBSTERSIGN].AddTexture("Tex", "Resource/Textures/T_LobsterCutout_D.png");
	m_cContexts[RC_LOBSTERSIGN].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_LOBSTERSIGN].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_LOBSTERSIGN].CreateRenderSet();

	m_cContexts[RC_LOBSTERTANK].AddTexture("Tex", "Resource/Textures/T_LobsterTank_D.png");
	m_cContexts[RC_LOBSTERTANK].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_LOBSTERTANK].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_LOBSTERTANK].CreateRenderSet();

	m_cContexts[RC_MEAT].AddTexture("Tex", "Resource/Textures/T_MeatSection_D.png");
	m_cContexts[RC_MEAT].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_MEAT].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_MEAT].CreateRenderSet();

	m_cContexts[RC_OFL].AddTexture("Tex", "Resource/Textures/T_OpenFreezerLeft_D.png");
	m_cContexts[RC_OFL].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_OFL].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_OFL].CreateRenderSet();

	m_cContexts[RC_OFR].AddTexture("Tex", "Resource/Textures/T_OpenFreezerRight_D.png");
	m_cContexts[RC_OFR].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_OFR].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_OFR].CreateRenderSet();

	m_cContexts[RC_OUTSIDE].AddTexture("Tex", "Resource/Textures/T_Outside_D.png");
	m_cContexts[RC_OUTSIDE].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_OUTSIDE].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_OUTSIDE].CreateRenderSet();

	m_cContexts[RC_PALLETS].AddTexture("Tex", "Resource/Textures/T_Pallets_D.png");
	m_cContexts[RC_PALLETS].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PALLETS].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PALLETS].CreateRenderSet();

	m_cContexts[RC_PIRATE].AddTexture("Tex", "Resource/Textures/T_Pirate_D.png");
	m_cContexts[RC_PIRATE].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PIRATE].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PIRATE].CreateRenderSet();

	m_cContexts[RC_PRODUCEL].AddTexture("Tex", "Resource/Textures/T_ProduceLeft_D.png");
	m_cContexts[RC_PRODUCEL].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PRODUCEL].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PRODUCEL].CreateRenderSet();

	m_cContexts[RC_PRODUCER].AddTexture("Tex", "Resource/Textures/T_ProduceRight_D.png");
	m_cContexts[RC_PRODUCER].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PRODUCER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PRODUCER].CreateRenderSet();

	m_cContexts[RC_REGISTER].AddTexture("Tex", "Resource/Textures/T_Register_D.png");
	m_cContexts[RC_REGISTER].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_REGISTER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_REGISTER].CreateRenderSet();
	
	m_cContexts[RC_SHELF].AddTexture("Tex", "Resource/Textures/T_Shelf_D.png");
	m_cContexts[RC_SHELF].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_SHELF].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SHELF].CreateRenderSet();
	
	m_cContexts[RC_SHELFSTUFF].AddTexture("Tex", "Resource/Textures/T_ShelfStuff_D.png");
	m_cContexts[RC_SHELFSTUFF].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_SHELFSTUFF].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SHELFSTUFF].CreateRenderSet();
	
	m_cContexts[RC_SNOWMAN].AddTexture("Tex", "Resource/Textures/T_Snowman_D.png");
	m_cContexts[RC_SNOWMAN].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_SNOWMAN].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SNOWMAN].CreateRenderSet();
	
	m_cContexts[RC_STARTLIGHT].AddTexture("Tex", "Resource/Textures/T_StartingLine1_D.png");
	m_cContexts[RC_STARTLIGHT].AddTexture("Tex2", "Resource/Textures/T_StartingLine2_D.png");
	m_cContexts[RC_STARTLIGHT].AddTexture("Tex3", "Resource/Textures/T_StartingLine3_D.png");
	m_cContexts[RC_STARTLIGHT].AddTexture("Tex4", "Resource/Textures/T_StartingLine4_D.png");
	m_cContexts[RC_STARTLIGHT].AddTexture("Tex5", "Resource/Textures/T_StartingLine5_D.png");
	m_cContexts[RC_STARTLIGHT].m_pShaderEffect = LoadShader("Resource/Shaders/MultiTexture.fx");
	m_cContexts[RC_STARTLIGHT].RenderFunc = DXRenderContext::TextureChangeContextFunc;
	m_cContexts[RC_STARTLIGHT].CreateRenderSet();
	
	m_cContexts[RC_UMBRA].AddTexture("Tex", "Resource/Textures/T_Umbra_D.png");
	m_cContexts[RC_UMBRA].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_UMBRA].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_UMBRA].CreateRenderSet();
	
	m_cContexts[RC_WALL0].AddTexture("Tex", "Resource/Textures/T_WallSet0_D.png");
	m_cContexts[RC_WALL0].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_WALL0].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_WALL0].CreateRenderSet();

	m_cContexts[RC_WALL1].AddTexture("Tex", "Resource/Textures/T_WallSet1_D.png");
	m_cContexts[RC_WALL1].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_WALL1].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_WALL1].CreateRenderSet();

	m_cContexts[RC_WALLSUPPORT].AddTexture("Tex", "Resource/Textures/T_WallSupport_D.png");
	m_cContexts[RC_WALLSUPPORT].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_WALLSUPPORT].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_WALLSUPPORT].CreateRenderSet();

	m_cContexts[RC_BREAKUP_SIGN0].AddTexture("Tex", "Resource/Textures/T_BreakUpSign0_D.png");
	m_cContexts[RC_BREAKUP_SIGN0].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BREAKUP_SIGN0].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BREAKUP_SIGN0].CreateRenderSet();

	m_cContexts[RC_BREAKUP_SIGN1].AddTexture("Tex", "Resource/Textures/T_BreakUpSign1_D.png");
	m_cContexts[RC_BREAKUP_SIGN1].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BREAKUP_SIGN1].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BREAKUP_SIGN1].CreateRenderSet();

	m_cContexts[RC_ALIEN].AddTexture("Tex", "Resource/Textures/T_Alien_D.png");
	m_cContexts[RC_ALIEN].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_ALIEN].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_ALIEN].CreateRenderSet();

	m_cContexts[RC_LITTLE_EVELYN].AddTexture("Tex", "Resource/Textures/T_LittleEvelyns_D.png");
	m_cContexts[RC_LITTLE_EVELYN].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_LITTLE_EVELYN].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_LITTLE_EVELYN].CreateRenderSet();

	////////////////////////////////////////////////////////////////////////////////////////
	// Goal Items //////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	m_cContexts[RC_GORILLA_MILK].AddTexture("Tex", "Resource/Textures/T_GorillaMilk_D.png");
	m_cContexts[RC_GORILLA_MILK].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_GORILLA_MILK].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_GORILLA_MILK].CreateRenderSet();
	
	m_cContexts[RC_CAPTAIN_FLAKEY].AddTexture("Tex", "Resource/Textures/T_CaptainFlakey_D.png");
	m_cContexts[RC_CAPTAIN_FLAKEY].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CAPTAIN_FLAKEY].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CAPTAIN_FLAKEY].CreateRenderSet();

	m_cContexts[RC_PIZZA].AddTexture("Tex", "Resource/Textures/T_Blue_Bomber_Pizza_D.png");
	m_cContexts[RC_PIZZA].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PIZZA].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PIZZA].CreateRenderSet();

	m_cContexts[RC_CAKE].AddTexture("Tex", "Resource/Textures/T_CAKE_D.png");
	m_cContexts[RC_CAKE].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CAKE].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CAKE].CreateRenderSet();

	m_cContexts[RC_CANNED_BEARS].AddTexture("Tex", "Resource/Textures/T_BearCan_D.png");
	m_cContexts[RC_CANNED_BEARS].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CANNED_BEARS].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CANNED_BEARS].CreateRenderSet();

	m_cContexts[RC_CARROTS].AddTexture("Tex", "Resource/Textures/T_Carrots_D.png");
	m_cContexts[RC_CARROTS].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CARROTS].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CARROTS].CreateRenderSet();

	m_cContexts[RC_SAMMY_SANDWICH].AddTexture("Tex", "Resource/Textures/T_SammySandwich_D.png");
	m_cContexts[RC_SAMMY_SANDWICH].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_SAMMY_SANDWICH].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SAMMY_SANDWICH].CreateRenderSet();

	m_cContexts[RC_SQUID].AddTexture("Tex", "Resource/Textures/T_Squid_D.png");
	m_cContexts[RC_SQUID].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_SQUID].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SQUID].CreateRenderSet();
	
	m_cContexts[RC_AGENT_ORANGE].AddTexture("Tex", "Resource/Textures/T_AgentOrange_D.png");
	m_cContexts[RC_AGENT_ORANGE].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_AGENT_ORANGE].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_AGENT_ORANGE].CreateRenderSet();
	////////////////////////////////////////////////////////////////////////////////////////
	// End Goal Items //////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	m_cContexts[RC_ANIM_STEVE].AddTexture("Tex", "Resource/Textures/T_Steve_D.png");
	m_cContexts[RC_ANIM_STEVE].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_STEVE].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_STEVE].CreateRenderSet();
	
	m_cContexts[RC_ANIM_EUGENE].AddTexture("Tex", "Resource/Textures/T_Eugene_D.png");
	m_cContexts[RC_ANIM_EUGENE].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_EUGENE].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_EUGENE].CreateRenderSet();
	
	m_cContexts[RC_ANIM_SCICART].AddTexture("Tex", "Resource/Textures/T_ScienceCart_D.png");
	m_cContexts[RC_ANIM_SCICART].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_SCICART].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_SCICART].CreateRenderSet();
	
	m_cContexts[RC_ANIM_BULLDOG].AddTexture("Tex", "Resource/Textures/T_BulldogBlue.png");
	m_cContexts[RC_ANIM_BULLDOG].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_BULLDOG].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_BULLDOG].CreateRenderSet();

	m_cContexts[RC_ANIM_MA].AddTexture("Tex", "Resource/Textures/T_MaBlue_D.png");
	m_cContexts[RC_ANIM_MA].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_MA].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_MA].CreateRenderSet();
	
	m_cContexts[RC_ANIM_BIKCART].AddTexture("Tex", "Resource/Textures/T_BikerCartBlack_D.png");
	m_cContexts[RC_ANIM_BIKCART].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_BIKCART].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_BIKCART].CreateRenderSet();
	
	m_cContexts[RC_ANIM_SEMOUR].AddTexture("Tex", "Resource/Textures/T_Semour_Yellow_D.png");
	m_cContexts[RC_ANIM_SEMOUR].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_SEMOUR].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_SEMOUR].CreateRenderSet();
	
	m_cContexts[RC_ANIM_DAVE].AddTexture("Tex", "Resource/Textures/T_Dave_Yellow_D.png");
	m_cContexts[RC_ANIM_DAVE].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_DAVE].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_DAVE].CreateRenderSet();
	
	m_cContexts[RC_ANIM_LARPCART].AddTexture("Tex", "Resource/Textures/T_LARPerCart_Yellow_D.png");
	m_cContexts[RC_ANIM_LARPCART].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_LARPCART].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_LARPCART].CreateRenderSet();
	
	m_cContexts[RC_ANIM_GERMINAL].AddTexture("Tex", "Resource/Textures/T_Germinal_Green_D.png");
	m_cContexts[RC_ANIM_GERMINAL].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_GERMINAL].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_GERMINAL].CreateRenderSet();
	
	m_cContexts[RC_ANIM_RICO].AddTexture("Tex", "Resource/Textures/T_Rico_Green_D.png");
	m_cContexts[RC_ANIM_RICO].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_RICO].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_RICO].CreateRenderSet();
	
	m_cContexts[RC_ANIM_BANDCART].AddTexture("Tex", "Resource/Textures/T_BanditoCart_Green_D.png");
	m_cContexts[RC_ANIM_BANDCART].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_BANDCART].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_BANDCART].CreateRenderSet();
	
	m_cContexts[RC_ANIM_SASHA].AddTexture("Tex", "Resource/Textures/T_SASHA_Blue_D.png");
	m_cContexts[RC_ANIM_SASHA].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_SASHA].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_SASHA].CreateRenderSet();

	m_cContexts[RC_ANIM_FITZ].AddTexture("Tex", "Resource/Textures/T_Fitz_D.png");
	m_cContexts[RC_ANIM_FITZ].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_FITZ].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_FITZ].CreateRenderSet();

	m_cContexts[RC_ANIM_PHAROH].AddTexture("Tex", "Resource/Textures/T_Pharaoh_D.png");
	m_cContexts[RC_ANIM_PHAROH].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_PHAROH].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_PHAROH].CreateRenderSet();

	m_cContexts[RC_ANIM_ALIENCART].AddTexture("Tex", "Resource/Textures/T_AlienCart_D.png");
	m_cContexts[RC_ANIM_ALIENCART].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_ALIENCART].RenderFunc = DXRenderContext::AnimationAlphaRenderContextFunc;
	m_cContexts[RC_ANIM_ALIENCART].CreateRenderSet();

	m_cContexts[RC_ANIM_EVELYN].AddTexture("Tex", "Resource/Textures/T_Evelyn_D.png");
	m_cContexts[RC_ANIM_EVELYN].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_EVELYN].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_EVELYN].CreateRenderSet();

	m_cContexts[RC_ANIM_SHADE].AddTexture("Tex", "Resource/Textures/T_Shade_D.png");
	m_cContexts[RC_ANIM_SHADE].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_SHADE].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_SHADE].CreateRenderSet();

	m_cContexts[RC_ANIM_NLCART].AddTexture("Tex", "Resource/Textures/T_CombinedNightlightCart_D.png");
	m_cContexts[RC_ANIM_NLCART].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_NLCART].RenderFunc = DXRenderContext::AnimationAlphaRenderContextFunc;
	m_cContexts[RC_ANIM_NLCART].CreateRenderSet();

	m_cContexts[RC_ANIM_DONUT].AddTexture("Tex", "Resource/Textures/FFP_2D_Donut_PH.png");
	m_cContexts[RC_ANIM_DONUT].m_pShaderEffect = LoadShader("Resource/Shaders/Character.fx");
	m_cContexts[RC_ANIM_DONUT].RenderFunc = DXRenderContext::AnimationRenderContextFunc;
	m_cContexts[RC_ANIM_DONUT].CreateRenderSet();

	m_cContexts[RC_BANANA].AddTexture("Tex", "Resource/Textures/FFP_2D_Banana_PH.png");
	m_cContexts[RC_BANANA].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_BANANA].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_BANANA].CreateRenderSet();

	m_cContexts[RC_CHICKENSOUP].AddTexture("Tex", "Resource/Textures/FFP_2D_Chicken_Noodle_PH.png");
	m_cContexts[RC_CHICKENSOUP].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_CHICKENSOUP].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CHICKENSOUP].CreateRenderSet();

	m_cContexts[RC_DONUT].AddTexture("Tex", "Resource/Textures/FFP_2D_Donut_Box_PH.png");
	m_cContexts[RC_DONUT].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_DONUT].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_DONUT].CreateRenderSet();

	m_cContexts[RC_TURKEYBOX].AddTexture("Tex", "Resource/Textures/FFP_2D_IceTurkeyBox_PH.png");
	m_cContexts[RC_TURKEYBOX].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_TURKEYBOX].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_TURKEYBOX].CreateRenderSet();

	m_cContexts[RC_JAM].AddTexture("Tex", "Resource/Textures/FFP_2D_Jam_PH.png");
	m_cContexts[RC_JAM].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_JAM].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_JAM].CreateRenderSet();

	m_cContexts[RC_PEANUTBUTTER].AddTexture("Tex", "Resource/Textures/FFP_2D_PeanutButter_PH.png");
	m_cContexts[RC_PEANUTBUTTER].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PEANUTBUTTER].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PEANUTBUTTER].CreateRenderSet();

	m_cContexts[RC_PIE].AddTexture("Tex", "Resource/Textures/FFP_2D_Pie_PH.png");
	m_cContexts[RC_PIE].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PIE].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PIE].CreateRenderSet();

	m_cContexts[RC_THOR].AddTexture("Tex", "Resource/Textures/FFP_2D_ThorsThunder_PH.png");
	m_cContexts[RC_THOR].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_THOR].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_THOR].CreateRenderSet();

	m_cContexts[RC_TURKEY].AddTexture("Tex", "Resource/Textures/T_IceTurkey_D.png");
	m_cContexts[RC_TURKEY].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_TURKEY].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_TURKEY].CreateRenderSet();

	m_cContexts[RC_BLUE_LIGHT_SPECIAL].AddTexture("Tex", "Resource/Textures/FFP_2D_BlueLamp_FIN.png");
	//m_cContexts[RC_BLUE_LIGHT_SPECIAL].AddTexture("Tex2", "Resource/Textures/FFP_2D_BlueLight_FIN.png");
	m_cContexts[RC_BLUE_LIGHT_SPECIAL].m_pShaderEffect = LoadShader("Resource/Shaders/MultiTexture.fx");
	m_cContexts[RC_BLUE_LIGHT_SPECIAL].RenderFunc = DXRenderContext::BlueLightContextFunc;
	m_cContexts[RC_BLUE_LIGHT_SPECIAL].CreateRenderSet();

	m_cContexts[RC_PEANUTPILE].AddTexture("Tex", "Resource/Textures/FFP_2D_PeanutSmash_FIN.png");
	m_cContexts[RC_PEANUTPILE].m_pShaderEffect = LoadShader("Resource/Shaders/Texture2D.fx");
	m_cContexts[RC_PEANUTPILE].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_PEANUTPILE].CreateRenderSet();

///// GOAL EFFECT

	m_cContexts[RC_ITEM_EFFECT_GLOW].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Glow_FIN.png");
	m_cContexts[RC_ITEM_EFFECT_GLOW].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_ITEM_EFFECT_GLOW].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_ITEM_EFFECT_GLOW].CreateRenderSet();

	m_cContexts[RC_ITEM_EFFECT_STAR].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Star_FIN.png");
	m_cContexts[RC_ITEM_EFFECT_STAR].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_ITEM_EFFECT_STAR].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_ITEM_EFFECT_STAR].CreateRenderSet();

	m_cContexts[RC_ITEM_EFFECT_SHINE].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Shine_FIN.png");
	m_cContexts[RC_ITEM_EFFECT_SHINE].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_ITEM_EFFECT_SHINE].RenderFunc = DXRenderContext::ParticleZWriteContextFunc;
	m_cContexts[RC_ITEM_EFFECT_SHINE].CreateRenderSet();

	m_cContexts[RC_GOAL_ICON_EFFECT_SHEET].AddTexture("Tex", "Resource/Textures/Particle Images/Goal Item Icons/FFP_2D_GoalItemSheet_FIN.png");
	m_cContexts[RC_GOAL_ICON_EFFECT_SHEET].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_GOAL_ICON_EFFECT_SHEET].RenderFunc = DXRenderContext::ParticleZWriteContextFunc;
	m_cContexts[RC_GOAL_ICON_EFFECT_SHEET].CreateRenderSet();

	m_cContexts[RC_CART_SMOKE_EFFECT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_TireSmoke_FIN.png");
	m_cContexts[RC_CART_SMOKE_EFFECT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_CART_SMOKE_EFFECT].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_CART_SMOKE_EFFECT].CreateRenderSet();

	m_cContexts[RC_CART_SKID_MARKS].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_TireBlob_FIN.png");
	m_cContexts[RC_CART_SKID_MARKS].m_pShaderEffect = LoadShader("Resource/Shaders/Skids.fx");
	m_cContexts[RC_CART_SKID_MARKS].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_CART_SKID_MARKS].CreateRenderSet();

	m_cContexts[RC_SCI_SKIDS].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_ScienceSkid_FIN.png");
	m_cContexts[RC_SCI_SKIDS].m_pShaderEffect = LoadShader("Resource/Shaders/Skids.fx");
	m_cContexts[RC_SCI_SKIDS].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_SCI_SKIDS].CreateRenderSet();	

	m_cContexts[RC_PB_MARKS].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_PBBlob_FIN.png");
	m_cContexts[RC_PB_MARKS].m_pShaderEffect = LoadShader("Resource/Shaders/Skids.fx");
	m_cContexts[RC_PB_MARKS].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_PB_MARKS].CreateRenderSet();

	m_cContexts[RC_CART_MUD].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_PeanutTrail_FIN.png");
	m_cContexts[RC_CART_MUD].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_CART_MUD].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_CART_MUD].CreateRenderSet();

	m_cContexts[RC_BOOST_GLOW_BLUE].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2DLightningSheet_FIN.png");
	m_cContexts[RC_BOOST_GLOW_BLUE].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_BOOST_GLOW_BLUE].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_BOOST_GLOW_BLUE].CreateRenderSet();

	m_cContexts[RC_BOOST_LIGHTNING].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Lightning_FIN.png");
	m_cContexts[RC_BOOST_LIGHTNING].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_BOOST_LIGHTNING].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_BOOST_LIGHTNING].CreateRenderSet();

	m_cContexts[RC_CLOUD_GRAY].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_CloudV2_FIN.png");
	m_cContexts[RC_CLOUD_GRAY].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_CLOUD_GRAY].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_CLOUD_GRAY].CreateRenderSet();

	m_cContexts[RC_TURKEY_KRACK].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_KrackSheet_FIN.png");
	m_cContexts[RC_TURKEY_KRACK].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_TURKEY_KRACK].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_TURKEY_KRACK].CreateRenderSet();

	m_cContexts[RC_SNOWFLAKE_EFFECT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Krack_FIN3.png");
	m_cContexts[RC_SNOWFLAKE_EFFECT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_SNOWFLAKE_EFFECT].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_SNOWFLAKE_EFFECT].CreateRenderSet();

	m_cContexts[RC_SNOWFLAKE_B_EFFECT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_SnowFlake_FIN.png");
	m_cContexts[RC_SNOWFLAKE_B_EFFECT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_SNOWFLAKE_B_EFFECT].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_SNOWFLAKE_B_EFFECT].CreateRenderSet();
	
	m_cContexts[RC_TURKEY_KRACK_SMALL].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Krack_FIN2.png");
	m_cContexts[RC_TURKEY_KRACK_SMALL].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_TURKEY_KRACK_SMALL].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_TURKEY_KRACK_SMALL].CreateRenderSet();

	m_cContexts[RC_TURKEY_KRACK_BIG].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Krack_FIN1.png");
	m_cContexts[RC_TURKEY_KRACK_BIG].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_TURKEY_KRACK_BIG].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_TURKEY_KRACK_BIG].CreateRenderSet();

	m_cContexts[RC_BROTH].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_BubblePopSheet_FIN.png");
	m_cContexts[RC_BROTH].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_BROTH].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_BROTH].CreateRenderSet();

	m_cContexts[RC_SOUP_SPARKLE].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_GlitterYellow_FIN.png");
	m_cContexts[RC_SOUP_SPARKLE].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_SOUP_SPARKLE].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_SOUP_SPARKLE].CreateRenderSet();

	m_cContexts[RC_FLOOR_SPLAT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_BananaSmush2_FIN.png");
	m_cContexts[RC_FLOOR_SPLAT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_FLOOR_SPLAT].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_FLOOR_SPLAT].CreateRenderSet();

	m_cContexts[RC_SHLOOP_POPUP].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_BananaShloop_FIN.png");
	m_cContexts[RC_SHLOOP_POPUP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_SHLOOP_POPUP].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_SHLOOP_POPUP].CreateRenderSet();

	m_cContexts[RC_DRIP_LEFT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_BananaGloopLft_FIN.png");
	m_cContexts[RC_DRIP_LEFT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_DRIP_LEFT].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_DRIP_LEFT].CreateRenderSet();

	m_cContexts[RC_DRIP_RIGHT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_BananaGloopRht_FIN.png");
	m_cContexts[RC_DRIP_RIGHT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_DRIP_RIGHT].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_DRIP_RIGHT].CreateRenderSet();

	m_cContexts[RC_PB_DRIP_LEFT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_PeanutBitsLft_FIN.png");
	m_cContexts[RC_PB_DRIP_LEFT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_PB_DRIP_LEFT].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_PB_DRIP_LEFT].CreateRenderSet();

	m_cContexts[RC_PB_DRIP_RIGHT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_PeanutBitsRht_FIN.png");
	m_cContexts[RC_PB_DRIP_RIGHT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_PB_DRIP_RIGHT].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_PB_DRIP_RIGHT].CreateRenderSet();

	m_cContexts[RC_SQUISH_POPUP].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_PeanutSquishSheet_FIN.png");
	m_cContexts[RC_SQUISH_POPUP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_SQUISH_POPUP].RenderFunc = DXRenderContext::ParticleAlphaContextFunc;
	m_cContexts[RC_SQUISH_POPUP].CreateRenderSet();

	// Crash Effect
	m_cContexts[RC_COLLISION_EFFECT_GREEN_SMALL].AddTexture("Tex", "Resource/Textures/Particle Images/Crash/FFP_2D_Crash1_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_GREEN_SMALL].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_GREEN_SMALL].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_GREEN_SMALL].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_GREEN_MID].AddTexture("Tex", "Resource/Textures/Particle Images/Crash/FFP_2D_Crash2_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_GREEN_MID].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_GREEN_MID].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_GREEN_MID].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_GREEN_BIG].AddTexture("Tex", "Resource/Textures/Particle Images/Crash/FFP_2D_Crash3_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_GREEN_BIG].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_GREEN_BIG].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_GREEN_BIG].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_CRASH_POPUP].AddTexture("Tex", "Resource/Textures/Particle Images/Crash/FFP_2D_CrashSheet_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_CRASH_POPUP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_CRASH_POPUP].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_CRASH_POPUP].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_RED_SMALL].AddTexture("Tex", "Resource/Textures/Particle Images/Pow/FFP_2D_Pow1_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_RED_SMALL].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_RED_SMALL].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_RED_SMALL].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_RED_MID].AddTexture("Tex", "Resource/Textures/Particle Images/Pow/FFP_2D_Pow2_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_RED_MID].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_RED_MID].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_RED_MID].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_RED_BIG].AddTexture("Tex", "Resource/Textures/Particle Images/Pow/FFP_2D_Pow3_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_RED_BIG].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_RED_BIG].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_RED_BIG].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_POW_POPUP].AddTexture("Tex", "Resource/Textures/Particle Images/Pow/FFP_2D_PowSheet_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_POW_POPUP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_POW_POPUP].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_POW_POPUP].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_BLUE_SMALL].AddTexture("Tex", "Resource/Textures/Particle Images/Smash/FFP_2D_Smash1_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_BLUE_SMALL].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_BLUE_SMALL].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_BLUE_SMALL].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_BLUE_MID].AddTexture("Tex", "Resource/Textures/Particle Images/Smash/FFP_2D_Smash2_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_BLUE_MID].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_BLUE_MID].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_BLUE_MID].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_BLUE_BIG].AddTexture("Tex", "Resource/Textures/Particle Images/Smash/FFP_2D_Smash3_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_BLUE_BIG].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_BLUE_BIG].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_BLUE_BIG].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_SMASH_POPUP].AddTexture("Tex", "Resource/Textures/Particle Images/Smash/FFP_2D_SmashSheet_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_SMASH_POPUP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_SMASH_POPUP].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_SMASH_POPUP].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_YELLOW_SMALL].AddTexture("Tex", "Resource/Textures/Particle Images/Crunch/FFP_2D_Crunch1_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_SMALL].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_SMALL].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_SMALL].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_YELLOW_MID].AddTexture("Tex", "Resource/Textures/Particle Images/Crunch/FFP_2D_Crunch2_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_MID].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_MID].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_MID].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_YELLOW_BIG].AddTexture("Tex", "Resource/Textures/Particle Images/Crunch/FFP_2D_Crunch3_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_BIG].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_BIG].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_YELLOW_BIG].CreateRenderSet();

	m_cContexts[RC_COLLISION_EFFECT_CRUNCH_POPUP].AddTexture("Tex", "Resource/Textures/Particle Images/Crunch/FFP_2D_CrunchSheet_FIN.png");
	m_cContexts[RC_COLLISION_EFFECT_CRUNCH_POPUP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_COLLISION_EFFECT_CRUNCH_POPUP].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_COLLISION_EFFECT_CRUNCH_POPUP].CreateRenderSet();

	m_cContexts[RC_JAM_USE_EFFECT_JAR].AddTexture("Tex", "Resource/Textures/Particle Images/JamFallSheet.png");
	m_cContexts[RC_JAM_USE_EFFECT_JAR].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_JAM_USE_EFFECT_JAR].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_JAM_USE_EFFECT_JAR].CreateRenderSet();

	m_cContexts[RC_JAM_HIT_EFFECT_BLOB].AddTexture("Tex", "Resource/Textures/Jam Sprites/FFP_2D_SplurchBlob_FIN.png");
	m_cContexts[RC_JAM_HIT_EFFECT_BLOB].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_JAM_HIT_EFFECT_BLOB].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_JAM_HIT_EFFECT_BLOB].CreateRenderSet();

	m_cContexts[RC_JAM_HIT_EFFECT_SPLURCH_POPUP].AddTexture("Tex", "Resource/Textures/Jam Sprites/FFP_2D_SplurchSheet_FIN.png");
	m_cContexts[RC_JAM_HIT_EFFECT_SPLURCH_POPUP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_JAM_HIT_EFFECT_SPLURCH_POPUP].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_JAM_HIT_EFFECT_SPLURCH_POPUP].CreateRenderSet();

	m_cContexts[RC_DONUT_EFFECT_PUFF].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_BurstSheetWhite_FIN.png");
	m_cContexts[RC_DONUT_EFFECT_PUFF].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_DONUT_EFFECT_PUFF].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_DONUT_EFFECT_PUFF].CreateRenderSet();

	m_cContexts[RC_BLUE_PUFF].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_BurstSheetBlue_FIN.png");
	m_cContexts[RC_BLUE_PUFF].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_BLUE_PUFF].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_BLUE_PUFF].CreateRenderSet();

	m_cContexts[RC_SPLAT_BURST].AddTexture("Tex", "Resource/Textures/Pie Sprites/FFP_2D_Splat_FIN.png");
	m_cContexts[RC_SPLAT_BURST].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_SPLAT_BURST].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_SPLAT_BURST].CreateRenderSet();

	m_cContexts[RC_SPLAT_POPUP].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Splat_Combo_FIN.png");
	m_cContexts[RC_SPLAT_POPUP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_SPLAT_POPUP].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_SPLAT_POPUP].CreateRenderSet();

	m_cContexts[RC_FIREWORK_BLUE].AddTexture("Tex", "Resource/Textures/Particle Images/Victory/FFP_2D_FireWorksBlue_FIN.png");
	m_cContexts[RC_FIREWORK_BLUE].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_FIREWORK_BLUE].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_FIREWORK_BLUE].CreateRenderSet();

	m_cContexts[RC_FIREWORK_GREEN].AddTexture("Tex", "Resource/Textures/Particle Images/Victory/FFP_2D_FireWorksGreen_FIN.png");
	m_cContexts[RC_FIREWORK_GREEN].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_FIREWORK_GREEN].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_FIREWORK_GREEN].CreateRenderSet();

	m_cContexts[RC_FIREWORK_BLACK].AddTexture("Tex", "Resource/Textures/Particle Images/Victory/FFP_2D_FireWorksGrey_FIN.png");
	m_cContexts[RC_FIREWORK_BLACK].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_FIREWORK_BLACK].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_FIREWORK_BLACK].CreateRenderSet();

	m_cContexts[RC_FIREWORK_ORANGE].AddTexture("Tex", "Resource/Textures/Particle Images/Victory/FFP_2D_FireWorksRed_FIN.png");
	m_cContexts[RC_FIREWORK_ORANGE].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_FIREWORK_ORANGE].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_FIREWORK_ORANGE].CreateRenderSet();

	m_cContexts[RC_BOLT].AddTexture("Tex", "Resource/Textures/Particle Images/FFP_2D_Bolt_FIN.png");
	m_cContexts[RC_BOLT].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_BOLT].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_BOLT].CreateRenderSet();


	m_cContexts[RC_CONFETTI_STRIP].AddTexture("Tex", "Resource/Textures/Particle Images/Victory/FFP_2D_ConfettiStrip_FIN.png");
	m_cContexts[RC_CONFETTI_STRIP].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_CONFETTI_STRIP].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_CONFETTI_STRIP].CreateRenderSet();

	m_cContexts[RC_CONFETTI_STAR].AddTexture("Tex", "Resource/Textures/Particle Images/Victory/FFP_2D_ConfettiWhite_FIN.png");
	m_cContexts[RC_CONFETTI_STAR].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_CONFETTI_STAR].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_CONFETTI_STAR].CreateRenderSet();

	m_cContexts[RC_CAUTION].AddTexture("Tex", "Resource/Textures/Particle Images/Winlossicon.png");
	m_cContexts[RC_CAUTION].m_pShaderEffect = LoadShader("Resource/Shaders/Particle.fx");
	m_cContexts[RC_CAUTION].RenderFunc = DXRenderContext::ParticleContextFunc;
	m_cContexts[RC_CAUTION].CreateRenderSet();

	// Shadows
	m_cContexts[RC_CHARACTER_SHADOW_BLOB].AddTexture("Tex", "Resource/Textures/T_Shadows_D.png");
	m_cContexts[RC_CHARACTER_SHADOW_BLOB].m_pShaderEffect = LoadShader("Resource/Shaders/Shadows.fx");
	m_cContexts[RC_CHARACTER_SHADOW_BLOB].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CHARACTER_SHADOW_BLOB].CreateRenderSet();

	m_cContexts[RC_CIRCLE_SHADOW].AddTexture("Tex", "Resource/Textures/Level Shadows/T_ShadowCircle_D.png");
	m_cContexts[RC_CIRCLE_SHADOW].m_pShaderEffect = LoadShader("Resource/Shaders/Shadows.fx");
	m_cContexts[RC_CIRCLE_SHADOW].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CIRCLE_SHADOW].CreateRenderSet();

	m_cContexts[RC_SHELF_SHADOW].AddTexture("Tex", "Resource/Textures/Level Shadows/T_ShadowShelves_D.png");
	m_cContexts[RC_SHELF_SHADOW].m_pShaderEffect = LoadShader("Resource/Shaders/Shadows.fx");
	m_cContexts[RC_SHELF_SHADOW].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SHELF_SHADOW].CreateRenderSet();
	
	m_cContexts[RC_FSDS_L_SHADOW].AddTexture("Tex", "Resource/Textures/Level Shadows/T_Shadow_FSD_Square_L_D.png");
	m_cContexts[RC_FSDS_L_SHADOW].m_pShaderEffect = LoadShader("Resource/Shaders/Shadows.fx");
	m_cContexts[RC_FSDS_L_SHADOW].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_FSDS_L_SHADOW].CreateRenderSet();
	
	m_cContexts[RC_SQUARE_SHADOW].AddTexture("Tex", "Resource/Textures/Level Shadows/T_Shadow_Special_D.png");
	m_cContexts[RC_SQUARE_SHADOW].m_pShaderEffect = LoadShader("Resource/Shaders/Shadows.fx");
	m_cContexts[RC_SQUARE_SHADOW].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_SQUARE_SHADOW].CreateRenderSet();
	
	m_cContexts[RC_CASHIER_SHADOW].AddTexture("Tex", "Resource/Textures/Level Shadows/T_Shadow_Cashier_D.png");
	m_cContexts[RC_CASHIER_SHADOW].m_pShaderEffect = LoadShader("Resource/Shaders/Shadows.fx");
	m_cContexts[RC_CASHIER_SHADOW].RenderFunc = DXRenderContext::TexturedRenderContextFunc;
	m_cContexts[RC_CASHIER_SHADOW].CreateRenderSet();
	
	
///////////////
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

// Factory
DXRenderContext* DXRenderContextManager::CreateRenderContext(
			string szTexFile, string szEffectFile, 
			void (*pRenderFunc)(RenderNode &node))
{
	// Get ID
	CIDGen* pIDGen = CIDGen::GetInstance();
	unsigned int uID = pIDGen->GetID(szTexFile);

	// Look for Match
	map<unsigned int, DXRenderContext*, less<unsigned int>,
		CAllocator<pair<unsigned int, DXRenderContext*>>>::iterator contextIter;
	contextIter = m_cDynamicContexts.find(uID);
	if(m_cDynamicContexts.end() == contextIter)
	{
		// Not Created Yet, Create a New Context
		DXRenderContext* pNewContext = MMNEW(DXRenderContext);

		// Add Textures (TODO: Add Multi-Textures)
		string szFilePath = "Resource\\Textures\\";
		szFilePath += szTexFile;
		pNewContext->AddTexture("Tex", szFilePath.c_str());

		// Set Effect File
		szFilePath = "Resource\\Shaders\\";
		szFilePath += szEffectFile;
		pNewContext->LoadShader(szFilePath.c_str());

		// Set Render Function
		pNewContext->RenderFunc = pRenderFunc;

		// Create Render Set
		pNewContext->CreateRenderSet();

		// Store New Context
		m_cDynamicContexts.insert(make_pair(uID, pNewContext));

		// Return the New Context
		return pNewContext;
	}
	else
	{
		// Found a Match
		return contextIter->second;
	}
}