TestChest.doInit = function () {
  this.opened = false;
};

TestChest.onUse = function (user) {
  if (this.opened) {
    user.sendMessage("TestChest is already open.\n");
    return;
  }

  this.opened = true;
  user.sendMessage("You open the TestChest.\n");
  user.gainItem("TestToken");
};

TestChest.onInspect = function (inspector) {
  return "A simple chest used for tests.\n";
};
