function Rat:doInit()
    self:setStat("MaxHealth" , 2)
    self:setStat("CurrentHealth" , self:getStat("MaxHealth"))
end
