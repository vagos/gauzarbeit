function Cheesecake:doInit()
end

function Cheesecake:onUse(event)
    event.actor:sendMessage("You ate the delicious cheesecake!")
    event.actor:loseItem(self)

    local s = event.actor:getStat("Sweetness")
    event.actor:setStat("Sweetness", s + 1)

    if Gauzarbeit.WithChance(0.5) then
        event.actor:sendMessage("You feel a sugar rush!\n")
        event.actor:setStat("Satisfaction", event.actor:getStat("Satisfaction") + 1)
    end
end
