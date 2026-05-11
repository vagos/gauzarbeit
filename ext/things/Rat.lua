function Rat:doInit()
    self:setStat("MaxHealth", 10)
    self:setStat("Attack", 1)
    self:gainItem("Cheese")
end

function Rat:onAttack(attacker)
    print(attacker:getName())
end

function Rat:onThink()
  if (Gauzarbeit.WithChance(0.05)) then
      self:doSay(self:getRoom(), Gauzarbeit.ColorString("Squeak\n", Gauzarbeit.Color.White, Gauzarbeit.Style.Faint))
   end
end
