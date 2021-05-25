function Ball:onUse(user)
    user:sendMessage("You threw the ball!\n")
    user:dropItem(self)
    
    user:broadcastMessage(user:getName().." threw a ball!\n");
end

function Ball:onNotify(actor, notification_type)
    
    if notification_type == Gauzarbeit.Event.Catch and not actor:hasItem(self) then
        actor:gainItem(self)
        actor:sendMessage("You caught the ball!\n")
    end

end
