local mirror = require('mirror')

local state = false

local camera = nil
local screen = nil

local SCREEN_POS = {
    X = 1250,
    Y = 600,
    WIDTH = 650,
    HEIGHT = 400
}

function main()
    repeat wait(0) until isSampAvailable()
    sampRegisterChatCommand('back', onBackCommand)
    while true do
        wait(0)
        processBack()
    end
end

function processBack()
    if not state or camera == nil then
        return
    end
    local cx, cy, cz = getActiveCameraCoordinates()
    local px, py, pz = getActiveCameraPointAt()
    local angleZ = math.atan2(cy - py, cx - px) * 180.0 / math.pi - 90
    camera.posX = cx
    camera.posY = cy
    camera.posZ = cz
    camera.rotX = 0
    camera.rotY = 0
    camera.rotZ = angleZ
end

function onBackCommand()
    state = not state
    if state then
        createBackCamera()
        return
    end
    removeBackCamera()
end

function createBackCamera()
    removeBackCamera()
    camera = mirror.createCamera(1910, 1080, 32, 0, 0, 0, 0, 0, 0)
    screen = mirror.createScreen2D(SCREEN_POS.X, SCREEN_POS.Y, SCREEN_POS.X + SCREEN_POS.WIDTH, SCREEN_POS.Y + SCREEN_POS.HEIGHT, camera)
end

function removeBackCamera()
    if screen then
        screen:delete()
        screen = nil
    end
    if camera then
        camera:delete()
        camera = nil
    end
end