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
#include "..\\..\\Event Manager\\EventStructs.h"
#include "..\\..\\..\\Component Managers\\CSpawningManager.h"
#include "..\\..\\..\\..\\Components\\Level\\CGoalItemComponent.h"

#define LIST_NUM_COL		(4)
#define LIST_CELL_WIDTH		(64)
#define LIST_CELL_HEIGHT	(64)
#define ENEMY_PROGRESS_X	(895)
#define CHECK_LIST_CELL_IDX (27)
#define SOLD_OUT_CELL_IDX	(21)

static const int g_yRankPos[4] = { 38, 100, 165, 228 };


CHUDManager::CHUDManager(void) : m_pEnemyProgress(NULL), m_pTM(NULL)
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

int CHUDManager::GetPlayerNum(CObject* player)
{
	if( 0 == _stricmp((char*)player->GetID(), "Player") )
	{
		return 0;
	}

	if( 0 == _stricmp((char*)player->GetID(), "Agent1") )
	{
		return 1;
	}

	if( 0 == _stricmp((char*)player->GetID(), "Agent2") )
	{
		return 2;
	}

	if( 0 == _stricmp((char*)player->GetID(), "Agent3") )
	{
		return 3;
	}

	return -1;
}

void CHUDManager::InitShoppingList(void)
{
	// Load Sprite Sheet
	m_nGoalIconsTex = m_pTM->LoadTexture("Resource\\HUD\\FFP_2D_GoalItems_FIN.png");
	m_nShoppingListBackgroundTex = m_pTM->LoadTexture("Resource\\HUD\\T_Biker_Shopping_List_D.png");

	// Idxs
	m_nGoalIconTextureIndices[GORILLA_MILK] = 4; 
	m_nGoalIconTextureIndices[CAPTAIN_FLAKEY] = 2; 
	m_nGoalIconTextureIndices[AGENT_ORANGE] = 5; 
	m_nGoalIconTextureIndices[BAKON] = 7; 
	m_nGoalIconTextureIndices[KILLER_TOMATO_SOUP] = 8; 
	m_nGoalIconTextureIndices[SAD_APPLE] = 6; 
	m_nGoalIconTextureIndices[FRENCHY_BREAD] = 3; 
	m_nGoalIconTextureIndices[SAMMY_SANDWICH] = 0; 
	m_nGoalIconTextureIndices[TV_DINNER] = 1; 

	// Setup Sprite Data
	
		// Bottom
		m_tListIconData[7].m_nTexture = m_nGoalIconsTex;
		m_tListIconData[7].m_nX = 70;
		m_tListIconData[7].m_nY = 670;
		m_tListIconData[7].m_nZ = 1;
		m_tListIconData[7].m_fScaleX = .5f;
		m_tListIconData[7].m_fScaleY = .5f;
		m_tListIconData[7].m_fRotCenterX = 0.0f;
		m_tListIconData[7].m_fRotCenterY = 0.0f;
		m_tListIconData[7].m_fRot = 0.0f;
		m_tListIconData[7].m_dwColor = -1;
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
		m_tListIconData[3].m_fScaleX = .9f;
		m_tListIconData[3].m_fScaleY = .9f;
		
		m_tListIconData[2] = m_tListIconData[3];
		m_tListIconData[2].m_nX += 55;
		
		m_tListIconData[1] = m_tListIconData[3];
		m_tListIconData[1].m_nY += 44;
		m_tListIconData[1].m_fScaleX = 1.0f;
		m_tListIconData[1].m_fScaleY = 1.0f;

		m_tListIconData[0] = m_tListIconData[3];
		m_tListIconData[0].m_nX += 55;
		m_tListIconData[0].m_nY += 44;

	// Create the Icon Objs
	CObject* pObj;
	TSpriteData tSpriteData;
	for(unsigned int uIcon = 0; uIcon < TOTAL_GOAL_ITEMS; ++uIcon)
	{
		// Creat the object
		string szObjID = "Icon";
		szObjID += uIcon;
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
		m_tGoalItemsDespawnedPool.push_back(tItemInfo);
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
	m_pEnemyProgressComponent = m_pTM->CreateSpriteComp(m_pEnemyProgress, tSpriteData, false);

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
		tSpriteData.m_nY = bit * 64;
		tSpriteData.m_nZ = 2;
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

	m_tRadar.verts[0].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_tRadar.verts[1].position = D3DXVECTOR3(100.0f, 0.0f, 0.0f);
	m_tRadar.verts[2].position = D3DXVECTOR3(100.0f, 100.0f, 0.0f);
	m_tRadar.verts[3].position = D3DXVECTOR3(0.0f, 100.0f, 0.0f);

	m_tRadar.verts[0].color = D3DXCOLOR(255, 0.0f, 0.0f, 255.0f);
	m_tRadar.verts[1].color = D3DXCOLOR(255, 0.0f, 0.0f, 255.0f);
	m_tRadar.verts[2].color = D3DXCOLOR(255, 0.0f, 0.0f, 255.0f);
	m_tRadar.verts[3].color = D3DXCOLOR(255, 0.0f, 0.0f, 255.0f);
	

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

	m_bHasHitThisUpdateBefore = false; // so the hud elements show up when gameplay enters

	int eAssociatedState = GAMEPLAY_STATE;

	

	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("GoalItemSpawned",
		(IComponent*)GetInstance(), GoalItemSpawnedCallback);

	pEM->RegisterEvent("GoalItemDespawned",
		(IComponent*)GetInstance(), GoalItemDespawnedCallback);

	pEM->RegisterEvent("GoalItemCollected",	(IComponent*)GetInstance(), GoalItemCollectedCallback);
	pEM->RegisterEvent("StateChange", (IComponent*)GetInstance(), MainMenuEnterCallback);

	string szEventName = "Update";
	szEventName += eAssociatedState; // catch update for only gameplay state
	pEM->RegisterEvent(szEventName,	(IComponent*)GetInstance(), UpdateCallback);

	pEM->RegisterEvent("GoalItemInit", (IComponent*)GetInstance(), GoalItemInitCallback);
	pEM->RegisterEvent("Boost",	(IComponent*)GetInstance(), BoostCallback);
	pEM->RegisterEvent("ItemDropped", (IComponent*)GetInstance(), GoalItemLostCallback);
	pEM->RegisterEvent("IncrementRankScore", (IComponent*)GetInstance(), PlayerPickupItem);

	szEventName = "EnableObjects";
	szEventName += (char)eAssociatedState;
	pEM->RegisterEvent(szEventName, (IComponent*)GetInstance(), GameplayStateEntered);

	szEventName = "DisableObjects";
	szEventName += (char)eAssociatedState;
	pEM->RegisterEvent(szEventName, (IComponent*)GetInstance(), GameplayStateExited);

	szEventName = "InitObjects";
	szEventName += (char)eAssociatedState;
	pEM->RegisterEvent(szEventName, (IComponent*)GetInstance(), GameplayStateInit);
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
void CHUDManager::GoalItemSpawned(TGoalItemEvent* pcObjEvent)
{
	// Update the List
	int nSpawnedType = pcObjEvent->m_eGoalItemType;

	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsDespawnedPool.begin();

	// swap what we spawned into the new slot
	while(goalItemIter != m_tGoalItemsDespawnedPool.end())
	{
		// if the one we're on is the item that spawned
		if(nSpawnedType == goalItemIter->nItemID)
		{
			TShoppingListItem toMove = *goalItemIter;

			m_tGoalItemsSpawnedPool.push_back(toMove);
			m_tGoalItemsDespawnedPool.erase(goalItemIter);
			break; // move the item that spawned. Stop looping to prevent double swaps
		}

		// Inc Pos
		goalItemIter++;
	}/**/
}

void CHUDManager::GoalItemSpawnedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	GetInstance()->GoalItemSpawned((TGoalItemEvent*)pEvent->GetData());
}

void CHUDManager::GoalItemDespawned(EGoalItemType eType)
{
	// Update the List
	int nSpawnedType = eType;

	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsSpawnedPool.begin();

	// swap what we spawned into the new slot
	while(goalItemIter != m_tGoalItemsSpawnedPool.end())
	{
		// if the one we're on is the item that despawned
		if(nSpawnedType == goalItemIter->nItemID)
		{
			TShoppingListItem toMove = *goalItemIter;

			m_tGoalItemsSpawnedPool.erase(goalItemIter);
			m_tGoalItemsDespawnedPool.push_back(toMove);
			break; // move the item that spawned. Stop looping to prevent double swaps
		}

		// Inc Pos
		goalItemIter++;
	}/**/
}

void CHUDManager::GoalItemDespawnedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	TGoalItemEvent* pcObjEvent = (TGoalItemEvent*)pEvent->GetData();

	EGoalItemType eType = pcObjEvent->m_eGoalItemType;
	
	GetInstance()->GoalItemDespawned(eType);
}

void CHUDManager::StartDespawnTimer(EGoalItemType eType)
{
	// Find a spot
	static bool bFirst = true;
	int uIdx = (bFirst) ? 0 : 1;

	// set a var to maybe blink or something if necessary?

	// toggle Pos
	bFirst = !bFirst;
}

void CHUDManager::StartDespawnTimerCallback(IEvent* pEvent, IComponent* pComp)
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
	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsSpawnedPool.begin();
	while(goalItemIter != m_tGoalItemsSpawnedPool.end())
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
}

void CHUDManager::GoalItemCollectedCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	TGoalItemCollectedEvent* pcObjEvent = (TGoalItemCollectedEvent*)pEvent->GetData();
	GetInstance()->GoalItemCollected(pcObjEvent);
}

void CHUDManager::Update(void)
{
	// Upate List
	int nItemType;
	TSpriteData tSpriteData;
	int nPosCounter = 0; // in loop counter
	int nCurrentIcon = 0; // counter for all the loops (doesn't reset)

	// Put correct marks on things that need it
	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator goalItemIter;
	goalItemIter = m_tGoalItemsSpawnedPool.begin();
	while(goalItemIter != m_tGoalItemsSpawnedPool.end())
	{
		// get the current iter's item type
		nItemType = goalItemIter->nItemID;

		// Get the list pos data
		tSpriteData = m_tListIconData[nCurrentIcon];

		//Set the position to the next spot on this list's section
		RECT pos = CellAlgo(nPosCounter, 2, 44,44);
		tSpriteData.m_nX = 65 + pos.left;
		tSpriteData.m_nY = 547 + pos.top;
		tSpriteData.m_fScaleX = .9f;
		tSpriteData.m_fScaleY = .9f;

		// if the item is collected by the player
		if(goalItemIter->nPlayerCollected == 0)
		{
			// Find the Cell for Check Mark
			tSpriteData.m_tRect = CellAlgo(
				CHECK_LIST_CELL_IDX, LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// Apply the checkmark
			goalItemIter->pIconComponent->SetSpriteData(tSpriteData);
			goalItemIter->pIconComponent->SetActive(true);
		}
		// if no one's picked it up
		else if(goalItemIter->nPlayerCollected == -1)
		{
			// Find the Cell for current icon
			tSpriteData.m_tRect = CellAlgo(
				m_nGoalIconTextureIndices[goalItemIter->nItemID], LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// Apply the normal icon
			goalItemIter->pIconComponent->SetSpriteData(tSpriteData);
			goalItemIter->pIconComponent->SetActive(true);
		}
		else // AI picked it up
		{
			// TODO: Make this into "SOLD OUT"
			// Find the Cell for current icon
			tSpriteData.m_tRect = CellAlgo(
				SOLD_OUT_CELL_IDX, LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// Apply the normal icon
			goalItemIter->pIconComponent->SetSpriteData(tSpriteData);
			goalItemIter->pIconComponent->SetActive(true);
		}

		// Inc Pos
		++nPosCounter;
		++nCurrentIcon;
		goalItemIter++;
	}

	// Draw the despawned pool
	nPosCounter = 0;
	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator goalItemIterDespawned;
	goalItemIterDespawned = m_tGoalItemsDespawnedPool.begin();
	while(goalItemIterDespawned != m_tGoalItemsDespawnedPool.end())
	{
		// get the current iter's item type
		nItemType = goalItemIterDespawned->nItemID;

		// Get the list pos data
		tSpriteData = m_tListIconData[nCurrentIcon];

		//Set the position to the next spot on this list's section
		RECT pos = CellAlgo(nPosCounter, 4, 22, 22);
		tSpriteData.m_nX = 70 + pos.left;
		tSpriteData.m_nY = 670 + pos.top;
		tSpriteData.m_fScaleX = .5f;
		tSpriteData.m_fScaleY = .5f;


		// if the item is collected by the player
		if(goalItemIterDespawned->nPlayerCollected == 0)
		{
			// Find the Cell for Check Mark
			tSpriteData.m_tRect = CellAlgo(
				CHECK_LIST_CELL_IDX, LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// Apply the checkmark
			goalItemIterDespawned->pIconComponent->SetSpriteData(tSpriteData);
			goalItemIterDespawned->pIconComponent->SetActive(true);
		}
		// if no one's picked it up
		else if(goalItemIterDespawned->nPlayerCollected == -1)
		{
			// Find the Cell for current icon
			tSpriteData.m_tRect = CellAlgo(
				m_nGoalIconTextureIndices[goalItemIterDespawned->nItemID], LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// Apply the normal icon
			goalItemIterDespawned->pIconComponent->SetSpriteData(tSpriteData);
			goalItemIterDespawned->pIconComponent->SetActive(true);
		}
		else // AI picked it up
		{
			// TODO: Make this into "SOLD OUT"
			// Find the Cell for current icon
			tSpriteData.m_tRect = CellAlgo(
				m_nGoalIconTextureIndices[goalItemIterDespawned->nItemID], LIST_NUM_COL,
				LIST_CELL_WIDTH, LIST_CELL_HEIGHT);

			// Apply the normal icon
			goalItemIterDespawned->pIconComponent->SetSpriteData(tSpriteData);
			goalItemIterDespawned->pIconComponent->SetActive(true);
		}

		// Inc Pos
		++nPosCounter;
		++nCurrentIcon;
		goalItemIterDespawned++;
	}

	// Sort the ranks/scoreboard
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
		m_tPlayerInfo[player].pScoreFontComps->SetActive(true);

		TSpriteData tSprite = m_tPlayerInfo[player].pSpriteComp->GetSpriteData();
		tSprite.m_nY = 64 * player;
		tSprite.m_nZ = 5;
		m_tPlayerInfo[player].pSpriteComp->SetSpriteData(tSprite);
		m_tPlayerInfo[player].pSpriteComp->SetActive(true);
		//if(m_tPlayerInfo[player].nScore <
	}
}

void CHUDManager::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get values from the event

	GetInstance()->Update();
	
}

void CHUDManager::GoalItemInitCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	TGoalItemEvent* pcObjEvent = (TGoalItemEvent*)pEvent->GetData();

	int nSpawnedItemID = pcObjEvent->m_eGoalItemType;

	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator iter;
	iter = GetInstance()->m_tGoalItemsDespawnedPool.begin();

	// for each slot in the list
	while(iter != GetInstance()->m_tGoalItemsDespawnedPool.end())
	{
		// if that slot doesn't have an item yet
		if(iter->nItemID == -1)
		{
			// set up its data
			iter->bSpawned = true;
			iter->nItemID = nSpawnedItemID;
			iter->nPlayerCollected = -1;
			//iter->pIconComponent->SetActive(true);

			// init the UVs for this particular item id
			RECT tUVRect(GetInstance()->CellAlgo(
				GetInstance()->m_nGoalIconTextureIndices[nSpawnedItemID],4, 64, 64));		
			iter->pIconComponent->GetSpriteData().m_tRect = tUVRect;

			break; // get out of here 'cause we added it
		}
		iter++;
	}
}

void CHUDManager::BoostCallback(IEvent* pEvent, IComponent* pComp)
{
	GetInstance()->m_pEnergyDrink->SetActive(false);
}

void CHUDManager::MainMenuEnterCallback(IEvent* pEvent, IComponent* pComp)
{
	TStateEvent* cEvent = (TStateEvent*)pEvent->GetData();

	if(cEvent->m_eToState == MAIN_MENU_STATE)
	{
		// turn off all the hud elements while in main menu
		GetInstance()->m_pInventory->SetActive(false);
		GetInstance()->m_pEnergyDrink->SetActive(false);
		GetInstance()->m_pEnemyProgressComponent->SetActive(false);
		GetInstance()->m_pRadarSpriteComponent->SetActive(false);

		for(int i=0; i<4; i++)
		{
			GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetActive(false);
			GetInstance()->m_tPlayerInfo[i].pScoreFontComps->SetActive(false);
		}

		list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator iter;
		iter = GetInstance()->m_tGoalItemsDespawnedPool.begin();
		while(iter != GetInstance()->m_tGoalItemsDespawnedPool.end()) // TODO: Add spawned pool too
		{
			iter->pIconComponent->SetActive(false);
		}

		GetInstance()->m_bHasHitThisUpdateBefore = false; // 

	}
}

void CHUDManager::GoalItemLostCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get the Values from the Event
	/*TGoalItemCollectedEvent* pcObjEvent = static_cast<TGoalItemCollectedEvent*>(pEvent->GetData());
	
	int nDropper = GetInstance()->GetPlayerNum(pcObjEvent->m_pcCollector);
	int nType = (int)pcObjEvent->m_pcGoalItem;

	// if it's the human
	if(nDropper == 0)
	{
		// Find the item in the shopping list
		list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator goalItemIter;
		goalItemIter = GetInstance()->m_tGoalItemsPool.begin();
		while(goalItemIter != GetInstance()->m_tGoalItemsPool.end())
		{
			// if we found the item that got dropped
			if(goalItemIter->nItemID == nType)
			{
				// say the item got dropped
				goalItemIter->nPlayerCollected = -1;
				break; // only one in the list needs to be updated (only one exists, even if it didnt...)
			}

			goalItemIter++;
		}
	}

	// Scoreboard: if teh player and type are valid
	if(nDropper < 4 && nType < TOTAL_GOAL_ITEMS)
	{
		// lower score for that player
		GetInstance()->m_tPlayerInfo[nDropper].nScore--;

		// make sure it's not below zero
		if(GetInstance()->m_tPlayerInfo[nDropper].nScore <= 0)
		{
			GetInstance()->m_tPlayerInfo[nDropper].nScore = 0;
		}

		// reflect the item got dropped
		//GetInstance()->m_bPlayerItems[nDropper][nType] = false;
	}/**/
}


void CHUDManager::GameplayStateEntered(IEvent* pEvent, IComponent* pComp)
{
	// show all hud elements
	GetInstance()->m_pInventory->SetActive(true);
	GetInstance()->m_pEnergyDrink->SetActive(true);
	GetInstance()->m_pEnemyProgressComponent->SetActive(true);
	GetInstance()->m_pRadarSpriteComponent->SetActive(true);

	for(int i=0; i<4; i++)
	{
		GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetActive(true);
		GetInstance()->m_tPlayerInfo[i].pScoreFontComps->SetActive(true);
	}

	
	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator iter;
	iter = GetInstance()->m_tGoalItemsDespawnedPool.begin();

	while(iter != GetInstance()->m_tGoalItemsDespawnedPool.end())
	{
		if(iter->pIconComponent)
			iter->pIconComponent->SetActive(true);
		iter++;
	}

	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator iter2;
	iter2 = GetInstance()->m_tGoalItemsSpawnedPool.begin();

	while(iter2 != GetInstance()->m_tGoalItemsSpawnedPool.end())
	{
		if(iter2->pIconComponent)
			iter2->pIconComponent->SetActive(true);
		iter2++;
	}

}

void CHUDManager::GameplayStateExited(IEvent* pEvent, IComponent* pComp)
{
	// Hide all hud elements
	GetInstance()->m_pInventory->SetActive(false);
	GetInstance()->m_pEnergyDrink->SetActive(false);
	GetInstance()->m_pEnemyProgressComponent->SetActive(false);
	GetInstance()->m_pRadarSpriteComponent->SetActive(false);

	for(int i=0; i<4; i++)
	{
		GetInstance()->m_tPlayerInfo[i].pSpriteComp->SetActive(false);
		GetInstance()->m_tPlayerInfo[i].pScoreFontComps->SetActive(false);
	}

	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator iter;
	iter = GetInstance()->m_tGoalItemsDespawnedPool.begin();

	while(iter != GetInstance()->m_tGoalItemsDespawnedPool.end())
	{
		if(iter->pIconComponent)
			iter->pIconComponent->SetActive(false);
		iter++;
	}

	list<TShoppingListItem, CAllocator<TShoppingListItem>>::iterator iter2;
	iter2 = GetInstance()->m_tGoalItemsSpawnedPool.begin();

	while(iter2 != GetInstance()->m_tGoalItemsSpawnedPool.end())
	{
		if(iter2->pIconComponent)
			iter2->pIconComponent->SetActive(false);
		iter2++;
	}


}

void CHUDManager::GameplayStateInit(IEvent* pEvent, IComponent* pComp)
{
	GameplayStateEntered(pEvent, pComp);

	for(int i=0; i<4; i++)
	{
		CHUDManager::GetInstance()->m_tPlayerInfo[i].nScore = 0;

	}

}


void CHUDManager::PlayerPickupItem(IEvent* pEvent, IComponent* pComp)
{
	TObjectEvent* tEvent = (TObjectEvent*)pEvent->GetData();
	
	int nPlayerNum = GetInstance()->GetPlayerNum(tEvent->m_pcObj);

	// if it's a valid player
	if(nPlayerNum < 4) 
	{
		// increment their score
		GetInstance()->m_tPlayerInfo[nPlayerNum].nScore++;
	}
}

