////////////////////////////////////////////////////////////////////////////////
//  File			:	"CCreditManager.h"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	05/18/11
//
//	Last Changed	:	07/24/11
//
//	Changed By		:	HN			
//
//  Purpose			:	Wrapper class for managing the credit screen
////////////////////////////////////////////////////////////////////////////////
#ifndef _CCREDITMANAGER_H_
#define _CCREDITMANAGER_H_

#include <D3dx9math.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


// For TSpriteData

#include "..\\..\\Rendering Managers\\dxutil.h"
#include "CTextureManager.h"
#include "..\\..\\..\\Global Managers\\Event Manager\\EventStructs.h"
#include "..\..\..\..\Components\Button\CButtonComponent.h"
#include "..\..\..\..\Components\Scroller\CScrollerComponent.h"
//#include "CTextureManager.h"
//#include "CBitmapFont.h"
#include "..\..\..\..\CObject.h"
#include "..\..\..\..\Enums.h"

class IEvent;
class CObjectManager;
class CObject;
class IComponent;
class CBitmapFontComp;

class CCreditManager
{
private: 
	CTextureManager*			m_pTM;
	CObjectManager*				m_pOM;
	CSpriteComponent*			m_pBMFont;
	CBitmapFontComp*			m_pCredits;
	float						m_fSeconds;
	float						m_fDT;
	CObject*					m_pCreditObj;
	CObject*					m_pCreditObj2;
	float						fX, fY;
	string						m_szCredits;
	//credit start
	CButtonComponent*			pButton;
	CScrollerComponent*			pScroller; 
	//credit middle
	CButtonComponent*			pButton2;
	CScrollerComponent*			pScroller2;
	//credit end
	CButtonComponent*			pButton3;
	CScrollerComponent*			pScroller3;

	CCreditManager(void);
	~CCreditManager(void);
	CCreditManager& operator=(const CCreditManager&);
	

public:

	static CCreditManager* GetInstance(void)
	{
		static CCreditManager pCreditInstance;
		return &pCreditInstance;
	}
	void Init(void);
	void Shutdown(void);
	void Update(void);
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
	static void InitCredits(IEvent* pEvent, IComponent* pComp);
	static void DisableCredits(IEvent* pEvent, IComponent* pComp);
};
#endif
/*

function CreateMenuButton(name, selectionEventName, 
buttonTextureName, nPositionX, nPositionY, bStartSelected, thisState, nTexDepth)

newButton = CreateObj(name, 0, 0, 0, 0);

print(name .. "Object Created")

buttonComponent = CreateButtonComponent(newButton, selectionEventName, buttonTextureName, nPositionX, nPositionY, bStartSelected, thisState, nTexDepth);

print("Button Component added to" .. name)

return buttonComponent
	
end

*/