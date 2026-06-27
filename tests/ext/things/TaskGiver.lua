function TaskGiver:doInit()
    self:addTask("Collect herbs")
end

function TaskGiver:onTalk(event)
    self:giveTask(event.actor, "Collect herbs")
end
