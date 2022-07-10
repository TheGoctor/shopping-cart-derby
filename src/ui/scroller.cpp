#include "CScrollerComponent.h"


#include "Managers\Global Managers\Event Manager\CEventManager.h"
#include "Managers\Global Managers\Event Manager\CIDGen.h"
#include "Managers\Global Managers\Event Manager\EventStructs.h"
#include "scd::object.h"
#include "Managers\Global Managers\Object Manager\scd::objectManager.h"
#include "Button\CButtonComponent.h"
using namespace EventStructs;


CScrollerComponent::CScrollerComponent(scd::object* pObj) : m_pObject(pObj), m_xSize(0.0f), m_ySize(0.0f),
					m_xVel(0.0f), m_yVel(0.0f), m_xStartPos(0.0f), m_yStartPos(0.0f), m_xPos(0.0f), m_yPos(0.0f)
{

}

// Lua stuff
int CScrollerComponent::CreateScrollerComponent(lua_State* pLua)
{
	// top down is in order in param list. Start at bottom with -1 and work your way to the more negative numbers
	CButtonComponent* pButton = (CButtonComponent*)lua_topointer(pLua, -5);
	float xVel = (float)lua_tonumber(pLua, -4);
	float yVel = (float)lua_tonumber(pLua, -3);
	float xSize = (float)lua_tonumber(pLua, -2);
	float ySize = (float)lua_tonumber(pLua, -1);

	CScrollerComponent* pScroller = CreateScrollerComponent(pButton, xVel, yVel, xSize, ySize);

	lua_pop(pLua, 5);

	lua_pushlightuserdata(pLua, pScroller);


	return 1;
}

CScrollerComponent* CScrollerComponent::CreateScrollerComponent(CButtonComponent* pButton, float xVel, float yVel,
																		float xSize, float ySize)
{
	scd::object* pObj = pButton->GetParent();
	CScrollerComponent* comp = MMNEW(CScrollerComponent(pObj));

	comp->m_pButton = pButton;
	comp->m_xVel = xVel;
	comp->m_yVel = yVel;
	comp->m_xSize = xSize;
	comp->m_ySize = ySize;

	comp->m_xStartPos = (float)pButton->GetSpriteComponent()->GetSpriteData().m_nX;
	comp->m_yStartPos = (float)pButton->GetSpriteComponent()->GetSpriteData().m_nY;
	comp->m_xPos = comp->m_xStartPos;
	comp->m_yPos = comp->m_yStartPos;

	int eAssociatedState = pButton->GetAssociatedState();
	
	// listens for these events
	string szEvent = "EnableObjects";
	szEvent += (char)eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, ScrollerStateEnable);

	szEvent = "DisableObjects";
	szEvent += (char)eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, ScrollerStateDisable);
	
	szEvent = "InitObjects";
	szEvent += (char)eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, ScrollerStateInit);
	
	szEvent = "Update";
	szEvent += (char)eAssociatedState;
	CEventManager::GetInstance()->RegisterEvent(szEvent, comp, Update);

	pObj->AddComponent(comp);



	return comp;
}
//	//Lua Stuff



///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CScrollerComponent::Update(IEvent* cEvent, scd::base_component* cCenter)
{
	CScrollerComponent* comp = (CScrollerComponent*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDt = eEvent->m_fDeltaTime;

	if(comp->m_pObject)
	{
		TSpriteData tSpriteData = comp->m_pButton->GetSpriteComponent()->GetSpriteData();

		float xChange = comp->m_xVel*fDt;
		float yChange = comp->m_yVel*fDt;

		// store in this since the deltas are too small for ints
		comp->m_xPos += xChange;
		comp->m_yPos += yChange;

		tSpriteData.m_nX = (int)comp->m_xPos;
		tSpriteData.m_nY = (int)comp->m_yPos;

		// check reset value for x axis
		if(tSpriteData.m_nX > comp->m_xStartPos + comp->m_xSize)
		{	
			tSpriteData.m_nX = (int)comp->m_xStartPos;
			comp->m_xPos = comp->m_xStartPos;
		}
		// check reset value for y axis
		if(tSpriteData.m_nY < -2048)
		{	
			tSpriteData.m_nY = 1024;
			comp->m_yPos = 1024;
		}

		comp->m_pButton->SetScreenPosition(tSpriteData.m_nX, tSpriteData.m_nY);
	}
}

void CScrollerComponent::ScrollerStateEnable(IEvent* /*cEvent*/, scd::base_component* /*cCenter*/)
{
	//CScrollerComponent* comp = (CScrollerComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	
}

void CScrollerComponent::ScrollerStateDisable(IEvent* /*cEvent*/, scd::base_component* /*cCenter*/)
{
	//CScrollerComponent* comp = (CScrollerComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	
}

void CScrollerComponent::ScrollerStateInit(IEvent* /*cEvent*/, scd::base_component* cCenter)
{
	CScrollerComponent* comp = (CScrollerComponent*)cCenter;
	//TStateEvent* pEvent = (TStateEvent*)cEvent->GetData();

	// if we have a button
	if(comp->m_pButton)
	{
		TSpriteData tSpriteData = comp->m_pButton->GetSpriteComponent()->GetSpriteData();
		tSpriteData.m_nX = (int)comp->m_xStartPos;
		tSpriteData.m_nY = (int)comp->m_yStartPos;
		comp->m_pButton->GetSpriteComponent()->SetSpriteData(tSpriteData);
	}

}
