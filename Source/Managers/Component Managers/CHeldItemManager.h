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

	////////////////////////////////////////////////////////////////////////////////
	// Function: Init
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		Initialized all the data members in this class.
	////////////////////////////////////////////////////////////////////////////////
	void Init(void);

	////////////////////////////////////////////////////////////////////////////////
	// Function: GetInstance
	//
	// Return:  
	//		CHeldItemManager* - the instance of this singleton
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		First call creates this beautiful creature. Every call gives it to you.
	////////////////////////////////////////////////////////////////////////////////
	inline static CHeldItemManager* GetInstance(void)
	{
		static CHeldItemManager cHeldItemManager;
		return &cHeldItemManager;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Function: UsePieCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		Listens for the use pie event and creates and initializes data
	//		for the pie to be shot.
	////////////////////////////////////////////////////////////////////////////////
	static void UsePieCallback(IEvent* cEvent, IComponent* cCenter);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: UseTurkeyCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		Listens for the use turkey event and creates and initializes data
	//		for the turkey to be shot.
	////////////////////////////////////////////////////////////////////////////////
	static void UseTurkeyCallback(IEvent* cEvent, IComponent* cCenter);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: UseBoostCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		Listens for the use boost event and creates and initializes data
	//		for the boost to be used.
	////////////////////////////////////////////////////////////////////////////////
	static void UseBoostCallback(IEvent* cEvent, IComponent* cCenter);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: UseBananaCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		Listens for the use banana event and creates and initializes data
	//		for the banana to be used.
	////////////////////////////////////////////////////////////////////////////////
	static void UseBananaCallback(IEvent* cEvent, IComponent* cCenter);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: UsePeanutButterCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		Listens for the use pb event and creates and initializes data
	//		for the pb to be shot.
	////////////////////////////////////////////////////////////////////////////////
	static void UsePeanutButterCallback(IEvent* cEvent, IComponent* cCenter);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: UseSoupCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		Listens for the use soup event and creates and initializes data
	//		for the soup to be used.
	////////////////////////////////////////////////////////////////////////////////
	static void UseSoupCallback(IEvent* cEvent, IComponent* cCenter);
	
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: UseDonutCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		Listens for the use donut event and creates and initializes data
	//		for the donut to be used.
	////////////////////////////////////////////////////////////////////////////////
	static void UseDonutCallback(IEvent* cEvent, IComponent* cCenter);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: UseJamCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		Listens for the use jam event and creates and initializes data
	//		for the jam to be shot.
	////////////////////////////////////////////////////////////////////////////////
	static void UseRayJamCallback(IEvent* cEvent, IComponent* cCenter);
	
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: ShutdownCallback
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		IEvent* cEvent - The event data
	//		IComponent* cCenter - The component listening for this event.
	//
	// Purpose:  
	//		despawns everything for when the game ends
	////////////////////////////////////////////////////////////////////////////////
	static void ShutdownCallback(IEvent* cEvent, IComponent* cCenter);
	
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: GetATurkey
	//
	// Return:  
	//		CTurkeyComponent* - The item being returned.
	//
	// Parameters: 
	//		D3DXVECTOR3 vPos - The pos to be spawned at
	//
	// Purpose:  
	//		Grabs an unused CTurkeyComponent (or creates one if there isn't an unused one)
	//		and returns it
	////////////////////////////////////////////////////////////////////////////////
	CTurkeyComponent* GetATurkey(D3DXVECTOR3 vPos);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: GetABanana
	//
	// Return:  
	//		CBanana* - The item being returned.
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		Grabs an unused CBanana (or creates one if there isn't an unused one)
	//		and returns it
	////////////////////////////////////////////////////////////////////////////////
	CBanana* GetABanana();
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: GetAChickenSoup
	//
	// Return:  
	//		CChickenSoupComponent* - The item being returned.
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		Grabs an unused CChickenSoupComponent (or creates one if there isn't an unused one)
	//		and returns it
	////////////////////////////////////////////////////////////////////////////////
	CChickenSoupComponent* GetAChickenSoup();
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: GetAnEnergyDrink
	//
	// Return:  
	//		CEnergyDrink* - The item being returned.
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		Grabs an unused CEnergyDrink (or creates one if there isn't an unused one)
	//		and returns it
	////////////////////////////////////////////////////////////////////////////////
	CEnergyDrink* GetAnEnergyDrink();
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: GetAPie
	//
	// Return:  
	//		CPie* - The item being returned.
	//
	// Parameters: 
	//		D3DXVECTOR3 vPos - The pos to be spawned at
	//
	// Purpose:  
	//		Grabs an unused CPie (or creates one if there isn't an unused one)
	//		and returns it
	////////////////////////////////////////////////////////////////////////////////
	CPie* GetAPie(D3DXVECTOR3 vPos);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: GetAPeanutButter
	//
	// Return:  
	//		CPeanutButter* - The item being returned.
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		Grabs an unused CPeanutButter (or creates one if there isn't an unused one)
	//		and returns it
	////////////////////////////////////////////////////////////////////////////////
	CPeanutButter* GetAPeanutButter();
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: GetADonut
	//
	// Return:  
	//		CDonut* - The item being returned.
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		Grabs an unused CDonut (or creates one if there isn't an unused one)
	//		and returns it
	////////////////////////////////////////////////////////////////////////////////
	CDonut* GetADonut();
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: GetARayJam
	//
	// Return:  
	//		CRayJam* - The item being returned.
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		Grabs an unused CRayJam (or creates one if there isn't an unused one)
	//		and returns it
	////////////////////////////////////////////////////////////////////////////////
	CRayJam* GetARayJam();


};

#endif