function Goblin:doInit()
    self:setMaxHealth(5)
end

function Goblin:doAttack(target)

    self:doSay(target, "I remove head!\n")

end

function Goblin:onAttack(attacker)
    self:doSay(attacker, "Ouch! Me remove your spleen.\n")
end

function Goblin:onInspect(inspector)
    
    inspector:sendMessage("The Goblin seems uneasy that you are inspecting it.\n")

    return "A Golbin. Seems okay.\n"
end
