////////////////////////////////////////////////////////////////////////////////
//	File			:	CEndgameManager.cpp
//	Date			:	6/17/11
//	Mod. Date		:	6/17/11
//	Mod. Initials	:	JL, HN, MS
//	Author			:	
//	Purpose			:	Encapsulates the Endgame Senarios
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "CEndgameManager.h"
#include "..\\..\\..\\Components\\Rendering\\CSpriteComponent.h"
#include "..\\Event Manager\\CEventManager.h"
#include "..\\Event Manager\\EventStructs.h"
#include "..\\Object Manager\\CObjectManager.h"
#include "..\\..\\..\\CObject.h"
#include "..\\Rendering Managers\\Texture Managers\\CTextureManager.h"
#include "..\\Sound Manager\CWwiseSoundManager.h"
using namespace EventStructs;

///Defines
#define FADE_ALPHA_RATE (0.5f)
#define LOSE_FADE_TIME  (3.0f)

// Constructor
CEndgameManager::CEndgameManager(void) : m_bExitingGameplay(false), m_bLoseFade(false),
										 m_fFadeScreenAlpha(0.0f), m_pFadeScreenComp(NULL),
										 m_pWinnerObject(NULL), m_pLoserObject1(NULL),
										 m_pLoserObject2(NULL), m_pLoserObject3(NULL),
										 m_pWinLoseComp(NULL), m_pMainMenuComp(NULL),
										 m_fLoseFadeTimer(0.0f),
										 m_bHumanWon(false), m_fWinLoseAlpha(0.0f), m_bSoundPlayed(false)
{
}

// Destructor
CEndgameManager::~CEndgameManager(void)
{
}

// Helper funcs
void CEndgameManager::ResetSprites(void)
{
	// Update the Fade Screen Sprite
	TSpriteData pSData = m_pFadeScreenComp->GetSpriteData();
	pSData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	m_pFadeScreenComp->SetSpriteData(pSData);

	// Update the Win/Lose Sprite
	pSData = m_pWinLoseComp->GetSpriteData();
	pSData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	m_pWinLoseComp->SetSpriteData(pSData);

	// Update the Main Menu Sprite
	pSData = m_pMainMenuComp->GetSpriteData();
	pSData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	m_pMainMenuComp->SetSpriteData(pSData);
}

// Initalize
void CEndgameManager::Init(void) 
{
	m_bExitingGameplay = false;
	m_fFadeScreenAlpha = 0.0f;

	// Register for Events
	CEventManager* pEM = CEventManager::GetInstance();
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;

	pEM->RegisterEvent(szEventName, (IComponent*)this, GameplayUpdateCallback);
	
	szEventName = "InitObjects";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, (IComponent*)this, GameplayInitCallback);

	szEventName = "InitObjects";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, (IComponent*)this, WinInitCallback);

	szEventName = "Update";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, (IComponent*)this, WinUpdateCallback);

	szEventName = "GameWon";
	pEM->RegisterEvent(szEventName, (IComponent*)this, WinGameCallback);

	szEventName = "ShutdownObjects";
	szEventName += WIN_STATE;
	pEM->RegisterEvent(szEventName, (IComponent*)this, WinStateExitCallback); 

	szEventName = "AcceptWinState";
	pEM->RegisterEvent(szEventName, (IComponent*)this, EnterPressedCallback); 

	//Create object
	CObjectManager* pOM = CObjectManager::GetInstance();
	CObject* pFadeScreenObj = pOM->CreateObject("EndFadeScreen");

	CTextureManager* pTM = CTextureManager::GetInstance();
	/// Setup sprite data
	TSpriteData tSpriteData;
	tSpriteData.m_nTexture = pTM->LoadTexture("Resource\\HUD\\T_Black_Screen_D.png", 0);
	tSpriteData.m_fScaleX = 1.0f;
	tSpriteData.m_fScaleY = 1.0f;
	tSpriteData.m_nZ = 130;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_nX = 0;
	tSpriteData.m_nY = 0;
	tSpriteData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	tSpriteData.m_tRect.bottom = 0;
	tSpriteData.m_tRect.left   = 0;
	tSpriteData.m_tRect.right  = 0;
	tSpriteData.m_tRect.top    = 0;

	m_pFadeScreenComp = pTM->CreateSpriteComp(pFadeScreenObj, tSpriteData, false);

	CObject* pWinTextObj = pOM->CreateObject("WinText");

	tSpriteData.m_nTexture = pTM->LoadTexture("Resource\\HUD\\T_Endings_Words_D.png", 0);
	tSpriteData.m_fScaleX = 2.0f;
	tSpriteData.m_fScaleY = 2.0f;
	tSpriteData.m_fRotCenterX = 0.0f;
	tSpriteData.m_fRotCenterY = 0.0f;
	tSpriteData.m_fRot = 0.0f;
	tSpriteData.m_nX = 250;
	tSpriteData.m_nY = 10.0f;
	tSpriteData.m_nZ = 131;
	tSpriteData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	tSpriteData.m_tRect.top = 0;
	tSpriteData.m_tRect.bottom = 0;
	tSpriteData.m_tRect.left = 0;
	tSpriteData.m_tRect.right = 0;

	m_pWinLoseComp = pTM->CreateSpriteComp(pWinTextObj, tSpriteData, false);

	CObject* pMainMenuText = pOM->CreateObject("MainMenuText");

	tSpriteData.m_fScaleX = 1.25f;
	tSpriteData.m_fScaleY = 1.25f;
	tSpriteData.m_tRect.top = 190;
	tSpriteData.m_tRect.left = 10;
	tSpriteData.m_tRect.right = 300;
	tSpriteData.m_tRect.bottom = 265;
	tSpriteData.m_nX = 350;
	tSpriteData.m_nY = 650;
	
	m_pMainMenuComp = pTM->CreateSpriteComp(pMainMenuText, tSpriteData, false);

}



void CEndgameManager::GameplayUpdateCallback(IEvent* pEvent, IComponent* pComp)
{
	// Get Singletons
	CEndgameManager* pEGM = GetInstance();

	// Get the Data from the Event
	TUpdateStateEvent* tData = (TUpdateStateEvent*)pEvent->GetData();
	
	//Check to see if Gameplay is exiting
	if(GetInstance()->m_bExitingGameplay)
	{
		// Update Time
		float fDT = tData->m_fDeltaTime;
		
		// Increase Fade
		pEGM->m_fFadeScreenAlpha += fDT * FADE_ALPHA_RATE;
		if(pEGM->m_fFadeScreenAlpha > 1.0f)
		{
			// Change States
			if (!pEGM->m_bSoundPlayed && pEGM->m_bHumanWon)
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(GAMEPLAY_WIN, GLOBAL_ID);
			}
			if (!pEGM->m_bSoundPlayed && !pEGM->m_bHumanWon)
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(GAMEPLAY_LOSE, GLOBAL_ID);
			}

			SendStateEvent("PushState", (IComponent*)GetInstance(), WIN_STATE,
				PRIORITY_NORMAL);

			// Reset Values
			pEGM->m_bExitingGameplay = false;
			pEGM->m_fFadeScreenAlpha = 1.0f;
		}

		// Update the Sprite
		TSpriteData pSData = pEGM->m_pFadeScreenComp->GetSpriteData();
		pSData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, pEGM->m_fFadeScreenAlpha);
		pEGM->m_pFadeScreenComp->SetSpriteData(pSData);

	}
}

void CEndgameManager::GameplayInitCallback(IEvent*, IComponent*)
{
	// Get Singletons
	CEndgameManager* pEGM = GetInstance();

	// reset values so when game restarts, this stuff works fine
	pEGM->m_bExitingGameplay = false;
	pEGM->m_fFadeScreenAlpha = 0.0f;
	pEGM->m_fWinLoseAlpha = 0.0f;
	pEGM->m_pWinnerObject = NULL;
	pEGM->m_pLoserObject1 = NULL;
	pEGM->m_pLoserObject2 = NULL;
	pEGM->m_pLoserObject3 = NULL;
	pEGM->m_bLoseFade = false;
	pEGM->m_fLoseFadeTimer = 0.0f;
	pEGM->ResetSprites();

	// set all the components to invisible false
	pEGM->m_pFadeScreenComp->SetActive(false);
	pEGM->m_pMainMenuComp->SetActive(false);
	pEGM->m_pWinLoseComp->SetActive(false);
}

void CEndgameManager::WinInitCallback(IEvent*, IComponent*)
{
	CEndgameManager* pEGM = GetInstance();
	
	// Position Player by their Rank
	
	// Set up a matrix for the players (So they face the right way)
	D3DXMATRIX mMat;
	D3DXMatrixRotationAxis(&mMat, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), 3.1415*.25f);

	// winner player
	if(pEGM->m_pWinnerObject) // if not null
	{
		// winner is in center
		D3DXMatrixIdentity(&pEGM->m_pWinnerObject->GetTransform()->GetLocalMatrix());
	}
	// loser players
	if(pEGM->m_pLoserObject1) // if not null
	{
		// loser1 is in left back
		mMat._41 = -5.0f;
		mMat._43 -= 2.0f;
		pEGM->m_pLoserObject1->GetTransform()->GetLocalMatrix() = mMat;
	}
	if(pEGM->m_pLoserObject2) // if not null
	{
		mMat._41 = -2.0f;
		pEGM->m_pLoserObject2->GetTransform()->GetLocalMatrix() = mMat;
	}
	if(pEGM->m_pLoserObject3) // if not null
	{
		mMat._41 = 4.0f;
		pEGM->m_pLoserObject3->GetTransform()->GetLocalMatrix() = mMat;
	}
	
	// Set appropriate sprite data
	if(pEGM->m_bHumanWon)
	{
		// Player has beaten the game

		// Update the Sprite
		TSpriteData pSData = pEGM->m_pWinLoseComp->GetSpriteData();
		pSData.m_tRect.left   = 0;
		pSData.m_tRect.top    = 0;
		pSData.m_tRect.right  = 285;
		pSData.m_tRect.bottom = 85;
		pEGM->m_pWinLoseComp->SetSpriteData(pSData);
	}
	else
	{
		// Player Sux

		// Update the Sprite
		TSpriteData pSData = pEGM->m_pWinLoseComp->GetSpriteData();
		pSData.m_tRect.left   = 0;
		pSData.m_tRect.top    = 100;
		pSData.m_tRect.right  = 320;
		pSData.m_tRect.bottom = 185;
		pEGM->m_pWinLoseComp->SetSpriteData(pSData);
	}
	
	// Turn Sprites On
	pEGM->m_pWinLoseComp->SetActive(true);
	pEGM->m_pMainMenuComp->SetActive(true);

	// Set the camera to point at everyone
	SendObjectEvent("AttachToCameraWin", 
		(IComponent*)GetInstance(), pEGM->m_pWinnerObject);


	CWwiseSoundManager::GetInstance()->PlayMusic(MENU_MUSIC_PLAY, GLOBAL_ID);
	// 
	if(pEGM->m_bHumanWon)
	{
		// TODO: Do confetti here
		SendIEvent("HumanWinEffect", (IComponent*)pEGM, NULL, PRIORITY_NORMAL);
	}


}

void CEndgameManager::WinUpdateCallback(IEvent* pEvent, IComponent*)
{
	// Get Singletons
	CEndgameManager* pEGM = GetInstance();



	// Get Data from Event
	TUpdateStateEvent* tData = (TUpdateStateEvent*)pEvent->GetData();
	float fDT = tData->m_fDeltaTime;

	// Decrease Fade
	if(pEGM->m_bLoseFade == false && pEGM->m_fLoseFadeTimer == 0.0f)
	{
		pEGM->m_fFadeScreenAlpha -= fDT * FADE_ALPHA_RATE;
		if(pEGM->m_fFadeScreenAlpha < 0.0f)
		{
			pEGM->m_fFadeScreenAlpha = 0.0f;

			if(pEGM->m_bHumanWon == false)
			{
				//pEGM->m_bLoseFade = true;
				pEGM->m_fLoseFadeTimer = LOSE_FADE_TIME;
			}
		}

		// Update the Fade Screen Sprite
		TSpriteData pSData = pEGM->m_pFadeScreenComp->GetSpriteData();
		pSData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, pEGM->m_fFadeScreenAlpha);
		pEGM->m_pFadeScreenComp->SetSpriteData(pSData);
	}

	// Decrease Lose Fade Time
	if(pEGM->m_fLoseFadeTimer > 0.0f)
	{
		pEGM->m_fLoseFadeTimer -= fDT;
		if(pEGM->m_fLoseFadeTimer <= 0.0f)
		{
			//pEGM->m_fLoseFadeTimer = 0.0f;
			pEGM->m_bLoseFade = true;
		}
	}

	// Increase Win/Lose Fade
	if(pEGM->m_fFadeScreenAlpha == 0.0f)
	{
		pEGM->m_fWinLoseAlpha += fDT * FADE_ALPHA_RATE;
		if(pEGM->m_fWinLoseAlpha > 1.0f)
		{
			pEGM->m_fWinLoseAlpha = 1.0f;
		}

		// Update the Win/Lose Sprite
		TSpriteData pSData = pEGM->m_pWinLoseComp->GetSpriteData();
		pSData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, pEGM->m_fWinLoseAlpha);
		pEGM->m_pWinLoseComp->SetSpriteData(pSData);

		// Update the Main Menu Sprite
		pSData = pEGM->m_pMainMenuComp->GetSpriteData();
		pSData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, pEGM->m_fWinLoseAlpha);
		pEGM->m_pMainMenuComp->SetSpriteData(pSData);
	}

	// Lose Fade
	if(pEGM->m_bLoseFade == true)
	{
		pEGM->m_fFadeScreenAlpha += fDT * FADE_ALPHA_RATE;
		if(pEGM->m_fFadeScreenAlpha > 0.5f)
		{
			pEGM->m_fFadeScreenAlpha = 0.5f;
			//pEGM->m_bLoseFade = false;

		}

		// Update the Fade Screen Sprite
		TSpriteData pSData = pEGM->m_pFadeScreenComp->GetSpriteData();
		//pSData.m_nZ = 110;
		pSData.m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, pEGM->m_fFadeScreenAlpha);
		pEGM->m_pFadeScreenComp->SetSpriteData(pSData);
	}

	// Tell the animations to animate, sending win anim to win obj and lose anim to lose obj
	SendInputEvent("TigerBlood", (IComponent*)pEGM, pEGM->m_pWinnerObject, 0.0f);
	SendInputEvent("GoodDaySir", (IComponent*)pEGM, pEGM->m_pLoserObject1, 0.0f);
	SendInputEvent("GoodDaySir", (IComponent*)pEGM, pEGM->m_pLoserObject2, 0.0f);
	SendInputEvent("GoodDaySir", (IComponent*)pEGM, pEGM->m_pLoserObject3, 0.0f);


}

void CEndgameManager::WinGameCallback(IEvent* pEvent, IComponent*)
{

	CEndgameManager* pEGM = GetInstance();

	if(pEGM->m_bGameWon)
		return;		//somebody already won this round
	pEGM->m_bGameWon = true;//somebody just won this round

	TInputEvent* tData = (TInputEvent*)pEvent->GetData();
	// Get the player who won from the event
	// store that as the winning player
	pEGM->m_pWinnerObject = tData->m_pPlayer;
	// Send the input event passing the player object who is winning

	// set the losing players to the rest of the players
	CObject* pThePlayers[4];
	pThePlayers[0] = CObjectManager::GetInstance()->GetObjectByName("Player0");
	pThePlayers[1] = CObjectManager::GetInstance()->GetObjectByName("Player1");
	pThePlayers[2] = CObjectManager::GetInstance()->GetObjectByName("Player2");
	pThePlayers[3] = CObjectManager::GetInstance()->GetObjectByName("Player3");

	// set our pointers to the proper objects
	if(pEGM->m_pWinnerObject == pThePlayers[0])
	{
		pEGM->m_pLoserObject1 = pThePlayers[1];
		pEGM->m_pLoserObject2 = pThePlayers[2];
		pEGM->m_pLoserObject3 = pThePlayers[3];
	}
	else if(pEGM->m_pWinnerObject == pThePlayers[1])
	{
		pEGM->m_pLoserObject1 = pThePlayers[0];
		pEGM->m_pLoserObject2 = pThePlayers[2];
		pEGM->m_pLoserObject3 = pThePlayers[3];
	}
	else if(pEGM->m_pWinnerObject == pThePlayers[2])
	{
		pEGM->m_pLoserObject1 = pThePlayers[0];
		pEGM->m_pLoserObject2 = pThePlayers[1];
		pEGM->m_pLoserObject3 = pThePlayers[3];
	}
	else
	{
		pEGM->m_pLoserObject1 = pThePlayers[0];
		pEGM->m_pLoserObject2 = pThePlayers[1];
		pEGM->m_pLoserObject3 = pThePlayers[2];
	}

	GetInstance()->m_bExitingGameplay = true;

		// Make the fade thingy show	
	pEGM->m_pFadeScreenComp->SetActive(true);

	pEGM->m_bHumanWon = (pThePlayers[0] == pEGM->m_pWinnerObject);

	if (!pEGM->m_bSoundPlayed && pEGM->m_bHumanWon)
	{
		CWwiseSoundManager::GetInstance()->PlayTheSound(GAMEPLAY_WIN, GLOBAL_ID);
		pEGM->m_bSoundPlayed = true;
	}
	if (!pEGM->m_bSoundPlayed && !pEGM->m_bHumanWon)
	{
		CWwiseSoundManager::GetInstance()->PlayTheSound(GAMEPLAY_LOSE, GLOBAL_ID);
		pEGM->m_bSoundPlayed = true;
	}

	SendIEvent("WonGame", (IComponent*)GetInstance(), NULL, PRIORITY_NORMAL);


}

void CEndgameManager::WinStateExitCallback(IEvent*, IComponent*)
{
	// Get Singletons
	CEndgameManager* pEGM = CEndgameManager::GetInstance();

	// Turn off the Sprites
	pEGM->m_pFadeScreenComp->SetActive(false);
	pEGM->m_pMainMenuComp->SetActive(false);
	pEGM->m_pWinLoseComp->SetActive(false);
	pEGM->m_bSoundPlayed = false;
	pEGM->m_bGameWon = false;
}

void CEndgameManager::EnterPressedCallback(IEvent*, IComponent*)
{
	// Get Singletons
	CEndgameManager* pEGM = CEndgameManager::GetInstance();

	// if we've hit enter and we're in the state that this listens for, change states to menu state
	SendStateEvent("StateChange", (IComponent*)pEGM, MAIN_MENU_STATE, PRIORITY_IMMEDIATE);
}