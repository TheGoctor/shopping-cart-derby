#ifndef RAYJAM_H_
#define RAYJAM_H_

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

#define JAM_TIME 10.0f

class CRayJam : public scd::base_component
{
private:
	scd::object* m_pParent;
	bool m_bIsSpawned;
	float m_fDuration;
	float m_fTimeLeft;
	int JAM_ID;
	
public:
	CRayJam(scd::object* pObj) : m_pParent(pObj)
	{
	}
	static CRayJam* CreateRayJamComponent(scd::object* pObj);
	void FirstInit();
	//call backs
	static void Update(IEvent* cEvent, scd::base_component* cCenter);
	static void ReInit(IEvent* cEvent, scd::base_component* cCenter);
	void Despawn();
	//accessors
	scd::object* GetParent(void)
	{
		return m_pParent;
	}
	bool GetIsSpawned(void)
	{
		return m_bIsSpawned;
	}
	float GetDuration(void)
	{
		return m_fDuration;
	}
	float GetTimeLeft(void)
	{
		return m_fTimeLeft;
	}
	//mutators
	void SetParent(scd::object* pParent)
	{
		m_pParent = pParent;
	}
	void SetIsSpawned(bool bIsSpawned)
	{
		m_bIsSpawned = bIsSpawned;
	}
	void SetDuration(float fDuration)
	{
		m_fDuration = fDuration;
	}
	void SetTimeLeft(float fRemaining)
	{
		m_fTimeLeft = fRemaining;
	}
};


#endif