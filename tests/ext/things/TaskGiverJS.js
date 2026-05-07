TaskGiverJS.doInit = function () {
  this.addTask("Collect JS token");
};

TaskGiverJS.onTalk = function (talker) {
  talker.addTask("Collect JS token");
};
