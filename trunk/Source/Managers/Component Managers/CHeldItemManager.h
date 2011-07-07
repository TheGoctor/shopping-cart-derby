///////////////////////////////////////////////////////////////////////////////
//  File			:	"CHeldItemManager.h"
//
//  Author			:	Mac Smith (MS)
//
//  Date Created	:	05/19/11
//
//	Last Changed	:	05/19/11
//
//  Purpose			:	A manager for HeldItem components
///////////////////////////////////////////////////////////////////////////////
#ifndef _CHELDITEMMANAGER_H_
#define _CHELDITEMMANAGER_H_

// Includes
#include <list>
using std::list;

// LUA
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// Includes
#include "..\\Global Managers\\Memory Manager\\CAllocator.h"
#include "..\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Enums.h"
#include "..\..\Components\HeldItems\Donut\Donut.h"
#include "..\..\Components\HeldItems\Turkey\CTurkeyComponent.h"
#include "..\..\Components\HeldItems\Energy Drink\EnergyDrink.h"
#include "..\..\Components\HeldItems\Banana\CBanana.h"
#include "..\..\Components\HeldItems\Chicken Soup\CChickenSoup.h"
#include "..\..\Components\HeldItems\Cream Pie\CreamPie.h"
#include "..\..\Components\HeldItems\Jam\Jam.h"
#include "..\..\Components\HeldItems\PeanutButter\PeanutButter.h"



// Foward Declares
class IEvent;
class CPie;
class CBanana;

class CHeldItemManager
{
private:
	
	// Components - LISTS BE HERE
	list<CTurkeyComponent*, CAllocator<CTurkeyComponent*>> m_lTurkeyComps;
	int nTurkeysCreated;

	list<CDonut*, CAllocator<CDonut*>> m_lDonutComps;
	int nDonutsCreated;

	list<CEnergyDrink*, CAllocator<CEnergyDrink*>> m_lEnergyDrinkComps;
	int nEnergyDrinksCreated;

	list<CBanana*, CAllocator<CBanana*>> m_lBananaComps;
	int nBananasCreated;

	list<CChickenSoupComponent*, CAllocator<CChickenSoupComponent*>> m_lChickenSoupComps;
	int nSoupsCreated;

	list<CPie*, CAllocator<CPie*>> m_lPieComps;
	int nPiesCreated;
	
	list<CRayJam*, CAllocator<CRayJam*>> m_lJamComps;
	int nJamsCreated;

	list<CPeanutButter*, CAllocator<CPeanutButter*>> m_lPeanutButterComps;
	int nPeanutButtersCreated;

	unsigned int m_nRenderContexts[MAX_GOAL_ITEMS];

	// Trilogy of Evil
	CHeldItemManager(void)
	{
	}
	CHeldItemManager(CHeldItemManager &ref);

public:
	// Destructor
	~CHeldItemManager(void)
	{
	}
	
	// Init
	void Init(void);

	inline static CHeldItemManager* GetInstance(void)
	{
		static CHeldItemManager cHeldItemManager;
		return &cHeldItemManager;
	}

	static void UsePieCallback(IEvent* cEvent, IComponent* cCenter);
	static void UseTurkeyCallback(IEvent* cEvent, IComponent* cCenter);
	static void UseBoostCallback(IEvent* cEvent, IComponent* cCenter);
	static void UseBananaCallback(IEvent* cEvent, IComponent* cCenter);
	static void UsePeanutButterCallback(IEvent* cEvent, IComponent* cCenter);
	static void UseSoupCallback(IEvent* cEvent, IComponent* cCenter);
	static void UseDonutCallback(IEvent* cEvent, IComponent* cCenter);
	static void UseRayJamCallback(IEvent* cEvent, IComponent* cCenter);
	
	static void ShutdownCallback(IEvent* cEvent, IComponent* cCenter);
	
	CTurkeyComponent* GetATurkey();
	CBanana* GetABanana();
	CChickenSoupComponent* GetAChickenSoup();
	CEnergyDrink* GetAnEnergyDrink();
	CPie* GetAPie();
	CPeanutButter* GetAPeanutButter();
	CDonut* GetADonut();
	CRayJam* GetARayJam();


};

#endif