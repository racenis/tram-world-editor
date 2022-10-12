#ifndef ACTIONS_H
#define ACTIONS_H

#include <editor.h>

namespace Editor {    
    class ActionChangeSelection : public Action {
    public:
        ActionChangeSelection (std::shared_ptr<Selection> new_selection) {
            prev_selection = selection;
            next_selection = new_selection;
            
            Perform();
        }
        
        void Perform() {
            std::cout << "New selection: ";
            for (auto& obj : next_selection->objects) std::cout << obj->GetName() << " ";
            std::cout << std::endl;
            selection = next_selection;
        }
        
        void Unperform() {
            selection = prev_selection;
        }
        
        std::shared_ptr<Selection> prev_selection = nullptr;
        std::shared_ptr<Selection> next_selection = nullptr;
    };
    
    class ActionNew : public Action {
    public:
        ActionNew () {
            // we will allow creating a new object if and only if a single object is selected.
            // otherwise it would be weird. usually you don't do something like that.
            if (selection->objects.size() == 1) {
                parent_object = selection->objects.front();
                child_object = parent_object->AddChild();
            } else {
                // you could throw an exception here or something
                std::cout << "Adding new objects can only if single object is selected!" << std::endl;
            }
        }
        
        void Perform() {
            parent_object->AddChild(child_object);
        }
        
        void Unperform() {
            parent_object->RemoveChild(child_object);
        }
        
        std::shared_ptr<Object> parent_object = nullptr;
        std::shared_ptr<Object> child_object = nullptr;
    };
    
    
    
}

#endif // ACTIONS_H