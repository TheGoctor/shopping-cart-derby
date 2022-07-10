////////////////////////////////////////////////////////////////////////////////
//	File			:	CObject.h
//	Date			:	3/29/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Class used for all game objects. Class is completely
//						generic and uses a list of Components to define unique
//						behaviors
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
	// Container of Components that define this object's behavior
	std::list<IComponent*, CAllocator<IComponent*>>	m_cComponents;

	// The objects Transform Matrices (World and Local)
	CFrame	m_tTransform;

	// The ID of the object. ID is generated from the name of the object
	unsigned int m_nID;

public:
	// Constructor
	CObject(string szName) : m_tTransform(), m_cComponents()
	{
		m_nID = CIDGen::GetInstance()->GetID(szName);
	}

	// Destructor
	~CObject()
	{
		// This assumes each of the managers have already released their 
		// handles to the components
		m_cComponents.clear();
	}

	// Accessors
	inline CFrame* GetTransform()
	{
		return &m_tTransform;
	}

	inline unsigned int GetID()
	{
		return m_nID;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Function: “AddComponent”
	//
	// Return:  void
	//
	// Parameters: IComponent* pcComponent	-	A pointer to the component to be
	//											added to the object
	//
	// Purpose:  This function is used to add components to an object. Objects
	//		should not, and cannot, have more than one instance of a single type of
	//		component i.e. One object having two render components.
	////////////////////////////////////////////////////////////////////////////////
	inline void AddComponent(IComponent* pcComponent)
	{
		m_cComponents.push_back(pcComponent);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Function: “RemoveComponent”
	//
	// Return:  void
	//
	// Parameters: IComponent* pcComponent	-	A pointer to the component to be
	//											removed from the object
	//
	// Purpose:  This function is used to remove components from an object. This
	//		function only removes the component from the object and does not clean
	//		up the component's memory or remove it from it's manager.
	////////////////////////////////////////////////////////////////////////////////
	inline void RemoveComponent(IComponent* pcComponent)
	{
		// This only removes the component from the object.
		// Also remove the component from the appropriate manager
		// and that manager is responsible for deleting the component
		m_cComponents.remove(pcComponent);
	}
};

#endif