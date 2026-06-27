function Cheese:doInit()
    Gauzarbeit.CreateStat("Cheesiness", function(val)
        return tostring(val)
    end)
end

function Cheese:onInspect(inspector)
    return "A tasty piece of cheese\n"
end

function Cheese:onUse(event)

    if (not event.actor:hasItem(self)) then
        return
    end

    event.actor:sendMessage(string.format("You eat the %s. Very cheesy.\n", self:getName()))
    event.actor:loseItem(self) -- Remove the cheese from the inventory.

    local s = event.actor:getStat("Cheesiness")
    event.actor:setStat("Cheesiness", s + 1)
end
