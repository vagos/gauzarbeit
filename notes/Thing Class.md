Components: 
* Renderable
* AI
* Destructible
* Usable
* Online
* Sound
* Placeable - Can be placed in the world. 
* Phsical
* Notifier

Should UI elements also be things? - NO! Make the UI as seperate from the rest of the engine as possible. 


The physical component will have a pointer/reference to the world. No other component needs to see the world, right?
