#ifndef CENERGYDRINK_H_
#define CENERGYDRINK_H_

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "scd::base_component.h"
#include "scd::object.h"
#include "Collision\CCollideable.h"
#include "Managers\Component Managers\CCollisionManager.h"

class IEvent;
class CRenderComponent;

class CEnergyDrink : public scd::base_component
{
private:
	scd::object* m_pParent;
	bool m_bIsSpawned;
	float m_fDuration;
	float m_fTimeLeft;
	float m_fBoostAmount;
	int	ENERGY_DRINK_ID;
public:
	CEnergyDrink(scd::object* pObj) : m_pParent(pObj)
	{

	}
	static CEnergyDrink* CreateEnergyDrinkComponent(scd::object* pObj);
	void FirstInit();
	//call backs
	static void Update(IEvent* cEvent, scd::base_component* cCenter);
	static void ReInit(IEvent* cEvent, scd::base_component* cCenter);
	void Despawn();
	//accessors
	scd::object* GetParent()
	{ 
		return m_pParent;
	}
	bool GetIsSpawned() 
	{
		return m_bIsSpawned;
	}
	float GetDuration()
	{
		return m_fDuration;
	}
	float GetTimeLeft()
	{ 
		return m_fTimeLeft; 
	}
	float GetBoostAmount()  { return m_fBoostAmount; }
	//mutators
	void SetParent(scd::object* pParent)
	{ 
		m_pParent = pParent; 
	}
	void SetIsSpawned(bool bSpawned) 
	{
		m_bIsSpawned = bSpawned;
	}
	void SetDuration(float fDuration) 
	{
		m_fDuration = fDuration;
	}
	void SetTimeLeft(float fRemaining) 
	{
		m_fTimeLeft = fRemaining; 
	}
	void SetBoostAmount(float fBoostAmount)
	{ 
		m_fBoostAmount = fBoostAmount; 
	}
};


#endif