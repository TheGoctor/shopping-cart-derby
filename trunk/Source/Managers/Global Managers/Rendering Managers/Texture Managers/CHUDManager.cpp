////////////////////////////////////////////////////////
//  File			:	"CHUDManager.cpp"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/10/11
//
//	Last Changed	:	04/11/11
//
//  Purpose			:	Wrapper class for manageing HUD elements
////////////////////////////////////////////////////////
#include "CHUDManager.h"
#include "..\\..\\Object Manager\\CObjectManager.h"
#include "..\\..\\Object Manager\\CObjectManager.h"
#include "..\\..\\Event Manager\\CEventManager.h"
#include "..\\..\\Event Manager\\CGoalItemCollected.h"
#include "..\\..\\..\\Component Managers\\CLevelManager.h"
#include "..\\..\\..\\..\\Components\\Level\\CGoalItemComponent.h"
#include "..\\..\\Event Manager\\CUpdateStateEvent.h"

#define LIST_NUM_COL		(4)
#define LIST_CELL_WIDTH		(64)
#define LIST_CELL_HEIGHT	(64)
#define ENEMY_PROGRESS_X	(895)
#define CHECK_LIST_CELL_IDX (27)

static const int g_yRankPos[4] = { 38, 100, 165, 228 };

enum EListIconPos { NO_POS = -1, TOP_1, TOP_2, MIDDLE_1, MIDDLE_2,
	BOTTOM_1, BOTTOM_2, BOTTOM_3, BOTTOM_4 };

CHUDManager::CHUDManager(void) : m_pEnemyProgress(NULL), m_pTM(NULL)
{
	for(int b = 0; b < TOTAL_GOAL_ITEMS; ++b)
	{
		m_bPlayerItems[b] = false;
	}
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

void CHUDManager::InitShoppingList(void)
{
	// Load Sprite Sheet
	m_nGoalIconsTex = m_pTM->LoadTexture("Resource\\HUD\\FFP_2D_GoalItems_FIN.png");

	// Idxs
	m_nGoalIconIdxs[GORILLA_MILK] = 4; 
	m_nGoalIconIdxs[CAPTAIN_FLAKEY] = 2; 
	m_nGoalIconIdxs[AGENT_ORANGE] = 5; 
	m_nGoalIconIdxs[BAKON] = 7; 
	m_nGoalIconIdxs[KILLER_TOMATO_SOUP] = 8; 
	m_nGoalIconIdxs[SAD_APPLE] = 6; 
	m_nGoalIconIdxs[FRENCHY_BREAD] = 3; 
	m_nGoalIconIdxs[SAMMY_SANDWICH] = 0; 

	// Setup Sprite Data
	
		// Bottom
		m_tListIconData[BOTTOM_1].m_nTexture = m_nGoalIconsTex;
		m_tListIconData[BOTTOM_1].m_nX = 45;
		m_tListIconData[BOTTOM_1].m_nY = 680;
		m_tListIconData[BOTTOM_1].m_nZ = 1;
		m_tListIconData[BOTTOM_1].m_fScaleX = 0.5f;
		m_tListIconData[BOTTOM_1].m_fScaleY = 0.5f;
		m_tListIconData[BOTTOM_1].m_fRotCenterX = 0.0f;
		m_tListIconData[BOTTOM_1].m_fRotCenterY = 0.0f;
		m_tListIconData[BOTTOM_1].m_fRot = 0.0f;
		m_tListIconData[BOTTOM_1].m_dwColor = -1;
		m_tListIconData[BOTTOM_1].m_tRect.top    = 0;
		m_tListIconData[BOTTOM_1].m_tRect.right  = 0;
		m_tListIconData[BOTTOM_1].m_tRect.left   = 0;
		m_tListIconData[BOTTOM_1].m_tRect.bottom = 0;

		m_tListIconData[BOTTOM_2] = m_tListIconData[BOTTOM_1];
		m_tListIconData[BOTTOM_2].m_nX += 23;

		m_tListIconData[BOTTOM_3] = m_tListIconData[BOTTOM_2];
		m_tListIconData[BOTTOM_3].m_nX += 23;

		m_tListIconData[BOTTOM_4] = m_tListIconData[BOTTOM_3];
		m_tListIconData[BOTTOM_4].m_nX += 23;

		// Middle
		m_tListIconData[MIDDLE_1] = m_tListIconData[BOTTOM_1];
		m_tListIconData[MIDDLE_1].m_nX = 60;
		m_tListIconData[MIDDLE_1].m_nY = 633;
		m_tListIconData[MIDDLE_1].m_fScaleX = 0.65f;
		m_tListIconData[MIDDLE_1].m_fScaleY = 0.65f;
		
		m_tListIconData[MIDDLE_2] = m_tListIconData[MIDDLE_1];
		m_tListIconData[MIDDLE_2].m_nX += 33;
		
		// Top
		m_tListIconData[TOP_1] = m_tListIconData[MIDDLE_1];
		m_tListIconData[TOP_1].m_nX = 45;
		m_tListIconData[TOP_1].m_nY = 600;
		m_tListIconData[TOP_1].m_fScaleX = 0.8f;
		m_tListIconData[TOP_1].m_fScaleY = 0.8f;

		m_tListIconData[TOP_2] = m_tListIconData[TOP_1];
		m_tListIconData[TOP_2].m_nX += 50;

	// Create the Icon Objs
	CObject* pObj;
	TSpriteData tSpriteData;
	for(unsigned int uIcon = 0; uIcon < TOTAL_GOAL_ITEMS; ++uIcon)
	{
		// Creat the object
		string szObjID = "Icon";
		szObjID += uIcon;
		pObj = m_pOM->CreateObject(szObjID);
		
		// Get Inital Sprite Data
		tSpriteData = m_tListIconData[uIcon];
				
		//CellAlgo(uIcon, LIST_NUM_COL, LIST_CELL_WIDTH, LIST_CELL_HEIGHT);
		CSpriteComponent* pSC = m_pTM->CreateSpriteComp(
			pObj, tSpriteData, false);

		// Add it to the list
		m_cListIconComps[uIcon] = pSC;
	}
}

void CHUDManager::InitEnemyProgress(void)
{
	// Load Sprite Sheet
	int nTexID = m_pTM->LoadTexture("Resource\\HUD\\Competition_Progress.png");
	int nCharSheetID = m_pTM->LoadTexture("Resource\\HUD\\Character_Sprite_Sheet.png");

	// Create the Object
	m_pEnemyProgress = m_pOM->CreateObject("EnemyProgressObj");

	// Get Inital Sprite Data
	TSpriteData tSpriteData;
	tSpriteData.m_nTexture = nTexID;
	tSpriteData.m_nX = ENEMY_PROGRESS_X;
	tSpriteData.m_nY = 0;
	tSpriteData.m_nZ = 0;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = -1;
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	// Add Sprite Comp
	m_pEnemyProgressComponent = m_pTM->CreateSpriteComp(m_pEnemyProgress, tSpriteData, false);

	// Font
	CBitmapFont font;
	font.LoadFont("Resource\\BitmapFont.png", "Resource\\BitmapFont.txt");
	font.SetCharHeight(15);
	font.SetRectWidth(13);
	font.SetNumColumns(33);

	int x, y;
	float scale;
	int nCharIdx = 0;
	for(int bit = 0; bit < 4; ++bit)
	{
		// Characters
		tSpriteData.m_nTexture = nCharSheetID;
		tSpriteData.m_nX = ENEMY_PROGRESS_X;
		tSpriteData.m_nY = bit * 64;
		tSpriteData.m_nZ = 1;
		tSpriteData.m_dwColor = -1;
		tSpriteData.m_tRect = CellAlgo(nCharIdx, 6, 64, 64);

		CObject* pCharObj = m_pOM->CreateObject("Char" + bit);

		m_tPlayerInfo[bit].nID = bit;

		m_tPlayerInfo[bit].pSpriteComp = 
			m_pTM->CreateSpriteComp(pCharObj, tSpriteData, false);

		// Fonts
		x = 960;
		y = g_yRankPos[bit];
		scale = 1.5f;
		CObject* pBitObj = m_pOM->CreateObject("Score" + bit);
		m_tPlayerInfo[bit].pScoreFontComps = m_pTM->CreateBitmapFontComp(pBitObj, "0/8", font, x,
			y, scale, D3DXCOLOR(0, 200, 0, 255), false);

		nCharIdx += 7;
	}
	
	
}

void CHUDManager::InitRadar(void)
{
	// Load Sprite Sheet
	int nTexID = m_pTM->LoadTexture("Resource\\HUD\\Mini-Map_Circle.png");

	// Create the Object
	m_pEnemyProgress = m_pOM->CreateObject("RadarObj");

	// Get Inital Sprite Data
	TSpriteData tSpriteData;
	tSpriteData.m_nTexture = nTexID;
	tSpriteData.m_nX = 750;
	tSpriteData.m_nY = 530;
	tSpriteData.m_nZ = 1;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = -1;
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	// Add Sprite Comp
	m_pRadarSpriteComponent = m_pTM->CreateSpriteComp(m_pEnemyProgress, tSpriteData, false);
}

void CHUDManager::Init(void)
{
	// Get Singletons
	m_pTM = CTextureManager::GetInstance();
	m_pOM = CObjectManager::GetInstance();

	// HUD Elements
	InitShoppingList();
	InitEnemyProgress();
	InitRadar();
	InitInventory();

	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("GoalItemSpawned",
		NULL, GoalItemSpawnedCallback);

	//pEM->RegisterEvent("GoalItemDespawned",
	//	NULL, GoalItemDespawnedCallback);

	pEM->RegisterEvent("GoalItemCollected",
		NULL, GoalItemCollectedCallback);

	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE; // catch update for only gameplay state
	pEM->RegisterEvent(szEventName,
		NULL, UpdateCallback);

	pEM->RegisterEvent("GoalItemInit",
		NULL, GoalItemInitCallback);

	pEM->RegisterEvent("Boost",
		NULL, BoostCallback);
}

void CHUDManager::InitInventory(void)
{
	// Load Sprite Sheet
	int nTexID = m_pTM->LoadTexture("Resource\\HUD\\Attack_Item_HUD_Left.png");
	int nDrinkID = m_pTM->LoadTexture("Resource\\HUD\\FFP_2D_HeldItem_FIN.png");

	// Create the Object
	CObject* pObj;
	pObj = m_pOM->CreateObject("InventoryHUD");

	// Get Inital Sprite Data
	TSpriteData tSpriteData;
	tSpriteData.m_nTexture = nTexID;
	tSpriteData.m_nX = 400;
	tSpriteData.m_nY = 0;
	tSpriteData.m_nZ = 1;
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_dwColor = -1;
	tSpriteData.m_tRect.top    = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.bottom = 0;

	// Add Sprite Comp
	m_pInventory = m_pTM->CreateSpriteComp(pObj, tSpriteData, false);

	// Energy Drink
	tSpriteData.m_nTexture = nDrinkID;
	tSpriteData.m_nY += 10;
	tSpriteData.m_tRect = CellAlgo(2, 2, 128, 128);
	pObj = m_pOM->CreateObject("EnergyDrinkHUD");
	m_pEnergyDrink = m_pTM->CreateSpriteComp(pObj, tSpriteData, false);
}

void CHUDManager::Render(void)
{
}

// Goal Item Spawn Callback
void CHUDManager::GoalItemSpawned(CGoalItemEvent* pcObjEvent)
{
	//static bool bSwitch = false;
	//bSwitch = !bSwitch;
	//if(bSwitch == false)
	//{
	//	return;
	//}
	
	// Update the List
	CGoalItems* pGIC;
	TSpriteData tSpriteData;
	int nItemType;
	int nPosCounter = 0;

	deque<CGoalItems*, CAllocator<CGoalItems*>>::iterator goalItemIter;
	goalItemIter = m_cGoalItemsPool.begin();
	while(goalItemIter != m_cGoalItemsPool.end())
	{
		// Get the list pos data
		tSpriteData = m_tListIconData[nPosCounter];

		// Get the type from the list
		pGIC = *goalItemIter;
		nItemType = pGIC->m_eType;

		// Find the Cell
		tSpriteData.m_tRect = CellAlgo(
			m_nGoalIconIdxs[nItemType], LIST_NUM_COL,
			LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

		// Apply it
		m_cListIconComps[nPosCounter]->SetSpriteData(tSpriteData);
		m_cListIconComps[nPosCounter]->SetActive(true);

		// Inc Pos
		++nPosCounter;
		goalItemIter++;
	}

	//for(int times = 0; times < 2; ++times)
	//{
		pGIC = m_cGoalItemsPool.front();
		m_cGoalItemsPool.pop_front();
		m_cGoalItemsPool.push_back(pGIC);
	//}
}

void CHUDManager::GoalItemSpawnedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CGoalItemEvent* pcObjEvent = static_cast<CGoalItemEvent*>(pEvent);
	GetInstance()->GoalItemSpawned(pcObjEvent);
}

void CHUDManager::GoalItemDespawned(EGoalItemType eType)
{
	// Find a spot
	static int nIdx = BOTTOM_1;

	// Get Inital Sprite Data
	CSpriteComponent* pSC;
	pSC = m_cListIconComps[nIdx];
	TSpriteData tSpriteData = pSC->GetSpriteData();
	tSpriteData.m_tRect = GetInstance()->CellAlgo(
		m_nGoalIconIdxs[eType], LIST_NUM_COL, 
		LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

	// Set New Data
	pSC->SetSpriteData(tSpriteData);
	pSC->SetActive(true);

	// Inc Pos
	if(BOTTOM_4 == nIdx)
		nIdx = BOTTOM_1;
	else
		nIdx += 1;
}

void CHUDManager::GoalItemDespawnedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CGoalItemEvent* pcObjEvent = static_cast<CGoalItemEvent*>(pEvent);
	EGoalItemType eType = pcObjEvent->GetGoalItemType();
	
	GetInstance()->GoalItemDespawned(eType);
}

void CHUDManager::StartDespawnTimer(EGoalItemType eType)
{
	// Find a spot
	static bool bFirst = true;
	int uIdx = (bFirst) ? TOP_1 : TOP_2;

	// Get Inital Sprite Data
	CSpriteComponent* pSC;
	pSC = m_cListIconComps[uIdx];
	TSpriteData tSpriteData = pSC->GetSpriteData();
	tSpriteData.m_tRect = GetInstance()->CellAlgo(
		m_nGoalIconIdxs[eType], LIST_NUM_COL,
		LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

	// Set New Data
	pSC->SetSpriteData(tSpriteData);
	pSC->SetActive(true);

	// toggle Pos
	bFirst = !bFirst;
}

void CHUDManager::StartDespawnTimerCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CGoalItemEvent* pcObjEvent = static_cast<CGoalItemEvent*>(pEvent);
	EGoalItemType eType = pcObjEvent->GetGoalItemType();
	
	GetInstance()->StartDespawnTimer(eType);
}

void CHUDManager::GoalItemCollected(CGoalItemCollectedEvent* pcObjEvent)
{
	static bool playerItems[4][8] = {0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0,
									 0, 0, 0, 0, 0, 0, 0, 0, };

	int playerID = pcObjEvent->GetCollectorType();

	EGoalItemType nType = CLevelManager::GetInstance()->GetGoalItemType(pcObjEvent->GetGoalItem());

	// Find the item
	if(playerItems[playerID][nType] == false)
	{
		for(int ply = 0; ply < 4; ply++)
		{
			if(m_tPlayerInfo[ply].nID == playerID)
				m_tPlayerInfo[ply].nScore++;
		}

		playerItems[playerID][nType] = true;

		if(playerID == 0)
		{
			m_bPlayerItems[nType] = true;
		}
	}
}

void CHUDManager::GoalItemCollectedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	CGoalItemCollectedEvent* pcObjEvent = static_cast<CGoalItemCollectedEvent*>(pEvent);
	GetInstance()->GoalItemCollected(pcObjEvent);
}

void CHUDManager::Update(void)
{
	static bool bHasHitThisUpdateBefore = false;
	if(!bHasHitThisUpdateBefore)
	{	
		m_pInventory->SetActive(true);
		m_pEnergyDrink->SetActive(true);
		m_pEnemyProgressComponent->SetActive(true);
		m_pRadarSpriteComponent->SetActive(true);
		
		for(int i=0; i<4; i++)
		{
			m_tPlayerInfo[i].pSpriteComp->SetActive(true);
			m_tPlayerInfo[i].pScoreFontComps->SetActive(true);
		}

		bHasHitThisUpdateBefore = true;

	}


	// Upate List
	CGoalItems* pGIC;
	int nItemType;
	TSpriteData tSpriteData;
	int nPosCounter = 0;
	deque<CGoalItems*, CAllocator<CGoalItems*>>::iterator goalItemIter;
	goalItemIter = m_cGoalItemsPool.begin();
	while(goalItemIter != m_cGoalItemsPool.end())
	{
		// Get the type from the list
		pGIC = *goalItemIter;
		nItemType = pGIC->m_eType;

		// Get the list pos data
		tSpriteData = m_tListIconData[nPosCounter];

		// Get the type from the list
		pGIC = *goalItemIter;
		nItemType = pGIC->m_eType;

		if(m_bPlayerItems[nItemType])
		{
			// Find the Cell
			tSpriteData.m_tRect = CellAlgo(
				CHECK_LIST_CELL_IDX, LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// Apply it
			m_cListIconComps[nPosCounter]->SetSpriteData(tSpriteData);
			m_cListIconComps[nPosCounter]->SetActive(true);
		}

		// Inc Pos
		++nPosCounter;
		goalItemIter++;
	}

	// Sort the ranks
	for(int rank = 0; rank < 3; ++rank)
	{
		for(int rank2 = rank+1; rank2 < 4; ++rank2)
		{
			if(m_tPlayerInfo[rank].nScore < m_tPlayerInfo[rank2].nScore)
			{		
				swap(m_tPlayerInfo[rank], m_tPlayerInfo[rank2]);
			}
		}
	}	

	for(int player = 0; player < 4; ++player)
	{	
		char score[3] = { m_tPlayerInfo[player].nScore + '0', '/', '8' };

		m_tPlayerInfo[player].pScoreFontComps->SetWord(score);
		m_tPlayerInfo[player].pScoreFontComps->SetY(g_yRankPos[player]);

		TSpriteData tSprite = m_tPlayerInfo[player].pSpriteComp->GetSpriteData();
		tSprite.m_nY = 64 * player;
		m_tPlayerInfo[player].pSpriteComp->SetSpriteData(tSprite);
		//if(m_tPlayerInfo[player].nScore <
	}
}

void CHUDManager::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get values from the event
	CUpdateStateEvent* pUpdateEvent = static_cast<CUpdateStateEvent*>(pEvent);

	GetInstance()->Update();
	
}

void CHUDManager::GoalItemInitCallback(IEvent* pEvent, IComponent* pComp)
{
	// Static counter
	static int nCounter = 0;
	if(nCounter >= TOTAL_GOAL_ITEMS)
		return;

	// Get the Values from the Event
	CGoalItemEvent* pcObjEvent = static_cast<CGoalItemEvent*>(pEvent);
	CGoalItems* pGIC = pcObjEvent->GetGoalItem();

	if(pGIC)
	{
		// Save the Comp Ptr
		GetInstance()->m_cGoalItemsPool.push_front(pGIC);
		
		// Inc Counter
		++nCounter;
	}
}

void CHUDManager::BoostCallback(IEvent* pEvent, IComponent* pComp)
{
	GetInstance()->m_pEnergyDrink->SetActive(false);
}