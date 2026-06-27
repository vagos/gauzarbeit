
function RedGlasses:onNotify(event)

    if not event.targetThing then
        return
    end

    if (event.type == Gauzarbeit.Event.Inspect) then
        event.actor:sendMessage("\nYou see with your Red Glasses: \n\n")
        local health = event.targetThing:getStat("Health")
        local max_health = event.targetThing:getStat("MaxHealth")
        if health == 0 then
            health = max_health
        end
        event.actor:sendMessage("HP: "..health.."/"..max_health.."\n")
    end

end

function RedGlasses:onInspect(inspector)
    return "A pair of glasses that lets you see MORE.\n"
end
