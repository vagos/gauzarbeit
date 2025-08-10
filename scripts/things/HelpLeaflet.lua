function HelpLeaflet:onInspect(inspector)
    local help = [[
COMMANDS:

-- USE <THING_NAME> : Use a thing.
-- CHECK/INSPECT <THING_NAME> : Get extra info about a thing.
-- ATTACK <THING_NAME> : Attack a thing.
-- MOVE <DIRECTION> : Move at a specific direction.
]]
    return help
end

function HelpLeaflet:onUse(user)
    local text = HelpLeaflet.onInspect(self, user)
    user:sendMessage(text)
end
