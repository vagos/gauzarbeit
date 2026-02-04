function TestChest:doInit()
    self.opened = false
end

function TestChest:onUse(user)
    if self.opened then
        user:sendMessage("TestChest is already open.\n")
        return
    end

    self.opened = true
    user:sendMessage("You open the TestChest.\n")
    user:gainItem("TestToken")
end

function TestChest:onInspect(inspector)
    return "A simple chest used for tests.\n"
end
