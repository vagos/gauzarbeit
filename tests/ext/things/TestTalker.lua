function TestTalker:doInit()
    self:addTask("TestTask")
end

function TestTalker:onTalk(talker)
    self:giveTask(talker, "TestTask")
    talker:sendMessage("TestTalker heard you.\n")
end

function TestTalker:onInspect(inspector)
    return "A test NPC that offers TestTask.\n"
end
