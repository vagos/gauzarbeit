
function RedGlasses:onNotify(actor, notification_type, target)

    if not target then
        return
    end

    if (notification_type == Gauzarbeit.Event.Inspect) then
        actor:sendMessage("\nYou see with your Red Glasses: \n\n")
        local health = target:getStat("Health")
        local max_health = target:getStat("MaxHealth")
        if health == 0 then
            health = max_health
        end
        actor:sendMessage("HP: "..health.."/"..max_health.."\n")
    end

end

function RedGlasses:onInspect(inspector)
    return "A pair of glasses that lets you see MORE.\n"
end
