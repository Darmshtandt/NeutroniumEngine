local keyboard = Keyboard()
local speed = 0.05;

function Start()
end

function Update(time)
	if keyboard:IsKeyPressed(Key._W, false) then
		g_ThisObject:Translate(Float3D(0, 0, -speed))
	elseif keyboard:IsKeyPressed(Key._S, false) then
		g_ThisObject:Translate(Float3D(0, 0, speed))
	end

	if keyboard:IsKeyPressed(Key._A, false) then
		g_ThisObject:Translate(Float3D(-speed, 0, 0))
	elseif keyboard:IsKeyPressed(Key._D, false) then
		g_ThisObject:Translate(Float3D(speed, 0, 0))
	end

	if keyboard:IsKeyPressed(Key._SPACE, false) then
		g_ThisObject:Translate(Float3D(0, speed, 0))
	elseif keyboard:IsKeyPressed(Key._SHIFT, false) then
		g_ThisObject:Translate(Float3D(0, -speed, 0))
	end
end
