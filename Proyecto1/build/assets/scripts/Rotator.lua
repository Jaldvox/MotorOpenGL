-- assets/scripts/rotator.lua
local Rotator = {}
Rotator.__index = Rotator

function Rotator:Start()
    self.speed = 45.0  -- grados por segundo
end

function Rotator:Update()
    local rot = self.transform.rotation
    self.transform.rotation = Vec3.new(rot.x, rot.y + (self.speed * Time.deltaTime()), rot.z)
end

return Rotator