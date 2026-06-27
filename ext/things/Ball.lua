function Ball:onUse(event)
    event.actor:sendMessage("You threw the ball!\n")
    event.actor:dropItem(self)
    
    event.actor:broadcastMessage(event.actor:getName().." threw a ball!\n");
end

function Ball:onNotify(event)
    
    if event.type == Gauzarbeit.Event.Do and event.target == "catch" and not event.actor:hasItem(self) then
        event.actor:gainItem(self)
        event.actor:sendMessage("You caught the ball!\n")
    end
end
