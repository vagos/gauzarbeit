TestTalker.doInit = function () {
  this.gainQuest("TestQuest");
};

TestTalker.onTalk = function (talker) {
  talker.sendMessage("TestTalker heard you.\n");
};

TestTalker.onInspect = function (inspector) {
  return "A test NPC that offers TestQuest.\n";
};
