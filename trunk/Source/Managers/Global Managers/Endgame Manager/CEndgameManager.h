////////////////////////////////////////////////////////////////////////////////
//	File			:	CEndgameManager.h
//	Date			:	6/17/11
//	Mod. Date		:	6/17/11
//	Mod. Initials	:	JL, HN, MS
//	Author			:	
//	Purpose			:	Encapsulates the Endgame Senarios
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CENDGAMEMANAGER_H_
#define _CENDGAMEMANAGER_H_

// Includes

// Foward Declares
class CSpriteComponent;
class IComponent;
class IEvent;
class CObject;

// End Game Manager
class CEndgameManager
{
private:

	// Flags and Timers
	bool	m_bExitingGameplay; // For Fading
	bool	m_bHumanWon;		// For Human player WINNING
	bool	m_bSoundPlayed;
	bool	m_bLoseFade;
	float	m_fFadeScreenAlpha;
	float	m_fWinLoseAlpha;
	float	m_fLoseFadeTimer;
	
	// Sprite Components
	CSpriteComponent* m_pFadeScreenComp;
	CSpriteComponent* m_pWinLoseComp;
	CSpriteComponent* m_pMainMenuComp;

	CObject*	m_pWinnerObject;
	CObject*	m_pLoserObject1;
	CObject*	m_pLoserObject2;
	CObject*	m_pLoserObject3;
	

	// Singleton
	CEndgameManager(void);
	~CEndgameManager(void);

	// Helper funcs
	void ResetSprites(void);


public:

	bool	m_bGameWon;		//for winning the game only once per play

	// Singleton
	static CEndgameManager* GetInstance(void) 
	{ 
		static CEndgameManager pcEGMan; 
		return &pcEGMan; 
	}

	// Initalize
	void Init(void);


	// Callbacks
	static void GameplayUpdateCallback(IEvent*, IComponent*);
	static void GameplayInitCallback(IEvent*, IComponent*);
	static void WinInitCallback(IEvent*, IComponent*);
	static void WinUpdateCallback(IEvent*, IComponent*);
	static void WinGameCallback(IEvent*, IComponent*);
	static void WinStateExitCallback(IEvent*, IComponent*);
	
	static void EnterPressedCallback(IEvent*, IComponent*);
};

#endif // _CENDGAMEMANAGER_H_
