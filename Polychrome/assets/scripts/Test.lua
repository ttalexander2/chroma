--local shared_module = include("assets/scripts/shared_test.lua")
--Global values will be exposed to the editor
--Any primitives here will be serialized
Speed = 150.0

Point = Vec2(1,2)

--Local values will not be exposed to the editor
local sprite
local transform

local to_serialize = 2


--This function executes once after the Scene is loaded
function EarlyInit()

end

--This function executes once after the Scene is loaded, after EarlyInit()
function Init()
	transform = entity:GetTransform()
	sprite = entity:GetSpriteRenderer()
	sprite:SetAnimation("idle front")
	start_coroutine("co_test")
end

function co_test()
	while true do
		coroutine.yield(1)
		transform.Position.X = transform.Position.X + 6
		coroutine.yield(1)
		transform.Position.Y = transform.Position.Y + 6
		coroutine.yield(1)
		transform.Position.X = transform.Position.X - 6
		coroutine.yield(1)
		transform.Position.Y = transform.Position.Y - 6
	end
end

--This function executes once after the Scene is loaded, after Init()
function LateInit()
	
end

--This function executes once every frame, at the begining of the frame
function EarlyUpdate()

end

--This function executes once every frame, after EarlyUpdate()
function Update()
	--log("yeet")
	--shared_module.aaaah()
	local gamepad = Input:GetFirstConnectedGamepad()
	if Input:GetGamepadButtonState(GamepadButton.DPAD_RIGHT, gamepad) == ButtonState.PRESSED then
		transform.Position.X = transform.Position.X + (Speed*time.Delta)
		transform.Scale.X = -1 * math.abs(transform.Scale.X)
		sprite:SetAnimation("Left run")
	elseif Input:GetGamepadButtonState(GamepadButton.DPAD_LEFT, gamepad) == ButtonState.PRESSED then
		transform.Position.X = transform.Position.X - (Speed*time.Delta)
		sprite:SetAnimation("Left run")
		transform.Scale.X = 1 * math.abs(transform.Scale.X)
	elseif Input:GetGamepadButtonState(GamepadButton.DPAD_DOWN, gamepad) == ButtonState.PRESSED then
		transform.Position.Y = transform.Position.Y - (Speed*time.Delta)
		sprite:SetAnimation("Front run")
		transform.Scale.X = 1 * math.abs(transform.Scale.X)
	elseif Input:GetGamepadButtonState(GamepadButton.DPAD_UP, gamepad) == ButtonState.PRESSED then
		transform.Position.Y = transform.Position.Y + (Speed*time.Delta)
		sprite:SetAnimation("Back run")
		transform.Scale.X = 1 * math.abs(transform.Scale.X)
	else
		if sprite:GetAnimationName(sprite:GetAnimation()) == "Left run" then
			sprite:SetAnimation("idle left")
		elseif sprite:GetAnimationName(sprite:GetAnimation()) == "Back run" then
			sprite:SetAnimation("idle back")
		else
			sprite:SetAnimation("idle front")
		end
	end

end

--This function executes once every frame, after Update()
function LateUpdate()
	
end


--This function is used to indicate to the engine any local variables to be serialized
--The user should provide the name of the values as strings
function Serialize()
	return {"to_serialize"}
end
















