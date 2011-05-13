/*******************************************************************************
 * Filename:  		CHeldItemComponent.h
 * Date:      		05/01/2011
 * Mod. Date: 		05/09/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This will represent the Held Item in the world.
					When created, Init should be called and a EHeldItemType
					should be given.
 ******************************************************************************/

#ifndef _CHELDITEMCOMPONENT_H_
#define _CHELDITEMCOMPONENT_H_

#include "..\..\IComponent.h"
#include "..\..\Enums.h"

class CObject;

class CHeldItemComponent : public IComponent
{
	CObject* m_pParent;
	EHeldItemType m_eType;
	bool m_bSpawned;

public:
	// Constructor
	CHeldItemComponent(CObject* pObj);
	CHeldItemComponent();

	// Destructor
	~CHeldItemComponent();

	void Init(EHeldItemType eType);
	void Spawn();

	void Update(const float fDT);

	inline CObject* GetParent(void)
	{
		return m_pParent;
	}

	inline EHeldItemType GetType()
	{
		return m_eType;
	}

	inline bool IsSpawned()
	{
		return m_bSpawned;
	}
};

#endif // _CHELDITEMCOMPONENT_H_