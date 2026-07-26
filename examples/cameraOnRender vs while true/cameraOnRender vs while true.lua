local mirror = require('mirror')

local font = renderCreateFont('Arial', 16, 0)

local state = false

local cameras = {
    ['cameraOnRender'] = 0,
    ['whileTrue'] = 0
}

local screens = {
    ['cameraOnRender'] = 0,
    ['whileTrue'] = 0 
}

local OFFSET = {
    x = -5,
    y = -5,
    z = 2
}

local INFO_BAR_HEIGHT = 30
local INFO_BAR_TEXT_OFFSET_X = 10
local INFO_BAR_TEXT_OFFSET_Y = 5

function main()
    repeat wait(0) until isSampAvailable()
    sampRegisterChatCommand('diff', onDiffCommand)
    while true do
        wait(0)
        processWhileTrueCamera()
        processInfo()
    end
end

function processWhileTrueCamera()
    if not state then return end
    if not cameras['whileTrue'] then return end
    local camData = getCameraPositionAndRotation()
    cameras['whileTrue'].posX, cameras['whileTrue'].posY, cameras['whileTrue'].posZ = camData.position.x, camData.position.y, camData.position.z
    cameras['whileTrue'].rotX, cameras['whileTrue'].rotY, cameras['whileTrue'].rotZ = camData.rotation.x, camData.rotation.y, camData.rotation.z
end

function onCameraRender(camera)
    if not state then return end
    if not cameras['cameraOnRender'] then return end
    if camera ~= cameras['cameraOnRender'] then return end
    local camData = getCameraPositionAndRotation()
    cameras['cameraOnRender'].posX, cameras['cameraOnRender'].posY, cameras['cameraOnRender'].posZ = camData.position.x, camData.position.y, camData.position.z
    cameras['cameraOnRender'].rotX, cameras['cameraOnRender'].rotY, cameras['cameraOnRender'].rotZ = camData.rotation.x, camData.rotation.y, camData.rotation.z
end

function processInfo()
    if not state then return end
    local xw, yw = getScreenResolution()
    yw = yw - INFO_BAR_HEIGHT + INFO_BAR_TEXT_OFFSET_Y
    renderDrawBox(0, yw - INFO_BAR_TEXT_OFFSET_Y, xw, yw + INFO_BAR_HEIGHT, 0xFF000000)
    renderFontDrawText(font, 'onCameraRender', INFO_BAR_TEXT_OFFSET_X, yw, -1)
    renderFontDrawText(font, 'while true', xw / 2 + INFO_BAR_TEXT_OFFSET_X, yw, -1)
end

function getCameraPositionAndRotation()
    local x, y, z = getCharCoordinates(PLAYER_PED)
    local heading = getCharHeading(PLAYER_PED)
    x = x + math.sin(-math.rad(heading)) * OFFSET.x
    y = y + math.cos(-math.rad(heading)) * OFFSET.y
    z = z + OFFSET.z
    return {
        position = {
            x = x,
            y = y,
            z = z
        },
        rotation = {
            x = 90 + 20,
            y = 0,
            z = heading + 180
        }
    }
end

function onDiffCommand()
    state = not state
    sampAddChatMessage(state and 'ON' or 'OFF', -1)
    if state then
        onActivate()
        return
    end
    onDeactivate()
end

function onActivate()
    local xw, yw = getScreenResolution()
    for i, _ in pairs(cameras) do
        cameras[i] = mirror.createCamera(xw - 10, yw - 10, 32, 0, 0, 0, 90, 0, 0)
    end
    yw = yw - INFO_BAR_HEIGHT
    screens['cameraOnRender'] = mirror.createScreen2D(0, 0, xw / 2, yw, cameras['cameraOnRender'])
    screens['whileTrue'] = mirror.createScreen2D(xw / 2, 0, xw, yw, cameras['whileTrue'])
end

function onDeactivate()
    for i, _ in pairs(cameras) do
        if cameras[i] then
            cameras[i]:delete()
            cameras[i] = 0
        end
    end
    for i, _ in pairs(screens) do
        if screens[i] then
            screens[i]:delete()
            screens[i] = 0
        end
    end
end