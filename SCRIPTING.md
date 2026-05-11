# Scripting en Capi Engine

Los scripts de Capi Engine se escriben en **Lua** y permiten controlar el comportamiento de las entidades en la escena: movimiento, iluminación, input del jugador, cámara, etc.

---

## Creando un script

Un script es un archivo `.lua` que puede exponer dos funciones opcionales que el motor llama automáticamente:

```lua
-- Se llama una vez al inicio
function onStart()
    -- inicialización
end

-- Se llama cada frame
function onUpdate()
    -- lógica por frame
end
```

Asigna el script a una entidad desde el editor para que el motor lo ejecute.

---

## API de referencia

### Vec3

Vector 3D utilizado para posiciones, rotaciones y escalas.

```lua
local v = Vec3.new(1.0, 0.0, 0.0)
print(v.x, v.y, v.z)
```

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `x` | `number` | Componente X |
| `y` | `number` | Componente Y |
| `z` | `number` | Componente Z |

| Método | Descripción |
|--------|-------------|
| `Vec3.new(x, y, z)` | Crea un nuevo Vec3 |

---

### Entity

Representa una entidad de la escena. Desde `onUpdate()` puedes acceder a la entidad propietaria del script.

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `name` | `string` | Nombre de la entidad |
| `active` | `boolean` | Si la entidad está activa |

| Método | Retorno | Descripción |
|--------|---------|-------------|
| `entity:transform()` | `Transform` | Accede al componente Transform |
| `entity:light()` | `Light` | Accede al componente de luz |

---

### Transform

Controla la posición, rotación y escala de una entidad.

```lua
function onUpdate()
    local tr = entity:transform()
    tr.position.x = tr.position.x + 1.0 * Time.deltaTime()
end
```

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `position` | `Vec3` | Posición en el mundo |
| `rotation` | `Vec3` | Rotación en euler (grados) |
| `scale` | `Vec3` | Escala |

---

### Camera

Accede y controla la cámara principal de la escena.

```lua
local cam = Scene.camera()
cam.movementSpeed = 5.0
```

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `position` | `Vec3` | Posición de la cámara (lectura/escritura) |
| `direction` | `Vec3` | Vector frontal normalizado (solo lectura) |
| `up` | `Vec3` | Vector superior normalizado (solo lectura) |
| `right` | `Vec3` | Vector derecho normalizado (solo lectura) |
| `movementSpeed` | `number` | Velocidad base de movimiento |

| Método | Descripción |
|--------|-------------|
| `cam:lookAt(mouseX, mouseY, sensitivity)` | Orienta la cámara según el movimiento del ratón |

---

### Light / PointLight

Accede a la luz de una entidad y configura sus propiedades.

```lua
local light = entity:light()
light.pointLight.intensity = 2.0
light.pointLight.color = Vec3.new(1.0, 0.8, 0.4)
```

**Light**

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `pointLight` | `PointLight` | Datos de la luz puntual |
| `position` | `Vec3` | Posición de la luz |

**PointLight**

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `color` | `Vec3` | Color de la luz (RGB 0–1) |
| `intensity` | `number` | Intensidad |
| `constant` | `number` | Término constante de atenuación |
| `linear` | `number` | Término lineal de atenuación |
| `quadratic` | `number` | Término cuadrático de atenuación |

---

### Input

Detecta entradas de teclado y ratón.

```lua
if Input.isKeyPressed(Key.W) then
    -- mover hacia adelante
end

if Input.isMousePressed(Input.MOUSE_LEFT) then
    -- click izquierdo
end
```

| Método | Retorno | Descripción |
|--------|---------|-------------|
| `Input.isKeyPressed(key)` | `boolean` | `true` mientras la tecla está pulsada |
| `Input.isKeyReleased(key)` | `boolean` | `true` cuando la tecla se suelta |
| `Input.isMousePressed(button)` | `boolean` | `true` mientras el botón está pulsado |
| `Input.mouseX()` | `number` | Posición X del ratón en pantalla |
| `Input.mouseY()` | `number` | Posición Y del ratón en pantalla |
| `Input.lockCursor(lock)` | `boolean` | Bloquea/desbloquea el cursor |

**Botones del ratón**

| Constante | Valor |
|-----------|-------|
| `Input.MOUSE_LEFT` | `0` |
| `Input.MOUSE_RIGHT` | `1` |
| `Input.MOUSE_MIDDLE` | `2` |

---

### Key

Constantes para identificar teclas del teclado.

**Letras:** `Key.A` – `Key.Z`

**Números (fila superior):** `Key.Num0` – `Key.Num9`

**Teclas de función:** `Key.F1` – `Key.F12`

**Especiales**

| Constante | Descripción |
|-----------|-------------|
| `Key.Space` | Barra espaciadora |
| `Key.Enter` | Enter |
| `Key.Escape` | Escape |
| `Key.Tab` | Tabulador |
| `Key.Backspace` | Retroceso |
| `Key.Delete` | Suprimir |
| `Key.Insert` | Insert |

**Flechas:** `Key.Up`, `Key.Down`, `Key.Left`, `Key.Right`

**Modificadores:** `Key.LeftShift`, `Key.RightShift`, `Key.LeftCtrl`, `Key.RightCtrl`, `Key.LeftAlt`, `Key.RightAlt`

**Numpad:** `Key.KP0` – `Key.KP9`, `Key.KPEnter`, `Key.KPAdd`, `Key.KPSubtract`, `Key.KPMultiply`, `Key.KPDivide`, `Key.KPDecimal`

**Navegación:** `Key.PageUp`, `Key.PageDown`, `Key.Home`, `Key.End`

---

### Scene

Gestión de la escena activa.

```lua
Scene.load("scenes/nivel2.scene")
local cam = Scene.camera()
```

| Método | Retorno | Descripción |
|--------|---------|-------------|
| `Scene.load(path)` | — | Carga una escena desde ruta |
| `Scene.camera()` | `Camera` | Devuelve la cámara principal |

---

### Time

Utilidades de tiempo para lógica independiente del framerate.

```lua
function onUpdate()
    local dt = Time.deltaTime()
    -- mover a velocidad constante independiente de los FPS
    tr.position.x = tr.position.x + velocidad * dt
end
```

| Método | Retorno | Descripción |
|--------|---------|-------------|
| `Time.deltaTime()` | `number` | Tiempo en segundos desde el último frame |

---

## Ejemplo completo: controlador de cámara FPS

```lua
local cam
local speed = 5.0
local sensitivity = 0.1

function onStart()
    cam = Scene.camera()
    cam.movementSpeed = speed
    Input.lockCursor(true)
end

function onUpdate()
    local dt = Time.deltaTime()

    -- Movimiento
    if Input.isKeyPressed(Key.W) then
        cam.position.z = cam.position.z - speed * dt
    end
    if Input.isKeyPressed(Key.S) then
        cam.position.z = cam.position.z + speed * dt
    end
    if Input.isKeyPressed(Key.A) then
        cam.position.x = cam.position.x - speed * dt
    end
    if Input.isKeyPressed(Key.D) then
        cam.position.x = cam.position.x + speed * dt
    end

    -- Rotación con el ratón
    cam:lookAt(Input.mouseX(), Input.mouseY(), sensitivity)

    -- Salir con Escape
    if Input.isKeyPressed(Key.Escape) then
        Input.lockCursor(false)
    end
end
```
