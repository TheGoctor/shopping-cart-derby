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
	
	-- Shopping List
	NewObj = CreateObj("ShoppingListObj", 0,0,0)
	print("'ShoppingListObj' Object Created")
	CreateSpriteComp(NewObj, "Resource\\HUD\\FFP_2D_BullMaList_PH.png",
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
	
	InitMainMenu()
	
	
end

function InitMainMenu()
	
	-- 2nd to last param is bum. fill out with whatever you want
	playGameButton = CreateMenuButton("PlayGameButton", "StateChangeGameplay", "MainMenuPlay.png", 400, 400, 0, true);
	exitGameButton = CreateMenuButton("ExitGameButton", "ShutdownGame", "MainMenuQuit.png", 400, 475, 0, false);
	
	SetNextButtonComponent(playGameButton, exitGameButton);
	SetNextButtonComponent(exitGameButton, playGameButton);
end

function CreateMenuButton(name, selectionEventName, buttonTextureName, nPositionX, nPositionY, nPositionY2, bStartSelected)
	newButton = CreateObj(name);
	print(name, "Object Created")
	buttonComponent = CreateButtonComponent(newButton, selectionEventName, buttonTextureName, nPositionX, nPositionY, bStartSelected);
	print("Button Component added to", name)
	return buttonComponent
	
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
	CreateRenderComp(NewObj, 0, 0, 1);
	print("Render Component added to",name)
	CreateCollideableComponent(NewObj, false, 1, xPos, yPos, zPos, radius);
	print("Collide Component Added to",name)
	CreateStartOfRaceComponent(NewObj)
	print("StartOfRace Component Added to",name)
	CreateHasWonComponent(NewObj)
	print("Has Won Component Added to",name)
end

function CreateAIObj(name, xPos, yPos, zPos)

	local NewObj = CreateObj(name, xPos, yPos, zPos)
	print(name, "Object Created")
	-- Then add all the components 
	-- with the obj as the param
	CreateMovementComponent(NewObj);
	print("Movement Component added to", name)
	CreateRenderComp(NewObj, 0, 0, 1);
	print("Render Component added to", name)
	CreateCollideableComponent(NewObj, false, 1, xPos, yPos, zPos, radius);
	print("Collide Component Added to", name)
	CreateAIComponent(NewObj);
	print("AI Component Added to", name)
end