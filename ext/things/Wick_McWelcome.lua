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

function Wick_McWelcome:onNotify(event)
    if (event.type == Gauzarbeit.Event.Enter) then
        self:doSay(event.actor, "Hello and welcome to Welcome Square!\n")
    end
end

function Wick_McWelcome:onTalk(event)
    if (event.object == "help") then
        event.actor:sendMessage("Oh? Here is a Leaflet with some help for starting out.\n")
        event.actor:gainItem("HelpLeaflet")
        return
    end

    if (event.object == "rats") then
        doRatQuest(self, event.actor)
        return
    end

    if (event.object ~= "") then
        self:giveTask(event.actor, event.object)
    end
end

function Wick_McWelcome:onInspect(inspector)
    return "He seems like a friendly guy. How about you "..
    "approach him and ask about the tasks he has.\n"..
    "Use 'ask Wick_McWelcome help' for more info!\n"
end

function Wick_McWelcome:getName()
    return Gauzarbeit.ColorString("Wick McWelcome", Gauzarbeit.Color.Green)
end
