function TestTalker:doInit()
    self:addTask("TestTask")
end

function TestTalker:onTalk(event)
    self:giveTask(event.actor, "TestTask")
    event.actor:sendMessage("TestTalker heard you.\n")
end

function TestTalker:onInspect(inspector)
    return "A test NPC that offers TestTask.\n"
end
