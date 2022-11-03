# Tramway SDK Level Editor

## To-do list
- reimplement loading/saving
- redo viewport


### Small features
- Reload levels without restarting editor
- Cut/Copy/Paste
- Different languages
- Disable undo/redo buttons when these actions can't be performed
- Add a settings menu
	- Change the length of the undo/redo chain
	- Change the key bindings
- A menu that shows undo/redo history
	- Need to add some metadata to undo/redo actions
- Localization
- Viewport transforms better when multiple axis
- Record the mouse pointer position when capturing it for the viewport and restore it when releasing it

### Optimizations
- Undo/redo makes a copy of all of the properties of the object before they are modified
	- It's possible to save some memory by copying only those properties that will actually be modified
- Rewrite the viewport movement code
- Add std::move() to where it would be useful

### Bugs
- Undo/redo does not change the name of the object in the tree menu
	- Will be fixed when an optimization is implemented
	
### Other features
- 2D viewport
	- Locks the camera angle facing towards the Z axis
	- Different viewport movement
- Grid rendering in viewport
- Viewport transform snapping
	- Snapping to grid for translations
	- Snapping to 15/30/45 degrees for rotations
	- Will need a special menu to configure the snap ammount
- Viewport transforms in different coordinate spaces
	- World space
	- Entity group space
	- Entity space
- Different rotation measurements
	- Radians
	- Degrees
- Objects selectable through the viewport
