function LuaToC()
	vegeta = TestLuaFunc();
	return vegeta;
end

function CToLua()
	return 9001
end

function InitValues()
	--numObjs = 1
	--obj = {	name = "TestObj",
	--		numComps = 1,
	--		CreateMovementComponent}
	--AllObjs.insert(obj);
	
	-- To create an object,
	-- name it in CreateObj	
	
	radius = 1.0
	
	
	CreatePlayerObj("Player", -17, 0, -28)
--	CreateCheckout(-17, 0, -28)--enable this and you shall ignite the white hot fiery wrath of Ray!
	
	-- Shopping List
	NewObj = CreateObj("ShoppingListObj", 0,0,0)
	print("'ShoppingListObj' Object Created")
	CreateSpriteComp(NewObj, "Resource\\HUD\\T_Biker_Shopping_List_D.png",
	 0, 500, 0,				  -- X, Y
	 1.0, 1.0,				  -- Scale
	 0.0, 0.0, 0.0,		      -- RotCenter, RotVal
	 256.0, 256.0, 256.0, 1.0,-- Color (RGBA)
	 0, 0, 256, 512,		  -- left, top, right, bottom
	 true);					  -- Active
	print("Sprite Component added to 'ShoppingListObj'")
	
	CreateAIObj("Agent1", -10, 0, -28)
	CreateAIObj("Agent2", -4, 0, -28)
	CreateAIObj("Agent3", 1, 0, -28)
	
	-- options
	stateEventOptions = CreateStateEvent(9)
	-- gameplay
	stateEventGameplay = CreateStateEvent(2)
	-- keybinds
	stateEventKeybinds = CreateStateEvent(11)
	-- game mode select
	stateEventGameModeSelect = CreateStateEvent(7)
	-- character select
	stateEventCharacterSelect = CreateStateEvent(8)
	-- credits
	stateEventCredits = CreateStateEvent(10)
	-- main menu
	stateEventMainMenu = CreateStateEvent(1)
	-- how to play
	stateEventHowToPlay = CreateStateEvent(12)
	
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
	
	
end

function InitMainMenu()
	
	-- 1 is main menu state
	local thisState = 1;
	
	-- From main to Options
	
	local background = CreateMenuButton("MainMenuBackground", "", "MainMenuBackground.png", 0, 0, false, thisState, 0, 2);
	local playGameButton = CreateMenuButton("PlayGameButton", "PushState", "MainMenuPlay.png", 400, 400, true, thisState, stateEventGameModeSelect, 3);
	local optionsGameButton = CreateMenuButton("BumButton", "PushState", "MainMenuOptions.png", 400, 475, false, thisState, stateEventOptions, 3);
	local creditsButton = CreateMenuButton("CreditsButton", "PushState", "MainMenuCredits.png", 400, 550, false, thisState, stateEventCredits, 3);
	local howToPlayButton = CreateMenuButton("HowToPlayButton", "PushState", "MainMenuHowToPlay.png", 400, 625, false, thisState, stateEventHowToPlay, 3);
	local exitGameButton = CreateMenuButton("ExitGameButton", "ShutdownGame", "MainMenuQuit.png", 400, 700, false, thisState, 0, 3);
	
	SetNextButtonComponent(playGameButton, optionsGameButton);
	SetNextButtonComponent(optionsGameButton, creditsButton);
	SetNextButtonComponent(creditsButton, howToPlayButton);
	SetNextButtonComponent(howToPlayButton, exitGameButton);
	SetNextButtonComponent(exitGameButton, playGameButton);
end

function InitWinGameMenu()
	
	-- 4 is win state
	local thisState = 4;
	
	local background = CreateMenuButton("WinBackground", "", "WinScreen.png", 200, 200, false, thisState, 0, 2);
	local playAgainButton = CreateMenuButton("WinPlayAgainButton", "StateChange", "MainMenuPlay.png", 700, 300, true, thisState, stateEventMainMenu, 3);
	local winExitGameButton = CreateMenuButton("WinExitButton", "ShutdownGame", "MainMenuQuit.png", 700, 375, false, thisState,  0, 3);
	
	SetNextButtonComponent(playAgainButton, winExitGameButton);
	SetNextButtonComponent(winExitGameButton, playAgainButton);
end

function InitLoseGameMenu()
	
	-- 5 is lose state
	local thisState = 5;
	
	local background = CreateMenuButton("LoseBackground", "", "LostScreen.png", 200, 200, false, thisState, 0, 2);
	local playAgainButton = CreateMenuButton("WinPlayAgainButton", "StateChange", "MainMenuPlay.png", 300, 300, true, thisState, stateEventMainMenu, 3);
	local loseExitGameButton = CreateMenuButton("WinExitButton", "ShutdownGame", "MainMenuQuit.png", 300, 375, false, thisState,  0, 3);
	
	SetNextButtonComponent(playAgainButton, loseExitGameButton);
	SetNextButtonComponent(loseExitGameButton, playAgainButton);
end

function InitPauseMenu()
	
	-- 3 is pause state
	local thisState = 3;
	
	local background = CreateMenuButton("PauseBackground", "", "T_Pause_Page_1_D.png", 0, 0, false, thisState, 0, 2);
	local resumeButton = CreateMenuButton("PauseResumeAgainButton", "Back", "", 385, 320, true, thisState, 0, 3);
	local optionsButton = CreateMenuButton("PauseOptionsButton", "PushState", "", 395, 385, false, thisState, stateEventOptions, 3);
	local mainMenuButton = CreateMenuButton("PauseMainMenuButton", "StateChange", "", 365, 450, false, thisState,  stateEventMainMenu, 3);
	local quitMenuButton = CreateMenuButton("PauseQuitButton", "ShutdownGame", "", 360, 515, false, thisState,  0, 3);
	
	SetNextButtonComponent(resumeButton, optionsButton);
	SetNextButtonComponent(optionsButton, mainMenuButton);
	SetNextButtonComponent(mainMenuButton, quitMenuButton);
	SetNextButtonComponent(quitMenuButton, resumeButton);
end

function InitOptionsMenu()
	
	-- 9 is options state
	local thisState = 9;
	
	-- CreateMenuSlider(name, selectionEventName, buttonTextureName, nStartX, nStartY, nEndX, nEndY, bStartSelected, thisState, nTexDepth)
	
	
	local background = CreateMenuButton("OptionsBackground", "", "T_Pause_Page_2_D.png", 0, 0, false, thisState, 0, 2);
	--local button1 = CreateMenuButton("OptionsMusicVolume", "", "MainMenuOptions.png", 350, 300, true, thisState, 0, 3);
	--local button2 = CreateMenuButton("OptionsSFXVolume", "", "MainMenuOptions.png", 350, 375, false, thisState, 0, 3);
	local button1 = CreateMenuSlider("OptionsMusicVolume", "SetMusicVolume", "T_Pause_Icon_D.png", 280, 235, 720, 235, true, thisState, 3);
	local button2 = CreateMenuSlider("OptionsSFXVolume", "SetSoundVolume", "T_Pause_Icon_D.png", 285, 315, 745, 315, false, thisState, 3);
	local button3 = CreateMenuButton("OptionsKeybindButton", "PushState", "", 205, 520, false, thisState,  stateEventKeybinds, 3);
	local button4 = CreateMenuButton("OptionsDefaultButton", "", "", 520, 520, false, thisState,  0, 3);
	local button5 = CreateMenuButton("OptionsReturnMenuButton", "Back", "", 570, 620, false, thisState,  0, 3);
	
	SetSliderValue(button2, 50) -- Matches up with the default starting values of volue and music
	SetSliderValue(button1, 50)
	
	SetNextButtonComponent(button1, button2);
	SetNextButtonComponent(button2, button3);
	SetNextButtonComponent(button3, button4);
	SetNextButtonComponent(button4, button5);
	SetNextButtonComponent(button5, button1);
end


function InitKeybindsMenu()
	
	-- 11 is keybind state
	local thisState = 11;
	
	local background = CreateMenuButton("KeybindsBackground", "", "T_Keybinds_D.png", 0, 0, false, thisState, 0, 2);
	local button1 = CreateMenuButton("KeybindButton", "", "MainMenuOptions.png", 350, 450, true, thisState,  0, 3);
	local button2 = CreateMenuButton("KeybindsReturnMenuButton", "Back", "MainMenuQuit.png", 350, 525, false, thisState,  0, 3);
	
	SetNextButtonComponent(button1, button2);
	SetNextButtonComponent(button2, button1);
end

function InitGameModeSelectMenu()
	
	-- 7 is game mode select state
	local thisState = 7;
	
	local background = CreateMenuButton("GameModeBackground", "", "T_Game_Mode_D1.PNG", 0, 0, false, thisState, 0, 2);
	
	-- HACK: StateChange should be push state but it hides behind the other one.
	local button1 = CreateMenuButton("GameModeButton", "PushState", "MainMenuPlay.png", 350, 450, true, thisState,  stateEventCharacterSelect, 3);
	local button2 = CreateMenuButton("GameModeReturnMenuButton", "Back", "MainMenuQuit.png", 350, 525, false, thisState,  stateEventMainMenu, 3);
	
	SetNextButtonComponent(button1, button2);
	SetNextButtonComponent(button2, button1);
end


function InitCharacterSelectMenu()
	
	-- 8 is character select state
	local thisState = 8;
	
	local background = CreateMenuButton("CharSelectBackground", "", "Character select_Screen_New.png", 0, 0, false, thisState, 0, 2);
	local button1 = CreateMenuButton("CharacterButton", "StateChange", "MainMenuPlay.png", 350, 375, true, thisState,  stateEventGameplay, 3);
	-- HACK: Does a state change back. Proper way (though it doesn't show properly) is to have pushes
	local button2 = CreateMenuButton("CharacterSelectReturnMenuButton", "Back", "MainMenuQuit.png", 350, 450, false, thisState,  stateEventGameModeSelect, 3);
	
	SetNextButtonComponent(button1, button2);
	SetNextButtonComponent(button2, button1);
end

function InitCreditsMenu()
	
	-- 10 is credits state
	local thisState = 10;
	
	local background = CreateMenuButton("CreditsBackground", "", "T_Credits_D.png", 0, 0, false, thisState, 0, 2);
	local button1 = CreateMenuButton("CreditsReturnMenuButton", "Back", "MainMenuQuit.png", 350, 450, true, thisState,  0, 3);
	
	SetNextButtonComponent(button1, button1);
end

function InitHowToPlayMenu()
	
	-- 12 is how to play state
	local thisState = 12;
	
	local background = CreateMenuButton("HowToPlayBackground", "", "Loading trial1.jpg", 0, 0, false, thisState, 0, 2);
	local button1 = CreateMenuButton("HowToPlayReturnMenuButton", "Back", "MainMenuQuit.png", 350, 450, true, thisState,  0, 3);
	
	SetNextButtonComponent(button1, button1);
end



function CreateMenuButton(name, selectionEventName, buttonTextureName, nPositionX, nPositionY, bStartSelected, thisState, pEventData, nTexDepth)
	newButton = CreateObj(name, 0, 0, 0);
	print(name, "Object Created")
	buttonComponent = CreateButtonComponent(newButton, selectionEventName, buttonTextureName, nPositionX, nPositionY, bStartSelected, thisState, pEventData, nTexDepth);
	print("Button Component added to", name)
	return buttonComponent
	
end

--// Lua Prototype: CreateSliderComponent(Object, "EventName", "ButtonTexture.png", nBarStartX, nBarStartY, nBarEndX, nBarEndY, 
--//										nTextureDepth, bStartSelected, nActiveState)
function CreateMenuSlider(name, selectionEventName, buttonTextureName, nStartX, nStartY, nEndX, nEndY, bStartSelected, thisState, nTexDepth)
	newSlider = CreateObj(name, 0, 0, 0);
	print(name, "Object Created")
	sliderComponent = CreateSliderComponent(newButton, selectionEventName, buttonTextureName, nStartX, nStartY, nEndX, nEndY,
						nTexDepth, bStartSelected, thisState);
	print("Slider Component added to", name)
	return sliderComponent
	
end

function CreateStateObjs()
	for obj=1, numObjs, 1
	do
		NewObj = CreateObj(AllObjs[obj].name)
		for comp=1, AllObjs[obj].numComps, 1
		do
			AllObjs[obj][comp]()
		end
	end
end

function CreatePlayerObj(name, xPos, yPos, zPos)

	-- NOTICE: CreateObj takes an additional 3 parameters for the position
	-- E.G. CreateObj("TestObj", x, y, z)
	-- But if you do it in code, the function has default (0,0,0) for the position
	--			- Agent Smith
	local NewObj = CreateObj(name, xPos, yPos, zPos)
	print(name, "Object Created")
	-- Then add all the components 
	-- with the obj as the param
	CreateMovementComponent(NewObj);
	print("Movement Component added to", name)
	CreateRenderComp(NewObj, 0, 1, 2);
	print("Render Component added to",name)
	CreateCollideableComponent(NewObj, false, true, 1, xPos, yPos, zPos, radius, 1);
	print("Collide Component Added to",name)
	
	CreateInventoryComp(NewObj)
	print("Inventory Component Added to",name)
end

function CreateAIObj(name, xPos, yPos, zPos)

	local NewObj = CreateObj(name, xPos, yPos, zPos)
	print(name, "Object Created")
	-- Then add all the components 
	-- with the obj as the param
	CreateMovementComponent(NewObj);
	print("Movement Component added to", name)
	CreateRenderComp(NewObj, 0, 1, 2);
	print("Render Component added to", name)
	CreateCollideableComponent(NewObj, false, true, 1, xPos, yPos, zPos, radius, 1);
	print("Collide Component Added to", name)
	CreateAIComponent(NewObj);
	print("AI Component Added to", name)
	CreateInventoryComp(NewObj)
	print("Inventory Component Added to",name)
end

function CreateCheckout(xPos, yPos, zPos)
	local NewObj = CreateObj("Checkout", xPos, yPos, zPos);
	print("Checkout Object Created")
	CreateCollideableComponent(NewObj, true, false, 7, xPos, yPos, zPos+1.5, 5.0, 1);
	print("Collide Component added to Checkout")
end