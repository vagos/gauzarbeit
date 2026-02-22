function Rat:doInit()
    self:setStat("MaxHealth", 10)
    self:setStat("Attack", 1)
    self:gainItem("Cheese")
end

function Rat:onAttack(attacker)
    print("I'm a rat, ouch!")
    print(attacker:getName())
end
