function EvilChair:doInit()

    self:setMaxHealth(20)
    self.setAttack(10)

end

function EvilChair:doAttack(target)
    print("I attacked")
end


function EvilChair:onUse(user)

    user:sendMessage("You try to sit on the chair...\n")

    self:doAttack(user)

    user:sendMessage("The chair goes back to sitting still...\n")
    
end

function EvilChair:onInspect(inspector)
    return "You notice the chair moving slightly.\n"
end
