local memory = require('memory')
local vk = require('vkeys')
local bit = require('bit')
local ffi = require('ffi')
local mirror = require('mirror')

local Drone = {}
local VEHICLE_MODEL_ID = 465

local APPLY_AIR_RESISTANCE_ADDR = 0x544C40

local applyAirResistance = ffi.cast('char (__thiscall *)(void*)', APPLY_AIR_RESISTANCE_ADDR)

local KEY_UP = vk.VK_NUMPAD9
local KEY_DOWN = vk.VK_NUMPAD3

local KEY_FORWARD = vk.VK_NUMPAD8
local KEY_BACKWARD = vk.VK_NUMPAD2
local KEY_LEFT = vk.VK_NUMPAD6
local KEY_RIGHT = vk.VK_NUMPAD4

local STOP_SPEED = 0.001
local TURN_SPEED = 0.025
local SPEED = 0.005
local MAX_SPEED = 0.5
local MAX_SPEED_Z = 0.05

local state = false
local drone = nil

function main()
    repeat wait(0) until isSampAvailable()
    sampRegisterChatCommand('drone', onDroneCommand)
    while true do
        wait(0)
        if drone then
            drone:update()
        end
    end
end

function onDroneCommand()
    state = not state
    if state then
        createDrone()
    else
        removeDrone()
    end
end

function createDrone()
    removeDrone()
    local x, y, z = getCharCoordinates(PLAYER_PED)
    local angle = getCharHeading(PLAYER_PED)
    drone = Drone.new(x, y, z, angle)
end

function removeDrone()
    if drone then
        drone:remove()
        drone = nil
    end
end

function onScriptTerminate(script, quit)
    if script == thisScript() then
        removeDrone()
    end
end

function Drone.new(posX, posY, posZ, angle)
    local spawnOffset = {
        x = 1.5,
        y = 0,
        z = 4
    }
    local instance = {
        position = {
            x = posX + spawnOffset.x,
            y = posY + spawnOffset.y,
            z = posZ + spawnOffset.z
        },
        cameraOffset = {
            x = 1,
            y = 1,
            z = -0.05
        },
        angle = angle
    }
    setmetatable(instance, {__index=Drone})
    instance:_createCar()

    instance.camera = mirror.createCamera(1910, 1080, 32, instance.position.x, instance.position.y, instance.position.z, 90, 0, instance.angle)
    instance.screen = mirror.createScreen2D(1200, 500, 1700, 1000, instance.camera)

    return instance
end

function Drone:_createCar()
    requestModel(VEHICLE_MODEL_ID)
    loadAllModelsNow()
    self.car = createCar(VEHICLE_MODEL_ID, self.position.x, self.position.y, self.position.z)
    setCarHeading(self.car, self.angle)
    setCarProofs(self.car, true, true, true, true, true)
    setCarEngineOn(self.car, true)

    local pCar = getCarPointer(self.car)
    local dwFlags = memory.getint32(pCar + 0x40)
    memory.setint32(pCar + 0x40, bit.bor(dwFlags, 0x40))
end

function Drone:update()
    if not self.car then
        return
    end
    self.position.x, self.position.y, self.position.z = getCarCoordinates(self.car)
    self.angle = getCarHeading(self.car)

    self.camera.posX = self.position.x + math.sin(-math.rad(self.angle)) * self.cameraOffset.x
    self.camera.posY = self.position.y + math.cos(-math.rad(self.angle)) * self.cameraOffset.y
    self.camera.posZ = self.position.z + self.cameraOffset.z
    self.camera.rotZ = self.angle - 180

    self:processKeys()
end

function Drone:processKeys()
    local speed = {x = 0, y = 0, z = 0}
    local turnSpeed = {x = 0, y = 0, z = 0}
    if isKeyDown(KEY_FORWARD) then
        speed.x = speed.x + SPEED * math.sin(-math.rad(self.angle))
        speed.y = speed.y + SPEED * math.cos(-math.rad(self.angle))
    end
    if isKeyDown(KEY_BACKWARD) then
        speed.x = speed.x - SPEED * math.sin(-math.rad(self.angle))
        speed.y = speed.y - SPEED * math.cos(-math.rad(self.angle))
    end
    if isKeyDown(KEY_LEFT) then
        turnSpeed.z = turnSpeed.z - TURN_SPEED
    end
    if isKeyDown(KEY_RIGHT) then
        turnSpeed.z = turnSpeed.z + TURN_SPEED
    end
    if isKeyDown(KEY_UP) then
        speed.z = speed.z + SPEED
    end
    if isKeyDown(KEY_DOWN) then
        speed.z = speed.z - SPEED
    end
    self:applySpeed(speed.x, speed.y, speed.z)
    self:setTurnSpeed(turnSpeed.x, turnSpeed.y, turnSpeed.z)
    applyAirResistance(ffi.cast('void*', getCarPointer(self.car)))
end

function Drone:applySpeed(x, y, z)
    local pCar = getCarPointer(self.car)
    if pCar == 0 then
        return
    end
    local newSpeedX = memory.getfloat(pCar + 0x44) + x
    local newSpeedY = memory.getfloat(pCar + 0x44 + 0x4) + y
    local newSpeedZ = memory.getfloat(pCar + 0x44 + 0x4 * 2) + z

    if math.abs(newSpeedX) > MAX_SPEED then
        newSpeedX = MAX_SPEED * math.sign(newSpeedX)
    end
    if math.abs(newSpeedY) > MAX_SPEED then
        newSpeedY = MAX_SPEED * math.sign(newSpeedY)
    end
    if math.abs(newSpeedZ) > MAX_SPEED_Z then
        newSpeedZ = MAX_SPEED_Z * math.sign(newSpeedZ)
    end
    if math.abs(newSpeedX) > 0 then
        newSpeedX = newSpeedX - STOP_SPEED * math.sign(newSpeedX)
    end
    if math.abs(newSpeedY) > 0 then
        newSpeedY = newSpeedY - STOP_SPEED * math.sign(newSpeedY)
    end
    if math.abs(newSpeedZ) > 0 then
        newSpeedZ = newSpeedZ - STOP_SPEED * math.sign(newSpeedZ)
    end

    self:setSpeed(newSpeedX, newSpeedY, newSpeedZ)
end

function Drone:setSpeed(x, y, z)
    local pCar = getCarPointer(self.car)
    if pCar == 0 then
        return
    end
    memory.setfloat(pCar + 0x44, x)
    memory.setfloat(pCar + 0x44 + 0x4, y)
    memory.setfloat(pCar + 0x44 + 0x4 * 2, z)
end

function Drone:applyTurnSpeed(x, y, z)
    local pCar = getCarPointer(self.car)
    if pCar == 0 then
        return
    end
    local newSpeedX = memory.getfloat(pCar + 0x50) + x
    local newSpeedY = memory.getfloat(pCar + 0x50 + 0x4) + y
    local newSpeedZ = memory.getfloat(pCar + 0x50 + 0x4 * 2) + z
    
    self:setTurnSpeed(newSpeedX, newSpeedY, newSpeedZ)
end

function Drone:setTurnSpeed(x, y, z)
    local pCar = getCarPointer(self.car)
    if pCar == 0 then
        return
    end
    memory.setfloat(pCar + 0x50, x)
    memory.setfloat(pCar + 0x50 + 0x4, y)
    memory.setfloat(pCar + 0x50 + 0x4 * 2, z)
end

function Drone:remove()
    if self.screen then
        self.screen:delete()
        self.screen = nil
    end
    if self.camera then
        self.camera:delete()
        self.camera = nil
    end
    if self.car then
        deleteCar(self.car)
        self.car = nil
    end
end

function math.sign(value)
    if value < 0 then
        return -1
    end
    return 1
end