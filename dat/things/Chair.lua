function Chair:doInit() 
    
    self.sits = 0

end

function Chair:onUse(user)
    self.sits = self.sits + 1
    user:sendMessage("You just sit on a chair!\n")
    user:sendMessage("Times sitted: "..self.sits.."\n")
end

function Chair:onInspect(inspector)
   return "This is a chair!\n" 
end
