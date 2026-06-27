function PokeBall:doInit()
    self.thingCaught = nil
end

function PokeBall:onInspect(inspector)
    if not self.thingCaught then
        return "An empty PokeBall for keeping one nearby thing.\n"
    end

    return "A PokeBall holding "..self.thingCaught:getName()..".\n"
end

function PokeBall:onUse(event)
    if not self.thingCaught then
        local target_name = event.object
        if target_name == "" then
            event.actor:sendMessage("Use PokeBall <thing> to catch something nearby.\n")
            return
        end

        local target = event.actor:getThing(target_name)
        if not target then
            event.actor:sendMessage("The PokeBall snaps shut on absolutely nothing.\n")
            return
        end

        self:gainItem(target)
        self.thingCaught = target
        event.actor:sendMessage("The PokeBall catches "..target:getName()..".\n")
        return
    end

    local released = self.thingCaught
    self.thingCaught = nil
    self:dropItem(released)
    event.actor:sendMessage("The PokeBall releases "..released:getName()..".\n")
end
