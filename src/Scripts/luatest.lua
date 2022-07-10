-- Lua File for initializing Game Objects

function InitValues()
	
	-- To create an object,
	-- name it in CreateObj	
	unlocked = false;
	radius = 0.8
	
	P0X = -6;
	P0Y = 0;
	P0Z = 68;
	P0Rot = 3.14159;
	
	P1X = 0;
	P1Y = 0;
	P1Z = 68;
	P1Rot = 3.14159;
	
	P2X = 6;
	P2Y = 0;
	P2Z = 68;
	P2Rot = 3.14159;
	
	P3X = 12;
	P3Y = 0;
	P3Z = 68;
	P3Rot = 3.14159;

	
	-- Removed Shopping list in here - Agent Smith

	-- Player Creation	
	-- CreatePlayerObj(ObjName, XPos, YPos, ZPos, Rot, IsHuman)
	Player0 = CreatePlayerObj("Player0", P0X, P0Y, P0Z, P0Rot, true);
	Player1 = CreatePlayerObj("Player1", P1X, P1Y, P1Z, P1Rot, false);
	Player2 = CreatePlayerObj("Player2", P2X, P2Y, P2Z, P2Rot, false);
	Player3 = CreatePlayerObj("Player3", P3X, P3Y, P3Z, P3Rot, false);
	
	-- Intro State
	stateEventIntroState = 48 -- ascii for '0'
	-- main menu
	stateEventMainMenu = stateEventIntroState + 1
	-- gameplay
	stateEventGameplay = stateEventMainMenu + 1
	-- pause state - 3
	stateEventPause = stateEventGameplay + 1
	-- win state
	stateEventWin = stateEventPause + 1
	-- lose state
	stateEventLose = stateEventWin + 1
	-- console state
	stateEventConsole = stateEventLose + 1
	-- game mode select
	stateEventGameModeSelect = stateEventConsole + 1
	-- character select
	stateEventCharacterSelect = stateEventGameModeSelect + 1
	-- character select 2
	stateEventCharacterSelect2 = stateEventCharacterSelect + 1
	-- character select 3
	stateEventCharacterSelect3 = stateEventCharacterSelect2 + 1
	-- character select 4
	stateEventCharacterSelect4 = stateEventCharacterSelect3 + 1
	-- character select 5
	stateEventCharacterSelect5 = stateEventCharacterSelect4 + 1
	-- character select 6
	stateEventCharacterSelect6 = stateEventCharacterSelect5 + 1
	-- character select 7
	stateEventCharacterSelect7 = stateEventCharacterSelect6 + 1
	-- options
	stateEventOptions = stateEventCharacterSelect7 + 1
	-- credits
	stateEventCredits = stateEventOptions + 1
	-- keybinds
	stateEventKeybinds = stateEventCredits + 1
	-- how to play 0
	stateEventHowToPlay = stateEventKeybinds + 1
	-- how to play 1
	stateEventHowToPlay1 = stateEventHowToPlay + 1
	-- how to play 2
	stateEventHowToPlay2 = stateEventHowToPlay1 + 1
	-- how to play 3
	stateEventHowToPlay3 = stateEventHowToPlay2 + 1
	-- how to play 4
	stateEventHowToPlay4 = stateEventHowToPlay3 + 1
	-- how to play 5
	stateEventHowToPlay5 = stateEventHowToPlay4 + 1
	-- how to play 6
	stateEventHowToPlay6 = stateEventHowToPlay5 + 1
	-- how to play 7
	stateEventHowToPlay7 = stateEventHowToPlay6 + 1
	-- how to play 8
	stateEventHowToPlay8 = stateEventHowToPlay7 + 1
	-- how to play 9
	stateEventHowToPlay9 = stateEventHowToPlay8 + 1
	-- how to play 10
	stateEventHowToPlay10 = stateEventHowToPlay9 + 1
	-- how to play 11
	stateEventHowToPlay11 = stateEventHowToPlay10 + 1
	-- how to play 12
	stateEventHowToPlay12 = stateEventHowToPlay11 + 1
	-- how to play 13
	stateEventHowToPlay13 = stateEventHowToPlay12 + 1
	-- how to play 14
	stateEventHowToPlay14 = stateEventHowToPlay13 + 1
	-- how to play 15
	stateEventHowToPlay15 = stateEventHowToPlay14 + 1
	-- pause options menu state
	statePauseOptions = stateEventHowToPlay15 + 1
	-- pause keybinds menu state
	statePauseKeybinds = statePauseOptions + 1
	-- in game how to play state
	stateInGameHowToPlay = statePauseKeybinds + 1
	-- level select state
	stateLevelSelect = stateInGameHowToPlay + 1
	-- level select state2
	stateLevelSelect2 = stateLevelSelect + 1
	-- Quit Confirmation menu
	stateEventQuitConfirmation = stateLevelSelect2 + 1
	
	
	InitIntroState()
	InitMainMenu()
	InitWinGameMenu()
	InitLoseGameMenu()
	InitPauseMenu()
	InitOptionsMenu()
	InitKeybindsMenu()
	InitCreditsMenu()
	InitCharacterSelectMenu()
	InitGameModeSelectMenu()
	InitHowToPlayMenu()
	InitPauseOptionsMenu()
	InitOptionsKeybindsMenu()
	InitLevelSelectMenu()
	InitQuitConfirmationMenu()
	
end
function InitIntroState()
	
	-- 0 is the Intro State
	--local thisState = stateEventIntroState;
	
	--local background = CreateMenuButton("GPSplash", "", "", "T_Black_Screen_D.PNG", 0, 0, false, stateEventIntroState, 2);

end

function InitMainMenu()
	
	-- 1 is main menu state
	local thisState = stateEventMainMenu;
	
	-- From main to Options
	
	local background = CreateMenuButton("MainMenuBackground", "", "", "T_Main_Menu_D.png", 0, 0, false, stateEventMainMenu, -1);
	local playGameButton = CreateMenuButton("PlayGameButton", "LuaPushState(stateEventCharacterSelect);", "",  "", 290, 245, true, stateEventMainMenu, 3);
	local optionsGameButton = CreateMenuButton("OptionsButton", "LuaPushState(stateEventOptions);", "",  "", 325, 340, false, stateEventMainMenu, 3);	
	local howToPlayButton = CreateMenuButton("HowToPlayButton", "LuaPushState(stateEventHowToPlay);", "",  "", 380, 425, false, stateEventMainMenu, 3);
	local creditsButton = CreateMenuButton("CreditsButton", "LuaPushState(stateEventCredits);", "",  "", 290, 515, false, stateEventMainMenu, 3);
	local exitGameButton = CreateMenuButton("ExitGameButton", "LuaPushState(stateEventQuitConfirmation);", "",  "", 260, 615, false, stateEventMainMenu, 3);
	
	SetNextButtonComponent(playGameButton, optionsGameButton, exitGameButton, 0, 0);
	SetNextButtonComponent(optionsGameButton, howToPlayButton, playGameButton, 0, 0);
	SetNextButtonComponent(howToPlayButton, creditsButton, optionsGameButton, 0, 0 );
	SetNextButtonComponent(creditsButton, exitGameButton, howToPlayButton, 0, 0);
	SetNextButtonComponent(exitGameButton, playGameButton, creditsButton, 0, 0);
end


function InitLevelSelectMenu()
	
	-- NOT CALLED ANYMORE
	
	--[[
	local thisState1 = stateLevelSelect;
	local thisState2 = stateLevelSelect2;
	
	
	local background = CreateMenuButton("MainMenuBackground", "", "", "CharacterSelect/T_Select_Level_D.png", 0, 0, false, thisState1, -1);
	local level1Button = CreateMenuButton("Level1Button", "LuaSendEvent(\"Level1Selected\");", "LuaStateChange(stateLevelSelect);",  "", 170, 220, true, thisState1, 3);
	local level2Button = CreateMenuButton("Level2Button", "LuaStateChange(stateEventGameplay);", "LuaStateChange(stateLevelSelect2);",  "", 370, 220, false, thisState1, 3);
	local backButton = CreateMenuButton("LevelSelectBackButton", "FromLevelSelectToCharSelect()", "",  "", 150, 625, false, thisState1, 3);
	
	SetNextButtonComponent(level1Button, backButton, backButton, level2Button, level2Button);
	SetNextButtonComponent(level2Button, backButton, backButton, level1Button, level1Button);
	SetNextButtonComponent(backButton, level1Button, level1Button, 0, 0 );
	
	local background2 = CreateMenuButton("MainMenuBackground2", "", "", "CharacterSelect/T_Select_Level2_D.png", 0, 0, false, thisState2, -1);
	local level1Button2 = CreateMenuButton("Level1Button2", "LuaStateChange(stateEventGameplay);", "LuaStateChange(stateLevelSelect);",  "", 170, 220, false, thisState2, 3);
	local level2Button2 = CreateMenuButton("Level2Button2", "LuaSendEvent(\"Level2Selected\");", "LuaStateChange(stateLevelSelect2);",  "", 370, 220, true, thisState2, 3);
	local backButton2 = CreateMenuButton("LevelSelectBackButton2", "FromLevelSelectToCharSelect()", "",  "", 150, 625, false, thisState2, 3);
	
	SetNextButtonComponent(level1Button2, backButton2, backButton2, level2Button2, level2Button2);
	SetNextButtonComponent(level2Button2, backButton2, backButton2, level1Button2, level1Button2);
	SetNextButtonComponent(backButton2, level2Button2, level2Button2, 0, 0 );
	
	]]--
	
end


function InitWinGameMenu()
	
	-- 4 is win state
	--local thisState = stateEventWin;
	
	--local background		= CreateMenuButton("WinBackground", "", "", 	"T_Bikers_Win_D.png", 0, 0, false, stateEventWin, 2);
	--local playAgainButton	= CreateMenuButton("WinPlayAgainButton", "LuaStateChange(stateEventMainMenu);", "",  "",	70, 700, true, stateEventWin, 3);
	--local winExitGameButton = CreateMenuButton("WinExitButton",	"LuaSendEvent(\"ShutdownGame\");", "",  "", 925, 700, false, stateEventWin, 3);
	
	--SetNextButtonComponent(playAgainButton, winExitGameButton, winExitGameButton, winExitGameButton, winExitGameButton);
	--SetNextButtonComponent(winExitGameButton, playAgainButton, playAgainButton, playAgainButton, playAgainButton);
end

function InitLoseGameMenu()
	
	-- 5 is lose state
	--local thisState = stateEventLose;
	
	--local background = CreateMenuButton("LoseBackground", "",  "", "T_Bikers_Lose_D.png", 0, 0, false, stateEventLose, 2);
	--local playAgainButton = CreateMenuButton("WinPlayAgainButton", "LuaStateChange(stateEventMainMenu);", "",  "", 70, 700, true, stateEventLose, 3);
	--local loseExitGameButton = CreateMenuButton("WinExitButton", "LuaSendEvent(\"ShutdownGame\");", "",  "", 925, 700, false, stateEventLose, 3);
	
	--SetNextButtonComponent(playAgainButton, loseExitGameButton, loseExitGameButton, loseExitGameButton, loseExitGameButton);
	--SetNextButtonComponent(loseExitGameButton, playAgainButton, playAgainButton, playAgainButton, playAgainButton);
end

function InitPauseMenu()
	
	-- 3 is pause state
	--local thisState = stateEventPause;
	
	local background = CreateMenuButton("PauseBackground",  "", "", "T_Pause_Page_1_D.png", 0, 0, false, stateEventPause, 4);
	local resumeButton = CreateMenuButton("PauseResumeAgainButton", "LuaBack();", "",  "", 385, 280, true, stateEventPause, 5);
	local optionsButton = CreateMenuButton("PauseOptionsButton", "LuaPushState(statePauseOptions);",  "", "", 395, 355, false, stateEventPause, 5);
	local mainMenuButton = CreateMenuButton("PauseMainMenuButton", "LuaStateChange(stateEventMainMenu);", "",  "", 365, 425, false, stateEventPause, 5);
	local quitMenuButton = CreateMenuButton("PauseQuitButton", "LuaPushState(stateEventQuitConfirmation)", "",  "", 360, 510, false, stateEventPause, 5);
		
	SetNextButtonComponent(resumeButton, optionsButton, quitMenuButton, 0, 0);
	SetNextButtonComponent(optionsButton, mainMenuButton, resumeButton, 0, 0);
	SetNextButtonComponent(mainMenuButton, quitMenuButton, optionsButton, 0, 0);
	SetNextButtonComponent(quitMenuButton, resumeButton, mainMenuButton, 0, 0);
end


function InitQuitConfirmationMenu()
	
	-- 3 is pause state
	--local thisState = stateEventPause;
	
	local background = CreateMenuButton("QuitConfBackground",  "", "", "T_Quit_Game_D.png", 250, 180, false, stateEventQuitConfirmation, 4);
	local yesbutton = CreateMenuButton("QuitConfYesButton", "ShutdownGameFromPause();", "",  "", 440, 470, false, stateEventQuitConfirmation, 5);
	local nobutton = CreateMenuButton("QuitConfNoButton", "LuaBack()",  "", "", 590, 470, true, stateEventQuitConfirmation, 5);
		
	SetNextButtonComponent(yesbutton, nobutton, nobutton, nobutton, nobutton);
	SetNextButtonComponent(nobutton, yesbutton, yesbutton, yesbutton, yesbutton);
	
end


function InitPauseOptionsMenu()
	
	
	local background = CreateMenuButton("PauseOptionsBackground", "", "",  "T_Pause_Page_2_D.png", 0, 0, false, statePauseOptions, 2);
	--local button1 = CreateMenuButton("OptionsMusicVolume", "", "MainMenuOptions.png", 350, 300, true, statePauseOptions, 3);
	--local button2 = CreateMenuButton("OptionsSFXVolume", "", "MainMenuOptions.png", 350, 375, false, statePauseOptions, 3);
	 PauseMusicVolButton = CreateMenuSlider("PauseOptionsMusicVolume", "SetMusicVolume", "T_Pause_Icon_D.png", 280, 230, 700, 230, true, statePauseOptions, 1, 3);
	 PauseSFXVolButton   = CreateMenuSlider("PauseOptionsSFXVolume", "SetSoundVolume", "T_Pause_Icon_D.png", 285, 300, 715, 300, false, statePauseOptions, 2, 3);
	 PauseGammaButton    = CreateMenuSlider("PauseOptionsGammaValue", "ChangeGamma",  "T_Pause_Icon_D.png", 280, 375, 700, 375, false, statePauseOptions, 3, 3);
	 PauseKeybindsButton = CreateMenuButton("PauseOptionsKeybindButton", "PushState(statePauseKeybinds);",  "", "", 200, 450, false, statePauseOptions, 3);
	 PauseDefaultButton  = CreateMenuButton("PauseOptionsDefaultButton", "ResetDefaultOptions(PauseMusicVolButton, PauseSFXVolButton, PauseGammaButton);", "",  "", 200, 510, false, statePauseOptions, 3);
	PauseBackButton     = CreateMenuButton("PauseOptionsReturnMenuButton", "LuaBack();", "",  "", 200, 565, false, statePauseOptions, 3);
	PauseCancelButton     = CreateMenuButton("PauseOptionsCancelButton", "LuaCancelBackPause();", "",  "", 200, 625, false, statePauseOptions, 3);
	
	ResetDefaultOptions(PauseMusicVolButton, PauseSFXVolButton, PauseGammaButton);
		
	SetNextButtonComponent(PauseMusicVolButton,		PauseSFXVolButton,		PauseCancelButton,		0, 0);
	SetNextButtonComponent(PauseSFXVolButton,		PauseGammaButton,		PauseMusicVolButton,	0, 0);
	SetNextButtonComponent(PauseGammaButton,		PauseKeybindsButton,	PauseSFXVolButton,		0, 0);
	SetNextButtonComponent(PauseKeybindsButton,		PauseDefaultButton,		PauseGammaButton,		0, 0);
	SetNextButtonComponent(PauseDefaultButton,		PauseBackButton,		PauseKeybindsButton,	0, 0);
	SetNextButtonComponent(PauseBackButton,			PauseCancelButton,	PauseDefaultButton,		0, 0);
	SetNextButtonComponent(PauseCancelButton,			PauseMusicVolButton,	PauseBackButton,		0, 0);
end



function InitOptionsKeybindsMenu()
	
	-- 11 is keybind state
	
	local background = CreateMenuButton("PauseKeybindsBackground", "",  "", "T_Pause_Page_3_D.png", 0, 0, false, statePauseKeybinds, 2);
	local button1 = CreateMenuButton("PauseKeybindButton1", "SetKey(0);","",  "", 190, 275, true, statePauseKeybinds, 3);
	local button2 = CreateMenuButton("PauseKeybindButton2", "SetKey(1);", "", "", 190, 330, false, statePauseKeybinds, 3);
	local button3 = CreateMenuButton("PauseKeybindButton3", "SetKey(2);", "", "", 190, 385, false, statePauseKeybinds, 3);
	local button4 = CreateMenuButton("PauseKeybindButton4", "SetKey(3);", "", "", 190, 440, false, statePauseKeybinds, 3);
	local button5 = CreateMenuButton("PauseKeybindButton5", "SetKey(4);", "", "", 190, 500, false, statePauseKeybinds, 3);
	local button6 = CreateMenuButton("PauseKeybindButton6", "SetKey(5);", "", "", 575, 275, false, statePauseKeybinds, 3);
	local button7 = CreateMenuButton("PauseKeybindButton7", "SetKey(6);", "", "", 575, 325, false, statePauseKeybinds, 3);
	local button8 = CreateMenuButton("PauseKeybindButton8", "SetKey(7);", "", "", 575, 380, false, statePauseKeybinds, 3);
	local button9 = CreateMenuButton("PauseKeybindButton9", "SetKey(8);", "", "", 575, 440, false, statePauseKeybinds, 3);
	local button10 = CreateMenuButton("PauseKeybindButton10", "SetDefaultKeys();", "", "", 565, 550, false, statePauseKeybinds, 3);
	local button11 = CreateMenuButton("PauseKeybindsReturnMenuButton", "LuaBack();",  "", "", 195, 570, false, statePauseKeybinds, 3);
	
	--						me		down		up		left	right
	SetNextButtonComponent(button1, button2, button11, button11, button6);
	SetNextButtonComponent(button2, button3, button1, button11, button7);
	SetNextButtonComponent(button3, button4, button2, button11, button8);
	SetNextButtonComponent(button4, button5, button3, button11, button9);
	SetNextButtonComponent(button5, button11, button4, button11, button10);
	
	SetNextButtonComponent(button6, button7, button5, button1, button11);
	SetNextButtonComponent(button7, button8, button6, button2, button11);
	SetNextButtonComponent(button8, button9, button7, button3, button11);
	SetNextButtonComponent(button9, button10, button8, button4, button11);
	SetNextButtonComponent(button10, button11, button9, button5, button11);
	SetNextButtonComponent(button11, button1, button5, button10, button10);
end


function InitOptionsMenu()
	
	-- 9 is options state
	
	-- CreateMenuSlider(name, selectionEventName, buttonTextureName, nStartX, nStartY, nEndX, nEndY, bStartSelected, stateEventOptions, nTexDepth)
	
	
	local background = CreateMenuButton("OptionsBackground", "", "",  "T_Main_Options_D.png", 0, 0, false, stateEventOptions, 2);
	--local button1 = CreateMenuButton("OptionsMusicVolume", "", "MainMenuOptions.png", 350, 300, true, stateEventOptions, 3);
	--local button2 = CreateMenuButton("OptionsSFXVolume", "", "MainMenuOptions.png", 350, 375, false, stateEventOptions, 3);
	MusicVolButton = CreateMenuSlider("OptionsMusicVolume", "SetMusicVolume", "T_Pause_Icon_D.png", 255, 160, 745, 160, true, stateEventOptions, 1, 3);
	SFXVolButton   = CreateMenuSlider("OptionsSFXVolume", "SetSoundVolume", "T_Pause_Icon_D.png", 255, 250, 745, 250, false, stateEventOptions, 2, 3);
	GammaButton    = CreateMenuSlider("OptionsGammaValue", "ChangeGamma",  "T_Pause_Icon_D.png", 255, 350, 745, 350, false, stateEventOptions, 3, 3);
	KeybindsButton = CreateMenuButton("OptionsKeybindButton", "PushState(stateEventKeybinds);",  "", "", 50, 450, false, stateEventOptions, 3);
	BackButton     = CreateMenuButton("OptionsReturnMenuButton", "LuaBack();", "",  "", 75, 525, false, stateEventOptions, 3);
	DefaultButton  = CreateMenuButton("OptionsDefaultButton", "ResetDefaultOptions(MusicVolButton, SFXVolButton, GammaButton);", "",  "", 75, 605, false, stateEventOptions, 3);
	CancelButton     = CreateMenuButton("OptionsCancelButton", "LuaCancelBackMain();", "",  "", 75, 685, false, stateEventOptions, 3);
		
	ResetDefaultOptions(MusicVolButton, SFXVolButton, GammaButton);
	
	SetNextButtonComponent(MusicVolButton, SFXVolButton, CancelButton, 0, 0);
	SetNextButtonComponent(SFXVolButton, GammaButton, MusicVolButton, 0, 0);
	SetNextButtonComponent(GammaButton, KeybindsButton, SFXVolButton, 0, 0);
	SetNextButtonComponent(KeybindsButton, BackButton, GammaButton, BackButton, DefaultButton);
	SetNextButtonComponent(BackButton, DefaultButton, KeybindsButton, KeybindsButton, BackButton);
	SetNextButtonComponent(DefaultButton, CancelButton, BackButton, CancelButton, KeybindsButton);
	SetNextButtonComponent(CancelButton, MusicVolButton, DefaultButton, DefaultButton, KeybindsButton);
end



function InitKeybindsMenu()
	
	-- 11 is keybind state
	
	local background = CreateMenuButton("KeybindsBackground", "",  "", "T_Control_Setup_D.png", 0, 0, false, stateEventKeybinds, 2);
	local button1 = CreateMenuButton("KeybindButton1", "SetKey(0);","",  "", 331, 321, true, stateEventKeybinds, 3);
	local button2 = CreateMenuButton("KeybindButton2", "SetKey(1);", "", "", 331, 388, false, stateEventKeybinds, 3);
	local button3 = CreateMenuButton("KeybindButton3", "SetKey(2);", "", "", 331, 450, false, stateEventKeybinds, 3);
	local button4 = CreateMenuButton("KeybindButton4", "SetKey(3);", "", "", 331, 525, false, stateEventKeybinds, 3);
	local button5 = CreateMenuButton("KeybindButton5", "SetKey(4);", "", "", 331, 595, false, stateEventKeybinds, 3);
	local button6 = CreateMenuButton("KeybindButton6", "SetKey(5);", "", "", 680, 300, false, stateEventKeybinds, 3);
	local button7 = CreateMenuButton("KeybindButton7", "SetKey(6);", "", "", 680, 345, false, stateEventKeybinds, 3);
	local button8 = CreateMenuButton("KeybindButton8", "SetKey(7);", "", "", 680, 390, false, stateEventKeybinds, 3);
	local button9 = CreateMenuButton("KeybindButton9", "SetKey(8);", "", "", 680, 435, false, stateEventKeybinds, 3);
	local button10 = CreateMenuButton("KeybindButton10", "SetDefaultKeys();", "", "", 720, 580, false, stateEventKeybinds, 3);
	local button11 = CreateMenuButton("KeybindsReturnMenuButton", "LuaBack();",  "", "", 150, 625, false, stateEventKeybinds, 3);
	
	--						me		down		up		left	right
	SetNextButtonComponent(button1, button2, button11, button11, button6);
	SetNextButtonComponent(button2, button3, button1, button11, button7);
	SetNextButtonComponent(button3, button4, button2, button11, button8);
	SetNextButtonComponent(button4, button5, button3, button11, button9);
	SetNextButtonComponent(button5, button6, button4, button11, button10);
	
	SetNextButtonComponent(button6, button7, button5, button1, button11);
	SetNextButtonComponent(button7, button8, button6, button2, button11);
	SetNextButtonComponent(button8, button9, button7, button3, button11);
	SetNextButtonComponent(button9, button10, button8, button4, button11);
	SetNextButtonComponent(button10, button11, button9, button5, button11);
	SetNextButtonComponent(button11, button1, button10, button10, button1);
end

function InitGameModeSelectMenu()
	
	-- 7 is game mode select state
	
	--local background = CreateMenuButton("GameModeBackground", "", "T_Game_Mode_D.PNG", 0, 0, false, thisState, 2);
	
	-- HACK: StateChange should be push state but it hides behind the other one.
	local button1 = CreateMenuButton("GameDerbyModeButton", "LuaPushState(stateEventCharacterSelect);",  "", "", 325, 315, true, stateEventGameModeSelect, 3);
	local button2 = CreateMenuButton("GameFoodFightModeButton", "",  "", "", 330, 475, false, stateEventGameModeSelect, 3);
	local button3 = CreateMenuButton("GamePlayerDecreaseButton", "", "",  "", 610, 525, false, stateEventGameModeSelect, 3);
	local button4 = CreateMenuButton("GamePlayerIncreaseButton", "", "",  "", 770, 525, false, stateEventGameModeSelect, 3);
	local button5 = CreateMenuButton("GameReturnButton", "LuaBack();",  "", "", 900, 550, false, stateEventGameModeSelect, 3);
	
	SetNextButtonComponent(button1, button3, button5, button2, button2);
	SetNextButtonComponent(button2, button4, button5, button1, button1);
	SetNextButtonComponent(button3, button5, button1, button4, button4);
	SetNextButtonComponent(button4, button5, button2, button3, button3);
	SetNextButtonComponent(button5, button1, button3, 0, 0);
end


function InitCharacterSelectMenu()
	
	-- 8 is character select state

	-- Select Bikers -- State 8
	local bground = CreateMenuButton("CharSelectBackground1",		"", "", "CharacterSelect/T_Character_Select_BullDog_D.png", 0, 0, false, stateEventCharacterSelect, 2);
	local button1 = CreateMenuButton("CharacterBulldogButton1",		"CreateBikersObj(Player0, 0);", "",  "", 130, 180, true, stateEventCharacterSelect, 3);
	local button2 = CreateMenuButton("CharacterBanditosButton1",	"LuaStateChange(stateEventCharacterSelect2)",  "LuaStateChange(stateEventCharacterSelect2)", "", 475, 170, false, stateEventCharacterSelect, 3);
	local button3 = CreateMenuButton("CharacterScientistButton1",	"LuaStateChange(stateEventCharacterSelect3);", "LuaStateChange(stateEventCharacterSelect3)", "", 130, 300, false, stateEventCharacterSelect, 3);
	local button4 = CreateMenuButton("CharacterSashaButton1",		"LuaStateChange(stateEventCharacterSelect4);", "LuaStateChange(stateEventCharacterSelect4)", "", 470, 340, false, stateEventCharacterSelect, 3);
	local button6 = CreateMenuButton("CharacterLarpersButton1",		"LuaStateChange(stateEventCharacterSelect5);", "LuaStateChange(stateEventCharacterSelect5)", "", 285, 415, false, stateEventCharacterSelect, 3);
	local button38 = CreateMenuButton("CharacterCryGameButton1",	"LuaStateChange(stateEventCharacterSelect6);", "LuaStateChange(stateEventCharacterSelect6)", "", 110, 425, false, stateEventCharacterSelect, 3); 
	local button39 = CreateMenuButton("CharacterStoryTimeButton1",	"LuaStateChange(stateEventCharacterSelect7);", "LuaStateChange(stateEventCharacterSelect7)", "", 470, 430, false, stateEventCharacterSelect, 3); 
	local button7 = CreateMenuButton("CharacterSelectReturnMenuButton1", "LuaStateChange(stateEventMainMenu);", "", "",  150, 625, false, stateEventCharacterSelect, 3);
		
		--					me		  down		up		left	right
	SetNextButtonComponent(button1, button3, button7, button2, button2);
	SetNextButtonComponent(button2, button4, button7, button1, button1);
	SetNextButtonComponent(button3, button38, button1, button4, button4);
	SetNextButtonComponent(button4, button39, button2, button3, button3);
	SetNextButtonComponent(button6, button7, button3, button38, button39);
	SetNextButtonComponent(button38, button7, button3, button39, button6);
	SetNextButtonComponent(button39, button7, button4, button6, button38);
	SetNextButtonComponent(button7, button1, button38, 0, 0);
	
	-- Select Banidtios -- State 9
	local bground1 = CreateMenuButton("CharSelectBackground2",		"", "", "CharacterSelect/T_Character_Select_Banditos_D.png", 0, 0, false, stateEventCharacterSelect2, 2);
	local button8 = CreateMenuButton("CharacterBulldogButton2",		"LuaStateChange(stateEventCharacterSelect);",  "LuaStateChange(stateEventCharacterSelect)",  "", 130, 180, false, stateEventCharacterSelect2, 3);
	local button9 = CreateMenuButton("CharacterBanditosButton2",	"CreateBanditosObj(Player0, 0);", "", "", 475, 170, true,stateEventCharacterSelect2, 3);
	local button10 = CreateMenuButton("CharacterScientistButton2",	"LuaStateChange(stateEventCharacterSelect);",  "LuaStateChange(stateEventCharacterSelect)",  "", 130, 300, false, stateEventCharacterSelect2, 3);
	local button11 = CreateMenuButton("CharacterSashaButton2",		"LuaStateChange(stateEventCharacterSelect4);", "LuaStateChange(stateEventCharacterSelect4)", "", 470, 340, false, stateEventCharacterSelect2, 3);
	local button12 = CreateMenuButton("CharacterLarpersButton2",	"LuaStateChange(stateEventCharacterSelect5);", "LuaStateChange(stateEventCharacterSelect5)", "", 285, 415, false, stateEventCharacterSelect2, 3);
	local button40 = CreateMenuButton("CharacterCryGameButton2",	"LuaStateChange(stateEventCharacterSelect6);", "LuaStateChange(stateEventCharacterSelect6)", "", 110, 425, false, stateEventCharacterSelect2, 3); 
	local button41 = CreateMenuButton("CharacterStoryTimeButton2",	"LuaStateChange(stateEventCharacterSelect7);", "LuaStateChange(stateEventCharacterSelect7)", "", 470, 430, false, stateEventCharacterSelect2, 3); 
	local button13 = CreateMenuButton("CharacterSelectReturnMenuButton2", "LuaStateChange(stateEventMainMenu);", "", "",  150, 625, false, stateEventCharacterSelect2, 3);
		
		--					me		  down		up		left	right
	SetNextButtonComponent(button8, button10, button13, button9, button9);
	SetNextButtonComponent(button9, button11, button13, button8, button8);
	SetNextButtonComponent(button10, button40, button8, button11, button11);
	SetNextButtonComponent(button11, button41, button9, button10, button10);
	SetNextButtonComponent(button12, button13, button10, button40, button41);
	SetNextButtonComponent(button40, button13, button10, button41, button12);
	SetNextButtonComponent(button41, button13, button11, button12, button40);
	SetNextButtonComponent(button13, button8, button40, 0, 0);
 
	 -- Select Scientist -- State 10
	 local bground2 = CreateMenuButton("CharSelectBackground3",		"","", "CharacterSelect/T_Character_Select_Scientist_D.png", 0, 0, false, stateEventCharacterSelect3, 2);
	local button14 = CreateMenuButton("CharacterBulldogButton3",	"LuaStateChange(stateEventCharacterSelect);",  "LuaStateChange(stateEventCharacterSelect)",  "", 130, 180, false, stateEventCharacterSelect3, 3);
	local button15 = CreateMenuButton("CharacterBanditosButton3",	"LuaStateChange(stateEventCharacterSelect2);", "LuaStateChange(stateEventCharacterSelect2)", "", 475, 170, false, stateEventCharacterSelect3, 3);
	local button16 = CreateMenuButton("CharacterScientistButton3",	"CreateScientistsObj(Player0, 0);", "", "", 130, 300, true, stateEventCharacterSelect3, 3);
	local button17 = CreateMenuButton("CharacterSashaButton3",		"LuaStateChange(stateEventCharacterSelect4);", "LuaStateChange(stateEventCharacterSelect4)", "", 470, 340, false, stateEventCharacterSelect3, 3);
	local button18 = CreateMenuButton("CharacterLarpersButton3",	"LuaStateChange(stateEventCharacterSelect5);", "LuaStateChange(stateEventCharacterSelect5)", "", 285, 415, false, stateEventCharacterSelect3, 3);
	local button42 = CreateMenuButton("CharacterCryGameButton3",	"LuaStateChange(stateEventCharacterSelect6);", "LuaStateChange(stateEventCharacterSelect6)", "", 110, 425, false, stateEventCharacterSelect3, 3); 
	local button43 = CreateMenuButton("CharacterStoryTimeButton3",	"LuaStateChange(stateEventCharacterSelect7);", "LuaStateChange(stateEventCharacterSelect7)", "", 470, 430, false, stateEventCharacterSelect3, 3); 
	local button19 = CreateMenuButton("CharacterSelectReturnMenuButton3", "LuaStateChange(stateEventMainMenu);",  "",  "",  150, 625, false, stateEventCharacterSelect3, 3);
		
		--					me		  down		up		left	right
	SetNextButtonComponent(button14, button18, button19, button15, button15); -- 1
	SetNextButtonComponent(button15, button17, button19, button14, button14); -- 2
	SetNextButtonComponent(button16, button42, button14, button17, button17); -- 3
	SetNextButtonComponent(button17, button43, button15, button16, button16); -- 4
	SetNextButtonComponent(button18, button19, button16, button42, button43); -- 5
	SetNextButtonComponent(button42, button19, button16, button43, button18);
	SetNextButtonComponent(button43, button19, button17, button18, button42);
	SetNextButtonComponent(button19, button14, button42, 0, 0); -- 6
	
	-- Select SASHA -- State 11
	local bground3 = CreateMenuButton("CharSelectBackground4",		"", "", "CharacterSelect/T_Character_Select_Sasha_D.png", 0, 0, false, stateEventCharacterSelect4, 2);
	local button21 = CreateMenuButton("CharacterBulldogButton4",	"LuaStateChange(stateEventCharacterSelect));", "LuaStateChange(stateEventCharacterSelect)",  "", 130, 180, false, stateEventCharacterSelect4, 3);
	local button22 = CreateMenuButton("CharacterBanditosButton4",	"LuaStateChange(stateEventCharacterSelect2);", "LuaStateChange(stateEventCharacterSelect2)", "", 475, 170, false, stateEventCharacterSelect4, 3);
	local button23 = CreateMenuButton("CharacterScientistButton4",	"LuaStateChange(stateEventCharacterSelect3);", "LuaStateChange(stateEventCharacterSelect3)", "", 130, 300, false, stateEventCharacterSelect4, 3);
	local button24 = CreateMenuButton("CharacterSashaButton4",		"CreateSashaObj(Player0, 0);", "", "", 470, 340, true, stateEventCharacterSelect4, 3);
	local button26 = CreateMenuButton("CharacterLarpersButton4",	"LuaStateChange(stateEventCharacterSelect5);", "LuaStateChange(stateEventCharacterSelect5)", "", 285, 415, false, stateEventCharacterSelect4, 3);
	local button44 = CreateMenuButton("CharacterCryGameButton4",	"LuaStateChange(stateEventCharacterSelect6);", "LuaStateChange(stateEventCharacterSelect6)", "", 110, 425, false, stateEventCharacterSelect4, 3); 
	local button45 = CreateMenuButton("CharacterStoryTimeButton4",	"LuaStateChange(stateEventCharacterSelect7);", "LuaStateChange(stateEventCharacterSelect7)", "", 470, 430, false, stateEventCharacterSelect4, 3);
	local button27 = CreateMenuButton("CharacterSelectReturnMenuButton4", "LuaStateChange(stateEventMainMenu);", "", "",  150, 625, false, stateEventCharacterSelect4, 3);
		
		--					me		  down		up		left	right
	SetNextButtonComponent(button21, button23, button27, button22, button22);
	SetNextButtonComponent(button22, button24, button27, button21, button21);
	SetNextButtonComponent(button23, button44, button21, button24, button24);
	SetNextButtonComponent(button24, button45, button22, button23, button23);
	SetNextButtonComponent(button26, button27, button23, button44, button45);
	SetNextButtonComponent(button44, button27, button23, button45, button26);
	SetNextButtonComponent(button45, button27, button24, button26, button44);
	SetNextButtonComponent(button27, button21, button44, 0, 0);
	
	-- Select Larpers -- State 12
	local bground4 = CreateMenuButton("CharSelectBackground5",		"", "", "CharacterSelect/T_Character_Select_Larpers_D.png", 0, 0, false, stateEventCharacterSelect5, 2);
	local button31 = CreateMenuButton("CharacterBulldogButton5",	"LuaStateChange(stateEventCharacterSelect);",  "LuaStateChange(stateEventCharacterSelect)",  "", 130, 180, false, stateEventCharacterSelect5, 3);
	local button32 = CreateMenuButton("CharacterBanditosButton5",	"LuaStateChange(stateEventCharacterSelect2);", "LuaStateChange(stateEventCharacterSelect2)", "", 475, 170, false, stateEventCharacterSelect5, 3);
	local button33 = CreateMenuButton("CharacterScientistButton5",	"LuaStateChange(stateEventCharacterSelect3);", "LuaStateChange(stateEventCharacterSelect3)", "", 130, 300, false, stateEventCharacterSelect5, 3);
	local button34 = CreateMenuButton("CharacterSashaButton5",		"LuaStateChange(stateEventCharacterSelect4);", "LuaStateChange(stateEventCharacterSelect4)", "", 470, 340, false, stateEventCharacterSelect5, 3);
	local button36 = CreateMenuButton("CharacterLarpersButton5",	"CreateLarpersObj(Player0, 0);", "", "", 285, 415, true, stateEventCharacterSelect5, 3);
	local button46 = CreateMenuButton("CharacterCryGameButton5",	"LuaStateChange(stateEventCharacterSelect6);", "LuaStateChange(stateEventCharacterSelect6)", "", 110, 425, false, stateEventCharacterSelect5, 3); 
	local button47 = CreateMenuButton("CharacterStoryTimeButton5",	"LuaStateChange(stateEventCharacterSelect7);", "LuaStateChange(stateEventCharacterSelect7)", "", 470, 430, false, stateEventCharacterSelect5, 3);
	local button37 = CreateMenuButton("CharacterSelectReturnMenuButton5", "LuaStateChange(stateEventMainMenu);", "", "",  150, 625, false, stateEventCharacterSelect5, 3);
		
		--					me		  down		up		left	right
	SetNextButtonComponent(button31, button33, button37, button32, button32);
	SetNextButtonComponent(button32, button34, button37, button31, button31);
	SetNextButtonComponent(button33, button46, button31, button34, button34);
	SetNextButtonComponent(button34, button47, button32, button33, button33);
	SetNextButtonComponent(button36, button37, button33, button46, button47);
	SetNextButtonComponent(button46, button37, button33, button47, button36);
	SetNextButtonComponent(button47, button37, button34, button36, button46);
	SetNextButtonComponent(button37, button31, button46, 0, 0);
	
	-- Select CryGame -- State 13
	local bground5 = CreateMenuButton("CharSelectBackground6",		"", "", "CharacterSelect/T_Character_Select_Secret_D.png", 0, 0, false, stateEventCharacterSelect6, 2);
	local button48 = CreateMenuButton("CharacterBulldogButton6",	"LuaStateChange(stateEventCharacterSelect);",  "LuaStateChange(stateEventCharacterSelect)",  "", 130, 180, false, stateEventCharacterSelect6, 3);
	local button49 = CreateMenuButton("CharacterBanditosButton6",	"LuaStateChange(stateEventCharacterSelect2);", "LuaStateChange(stateEventCharacterSelect2)", "", 475, 170, false, stateEventCharacterSelect6, 3);
	local button50 = CreateMenuButton("CharacterScientistButton6",	"LuaStateChange(stateEventCharacterSelect3);", "LuaStateChange(stateEventCharacterSelect3)", "", 130, 300, false, stateEventCharacterSelect6, 3);
	local button51 = CreateMenuButton("CharacterSashaButton6",		"LuaStateChange(stateEventCharacterSelect4);", "LuaStateChange(stateEventCharacterSelect4)", "", 470, 340, false, stateEventCharacterSelect6, 3);
	local button52 = CreateMenuButton("CharacterLarpersButton6",	"LuaStateChange(stateEventCharacterSelect5);", "LuaStateChange(stateEventCharacterSelect5)", "", 285, 415, false, stateEventCharacterSelect6, 3);
	local button53 = CreateMenuButton("CharacterCryGameButton6",	"CreateCryGameObj(Player0, 0);", "", "", 110, 425, true, stateEventCharacterSelect6, 3); 
	local button54 = CreateMenuButton("CharacterStoryTimeButton6",	"LuaStateChange(stateEventCharacterSelect7);", "LuaStateChange(stateEventCharacterSelect7)", "", 470, 430, false, stateEventCharacterSelect6, 3);
	local button55 = CreateMenuButton("CharacterSelectReturnMenuButton6", "LuaStateChange(stateEventMainMenu);", "", "",  150, 625, false, stateEventCharacterSelect6, 3);
		
		--					me		  down		up		left	right
	SetNextButtonComponent(button48, button50, button55, button49, button49);
	SetNextButtonComponent(button49, button51, button55, button48, button48);
	SetNextButtonComponent(button50, button53, button48, button51, button51);
	SetNextButtonComponent(button51, button54, button49, button50, button50);
	SetNextButtonComponent(button52, button55, button50, button53, button54);
	SetNextButtonComponent(button53, button55, button50, button54, button52);
	SetNextButtonComponent(button54, button55, button51, button52, button53);
	SetNextButtonComponent(button55, button48, button53, 0, 0);
	
	-- Select StoryTime -- State 14
	local bground6 = CreateMenuButton("CharSelectBackground7",	"", "", "CharacterSelect/T_Character_Select_Secret_D.png", 0, 0, false, stateEventCharacterSelect7, 2);
	local button56 = CreateMenuButton("CharacterBulldogButton7",	"LuaStateChange(stateEventCharacterSelect);",  "LuaStateChange(stateEventCharacterSelect)",  "", 130, 180, false, stateEventCharacterSelect7, 3);
	local button57 = CreateMenuButton("CharacterBanditosButton7",	"LuaStateChange(stateEventCharacterSelect2);", "LuaStateChange(stateEventCharacterSelect2)", "", 475, 170, false, stateEventCharacterSelect7, 3);
	local button58 = CreateMenuButton("CharacterScientistButton7",	"LuaStateChange(stateEventCharacterSelect3);", "LuaStateChange(stateEventCharacterSelect3)", "", 130, 300, false, stateEventCharacterSelect7, 3);
	local button59 = CreateMenuButton("CharacterSashaButton7",		"LuaStateChange(stateEventCharacterSelect4);", "LuaStateChange(stateEventCharacterSelect4)", "", 470, 340, false, stateEventCharacterSelect7, 3);
	local button60 = CreateMenuButton("CharacterLarpersButton7",	"LuaStateChange(stateEventCharacterSelect5);", "LuaStateChange(stateEventCharacterSelect5)", "", 285, 415, false, stateEventCharacterSelect7, 3);
	local button61 = CreateMenuButton("CharacterCryGameButton7",	"LuaStateChange(stateEventCharacterSelect6);", "LuaStateChange(stateEventCharacterSelect6)", "", 110, 425, false, stateEventCharacterSelect7, 3); 
	local button62 = CreateMenuButton("CharacterStoryTimeButton7",	"CreateStoryTimeObj(Player0, 0);", "", "", 470, 430, true, stateEventCharacterSelect7, 3);
	local button63 = CreateMenuButton("CharacterSelectReturnMenuButton7", "LuaStateChange(stateEventMainMenu);", "", "",  150, 625, false, stateEventCharacterSelect7, 3);
		
		--					me		  down		up		left	right
	SetNextButtonComponent(button56, button58, button63, button57, button57);
	SetNextButtonComponent(button57, button59, button63, button56, button56);
	SetNextButtonComponent(button58, button61, button56, button59, button59);
	SetNextButtonComponent(button59, button62, button57, button58, button58);
	SetNextButtonComponent(button60, button63, button58, button61, button62);
	SetNextButtonComponent(button61, button63, button58, button62, button60);
	SetNextButtonComponent(button62, button63, button59, button60, button61);
	SetNextButtonComponent(button63, button56, button61, 0, 0);
	
	
end

function InitCreditsMenu()
	
	-- 10 is credits state
	--local thisState = stateEventCredits;
	
	local button3 = CreateMenuButton("CreditsBackground", "", "", "T_Credits_Backdrop_D.png", 0, 0, false, stateEventCredits, 2);
--	local background = CreateMenuButton("CreditsScrolling", "", "", "T_CreditsStart_D.png", 250, 768, false, stateEventCredits, 3);
--	local background2 = CreateMenuButton("CreditsScrolling", "","", "T_CreditsEnd_D.png", 250, 1792, false, stateEventCredits, 3);
	local button1 = CreateMenuButton("CreditsReturnMenuButton", "LuaBack();", "", "", 160, 635, true, stateEventCredits, 3);
	
	
--	CreateScrollerComponent(background, 0, -60, 512, 1024)
--	CreateScrollerComponent(background2,0, -60, 512, 1024);
	SetNextButtonComponent(button1, button1, button1, button1, button1);
end

function InitHowToPlayMenu()
	
	-- 12 is how to play state, each screen after will be the next one up in the enums list
	--local thisState = stateEventHowToPlay;
	
		
	prevX = 600
	prevY = 680
	nextX = 900
	nextY = 690
	
	
	-- Screen 0 - State 16
	local background0 = CreateMenuButton("HowToPlayBackground0", "", "", "/HowToPlay/T_How_To_Play1_D.png", 0, 0, false, stateEventHowToPlay, 2);
	local backtomainmenu0 = CreateMenuButton("HowToPlayReturnMenuButton0", "LuaStateChange(stateEventMainMenu);", "", "", 150, 625, false, stateEventHowToPlay, 3);
	local prev0 = CreateMenuButton("HowToPlayNextButton0", "LuaStateChange(stateEventHowToPlay6);", "", "", prevX, prevY, false, stateEventHowToPlay, 3);
	local next0 = CreateMenuButton("HowToPlayNextButton0", "LuaStateChange(stateEventHowToPlay1);",  "","", nextX, nextY, true, stateEventHowToPlay, 3);
	
	SetNextButtonComponent(backtomainmenu0, prev0, next0, next0, prev0);
	SetNextButtonComponent(prev0, next0, backtomainmenu0, backtomainmenu0, next0);
	SetNextButtonComponent(next0, backtomainmenu0, prev0, prev0, backtomainmenu0);
	
	
	-- Screen 1 - State 17
	local background1 = CreateMenuButton("HowToPlayBackground1", "", "", "/HowToPlay/T_How_To_Play2_D.png", 0, 0, false, stateEventHowToPlay1, 2);
	local backtomainmenu1 = CreateMenuButton("HowToPlayReturnMenuButton1", "LuaStateChange(stateEventMainMenu);", "", "", 150, 625, false, stateEventHowToPlay1, 3);
	local prev1 = CreateMenuButton("HowToPlayNextButton1", "LuaStateChange(stateEventHowToPlay);", "", "", prevX, prevY, false, stateEventHowToPlay1, 3);
	local next1 = CreateMenuButton("HowToPlayNextButton1", "LuaStateChange(stateEventHowToPlay2);", "", "",nextX, nextY, true, stateEventHowToPlay1, 3);
	
	SetNextButtonComponent(backtomainmenu1, prev1, next1, next1, prev1);
	SetNextButtonComponent(prev1, next1, backtomainmenu1, backtomainmenu1, next1);
	SetNextButtonComponent(next1, backtomainmenu1, prev1, prev1, backtomainmenu1);
	
	
	-- Screen 2 - State 18
	local background2 = CreateMenuButton("HowToPlayBackground2", "",  "","/HowToPlay/T_How_To_Play3_D.png", 0, 0, false, stateEventHowToPlay2, 2);
	local backtomainmenu2 = CreateMenuButton("HowToPlayReturnMenuButton2", "LuaStateChange(stateEventMainMenu);", "", "", 150, 625, false, stateEventHowToPlay2, 3);
	local prev2 = CreateMenuButton("HowToPlayNextButton2", "LuaStateChange(stateEventHowToPlay1);",  "","", prevX, prevY, false, stateEventHowToPlay2, 3);
	local next2 = CreateMenuButton("HowToPlayNextButton2", "LuaStateChange(stateEventHowToPlay3);", "", "", nextX, nextY, true, stateEventHowToPlay2, 3);
	
	SetNextButtonComponent(backtomainmenu2, prev2, next2, next2, prev2);
	SetNextButtonComponent(prev2, next2, backtomainmenu2, backtomainmenu2, next2);
	SetNextButtonComponent(next2, backtomainmenu2, prev2, prev2, backtomainmenu2);
	
	
	-- Screen 3 - State 19
	local background3 = CreateMenuButton("HowToPlayBackground3", "", "", "/HowToPlay/T_How_To_Play4_D.png", 0, 0, false, stateEventHowToPlay3, 2);
	local backtomainmenu3 = CreateMenuButton("HowToPlayReturnMenuButton3", "LuaStateChange(stateEventMainMenu);", "", "", 150, 625, false, stateEventHowToPlay3, 3);
	local prev3 = CreateMenuButton("HowToPlayNextButton3", "LuaStateChange(stateEventHowToPlay2);", "", "", prevX, prevY, false, stateEventHowToPlay3, 3);
	local next3 = CreateMenuButton("HowToPlayNextButton3", "LuaStateChange(stateEventHowToPlay4);", "", "", nextX, nextY, true, stateEventHowToPlay3, 3);
	
	SetNextButtonComponent(backtomainmenu3, prev3, next3, next3, prev3);
	SetNextButtonComponent(prev3, next3, backtomainmenu3, backtomainmenu3, next3);
	SetNextButtonComponent(next3, backtomainmenu3, prev3, prev3, backtomainmenu3);
	
	-- Screen 4 - State 20
	local background4 = CreateMenuButton("HowToPlayBackground4", "", "", "/HowToPlay/T_How_To_Play5_D.png", 0, 0, false, stateEventHowToPlay4, 2);
	local backtomainmenu4 = CreateMenuButton("HowToPlayReturnMenuButton4", "LuaStateChange(stateEventMainMenu);", "", "", 150, 625, false, stateEventHowToPlay4, 3);
	local prev4 = CreateMenuButton("HowToPlayNextButton4", "LuaStateChange(stateEventHowToPlay3);", "", "", prevX, prevY, false, stateEventHowToPlay4, 3);
	local next4 = CreateMenuButton("HowToPlayNextButton4", "LuaStateChange(stateEventHowToPlay5);", "", "", nextX, nextY, true, stateEventHowToPlay4, 3);
	
	SetNextButtonComponent(backtomainmenu4, prev4, next4, next4, prev4);
	SetNextButtonComponent(prev4, next4, backtomainmenu4, backtomainmenu4, next4);
	SetNextButtonComponent(next4, backtomainmenu4, prev4, prev4, backtomainmenu4);
	
	
	-- Screen 5 - State 21
	local background5 = CreateMenuButton("HowToPlayBackground5", "", "", "/HowToPlay/T_How_To_Play6_D.png", 0, 0, false, stateEventHowToPlay5, 2);
	local backtomainmenu5 = CreateMenuButton("HowToPlayReturnMenuButton5", "LuaStateChange(stateEventMainMenu);", "", "", 150, 625, false, stateEventHowToPlay5, 3);
	local prev5 = CreateMenuButton("HowToPlayNextButton5", "LuaStateChange(stateEventHowToPlay4);", "", "", prevX, prevY, false, stateEventHowToPlay5, 3);
	local next5 = CreateMenuButton("HowToPlayNextButton5", "LuaStateChange(stateEventHowToPlay6);", "", "", nextX, nextY, true, stateEventHowToPlay5, 3);
	
	SetNextButtonComponent(backtomainmenu5, prev5, next5, next5, prev5);
	SetNextButtonComponent(prev5, next5, backtomainmenu5, backtomainmenu5, next5);
	SetNextButtonComponent(next5, backtomainmenu5, prev5, prev5, backtomainmenu5);
	
	
	-- Screen 6 - State 22
	local background6 = CreateMenuButton("HowToPlayBackground6", "", "", "/HowToPlay/T_How_To_Play7_D.png", 0, 0, false, stateEventHowToPlay6, 2);
	local backtomainmenu6 = CreateMenuButton("HowToPlayReturnMenuButton6", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay6, 3);
	local prev6 = CreateMenuButton("HowToPlayNextButton6", "LuaStateChange(stateEventHowToPlay5);", "", "", prevX, prevY, false, stateEventHowToPlay6, 3);
	local next6 = CreateMenuButton("HowToPlayNextButton6", "LuaStateChange(stateEventHowToPlay);", "", "", nextX, nextY, true, stateEventHowToPlay6, 3);
	
	SetNextButtonComponent(backtomainmenu6, prev6, next6, next6, prev6);
	SetNextButtonComponent(prev6, next6, backtomainmenu6, backtomainmenu6, next6);
	SetNextButtonComponent(next6, backtomainmenu6, prev6, prev6, backtomainmenu6);
		
	--[[ -- these aren't  being used anymore
	
	-- Screen 7 - State 23 
	local background7 = CreateMenuButton("HowToPlayBackground7", "", "", "/HowToPlay/T_How_To_Play8_D.png", 0, 0, false, stateEventHowToPlay7, 2);
	local backtomainmenu7 = CreateMenuButton("HowToPlayReturnMenuButton7", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay7, 3);
	local prev7 = CreateMenuButton("HowToPlayNextButton7", "LuaStateChange(stateEventHowToPlay6);", "", "", 500, 690, false, stateEventHowToPlay7, 3);
	local next7 = CreateMenuButton("HowToPlayNextButton7", "LuaStateChange(stateEventHowToPlay8);", "", "", 855, 680, true, stateEventHowToPlay7, 3);
	
	SetNextButtonComponent(backtomainmenu7, prev7, next7, next7, prev7);
	SetNextButtonComponent(prev7, next7, backtomainmenu7, backtomainmenu7, next7);
	SetNextButtonComponent(next7, backtomainmenu7, prev7, prev7, backtomainmenu7);

	-- Screen 8 - State 24
	local background8 = CreateMenuButton("HowToPlayBackground8", "", "", "/HowToPlay/T_How_To_Play9_D.png", 0, 0, false, stateEventHowToPlay8, 2);
	local backtomainmenu8 = CreateMenuButton("HowToPlayReturnMenuButton8", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay8, 3);
	local prev8 = CreateMenuButton("HowToPlayNextButton8", "LuaStateChange(stateEventHowToPlay7);", "", "", 500, 690, false, stateEventHowToPlay8, 3);
	local next8 = CreateMenuButton("HowToPlayNextButton8", "LuaStateChange(stateEventHowToPlay9);", "", "", 855, 680, true, stateEventHowToPlay8, 3);
	
	SetNextButtonComponent(backtomainmenu8, prev8, next8, next8, prev8);
	SetNextButtonComponent(prev8, next8, backtomainmenu8, backtomainmenu8, next8);
	SetNextButtonComponent(next8, backtomainmenu8, prev8, prev8, backtomainmenu8);
	
	-- Screen 9 - State 25 
	local background9 = CreateMenuButton("HowToPlayBackground9", "", "", "/HowToPlay/T_How_To_Play10_D.png", 0, 0, false, stateEventHowToPlay9, 2);
	local backtomainmenu9 = CreateMenuButton("HowToPlayReturnMenuButton9", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay9, 3);
	local prev9 = CreateMenuButton("HowToPlayNextButton9", "LuaStateChange(stateEventHowToPlay8);", "", "", 500, 690,false, stateEventHowToPlay9, 3);
	local next9 = CreateMenuButton("HowToPlayNextButton9", "LuaStateChange(stateEventHowToPlay10);", "", "",855, 680, true, stateEventHowToPlay9, 3);
	
	SetNextButtonComponent(backtomainmenu9, prev9, next9, next9, prev9);
	SetNextButtonComponent(prev9, next9, backtomainmenu9, backtomainmenu9, next9);
	SetNextButtonComponent(next9, backtomainmenu9, prev9, prev9, backtomainmenu9);
	
	-- Screen 10 - State 26 
	local background10 = CreateMenuButton("HowToPlayBackground10", "", "", "/HowToPlay/T_How_To_Play11_D.png", 0, 0, false, stateEventHowToPlay10, 2);
	local backtomainmenu10 = CreateMenuButton("HowToPlayReturnMenuButton10", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay10, 3);
	local prev10 = CreateMenuButton("HowToPlayNextButton10", "LuaStateChange(stateEventHowToPlay9);", "", "", 500, 690,false, stateEventHowToPlay10, 3);
	local next10 = CreateMenuButton("HowToPlayNextButton10", "LuaStateChange(stateEventHowToPlay11);", "", "",855, 680, true, stateEventHowToPlay10, 3);
	
	SetNextButtonComponent(backtomainmenu10, prev10, next10, next10, prev10);
	SetNextButtonComponent(prev10, next10, backtomainmenu10, backtomainmenu10, next10);
	SetNextButtonComponent(next10, backtomainmenu10, prev10, prev10, backtomainmenu10);
	
	-- Screen 11 - State 27 
	local background11 = CreateMenuButton("HowToPlayBackground11", "", "", "/HowToPlay/T_How_To_Play12_D.png", 0, 0, false, stateEventHowToPlay11, 2);
	local backtomainmenu11 = CreateMenuButton("HowToPlayReturnMenuButton11", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay11, 3);
	local prev11 = CreateMenuButton("HowToPlayNextButton11", "LuaStateChange(stateEventHowToPlay10);", "", "", 500, 690, false, stateEventHowToPlay11, 3);
	local next11 = CreateMenuButton("HowToPlayNextButton11", "LuaStateChange(stateEventHowToPlay12);", "", "", 855, 680, true, stateEventHowToPlay11, 3);
	
	SetNextButtonComponent(backtomainmenu11, prev11, next11, next11, prev11);
	SetNextButtonComponent(prev11, next11, backtomainmenu11, backtomainmenu11, next11);
	SetNextButtonComponent(next11, backtomainmenu11, prev11, prev11, backtomainmenu11);
	
	-- Screen 12 - State 28 
	local background12 = CreateMenuButton("HowToPlayBackground12", "", "", "/HowToPlay/T_How_To_Play13_D.png", 0, 0, false, stateEventHowToPlay12, 2);
	local backtomainmenu12 = CreateMenuButton("HowToPlayReturnMenuButton12", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay12, 3);
	local prev12 = CreateMenuButton("HowToPlayNextButton12", "LuaStateChange(stateEventHowToPlay11);", "", "", 500, 690, false, stateEventHowToPlay12, 3);
	local next12 = CreateMenuButton("HowToPlayNextButton12", "LuaStateChange(stateEventHowToPlay13);", "", "", 855, 680, true, stateEventHowToPlay12, 3);
	
	SetNextButtonComponent(backtomainmenu12, prev12, next12, next12, prev12);
	SetNextButtonComponent(prev12, next12, backtomainmenu12, backtomainmenu12, next12);
	SetNextButtonComponent(next12, backtomainmenu12, prev12, prev12, backtomainmenu12);
	
	-- Screen 13 - State 29 
	local background13 = CreateMenuButton("HowToPlayBackground13", "", "", "/HowToPlay/T_How_To_Play14_D.png", 0, 0, false, stateEventHowToPlay13, 2);
	local backtomainmenu13 = CreateMenuButton("HowToPlayReturnMenuButton13", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay13, 3);
	local prev13 = CreateMenuButton("HowToPlayNextButton13", "LuaStateChange(stateEventHowToPlay12);", "", "", 500, 690, false, stateEventHowToPlay13, 3);
	local next13 = CreateMenuButton("HowToPlayNextButton13", "LuaStateChange(stateEventHowToPlay14);", "", "", 855, 680, true, stateEventHowToPlay13, 3);
	
	SetNextButtonComponent(backtomainmenu13, prev13, next13, next13, prev13);
	SetNextButtonComponent(prev13, next13, backtomainmenu13, backtomainmenu13, next13);
	SetNextButtonComponent(next13, backtomainmenu13, prev13, prev13, backtomainmenu13);
	
	-- Screen 14 - State 30 
	local background14 = CreateMenuButton("HowToPlayBackground14", "", "", "/HowToPlay/T_How_To_Play15_D.png", 0, 0, false, stateEventHowToPlay14, 2);
	local backtomainmenu14 = CreateMenuButton("HowToPlayReturnMenuButton14", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay14, 3);
	local prev14 = CreateMenuButton("HowToPlayNextButton14", "LuaStateChange(stateEventHowToPlay13);", "", "", 500, 690, false, stateEventHowToPlay14, 3);
	local next14 = CreateMenuButton("HowToPlayNextButton14", "LuaStateChange(stateEventHowToPlay15);", "", "", 855, 680, true, stateEventHowToPlay14, 3);
	
	SetNextButtonComponent(backtomainmenu14, prev14, next14, next14, prev14);
	SetNextButtonComponent(prev14, next14, backtomainmenu14, backtomainmenu14, next14);
	SetNextButtonComponent(next14, backtomainmenu14, prev14, prev14, backtomainmenu14);
	
	-- Screen 15 - State 31 
	local background15 = CreateMenuButton("HowToPlayBackground15", "", "", "/HowToPlay/T_How_To_Play16_D.png", 0, 0, false, stateEventHowToPlay15, 2);
	local backtomainmenu15 = CreateMenuButton("HowToPlayReturnMenuButton15", "LuaStateChange(stateEventMainMenu);",  "","", 150, 625, false, stateEventHowToPlay15, 3);
	local prev15 = CreateMenuButton("HowToPlayNextButton15", "LuaStateChange(stateEventHowToPlay14);", "", "", 500, 690, false, stateEventHowToPlay15, 3);
	local next15 = CreateMenuButton("HowToPlayNextButton15", "LuaStateChange(stateEventHowToPlay);", "", "",  855, 680, true, stateEventHowToPlay15, 3);
	
	SetNextButtonComponent(backtomainmenu15, prev15, next15, next15, prev15);
	SetNextButtonComponent(prev15, next15, backtomainmenu15, backtomainmenu15, next15);
	SetNextButtonComponent(next15, backtomainmenu15, prev15, prev15, backtomainmenu15);
	
	]]--
	
	
	-- Make the f1 screen in game
	local inGameHowToPlayBackground = CreateMenuButton("inGameHowToPlayBackground", "", "", "T_F1_How_To_Play_D.png", 0, 0, false, stateInGameHowToPlay, 2);
	
end



function CreateMenuButton(name, selectionEventName, scrolloverLuaFunc, buttonTextureName, nPositionX, nPositionY, bStartSelected, thisState, nTexDepth)
	newButton = CreateObj(name, 0, 0, 0, 0);
	print(name .. " Object Created")
	buttonComponent = CreateButtonComponent(newButton, selectionEventName, scrolloverLuaFunc, buttonTextureName, nPositionX, nPositionY, bStartSelected, thisState, nTexDepth);
	print("Button Component added to " .. name)
	return buttonComponent
	
end


--// Lua Prototype: CreateSliderComponent(Object, "EventName", "ButtonTexture.png", nBarStartX, nBarStartY, nBarEndX, nBarEndY, 
--//										nTextureDepth, bStartSelected, nActiveState)
function CreateMenuSlider(name, selectionEventName, buttonTextureName, nStartX, nStartY, nEndX, nEndY, bStartSelected, thisState, nSliderFor, nTexDepth)
	newSlider = CreateObj(name, 0, 0, 0, 0);
	print(name .. " Object Created")
	sliderComponent = CreateSliderComponent(newButton, selectionEventName, buttonTextureName, nStartX, nStartY, nEndX, nEndY,
						nTexDepth, bStartSelected, nSliderFor, thisState);
	print("Slider Component added to " .. name)
	return sliderComponent
	
end

function CreatePlayerObj(name, xPos, yPos, zPos, rot, isHuman)

	-- NOTICE: CreateObj takes an additional 3 parameters for the position
	-- E.G. CreateObj("TestObj", x, y, z)
	-- But if you do it in code, the function has default (0,0,0) for the position
	--			- Agent Smith
	local NewObj = CreateObj(name, xPos, yPos, zPos, rot)
	print(name .. " Object Created")
	-- Then add all the components 
	-- with the obj as the param
	
	-- Player Object is an AI
	if(isHuman == false) then
		CreateAIComponent(NewObj, false);
		print("AI Component Added to " .. name)
	end
	
	CreateMovementComponent(NewObj);
	print("Movement Component added to " .. name)
	--CreateRenderComp(NewObj, 0, 1, 2);
	--print("Render Component added to" .. name)
--	CreateCollideableComponent(NewObj, false, true, 1, xPos, yPos, zPos, radius, 1);--player is sphere
	CreateCollideableComponent(NewObj, false, true, 1, xPos, yPos, zPos,
		 1.0, 0.0, 0.0,
		 0.0, 1.0, 0.0,
		 0.0, 0.0, 1.0,
		 0.8, 1.0, 1.0, 3);
--	CreateCollideableComponent(NewObj, false, true, 1, -- (1 == PLAYER)
--		0.0, 0.0, 1.0,  -- forward sphere offset
--		0.0, 0.0, -1.3,    -- rear sphere offset
--		0.5,      -- radius of spheres
--		7);       -- BCAPSULE (type)
	print("Collide Component Added to " .. name);--player is box
	
	
	CreateInventoryComp(NewObj)
	print("Inventory Component Added to " .. name)

	return NewObj;
end

function CreateBikersObj(PlayerObj, PlayerNumber)
		
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim0";
	local AnimObj = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr = CreateAnimComp(AnimObj, "FFP_3D_Bulldog1_FINShape",
		"Default",				"Bulldog_Idle.anim",			"",								-- 1
		"ShoveLeftAnimation",	"Bulldog_Shove_Left.anim",		"",								-- 2
		"ShoveRightAnimation",	"Bulldog_Shove_Right.anim",		"",								-- 3
		"SteerLeftAnim",		"Bulldog_Turn_Left_GOTO.anim",	"Bulldog_Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		"Bulldog_Turn_Right_GOTO.anim", "Bulldog_Turn_Right_LOOP.anim", -- 5
		"Reverse",				"Bulldog_Reverse.anim",			"",								-- 6
		"BoostAnimation",		"Bulldog_Boost.anim",			"",								-- 7
		"SlipEffect",			"Bulldog_Spin.anim",			"",								-- 8
		"CartCollision",		"Bulldog_Collision.anim",		"",								-- 9
		"SlowAnimation",		"Bulldog_Trudge_Goto.anim",		"Bulldog_Trudge_Loop.anim",		-- 10
		"TigerBlood",			"Bulldog_Victory.anim",			"Bulldog_Victory_Loop.anim",	-- 11
		"GoodDaySir",			"Bulldog_Lose.anim",			"Bulldog_Lose.anim",			   12);
	-- 47 is for RC_ANIM_BULLDOG and 4 is for RF_ANIM
	CreateRenderComp(AnimObj, MeshPtr, 79, 4);
	CreateInvulnerableVFXComponent(AnimObj, MeshPtr);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim1";
	local AnimObj1 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr1 = CreateAnimComp(AnimObj1, "FFP_3D_Ma1_FINShape",
		"Default",				"Ma_Idle.anim",				"",							-- 1
		"ShoveLeftAnimation",	"Ma_Shove_Left.anim",		"",							-- 2
		"ShoveRightAnimation",	"Ma_Shove_Right.anim",		"",							-- 3
		"SteerLeftAnim",		"Ma_Turn_Left_GOTO.anim",	"Ma_Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		"Ma_Turn_Right_GOTO.anim",	"Ma_Turn_Right_LOOP.anim",	-- 5
		"Reverse",				"Ma_Reverse.anim",			"",							-- 6
		"BoostAnimation",		"Ma_Boost.anim",			"",							-- 7
		"CartCollision",		"Ma_Collision.anim",		"",							-- 8
		"SlipEffect",			"Ma_Spin.anim",				"",							-- 9
		"GoalItemCollected",	"Ma_Collection.anim",		"",							-- 10
		"SlowAnimation",		"Ma_Trudge_Goto.anim",		"Ma_Trudge_Loop.anim",		-- 11
		"TigerBlood",			"Ma_Victory.anim",			"Ma_Victory_Loop.anim",		-- 12
		"GoodDaySir",			"Ma_Lose.anim",				"Ma_Lose.anim",				   13);
	-- 48 is for RC_ANIM_MA and 4 is for RF_ANIM
	CreateRenderComp(AnimObj1, MeshPtr1, 80, 4);
	CreateInvulnerableVFXComponent(AnimObj1, MeshPtr1);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj1);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim2";
	local AnimObj2 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Biker_Cart_"
	local MeshPtr2 = CreateAnimComp(AnimObj2, "FFP_3D_BikerCart1_FINShape",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse.anim",			"",									-- 6
		"BoostAnimation",		header .. "Boosting.anim",			"",									-- 7
		"SlipEffect",			header .. "Spin.anim",				"",									-- 8
		"CartCollision",		header .. "Collision.anim",			"",									-- 9
		"SlowAnimation",		header .. "Trudge_Goto.anim",		header .. "Trudge_Loop.anim",		--10
		"TigerBlood",			header .. "Victory.anim",			header .. "Victory_Loop.anim",		--11
		"GoodDaySir",			header .. "Lose.anim",				header .. "Lose.anim",				  12);
	-- 49 is for RC_ANIM_BIKCART and 4 is for RF_ANIM
	CreateRenderComp(AnimObj2, MeshPtr2, 81, 4);
	CreateInvulnerableVFXComponent(AnimObj2, MeshPtr2);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj2);
	
	-- Donut
	CreateDonutObj(PlayerObj, PlayerNumber);
	
	-- Skid Marks
	CreateSkidMarksComponent(AnimObj2, 0);
	CreatePBVFXComp(AnimObj2, 0);
	
	-- tell movement what weight it is (L=1, M=2, H=3)
	SetCartWeight(PlayerNumber, 3)
	-- tell HUD which player you are (number relating to index of picture in portraits image)
	SetCharacterPicked(PlayerNumber, 0)
	
	-- Shadow Time!
	CreateShadowComponent(PlayerObj, 0);
	
	print("Bikers Created");
	
	if PlayerNumber == 0 and Player1 ~= nil then
		-- Create the AI
		CreateLarpersObj(Player1, 1);
	elseif PlayerNumber == 1 and Player2 ~= nil then
		-- Create Another AI
		CreateLarpersObj(Player2, 2);
	elseif PlayerNumber == 2 and Player3 ~= nil then
		-- Create Another AI
		CreateLarpersObj(Player3, 3);
	else
		-- Start the Game
		print("Starting the Game!");
		LuaStateChange(stateEventGameplay);
	end
	
end

function CreateLarpersObj(PlayerObj, PlayerNumber)

	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim0";
	print(AnimName);
	local AnimObj = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "BanLarp_Driver_";
	local MeshPtr = CreateAnimComp(AnimObj, "FFP_3D_Semour_FIN",
		"Default",				header .. "Idle.anim",						"",										-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",					"",										-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",				"",										-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",			header .. "Turn_Left_LOOP.anim",		-- 4
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",			header .. "Turn_Right_LOOP.anim",		-- 5
		"Reverse",				header .. "Reverse.anim",					"",										-- 6
		"BoostAnimation",		header .. "Boost.anim",						"",										-- 7
		"SlipEffect",			header .. "Spin.anim",						"",										-- 8
		"CartCollision",		header .. "Collision.anim",					"",										-- 9   
		"SlowAnimation",		header .. "Trudge_Goto.anim",				header .. "Trudge_Loop.anim",			-- 10
		"TigerBlood",			"BanLarp_Larp_Driver_Victory_Goto.anim",	"BanLarp_Larp_Driver_Victory_Loop.anim", --11
		"GoodDaySir",			"BanLarp_Larp_Driver_Lose.anim",			"BanLarp_Larp_Driver_Lose.anim",		   12);
	-- 50 is for RC_ANIM_SEMOUR and 4 is for RF_ANIM
	CreateRenderComp(AnimObj, MeshPtr, 82, 4);
	CreateInvulnerableVFXComponent(AnimObj, MeshPtr);
	
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim1";
	print(AnimName);
	local AnimObj1 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr1 = CreateAnimComp(AnimObj1, "FFP_3D_Dave_FIN",
		"Default",				"BanLarp_Gunner_Idle.anim",					"",										-- 1
		"ShoveLeftAnimation",	"BanLarp_Gunner_ShoveLeft.anim",			"",										-- 2
		"ShoveRightAnimation",	"BanLarp_Gunner_ShoveRight.anim",			"",										-- 3
		"SteerLeftAnim",		"BanLarp_Gunner_Turn_Left_GOTO.anim",		"BanLarp_Gunner_Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		"BanLarp_Gunner_Turn_Right_GOTO.anim",		"BanLarp_Gunner_Turn_Right_LOOP.anim",	-- 5
		"Reverse",				"BanLarp_Gunner_Reverse.anim",				"",										-- 6
		"BoostAnimation",		"BanLarp_Gunner_Boost.anim",				"",										-- 7
		"CartCollision",		"BanLarp_Gunner_Collision.anim",			"",										-- 8
		"SlipEffect",			"BanLarp_Gunner_Spin.anim",					"",										-- 9
		"GoalItemCollected",	"BanLarp_Gunner_Collection.anim",			"",										-- 10
		"SlowAnimation",		"BanLarp_Gunner_Trudge_Goto.anim",			"BanLarp_Gunner_Trudge_Loop.anim",		-- 11
		"TigerBlood",			"BanLarp_Larp_Gunner_Victory_Goto.anim",	"BanLarp_Larp_Gunner_Victory_Loop.anim", --12
		"GoodDaySir",			"BanLarp_Larp_Gunner_Lose.anim",			"BanLarp_Larp_Gunner_Lose.anim",		   13);
	-- 51 is for RC_ANIM_DAVE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj1, MeshPtr1, 83, 4);
	CreateInvulnerableVFXComponent(AnimObj1, MeshPtr1);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj1);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim2";
	print(AnimName);
	local AnimObj2 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr2 = CreateAnimComp(AnimObj2, "FFP_3D_LARPer_Cart_FINShape",
		"Default",			"BanLarp_Idle.anim",					"",										-- 1
		"SteerLeftAnim",	"BanLarp_Cart_Turn_Left_GOTO.anim",		"BanLarp_Cart_Turn_Left_LOOP.anim",		-- 2
		"SteerRightAnim",	"BanLarp_Cart_Turn_Right_GOTO.anim",	"BanLarp_Cart_Turn_Right_LOOP.anim",	-- 3
		"Reverse",			"BanLarp_Reverse.anim",					"",										-- 4
		"BoostAnimation",	"BanLarp_Boost.anim",					"",										-- 5
		"SlipEffect",		"BanLarp_Spin.anim",					"",										-- 6
		"CartCollision",	"BanLarp_Collision.anim",				"",										-- 7
		"SlowAnimation",	"BanLarp_Cart_Trudge_Goto.anim",		"BanLarp_Cart_Trudge_Loop.anim",		-- 8
		"TigerBlood",		"BanLarp_Larp_Cart_Victory_Goto.anim",	"BanLarp_Larp_Cart_Victory_Loop.anim",	-- 9
		"GoodDaySir",		"BanLarp_Larp_Cart_Lose.anim",			"BanLarp_Larp_Cart_Lose.anim",			   10);
	-- 52 is for RC_ANIM_BANDCART and 4 is for RF_ANIM
	CreateRenderComp(AnimObj2, MeshPtr2, 84, 4);
	CreateInvulnerableVFXComponent(AnimObj2, MeshPtr2);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj2);
	
	-- Donut
	CreateDonutObj(PlayerObj, PlayerNumber);
	
	-- Skid Marks
	CreateSkidMarksComponent(AnimObj2, 2);
	CreatePBVFXComp(AnimObj2, 2);
	
	
	-- tell movement what weight it is (L=1, M=2, H=3)
	SetCartWeight(PlayerNumber, 1)
	-- tell HUD which player you are (number relating to index of picture in portraits image)
	SetCharacterPicked(PlayerNumber, 2)
	
	-- Shadow Time!
	CreateShadowComponent(PlayerObj, 2);
	
	-- HACK for bad cart animations
	--CreateRenderComp(PlayerObj, 0, 1, 2);
	print("Larpers Created");
	
	if PlayerNumber == 0 and Player1 ~= nil then
		-- Create the AI
		CreateScientistsObj(Player1, 1);
	elseif PlayerNumber == 1 and Player2 ~= nil then
		-- Create Another AI
		CreateScientistsObj(Player2, 2);
	elseif PlayerNumber == 2 and Player3 ~= nil then
		-- Create Another AI
		CreateScientistsObj(Player3, 3);
	else
		-- Start the Game
		LuaStateChange(stateEventGameplay);
	end

end

function CreateScientistsObj(PlayerObj, PlayerNumber)
		
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim0";
	print(AnimName);
	local AnimObj = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Driver_"
	local MeshPtr = CreateAnimComp(AnimObj, "FFP_3D_Steve1_FINShape",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4 
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"SlipEffect",			header .. "Spin.anim",				"",									-- 8
		"CartCollision",		header .. "Collision.anim",			"",									-- 9
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 10
		"TigerBlood",			header .. "Win_GOTO.anim",			header .. "Victory_Loop.anim",		-- 11
		"GoodDaySir",			header .. "Clap.anim",			header .. "Clap.anim",			   12);
	-- 44 is for RC_ANIM_STEVE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj, MeshPtr, 76, 4);
	CreateInvulnerableVFXComponent(AnimObj, MeshPtr);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim1";
	print(AnimName);
	local AnimObj1 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Gunner_"
	local MeshPtr1 = CreateAnimComp(AnimObj1, "FFP_3D_Eugine1_FINShape",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"CartCollision",		header .. "Collision.anim",			"",									-- 8
		"SlipEffect",			header .. "Spin.anim",				"",									-- 9
		"GoalItemCollected",	header .. "Collection.anim",		"",									-- 10
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 11
		"TigerBlood",			header .. "Victory_Goto.anim",		header .. "Win_LOOP.anim",			-- 12
		"GoodDaySir",			header .. "Clap.anim",			header .. "Clap.anim",			   13);
	-- 45 is for RC_ANIM_EUGENE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj1, MeshPtr1, 77, 4);
	CreateInvulnerableVFXComponent(AnimObj1, MeshPtr1);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj1);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim2";
	print(AnimName);
	local AnimObj2 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Cart_"
	local MeshPtr2 = CreateAnimComp(AnimObj2, "FFP_3D_ScientistCart1_FINShape",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"SlipEffect",			header .. "Spin.anim",				"",									-- 8
		"CartCollision",		header .. "Collision.anim",			"",									-- 9
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 10
		"TigerBlood",			header .. "Win_GOTO.anim",			header .. "Victory_Loop.anim",		-- 11
		"GoodDaySir",			header .. "Clap.anim",			header .. "Clap.anim",			   12);
	-- 45 is for RC_ANIM_EUGENE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj2, MeshPtr2, 78, 4);
	CreateInvulnerableVFXComponent(AnimObj2, MeshPtr2);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj2);
	
	-- Donut
	CreateDonutObj(PlayerObj, PlayerNumber);
	
	-- Skid Marks
	CreateSkidMarksComponent(AnimObj2, 3);
	CreatePBVFXComp(AnimObj2, 3);
	
	-- Scrolling UVs
	CreateScrollingUVComponent(AnimObj2, MeshPtr2, 3); 
	
	-- tell movement what weight it is (L=1, M=2, H=3)
	SetCartWeight(PlayerNumber, 2)
	-- tell HUD which player you are (number relating to index of picture in portraits image)
	SetCharacterPicked(PlayerNumber, 3)
	
	-- Shadow Time!
	CreateShadowComponent(PlayerObj, 3);
	
	if PlayerNumber == 0 and Player1 ~= nil then
		-- Create the AI
		CreateBanditosObj(Player1, 1);
	elseif PlayerNumber == 1 and Player2 ~= nil then
		-- Create Another AI
		CreateBanditosObj(Player2, 2);
	elseif PlayerNumber == 2 and Player3 ~= nil then
		-- Create Another AI
		CreateBanditosObj(Player3, 3);
	else
		-- Start the Game
		LuaStateChange(stateEventGameplay);
	end

end

function CreateBanditosObj(PlayerObj, PlayerNumber)
		
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim0";
	print(AnimName);
	local AnimObj = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr = CreateAnimComp(AnimObj, "FFP_3D_Germinal_FIN",
		"Default",				"BanLarp_Driver_Idle.anim",				"",										-- 1
		"ShoveLeftAnimation",	"BanLarp_Driver_ShoveLeft.anim",		"",										-- 2
		"ShoveRightAnimation",	"BanLarp_Driver_ShoveRight.anim",		"",										-- 3
		"SteerLeftAnim",		"BanLarp_Driver_Turn_Left_GOTO.anim",	"BanLarp_Driver_Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		"BanLarp_Driver_Turn_Right_GOTO.anim",	"BanLarp_Driver_Turn_Right_LOOP.anim",	-- 5
		"Reverse",				"BanLarp_Driver_Reverse.anim",			"",										-- 6
		"BoostAnimation",		"BanLarp_Driver_Boost.anim",			"",										-- 7
		"SlipEffect",			"BanLarp_Driver_Spin.anim",				"",										-- 8
		"CartCollision",		"BanLarp_Driver_Collision.anim",		"",										-- 9
		"SlowAnimation",		"BanLarp_Driver_Trudge_Goto.anim",		"BanLarp_Driver_Trudge_Loop.anim",		-- 10
		"TigerBlood",			"BanLarp_Ban_Driver_Victory_Goto.anim", "BanLarp_Ban_Driver_Victory_Loop.anim", -- 11
		"GoodDaySir",			"BanLarp_Ban_Driver_Lose.anim",		"BanLarp_Ban_Driver_Lose.anim",		   12);
		
	-- 53 is for RC_ANIM_GERMINAL and 4 is for RF_ANIM
	CreateRenderComp(AnimObj, MeshPtr, 85, 4);
	CreateInvulnerableVFXComponent(AnimObj, MeshPtr);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim1";
	print(AnimName);
	local AnimObj1 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr1 = CreateAnimComp(AnimObj1, "FFP_3D_Rico_FINShape",
		"Default",				"BanLarp_Gunner_Idle.anim",				"",										-- 1
		"ShoveLeftAnimation",	"BanLarp_Gunner_ShoveLeft.anim",		"",										-- 2
		"ShoveRightAnimation",	"BanLarp_Gunner_ShoveRight.anim",		"",										-- 3
		"SteerLeftAnim",		"BanLarp_Gunner_Turn_Left_GOTO.anim",	"BanLarp_Gunner_Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		"BanLarp_Gunner_Turn_Right_GOTO.anim",	"BanLarp_Gunner_Turn_Right_LOOP.anim",	-- 5
		"Reverse",				"BanLarp_Gunner_Reverse.anim",			"",										-- 6
		"BoostAnimation",		"BanLarp_Gunner_Boost.anim",			"",										-- 7
		"CartCollision",		"BanLarp_Gunner_Collision.anim",		"",										-- 8
		"SlipEffect",			"BanLarp_Gunner_Spin.anim",				"",										-- 9
		"GoalItemCollected",	"BanLarp_Gunner_Collection.anim",		"",										-- 10
		"SlowAnimation",		"BanLarp_Gunner_Trudge_Goto.anim",		"BanLarp_Gunner_Trudge_Loop.anim",		-- 11
		"TigerBlood",			"BanLarp_Ban_Gunner_Victory_Goto.anim", "BanLarp_Ban_Gunner_Victory_Loop.anim", -- 12
		"GoodDaySir",			"BanLarp_Ban_Gunner_Lose.anim",		"BanLarp_Ban_Gunner_Lose.anim",		   13);
	-- 54 is for RC_ANIM_RICO and 4 is for RF_ANIM
	CreateRenderComp(AnimObj1, MeshPtr1, 86, 4);
	CreateInvulnerableVFXComponent(AnimObj1, MeshPtr1);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj1);
			
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim2";
	print(AnimName);
	local AnimObj2 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr2 = CreateAnimComp(AnimObj2, "FFP_3D_BanditoCart_FIN8",
		"Default",			"BanLarp_Idle.anim",					"",										-- 1
		"SteerLeftAnim",	"BanLarp_Cart_Turn_Left_GOTO.anim",		"BanLarp_Cart_Turn_Left_LOOP.anim",		-- 2
		"SteerRightAnim",	"BanLarp_Cart_Turn_Right_GOTO.anim",	"BanLarp_Cart_Turn_Right_LOOP.anim",	-- 3
		"Reverse",			"BanLarp_Reverse.anim",					"",										-- 4
		"BoostAnimation",	"BanLarp_Boost.anim",					"",										-- 5
		"SlipEffect",		"BanLarp_Spin.anim",					"",										-- 6
		"CartCollision",	"BanLarp_Collision.anim",				"",										-- 7	  
		"SlowAnimation",	"BanLarp_Cart_Trudge_Goto.anim",		"BanLarp_Cart_Trudge_Loop.anim",		-- 8
		"TigerBlood",		"BanLarp_Ban_Cart_Victory_Goto.anim",	"BanLarp_Ban_Cart_Victory_Loop.anim",	-- 9
		"GoodDaySir",		"BanLarp_Ban_Cart_Lose.anim",			"BanLarp_Ban_Cart_Lose.anim",			   10);
	-- 55 is for RC_ANIM_BANDCART and 4 is for RF_ANIM
	CreateRenderComp(AnimObj2, MeshPtr2, 87, 4);
	CreateInvulnerableVFXComponent(AnimObj2, MeshPtr2);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj2);
	
	-- Donut
	CreateDonutObj(PlayerObj, PlayerNumber);
	
	-- Skid Marks
	CreateSkidMarksComponent(AnimObj2, 1);
	CreatePBVFXComp(AnimObj2, 1);
	
	-- tell movement what weight it is (L=1, M=2, H=3)
	SetCartWeight(PlayerNumber, 1)
	-- tell HUD which player you are (number relating to index of picture in portraits image)
	SetCharacterPicked(PlayerNumber, 1)
	
	-- Shadow Time!
	CreateShadowComponent(PlayerObj, 1);
	
	print("Banditos Created");
	
	if PlayerNumber == 0 and Player1 ~= nil then
		-- Create the AI
		CreateBikersObj(Player1, 1);
	elseif PlayerNumber == 1 and Player2 ~= nil then
		-- Create Another AI
		CreateBikersObj(Player2, 2);
	elseif PlayerNumber == 2 and Player3 ~= nil then
		-- Create Another AI
		CreateBikersObj(Player3, 3);
	else
		-- Start the Game
		LuaStateChange(stateEventGameplay);
	end

end

function CreateSashaObj(PlayerObj, PlayerNumber)
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim0";
	print(AnimName);
	local AnimObj = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr = CreateAnimComp(AnimObj, "FFP_3D_SASHA_FIN",
		"Default",				"SASHA_Idle.anim",				"",								-- 1
		"ShoveLeftAnimation",	"SASHA_ShoveLeft.anim",			"",								-- 2
		"ShoveRightAnimation",	"SASHA_ShoveRight.anim",		"",								-- 3
		"SteerLeftAnim",		"Sasha_Turn_Left_GOTO.anim",	"Sasha_Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		"Sasha_Turn_Right_GOTO.anim",	"Sasha_Turn_Right_LOOP.anim",	-- 5
		"Reverse",				"SASHA_Reverse.anim",			"",								-- 6
		"BoostAnimation",		"SASHA_Boost.anim",				"",								-- 7
		"CartCollision",		"SASHA_Collision.anim",			"",								-- 8
		"SlipEffect",			"SASHA_Spin.anim",				"",								-- 9
		"GoalItemCollected",	"SASHA_Collection.anim",		"",								-- 10
		"SlowAnimation",		"SASHA_Trudge_Goto.anim",		"SASHA_Trudge_Loop.anim",		-- 11  
		"TigerBlood",			"SASHA_Victory.anim",			"SASHA_Victory_Loop.anim",		-- 12
		"GoodDaySir",			"SASHA_Lose.anim",				"SASHA_Lose_Loop.anim",			   13);
	-- 56 is for RC_ANIM_SASHA and 4 is for RF_ANIM
	CreateRenderComp(AnimObj, MeshPtr, 88, 4);
	CreateInvulnerableVFXComponent(AnimObj, MeshPtr);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj);
	
	-- Donut
	CreateDonutObj(PlayerObj, PlayerNumber);
	
	-- Skid Marks
	CreateSkidMarksComponent(AnimObj, 4);
	CreatePBVFXComp(AnimObj, 4);
	
	-- tell movement what weight it is (L=1, M=2, H=3)
	SetCartWeight(PlayerNumber, 3)
	-- tell HUD which player you are (number relating to index of picture in portraits image)
	SetCharacterPicked(PlayerNumber, 4)
	
	-- Shadow Time!
	CreateShadowComponent(PlayerObj, 4);
	
	print("SASHA Created");
	
	if PlayerNumber == 0 and Player1 ~= nil then
		-- Create the AI
		CreateBikersObj(Player1, 1);
	elseif PlayerNumber == 1 and Player2 ~= nil then
		-- Create Another AI
		CreateBikersObj(Player2, 2);
	elseif PlayerNumber == 2 and Player3 ~= nil then
		-- Create Another AI
		CreateBikersObj(Player3, 3);
	else
		-- Start the Game
		LuaStateChange(stateEventGameplay);
	end

end

function CreateCryGameObj(PlayerObj, PlayerNumber)

	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim0";
	print(AnimName);
	local AnimObj = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Driver_"
	local MeshPtr = CreateAnimComp(AnimObj, "FFP_3D_Pharoh_FINShape",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4 
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"SlipEffect",			header .. "Spin.anim",				"",									-- 8
		"CartCollision",		header .. "Collision.anim",			"",									-- 9
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 10
		"TigerBlood",			"EndGame_Driver_Win_GOTO.anim",		"EndGame_Driver_Win_Loop.anim",		-- 11
		"GoodDaySir",			header .. "Clap.anim",			header .. "Clap.anim",			   12);
	-- 44 is for RC_ANIM_STEVE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj, MeshPtr, 90, 4);
	CreateInvulnerableVFXComponent(AnimObj, MeshPtr);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim1";
	print(AnimName);
	local AnimObj1 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Gunner_"
	local MeshPtr1 = CreateAnimComp(AnimObj1, "FFP_3D_Fitz_FINShape",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"CartCollision",		header .. "Collision.anim",			"",									-- 8
		"SlipEffect",			header .. "Spin.anim",				"",									-- 9
		"GoalItemCollected",	header .. "Collection.anim",		"",									-- 10
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 11
		"TigerBlood",			"EndGame_Gunner_Win_Goto.anim",	"EndGame_Gunner_Win_LOOP.anim",			-- 12
		"GoodDaySir",			header .. "Clap.anim",			header .. "Clap.anim",			   13);
	-- 45 is for RC_ANIM_EUGENE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj1, MeshPtr1, 89, 4);
	CreateInvulnerableVFXComponent(AnimObj1, MeshPtr1);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj1);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim2";
	print(AnimName);
	local AnimObj2 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Cart_"
	local MeshPtr2 = CreateAnimComp(AnimObj2, "FFP_3D_AlienCart_FIN",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"SlipEffect",			header .. "Spin.anim",				"",									-- 8
		"CartCollision",		header .. "Collision.anim",			"",									-- 9
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 10
		"TigerBlood",			"EndGame_Cart_Win_GOTO.anim",		"EndGame__Cart_Win_Loop.anim",		-- 11
		"GoodDaySir",			header .. "Clap.anim",			header .. "Clap.anim",			   12);
	-- 45 is for RC_ANIM_EUGENE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj2, MeshPtr2, 91, 4);
	CreateInvulnerableVFXComponent(AnimObj2, MeshPtr2);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj2);
	
	-- Donut
	CreateDonutObj(PlayerObj, PlayerNumber);
	
	-- Skid Marks
	CreateSkidMarksComponent(AnimObj2, 3);
	CreatePBVFXComp(AnimObj2, 3);
	
	-- Scrolling UVs
	CreateScrollingUVComponent(AnimObj2, MeshPtr2, 6); 
	
	-- tell movement what weight it is (L=1, M=2, H=3)
	SetCartWeight(PlayerNumber, 2)
	-- tell HUD which player you are (number relating to index of picture in portraits image)
	SetCharacterPicked(PlayerNumber, 5)
	
	-- Shadow Time!
	CreateShadowComponent(PlayerObj, 3);
	
	if PlayerNumber == 0 and Player1 ~= nil then
		-- Create the AI
		CreateBikersObj(Player1, 1);
	elseif PlayerNumber == 1 and Player2 ~= nil then
		-- Create Another AI
		CreateBikersObj(Player2, 2);
	elseif PlayerNumber == 2 and Player3 ~= nil then
		-- Create Another AI
		CreateBikersObj(Player3, 3);
	else
		-- Start the Game
		LuaStateChange(stateEventGameplay);
	end

end

function CreateStoryTimeObj(PlayerObj, PlayerNumber)
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim0";
	print(AnimName);
	local AnimObj = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Driver_"
	local MeshPtr = CreateAnimComp(AnimObj, "FFP_3D_Shade_FINShape",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4 
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"SlipEffect",			header .. "Spin.anim",				"",									-- 8
		"CartCollision",		header .. "Collision.anim",			"",									-- 9
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 10
		"TigerBlood",			"StoryTime_Driver_Win_Goto.anim",	"StoryTime_Driver_Win_Loop.anim",		-- 11
		"GoodDaySir",			header ..  "Clap.anim",				header .. "Clap.anim",			   12);
	-- 44 is for RC_ANIM_STEVE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj, MeshPtr, 93, 4);
	CreateInvulnerableVFXComponent(AnimObj, MeshPtr);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim1";
	print(AnimName);
	local AnimObj1 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Gunner_"
	local MeshPtr1 = CreateAnimComp(AnimObj1, "FFP_3D_Evelyn_FINShape",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"CartCollision",		header .. "Collision.anim",			"",									-- 8
		"SlipEffect",			header .. "Spin.anim",				"",									-- 9
		"GoalItemCollected",	header .. "Collection.anim",		"",									-- 10
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 11
		"TigerBlood",			"StoryTime_Gunner_Win_Goto.anim",	"StoryTime_Gunner_Win_Loop.anim",		-- 12
		"GoodDaySir",			header ..  "Clap.anim",				header .. "Clap.anim",			   13);
	-- 45 is for RC_ANIM_EUGENE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj1, MeshPtr1, 92, 4);
	CreateInvulnerableVFXComponent(AnimObj1, MeshPtr1);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj1);
	
	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	AnimName = "Player".. PlayerNumber .. "Anim2";
	print(AnimName);
	local AnimObj2 = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local header = "Scientist_Cart_"
	local MeshPtr2 = CreateAnimComp(AnimObj2, "FFP_3D_CombinedNightlightCart_FIN",
		"Default",				header .. "Idle.anim",				"",									-- 1
		"ShoveLeftAnimation",	header .. "ShoveLeft.anim",			"",									-- 2
		"ShoveRightAnimation",	header .. "ShoveRight.anim",		"",									-- 3
		"SteerLeftAnim",		header .. "Turn_Left_GOTO.anim",	header .. "Turn_Left_LOOP.anim",	-- 4
		"SteerRightAnim",		header .. "Turn_Right_GOTO.anim",	header .. "Turn_Right_LOOP.anim",	-- 5
		"Reverse",				header .. "Reverse_GOTO.anim",		header .. "Reverse_LOOP.anim",		-- 6
		"BoostAnimation",		header .. "Boost.anim",				"",									-- 7
		"SlipEffect",			header .. "Spin.anim",				"",									-- 8
		"CartCollision",		header .. "Collision.anim",			"",									-- 9
		"SlowAnimation",		header .. "Trudge_GOTO.anim",		header .. "Trudge_LOOP.anim",		-- 10
		"TigerBlood",			"StoryTime_Cart_Win_Goto.anim",	"StoryTime_Cart_Win_Loop.anim",		-- 11
		"GoodDaySir",			header .. "Clap.anim",				header .. "Clap.anim",			   12);
	-- 45 is for RC_ANIM_EUGENE and 4 is for RF_ANIM
	CreateRenderComp(AnimObj2, MeshPtr2, 94, 4);
	CreateInvulnerableVFXComponent(AnimObj2, MeshPtr2);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj2);
	
	-- Donut
	CreateDonutObj(PlayerObj, PlayerNumber);
	
	-- Skid Marks
	CreateSkidMarksComponent(AnimObj2, 3);
	CreatePBVFXComp(AnimObj2, 3);
	
	-- tell movement what weight it is (L=1, M=2, H=3)
	SetCartWeight(PlayerNumber, 1)
	-- tell HUD which player you are (number relating to index of picture in portraits image)
	SetCharacterPicked(PlayerNumber, 6)
	
	-- Shadow Time!
	--CreateShadowComponent(PlayerObj, 3);
	
	-- Scrolling UVs
	CreateScrollingUVComponent(AnimObj2, MeshPtr2, 5); 
	
	if PlayerNumber == 0 and Player1 ~= nil then
		-- Create the AI
		CreateBikersObj(Player1, 1);
	elseif PlayerNumber == 1 and Player2 ~= nil then
		-- Create Another AI
		CreateBikersObj(Player2, 2);
	elseif PlayerNumber == 2 and Player3 ~= nil then
		-- Create Another AI
		CreateBikersObj(Player3, 3);
	else
		-- Start the Game
		LuaStateChange(stateEventGameplay);
	end

end

function CreateDonutObj(PlayerObj, PlayerNumber)

	-- Create a separate object for the animated model and attach
	-- it to the player's cart
	local AnimName = "Player".. PlayerNumber .. "DonutAnim";
	print(AnimName);
	local AnimObj = CreateObj(AnimName, 0, 0, 0, 0);
	-- ... is any number of {EventName, AnimName} combos
	-- CreateAnimComp(ObjectPtr, MeshName, EventName, AnimName, ..., NumAnims);
	local MeshPtr = CreateAnimComp(AnimObj, "FFP_3D_DONUTS_FINShape",
			"Default",  "Donut_LOOP.anim", "Donut_LOOP.anim",
			"UseDonut", "Donut_GOTO.anim", "Donut_LOOP.anim", 2);
			
	-- 81 is for RC_ANIM_DONUT and 4 is for RF_ANIM
	CreateRenderComp(AnimObj, MeshPtr, 95, 4);
	CreateAnimatedDonutComp(AnimObj);
	
	-- Bind the Animated object to the player cart
	BindObjects(PlayerObj, AnimObj);
end


-- Button Functions
function LuaPushState(gameState)
	
	PushState(gameState);
	
end

function LuaStateChange(gameState)

	if(gameState == stateEventCharacterSelect6 or gameState == stateEventCharacterSelect7) then
		if(unlocked) then
			print("Characters unlocked" .. gameState)			
			StateChange(gameState);
		end
	else
		print(gameState);
		StateChange(gameState);
	end

end

function LuaBack()

	Back();
	
end


function LuaCancelBackPause()

	LuaSendEvent("CancelOptionsPause");
	Back();
	
end

function LuaCancelBackMain()

	LuaSendEvent("CancelOptionsMain");
	Back();
	
end


function LuaSendEvent(szEvent)
	
	SendLuaEvent(szEvent);
	
end	

function ShutdownGameFromPause()

	-- Shutdown the objects created from Gameplay State first before shutting down the game
	-- 2 is supposed to be for GAMEPLAY_STATE
	LuaSendEvent("ShutdownObjects2");
	LuaSendEvent("ShutdownGame");

end

function ResetDefaultOptions(slider1, slider2, slider3)
 
	-- Matches up with the default starting values of volue and music
	SetSliderValue(slider1, 25)
	SetSliderValue(slider2, 50)
	SetSliderValue(slider3, 50)

end

function TCol()

	Collisions();
	
end

function TGeo()

	Geometry();
	
end

function ToggleTris()

	SendLuaEvent("ToggleTris");

end

function ToggleAI()

	SendLuaEvent("ToggleAI");

end

function ToggleInfo()

	SendLuaEvent("ToggleInfo");
	
end

function AISetGoalTri(PlayerNumber, TriNumber)

	SetGoalTri("Player"..PlayerNumber, TriNumber);

end


function FromLevelSelectToCharSelect()

	-- tell stuff to unload so we can repick them
	LuaSendEvent("ShutdownObjects2");
	StateChange(stateEventCharacterSelect)

end

function EndGameFromPause()

	-- tell stuff to unload so we can repick them
	LuaSendEvent("ShutdownObjects4"); -- 4 is the Win_State
	LuaStateChange(stateEventMainMenu);

end

function SetUnlocked()
	unlocked = true
end