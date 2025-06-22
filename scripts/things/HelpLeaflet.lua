function HelpLeaflet:onInspect(inspector)

    local helpFile = io.open("./scripts/things/HELP.txt", "r")

    local fileContent = helpFile:read("a")

    helpFile:close()

    return fileContent
end

function HelpLeaflet:onUse(user)
    self:onInspect(user)
end
