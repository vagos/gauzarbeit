function Chair:doInit()
    self.sits = 99
    
end


function Chair:doUpdate()
    self.sits = self.sits + 10
    self:printRef()
    print(self.sits)
end

