if (typeof Gauzarbeit === "undefined") {
  Gauzarbeit = {};
}

Gauzarbeit.Stats = {};
Gauzarbeit.CreateStat = function (name, fn) {
  Gauzarbeit.Stats[name] = fn;
};
Gauzarbeit.CreateStat("Attack", function (val) { return String(val); });
Gauzarbeit.CreateStat("Defense", function (val) { return String(val); });
Gauzarbeit.CreateStat("MaxHealth", function (val) { return String(val); });
Gauzarbeit.CreateStat("CurrentHealth", function (val) { return String(val); });

// Add a test room with scripted test entities.
Gauzarbeit.GetRoom("TestLab", 0, 1);

print("JavaScript initialised!");
