

function Volunteer:doInit()
    self:setMaxHealth(2)
    self.team = "IEEE"

    self:gainItem("Hat")
    self:gainItem("Winston")
end

function Volunteer:onAttack(attacker)
    attacker:sendMessage("Volunteer: Ouch!\n")
end

function Volunteer:onInspect(inspector)
    return "A nice looking "..self.team.." Volunteer\n"
end
