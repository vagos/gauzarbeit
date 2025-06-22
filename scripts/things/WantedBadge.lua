Thing = WantedBadge

function Thing:doInit()
    
    Gauzarbeit.CreateStat("Wanted Level", function (val) return string.rep("*", val % 10) end)

end


function Thing:onNotify(actor, notification_type, target)

    if (not actor:hasItem(self)) then return end

    if notification_type == Gauzarbeit.Event.Kill then  
        actor:setStat("Wanted Level", actor:getStat("Wanted Level") + 1)
    end

end

function Thing:onInspect(inspector)
    return "You are hunted by the police\n"
end
