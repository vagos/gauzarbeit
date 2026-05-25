function EvilChair:doInit()

    self:setStat("MaxHealth", 20)
    self:setStat("dmg", 10)

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
