function Kitchen:doInit()
    Gauzarbeit.Spawn(self, "Wick_McWelcome")
    Gauzarbeit.Spawn(self, "Chair")
    Gauzarbeit.Spawn(self, "Cheese")

    for i=1,5 do
        Gauzarbeit.Spawn(self, "Rat")
    end
end
