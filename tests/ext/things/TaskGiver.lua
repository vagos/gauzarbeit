function TaskGiver:doInit()
    self:addTask("Collect herbs")
end

function TaskGiver:onTalk(talker)
    self:giveTask(talker, "Collect herbs")
end
