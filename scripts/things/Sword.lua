function Sword:doInit()

    self.attack_bonus = 1


end

function Sword:onUse(user)

    user:setAttack(user:getAttack() + self.attack_bonus)

    user:doEquip(self)
    
end
