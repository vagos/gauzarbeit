function Ratz:doInit()
    self.ratTask = self:addTask()
end

function Ratz:onNotify(actor, notification_type, target)
    
    if notification_type == Gauzarbeit.Event.Catch then
        self:tickTask(self.ratTask)
    end

    print(actor:getName())

    if target:isValid() then
        print(target:getName()) 
    end
end

function Ratz:giveRewards(completer)
    completer:gainXP(1000)

    print(completer:getLevel())
end
