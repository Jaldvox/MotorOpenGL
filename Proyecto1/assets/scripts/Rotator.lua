-- assets/scripts/rotator.lua
local Rotator = {}
Rotator.__index = Rotator

function Rotator:Start()
    self.speed = 45.0  -- grados por segundo
    print("Rotator iniciado")
end

function Rotator:Update()
    -- self.transform vendría de haber expuesto Transform a Lua
    -- self.transform:rotate(Vec3(0, self.speed * deltaTime, 0))
    print("Rotando a velocidad: 1" .. self.speed)
end

return Rotator