////////////////////////////////////////////////////////////////////////////////
//	File			:	CScrollingUVComp.cpp
//	Date			:	7/12/11
//	Mod. Date		:	7/12/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets Meshes Scroll UVs
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CScrollingUVComp.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\ModelManager.h"

// Defines
#define SCI_CART_SCROLL_RATE			 (0.5f)
#define SCI_CART_SCROLL_TOP_SPEED		 (18.0f)
#define SCI_CART_SCROLL_MIN_SPEED		 (1.0f)

#define STORY_TIME_CART_SCROLL_RATE		 (0.25f)
#define STORY_TIME_CART_SCROLL_TOP_SPEED (12.0f)
#define STORY_TIME_CART_SCROLL_MIN_SPEED (1.0f)

#define ENDGAME_CART_SCROLL_RATE		 (0.5f)
#define ENDGAME_CART_SCROLL_TOP_SPEED	 (18.0f)
#define ENDGAME_CART_SCROLL_MIN_SPEED	 (1.0f)

// Initalize
void CScrollingUVComp::Init(void)
{
	// Find Top Speed
	switch(m_eCharacterSetType)
	{
	case CS_SCIENTISTS:
		m_fTopSpeed = SCI_CART_SCROLL_TOP_SPEED;
		m_fMinSpeed = SCI_CART_SCROLL_MIN_SPEED;
		break;
	case CS_STORYTIME:
		m_fTopSpeed = STORY_TIME_CART_SCROLL_TOP_SPEED;
		m_fMinSpeed = STORY_TIME_CART_SCROLL_MIN_SPEED;
		break;
	case CS_ENDGAME:
		m_fTopSpeed = ENDGAME_CART_SCROLL_TOP_SPEED;
		m_fMinSpeed = ENDGAME_CART_SCROLL_MIN_SPEED;
		break;
	default:
		break;
	}

	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();

	// Update Gameplay
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	// Update Win
	szEventName = "Update";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, this, UpdateCallback);

	// Accelerate
	pEM->RegisterEvent("AccelerateEffect", this, AccelerateCallback);
	pEM->RegisterEvent("DecelerateEffect", this, AccelerateCallback);

	// Destroy Obj
	pEM->RegisterEvent("DestroyObject", this, DestroyObjectCallback);
}

// Shutdown
void CScrollingUVComp::Shutdown(void)
{
	CEventManager::GetInstance()->UnregisterEventAll(this);
}

// Factory
int CScrollingUVComp::CreateScrollingUVComponent(lua_State* pLua)
{
	// Get Data from Lua
	CObject* pObj = (CObject*)lua_topointer(pLua, -3);
	DXMesh* pMesh = (DXMesh*)lua_topointer(pLua,  -2);
	ECharacterSet eType = (ECharacterSet)lua_tointeger(pLua, -1);

	// Pop that Solda Boi!
	lua_pop(pLua, 3);

	// Create the Component
	CreateScrollingUVComponent(pObj, pMesh, eType);

	// Do not Return anythin to LUA
	return 0;
}

// Update
void CScrollingUVComp::UpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CScrollingUVComp* pSUVC = (CScrollingUVComp*)pComp;

	// Get Data from Event
	TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
	float fDT =	pUpdateEvent->m_fDeltaTime;

	// Update Component
	pSUVC->Update(fDT);
}
void CScrollingUVComp::Update(float fDT)
{
	// Scientist Update
	switch(m_eCharacterSetType)
	{
	case CS_SCIENTISTS:
		UpdateSciCart(fDT);
		break;
	case CS_STORYTIME:
		UpdateStoryTimeCart(fDT);
		break;
	case CS_ENDGAME:
		UpdateEndGameCart(fDT);
		break;
	default:
		break;
	}
}

// Sci Cart
void CScrollingUVComp::UpdateSciCart(float fDT)
{
	// Get Vert Info
	TMeshVertexInfo* pVertInfo = ModelManager::GetInstance()->GetCloneVerts(m_pMesh->GetName());
	m_pMesh = ModelManager::GetInstance()->GetAnimMesh(m_pMesh->GetName());

	// Scroll
	for(unsigned int uVert = 0; uVert < m_pMesh->GetVertCount(); ++uVert)
	{
		if(pVertInfo->m_vUV[uVert].x >= 0.98f)
		{
			pVertInfo->m_vUV[uVert].y -= fDT * (SCI_CART_SCROLL_RATE * m_fSpeed);
		}
	}

	// Update Vert Info
	m_pMesh->SetIndexedTexturedVertexInfo(pVertInfo);

	//m_fSpeed = 1.0f;
}

// Story Cart
void CScrollingUVComp::UpdateStoryTimeCart(float fDT)
{
	// Get Vert Info
	TMeshVertexInfo* pVertInfo = ModelManager::GetInstance()->GetCloneVerts(m_pMesh->GetName());
	m_pMesh = ModelManager::GetInstance()->GetAnimMesh(m_pMesh->GetName());

	// Scroll
	for(unsigned int uVert = 0; uVert < m_pMesh->GetVertCount(); ++uVert)
	{
		if(pVertInfo->m_vUV[uVert].y >= -0.5f)
		{
			pVertInfo->m_vUV[uVert].x += fDT * (STORY_TIME_CART_SCROLL_RATE * m_fSpeed);
		}
	}

	// Update Vert Info
	m_pMesh->SetIndexedTexturedVertexInfo(pVertInfo);

	//m_fSpeed = 1.0f;
}

// Endgame Cart
void CScrollingUVComp::UpdateEndGameCart(float fDT)
{
	// Get Vert Info
	TMeshVertexInfo* pVertInfo = ModelManager::GetInstance()->GetCloneVerts(m_pMesh->GetName());
	m_pMesh = ModelManager::GetInstance()->GetAnimMesh(m_pMesh->GetName());

	// Scroll
	for(unsigned int uVert = 0; uVert < m_pMesh->GetVertCount(); ++uVert)
	{
		if(pVertInfo->m_vUV[uVert].x >= 0.89f)
		{
			pVertInfo->m_vUV[uVert].y -= fDT * (ENDGAME_CART_SCROLL_RATE * m_fSpeed);
		}
	}

	// Update Vert Info
	m_pMesh->SetIndexedTexturedVertexInfo(pVertInfo);

	//m_fSpeed = 1.0f;
}

// Accelerate
void CScrollingUVComp::AccelerateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CScrollingUVComp* pSUVC = (CScrollingUVComp*)pComp;
	
	// Get Data
	TInputEvent* pData = (TInputEvent*)pEvent->GetData();

	if(CScrollingUVComp::CheckForPlayerMatch(pSUVC->GetParent(), pData->m_pPlayer))
	{
		if(pData->m_fAmount < pSUVC->m_fMinSpeed)
		{
			pSUVC->m_fSpeed = pSUVC->m_fMinSpeed;
		}
		else if(pData->m_fAmount > pSUVC->m_fTopSpeed)
		{
			pSUVC->m_fSpeed = pSUVC->m_fTopSpeed;
		}
		else
		{
			pSUVC->m_fSpeed = pData->m_fAmount;
		}
	}
}

// Check for Player
bool CScrollingUVComp::CheckForPlayerMatch(CObject* pParentObj, CObject* pOtherObj)
{
	// Get Object Names
	char* szEventObjName = (char*)pOtherObj->GetID();
	char* szCompObjName  = (char*)pParentObj->GetID();

	// Check for a name Match
	bool bMatch = true;
	for(int i = 0; i < 7; ++i)
	{
		if(szEventObjName[i] != szCompObjName[i])
		{
			bMatch = false;
		}
	}

	return bMatch;
}

// Destroy Object
void CScrollingUVComp::DestroyObjectCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Component from Event
	CScrollingUVComp* pSUVC = (CScrollingUVComp*)pComp;
	TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();
	
	if(pSUVC->GetParent()->GetID() == pData->m_pcObj->GetID())
	{
		pSUVC->Shutdown();
	}
}