////////////////////////////////////////////////////////
//  File			:	"CHUDManager.cpp"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/10/11
//
//	Last Changed	:	07/24/11
//
//	Last Changed by :	HN
//
//  Purpose			:	Wrapper class for manageing HUD elements
////////////////////////////////////////////////////////
#include "CHUDManager.h"
#include "CIntroManager.h"
#include "..\\..\\Object Manager\\CObjectManager.h"
#include "..\\..\\Object Manager\\CObjectManager.h"
#include "..\\..\\Event Manager\\CEventManager.h"
#include "..\\..\\Event Manager\\EventStructs.h"
#include "..\\..\\Input Manager\\CInputManager.h"
#include "..\\..\\..\\Component Managers\\CSpawningManager.h"
#include "..\\..\\..\\Component Managers\\CLevelManager.h"
#include "..\\..\\..\\..\\Components\\Level\\CGoalItemComponent.h"
#include "..\\..\\..\\..\\Enums.h"
#include "..\\..\\Sound Manager\\CWwiseSoundManager.h"

#include <iostream>
using namespace std;

#define LIST_NUM_COL		(4)
#define LIST_CELL_WIDTH		(128)
#define LIST_CELL_HEIGHT	(128)
#define ENEMY_PROGRESS_X	(875)
#define CHECK_LIST_CELL_IDX (11)
#define CHECKOUT_FLAG_CELL_IDX (9)
#define SOLD_OUT_CELL_IDX	(21)
#define RANKS_Y_LOC			(50)

static const int g_yRankPos[4] = { 38, 100, 165, 228 };


CHUDManager::CHUDManager(void) : m_pEnemyProgress(NULL), m_pTM(NULL) ,
m_pButtonItem1(NULL), m_pButtonItem2(NULL), m_bUpdateSprites(false), m_fTimerAccumulator(0.0f)
{
}

CHUDManager::~CHUDManager(void)
{
}

CHUDManager& CHUDManager::operator=(const CHUDManager&)
{
	return *this;
}

RECT CHUDManager::CellAlgo(int nID, int nNumCols,
						   int nCellWidth, int nCellHeight)
{
	RECT rCell;

	// pick out the proper letter from the image
	rCell.left = (nID % nNumCols) * nCellWidth;
	rCell.top = (nID / nNumCols) * nCellHeight;
	rCell.right = rCell.left + nCellWidth;
	rCell.bottom = rCell.top + nCellHeight;

	return rCell;
}
//Return the player number of the player passed in
int CHUDManager::GetPlayerNum(CObject* player)
{
	
	if(player->GetID() == CIDGen::GetInstance()->GetID("Player0"))
	{
		return 0;
	}

	if(player->GetID() == CIDGen::GetInstance()->GetID("Player1"))
	{
		return 1;
	}

	if(player->GetID() == CIDGen::GetInstance()->GetID("Player2"))
	{
		return 2;
	}

	
	{
		return 3;
	}

	
}


void CHUDManager::Init(void)
{
	// Get Singletons
	m_pTM = CTextureManager::GetInstance();
	m_pOM = CObjectManager::GetInstance();

	m_pKey1Comp = NULL;
	m_pKey2Comp = NULL;

	CObject* pTimerObj = CObjectManager::GetInstance()->CreateObject("TimerObject");
	CBitmapFont font(-1, 13, 15, 33);
	font.LoadFont("Resource\\BitmapFont.png", "Resource\\BitmapFont_Width.bin");

	m_pKey1Comp = m_pTM->CreateBitmapFontComp(pTimerObj, "1", font, 400, 10, 1.5f, scd::vector4(1.0f, 1.0f, 1.0f, 1.0f), false);
	m_pKey2Comp = m_pTM->CreateBitmapFontComp(pTimerObj, "2", font, 400, 10, 1.5f, scd::vector4(1.0f, 1.0f, 1.0f, 1.0f), false);


	// HUD Elements
	InitInventory();
	InitShoppingList();
	InitEnemyProgress();
	InitMiniMap();


	m_bHasHitThisUpdateBefore = false; // so the hud elements show up when gameplay enters

	int eAssociatedState = GAMEPLAY_STATE;

	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("GoalItemSpawned",
		(IComponent*)GetInstance(), GoalItemSpawnedCallback);

	pEM->RegisterEvent("GoalItemDespawned",
		(IComponent*)GetInstance(), GoalItemDespawnedCallback);

	pEM->RegisterEvent("GoalItemCollected",	(IComponent*)GetInstance(), 
		GoalItemCollectedCallback);
	pEM->RegisterEvent("GoalItemBlinking",	(IComponent*)GetInstance(), 
		GoalItemDespawning);



	pEM->RegisterEvent("PickupItemCollected",	(IComponent*)GetInstance(), 
		PickupItemCollectedCallback);

	string szEventName = "Update";
	szEventName += (char)eAssociatedState; // catch update for only gameplay state
	pEM->RegisterEvent(szEventName,	(IComponent*)GetInstance(), UpdateCallback);

	szEventName += MAIN_MENU_STATE;
	pEM->RegisterEvent(szEventName,	(IComponent*)GetInstance(), DisableImages);

	pEM->RegisterEvent("GoalItemInit", (IComponent*)GetInstance(), GoalItemInitCallback);
	pEM->RegisterEvent("ItemDropped", (IComponent*)GetInstance(), GoalItemLostCallback);
	pEM->RegisterEvent("IncrementRankScore", (IComponent*)GetInstance(), PlayerPickupItem);
	pEM->RegisterEvent("Player1ObjectCreation", (IComponent*)GetInstance(), PlayerLocation);
	pEM->RegisterEvent("HeldItemCollected", (IComponent*)GetInstance(),
		HeldItemCollectedCallBack);
	pEM->RegisterEvent("UseHeldItem1Hud", (IComponent*)GetInstance(), UseHeldItem1);
	pEM->RegisterEvent("UseHeldItem2Hud", (IComponent*)GetInstance(), UseHeldItem2);
	pEM->RegisterEvent("InitCheckoutLocation", (IComponent*)GetInstance(), InitCheckoutLocation);

	szEventName = "EnableObjects";
	szEventName += (char)eAssociatedState;
	pEM->RegisterEvent(szEventName, (IComponent*)GetInstance(), GameplayStateEntered);

	szEventName = "DisableObjects";
	szEventName += (char)eAssociatedState;
	pEM->RegisterEvent(szEventName, (IComponent*)GetInstance(), GameplayStateExited);

	szEventName = "InitObjects";
	szEventName += (char)eAssociatedState;
	pEM->RegisterEvent(szEventName, (IComponent*)GetInstance(), GameplayStateInit);

	szEventName = "ShutdownObjects";
	szEventName += (char)eAssociatedState;
	pEM->RegisterEvent(szEventName, (IComponent*)GetInstance(), GameplayStateExited);

}


void CHUDManager::InitShoppingList(void)
{
	// Load Sprite Sheet
	m_nGoalIconsTex = m_pTM->LoadTexture("Resource\\HUD\\FFP_2D_GoalItems_FIN.png");
	m_nGoalIconsDarkenedTex = m_pTM->LoadTexture("Resource\\HUD\\FFP_2D_GoalItems_Darkened_FIN.png");
	
	m_nShoppingListBackgroundTex[0] = m_pTM->LoadTexture("Resource\\HUD\\T_Biker_Shopping_List_D.png");
	m_nShoppingListBackgroundTex[1] = m_pTM->LoadTexture("Resource\\HUD\\T_Banditos_Shopping_List_D.png");
	m_nShoppingListBackgroundTex[2] = m_pTM->LoadTexture("Resource\\HUD\\T_Larpers_Shopping_List_D.png");
	m_nShoppingListBackgroundTex[3] = m_pTM->LoadTexture("Resource\\HUD\\T_Scientist_Shopping_List_D.png");
	m_nShoppingListBackgroundTex[4] = m_pTM->LoadTexture("Resource\\HUD\\T_Sasha_Shopping_List_D.png");
	m_nShoppingListBackgroundTex[5] = m_pTM->LoadTexture("Resource\\HUD\\T_Fitz_Shopping_List_D.png");
	m_nShoppingListBackgroundTex[6] = m_pTM->LoadTexture("Resource\\HUD\\T_Evelyn_Shopping_List_D.png");

	// Load texture for the icons for radar
	m_nRadarNodeTexture = m_nGoalIconsTex;//m_pTM->LoadTexture("Resource\\HUD\\FFP_2D_Goal_Locators_FIN.png");

	// Idxs
	m_nGoalIconTextureIndices[GORILLA_MILK] = 7; 
	m_nGoalIconTextureIndices[CAPTAIN_FLAKEY] = 0; 
	m_nGoalIconTextureIndices[AGENT_ORANGE] = 1; 
	m_nGoalIconTextureIndices[CAKE] = 5; 
	m_nGoalIconTextureIndices[PIZZA] = 3; 
	m_nGoalIconTextureIndices[CANNED_BEARS] = 4; 
	m_nGoalIconTextureIndices[CARROTS] = 2; 
	m_nGoalIconTextureIndices[SAMMY_SANDWICH] = 8; 
	m_nGoalIconTextureIndices[SQUID] = 6; 

	// Setup Sprite Data

	// Background list
	TSpriteData tListData;

	tListData.m_nTexture = m_nShoppingListBackgroundTex[0];
	tListData.m_nX = 27;
	tListData.m_nY = 230;
	tListData.m_nZ = 4;
	tListData.m_fScaleX = 1.3f;
	tListData.m_fScaleY = 1.3f;
	tListData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tListData.m_tRect.top    = 0;
	tListData.m_tRect.right  = 0;
	tListData.m_tRect.left   = 0;
	tListData.m_tRect.bottom = 0;
	tListData.m_fRotCenterX = 0.0f;
	tListData.m_fRotCenterY = 0.0f;
	tListData.m_fRot = 0.0f;

	// Get Inital Sprite Data
	CObject* pObj = m_pOM->CreateObject("ShoppingListBackground444"); // TODO: Clean this up?

	m_pShoppingListBackgroundSpriteComp = m_pTM->CreateSpriteComp(
		pObj, tListData, false);

	// Bottom
	m_tListIconData[7].m_nTexture = m_nGoalIconsDarkenedTex;
	m_tListIconData[7].m_nX = 120;
	m_tListIconData[7].m_nY = 670;
	m_tListIconData[7].m_nZ = 6;
	m_tListIconData[7].m_fScaleX = .4f;
	m_tListIconData[7].m_fScaleY = .4f;
	m_tListIconData[7].m_fRotCenterX = 0.0f;
	m_tListIconData[7].m_fRotCenterY = 0.0f;
	m_tListIconData[7].m_fRot = 0.0f;
	m_tListIconData[7].m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_tListIconData[7].m_tRect.top    = 0;
	m_tListIconData[7].m_tRect.right  = 0;
	m_tListIconData[7].m_tRect.left   = 0;
	m_tListIconData[7].m_tRect.bottom = 0;

	m_tListIconData[6] = m_tListIconData[7];
	m_tListIconData[6].m_nX += 54;

	m_tListIconData[5] = m_tListIconData[7];
	m_tListIconData[5].m_nX += 25;
	m_tListIconData[5].m_nY += 25;

	m_tListIconData[4] = m_tListIconData[7];
	m_tListIconData[4].m_nX += 74;
	m_tListIconData[4].m_nY += 25;

	// Top
	m_tListIconData[3] = m_tListIconData[7];
	m_tListIconData[3].m_nX = 65;
	m_tListIconData[3].m_nY = 547;

	m_tListIconData[2] = m_tListIconData[3];
	m_tListIconData[2].m_nX += 55;

	m_tListIconData[1] = m_tListIconData[3];
	m_tListIconData[1].m_nY += 44;

	m_tListIconData[0] = m_tListIconData[3];
	m_tListIconData[0].m_nX += 55;
	m_tListIconData[0].m_nY += 44;

	// Create the Icon Objs
	TSpriteData tSpriteData = {};
	for(unsigned int uIcon = 0; uIcon < 8; ++uIcon) // HACK: 8 should be NUM_ITEMS_TO_SPAWN or something like that
	{
		// Creat the object
		string szObjID = "Icon";
		szObjID += (char)uIcon;
		pObj = m_pOM->CreateObject(szObjID); // TODO: Clean this up?

		// Get Inital Sprite Data
		tSpriteData = m_tListIconData[uIcon];

		CSpriteComponent* pSC = m_pTM->CreateSpriteComp(
			pObj, tSpriteData, false);

		TShoppingListItem tItemInfo;
		tItemInfo.bSpawned = false;
		tItemInfo.nItemID = -1;
		tItemInfo.nPlayerCollected = -1;
		tItemInfo.pIconComponent = pSC;

		// Add it to the list
		m_tGoalItemsSpawnedPool.push_back(tItemInfo);
	}

	// create the checkmark stuff
	for(int i=0; i<MAX_GOAL_ITEMS; i++) // HACK: Make goalitems in level
	{
		// Creat the object
		string szObjID = "IconCheckmark";
		szObjID += (char)i;
		pObj = m_pOM->CreateObject(szObjID); // TODO: Clean this up?

		// Get Inital Sprite Data
		CSpriteComponent* pSC = m_pTM->CreateSpriteComp(
			pObj, tSpriteData, false);

		m_pCheckMarkSpriteComp[i] = pSC;

	}
}

void CHUDManager::InitEnemyProgress(void)
{
	// Load Sprite Sheet
	int nTexID = m_pTM->LoadTexture("Resource\\HUD\\Competition_Progress.png");
	int nCharSheetID = m_pTM->LoadTexture("Resource\\HUD\\Character_Sprite_Sheet.png");

	m_nRankNumberTextureIDs[0] = m_pTM->LoadTexture("Resource\\HUD\\RankPositions\\T_Position1_D.png");
	m_nRankNumberTextureIDs[1] = m_pTM->LoadTexture("Resource\\HUD\\RankPositions\\T_Position2_D.png");
	m_nRankNumberTextureIDs[2] = m_pTM->LoadTexture("Resource\\HUD\\RankPositions\\T_Position3_D.png");
	m_nRankNumberTextureIDs[3] = m_pTM->LoadTexture("Resource\\HUD\\RankPositions\\T_Position4_D.png");
	int nRankBGTex = m_pTM->LoadTexture("Resource\\HUD\\RankPositions\\T_PositionGlow_D.png");

	// Create the Object
	m_pEnemyProgress = m_pOM->CreateObject("EnemyProgressObj");

	// Get Inital Sprite Data
	TSpriteData tSpriteData;
	tSpriteData.m_nTexture = nTexID;
	tSpriteData.m_nX = ENEMY_PROGRESS_X;
	tSpriteData.m_nY = RANKS_Y_LOC;
	tSpriteData.m_nZ = 2;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	// Add Sprite Comp
	// Font
	CBitmapFont font(-1, 13, 15, 33);
	font.LoadFont("Resource\\BitmapFont.png", "Resource\\BitmapFont_Width.bin");

	int x, y;
	float scale;
	int nCharIdx = 0;
	for(int bit = 0; bit < 4; ++bit)
	{
		// Characters
		tSpriteData.m_nTexture = nCharSheetID;
		tSpriteData.m_nX = ENEMY_PROGRESS_X - (bit == 0 ? 10 : 0);
		tSpriteData.m_nY = bit * 64 + RANKS_Y_LOC; // + distance from top (so we can show first 2nd 3rd etc
		tSpriteData.m_nZ = 1;
		tSpriteData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
		tSpriteData.m_tRect = CellAlgo(nCharIdx, 2, 190, 114);

		m_tPlayerInfo[bit].nID = bit;

		// portrait
		CObject* pCharObj = m_pOM->CreateObject("Char" + bit);
		m_tPlayerInfo[bit].pSpriteComp = 
			m_pTM->CreateSpriteComp(pCharObj, tSpriteData, false);

		// border image
		CObject* pCharBorderObj = m_pOM->CreateObject("CharBorder" + bit);
		tSpriteData.m_tRect.left = tSpriteData.m_tRect.top = 0;
		tSpriteData.m_tRect.right = 217;
		tSpriteData.m_tRect.bottom = 129;
		tSpriteData.m_nTexture = nTexID;
		m_tPlayerInfo[bit].pBackgroundSpriteComp = 
			m_pTM->CreateSpriteComp(pCharBorderObj, tSpriteData, false);
		/**/

		// Rank Numbers picture
		tSpriteData.m_nTexture = m_nRankNumberTextureIDs[bit];
		tSpriteData.m_fScaleX = tSpriteData.m_fScaleY = 1.0f;
		tSpriteData.m_nZ = 1;
		tSpriteData.m_tRect.bottom = 0;
		tSpriteData.m_tRect.right = 0;
		tSpriteData.m_tRect.top = 0;
		tSpriteData.m_tRect.left = 0;

		CObject* pRankNumObj = m_pOM->CreateObject("RankNumberSpriteObj"+ bit);
		m_tPlayerInfo[bit].pRankNumberComponent = m_pTM->CreateSpriteComp(pRankNumObj, tSpriteData, false);

		tSpriteData.m_nTexture = nRankBGTex;
		tSpriteData.m_nY -= 25;
		tSpriteData.m_nX -= 20;
		tSpriteData.m_nZ -= 1;
		tSpriteData.m_fScaleX = tSpriteData.m_fScaleY = .75f;
		pRankNumObj = m_pOM->CreateObject("RankNumberBGSpriteObj" + bit);
		m_tPlayerInfo[bit].pRankNumberBackgroundComponent = m_pTM->CreateSpriteComp(pRankNumObj, tSpriteData, false);


		// Fonts
		x = 960;
		y = RANKS_Y_LOC + g_yRankPos[bit];
		scale = 1.5f;
		CObject* pBitObj = m_pOM->CreateObject("Score" + bit);
		m_tPlayerInfo[bit].pScoreFontComps = m_pTM->CreateBitmapFontComp(pBitObj, "0/8", font, x,
			y, scale, scd::vector4(0, 200, 0, 255), false);

		nCharIdx += 1;
	}
}

void CHUDManager::InitMiniMap(void)
{	


	// Radar Stuff  for goal indicator/////////////////////////////////////////
	TRadarNode tNode;
	tNode.bSpawned = false;
	tNode.nGoalItemType = -5; // -5 signifies goal icon
	tNode.vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// Set up sprite data for this
	TSpriteData tData;
	tData.m_nTexture = GetInstance()->m_nRadarNodeTexture;
	tData.m_nX = 0;
	tData.m_nY = 0;
	tData.m_nZ = 103;
	tData.m_fScaleX = 1.0f; // scale is changed in update
	tData.m_fScaleY = 1.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_fRot = 0.0f;
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect = GetInstance()->CellAlgo(CHECKOUT_FLAG_CELL_IDX, LIST_NUM_COL, LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

	string szObjName = "RadarNodeGoal";
	CObject* pRadarSpriteObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	tNode.pIconSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pRadarSpriteObj, tData, false);

	tData.m_nTexture = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\FFP_2D_CollectionArrow_FIN.png");

	szObjName = "RadarNodeBGGoal";
	CObject* pRadarBGSpriteObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	tNode.pBackgroundSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pRadarBGSpriteObj, tData, false);

	GetInstance()->m_lRadarNodes.push_back(tNode);

	////////////////////////////////////////////////////////////////

	// grab the player pointers
	CObject* pPlayer0 = CObjectManager::GetInstance()->GetObjectByName("Player0");
	CObject* pPlayer1 = CObjectManager::GetInstance()->GetObjectByName("Player1");
	CObject* pPlayer2 = CObjectManager::GetInstance()->GetObjectByName("Player2");
	CObject* pPlayer3 = CObjectManager::GetInstance()->GetObjectByName("Player3");

	// init the players objects for the radar node
	TRadarNode tRadarNode;
	tRadarNode.bSpawned = false;
	tRadarNode.nGoalItemType = -6;// Make sure this is unique per node making
	tRadarNode.vTargetPosition = pPlayer1->GetTransform()->GetWorldPosition();

	tData.m_nTexture = GetInstance()->m_nRadarNodeTexture;
	tData.m_tRect = GetInstance()->CellAlgo(CHECKOUT_FLAG_CELL_IDX+1, LIST_NUM_COL, LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

	szObjName = "RadarNodePlayer1";
	CObject* pOBJ = CObjectManager::GetInstance()->CreateObject(szObjName);
	tRadarNode.pIconSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pOBJ, tData, false);

	tData.m_nTexture = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\FFP_2D_CollectionArrow_FIN.png");
	tData.m_tRect.bottom = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 0;
	tData.m_tRect.top = 0;

	szObjName = "RadarNodeBGPlayer1";
	pOBJ = CObjectManager::GetInstance()->CreateObject(szObjName);
	tRadarNode.pBackgroundSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pOBJ, tData, false);

	TPlayerRadarNodeInfo tP1PlayerInfo;
	tP1PlayerInfo.pPlayer = pPlayer1;
	tP1PlayerInfo.nCorrespondingGoalItemType = tRadarNode.nGoalItemType;

	GetInstance()->m_tPlayersGoingToCheckout.push_back(tP1PlayerInfo);
	GetInstance()->m_lRadarNodes.push_back(tRadarNode);

	////////////

	tRadarNode.nGoalItemType = -7;// Make sure this is unique per node making
	tRadarNode.vTargetPosition = pPlayer2->GetTransform()->GetWorldPosition();

	tData.m_nTexture = GetInstance()->m_nRadarNodeTexture;
	tData.m_tRect = GetInstance()->CellAlgo(CHECKOUT_FLAG_CELL_IDX+1, LIST_NUM_COL, LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

	szObjName = "RadarNodePlayer2";
	pOBJ = CObjectManager::GetInstance()->CreateObject(szObjName);
	tRadarNode.pIconSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pOBJ, tData, false);

	tData.m_nTexture = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\FFP_2D_CollectionArrow_FIN.png");
	tData.m_tRect.bottom = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 0;
	tData.m_tRect.top = 0;

	szObjName = "RadarNodeBGPlayer2";
	pOBJ = CObjectManager::GetInstance()->CreateObject(szObjName);
	tRadarNode.pBackgroundSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pOBJ, tData, false);

	tP1PlayerInfo.pPlayer = pPlayer2;
	tP1PlayerInfo.nCorrespondingGoalItemType = tRadarNode.nGoalItemType;

	GetInstance()->m_tPlayersGoingToCheckout.push_back(tP1PlayerInfo);
	GetInstance()->m_lRadarNodes.push_back(tRadarNode);

	/////////////////

	tRadarNode.nGoalItemType = -8; // Make sure this is unique per node making
	tRadarNode.vTargetPosition = pPlayer3->GetTransform()->GetWorldPosition();

	tData.m_nTexture = GetInstance()->m_nRadarNodeTexture;
	tData.m_tRect = GetInstance()->CellAlgo(CHECKOUT_FLAG_CELL_IDX+1, LIST_NUM_COL, LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

	szObjName = "RadarNodePlayer3";
	pOBJ = CObjectManager::GetInstance()->CreateObject(szObjName);
	tRadarNode.pIconSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pOBJ, tData, false);

	tData.m_nTexture = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\FFP_2D_CollectionArrow_FIN.png");
	tData.m_tRect.bottom = 0;
	tData.m_tRect.left = 0;
	tData.m_tRect.right = 0;
	tData.m_tRect.top = 0;

	szObjName = "RadarNodeBGPlayer3";
	pOBJ = CObjectManager::GetInstance()->CreateObject(szObjName);
	tRadarNode.pBackgroundSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pOBJ, tData, false);

	tP1PlayerInfo.pPlayer = pPlayer3;
	tP1PlayerInfo.nCorrespondingGoalItemType = tRadarNode.nGoalItemType;

	GetInstance()->m_tPlayersGoingToCheckout.push_back(tP1PlayerInfo);
	GetInstance()->m_lRadarNodes.push_back(tRadarNode);


}


void CHUDManager::InitInventory(void)
{
	// Load Sprite Sheet
	// Held Item locations
	int nInventory = m_pTM->LoadTexture("Resource\\HUD\\Attack_Item_HUD_Left.png");
	int nbutton = m_pTM->LoadTexture("Resource\\HUD\\Attack_Item_HUD_Button.png");
	// Energy Drink icon
	m_nEnergyDrinkID = m_pTM->LoadTexture("Resource\\HUD\\FFP_2D_HeldItem_FIN.png");

	// Create the Object
	CObject* pInventory = m_pOM->CreateObject("HUDInventory");
	CObject* pHeldItem1 = m_pOM->CreateObject("HUDHeldItem1");
	CObject* pHeldItem2 = m_pOM->CreateObject("HUDHeldItem2");
	CObject* pbutton = m_pOM->CreateObject("Button");

	int nScreenPosX = 30;
	int nScreenPosY = 30;

	// Get Inital Sprite Data
	TSpriteData tInventory;
	ZeroMemory(&tInventory, sizeof(tInventory));
	tInventory.m_nTexture = nInventory;
	tInventory.m_nX = nScreenPosX;
	tInventory.m_nY = nScreenPosY;
	tInventory.m_nZ = 0;
	tInventory.m_fScaleX = 1.0f;
	tInventory.m_fScaleY = 1.0f;
	tInventory.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);

	TSpriteData tItem1 = tInventory;
	tItem1.m_nZ = 2;
	tItem1.m_fScaleX = 0.9f;
	tItem1.m_fScaleY = 0.9f;
	tItem1.m_nX += 10;
	tItem1.m_nY += 20;
	TSpriteData tItem2 = tItem1;
	tItem2.m_nX += 110;

	// Add Sprite Comp
	m_pInventory = m_pTM->CreateSpriteComp(pInventory, tInventory, false);
	m_pHeldItem1 = m_pTM->CreateSpriteComp(pHeldItem1, tItem1, false);
	m_pHeldItem2 = m_pTM->CreateSpriteComp(pHeldItem2, tItem2, false);

	TSpriteData tbutton1;
	tbutton1.m_nTexture = nbutton;
	tbutton1.m_nX = nScreenPosX + 13;
	tbutton1.m_nY = nScreenPosY + 82;
	tbutton1.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tbutton1.m_fScaleX = 0.35f;
	tbutton1.m_fScaleY = 0.35f;
	tbutton1.m_fRot = 0.0f;
	tbutton1.m_fRotCenterX = 0.0f;
	tbutton1.m_fRotCenterY = 0.0f;
	tbutton1.m_nZ = 2;
	m_pButtonItem1 = m_pTM->CreateSpriteComp(pbutton, tbutton1, false);
	TSpriteData tbutton2 = tbutton1;
	tbutton2.m_nX += 107;
	tbutton2.m_nZ = 3;
	m_pButtonItem2 = m_pTM->CreateSpriteComp(pbutton, tbutton2, false);

	m_pKey1Comp->SetX(nScreenPosX + 33);
	m_pKey1Comp->SetY(nScreenPosY + 95);
	m_pKey1Comp->SetWord("Z");

	m_pKey2Comp->SetX(nScreenPosX + 136);
	m_pKey2Comp->SetY(nScreenPosY + 95);
	m_pKey2Comp->SetWord("X");
}


void CHUDManager::Render(void)
{
}

// Goal Item Spawn Callback
void CHUDManager::GoalItemSpawned(TGoalItemEvent* pcObjEvent)
{
	// Update the List
	int nSpawnedType = CSpawningManager::GetInstance()->GetGoalItemType(pcObjEvent->m_pcGoalItem->GetParent());

	bool bAlreadyCollected = false; // a bool that is turned true if we've already collected this item.

	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsSpawnedPool.begin();

	// swap what we spawned into the new slot
	while(goalItemIter != m_tGoalItemsSpawnedPool.end())
	{
		// if the one we're on is the item that spawned
		if(nSpawnedType == goalItemIter->nItemID)
		{
			goalItemIter->bSpawned = true;
			bAlreadyCollected = (goalItemIter->nPlayerCollected == 0);
			break; // move the item that spawned. Stop looping to prevent double swaps
		}

		// Inc Pos
		goalItemIter++;
	}/**/


	// Make the radar node active
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = m_lRadarNodes.begin();

	while(nodeIter != m_lRadarNodes.end())
	{
		// if it's spawned AND we need to pick it up (aka we haven't collected; we dont have a checkmark on it)
		if(nodeIter->nGoalItemType == nSpawnedType && !bAlreadyCollected)
		{
			nodeIter->bSpawned = true;
			nodeIter->bDespawning = false;
			nodeIter->vTargetPosition = pcObjEvent->m_pcGoalItem->GetParent()->GetTransform()->GetWorldPosition();
			break;
			// update will handle the placement code
		}
		nodeIter++;
	}

	//GetInstance()->UpdateShoppingList();

}

void CHUDManager::GoalItemSpawnedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	GetInstance()->GoalItemSpawned((TGoalItemEvent*)pEvent->GetData());
}

void CHUDManager::GoalItemDespawned(EGoalItemType eType)
{
	// Update the List
	int nSpawnedType = eType;

	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsSpawnedPool.begin();

	// swap what we spawned into the new slot
	while(goalItemIter != m_tGoalItemsSpawnedPool.end())
	{
		// if the one we're on is the item that despawned
		if(nSpawnedType == goalItemIter->nItemID)
		{
			goalItemIter->bSpawned = false;
			break; // move the item that spawned. Stop looping to prevent double swaps
		}

		// Inc Pos
		goalItemIter++;
	}/**/


	// Make the radar node active
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = m_lRadarNodes.begin();

	while(nodeIter != m_lRadarNodes.end())
	{
		if(nodeIter->nGoalItemType == nSpawnedType)
		{
			nodeIter->bSpawned = false;
			nodeIter->bDespawning = false;
			break;
		}
		nodeIter++;

	}

	//GetInstance()->UpdateShoppingList();

}

void CHUDManager::GoalItemDespawnedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	TGoalItemEvent* pcObjEvent = (TGoalItemEvent*)pEvent->GetData();

	EGoalItemType eType = CSpawningManager::GetInstance()->GetGoalItemType(pcObjEvent->m_pcGoalItem->GetParent());

	GetInstance()->GoalItemDespawned(eType);
}

void CHUDManager::HeldItemCollectedCallBack(IEvent* pEvent, IComponent*)
{
	THeldItemCollected* tHeldItemCollected = (THeldItemCollected*)pEvent->GetData();

	CHUDManager::GetInstance()->HeldItemCollected(tHeldItemCollected);
}

void CHUDManager::HeldItemCollected(THeldItemCollected* pHeldItem)
{
	if(m_pPlayerObject->GetID() != 
		pHeldItem->m_pcCollector->GetID())
	{
		return;
	}

	EHeldItemType eType = CSpawningManager::GetInstance()->GetHeldItemType(
		pHeldItem->m_pcHeldItem);

	RECT tRect;

	switch(eType)
	{
	case ENERGY_DRINK:
		{
			tRect = CellAlgo(2, 2, 128, 128);
			break;
		}
	case TURKEY:
		{
			tRect = CellAlgo(0, 2, 128, 128);
			break;
		}
	case BANANA:
		{
			tRect = CellAlgo(4, 2, 128, 128);
			break;
		}
	case PEANUT_BUTTER:
		{
			tRect = CellAlgo(5, 2, 128, 128);
			break;
		}
	case PIE:
		{
			tRect = CellAlgo(6, 2, 128, 128);
			break;
		}
	case SOUP:
		{
			tRect = CellAlgo(1, 2, 128, 128);
			break;
		}
	case DONUT:
		{
			tRect = CellAlgo(3, 2, 128, 128);
			break;
		}
	case JAM:
		{
			tRect = CellAlgo(7, 2, 128, 128);
			break;
		}
	default:
		{
			// Error...
			tRect.bottom = tRect.top = tRect.right = tRect.left = 0;
			eType = NO_HELD_ITEM;
		}
	};

	if(m_bShowHeld1 == false)
	{
		TSpriteData tSpriteData = m_pHeldItem1->GetSpriteData();

		tSpriteData.m_nTexture = m_nEnergyDrinkID; // texture for all of them
		tSpriteData.m_tRect = tRect;
		tSpriteData.m_nZ = 2;

		m_pHeldItem1->SetSpriteData(tSpriteData);
		m_pHeldItem1->SetActive(true);
		m_bShowHeld1 = true;

		SendIEvent("HeldItemCollectedEffect", (IComponent*)m_pHeldItem1, NULL, PRIORITY_NORMAL);

	}
	else if(m_bShowHeld2 == false)
	{
		TSpriteData tSpriteData = m_pHeldItem2->GetSpriteData();

		tSpriteData.m_nTexture = m_nEnergyDrinkID;
		tSpriteData.m_tRect = tRect;
		tSpriteData.m_nZ = 2;

		m_pHeldItem2->SetSpriteData(tSpriteData);
		m_pHeldItem2->SetActive(true);
		m_bShowHeld2 = true;

		SendIEvent("HeldItemCollectedEffect", (IComponent*)m_pHeldItem2, NULL, PRIORITY_NORMAL);

	}
}


void CHUDManager::HeldItemIDCollected(EHeldItemType nHeldItem)
{
	EHeldItemType eType = nHeldItem;

	RECT tRect = {};

	switch(eType)
	{
	case ENERGY_DRINK:
		{
			tRect = CellAlgo(2, 2, 128, 128);
			break;
		}
	case TURKEY:
		{
			tRect = CellAlgo(0, 2, 128, 128);
			break;
		}
	case BANANA:
		{
			tRect = CellAlgo(4, 2, 128, 128);
			break;
		}
	case PEANUT_BUTTER:
		{
			tRect = CellAlgo(5, 2, 128, 128);
			break;
		}
	case PIE:
		{
			tRect = CellAlgo(6, 2, 128, 128);
			break;
		}
	case SOUP:
		{
			tRect = CellAlgo(1, 2, 128, 128);
			break;
		}
	case DONUT:
		{
			tRect = CellAlgo(3, 2, 128, 128);
			break;
		}
	case JAM:
		{
			tRect = CellAlgo(7, 2, 128, 128);
			break;
		}
	default:
		{
			// Error...
			eType = NO_HELD_ITEM;
		}
	};

	if(m_pHeldItem1->IsActive() == false)
	{
		TSpriteData tSpriteData = m_pHeldItem1->GetSpriteData();

		tSpriteData.m_nTexture = m_nEnergyDrinkID; // texture for all of them
		tSpriteData.m_tRect = tRect;
		tSpriteData.m_nZ = 2;

		m_pHeldItem1->SetSpriteData(tSpriteData);
		m_pHeldItem1->SetActive(true);
		m_bShowHeld1 = true;
	}
	else if(m_pHeldItem2->IsActive() == false)
	{
		TSpriteData tSpriteData = m_pHeldItem2->GetSpriteData();

		tSpriteData.m_nTexture = m_nEnergyDrinkID;
		tSpriteData.m_tRect = tRect;
		tSpriteData.m_nZ = 2;

		m_pHeldItem2->SetSpriteData(tSpriteData);
		m_pHeldItem2->SetActive(true);
		m_bShowHeld2 = true;
	}
}

void CHUDManager::UseHeldItem1(IEvent* iEvent, IComponent*)
{
	TInputEvent* tEvent = (TInputEvent*)iEvent->GetData();

	// if we're the hud player (player 0)
	CHUDManager::GetInstance()->PlayUseItemSound(tEvent->m_pPlayer);
	if(tEvent->m_pPlayer == CObjectManager::GetInstance()->GetObjectByName("Player0"))
	{
		CHUDManager::GetInstance()->UseHeldItem(1);
	}
}

void CHUDManager::UseHeldItem2(IEvent* iEvent, IComponent*)
{
	TInputEvent* tEvent = (TInputEvent*)iEvent->GetData();

	CHUDManager::GetInstance()->PlayUseItemSound(tEvent->m_pPlayer);
	// if we're the hud player (player 0)
	if(tEvent->m_pPlayer == CObjectManager::GetInstance()->GetObjectByName("Player0"))
	{
		CHUDManager::GetInstance()->UseHeldItem(2);
	}
}
void CHUDManager::UseHeldItem(int nIndex)
{
	if(nIndex == 1)
	{
		//m_pHeldItem1->SetActive(false);
		SendIEvent("HeldItemUsedEffect", (IComponent*)m_pHeldItem1, NULL, PRIORITY_NORMAL);
		m_bShowHeld1 = false;
	}
	else
	{
		//m_pHeldItem2->SetActive(false);
		SendIEvent("HeldItemUsedEffect", (IComponent*)m_pHeldItem2, NULL, PRIORITY_NORMAL);
		m_bShowHeld2 = false;
	}
}

void CHUDManager::StartDespawnTimer(EGoalItemType /*eType*/)
{
	// Find a spot
	static bool bFirst = true;
	//int uIdx = (bFirst) ? 0 : 1;

	// set a var to maybe blink or something if necessary?

	// toggle Pos
	bFirst = !bFirst;
}

void CHUDManager::StartDespawnTimerCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	TGoalItemEvent* pcObjEvent = (TGoalItemEvent*)pEvent->GetData();

	EGoalItemType eType = pcObjEvent->m_eGoalItemType;

	GetInstance()->StartDespawnTimer(eType);
}

void CHUDManager::GoalItemCollected(TGoalItemCollectedEvent* pcObjEvent)
{	
	int playerID = GetPlayerNum(pcObjEvent->m_pcCollector);

	EGoalItemType nType = CSpawningManager::GetInstance()->GetGoalItemType(pcObjEvent->m_pcGoalItem);

	// Find the item in the shopping list
	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsSpawnedPool.begin();
	while(goalItemIter != m_tGoalItemsSpawnedPool.end())
	{
		// if we found the item that got picked up AND we haven't collected it
		if(goalItemIter->nItemID == nType)
		{
			// set the player who collected it to the correct player
			if(goalItemIter->nPlayerCollected != 0)
			{
				// this check makes sure that if the hud player has it collected, it stays collected
				// (if the player collected was, say, p2, it would show a sold out sign, but not now
				goalItemIter->nPlayerCollected = playerID;
			}

			// say it despawned regardless
			goalItemIter->bSpawned = false;
			break; // only one in the list needs to be updated (only one exists, even if it didnt...)
		}

		goalItemIter++;
	}

	// Make the radar node active
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = m_lRadarNodes.begin();

	while(nodeIter != m_lRadarNodes.end())
	{
		if(nodeIter->nGoalItemType == nType)
		{
			nodeIter->bSpawned = false;
			nodeIter->vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			break;
			// update will handle the placement code
		}
		nodeIter++;
	}

	//GetInstance()->UpdateShoppingList();
}

void CHUDManager::GoalItemCollectedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	TGoalItemCollectedEvent* pcObjEvent = (TGoalItemCollectedEvent*)pEvent->GetData();
	GetInstance()->GoalItemCollected(pcObjEvent);
}

void CHUDManager::PickupItemCollectedCallback(IEvent* pEvent, IComponent* /*pComp*/)
{	
	TPickupItemCollectedEvent* pcObjEvent = (TPickupItemCollectedEvent*)pEvent->GetData();
	int playerID = GetInstance()->GetPlayerNum(pcObjEvent->m_pcCollector);

	EGoalItemType nType = pcObjEvent->m_eItemType;

	// Find the item in the shopping list
	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = GetInstance()->m_tGoalItemsSpawnedPool.begin();
	while(goalItemIter != GetInstance()->m_tGoalItemsSpawnedPool.end())
	{
		// if we found the item that got picked up AND we haven't collected it
		if(goalItemIter->nItemID == nType && goalItemIter->nPlayerCollected != 0)
		{
			// set the player who collected it to the correct player
			goalItemIter->nPlayerCollected = playerID;
			break; // only one in the list needs to be updated (only one exists, even if it didnt...)
		}

		goalItemIter++;
	}

	// Make the radar node inactive
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = GetInstance()->m_lRadarNodes.begin();

	while(nodeIter != GetInstance()->m_lRadarNodes.end())
	{
		if(nodeIter->nGoalItemType == nType)
		{
			nodeIter->bSpawned = false;
			nodeIter->vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			break;
			// update will handle the placement code
	}
		nodeIter++;
	}


	//GetInstance()->UpdateShoppingList();

}


void CHUDManager::Update(void)
{
	UpdateShoppingList();



	// Sort the ranks/scoreboard
	for(int rank = 0; rank < 3; ++rank)
	{
		for(int rank2 = rank+1; rank2 < 4; ++rank2)
		{
			if(m_tPlayerInfo[rank].nScore < m_tPlayerInfo[rank2].nScore)
			{	
				if (rank == 0)
				{
					PlayLead(m_tPlayerInfo[rank2].nID);
				}
				swap(m_tPlayerInfo[rank], m_tPlayerInfo[rank2]);
			}
		}
	}	

	int nYPos = RANKS_Y_LOC;

	float fDefaultScale = .5f;
	float fPlayerScale = .75f;

	for(int player = 0; player < 4; ++player)
	{	
		// Portrait data
		TSpriteData tSprite = m_tPlayerInfo[player].pSpriteComp->GetSpriteData();
		tSprite.m_nY = nYPos;
		tSprite.m_nZ = 5;
		tSprite.m_fScaleX = m_tPlayerInfo[player].nID == 0 ? fPlayerScale : fDefaultScale;
		tSprite.m_fScaleY = tSprite.m_fScaleX;

		int nYSize = tSprite.m_tRect.bottom - tSprite.m_tRect.top;
		int nXSize = tSprite.m_tRect.right - tSprite.m_tRect.left;

		if(m_tPlayerInfo[player].nID == 0)
		{
			tSprite.m_nX = (int)(ENEMY_PROGRESS_X - nXSize*tSprite.m_fScaleX + nXSize*fDefaultScale); // *.5f to make it the "normal" size to make the right side even
		}
		else // is not hud's player
		{
			tSprite.m_nX = ENEMY_PROGRESS_X;
		}

		m_tPlayerInfo[player].pSpriteComp->SetSpriteData(tSprite);
		m_tPlayerInfo[player].pSpriteComp->SetActive(true);

		// take care of border stuff
		TSpriteData tBorderData = m_tPlayerInfo[player].pBackgroundSpriteComp->GetSpriteData();
		tBorderData.m_nX = tSprite.m_nX - 4;
		tBorderData.m_nY = tSprite.m_nY - 4;
		tBorderData.m_nZ = 5 + 1;
		tBorderData.m_fScaleX = m_tPlayerInfo[player].nID == 0 ? fPlayerScale : fDefaultScale;
		tBorderData.m_fScaleY = tBorderData.m_fScaleX;
		m_tPlayerInfo[player].pBackgroundSpriteComp->SetSpriteData(tBorderData);
		m_tPlayerInfo[player].pBackgroundSpriteComp->SetActive(true);


		// rank images
		TSpriteData tRankNumData = GetInstance()->m_tPlayerInfo[player].pRankNumberComponent->GetSpriteData();
		tRankNumData.m_fScaleX = tRankNumData.m_fScaleY = m_tPlayerInfo[player].nID == 0 ? fPlayerScale*.75f : fDefaultScale*.75f;
		tRankNumData.m_nX = int(tBorderData.m_nX - 100 * tRankNumData.m_fScaleX);
		tRankNumData.m_nY = int(tBorderData.m_nY + 30 * tRankNumData.m_fScaleY);
		tRankNumData.m_nZ = 5-1;
		tRankNumData.m_nTexture = m_nRankNumberTextureIDs[player];
		GetInstance()->m_tPlayerInfo[player].pRankNumberComponent->SetSpriteData(tRankNumData);

		TSpriteData tRankNumBGData = GetInstance()->m_tPlayerInfo[player].pRankNumberBackgroundComponent->GetSpriteData();
		tRankNumBGData.m_fScaleX = tRankNumBGData.m_fScaleY = m_tPlayerInfo[player].nID == 0 ? fPlayerScale*.75f : fDefaultScale*.75f;
		tRankNumBGData.m_nX = int(tBorderData.m_nX - 100 * tRankNumBGData.m_fScaleX);
		tRankNumBGData.m_nY = int(tBorderData.m_nY - 10 * tRankNumBGData.m_fScaleY);
		tRankNumBGData.m_nZ = tRankNumData.m_nZ-1;
		GetInstance()->m_tPlayerInfo[player].pRankNumberBackgroundComponent->SetSpriteData(tRankNumBGData);

		// Amounts of items
		char score[4] = { (char)m_tPlayerInfo[player].nScore + '0', '/', '8' };
		score[3] = '\0'; // Null terminate this sucker

		m_tPlayerInfo[player].pScoreFontComps->SetWord(score);
		m_tPlayerInfo[player].pScoreFontComps->SetY((int)(tSprite.m_nY + 5*tSprite.m_fScaleY));
		m_tPlayerInfo[player].pScoreFontComps->SetX((int)(ENEMY_PROGRESS_X + nXSize*fDefaultScale - 55)); // -constant (the width of "x/8") to get that right justified to the right of the portrait
		m_tPlayerInfo[player].pScoreFontComps->SetActive(true);



		// increment our y value
		nYPos += (int)(nYSize*tSprite.m_fScaleY + 10); // +constant for space between portraits



	}

	// Radar node stuff
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = m_lRadarNodes.begin();

	while(nodeIter != m_lRadarNodes.end())
	{
		// Update the "checkout impending" radar node positions (radar node player checkout goal item types are -6, -7, -8...)
		if(nodeIter->nGoalItemType <= -6)
		{
			list<TPlayerRadarNodeInfo, scd::allocator<TPlayerRadarNodeInfo>>::iterator nodeIterToFind;
			nodeIterToFind = m_tPlayersGoingToCheckout.begin();

			// look through all the player radar nodes
			while(nodeIterToFind != m_tPlayersGoingToCheckout.end())
			{
				// if the corresponding goal item type is the same as the one we're searching for
				if(nodeIterToFind->nCorrespondingGoalItemType == nodeIter->nGoalItemType)
				{
					// update the position using the object stored
					nodeIter->vTargetPosition = nodeIterToFind->pPlayer->GetTransform()->GetWorldPosition();
				}

				nodeIterToFind++;
			}
		}

		//
		if(nodeIter->bSpawned)
		{
			TSpriteData tData = nodeIter->pIconSpriteComponent->GetSpriteData();

			// scale the image to the size we want
			tData.m_fScaleX = tData.m_fScaleY = .5f; 

			D3DXVECTOR2 toObj;
			CObject* pPlayerObj = CObjectManager::GetInstance()->GetObjectByName("Player0");
			D3DXVECTOR3 fwdVec;
			fwdVec.x = pPlayerObj->GetTransform()->GetLocalMatrix()._31;
			fwdVec.y = pPlayerObj->GetTransform()->GetLocalMatrix()._32;
			fwdVec.z = pPlayerObj->GetTransform()->GetLocalMatrix()._33;

			// normalize (in case it's not?) for angle checks
			D3DXVec3Normalize(&fwdVec, &fwdVec);


			D3DXVECTOR3 toVec = nodeIter->vTargetPosition - pPlayerObj->GetTransform()->GetWorldPosition();

			// get length (distance to it) for checks that require distance
			float toVecLengthSq = D3DXVec3Length(&toVec);

			// normalize for angle between checks
			D3DXVec3Normalize(&toVec, &toVec);


			float fFwdDotToItem = D3DXVec3Dot(&fwdVec, &toVec);

			// generate a right 
			D3DXVECTOR3 vRightDirection;
			D3DXVECTOR3 vDownDirection(0.0f, -1.0f, 0.0f);
			D3DXVec3Cross(&vRightDirection, &vDownDirection, &fwdVec); // [0] is fwd heading
			D3DXVec3Normalize(&vRightDirection, &vRightDirection);

			float fRightDotToItem = D3DXVec3Dot(&vRightDirection, &toVec);

			int nScreenSizeX = 1024;
			int nScreenSizeY = 768;

			int nBorderSizeX = 5;
			int nBorderSizeY = 5;

			int nItemPathRectSizeX = nScreenSizeX - nBorderSizeX;
			int nItemPathRectSizeY = nScreenSizeY - nBorderSizeY;

			bool bCloseAndFacingItem = fFwdDotToItem >= 0.0f && toVecLengthSq < 7.0f;
			float zeroToOneScaledLength = ((toVecLengthSq)/7.0f); // proprotion out of starting distance times pi/2 to get 0-pi/2

			// multiply the x by the distance so if we're close, it stays much in front of us, but only if it *is* in front of us
			toObj.x = fRightDotToItem * ( bCloseAndFacingItem ? zeroToOneScaledLength*zeroToOneScaledLength : 1.0f); // in front? scale : dont
			toObj.y = fFwdDotToItem * -1.0f; // negate because screen coords are "top 0"
			D3DXVec2Normalize(&toObj, &toObj);
			toObj.x *= (nScreenSizeX)*.9f; // scale to make it further out from the center (*costant to make it slightly smaller than the screen size so movement isn't really jerky but it still doesn't snap at the corners)
			toObj.y *= (nScreenSizeY)*.9f; // scale to make it further out from the center

			// offset to the center of the screen (screensize/2)
			toObj.x += (nScreenSizeX*.5f); // - width/2
			toObj.y += (nScreenSizeY*.5f); // - height/2;

			// offset to the center of the image
			toObj.x -= (tData.m_tRect.right - tData.m_tRect.left)*.5f * tData.m_fScaleX; // *scale so any size changes made to the sprite are affecting the offset amt
			toObj.y -= (tData.m_tRect.bottom - tData.m_tRect.top)*.5f * tData.m_fScaleY;


			// clamp things to the square edges
			// if frontnormed dot tovecnormed > .5 (45 degrees from front either size) (right and left would be closer to 0)
			if(fFwdDotToItem >= .5f)
			{
				// it's in front so clamp the y to the top part of the screen
				toObj.y = (float)(nBorderSizeY);
			}
			// else if (same number as up there) < -.5 (45 degrees from beind)
			else if(fFwdDotToItem < -.5f)
			{
				// it's behind us, clamp to the bottom wall
				toObj.y = (float)(nItemPathRectSizeY); // HACK: hardcoded screen size, also calculate offset so the bot is at "pos"
			}

			// else if(rightnormed dot tovecnormed > .5 (45 degrees from right) (front and back would be closer to 0)
			if(fRightDotToItem >= .5f)
			{
				// it's to the right, clamp to right wall
				toObj.x = (float)(nItemPathRectSizeX); // offset by the image size so "pos" is on the top right
			}
			// else it's on the left by default
			else if(fRightDotToItem < -.5f)
			{
				// clamp to the left wall of screen
				toObj.x = (float)(nBorderSizeX); // no need to clamp here because it's already on the left side
			}


			// offset things that are nearly off the right and bottom to the right and bottom so they don't fall off the screen
			// if we're closer to the edge than our width
			if(toObj.x > nItemPathRectSizeX  - (tData.m_tRect.right - tData.m_tRect.left) * tData.m_fScaleX)
			{
				toObj.x = nItemPathRectSizeX - (tData.m_tRect.right - tData.m_tRect.left) * tData.m_fScaleX; // make us right on the edge
			}
			// do the same with the y axis
			if(toObj.y > nItemPathRectSizeY - (tData.m_tRect.bottom - tData.m_tRect.top) * tData.m_fScaleY)
			{
				toObj.y = nItemPathRectSizeY - (tData.m_tRect.bottom - tData.m_tRect.top) * tData.m_fScaleY; // make us right on the edge
			}

			// set the center point and the circular offset
			tData.m_nX = (int)(toObj.x);
			tData.m_nY = (int)(toObj.y);

			TSpriteData tBGData = nodeIter->pBackgroundSpriteComponent->GetSpriteData();
			tBGData.m_fRotCenterX = 48 * tData.m_fScaleX*2.0f; // hard coded center of rot from looking at image
			tBGData.m_fRotCenterY = 50 * tData.m_fScaleY*2.0f; // *2 because we halved the scale of the icons from when this was first calculated

			tBGData.m_nX = (int)(toObj.x); // offset the bg so the icon is right on it
			tBGData.m_nY = (int)(toObj.y);
			tBGData.m_fScaleX = tBGData.m_fScaleY = 2.0f *.66f * tData.m_fScaleX;

			static float fRotdd = 0.0f;
			fRotdd  = acos(fFwdDotToItem); // no /len*len because they're unit vectors doing the dots

			// if it's on one side
			if(fRightDotToItem < 0.0f)
			{
				// negate it
				fRotdd *= -1.0f; 
			}

			// if it's right in front of us (same criteria for if we start scaling the offset from center)
			if(bCloseAndFacingItem)
			{
				// negate it again so it points TO the object
				fRotdd *= -1.0f; 
			}
			fRotdd += 3.14159f; // add half a revolution so it points the right way since the image points down at 0 rot

			/**/
			tBGData.m_fRot = fRotdd;
			tBGData.m_tRect.bottom = 0;
			tBGData.m_tRect.top = 0;
			tBGData.m_tRect.right = 0;
			tBGData.m_tRect.left = 0;

			// handle flashing transparency thing if it's despawning
			if(nodeIter->bDespawning)
			{
				tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, .9f + sin(GetInstance()->m_fTimerAccumulator*20.0f)); // minval + sin(speed)
			}
			else
			{
				tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f); // else make it fully visible 
			}


			nodeIter->pIconSpriteComponent->SetSpriteData(tData);
			nodeIter->pIconSpriteComponent->SetActive(true);
			nodeIter->pBackgroundSpriteComponent->SetSpriteData(tBGData);
			nodeIter->pBackgroundSpriteComponent->SetActive(true);
		}
		else
		{
			nodeIter->pIconSpriteComponent->SetActive(false);
			nodeIter->pBackgroundSpriteComponent->SetActive(false);
		}

		nodeIter++;				
	}


}

void CHUDManager::UpdateCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get values from the event
	TUpdateStateEvent* tEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDt = tEvent->m_fDeltaTime;

	GetInstance()->m_fTimerAccumulator += fDt;

	GetInstance()->Update();

}

void CHUDManager::GoalItemInitCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	TGoalItemEvent* pcObjEvent = (TGoalItemEvent*)pEvent->GetData();

	D3DXVECTOR3 objPos = pcObjEvent->m_pcGoalItem->GetParent()->GetTransform()->GetWorldPosition();

	int nSpawnedItemID = pcObjEvent->m_eGoalItemType;

	// Shopping list stuff

	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator iter;
	iter = GetInstance()->m_tGoalItemsSpawnedPool.begin();

	// for each slot in the list
	while(iter != GetInstance()->m_tGoalItemsSpawnedPool.end())
	{
		// if that slot doesn't have an item yet
		if(iter->nItemID == -1)
		{
			// set up its data
			iter->bSpawned = false;
			iter->nItemID = nSpawnedItemID;
			iter->nPlayerCollected = -1;
			//iter->pIconComponent->SetActive(true);

			// init the UVs for this particular item id
			RECT tUVRect(GetInstance()->CellAlgo(
				GetInstance()->m_nGoalIconTextureIndices[nSpawnedItemID], LIST_NUM_COL, LIST_CELL_WIDTH, LIST_CELL_HEIGHT));		
			iter->pIconComponent->GetSpriteData().m_tRect = tUVRect;

			break; // get out of here 'cause we added it
		}
		iter++;
	}



	// Radar Stuff /////////////////////////////////////////
	TRadarNode tNode;
	tNode.bSpawned = false;
	tNode.nGoalItemType = nSpawnedItemID;
	tNode.vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// Set up sprite data for this
	TSpriteData tData;
	tData.m_nTexture = GetInstance()->m_nRadarNodeTexture;
	tData.m_nX = 0;
	tData.m_nY = 0;
	tData.m_nZ = 103;
	tData.m_fScaleX = 1.0f; // scale is changed in update
	tData.m_fScaleY = 1.0f;
	tData.m_fRotCenterX = 0.0f;
	tData.m_fRotCenterY = 0.0f;
	tData.m_fRot = 0.0f;
	tData.m_dwColor = scd::vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tData.m_tRect = GetInstance()->CellAlgo(GetInstance()->m_nGoalIconTextureIndices[nSpawnedItemID], LIST_NUM_COL, LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

	static int nNumRadarNodes = 0;
	string szObjName = "RadarNode";
	szObjName += (char)('0' + nNumRadarNodes);
	CObject* pRadarSpriteObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	tNode.pIconSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pRadarSpriteObj, tData, false);

	tData.m_nTexture = CTextureManager::GetInstance()->LoadTexture("Resource\\HUD\\FFP_2D_CollectionArrow_FIN.png");

	szObjName = "RadarNodeBG";
	szObjName += (char)('0' + nNumRadarNodes++);
	CObject* pRadarBGSpriteObj = CObjectManager::GetInstance()->CreateObject(szObjName);
	tNode.pBackgroundSpriteComponent = CTextureManager::GetInstance()->CreateSpriteComp(pRadarBGSpriteObj, tData, false);

	GetInstance()->m_lRadarNodes.push_back(tNode);

	GetInstance()->SetUpdateSpriteData(true);
	GetInstance()->UpdateShoppingList();
	GetInstance()->SetUpdateSpriteData(false);
}

void CHUDManager::MainMenuEnterCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	TStateEvent* cEvent = (TStateEvent*)pEvent->GetData();

	if(cEvent->m_eToState == MAIN_MENU_STATE)
	{
		// turn off all the hud elements while in main menu
		GetInstance()->m_pInventory->SetActive(false);
		GetInstance()->m_pHeldItem1->SetActive(false);
		GetInstance()->m_pHeldItem2->SetActive(false);
		GetInstance()->m_pButtonItem1->SetActive(false);
		GetInstance()->m_pButtonItem2->SetActive(false);
		GetInstance()->m_pKey1Comp->SetActive(false);
		GetInstance()->m_pKey2Comp->SetActive(false);
		//GetInstance()->m_pEnemyProgressComponent->SetActive(false);

		for(int i=0; i<4; i++)
		{
			GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetActive(false);
			GetInstance()->m_tPlayerInfo[i].pScoreFontComps->SetActive(false);
			GetInstance()->m_tPlayerInfo[i].pBackgroundSpriteComp->SetActive(false);
		}

		list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator iter;
		iter = GetInstance()->m_tGoalItemsSpawnedPool.begin();
		while(iter != GetInstance()->m_tGoalItemsSpawnedPool.end()) // TODO: Add spawned pool too
		{
			iter->pIconComponent->SetActive(false);
			iter++;
		}

		GetInstance()->m_bHasHitThisUpdateBefore = false; // 

	}
}

void CHUDManager::GoalItemLostCallback(IEvent* pEvent, IComponent* /*pComp*/)
{
	// Get the Values from the Event
	TGoalItemCollectedEvent* pcObjEvent = static_cast<TGoalItemCollectedEvent*>(pEvent->GetData());

	int nDropper = GetInstance()->GetPlayerNum(pcObjEvent->m_pcCollector);
	int nType = (int)pcObjEvent->m_pcGoalItem; // cast the obj* to the int for the item id (sent in inventory rammed)

	// if it's the human
	if(nDropper == 0)
	{
		// do it for the spawned pool

		list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator goalItemIter2;
		goalItemIter2 = GetInstance()->m_tGoalItemsSpawnedPool.begin();
		while(goalItemIter2 != GetInstance()->m_tGoalItemsSpawnedPool.end())
		{
			// if we found the item that got dropped
			if(goalItemIter2->nItemID == nType)
			{
				// say the item got dropped
				goalItemIter2->nPlayerCollected = -1;
				//break; // only one in the list needs to be updated (only one exists, even if it didnt...)
			}

			goalItemIter2++;
		}

		// Make the goal indicator radar node inactive
		list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
		nodeIter = GetInstance()->m_lRadarNodes.begin();

		while(nodeIter != GetInstance()->m_lRadarNodes.end())
		{
			// -5 is the ID for goal indicator
			if(nodeIter->nGoalItemType == -5)
			{
				nodeIter->bSpawned = false;
				break;
				// update will handle the placement code
			}
			nodeIter++;
		}

	}

	// Scoreboard: if teh player and item type are valid
	if(nDropper < 4 && nType < MAX_GOAL_ITEMS)
	{
		// for each rank place
		for(int i=0; i<4; i++)
		{
			// if the current rank's slot's id is the dropper
			if(GetInstance()->m_tPlayerInfo[i].nID == nDropper)
			{
				// lower score for that player
				GetInstance()->m_tPlayerInfo[i].nScore--;

				// make sure it's not below zero
				if(GetInstance()->m_tPlayerInfo[i].nScore <= 0)
				{
					GetInstance()->m_tPlayerInfo[i].nScore = 0;
				}

				// undisplay their checking out radar node
				char szPlayerObjName[8] = "PlayerX";
				szPlayerObjName[6] = (nDropper + '0'); // set the X value to the number the player is right now

				// get the object by that name
				CObject* pPlayerObj = CObjectManager::GetInstance()->GetObjectByName(szPlayerObjName);

				// if we got a valid object back
				if(pPlayerObj)
				{
					// find the radar info node that this player has
					list<TPlayerRadarNodeInfo, scd::allocator<TPlayerRadarNodeInfo>>::iterator infoNodeIter;
					infoNodeIter = GetInstance()->m_tPlayersGoingToCheckout.begin();
					while(infoNodeIter != GetInstance()->m_tPlayersGoingToCheckout.end())
					{
						// if the player is the one what dropped
						if(infoNodeIter->pPlayer == pPlayerObj)
						{
							// find the corresponding radar node in the radar node list that this info is for
							list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
							nodeIter = GetInstance()->m_lRadarNodes.begin();
							while(nodeIter != GetInstance()->m_lRadarNodes.end())
							{
								// if the goal item type is the corresponding one for the player
								if(nodeIter->nGoalItemType == infoNodeIter->nCorrespondingGoalItemType)
								{
									nodeIter->bSpawned = false;
								}
								nodeIter++;
							}
						}

						infoNodeIter++;
					}
				}
			}
		}
	}/**/

	GetInstance()->UpdateShoppingList();

}


void CHUDManager::GameplayStateEntered(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	// show all hud elements
	GetInstance()->m_pInventory->SetActive(true);
	GetInstance()->m_pButtonItem1->SetActive(true);
	GetInstance()->m_pButtonItem2->SetActive(true);
	GetInstance()->m_pKey1Comp->SetActive(true);
	// set the name of the key for text 
	if(!CInputManager::GetInstance()->IsControllerConnected())
	{
		GetInstance()->m_pKey1Comp->SetWord(CInputManager::GetInstance()->GetStringKeyboard(
			CInputManager::GetInstance()->GetKeyboardKey(CMD_USEITEM1)));
	}
	else // controller
	{
		GetInstance()->m_pKey1Comp->SetWord(CInputManager::GetInstance()->GetStringController(
			CInputManager::GetInstance()->GetControllerButton(CMD_USEITEM1)));
	}

	GetInstance()->m_pKey2Comp->SetActive(true);
	// set the name of the key for text 
	if(!CInputManager::GetInstance()->IsControllerConnected())
	{
		GetInstance()->m_pKey2Comp->SetWord(CInputManager::GetInstance()->GetStringKeyboard(
			CInputManager::GetInstance()->GetKeyboardKey(CMD_USEITEM2)));
	}
	else // controller
	{
		GetInstance()->m_pKey2Comp->SetWord(CInputManager::GetInstance()->GetStringController(
			CInputManager::GetInstance()->GetControllerButton(CMD_USEITEM2)));
	}

	GetInstance()->m_pShoppingListBackgroundSpriteComp->SetActive(true);

	//// disable the check marks
	//for(int i=0; i<MAX_GOAL_ITEMS; i++)
	//{
	//	GetInstance()->m_pCheckMarkSpriteComp[i]->SetActive(true); // no need for this because they're set/unset finely in update
	//}

	if(GetInstance()->m_bShowHeld1 == true)
	{
		GetInstance()->m_pHeldItem1->SetActive(true);
	}
	if(GetInstance()->m_bShowHeld2 == true)
	{
		GetInstance()->m_pHeldItem2->SetActive(true);
	}



	// radar nodes
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = GetInstance()->m_lRadarNodes.begin();

	while(nodeIter != GetInstance()->m_lRadarNodes.end())
	{
		if(nodeIter->bSpawned)
		{
			nodeIter->pIconSpriteComponent->SetActive(true);
			nodeIter->pBackgroundSpriteComponent->SetActive(true);
		}
		nodeIter++;
	}


	// Show leaderboard
	for(int i=0; i<4; i++)
	{
		GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pScoreFontComps->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pBackgroundSpriteComp->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pRankNumberComponent->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pRankNumberBackgroundComponent->SetActive(true);
	}


	// show all the stuff in the spawned pool
	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator iter2;
	iter2 = GetInstance()->m_tGoalItemsSpawnedPool.begin();

	while(iter2 != GetInstance()->m_tGoalItemsSpawnedPool.end())
	{
		if(iter2->pIconComponent)
		{
			iter2->pIconComponent->SetActive(true);
			//int itemid = iter2->nItemID;
		}
		iter2++;
	}

	GetInstance()->UpdateShoppingList();
}

void CHUDManager::GameplayStateExited(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	// Hide all hud elements
	GetInstance()->m_pInventory->SetActive(false);
	GetInstance()->m_pButtonItem1->SetActive(false);
	GetInstance()->m_pButtonItem2->SetActive(false);
	GetInstance()->m_pKey1Comp->SetActive(false);
	GetInstance()->m_pKey2Comp->SetActive(false);
	GetInstance()->m_pShoppingListBackgroundSpriteComp->SetActive(false);

	// disable the check marks
	for(int i=0; i<MAX_GOAL_ITEMS; i++)
	{
		GetInstance()->m_pCheckMarkSpriteComp[i]->SetActive(false);
	}

	// radar nodes
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = GetInstance()->m_lRadarNodes.begin();

	while(nodeIter != GetInstance()->m_lRadarNodes.end())
	{
		nodeIter->pIconSpriteComponent->SetActive(false);
		nodeIter->pBackgroundSpriteComponent->SetActive(false);
		nodeIter++;
	}

	//Held items
	if(GetInstance()->m_bShowHeld1 == true)
	{
		GetInstance()->m_pHeldItem1->SetActive(false);
	}
	if(GetInstance()->m_bShowHeld2 == true)
	{
		GetInstance()->m_pHeldItem2->SetActive(false);
	}

	//Ranks
	for(int i=0; i<4; i++)
	{
		GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pScoreFontComps->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pBackgroundSpriteComp->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pRankNumberComponent->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pRankNumberBackgroundComponent->SetActive(false);
	}

	//Shopping list
	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator iter2;
	iter2 = GetInstance()->m_tGoalItemsSpawnedPool.begin();

	while(iter2 != GetInstance()->m_tGoalItemsSpawnedPool.end())
	{
		if(iter2->pIconComponent)
		{
			iter2->pIconComponent->SetActive(false);

		}
		iter2++;
	}
}

void CHUDManager::GameplayStateInit(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	GetInstance()->m_bShowHeld1 = false;
	GetInstance()->m_bShowHeld2 = false;

	TSpriteData tSpriteData = GetInstance()->m_pHeldItem1->GetSpriteData();
	tSpriteData.m_nTexture = 31;
	tSpriteData = GetInstance()->m_pHeldItem2->GetSpriteData();
	tSpriteData.m_nTexture = 31;

	// show all hud elements
	GetInstance()->m_pInventory->SetActive(true);
	//GetInstance()->m_pHeldItem1->SetActive(true);
	//GetInstance()->m_pHeldItem2->SetActive(true);
	GetInstance()->m_pButtonItem1->SetActive(true);
	GetInstance()->m_pButtonItem2->SetActive(true);
	GetInstance()->m_pKey1Comp->SetActive(true);


	// Unshow the all the radar nodes so they can reshow when they spawn
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = GetInstance()->m_lRadarNodes.begin();
	while(nodeIter != GetInstance()->m_lRadarNodes.end())
	{
		nodeIter->bSpawned = false;
		nodeIter++;
	}


	// set the name of the key for text 
	if(!CInputManager::GetInstance()->IsControllerConnected())
	{
		GetInstance()->m_pKey1Comp->SetWord(CInputManager::GetInstance()->GetStringKeyboard(
			CInputManager::GetInstance()->GetKeyboardKey(CMD_USEITEM1)));
	}
	else // controller
	{
		GetInstance()->m_pKey1Comp->SetWord(CInputManager::GetInstance()->GetStringController(
			CInputManager::GetInstance()->GetControllerButton(CMD_USEITEM1)));
	}

	GetInstance()->m_pKey2Comp->SetActive(true);
	// set the name of the key for text 
	if(!CInputManager::GetInstance()->IsControllerConnected())
	{
		GetInstance()->m_pKey2Comp->SetWord(CInputManager::GetInstance()->GetStringKeyboard(
			CInputManager::GetInstance()->GetKeyboardKey(CMD_USEITEM2)));
	}
	else // controller
	{
		GetInstance()->m_pKey2Comp->SetWord(CInputManager::GetInstance()->GetStringController(
			CInputManager::GetInstance()->GetControllerButton(CMD_USEITEM2)));
	}
	//GetInstance()->m_pEnemyProgressComponent->SetActive(true);
	GetInstance()->m_pHeldItem1->SetActive(false);
	GetInstance()->m_pHeldItem2->SetActive(false);
	GetInstance()->m_pShoppingListBackgroundSpriteComp->SetActive(true);

	// disable the check marks
	for(int i=0; i<MAX_GOAL_ITEMS; i++)
	{
		GetInstance()->m_pCheckMarkSpriteComp[i]->SetActive(false);
	}


	// Show leaderboard
	for(int i=0; i<4; i++)
	{
		GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pScoreFontComps->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pBackgroundSpriteComp->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pRankNumberComponent->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pRankNumberBackgroundComponent->SetActive(true);
	}


	// Move all show all the icons in the despawned pool
	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator iter2;
	iter2 = GetInstance()->m_tGoalItemsSpawnedPool.begin();

	// show and move to despawned all the stuff in the spawned pool
	while(iter2 != GetInstance()->m_tGoalItemsSpawnedPool.end())
	{
		if(iter2->pIconComponent)
			iter2->pIconComponent->SetActive(true);
		iter2->nPlayerCollected = -1;

		iter2++;
	}
	for(int i=0; i<4; i++)
	{
		CHUDManager::GetInstance()->m_tPlayerInfo[i].nScore = 0;
	}

	GetInstance()->SetUpdateSpriteData(true);
	GetInstance()->UpdateShoppingList();
	GetInstance()->SetUpdateSpriteData(false);

}


void CHUDManager::PlayerPickupItem(IEvent* pEvent, IComponent* /*pComp*/)
{
	TObjectEvent* tEvent = (TObjectEvent*)pEvent->GetData();

	int nPlayerNum = GetInstance()->GetPlayerNum(tEvent->m_pcObj);

	// if it's a valid player
	if(nPlayerNum < 4) 
	{
		// [i] is rank, not player ID
		for(int i=0; i<4; i++)
		{
			// if the current rank's player id is the player number
			if(GetInstance()->m_tPlayerInfo[i].nID == nPlayerNum)
			{
				// increment their score
				GetInstance()->m_tPlayerInfo[i].nScore++;

				// if their score is high enough to win
				if(GetInstance()->m_tPlayerInfo[i].nScore >= 8)
				{
					// Take care of showing enemies who are checking out on the radar
					char szPlayerObjName[8] = "PlayerX";
					szPlayerObjName[6] = (nPlayerNum + '0'); // set the X value to the number the player is right now

					// get the object by that name
					CObject* pPlayerObj = CObjectManager::GetInstance()->GetObjectByName(szPlayerObjName);

					// if we actually got an object back
					if(pPlayerObj)
					{
						// find the radar info node that this player has
						list<TPlayerRadarNodeInfo, scd::allocator<TPlayerRadarNodeInfo>>::iterator infoNodeIter;
						infoNodeIter = GetInstance()->m_tPlayersGoingToCheckout.begin();
						while(infoNodeIter != GetInstance()->m_tPlayersGoingToCheckout.end())
						{
							if(infoNodeIter->pPlayer == pPlayerObj)
							{
								// find the corresponding radar node in the radar node list that this info is for
								list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
								nodeIter = GetInstance()->m_lRadarNodes.begin();
								while(nodeIter != GetInstance()->m_lRadarNodes.end())
								{
									// if the goal item type is the corresponding one for the player
									if(nodeIter->nGoalItemType == infoNodeIter->nCorrespondingGoalItemType)
									{
										nodeIter->bSpawned = true;
										break;
									}
									nodeIter++;
								}
							}

							infoNodeIter++;
						}

					}
				}

				// if it's the HUD's player and if they have all the items
				if(nPlayerNum == 0 &&
					GetInstance()->m_tPlayerInfo[i].nScore >= 8)
				{
					// Make the goal indicator radar node active
					list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
					nodeIter = GetInstance()->m_lRadarNodes.begin();

					while(nodeIter != GetInstance()->m_lRadarNodes.end())
					{
						// -5 is the ID for goal indicator
						if(nodeIter->nGoalItemType == -5)
						{
							nodeIter->bSpawned = true;
							break;
							// update will handle the placement code
						}
						nodeIter++;
					}
				}
			}
		}
	}
}


void CHUDManager::PlayerLocation(IEvent* pEvent, IComponent* /*pComp*/)
{
	TObjectEvent* tEvent = (TObjectEvent*)pEvent->GetData();

	GetInstance()->m_pPlayerObject = tEvent->m_pcObj;	
}

void CHUDManager::DisableImages(IEvent* /*pEvent*/, IComponent* /*pComp*/)
{
	GetInstance()->m_pInventory->SetActive(false);
	GetInstance()->m_pButtonItem1->SetActive(false);
	GetInstance()->m_pButtonItem2->SetActive(false);
	GetInstance()->m_pKey1Comp->SetActive(false);
	GetInstance()->m_pKey2Comp->SetActive(false);
	GetInstance()->m_pRankNumberBackgroundComponent->SetActive(false);

	//CWwiseSoundManager::GetInstance()->PlayTheSound(BULLDOG_SPEED_STOP, GLOBAL_ID);

	if(GetInstance()->m_bShowHeld1 == true)
	{
		GetInstance()->m_pHeldItem1->SetActive(false);
	}
	if(GetInstance()->m_bShowHeld2 == true)
	{
		GetInstance()->m_pHeldItem2->SetActive(false);
	}

	for(int i=0; i<4; i++)
	{
		GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pScoreFontComps->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pBackgroundSpriteComp->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pRankNumberComponent->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pRankNumberBackgroundComponent->SetActive(false);
	}


	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator iter2;
	iter2 = GetInstance()->m_tGoalItemsSpawnedPool.begin();

	while(iter2 != GetInstance()->m_tGoalItemsSpawnedPool.end())
	{
		if(iter2->pIconComponent)
		{
			iter2->pIconComponent->SetActive(false);

		}
		iter2++;
	}

}

void CHUDManager::SendStolenItemEvent(int nSlot)
{
	if(nSlot == 0)
	{
		SendIEvent("StealHeldItemEffect", m_pHeldItem1, NULL, PRIORITY_NORMAL);
	}
	else // other slot
	{
		SendIEvent("StealHeldItemEffect", m_pHeldItem2, NULL, PRIORITY_NORMAL);
	}
}


void CHUDManager::SendStolenVictimItemEvent(int nSlot)
{
	if(nSlot == 0)
	{
		SendIEvent("HeldItemStolenEffect", m_pHeldItem1, NULL, PRIORITY_NORMAL);
	}
	else // other slot
	{
		SendIEvent("HeldItemStolenEffect", m_pHeldItem2, NULL, PRIORITY_NORMAL);
	}
}


int CHUDManager::SetCharacterPicked(lua_State* pLua)
{
	int nPlayer = lua_tointeger(pLua, -2);
	int nCharNum = lua_tointeger(pLua, -1);

	for(int i=0; i<4; i++)
	{
		if(GetInstance()->m_tPlayerInfo[i].nID == nPlayer)
		{
			// nPlayer is the player number and nCharNum is the number corresponding to the index of characters in the portraits file
			SendTwoIntEvent("PlayerCharacterSet", (IComponent*)GetInstance(), nPlayer, nCharNum);
			TSpriteData tData = GetInstance()->m_tPlayerInfo[i].pSpriteComp->GetSpriteData();
			tData.m_tRect = GetInstance()->CellAlgo(nCharNum, 2, 190, 114);
			GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetSpriteData(tData);
			GetInstance()->SetPlayerChar(nPlayer, nCharNum);
		}
	}

	// if it's the human hud user and a valid player type, set the shopping list texture to the char's one
	if(nPlayer == 0 && nCharNum >= 0 && nCharNum <7)
	{
		TSpriteData tData = GetInstance()->m_pShoppingListBackgroundSpriteComp->GetSpriteData();
		tData.m_nTexture = GetInstance()->m_nShoppingListBackgroundTex[nCharNum];
		GetInstance()->m_pShoppingListBackgroundSpriteComp->SetSpriteData(tData);
	}
	lua_pop(pLua, 2);
	return 0;
}


void CHUDManager::InitCheckoutLocation(IEvent* pEvent, IComponent* /*pComp*/)
{
	// set the checkout's radar loc from level mgr
	D3DXVECTOR3 vCheckoutLoc(CLevelManager::GetInstance()->GetCheckoutNode()->tWorldMatrix._41,
		CLevelManager::GetInstance()->GetCheckoutNode()->tWorldMatrix._42,
		CLevelManager::GetInstance()->GetCheckoutNode()->tWorldMatrix._43);

	list<TRadarNode, scd::allocator<TRadarNode>>::iterator iter;
	iter = GetInstance()->m_lRadarNodes.begin();

	while(iter != GetInstance()->m_lRadarNodes.end())
	{
		// if its type is the one specified for checkout
		if(iter->nGoalItemType == -5)
		{
			iter->vTargetPosition = vCheckoutLoc;
			break;
		}
		iter++;
	}

}
////////////////////////////////////////////////////////////////////////////////
//Return the character id for the correct player passed in
//this helps with the correct sounds being played
////////////////////////////////////////////////////////////////////////////////
int CHUDManager::GetPlayerCharID(int playerNum)
{
	switch(m_nPlayer[playerNum])
	{
	case BIKER_CHARACTER:
		{
			return BIKER_ID;
			break;
		}
	case BANDITOS_CHARACTER:
		{
			return BANDITOS_ID;
			break;
		}
	case LARPERS_CHARACTER:
		{
			return LARPER_ID;
			break;
		}
	case SCIENTIST_CHARACTER:
		{
			return SCIENTIST_ID;
			break;
		}
	case SASHA_CHARACTER:
		{
			return SASHA_ID;
			break;
		}
	case CRYGAME_CHARACTER:
		{
			return CRYGAME_ID;
			break;
		}
	case STORYTIME_CHARACTER:
		{
			return STORYTIME_ID;
			break;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
//Play the character item use sound for the correct character
////////////////////////////////////////////////////////////////////////////////
void CHUDManager::PlayUseItemSound(CObject *player)
{
	switch(m_nPlayer[GetPlayerNum(player)])
	{
	case BIKER_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(BULLDOG_ITEM_USE, BIKER_ID);
			break;
		}
	case BANDITOS_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(BANDITOS_ITEMUSE, BANDITOS_ID);
			break;
		}
	case LARPERS_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(LARPER_ITEMUSE, LARPER_ID);
			break;
		}
	case SCIENTIST_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(SCIENTIST_ITEMUSE, SCIENTIST_ID);
			break;
		}
	case SASHA_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(SASHA_ITEMUSE, SASHA_ID);
			break;
		}
	case CRYGAME_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(CRYGAME_ITEMUSE, CRYGAME_ID);
			break;
		}
	case STORYTIME_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(STORYTIME_SHOVE, STORYTIME_ID);
			break;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CSpriteComponent* CHUDManager::GetListIconComponent(EGoalItemType eType)
{
	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsSpawnedPool.begin();
	while(goalItemIter != m_tGoalItemsSpawnedPool.end())
	{
		if(goalItemIter->nItemID == eType)
		{
			return goalItemIter->pIconComponent;
		}

		goalItemIter++;
	}
	return 0;
}


void CHUDManager::UpdateShoppingList()
{	
	// Upate List
	int nItemType;
	TSpriteData tSpriteData = {};
	int nPosCounter = 0; // in loop counter
	int nCurrentIcon = 0; // counter for all the loops (doesn't reset)

	// Put correct marks on things that need it
	list<TShoppingListItem, scd::allocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsSpawnedPool.begin();
	while(goalItemIter != m_tGoalItemsSpawnedPool.end())
	{
		// get the current iter's item type
		nItemType = goalItemIter->nItemID;

		// Get the list pos data
		tSpriteData = m_tListIconData[nCurrentIcon];

		if(true)
		{
			//Set the position to the next spot on this list's section
			RECT pos = CellAlgo(nPosCounter, 2, 44, 44);
			float fListX = 35;
			float fListY = 285;
			tSpriteData.m_nX = (int)(fListX + pos.left);
			tSpriteData.m_nY = (int)(fListY + pos.top);
			//tSpriteData.m_fScaleX = .4f;
			//tSpriteData.m_fScaleY = .4f;
		}


		// Lit or unlit check

		// if no one's picked it up (it's in the level)
		if(goalItemIter->bSpawned)
		{
			// Find the Cell for current icon
			tSpriteData.m_tRect = CellAlgo(
				m_nGoalIconTextureIndices[goalItemIter->nItemID], LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// set to lit texture since it's spawned
			tSpriteData.m_nTexture = m_nGoalIconsTex;

			// Apply the normal icon
			goalItemIter->pIconComponent->SetSpriteData(tSpriteData);
			//goalItemIter->pIconComponent->SetActive(true);
		}
		else // it's not spawned
		{
			tSpriteData.m_tRect = CellAlgo(
				m_nGoalIconTextureIndices[goalItemIter->nItemID], LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// set to UNlit texture since it's despawned
			tSpriteData.m_nTexture = m_nGoalIconsDarkenedTex;


			// Apply the normal icon
			goalItemIter->pIconComponent->SetSpriteData(tSpriteData);
			//goalItemIter->pIconComponent->SetActive(true);
		}

		// Check mark check:

		// if the item is collected by the player
		if(goalItemIter->nPlayerCollected == 0)
		{
			// TODO: Put check mark over that slot

			m_tCheckMarkSpriteInfo = tSpriteData;

			// make the texture the check mark
			m_tCheckMarkSpriteInfo.m_nTexture = m_nGoalIconsTex;

			// Set the sprite to show the check mark part
			m_tCheckMarkSpriteInfo.m_tRect = CellAlgo(CHECK_LIST_CELL_IDX, 4, LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			//m_tCheckMarkSpriteInfo.m_fScaleX = .4f;
			//m_tCheckMarkSpriteInfo.m_fScaleY = .4f;
			m_tCheckMarkSpriteInfo.m_nZ += 1; 


			
			// Apply the checkmark
			m_pCheckMarkSpriteComp[goalItemIter->nItemID]->SetSpriteData(m_tCheckMarkSpriteInfo);
			m_pCheckMarkSpriteComp[goalItemIter->nItemID]->SetActive(true);
		}
		else // make sure that the checkmark isn't showing
		{
			if(m_pCheckMarkSpriteComp[goalItemIter->nItemID])
			{
				m_pCheckMarkSpriteComp[goalItemIter->nItemID]->SetActive(false);
			}
		}

		// Inc Pos
		++nPosCounter;
		++nCurrentIcon;
		goalItemIter++;
	}


}
////////////////////////////////////////////////////////////////////////////////
//Play the character Lead taken sound for the correct character
////////////////////////////////////////////////////////////////////////////////
void CHUDManager::PlayLead(int playerNum)
{
	switch(m_nPlayer[playerNum])
	{
	case BIKER_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(LEADTAKEN_BIKERS, GLOBAL_ID);
			break;
		}
	case BANDITOS_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(LEADTAKEN_BANDITOS, GLOBAL_ID);
			break;
		}
	case LARPERS_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(LEADTAKEN_LARPERS, GLOBAL_ID);
			break;
		}
	case SCIENTIST_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(LEADTAKEN_SCIENTISTS, GLOBAL_ID);
			break;
		}
	case SASHA_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(LEADTAKEN_SASHA, GLOBAL_ID);
			break;
		}
	case CRYGAME_CHARACTER:
		{
			break;
		}
	case STORYTIME_CHARACTER:
		{
			break;
		}
	}
}


void CHUDManager::GoalItemDespawning(IEvent* pEvent, IComponent* pComp)
{
	TGoalItemEvent* tEvent = (TGoalItemEvent*)pEvent->GetData();

	int nType = tEvent->m_eGoalItemType;

	// Make the radar node inactive
	list<TRadarNode, scd::allocator<TRadarNode>>::iterator nodeIter;
	nodeIter = GetInstance()->m_lRadarNodes.begin();

	while(nodeIter != GetInstance()->m_lRadarNodes.end())
	{
		if(nodeIter->nGoalItemType == nType)
		{
			nodeIter->bDespawning = true;
			break;
			// update will handle the placement code
	}
		nodeIter++;
	}


}