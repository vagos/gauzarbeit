function Ratz:doInit()
    self.ratTask = self:addTask()
end

function Ratz:onNotify(actor, notification_type)
    
    if notification_type == Gauzarbeit.Event.Catch then
        self:tickTask(self.ratTask)
    end
end

function Ratz:giveRewards(completer)
    completer:gainXP(1000)
end
