function Cheesecake:doInit()
end

function Cheesecake:onUse(user)
    user:sendMessage("You ate the delicious cheesecake!")
    user:loseItem(self)

    local s = user:getStat("Sweetness")
    user:setStat("Sweetness", s + 1)

    if Gauzarbeit.WithChance(0.5) then
        user:sendMessage("You feel a sugar rush!\n")
        user:setStat("Satisfaction", user:getStat("Satisfaction") + 1)
    end
end
