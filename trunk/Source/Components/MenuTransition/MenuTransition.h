////////////////////////////////////////////////////////////////////////////////
//  File			:	"CMenuTransition.h"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	06/02/11
//
//	Last Changed	:	06/02/11
//
//	Changed By		:				
//
//  Purpose			:	Wrapper class for manageing Menu transitions screen
////////////////////////////////////////////////////////////////////////////////
#ifndef _CMENUTRANSITION_H_
#define _CMENUTRANSITION_H_
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\dxutil.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\Texture Managers\\CTextureManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

class CObjectManager;
class CObject;
class IEvent;
class IComponent;

class CMenuTransition
{
private:
	CTextureManager* m_pTM;
	CObjectManager*  m_pOM;
	CSpriteComponent* m_pFontImage;
	

	float m_fSeconds;
	float m_fDT;
	float m_dt;
	float m_fAlphaValue;

	CObject* m_pMenuObj;
	bool	faded;
	bool	m_bStartFading;
	bool	m_bGameMode;
	bool	m_bOptions;
	bool	m_bCredits;
	bool	m_bHowToPlay;

	CMenuTransition(void);
	~CMenuTransition(void);
	CMenuTransition& operator=(const CMenuTransition&);

	void InitMenu(void);
public:
	static CMenuTransition* GetInstance(void) 
	{ 
		static CMenuTransition pMenuTranInstance;
		return &pMenuTranInstance; 
	}

	void Init(void);
	static void Shutdown(IEvent* pEvent, IComponent* pComp);
	void Update(void);
	void SetStartFading(bool fade)
	{
		m_bStartFading = fade;
	}
	bool StartFading()
	{
		return m_bStartFading;
	}
	void SetFaded(bool isfade)
	{
		faded = isfade;
	}
	bool DoneFading(void)
	{
		return faded;
	}
	void Reset();
	
 static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
 static void GoToGameModeCallback(IEvent* pEvent, IComponent* pComp);
 static void GoToOptionsCallback(IEvent* pEvent, IComponent* pComp);
 static void GoToHowToPlayCallback(IEvent* pEvent, IComponent* pComp);
 static void GoToCreditCallback(IEvent* pEvent, IComponent* pComp);
 static void EnableMenuImage(IEvent* pEvent, IComponent* pComp);
};
#endif //_CMENUTRANSITION_H_