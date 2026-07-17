local mirror = require('mirror')
local ev = require('lib.samp.events')
local ffi = require('ffi')

local getBonePosition = ffi.cast("int (__thiscall*)(void*, float*, int, bool)", 0x5E4280)

local target = -1
local camera = nil
local screen = nil

local TYPE_PIXEL = 1
local TYPE_PERCENT = 2

local TRANSITION = 0.3
local timeSinceLastTransition = 0

local SCREEN_POS = {
    x = {
        type = TYPE_PERCENT,
        value = 60
    },
    y = {
        type = TYPE_PERCENT,
        value = 60
    },
    w = {
        type = TYPE_PIXEL,
        value = 600
    },
    h = {
        type = TYPE_PIXEL,
        value = 400
    }
}

local camPos = {
    current = nil,
    last = {
        x = 0,
        y = 0,
        z = 0
    }
}

function main()
    repeat wait(0) until isSampAvailable()
    sampRegisterChatCommand('spy', onSpyCommand)
    while true do
        wait(0)
        processCamera()
    end
end

function processCamera()
    if not camera then
        return
    end
    if not camPos.current then
        return
    end
    local doesExist, targetPed = sampGetCharHandleBySampPlayerId(target)
    if not doesExist then
        destroyAll()
        sampAddChatMessage('Player disappeared', -1)
        return
    end
    local hx, hy, hz = getBodyPartCoordinates(8, targetPed)
    camPos.current.x = bringFloatTo(camPos.current.x, camPos.last.x, timeSinceLastTransition, TRANSITION)
    camPos.current.y = bringFloatTo(camPos.current.y, camPos.last.y, timeSinceLastTransition, TRANSITION)
    camPos.current.z = bringFloatTo(camPos.current.z, camPos.last.z, timeSinceLastTransition, TRANSITION)
    timeSinceLastTransition = os.clock()
    camera.posX = hx
    camera.posY = hy
    camera.posZ = hz

    local angle = getHeadingFromVector2d(camPos.current.x, camPos.current.y)
    camera.rotX = 90
    camera.rotY = 0
    camera.rotZ = angle + 180
end

function onSpyCommand(arg)
    if camera then
        destroyAll()
        sampAddChatMessage('Disabled', -1)
        return
    end
    target = arg:match('(%d+)')
    if target == nil then
        sampAddChatMessage('Usage: /spy [playerid]', -1)
        return
    end
    target = tonumber(target)
    local doesExists, _ = sampGetCharHandleBySampPlayerId(target)
    if not doesExists then
        sampAddChatMessage('Player not found', -1)
        return
    end
    createAll()
    sampAddChatMessage('Enabled', -1)
end

function ev.onAimSync(playerId, data)
    if camera and playerId == target then
        camPos.last.x = data.camFront.x
        camPos.last.y = data.camFront.y
        camPos.last.z = data.camFront.z
        if camPos.current == nil then
            camPos.current = {
                x = data.camFront.x,
                y = data.camFront.y,
                z = data.camFront.z
            }
        end
    end
end

function bringFloatTo(from, to, start_time, duration) -- by cosmo
    local timer = os.clock() - start_time
    if timer >= 0.00 and timer <= duration then
        local count = timer / (duration / 100)
        return from + (count * (to - from) / 100), true
    end
    return (timer > duration) and to or from, false
end

function getBodyPartCoordinates(id, handle)
    local pedptr = getCharPointer(handle)
    local vec = ffi.new("float[3]")
    getBonePosition(ffi.cast("void*", pedptr), vec, id, true)
    return vec[0], vec[1], vec[2]
end

function createAll()
    destroyAll()
    local sx, sy = getScreenResolution()
    camera = mirror.createCamera(sx - 10, sy, 32)
    local x = sx / 100 * SCREEN_POS.x.value
    local y = sy / 100 * SCREEN_POS.y.value
    screen = mirror.createScreen2D(x, y, x + SCREEN_POS.w.value, y + SCREEN_POS.h.value, camera, 90, 0, 0)
end

function destroyAll()
    if screen then
        screen:delete()
        screen = nil
    end
    if camera then
        camera:delete()
        camera = nil
    end
    camPos = {
        current = nil,
        last = {
            x = 0,
            y = 0,
            z = 0
        }
    }
end

function onScriptTerminate(script, quit)
    if script == thisScript() then
        destroyAll()
    end
end