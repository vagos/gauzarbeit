Thing = Firework

function Thing:doInit()
    self.fuseLit = false
end

local function RainbowString(parts)
    local colors = {
        Gauzarbeit.Color.Red,
        Gauzarbeit.Color.Yellow,
        Gauzarbeit.Color.Green,
        Gauzarbeit.Color.Cyan,
        Gauzarbeit.Color.Blue,
        Gauzarbeit.Color.Magenta,
    }

    local text = ""
    for index, part in ipairs(parts) do
        local color = colors[((index - 1) % #colors) + 1]
        text = text..Gauzarbeit.ColorString(part, color, Gauzarbeit.Style.Bold)
    end

    return text
end

function Thing:onUse(event)
    if self.fuseLit then
        event.actor:sendMessage("The Firework is already hissing. This is not the time for bravery.\n")
        return
    end

    self.fuseLit = true
    event.actor:sendMessage("You light the Firework. The fuse snaps to life.\n")
    event.actor:broadcastMessage(event.actor:getName().." lit a Firework. It starts hissing.\n")
    self:doDestroy()

    self:doLater(function()
        event.actor:sendMessage("The Firework spits green sparks across the floor.\n")
    end, 1)

    self:doLater(function()
        event.actor:broadcastMessage("The Firework jumps, screams, and paints the room with sparks.\n")
    end, 3)

    self:doLater(function()
        local pop = RainbowString({"BANG", "! ", "The ", "Firework ", "erupts ", "in ", "rainbow ", "sparks", "!\n"})
        event.actor:sendMessage(pop)
        event.actor:broadcastMessage(pop)
    end, 5)
end

function Thing:onInspect(inspector)
    if self.fuseLit then
        return "A lit Firework. It is making increasingly poor choices.\n"
    end

    return "A bright paper Firework. Use it to start a spectacle.\n"
end

function Thing:onHelp(user)
    return "Use Firework to light it. The effects happen over the next few seconds.\n"
end
