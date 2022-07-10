////////////////////////////////////////////////////////////////////////////////
//	File: CComponentManager.h"
//
//	Author: Mac Reichelt (MR)
//
//	Description: This class automatically manages components that don't already
//				 have a manager and cleans them up appropriately. Components
//				 are automatically added to the manager and removed when created
//				 and destroyed. The manager is automatically instantiated when 
//				 the first component is created.
////////////////////////////////////////////////////////////////////////////////

#include <list>
using std::list;

#ifndef _CCOMPONENTMANAGER_H_
#define _CCOMPONENTMANAGER_H_

#include "..\Global Managers\Event Manager\CEventManager.h"
#include "..\Global Managers\Memory Manager\CAllocator.h"

// Although this is templated, the type must have a GetParent() function that returns a CObject*
template <typename TYPE>
class CComponentManager
{
	std::list<TYPE *, CAllocator<TYPE*>> m_cAutoList;
	unsigned int m_nShutdownEventID;
public:
	CComponentManager()
	{
		m_nShutdownEventID = CEventManager::GetInstance()->RegisterEvent("Shutdown", 
			(IComponent*)this, Shutdown);
	}

	~CComponentManager()
	{
		//CEventManager::GetInstance()->UnregisterEvent(m_nShutdownEventID, (IComponent*)this);
		//Shutdown(NULL, (IComponent*)this);
	}

	static void Shutdown(IEvent*, IComponent* pComp)
	{
		CComponentManager* pMe = (CComponentManager*)pComp;
		std::list<TYPE* , CAllocator<TYPE*>>::iterator pIter;
		while(!pMe->m_cAutoList.empty())
		{
			pIter = pMe->m_cAutoList.begin();
			MMDEL(*pIter);
		}
		pMe->m_cAutoList.clear();
	}

	void Add(TYPE * obj) 
	{ 
		m_cAutoList.push_back(obj);
	}

	void Remove(TYPE* obj)
	{
		m_cAutoList.remove(obj);;
	}

	TYPE* GetComponent(unsigned int nObjID)
	{
		std::list<TYPE*, CAllocator<TYPE*>>::iterator pIter;
		pIter = m_cAutoList.begin();
		while(pIter != m_cAutoList.end())
		{
			if((*pIter)->GetParent()->GetID() == nObjID)
				return *pIter;
			++pIter;
		}
		return NULL;
	}

	void Remove(unsigned int nObjID)
	{
		Remove(GetComponent(nObjID));
	}
};

////////////////////////////////////////////////////////////////////////////////
//
// Use the class below as a template for how to lay out the classes that need 
// to use this class as a manager. Notice the constructor and destructor of the
// object and how they use the static manager declared in the class
//
////////////////////////////////////////////////////////////////////////////////

//class TestClass
//{
//public:
//	static ComponentManager<TestClass> m_cManager;
//	TestClass() { m_cManager::Add(this); }
//	~TestClass() { m_cManager::Remove(this); }
//};

#endif // _CCOMPONENTMANAGER_H_