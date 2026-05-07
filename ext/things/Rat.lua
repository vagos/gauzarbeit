function Rat:doInit()
    self:setStat("MaxHealth", 10)
    self:setStat("Attack", 1)
    self:gainItem("Cheese")
end

function Rat:onAttack(attacker)
    print(attacker:getName())
end

function Rat:onThink()
  if (Gauzarbeit.WithChance(0.01)) then
      self:doSay(self:getRoom(), "Squeak\n")
   end

    local directions = {
        Gauzarbeit.Direction.North,
        Gauzarbeit.Direction.South,
        Gauzarbeit.Direction.East,
        Gauzarbeit.Direction.West
    }

    self:doMove(directions[math.random(#directions)])
end
