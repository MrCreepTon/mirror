local mirror = require('mirror')

local SCREEN_DISTANCE = 3.0 -- meters in front of the player
local SCREEN_WIDTH = 5.0
local SCREEN_HEIGHT = 3.0

local camera = nil
local screen = nil

function main()
    repeat wait(0) until isSampAvailable()
    sampRegisterChatCommand('tv', onTvCommand)
    wait(-1)
end

function onTvCommand()
    if camera then
        destroyTv()
        return
    end
    createTv()
end

function createTv()
    local px, py, pz = getCharCoordinates(PLAYER_PED)
    local heading = getCharHeading(PLAYER_PED)

    -- direction the player is facing
    local dirX = math.sin(-math.rad(heading))
    local dirY = math.cos(-math.rad(heading))

    -- screen center, SCREEN_DISTANCE meters in front of the player
    local cx = px + dirX * SCREEN_DISTANCE
    local cy = py + dirY * SCREEN_DISTANCE

    -- right vector of the screen plane (perpendicular to the view direction)
    local rightX, rightY = dirY, -dirX

    local hw = SCREEN_WIDTH / 2
    local topZ = pz + SCREEN_HEIGHT

    -- camera hangs at the screen center and looks back at the player
    local x, y = getScreenResolution()
    camera = mirror.createCamera(x - 1, y - 1, 32, cx, cy, pz + 0.5, 90, 0, heading)

    screen = mirror.createScreen3D(
        cx - rightX * hw, cy - rightY * hw, pz, -- left bottom
        cx + rightX * hw, cy + rightY * hw, pz, -- right bottom
        cx - rightX * hw, cy - rightY * hw, topZ,    -- left top
        cx + rightX * hw, cy + rightY * hw, topZ,    -- right top
        camera
    )
end

function destroyTv()
    if screen then
        screen:delete()
        screen = nil
    end
    if camera then
        camera:delete()
        camera = nil
    end
end

function onScriptTerminate(script, quit)
    if script == thisScript() then
        destroyTv()
    end
end
