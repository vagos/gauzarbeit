-- Wick McWelcome

function  WelcomeMan:doInit()
    self:gainQuest("SimpleQuest")
end

function WelcomeMan:onNotify(actor, notification_type, target)
    if (notification_type == Gauzarbeit.Event.Greet) then
        self:doSay(actor, "Hello and welcome to Welcome Square!\n")
    end
end

function WelcomeMan:onTalk(talker)
    if (talker:getEventInfo().object == "help") then
        talker:sendMessage("Oh? Here is a Leaflet with some help for starting out.\n")
        talker:gainItem("HelpLeaflet")
    end
end

function WelcomeMan:onInspect(inspector)
    return "He seems like a friendly guy. How about you "..
    "approach him and ask about the quests he has.\n"..
    "Use 'ask WelcomeMan help' for more info!\n"
end

function WelcomeMan:getName()
    return Gauzarbeit.ColorString("WelcomeMan", Gauzarbeit.Color.Green)
end

