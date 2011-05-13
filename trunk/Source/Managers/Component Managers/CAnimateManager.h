////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimateManager.h
//	Date			:	5/12/11
//	Mod. Date		:	5/12/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Factory for Animate Components
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _CANIMATEMANAGER_H_
#define _CANIMATEMANAGER_H_

// Includes
#include <map>
using namespace std;
#include "..\\Global Managers\\Memory Manager\\CAllocator.h"

// Foward Declares
class CAnimateComponent;
class DXMesh;
class CAnimation;
class CObject;

class CAnimateManager
{
private:

	// Singleton
	CAnimateManager(void);
	~CAnimateManager(void);

	// Map of Animate Components
	map<unsigned int, CAnimateComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CAnimateComponent*>>> m_cAnimateComps;

public:

	// Singleton
	static CAnimateManager* GetInstance(void) 
	{ 
		static CAnimateManager instance; 
		return &instance; 
	}

	// Factory
	static CAnimateComponent* CreateAnimateComp(CObject* pParent, DXMesh* pMesh,
							   CAnimation* pAnimation);
};

#endif // _CANIMATEMANAGER_H_