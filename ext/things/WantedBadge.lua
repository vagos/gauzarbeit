Thing = WantedBadge

function Thing:doInit()
    
    Gauzarbeit.CreateStat("Wanted Level", function (val) return string.rep("*", val % 10) end)

end


function Thing:onNotify(event)

    if (not event.actor:hasItem(self)) then return end

    if event.type == Gauzarbeit.Event.Kill then  
        event.actor:setStat("Wanted Level", event.actor:getStat("Wanted Level") + 1)
    end

end

function Thing:onInspect(inspector)
    return "You are hunted by the police\n"
end
