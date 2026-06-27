function Chair:doInit() 
    
    self.sits = 0

end

function Chair:onUse(event)
    self.sits = self.sits + 1
    event.actor:sendMessage("You just sit on a chair!\n")
    event.actor:sendMessage("Times sitted: "..self.sits.."\n")
end

function Chair:onInspect(inspector)
   return "This is a chair!\n" 
end
