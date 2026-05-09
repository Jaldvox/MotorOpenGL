local Player = {}
Player.__index = Player

function Player:Start()
    self.speed = 5.0
end

function Player:Update()
    if Input.isKeyPressed(Key.I) then
        print("Se ha pulsado la tecla W")
        local pos = self.transform.position
        self.transform.position = Vec3.new(pos.x, pos.y + self.speed * Time.deltaTime(), pos.z)
    end

    if Input.isKeyPressed(Key.K) then
        print("Se ha pulsado la tecla W")
        local pos = self.transform.position
        self.transform.position = Vec3.new(pos.x, pos.y - self.speed * Time.deltaTime(), pos.z)
    end
end

return Player