
function RedGlasses:onNotify(actor, notification_type, target)

    if (notification_type == Gauzarbeit.Event.Inspect) then
        
        actor:sendMessage("\nYou see with your Red Glasses: \n\n")

        actor:sendMessage("HP: "..target:getHP().."\n")   
    end

end

function RedGlasses:onInspect(inspector)
    
    return "A pair of glasses that lets you see MORE.\n"

end
