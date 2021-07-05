
function  WelcomeMan:doInit()

    self:gainQuest("SimpleQuest")
    
end

function WelcomeMan:onNotify(actor, notification_type, target)
    
    if (notification_type == Gauzarbeit.Event.Greet) then
        self:doSay("Hello!\n", actor)
    end
end
