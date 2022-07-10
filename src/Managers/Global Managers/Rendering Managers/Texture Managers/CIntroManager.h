////////////////////////////////////////////////////////////////////////////////
//  File			:	"CIntroManager.h"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	05/18/11
//
//	Last Changed	:	07/24/11
//
//	Changed By		:	HN			
//
//  Purpose			:	Wrapper class for manageing Intro screen
////////////////////////////////////////////////////////////////////////////////
#ifndef _CINTROMANAGER_H_
#define _CINTROMANAGER_H_
#include "..\\..\\Rendering Managers\\dxutil.h"

class CObjectManager;
class CObject;
class IEvent;
class IComponent;
class CTextureManager;
class CSpriteComponent;

class CIntroManager
{
private:
	CTextureManager* m_pTM;					
	CObjectManager*  m_pOM;				
	CSpriteComponent* m_pGPSplash;			
	CSpriteComponent* m_pShatteredSplash;
	CSpriteComponent* m_pWwiseSplash;
	CSpriteComponent* m_pFalconSplash;
	CSpriteComponent* m_pTitleScreen;
	CSpriteComponent* m_pPressEnter;

	float m_fSeconds;
	float m_fDT;
	float m_dt;
	float m_fAlphaValue;
	float m_fTitleBarAlphValue;
	bool m_bEnterMenu;
	bool m_pHasPlayed;

	int nTexID1;
	int nTexID2;
	int nTexID3;
	int nTexID4;
	int nTexID5;
	int nTexID6;


	CObject* m_pIntroObj;
	//Constructor
	CIntroManager(void);
	//Destructor
	~CIntroManager(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function:"InitIntro" 
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose:  Loads all the textures 
	//    
	////////////////////////////////////////////////////////////////////////////////
	void InitIntro(void);

public:
	////////////////////////////////////////////////////////////////////////////////
	// Function: "GetInstance"
	//
	// Return: static CIntroManager - instance of the manager
	//
	// Parameters: void
	//
	// Purpose:  gets an instance of the manager
	//    
	////////////////////////////////////////////////////////////////////////////////
	static CIntroManager* GetInstance(void) 
	{ 
		static CIntroManager pIntroInstance;
		return &pIntroInstance; 
	}
	////////////////////////////////////////////////////////////////////////////////
	// Function: "Init"
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose: Registers for events and initializes all the instances
	//    
	////////////////////////////////////////////////////////////////////////////////
	void Init(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "Update"
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose: Updates the state and fades the splash screens
	//    
	////////////////////////////////////////////////////////////////////////////////
	void Update(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "PlayMusic"
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose:  Plays the menu music
	//    
	////////////////////////////////////////////////////////////////////////////////
	void PlayMusic(void);
	////////////////////////////////////////////////////////////////////////////////
	// Function: "ChangeToMainMenu"
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose:  Pops the intro state and enters MainMenu
	//    
	////////////////////////////////////////////////////////////////////////////////
	void ChangeToMainMenu(void);

	//Event Callback functions
	static void EnterMenu(IEvent* pEvent, IComponent* pComp);
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp); 
	static void Shutdown(IEvent* pEvent, IComponent* pComp);
};

#endif //C_INTROMANAGER_H_