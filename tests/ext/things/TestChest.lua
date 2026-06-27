function TestChest:doInit()
    self.opened = false
end

function TestChest:onUse(event)
    if self.opened then
        event.actor:sendMessage("TestChest is already open.\n")
        return
    end

    self.opened = true
    event.actor:sendMessage("You open the TestChest.\n")
    event.actor:gainItem("TestToken")
end

function TestChest:onInspect(inspector)
    return "A simple chest used for tests.\n"
end
