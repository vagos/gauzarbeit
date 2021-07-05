```
   _____                           _          _ _   
  / ____|                         | |        (_) |  
 | |  __  __ _ _   _ ______ _ _ __| |__   ___ _| |_ 
 | | |_ |/ _` | | | |_  / _` | '__| '_ \ / _ \ | __|
 | |__| | (_| | |_| |/ / (_| | |  | |_) |  __/ | |_ 
  \_____|\__,_|\__,_/___\__,_|_|  |_.__/ \___|_|\__|
```


Description: A Terminal ASCII online game. 

### DOING
- [ ] Procedular Generation
- [.] GUIs
- [ ] Networking
- [ ] Combat


### TODO
- [ ] [Thing Class](Thing Class)
- [ ] [Enemies](Enemies)
- [ ] A way to slap someone
- [ ] Dialog
- [ ] Add Combat GUI

### REFACTOR

- [X] Fix how event strings work
- [X] Make ScriptedThing::GetItem return nil (return 0)
- [X] Maybe move all the talking stuff to the talker component.
- [ ] Rework enemy attack behavior
- [X] Rework event types
- [X] Change the way things are passed in "Inform" events
- [X] ( Think of adding exceptions instead of error messages. )
- [ ] Fix the ball
- [ ] Fix the handling of dead things in Rooms
- [ ] The Server/Communication part of the code needs a rework.
- [X] Add onTalk / onGreet functions to Talker
- [ ] Maybe gainQuest/gainItem should have an owner argument

### RANDOM IDEAS
* Have each wall have something on it when you interact with it.

### FEATURE IDEAS 

- [ ] Card Game
- [o] Rock Paper Scissors
- [ ] Music Making
- [ ] Farming
- [ ] Potion Making
- [ ] Logic Circuit Making
- [ ] Cooking
- [ ] Player Housing
- [ ] Pets

### BUGS

* [Problems During Development](Problems During Development)


### COMMANDS

TELL - Use "TELL {NAME}" to tell someone something privately. 
       Use "TELL EVERYONE" so that everyone in the room hears you.
