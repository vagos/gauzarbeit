function TestTalker:doInit()
    self:gainQuest("TestQuest")
end

function TestTalker:onTalk(talker)
    talker:sendMessage("TestTalker heard you.\n")
end

function TestTalker:onInspect(inspector)
    return "A test NPC that offers TestQuest.\n"
end
