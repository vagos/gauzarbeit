TestTalker.doInit = function () {
  this.addTask("TestTask");
};

TestTalker.onTalk = function (talker) {
  this.giveTask(talker, "TestTask");
  talker.sendMessage("TestTalker heard you.\n");
};

TestTalker.onInspect = function (inspector) {
  return "A test NPC that offers TestTask.\n";
};
