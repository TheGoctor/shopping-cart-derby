////////////////////////////////////////////////////////////////////////////////
//	File			:	EventStructs.cpp
//	Date			:	4/21/11
//	Mod. Date		:	4/21/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Groups events structures with send functions
////////////////////////////////////////////////////////////////////////////////
#include "EventStructs.h"
using namespace EventStructs;

void EventStructs::SendIEvent(string szEventName, IComponent* pSender, void* pData,
	EEventPriority ePriority)
{
	IEvent* pEvent = MMNEWEVENT(
		IEvent(CIDGen::GetInstance()->GetID(szEventName), pSender, pData));
	CEventManager::GetInstance()->PostEvent(pEvent, ePriority);
}

int EventStructs::CreateUpdateStateEvent(lua_State* pLua)
{
	float fDT = (float)lua_tonumber(pLua, -1);
	lua_pop(pLua, 1);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TUpdateStateEvent(fDT)));

	return 1;
}

void EventStructs::SendUpdateEvent(string szEventName, IComponent* pSender,
	float fDT, EEventPriority ePriority)
{
	TUpdateStateEvent* ptUpdateData = MMNEWEVENT(TUpdateStateEvent(fDT));
	SendIEvent(szEventName, pSender, ptUpdateData, PRIORITY_UPDATE);
}

int EventStructs::CreateStateEvent(lua_State* pLua)
{
	EGameState eTo = (EGameState)((int)lua_tonumber(pLua, -1));
	lua_pop(pLua, 1);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TStateEvent(eTo)));

	return 1;
}

void EventStructs::SendStateEvent(string szEventName, IComponent* pSender,
	EGameState eTo, EEventPriority ePriority)
{
	TStateEvent* ptStateData = MMNEWEVENT(TStateEvent(eTo));
	SendIEvent(szEventName, pSender, ptStateData, PRIORITY_INPUT);
}

int EventStructs::CreateRenderEvent(lua_State* pLua)
{
	CObject* pParent = (CObject*)lua_topointer(pLua, -1);
	lua_pop(pLua, 1);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TRenderEvent(pParent)));

	return 1;
}

void EventStructs::SendRenderEvent(string szEventName, IComponent* pSender, CObject*
	pParent, EEventPriority ePriority)
{
	TRenderEvent* ptRenderData = MMNEWEVENT(TRenderEvent(pParent));
	SendIEvent(szEventName, pSender, ptRenderData, ePriority);
}

int EventStructs::CreateRamEvent(lua_State* pLua)
{
	CObject* pRammer = (CObject*)lua_topointer(pLua, -2);
	CObject* pRammee = (CObject*)lua_topointer(pLua, -1);
	lua_pop(pLua, 2);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TRamEvent(pRammer, pRammee)));

	return 1;
}

void EventStructs::SendRamEvent(string szEventName, IComponent* pSender, CObject* pRammer,
	CObject* pRammee, EEventPriority ePriority)
{
	TRamEvent* ptRamData = MMNEWEVENT(TRamEvent(pRammer, pRammee));
	SendIEvent(szEventName, pSender, ptRamData, ePriority);
}

int EventStructs::CreateObjectEvent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -1);
	lua_pop(pLua, 1);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TObjectEvent(pObj)));

	return 1;
}

void EventStructs::SendObjectEvent(string szEventName, IComponent* pSender, CObject* pObj,
	EEventPriority ePriority)
{
	TObjectEvent* ptObjData = MMNEWEVENT(TObjectEvent(pObj));
	SendIEvent(szEventName, pSender, ptObjData, ePriority);
}

int EventStructs::CreateInputEvent(lua_State* pLua)
{
	int nPlayer = (int)lua_tonumber(pLua, -2);
	float fAmount = (float)lua_tonumber(pLua, -1);
	lua_pop(pLua, 2);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TInputEvent(nPlayer, fAmount)));

	return 1;
}

void EventStructs::SendInputEvent(string szEventName, IComponent* pSender,
	int nPlayer, float fAmount, EEventPriority ePriority)
{
	TInputEvent* ptInputData = MMNEWEVENT(TInputEvent(nPlayer, fAmount));
	SendIEvent(szEventName, pSender, ptInputData, PRIORITY_INPUT);
}



void EventStructs::SendHeldItemEvent(string szEventName, IComponent* pSender, 
	EHeldItemType eHeldItemType, CHeldItemComponent* pHeldItem,
	EEventPriority ePriority)
{
	THeldItemEvent* ptItemData = MMNEWEVENT(THeldItemEvent(eHeldItemType,
		pHeldItem));
	SendIEvent(szEventName, pSender, ptItemData, ePriority);
}

int EventStructs::CreateHeadingEvent(lua_State* pLua)
{
	D3DXVECTOR3 vHeading;
	vHeading.x = (float)lua_tonumber(pLua, -3);
	vHeading.x = (float)lua_tonumber(pLua, -2);
	vHeading.x = (float)lua_tonumber(pLua, -1);
	lua_pop(pLua, 3);

	lua_pushlightuserdata(pLua, MMNEWEVENT(THeadingEvent(vHeading)));

	return 1;
}

void EventStructs::SendHeadingEvent(string szEventName, IComponent* pSender, D3DXVECTOR3
	vHeading, EEventPriority ePriority)
{
	THeadingEvent* ptHeadingData = MMNEWEVENT(THeadingEvent(vHeading));
	SendIEvent(szEventName, pSender, ptHeadingData, ePriority);
}

int EventStructs::CreateGoalItemEvent(lua_State* pLua)
{
	EGoalItemType eType = (EGoalItemType)lua_tointeger(pLua, -2);
	CGoalItems* pGIC = (CGoalItems*)lua_touserdata(pLua, -1);
	lua_pop(pLua, 2);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TGoalItemEvent(eType, pGIC)));

	return 1;
}


void EventStructs::SendGoalItemEvent(string szEventName, IComponent* pSender, EGoalItemType
	eGoalItemType, CGoalItems* pGoalItem, EEventPriority ePriority)
{
	TGoalItemEvent* ptGoalData = MMNEWEVENT( 
		TGoalItemEvent(eGoalItemType, pGoalItem));
	SendIEvent(szEventName, pSender, ptGoalData, ePriority);
}

int EventStructs::CreateWeightClassEvent(lua_State* pLua)
{
	ECartWeight eWeightClass = (ECartWeight)((int)lua_tonumber(pLua, -2));
	CObject* pObj = (CObject*)(lua_touserdata(pLua, -1));
	lua_pop(pLua, 2);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TWeightClassEvent(eWeightClass, pObj)));

	return 1;
}

void EventStructs::SendWeightClassEvent(string szEventName, IComponent* pSender, 
		CObject* pObjectToChange, ECartWeight eWeightClass, EEventPriority ePriority)
{
	TWeightClassEvent* pWtData = MMNEWEVENT( 
		TWeightClassEvent(eWeightClass, pObjectToChange));
	SendIEvent(szEventName, pSender, pWtData, ePriority);
}

int EventStructs::CreateGoalItemCollectedEvent(lua_State* pLua)
{
	CObject* pGoalItem  = (CObject*)((int)lua_touserdata(pLua, -2));
	CObject* pCollector  = (CObject*)((int)lua_touserdata(pLua, -1));
	lua_pop(pLua, 2);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TGoalItemCollectedEvent(pGoalItem, pCollector)));

	return 1;
}

void EventStructs::SendGoalItemCollectedEvent(string szEventName, IComponent* pSender,
		CObject* pGoalItem, CObject* pCollector, EGoalItemType eType, EEventPriority ePriority)
{
	TGoalItemCollectedEvent* ptGoalData = MMNEWEVENT(
		TGoalItemCollectedEvent(pGoalItem, pCollector, eType));
	SendIEvent(szEventName, pSender, ptGoalData, ePriority);
}

void EventStructs::SendStatusEffectEvent(string szEventName, IComponent* pSender,
		CObject* pObjectToAffect, float fDuration, EEventPriority ePriority)
{
	TStatusEffectEvent* ptStatusData = MMNEWEVENT(
		TStatusEffectEvent(pObjectToAffect, fDuration));
	SendIEvent(szEventName, pSender, ptStatusData, ePriority);
}

int EventStructs::CreateStatusEffectEvent(lua_State* pLua)
{
	CObject* pObject = (CObject*)lua_touserdata(pLua, -2);
	float fDuration = (float)lua_tonumber(pLua, -1);
	lua_pop(pLua, 2);

	lua_pushlightuserdata(pLua, MMNEWEVENT(TStatusEffectEvent(pObject, fDuration)));

	return 1;
}



void EventStructs::SendFloatEvent(string szEventName, IComponent* pSender,
		float fValue, EEventPriority ePriority)
{
	TFloatEvent* ptFloatData = MMNEWEVENT(
		TFloatEvent(fValue));
	SendIEvent(szEventName, pSender, ptFloatData, ePriority);
}
