////////////////////////////////////////////////////////
//  File			:	"CHUDManager.h"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/10/11
//
//	Last Changed	:	04/11/11
//
//  Purpose			:	Wrapper class for manageing HUD elements
////////////////////////////////////////////////////////
#ifndef _CHUDMANAGER_H_
#define _CHUDMANAGER_H_

#include <deque>
#include <map>
using namespace std;

#include "..\\..\\Memory Manager\\CAllocator.h"
#include "..\\..\\Rendering Managers\\Texture Managers\\CBitmapFont.h"
#include "..\\..\\Rendering Managers\\dxutil.h"
#include "CTextureManager.h"
#include "CBitmapFont.h"
#include "..\\..\\..\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

class CObjectManager;
class CObject;
class CGoalItems;

#define TOTAL_GOAL_ITEMS (9)

enum EHUDElement { HUD_MIN = -1, HUD_SHOPPINGLIST, HUD_MAX };

struct TPlayerHUDInfo
{
	int nID;
	int nScore;
	TSpriteData spriteData;
	CBitmapFontComp* pScoreFontComps;
	CSpriteComponent* pSpriteComp;
	DWORD dwColor;
};

struct TRadar
{
	VERTEX_POSCOLOR verts[4];
};

struct TShoppingListItem
{
	CSpriteComponent* pIconComponent;
	int nItemID;
	int nPlayerCollected; // -1 means not recently collected or collected by the human
	bool bSpawned; // spawned in level or not (bSpawned && nPlayerCollected > 0) means it's "sold out" (in level but got collected)

	bool operator==(TShoppingListItem other)
	{
		return (nItemID == other.nItemID) &&
			nPlayerCollected == other.nPlayerCollected &&
			bSpawned == other.bSpawned &&
			pIconComponent == other.pIconComponent;
	}
};

class CHUDManager
{
	// Singletons
	CTextureManager* m_pTM;
	CObjectManager*  m_pOM;


	// Inventory
	CSpriteComponent* m_pInventory;
	CSpriteComponent* m_pEnergyDrink;
	CSpriteComponent* m_pEnemyProgressComponent;
	CSpriteComponent* m_pRadarSpriteComponent;
	bool			  m_bHasHitThisUpdateBefore; //Turn icons on in gameplay bool
	
	// Scoreboard
	CObject* m_pEnemyProgress;
	TPlayerHUDInfo m_tPlayerInfo[4];

	// Radar
	TRadar m_tRadar;

	// Shopping List
	
	int m_nGoalIconsTex;

	int m_nShoppingListBackgroundTex;
	
		// Map of goal items in this game (stored associciated with nItemID)
	list<TShoppingListItem, CAllocator<TShoppingListItem>> 
			m_tGoalItemsDespawnedPool;
	list<TShoppingListItem, CAllocator<TShoppingListItem>> 
			m_tGoalItemsSpawnedPool;
	
	TSpriteData m_tListIconData[TOTAL_GOAL_ITEMS];
	int m_nGoalIconTextureIndices[TOTAL_GOAL_ITEMS]; // indices of picture in texture grid
	

	// Trilogy of Evil
	CHUDManager(void);
	~CHUDManager(void);
	CHUDManager& operator=(const CHUDManager&);

	// Helper Func
	void InitShoppingList(void);
	void InitEnemyProgress(void);
	void InitRadar(void);
	void InitInventory(void);
	RECT CellAlgo(int nID, int nNumCols,
						   int nCellWidth, int nCellHeight);

	int GetPlayerNum(CObject* player);

public:

	static CHUDManager* GetInstance(void) 
	{ 
		static CHUDManager pInstance;
		return &pInstance; 
	}

	void Init(void);

	void Render(void);

	// Callbacks
	void GoalItemSpawned(TGoalItemEvent* pcObjEvent);
	static void GoalItemSpawnedCallback(IEvent* pEvent, IComponent* pComp);
	
	void GoalItemDespawned(EGoalItemType eType);
	static void GoalItemDespawnedCallback(IEvent* pEvent, IComponent* pComp);

	void StartDespawnTimer(EGoalItemType eType);
	static void StartDespawnTimerCallback(IEvent* pEvent, IComponent* pComp);

	void GoalItemCollected(TGoalItemCollectedEvent* pcObjEvent);
	static void GoalItemCollectedCallback(IEvent* pEvent, IComponent* pComp);

	void Update(void);
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

	static void GoalItemInitCallback(IEvent* pEvent, IComponent* pComp);
	static void BoostCallback(IEvent* pEvent, IComponent* pComp);
	static void MainMenuEnterCallback(IEvent* pEvent, IComponent* pComp);
	static void GoalItemLostCallback(IEvent* pEvent, IComponent* pComp);

	static void GameplayStateEntered(IEvent* pEvent, IComponent* pComp);
	static void GameplayStateExited(IEvent* pEvent, IComponent* pComp);
	static void GameplayStateInit(IEvent* pEvent, IComponent* pComp);
	
	static void PlayerPickupItem(IEvent* pEvent, IComponent* pComp);

};

#endif // _CHUDMANAGER_H_