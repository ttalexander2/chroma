--Global values will be exposed to the editor
--Any primitives here will be serialized
Speed = 5.0

Point = Vec2(1,2)

--Local values will not be exposed to the editor
local transform
local some_val = 43


--This function executes once after the Scene is loaded
function EarlyInit()
	
end

--This function executes once after the Scene is loaded, after EarlyInit()
function Init()
	transform = entity:GetTransform()
end

--This function executes once after the Scene is loaded, after Init()
function LateInit()
	
end

--This function executes once every frame, at the begining of the frame
function EarlyUpdate()

end

--This function executes once every frame, after EarlyUpdate()
function Update()
	transform.Position.X = transform.Position.X + math.random(-25, 25)*time.Delta
	transform.Position.Y = transform.Position.Y + math.random(-25, 25)*time.Delta
end

--This function executes once every frame, after Update()
function LateUpdate()
	
end


--This function is used to indicate to the engine any local variables to be serialized
--The user should provide the name of the values as strings
function Serialize()
	return {"some_val"}
end