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

### Optimizations
- Undo/redo makes a copy of all of the properties of the object before they are modified
	- It's possible to save some memory by copying only those properties that will actually be modified
- 

### Bugs
- Undo/redo does not change the name of the object in the tree menu
	- Will be fixed when an optimization is implemented