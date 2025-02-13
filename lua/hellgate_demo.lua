-- SPDX-License-Identifier: GPL-3.0-only

local screen_bitmap = "doom"
local capture_input = false

function PluginMetadata()
    return {
        name = "hellgate_demo",
        author = "MangoFizz",
        version = "1.0.0",
        targetApi = "1.1.0",
        reloadable = true
    }
end

function PluginLoad() 
    Balltze.event.mapLoad.subscribe(function (ev)  
        if ev.time == "before" then
            Balltze.features.importTagFromMap("ui", screen_bitmap, Engine.tag.classes.bitmap)
        else
            Balltze.command.executeCommand("hellgate_set_screen_bitmap " .. screen_bitmap)
            Balltze.command.executeCommand("hellgate_render_on_screen true")
            Balltze.command.executeCommand("hellgate_resume")
        end
    end)

    Balltze.event.keyboardInput.subscribe(function (ev)
        if ev.time == "before" then
            if ev.context.key.character == 107 then -- k
                if capture_input then
                    Balltze.command.executeCommand("hellgate_capture_input false")
                else
                    Balltze.command.executeCommand("hellgate_capture_input true")
                end
                capture_input = not capture_input
            end
        end
    end)
    
    return true
end
