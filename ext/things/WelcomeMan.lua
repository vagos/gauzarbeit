-- Wick McWelcome

local function doRatQuest(self, player)
    local rat_task = "Kill 5 Rats."

    if (player:rewardTask(rat_task)) then
        player:sendMessage("Good work. Here are ten Winston cigarretes.\n")
        for i = 1, 10 do
            player:gainItem("Winston")
        end
    elseif (player:hasDoneTask(rat_task)) then
        player:sendMessage("Thanks again for handling those rats.\n")
    elseif (player:hasTask(rat_task)) then
        player:sendMessage("I still need you to kill five rats for me.\n")
    else
        player:sendMessage("I need some help with the rats around here. Can you kill some of them for me?\n")
        self:giveTask(
            player,
            rat_task,
            { rats = 0 },
            function(state, owner, actor, notification_type, target)
                if notification_type == Gauzarbeit.Event.Kill and target:getName() == "Rat" then
                    state.rats = state.rats + 1
                end

                return state, state.rats >= 5
            end,
            function(state, description)
                return description .. " " .. state.rats .. "/5 Rats killed."
            end
        )
    end
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
        return
    end

    if (talker:getEventInfo().object == "rats") then
        doRatQuest(self, talker)
        return
    end

    if (talker:getEventInfo().object ~= "") then
        self:giveTask(talker, talker:getEventInfo().object)
    end
end

function WelcomeMan:onInspect(inspector)
    return "He seems like a friendly guy. How about you "..
    "approach him and ask about the tasks he has.\n"..
    "Use 'ask WelcomeMan help' for more info!\n"
end

function WelcomeMan:getName()
    return Gauzarbeit.ColorString("WelcomeMan", Gauzarbeit.Color.Green)
end
