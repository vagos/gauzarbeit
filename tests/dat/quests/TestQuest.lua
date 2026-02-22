function TestQuest:doInit()
    self:addTask("Talk to TestTalker")
    self:addTask("Open TestChest")
end

function TestQuest:doReward(completer)
    completer:sendMessage("TestQuest complete!\n")
end
