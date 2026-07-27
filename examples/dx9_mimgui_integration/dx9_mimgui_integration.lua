local mirror = require('mirror')
local memory = require('memory')
local ffi = require('ffi')
local imgui = require('mimgui')

local RASTER_EXT_OFFSET_ADDR = 0xB4E9E0

local camera = nil
local menuActive = imgui.new.bool(false)

function main()
    repeat wait(0) until isSampAvailable()
    sampRegisterChatCommand('mi_camera', onCameraCommand)
    sampRegisterChatCommand('mi_menu', onMenuCommand)
    wait(-1)
end

imgui.OnFrame(
    function() return menuActive[0] end,
    function()
        local xw, yw = getScreenResolution()
        local width, height = 400, 300
        imgui.SetNextWindowPos(imgui.ImVec2(xw / 2, yw / 2), imgui.Cond.FirstUseEver, imgui.ImVec2(0.5, 0.5))
        imgui.SetNextWindowSize(imgui.ImVec2(width, height), imgui.Cond.FirstUseEver)
        imgui.Begin('Camera Info', menuActive)
        imgui.Text(string.format('Camera: %s', camera and 'Created' or 'Not created'))
        if camera then
            local raster = camera.frameBuffer
            local d3dTexture = getD3DTextureFromRaster(raster)
            imgui.Image(d3dTexture, imgui.ImVec2(width - 20, height - 60))
        end
        imgui.End()
    end
)

function onCameraCommand()
    if camera then
        camera:delete()
        camera = nil
        sampAddChatMessage('Camera destroyed', -1)
        return
    end
    local xw, yw = getScreenResolution()
    local x, y, z = getCharCoordinates(PLAYER_PED)
    camera = mirror.createCamera(xw - 1, yw - 1, 32, x, y, z)
    sampAddChatMessage('Camera created', -1)
end

function onMenuCommand()
    menuActive[0] = not menuActive[0]
end

function getD3DTextureFromRaster(raster)
    if not raster then return nil end
    local extOffset = memory.getint32(RASTER_EXT_OFFSET_ADDR)
    if extOffset < 0 then return nil end
    local rasterAddr = ffi.cast('uintptr_t', raster)
    return ffi.cast('void**', ffi.cast('void*', rasterAddr + extOffset))[0]
end