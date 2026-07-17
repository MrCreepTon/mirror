local cam = require('mirror_core')

addEventHandler('onScriptTerminate', function(script, quit)
    if script == thisScript() then
        cam.unload()
    end
end)

return cam