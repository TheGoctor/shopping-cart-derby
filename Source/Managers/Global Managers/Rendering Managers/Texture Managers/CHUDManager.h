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
#include <D3dx9math.h>
#include "CTextureManager.h"
#include "CBitmapFont.h"
#include "..\\..\\..\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

class CObjectManager;
class CObject;
class CGoalItems;


enum EHUDElement { HUD_MIN = -1, HUD_SHOPPINGLIST, HUD_MAX };

struct TPlayerHUDInfo
{
	int nID;
	int nScore;
	TSpriteData spriteData;
	CBitmapFontComp* pScoreFontComps;
	CSpriteComponent* pSpriteComp;
	CSpriteComponent* pBackgroundSpriteComp;
	CSpriteComponent* pRankNumberComponent;
	CSpriteComponent* pRankNumberBackgroundComponent;

	DWORD dwColor;
};

struct TRadar
{
	VERTEX_POSCOLOR verts[4];
};

struct TRadarNode
{
	bool bSpawned; // tells if we're showing the object or not
	int nGoalItemType; // the type of item spawned
	D3DXVECTOR3 vTargetPosition; // the location of the spawned object (as opposed to an object which could cause problems)

	CSpriteComponent* pIconSpriteComponent;
	CSpriteComponent* pBackgroundSpriteComponent;
};

struct TPlayerRadarNodeInfo
{
	CObject* pPlayer;
	int nCorrespondingGoalItemType;
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
	CSpriteComponent* m_pHeldItem1;
	CSpriteComponent* m_pHeldItem2;
	CSpriteComponent* m_pButtonItem1;
	CSpriteComponent* m_pButtonItem2;
	bool			  m_bHasHitThisUpdateBefore; //Turn icons on in gameplay bool
	CBitmapFontComp* m_pKey1Comp;
	CBitmapFontComp* m_pKey2Comp;
	int m_nRankNumberTextureIDs[4];
	
	int m_nEnergyDrinkID;

	bool m_bShowHeld1, m_bShowHeld2;
	
	// Scoreboard
	CObject* m_pEnemyProgress;
	TPlayerHUDInfo m_tPlayerInfo[4];
	CSpriteComponent*	m_pEnemyProgressComponent;
	CSpriteComponent*	m_pRankNumberBackgroundComponent;
	
	// Shopping List
	int m_nGoalIconsTex;
	int m_nGoalIconsDarkenedTex;
	int m_nGoalIconsCheckMarkTex;
	int m_nShoppingListBackgroundTex[5];

	//PLAYER CHARACTER
	int m_nPlayer[4];
	int m_nPlayer1Char;
	int m_nPlayer2Char;
	int m_nPlayer3Char;
	int m_nPlayer4Char;

	
	
		// check mark stuff
	TSpriteData m_tCheckMarkSpriteInfo;
	CSpriteComponent* m_pCheckMarkSpriteComp[MAX_GOAL_ITEMS]; // HACK: 8 for num items spawned in level

		// Shopping list background
	CSpriteComponent* m_pShoppingListBackgroundSpriteComp;

		// Map of goal items in this game (stored associciated with nItemID)
	list<TShoppingListItem, CAllocator<TShoppingListItem>> 
			m_tGoalItemsSpawnedPool;
	TSpriteData m_tListIconData[MAX_GOAL_ITEMS];
	
	list<TPlayerRadarNodeInfo, CAllocator<TPlayerRadarNodeInfo>> 
		m_tPlayersGoingToCheckout;

	// New radar
	list<TRadarNode, CAllocator<TRadarNode>> 
		m_lRadarNodes; // inits in GoalItemInit
	int			m_nRadarNodeTexture;
	

	// Objects on minimap
	CObject* m_pPlayerObject;

	
	TSpriteData m_tItemMiniMap[MAX_GOAL_ITEMS];
	int m_nGoalIconTextureIndices[MAX_GOAL_ITEMS]; // indices of picture in texture grid
	//List of cart icons
	TSpriteData	m_tListIconDataMap[MAX_GOAL_ITEMS];
	TSpriteData m_tListofCarts[1];
	

	// Trilogy of Evil
	CHUDManager(void);
	~CHUDManager(void);
	CHUDManager& operator=(const CHUDManager&);

	// Helper Func
	void InitShoppingList(void);
	void InitEnemyProgress(void);
	void InitMiniMap(void);
	void UpdateMiniMap(void);
	void InitInventory(void);
	RECT CellAlgo(int nID, int nNumCols,
						   int nCellWidth, int nCellHeight);

	


public:

	static CHUDManager* GetInstance(void) 
	{ 
		static CHUDManager pInstance;
		return &pInstance; 
	}
int GetPlayerNum(CObject* player);
	void Init(void);
	
	void Render(void);

	//Mutator for character select
	//Player1
	void SetPlayer1Char(int character)
	{
		character = m_nPlayer1Char;
	}
	//Player2
	void SetPlayer2Char(int character)
	{
		character = m_nPlayer2Char;
	}
	//Player3
	void SetPlayer3Char(int character)
	{
		character = m_nPlayer3Char;
	}
	//Player4
	void SetPlayer4Char(int character)
	{
		character = m_nPlayer4Char;
	}
	//Accessors for character select
	//Player1
	int GetPlayer1Char()
	{
		return m_nPlayer1Char; 
	}
	//Player2
	int GetPlayer2Char()
	{
		return m_nPlayer2Char; 

	}
	//Player3
	int GetPlayer3Char()
	{
		return m_nPlayer3Char; 

	}
	//Player4
	int GetPlayer4Char()
	{
		return m_nPlayer4Char; 
	}

	int GetPlayerCharID(int playerNum);

	// Callbacks
	void RenderAiAgentsOnMiniMap(TObjectEvent* pcObjEvent);
	static void RenderAiAgentsOnMiniMapCallback(IEvent* pEvent, IComponent* pComp);
	void UpdateAIAgentPos(void);

	void GoalItemSpawned(TGoalItemEvent* pcObjEvent);
	static void GoalItemSpawnedCallback(IEvent* pEvent, IComponent* pComp);
	
	void GoalItemDespawned(EGoalItemType eType);
	static void GoalItemDespawnedCallback(IEvent* pEvent, IComponent* pComp);

	void StartDespawnTimer(EGoalItemType eType);
	static void StartDespawnTimerCallback(IEvent* pEvent, IComponent* pComp);

	void GoalItemCollected(TGoalItemCollectedEvent* pcObjEvent);
	static void GoalItemCollectedCallback(IEvent* pEvent, IComponent* pComp);
	
	static void PickupItemCollectedCallback(IEvent* pEvent, IComponent* pComp);

	static void HeldItemCollectedCallBack(IEvent*, IComponent*);
	void HeldItemCollected(THeldItemCollected*);
	void HeldItemIDCollected(EHeldItemType pHeldItem);

	static void UseHeldItem1(IEvent*, IComponent*);
	static void UseHeldItem2(IEvent*, IComponent*);
	void UseHeldItem(int);

	void SendStolenItemEvent(int nSlot);

	void Update(void);
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

	static void GoalItemInitCallback(IEvent* pEvent, IComponent* pComp);
	static void MainMenuEnterCallback(IEvent* pEvent, IComponent* pComp);
	static void GoalItemLostCallback(IEvent* pEvent, IComponent* pComp);

	static void GameplayStateEntered(IEvent* pEvent, IComponent* pComp);
	static void GameplayStateExited(IEvent* pEvent, IComponent* pComp);
	static void GameplayStateInit(IEvent* pEvent, IComponent* pComp);
	
	static void PlayerPickupItem(IEvent* pEvent, IComponent* pComp);

	static void PlayerLocation(IEvent* pEvent, IComponent* pComp);
	static void RaceStarted(IEvent* pEvent, IComponent* pComp);

	static void DisableImages(IEvent* pEvent, IComponent* pComp);
	
	static void InitCheckoutLocation(IEvent* pEvent, IComponent* pComp);


	// Lua stuff
	static int SetCharacterPicked(lua_State* pLua);
	
};

#endif // _CHUDMANAGER_H_