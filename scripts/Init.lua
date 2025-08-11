
-- Adding Helper function on the Lua side.

Gauzarbeit.Stats = {}

function Gauzarbeit.CreateStat(s_name, s_func)
    Gauzarbeit.Stats[s_name] = s_func;
end

-- (Testing)

Gauzarbeit.CreateStat("Attack", function (val) return tostring(val) end)
Gauzarbeit.CreateStat("Defense", function (val) return tostring(val) end)
Gauzarbeit.CreateStat("MaxHealth", function (val) return tostring(val) end)
Gauzarbeit.CreateStat("CurrentHealth", function (val) return tostring(val) end)

-- World Init Stuff

Gauzarbeit.Spawn(0, 0, "Chair")
Gauzarbeit.Spawn(0, 0, "Gun")

Gauzarbeit.Spawn(0, 0, "WelcomeMan")
Gauzarbeit.Spawn(0, 0, "WantedBadge")

Gauzarbeit.GetRoom("Kitchen", 0, 0)

-- Server configurations

LoginInfo = "Use \"login {name} {pswrd}\" to enter the world!\n"
MOTD = "\n\n\tWelcome to Gauzarbeit!\n\n"..LoginInfo

print("Lua initialised!")
