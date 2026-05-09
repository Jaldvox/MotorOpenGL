---@meta

-- VEC3
---@class Vec3
---@field x number
---@field y number
---@field z number
Vec3 = {}

---@param x number
---@param y number  
---@param z number
---@return Vec3
function Vec3.new(x, y, z) end

-- TRANSFORM
---@class Transform
---@field position Vec3
---@field rotation Vec3
---@field scale Vec3
Transform = {}

-- ENTITY
---@class Entity
---@field name string
---@field active boolean
Entity = {}

---@return Transform
function Entity:transform() end

---@return Light
function Entity:light() end

-- CAMERA
---@class Camera
---@field position Vec3
---@field yaw number
---@field pitch number
---@field movementSpeed number
Camera = {}

-- POINTLIGHT
---@class PointLight
---@field color Vec3
---@field intensity number
---@field constant number
---@field linear number
---@field quadratic number
PointLight = {}

-- LIGHT
---@class Light
---@field pointLight PointLight
---@field position Vec3
Light = {}

-- INPUT
---@param key integer
---@return boolean
function Input.isKeyPressed(key) end

---@param key integer
---@return boolean
function Input.isKeyReleased(key) end

---@param button integer
---@return boolean
function Input.isMousePressed(button) end

---@return number
function Input.mouseX() end

---@return number
function Input.mouseY() end

Input.MOUSE_LEFT   = 0
Input.MOUSE_RIGHT  = 1
Input.MOUSE_MIDDLE = 2

-- SCENE
---@class SceneClass
Scene = {}

---@param path string
function Scene.load(path) end

---@return Camera
function Scene.camera() end

---@class Time
Time = {}

---@return number
function Time.deltaTime() end
    

---@class KeyClass
Key = {}
Key.A = 65
Key.B = 66
Key.C = 67
Key.D = 68
Key.E = 69
Key.F = 70
Key.G = 71
Key.H = 72
Key.I = 73
Key.J = 74
Key.K = 75
Key.L = 76
Key.M = 77
Key.N = 78
Key.O = 79
Key.P = 80
Key.Q = 81
Key.R = 82
Key.S = 83
Key.T = 84
Key.U = 85
Key.V = 86
Key.W = 87
Key.X = 88
Key.Y = 89
Key.Z = 90

-- Números fila superior
Key.Num0 = 48
Key.Num1 = 49
Key.Num2 = 50
Key.Num3 = 51
Key.Num4 = 52
Key.Num5 = 53
Key.Num6 = 54
Key.Num7 = 55
Key.Num8 = 56
Key.Num9 = 57

-- Función
Key.F1 = 290
Key.F2 = 291
Key.F3 = 292
Key.F4 = 293
Key.F5 = 294
Key.F6 = 295
Key.F7 = 296
Key.F8 = 297
Key.F9 = 298
Key.F10 = 299
Key.F11 = 300
Key.F12 = 301

-- Especiales
Key.Space = 32
Key.Enter = 257
Key.Escape = 256
Key.Tab = 258
Key.Backspace = 259
Key.Delete = 261
Key.Insert = 260

-- Flechas
Key.Right = 262
Key.Left = 263
Key.Down = 264
Key.Up = 265

-- Modificadores
Key.LeftShift = 340
Key.RightShift = 344
Key.LeftCtrl = 341
Key.RightCtrl = 345
Key.LeftAlt = 342
Key.RightAlt = 346

-- Numpad
Key.KP0 = 320
Key.KP1 = 321
Key.KP2 = 322
Key.KP3 = 323
Key.KP4 = 324
Key.KP5 = 325
Key.KP6 = 326
Key.KP7 = 327
Key.KP8 = 328
Key.KP9 = 329
Key.KPDecimal = 330
Key.KPEnter = 335
Key.KPAdd = 334
Key.KPSubtract = 333
Key.KPMultiply = 332
Key.KPDivide = 331

-- Navegación
Key.PageUp = 266
Key.PageDown = 267
Key.Home = 268
Key.End = 269

Key.Unknown = -1