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
using std::deque;

#include "..\\..\\Memory Manager\\CAllocator.h"
#include "..\\..\\Rendering Managers\\Texture Managers\\CBitmapFont.h"
#include "..\\..\\Event Manager\\CGoalItemEvent.h"
//#include "..\\..\\..\\..\\Components\\Level\\CGoalItemComponent.h"
#include "CTextureManager.h"
#include "CBitmapFont.h"

class CObjectManager;
class CObject;
class CGoalItems;
class CGoalItemCollectedEvent;

#define TOTAL_GOAL_ITEMS (8)

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

class CHUDManager
{
	// Singletons
	CTextureManager* m_pTM;
	CObjectManager*  m_pOM;

	TPlayerHUDInfo m_tPlayerInfo[4];

	// Icon Objs
	CSpriteComponent* m_cListIconComps[TOTAL_GOAL_ITEMS];
	CObject* m_pEnemyProgress;

	// Icon Idxs
	int m_nGoalIconsTex;
	int m_nGoalIconIdxs[TOTAL_GOAL_ITEMS];
	
	deque<CGoalItems*, CAllocator<CGoalItems*>> m_cGoalItemsPool;
	bool m_bPlayerItems[TOTAL_GOAL_ITEMS];
	
	TSpriteData m_tListIconData[TOTAL_GOAL_ITEMS];

	// Inventory
	CSpriteComponent* m_pInventory;
	CSpriteComponent* m_pEnergyDrink;
	CSpriteComponent* m_pEnemyProgressComponent;
	CSpriteComponent* m_pRadarSpriteComponent;

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

public:

	static CHUDManager* GetInstance(void) 
	{ 
		static CHUDManager pInstance;
		return &pInstance; 
	}

	void Init(void);

	void Render(void);

	// Callbacks
	void GoalItemSpawned(CGoalItemEvent* pcObjEvent);
	static void GoalItemSpawnedCallback(IEvent* pEvent, IComponent* pComp);
	
	void GoalItemDespawned(EGoalItemType eType);
	static void GoalItemDespawnedCallback(IEvent* pEvent, IComponent* pComp);

	void StartDespawnTimer(EGoalItemType eType);
	static void StartDespawnTimerCallback(IEvent* pEvent, IComponent* pComp);

	void GoalItemCollected(CGoalItemCollectedEvent* pcObjEvent);
	static void GoalItemCollectedCallback(IEvent* pEvent, IComponent* pComp);

	void Update(void);
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

	static void GoalItemInitCallback(IEvent* pEvent, IComponent* pComp);
	static void BoostCallback(IEvent* pEvent, IComponent* pComp);
};

#endif // _CHUDMANAGER_H_