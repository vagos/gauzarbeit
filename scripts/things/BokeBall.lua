function BokeBall:doInit()

    self.thingCaught = nil
    
    function self:sendThing(user)
        user:sendMessage("Inside the BokeBall: "..self.thingCaught:getName().."\n")
    end
end



function BokeBall:onInspect(inspector)
    
    self:sendThing(inspector)
    
end

function BokeBall:onUse(user)
    
    -- use BokeBall <thing>
    
    if not self.thingCaught then
        self.thingCaught = user:getThing( user:getEventInfo().object )
    else
        user:sendMessage("Your BokeBall is full!\n")
    end

    self:sendThing(user)
end
