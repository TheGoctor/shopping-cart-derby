////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateManager.cpp
//	Date			:	5/12/11
//	Mod. Date		:	5/12/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Factory for Animate Components
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CAnimateManager.h"
#include "..\\..\\Components\\Rendering\\CAnimateComponent.h"
#include "..\\Global Managers\\Rendering Managers\\CAnimation.h"
#include "..\\Global Managers\\Rendering Managers\\DXMesh.h"
#include "..\\Global Managers\\Rendering Managers\\ModelManager.h"
#include "..\\Global Managers\\Event Manager\\CEventManager.h"
#include "..\Global Managers\Console Manager\CConsoleManager.h"
#include "..\Global Managers\Object Manager\CObjectManager.h"
#include "..\\..\\CObject.h"

// Constructor
CAnimateManager::CAnimateManager(void)
{
}

// Destructor
CAnimateManager::~CAnimateManager(void)
{
}

// Initalize
void CAnimateManager::Init(void)
{
	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("Shutdown", (IComponent*)GetInstance(), ShutdownCallback);
	
	string szEvent = "ShutdownObjects";
	szEvent += (char)GAMEPLAY_STATE;
	pEM->RegisterEvent(szEvent, (IComponent*)GetInstance(), DisableCallback);
}

// Factory
int CAnimateManager::CreateAnimComp(lua_State* pLua)
{
	// Get the number of event/anim pairs
	// Do math so we can just use nLastParam in the loop
	int nLastParam = lua_tointeger(pLua, -1) * -3 - 3;

	// Store number of parameters so we can pop them off at the end
	int nNumParams = nLastParam * -1;

	// Get the object the component is bound to
	CObject* pObj = (CObject*)lua_topointer(pLua, nLastParam++);

	// Get the name of the mesh the animation uses
	string szMeshName = lua_tostring(pLua, nLastParam++);

	// Get the correct mesh for the animation
	DXMesh* pMesh = ModelManager::GetInstance()->GetCloneMesh(szMeshName);
	if(pMesh == NULL)
	{
		lua_pop(pLua, nNumParams);
		return 0;
	}

	// Create the Animation Component
	CAnimateComponent* pComp = MMNEW(CAnimateComponent(pObj, pMesh));

	// Get all the Event/Anim pairs
	string szEventName, szPreAnimName, szLoopAnimName;
	for(; nLastParam < -1; ++nLastParam)
	{
		szEventName = lua_tostring(pLua, nLastParam);
		szPreAnimName = lua_tostring(pLua, ++nLastParam);
		szLoopAnimName = lua_tostring(pLua, ++nLastParam);
		pComp->AddAnimation(szEventName, szPreAnimName, szLoopAnimName);
	}

	// Build the interpolator
	pComp->GetInterpolator().SetMesh(pMesh);
	//pComp->GetInterpolator().SetAnimation(); // IDK what to set to
	pComp->GetInterpolator().BuildHiearchy();

	pObj->AddComponent(pComp);
	pObj->GetTransform()->AddChildFrame(&pComp->GetInterpolator().GetCurrentBoneFrames()[0]);
	GetInstance()->m_cAnimateComps.insert(make_pair((unsigned int)pObj->GetID(), pComp));

	lua_pop(pLua, nNumParams);

	// Send the Mehs pointer to Lua so it can be used with the render comp
	lua_pushlightuserdata(pLua, pMesh);

	return 1;
}

CAnimateComponent* CAnimateManager::CreateAnimateComp(CObject* pParent, DXMesh* pMesh)
{
	// NOTE: I'm not sure if this function still works since the 
	// interpolator is no longer receiving an animation before it builds
	// the hierarchy. Someone needs to take a look at this and determine
	// the best thing to do about it. Until then, try not to use this
	// function.
	//					- MR
	
	// Create Component
	CAnimateComponent* comp = MMNEW(CAnimateComponent(pParent, pMesh));

	comp->GetInterpolator().SetMesh(pMesh);
	//comp->GetInterpolator().BuildHiearchy();
	
	// Attach to Obj
	pParent->AddComponent(comp);

	pParent->GetTransform()->AddChildFrame(&comp->GetInterpolator().GetCurrentBoneFrames()[0]);
	//pParent->GetTransform()->AddChildFrame(&pMesh->GetCurrentBoneFrames()[0]);

	// Insert into Map
	GetInstance()->m_cAnimateComps.insert(make_pair((unsigned int)pParent->GetID(), comp));

	return comp;
}

// Shutdown
void CAnimateManager::ShutdownCallback(IEvent*, IComponent*)
{
	GetInstance()->Shutdown();
}
void CAnimateManager::Shutdown(void)
{
	// Cleanup Comps
	map<unsigned int, CAnimateComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAnimateComponent*>>>::iterator compIter
			= m_cAnimateComps.begin();
	while(compIter != m_cAnimateComps.end())
	{
		MMDEL(compIter->second);

		compIter++;
	}
}

void CAnimateManager::DisableCallback(IEvent*, IComponent*)
{
	CAnimateManager* pAM = GetInstance();

	pAM->Disable();

	Debug.Print("Disabling All Animate Components");
}

void CAnimateManager::Disable()
{
	// I'll take it upon myself to clean up everything that has to do with the
	// Animated Objects. Send event so the other managers can remove their components
	map<unsigned int, CAnimateComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CAnimateComponent*>>>::iterator pIter;
	CEventManager* pEM = CEventManager::GetInstance();
	pIter = m_cAnimateComps.begin();

	while(pIter != m_cAnimateComps.end())
	{
		pIter->second->GetParent()->RemoveComponent(pIter->second);
		pEM->UnregisterEventAll(pIter->second);
		CObjectManager::GetInstance()->DestroyObject(pIter->second->GetParent());
		MMDEL(pIter->second);
		++pIter;
	}
	m_cAnimateComps.clear();
	
	Debug.Print("All Animate Components Disabled");
}