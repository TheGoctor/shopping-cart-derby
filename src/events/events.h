#ifndef _EVENTSTRUCTS_H_
#define _EVENTSTRUCTS_H_

#include <D3dx9math.h>
#include <string>
using std::string;

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
		CObject*	m_pPlayer;
		float		m_fAmount;

		TInputEvent(CObject* pPlayer, float fAmount) : m_pPlayer(pPlayer),
			m_fAmount(fAmount)
		{
		}
	};
	 int CreateInputEvent(lua_State* pLua);
	void SendInputEvent(string szEventName, IComponent* pSender,
		CObject* pPlayer, float fAmount, EEventPriority ePriority = PRIORITY_INPUT);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct THeldItemCollected
	{
		CObject* m_pcHeldItem;
		CObject* m_pcCollector;

		THeldItemCollected(CObject* pHeldItem, CObject* pCollector) :
		m_pcHeldItem(pHeldItem), m_pcCollector(pCollector)
		{
		}
	};
	int SendHeldItemCollectedEvent(lua_State* pLua);
	void SendHeldItemCollectedEvent(string szEventName, IComponent* pSender,
		CObject* pHeldItem, CObject* pCollector, EEventPriority ePriority = 
		PRIORITY_NORMAL);

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

		TGoalItemCollectedEvent(CObject* pGoalItem, CObject* pCollector) :
			m_pcGoalItem(pGoalItem), m_pcCollector(pCollector)
		{
		}
	};
	 int CreateGoalItemCollectedEvent(lua_State* pLua);
	void SendGoalItemCollectedEvent(string szEventName, IComponent* pSender,
		CObject* pGoalItem, CObject* pCollector, EEventPriority ePriority = 
		PRIORITY_NORMAL);

	//event struct sent on collision with any object that reacts
	//contains the object that is effected and the reflection vector from the object it hit
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TImpactEvent
	{
		CObject* m_pcCollider;	//object to rebound
		CObject* m_pCollidedWith;
		D3DXVECTOR3 m_vNormal;	
		D3DXVECTOR3 m_vColPt;
		TImpactEvent(CObject* pCollider, CObject* pCollidedwith, D3DXVECTOR3 vNorm, D3DXVECTOR3 vColPt) : 
		m_pcCollider(pCollider), m_pCollidedWith(pCollidedwith), m_vNormal(vNorm), m_vColPt(vColPt)
		{
		}
	};
	int CreateImpactEvent(lua_State* pLua);
	void SendImpactEvent(string szEventName, IComponent* pSender,
								   CObject* pCollider, CObject* pCollidedwith, 
								   D3DXVECTOR3 vNormal, 
								   EEventPriority ePriority = PRIORITY_NORMAL, 
								   D3DXVECTOR3 vColPt = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

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
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	/*struct TCharacterEvent
	{
		ECharacter eCharacter;

		TCharacterEvent(ECharacter echaracter)
		{
		}	
	};
	void SendCharacterEvent(string szEventName, IComponent* pSender, 
		ECharacter eCharacter, 
		EEventPriority ePriority = PRIORITY_NORMAL);*/
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	struct TSpawnPickupItemEvent 
	{
		D3DXVECTOR3 m_vVec1;
		D3DXVECTOR3 m_vVec2;
		int			m_nItemID;

		TSpawnPickupItemEvent(D3DXVECTOR3 v1, D3DXVECTOR3 v2, int nID) : m_vVec1(v1), m_vVec2(v2), m_nItemID(nID)
		{
		}
	};
	void SendSpawnPickupItemEvent(string szEventName, IComponent* pSender, 
		D3DXVECTOR3 v1, D3DXVECTOR3 v2, int nItemID,
		EEventPriority ePriority = PRIORITY_NORMAL);


	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TPickupItemCollectedEvent 
	{
		CObject* m_pcPickupItem;
		CObject* m_pcCollector;
		EGoalItemType m_eItemType;

		TPickupItemCollectedEvent(CObject* pPickupItem, CObject* pCollector, EGoalItemType eType = NO_ITEM) :
			m_pcPickupItem(pPickupItem), m_pcCollector(pCollector), m_eItemType(eType)
		{
		}
	};
	void SendPickupItemCollectedEvent(string szEventName, IComponent* pSender,
		CObject* pGoalItem, CObject* pCollector, EGoalItemType eType, EEventPriority ePriority = 
		PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TPickupItemEvent 
	{
		CObject* m_pcPickupItem;
		EGoalItemType m_eItemType;

		TPickupItemEvent(CObject* pPickupItem, EGoalItemType eType = NO_ITEM) :
			m_pcPickupItem(pPickupItem), m_eItemType(eType)
		{
		}
	};
	void SendPickupItemEvent(string szEventName, IComponent* pSender,
		CObject* pGoalItem, EGoalItemType eType, EEventPriority ePriority = 
		PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TTwoIntEvent 
	{
		int m_nInt1;
		int m_nInt2;

		TTwoIntEvent(int n1, int n2) :
			m_nInt1(n1), m_nInt2(n2)
		{
		}
	};
	void SendTwoIntEvent(string szEventName, IComponent* pSender,
		int n1, int n2, EEventPriority ePriority = PRIORITY_NORMAL);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TMouseEvent
	{
		LONG m_lMouseX;
		LONG m_lMouseY;

		TMouseEvent(LONG nX, LONG nY) : m_lMouseX(nX), m_lMouseY(nY)
		{
		}
	};
	void SendMouseEvent(string szEventName, IComponent* pSender,
		LONG nX, LONG nY, EEventPriority ePriority = PRIORITY_INPUT);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	int SendLuaEvent(lua_State*);
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////

	struct TConsoleEvent
	{
		char m_chKeyPressed;

		TConsoleEvent(char chKeyPressed) : m_chKeyPressed(chKeyPressed)
		{
		}
	};
	void SendConsoleEvent(string szEventName, IComponent* pSender,
		char chKey, EEventPriority ePriority = PRIORITY_INPUT);
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TSpeedEvent 
	{
		CObject*	m_pObject;
		float		m_fSpeed;

		TSpeedEvent(CObject* pObject, float fSpeed) : m_pObject(pObject), m_fSpeed(fSpeed)
		{
		}
	};
	void SendSpeedEvent(string szEventName, IComponent* pSender, 
		CObject* pObject, float fSpeed, 
		EEventPriority ePriority = PRIORITY_NORMAL);
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct THeldItemSpawned 
	{
		CHeldItemComponent*	m_pHeldItem;

		THeldItemSpawned(CHeldItemComponent* pHeldItem) : m_pHeldItem(pHeldItem)
		{
		}
	};
	void SendHeldItemSpawnedEvent(string szEventName, IComponent* pSender, 
		CHeldItemComponent*	pHeldItem,
		EEventPriority ePriority = PRIORITY_NORMAL);
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	struct TStringEvent 
	{
		string m_szString;

		TStringEvent(string& szString) : m_szString(szString)
		{
		}

		TStringEvent(char* szString)
		{
			m_szString = szString;
		}
	};
	void SendStringEvent(string szEventName, IComponent* pSender, 
		string&	pString, EEventPriority ePriority = PRIORITY_NORMAL);
	void SendStringEvent(string szEventName, IComponent* pSender, 
		char* pString, EEventPriority ePriority = PRIORITY_NORMAL);
};

#endif // _EVENTSTRUCTS_H_