/*******************************************************************************
 * Filename:  		CHeldItemComponent.cpp
 * Date:      		05/01/2011
 * Mod. Date: 		05/09/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This will represent the Held Item in the world.
					When created, Init should be called and a EHeldItemType
					should be given.
 ******************************************************************************/

#include "CHeldItemComponent.h"
#include "..\..\CObject.h"

CHeldItemComponent::CHeldItemComponent(CObject* pObj) : m_pParent(pObj)
{
}

CHeldItemComponent::CHeldItemComponent()
{
}

CHeldItemComponent::~CHeldItemComponent()
{
}

void CHeldItemComponent::Init(EHeldItemType eType)
{
	m_eType = eType;
}

void CHeldItemComponent::Spawn()
{
	m_bSpawned = true;
}

void CHeldItemComponent::Update(const float fDT)
{
}