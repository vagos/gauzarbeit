TestDummy.doInit = function () {
  this.counter = 0;
  this.setStat("MaxHealth", 10);
  this.setStat("dmg", 2);
  this.addTask("Say hello");
};

TestDummy.onUse = function (user) {
  this.counter = this.counter + 1;
  user.sendMessage("TestDummy used " + this.counter + " times.\n");
  user.gainXP(1);
};

TestDummy.onInspect = function (inspector) {
  return "TestDummy is a simple test actor.\n";
};
