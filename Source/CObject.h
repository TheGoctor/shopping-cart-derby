////////////////////////////////////////////////////////////////////////////////
//	File			:	CObject.h
//	Date			:	3/29/11
//	Mod. Date		:	3/29/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Class used for all game objects. Class is completely
//						generic and uses a list of Components to define unique
//						objects
////////////////////////////////////////////////////////////////////////////////

#ifndef _COBJECT_H_
#define _COBJECT_H_

#include <string>
#include <list>

#include "Managers/Global Managers/Memory Manager/CAllocator.h"
#include "Managers/Global Managers/Event Manager/CIDGen.h"


#include "CFrame.h"

class IComponent;

class CObject
{
	std::list<IComponent*, CAllocator<IComponent*>>	m_cComponents;
	CFrame					m_tTransform;
	unsigned int			m_nID;

public:
	CObject(string szName) : m_tTransform(), m_cComponents()
	{
		m_nID = CIDGen::GetInstance()->GetID(szName);
	}
	~CObject()
	{
		// This assumes each of the managers have already released their 
		// handles to the components
		//while(m_cComponents.empty() == false)
		//{
		//	// Delete here
		//}
		m_cComponents.clear();
	} 
	//CObject(const CObject& cObj)
	//{
	//	//m_nID = cObj.m_nID;
	//	//m_tTransform = cObj.m_tTransform;
	//	//list<IComponent*, CAllocator<IComponent*>>::const_iterator cIter;
	//	//cIter = cObj.m_cComponents.begin();
	//	//for(cIter; cIter != cObj.m_cComponents.end(); cIter++)
	//	//{
	//	//	m_cComponents.insert(m_cComponents.end(), *cIter);
	//	//}
	//}
	//CObject& operator=(const CObject& cObj)
	//{
	//	//if(this != &cObj)
	//	//{
	//	//	m_nID = cObj.m_nID;
	//	//	m_tTransform = cObj.m_tTransform;
	//	//	m_cComponents = list<IComponent*, CAllocator<IComponent*>>();
	//	//	m_cComponents = cObj.m_cComponents;
	//	//	//list<IComponent*, CAllocator<IComponent*>>::const_iterator cIter;
	//	//	//cIter = cObj.m_cComponents.begin();
	//	//	//for(cIter; cIter != cObj.m_cComponents.end(); cIter++)
	//	//	//{
	//	//	//	m_cComponents.insert(m_cComponents.end(), *cIter);
	//	//	//}
	//	//}
	//	//return *this;
	//	memcpy(this, &cObj, sizeof(cObj));
	//	return *this;
	//}

	inline CFrame* GetTransform()
	{
		return &m_tTransform;
	}

	inline int GetID()
	{
		return m_nID;
	}

	inline void AddComponent(IComponent* pcComponent)
	{
		m_cComponents.push_back(pcComponent);
	}

	void RemoveComponent(IComponent* pcComponent)
	{
		// This only removes the component from the manager
		// Also remove the comonent from the appropriate manager
		// and that manager is responsible for deleting the component
		m_cComponents.remove(pcComponent);
	}
};

#endif