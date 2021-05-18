function BokeBall:doInit()

    self.thingCaught = nil
    
    function self:sendThing(user)
        user:sendMessage("Inside the BokeBall: "..self.thingCaught:getName().."\n")
    end
end



function BokeBall:onInspect(user)
    
    user:sendMessage("Inside the BokeBall: "..self.thingCaught:getName().."\n")
    
end

function BokeBall:onUse(user)
    
    -- get use target 
    
    if not self.thingCaught then
        self.thingCaught = user:getThing()
    else
        user:sendMessage("Your BokeBall is full!\n")
    end

    self:sendThing(user)

end


