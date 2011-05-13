/*******************************************************************************
 * Filename:  		CLevelManager.h
 * Date:      		04/09/2011
 * Mod. Date: 		04/16/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This manager will be used to
 *					spawn items on the level and
 *					cycle through them.
 ******************************************************************************/

#ifndef _CLEVELMANAGER_H_
#define _CLEVELMANAGER_H_

#include <D3dx9math.h>
#include <list>
using std::list;

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\Global Managers\Memory Manager\CAllocator.h"

class CEventManager;
class IEvent;
class IComponent;
class CObject;
class CRenderComponent;

class CLevelManager
{
private:

	struct CNode
	{
		char szName[40];
		D3DXMATRIX tLocalMatrix;
		D3DXMATRIX tWorldMatrix;
	};

	list<CRenderComponent*, CAllocator<CRenderComponent*>> m_cLevelRenderComps;
	list<CRenderComponent*, CAllocator<CRenderComponent*>> m_cLevelCollisionRenderComps;
	
	int	m_objectcount;

	CNode* m_levelNodes;

	// Constructor
	CLevelManager();

	/////////////////////
	// Trilogy of Evil //
	/////////////////////
	~CLevelManager();
	CLevelManager(const CLevelManager&) {}
	CLevelManager& operator=(const CLevelManager&) {}

	void BuildLevel(char* szNodeMap);
	bool AddCollision(char* szMeshName);

public:
	
	static CLevelManager* GetInstance()
	{
		static CLevelManager cLevelManager;
		return &cLevelManager;
	}

	void Init();
	static void Shutdown(IEvent*, IComponent*);
	static void Update(IEvent*, IComponent*);

	void AddRenderCollision(CRenderComponent* pRC);

	// Mutators
};

#endif