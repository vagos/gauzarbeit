function Gun:doInit()
    self.ammo = 10
end


function Gun:onUse(event)
    event.actor:sendMessage("You shot a gun!\n")
    self.ammo = self.ammo - 1
    event.actor:sendMessage("Current ammo: "..self.ammo.."\n")
end

function Gun:doDatabaseSave()
    return tostring(self.ammo);
end

function Gun:doDatabaseLoad()
    self.ammo = tonumber(Gauzarbeit.GetDBLine());
end
