local FPSController = {}
FPSController.__index = FPSController

function FPSController:Start()
    self.speed = 5.0
    self.sensivity = 0.08
    self.moveCam = true;
    -- Bloqueamos el cursor en el centro para el FPS
    Input.lockCursor(true)
end

function FPSController:Update()
    local cam = Scene.camera()

    -- ROTACIÓN DE LA CÁMARA
    if self.moveCam then
        cam:lookAt(Input.mouseX(), Input.mouseY(), self.sensivity)
    end

    if Input.isMousePressed(Input.MOUSE_RIGHT) then
        Input.lockCursor(true)
        self.moveCam = true
    end

    -- MOVIMIENTO DEL JUGADOR (WASD)
    local velocity = self.speed * Time.deltaTime()
    local moveDir = Vec3.new(0, 0, 0)
    
    if Input.isKeyPressed(Key.W) then
        moveDir = moveDir + cam.direction
    end
    if Input.isKeyPressed(Key.S) then
        moveDir = moveDir - cam.direction
    end
    if Input.isKeyPressed(Key.A) then
        moveDir = moveDir - cam.right
    end
    if Input.isKeyPressed(Key.D) then
        moveDir = moveDir + cam.right
    end

    if Input.isKeyPressed(Key.Space) then
        moveDir = moveDir + cam.up
    end
     if Input.isKeyPressed(Key.LeftShift) then
        moveDir = moveDir - cam.up
    end

    -- Actualizamos la posición
    cam.position = cam.position + (moveDir * velocity)
    
    -- Liberar ratón
    if Input.isKeyPressed(Key.Escape) then
        Input.lockCursor(false)
        self.moveCam = false
    end
end

return FPSController