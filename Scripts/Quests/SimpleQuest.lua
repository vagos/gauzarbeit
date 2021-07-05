function SimpleQuest:doInit()

    self.moveTask = self:addTask()
    
end

function SimpleQuest:onNotify(actor, notification_type)
   
    if (notification_type == Gauzarbeit.Event.Move) then
        self:tickTask(self.moveTask)
    end

end

function SimpleQuest:onInspect()
    return "You can move between rooms with the command MOVE!\n"
end
