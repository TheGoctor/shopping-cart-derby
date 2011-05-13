#ifndef _EVENTSTRUCTS_H_
#define _EVENTSTRUCTS_H_

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "CEventManager.h"
#include "IEvent.h"
#include "..\..\..\CObject.h"
#include "..\State Manager\CStateManager.h"
#include "..\..\Component Managers\CMovementManager.h"

class IComponent;
class CGoalItems;
class CHeldItemComponent;
class CObject;

namespace EventStructs
{
	// Global Event Man
	//CEventManager* g_pEM = CEventManager::GetInstance();

	void SendIEvent(string szEventName, IComponent* pSender, void* pData,
		EEventPriority ePriority);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TUpdateStateEvent 
	{
		float m_fDeltaTime;

		TUpdateStateEvent(float fDT) : m_fDeltaTime(fDT)
		{
		}
	};
	 int CreateUpdateStateEvent(lua_State* pLua);
	void SendUpdateEvent(string szEventName, IComponent* pSender,
		float fDT, EEventPriority ePriority = PRIORITY_UPDATE);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TStateEvent 
	{
		EGameState m_eToState;

		TStateEvent(EGameState eTo) :	m_eToState(eTo)
		{
		}
	};
	 int CreateStateEvent(lua_State* pLua);
	void SendStateEvent(string szEventName, IComponent* pSender,
		EGameState eTo, EEventPriority ePriority = 
		PRIORITY_INPUT);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TRenderEvent 
	{
		CObject* m_pParent;

		TRenderEvent(CObject* pParent) : m_pParent(pParent)
		{
		}
	};
	 int CreateRenderEvent(lua_State* pLua);
	void SendRenderEvent(string szEventName, IComponent* pSender, CObject*
		pParent, EEventPriority ePriority = PRIORITY_RENDER);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TRamEvent 
	{
		CObject* m_pcRammer;
		CObject* m_pcRammee;

		TRamEvent(CObject* pRammer, CObject* pRammee) : m_pcRammer(pRammer), 
			m_pcRammee(pRammee)
		{
		}
	};
	 int CreateRamEvent(lua_State* pLua);
	void SendRamEvent(string szEventName, IComponent* pSender, CObject* pRammer,
		CObject* pRammee, EEventPriority ePriority = PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TObjectEvent 
	{
		CObject* m_pcObj;

		TObjectEvent(CObject* pObj) : m_pcObj(pObj)
		{
		}
	};
	 int CreateObjectEvent(lua_State* pLua);
	void SendObjectEvent(string szEventName, IComponent* pSender, CObject* pObj,
		EEventPriority ePriority = PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TInputEvent 
	{
		int		m_nPlayer;
		float	m_fAmount;

		TInputEvent(int nPlayer, float fAmount) : m_nPlayer(nPlayer),
			m_fAmount(fAmount)
		{
		}
	};
	 int CreateInputEvent(lua_State* pLua);
	void SendInputEvent(string szEventName, IComponent* pSender,
		int nPlayer, float fAmount, EEventPriority ePriority = PRIORITY_INPUT);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct THeldItemEvent 
	{
		EHeldItemType m_eHeldItemType;
		CHeldItemComponent* m_pcHeldItem;

		THeldItemEvent(EHeldItemType eHeldItemType, CHeldItemComponent*
			pHeldItem) : m_eHeldItemType(eHeldItemType), m_pcHeldItem(pHeldItem)
		{
		}
	};
	void SendHeldItemEvent(string szEventName, IComponent* pSender, 
		EHeldItemType eHeldItemType, CHeldItemComponent* pHeldItem,
		EEventPriority ePriority = PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct THeadingEvent 
	{
		D3DXVECTOR3	m_vHeading;

		THeadingEvent(D3DXVECTOR3 vHeading) : m_vHeading(vHeading)
		{
		}
	};
	 int CreateHeadingEvent(lua_State* pLua); // takes (x,y,z)
	void SendHeadingEvent(string szEventName, IComponent* pSender, D3DXVECTOR3
		vHeading, EEventPriority ePriority = PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TGoalItemEvent 
	{
		EGoalItemType m_eGoalItemType;
		CGoalItems* m_pcGoalItem;

		TGoalItemEvent(EGoalItemType eType, CGoalItems* pGIC) :
			m_eGoalItemType(eType), m_pcGoalItem(pGIC)
		{
		}
	};
	 int CreateGoalItemEvent(lua_State* pLua);
	void SendGoalItemEvent(string szEventName, IComponent* pSender, EGoalItemType
		eGoalItemType, CGoalItems* pGoalItem, EEventPriority ePriority = 
		PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TWeightClassEvent 
	{
		ECartWeight	m_eWeightClass;
		CObject*	m_pObject;

		TWeightClassEvent(ECartWeight eWeightClass, CObject* pObj) : 
			m_eWeightClass(eWeightClass), m_pObject(pObj)
		{
		}
	};
	 int CreateWeightClassEvent(lua_State* pLua);
	void SendWeightClassEvent(string szEventName, IComponent* pSender, 
		CObject* pObjectToChange, ECartWeight eWeightClass, 
		EEventPriority ePriority = PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TGoalItemCollectedEvent 
	{
		CObject* m_pcGoalItem;
		CObject* m_pcCollector;
		EGoalItemType m_eType;

		TGoalItemCollectedEvent(CObject* pGoalItem, CObject* pCollector, EGoalItemType eType = NO_ITEM) :
			m_pcGoalItem(pGoalItem), m_pcCollector(pCollector), m_eType(eType)
		{
		}
	};
	 int CreateGoalItemCollectedEvent(lua_State* pLua);
	void SendGoalItemCollectedEvent(string szEventName, IComponent* pSender,
		CObject* pGoalItem, CObject* pCollector, EGoalItemType eType = NO_ITEM, EEventPriority ePriority = 
		PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TStatusEffectEvent 
	{
		CObject*	m_pObject;
		float		m_fDuration;

		TStatusEffectEvent(CObject* pObj, float fDuration) : 
			m_pObject(pObj), m_fDuration(fDuration)
		{
		}
	};
	 int CreateStatusEffectEvent(lua_State* pLua);
	void SendStatusEffectEvent(string szEventName, IComponent* pSender, 
		CObject* pObjectToAffect, float fDuration, 
		EEventPriority ePriority = PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TFloatEvent 
	{
		float		m_fValue;

		TFloatEvent(float fValue) : m_fValue(fValue)
		{
		}
	};
	void SendFloatEvent(string szEventName, IComponent* pSender, 
		float fValue, 
		EEventPriority ePriority = PRIORITY_NORMAL);
};

#endif // _EVENTSTRUCTS_H_