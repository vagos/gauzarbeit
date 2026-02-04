function TestDummy:doInit()
    self.counter = 0
    self:setStat("MaxHealth", 10)
    self:setStat("dmg", 2)
    self:addTask("Say hello")
end

function TestDummy:onUse(user)
    self.counter = self.counter + 1
    user:sendMessage("TestDummy used " .. self.counter .. " times.\n")
    user:gainXP(1)
end

function TestDummy:onInspect(inspector)
    return "TestDummy is a simple test actor.\n"
end
