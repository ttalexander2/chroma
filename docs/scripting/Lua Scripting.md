# Lua Scripting

The chroma engine contains bindings to support behavior in lua. A set of custom functions and environments are provided to control and interact with the Entity Component System provided by the engine.

Each script can be attached to an [entity](md_scripting_API_Entity.html), and is provided with that entities' context. The script will be executed in the context of that entity, and can be attached to multiple entities.

In order to communicate with the engine, in addition to exposing the entity, a number of functions may be defined in order to execute code during the game loop.

#### The following functions can be defined:

These functions will be called in order when the Scene is initialized.

- EarlyInit()
- Init()
- LateInit()

These functions will be called once per frame in order. Use `time.Delta` if using a variable framerate.

- EarlyUpdate()
- Update()
- LateUpdate()

These functions are defined in an opt-in fashion. The [entity](md_scripting_API_Entity.html) table is only defined in the context of these functions.
**DO NOT** use the [entity](md_scripting_API_Entity.html) variable outside the context of these functions, as it is *not* guaranteed to be defined.

### Example Script
```lua
--Functions from any script can be referenced from this script by using the include() function.
--This is used instead of require() in order to support the engine's hot reloading of scripts.
local shared_module = include("assets/scripts/shared_test.lua")

--Global values will be exposed to the editor
Speed = 150.0
Point = Vec2(1,2)

--Local values will not be exposed to the editor
local sprite
local transform


--The engine will automatically call this function during the Init step.
function Init()
	transform = entity:GetTransform()
	sprite = entity:GetSpriteRenderer()
	sprite:SetAnimation("idle front")
	start_coroutine("co_test")
end

--Coroutines also work by yielding time values (in seconds).
--These coroutines can be started and ended at any point in time.
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

--This function is called by the engine every frame.
function Update()
    --function to log a value to the console (used instead of print, to also print to the editor console)
	log("Some debug value")

    --calling a shared module function
	shared_module.some_function()

    --gets the connected gamepad
	local gamepad = Input:GetFirstConnectedGamepad()
    
    --Checks which buttons are being pressed to move the player around.
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


--This function is used to indicate to the engine any variables to be serialized
--The user should provide the name of the values as strings. The editor will automatically
--retrieve the value at serialization time.
function Serialize()
	return {"to_serialize"}
end

```