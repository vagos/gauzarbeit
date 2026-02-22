function Cheese:doInit()
    Gauzarbeit.CreateStat("Cheesiness", function(val)
        return tostring(val)
    end)
end

function Cheese:onInspect(inspector)
    return "A tasty piece of cheese\n"
end

function Cheese:onUse(user)

    if (not user:hasItem(self)) then
        return
    end

    user:sendMessage(string.format("You eat the %s. Very cheesy.\n", self:getName()))
    user:loseItem(self) -- Remove the cheese from the inventory.

    local s = user:getStat("Cheesiness")
    user:setStat("Cheesiness", s + 1)
end
