local mirror = require('mirror')
local ffi = require("ffi")

ffi.cdef[[
    typedef void*(__cdecl *CallBack)(void* pMaterial, void* pData);
    typedef void*(__cdecl *AtomicRenderCB)(void* pAtomic);
]]

local RwTextureCreate               = ffi.cast("void*(__cdecl*)(void*)", 0x7F37C0)
local RwTextureDestroy              = ffi.cast("void*(__cdecl*)(void*)", 0x7F3820)
local RpClumpForAllAtomics          = ffi.cast("void*(__cdecl*)(void*, void*(__cdecl*)(void*, void*), void*)", 0x749B70)
local RpGeometryForAllMaterials     = ffi.cast("void*(__cdecl*)(void*, void*(__cdecl*)(void*, void*), void*)", 0x74C790)
local AtomicDefaultRenderCallBack   = ffi.cast("AtomicRenderCB", 0x7491C0)

local COMMAND_HANDLERS = {
    ['camera'] = 'onCameraCommand',
    ['screen'] = 'onScreenCommand',
}

local CAMERA_COMMAND_HANDLERS = {
    ['create'] = 'onCameraCreateCommand',
    ['remove'] = 'onCameraRemoveCommand',
}

local SCREEN_COMMAND_HANDLERS = {
    ['create'] = 'onScreenCreateCommand',
    ['remove'] = 'onScreenRemoveCommand',
    ['link'] = 'onScreenLinkCommand',
}

local OBJECT_SCREEN_MODEL = 1518
local TARGET_MATERIAL_NAME = 'CJ_TV_SCREEN'

local cameras = {}
local screens = {}
local screenLinks = {}

local linkCtx = {}
local screenBindings = {}

function main()
    repeat wait(0) until isSampAvailable()
    sampRegisterChatCommand('os', onCommand)
    wait(-1)
end

function onCommand(argsStr)
    local args = string.split(argsStr, ' ')
    if #args == 0 then
        sampAddChatMessage('Usage: /os <command> [args]', -1)
        return
    end
    local command = args[1]:lower()
    local handler = _G[COMMAND_HANDLERS[command]]
    if not handler then
        sampAddChatMessage('Unknown command', -1)
        return
    end
    table.remove(args, 1)
    handler(args)
end

function onCameraCommand(args)
    if #args == 0 then
        sampAddChatMessage('Usage: /os camera <create|remove> [index]', -1)
        return
    end
    local subcommand = args[1]:lower()
    local handler = _G[CAMERA_COMMAND_HANDLERS[subcommand]]
    if not handler then
        sampAddChatMessage('Unknown camera command', -1)
        return
    end
    table.remove(args, 1)
    handler(args)
end

function onScreenCommand(args)
    if #args == 0 then
        sampAddChatMessage('Usage: /os screen <create|remove> [index]', -1)
        return
    end
    local subcommand = args[1]:lower()
    local handler = _G[SCREEN_COMMAND_HANDLERS[subcommand]]
    if not handler then
        sampAddChatMessage('Unknown screen command', -1)
        return
    end
    table.remove(args, 1)
    handler(args)
end

function onCameraCreateCommand()
    local xw, yw = getScreenResolution()
    local x, y, z = getCharCoordinates(PLAYER_PED)
    local heading = getCharHeading(PLAYER_PED)
    local camera = mirror.createCamera(xw - 1, yw - 1, 32, x, y, z, 90, 0, heading+180)
    local index = getAvailableLowestIndex(cameras)
    cameras[index] = camera
    sampAddChatMessage(string.format('Camera created with index %d', index), -1)
end

function onCameraRemoveCommand(args)
    local index = tonumber(args[1])
    if not index or not cameras[index] then
        sampAddChatMessage('Invalid camera index', -1)
        return
    end
    for screenIndex, link in pairs(screenLinks) do
        if link.cameraIndex == index then
            for _, atomic in ipairs(link.atomics) do
                unlinkAtomic(atomic)
            end
            screenLinks[screenIndex] = nil
        end
    end
    cameras[index]:delete()
    cameras[index] = nil
    sampAddChatMessage(string.format('Camera with index %d removed', index), -1)
end

function onScreenCreateCommand()
    local x, y, z = getCharCoordinates(PLAYER_PED)
    local heading = getCharHeading(PLAYER_PED)
    local screen = createObject(OBJECT_SCREEN_MODEL, x, y, z)
    setObjectRotation(screen, 0, 0, heading)
    local index = getAvailableLowestIndex(screens)
    screens[index] = screen
    sampAddChatMessage(string.format('Screen created with index %d', index), -1)
end

function onScreenRemoveCommand(args)
    local index = tonumber(args[1])
    if not index or not screens[index] then
        sampAddChatMessage('Invalid screen index', -1)
        return
    end
    if screenLinks[index] then
        for _, atomic in ipairs(screenLinks[index].atomics) do
            unlinkAtomic(atomic)
        end
        screenLinks[index] = nil
    end
    deleteObject(screens[index])
    screens[index] = nil
    sampAddChatMessage(string.format('Screen with index %d removed', index), -1)
end

function onScreenLinkCommand(args)
    if #args < 2 then
        sampAddChatMessage('Usage: /os screen link <screen_index> <camera_index>', -1)
        return
    end
    local screenIndex = tonumber(args[1])
    local cameraIndex = tonumber(args[2])
    if not screenIndex or not screens[screenIndex] then
        sampAddChatMessage('Invalid screen index', -1)
        return
    end
    if not cameraIndex or not cameras[cameraIndex] then
        sampAddChatMessage('Invalid camera index', -1)
        return
    end
    local screen = screens[screenIndex]
    local camera = cameras[cameraIndex]
    if screenLinks[screenIndex] then
        for _, atomic in ipairs(screenLinks[screenIndex].atomics) do
            unlinkAtomic(atomic)
        end
        screenLinks[screenIndex] = nil
    end
    local atomics = linkEntityToRaster(getObjectPointer(screen), camera.frameBuffer, TARGET_MATERIAL_NAME)
    if not atomics then
        sampAddChatMessage(string.format('Failed to link screen (material "%s" not found)', TARGET_MATERIAL_NAME), -1)
        return
    end
    screenLinks[screenIndex] = { atomics = atomics, cameraIndex = cameraIndex }
    sampAddChatMessage(string.format('Linked screen %d to camera %d', screenIndex, cameraIndex), -1)
end

function linkEntityToRaster(pEntity, raster, targetMaterialName)
    local camTexture = RwTextureCreate(ffi.cast("void*", raster))
    if camTexture == nil then return nil end

    local pRwObject = ffi.cast("uintptr_t*", pEntity + 0x18)[0]
    if pRwObject == nil then return nil end
    local rwType = ffi.cast("unsigned char*", pRwObject)[0]

    linkCtx = { camTexture = camTexture, targetName = targetMaterialName, atomic = nil, boundAtomics = {} }

    if rwType == 1 then
        onAtomicFound(ffi.cast("void*", pRwObject), nil)
    elseif rwType == 2 then
        RpClumpForAllAtomics(ffi.cast("void*", pRwObject), onAtomicFoundCB, nil)
    end

    local boundAtomics = linkCtx.boundAtomics
    linkCtx = nil

    if #boundAtomics == 0 then
        ffi.cast("void**", camTexture)[0] = nil
        RwTextureDestroy(camTexture)
        return nil
    end
    return boundAtomics
end

function onAtomicFound(pAtomic, _)
    linkCtx.atomic = pAtomic
    local pGeometry = ffi.cast("uintptr_t*", ffi.cast("uintptr_t", pAtomic) + 0x18)[0]
    RpGeometryForAllMaterials(ffi.cast("void*", pGeometry), onMaterialFoundCB, nil)
    return pAtomic
end

function onMaterialFound(pMaterial, _)
    local curTex = ffi.cast("void**", pMaterial)[0]
    local matches = not linkCtx.targetName or
        (curTex ~= nil and ffi.string(ffi.cast("char*", ffi.cast("uintptr_t", curTex) + 0x10)):lower() == linkCtx.targetName:lower())

    if matches then
        local cbSlot = ffi.cast("AtomicRenderCB*", ffi.cast("uintptr_t", linkCtx.atomic) + 0x48) 
        screenBindings[atomicKey(linkCtx.atomic)] = {
            material = pMaterial, originalTexture = curTex,
            camTexture = linkCtx.camTexture, originalCallback = cbSlot[0],
        }
        cbSlot[0] = customScreenRenderCB
        table.insert(linkCtx.boundAtomics, linkCtx.atomic)
    end
    return pMaterial
end

function customScreenRenderImpl(pAtomic)
    local binding = screenBindings[atomicKey(pAtomic)]
    if binding then
        local materialTexSlot = ffi.cast("void**", binding.material)
        materialTexSlot[0] = binding.camTexture
        local result = binding.originalCallback(pAtomic)
        materialTexSlot[0] = binding.originalTexture
        return result
    end
    return AtomicDefaultRenderCallBack(pAtomic)
end

customScreenRenderCB = ffi.cast("AtomicRenderCB", customScreenRenderImpl)
onMaterialFoundCB = ffi.cast("CallBack", onMaterialFound)
onAtomicFoundCB = ffi.cast("CallBack", onAtomicFound)

function atomicKey(pAtomic) 
    return tonumber(ffi.cast("uintptr_t", pAtomic))
end

function unlinkAtomic(pAtomic)
    local key = atomicKey(pAtomic)
    local binding = screenBindings[key]
    if not binding then return end

    ffi.cast("AtomicRenderCB*", ffi.cast("uintptr_t", pAtomic) + 0x48)[0] = binding.originalCallback
    ffi.cast("void**", binding.material)[0] = binding.originalTexture
    ffi.cast("void**", binding.camTexture)[0] = nil
    RwTextureDestroy(binding.camTexture)

    screenBindings[key] = nil
end

function onScriptTerminate(script, quit)
    if script ~= thisScript() then return end
    for _, link in pairs(screenLinks) do
        for _, atomic in ipairs(link.atomics) do
            unlinkAtomic(atomic)
        end
    end
    screenLinks = {}
    for index, camera in pairs(cameras) do
        camera:delete()
        cameras[index] = nil
    end
    for index, screen in pairs(screens) do
        deleteObject(screen)
        screens[index] = nil
    end
end

function getAvailableLowestIndex(t)
    local index = 1
    while t[index] do
        index = index + 1
    end
    return index
end

function string.split(inputstr, sep)
    if sep == nil then
      sep = "%s"
    end
    local t = {}
    for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
      table.insert(t, str)
    end
    return t
end