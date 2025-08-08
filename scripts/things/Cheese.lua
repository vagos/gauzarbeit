function Cheese:doInit()

    Gauzarbeit.CreateStat("Cheesiness", function(val)
        return tostring(val)
    end)

end

function Cheese:onInspect(inspector)
    return "A tasty piece of cheese\n"
end

function Cheese:onUse(user)

    -- if (not user:hasItem(self)) then
    --     user:sendMessage("You don't have any cheese on you!\n")
    --     return
    -- end

    print(self:getName())

    user:sendMessage("You eat the cheese. Very cheesy.\n")

    user:loseItem(self) -- Remove the cheese from the inventory.

    local s = user:getStat("Cheesiness")
    user:setStat("Cheesiness", s + 1)
end
