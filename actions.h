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
                if (parent_object->IsChildrenListable()) Editor::WorldTree::Add(child_object.get());
            } else {
                std::cout << "Adding new objects can only if single object is selected!" << std::endl;
            }
        }
        
        void Perform() {
            parent_object->AddChild(child_object);
            if (parent_object->IsChildrenListable()) Editor::WorldTree::Add(child_object.get());
        }
        
        void Unperform() {
            parent_object->RemoveChild(child_object);
            if (parent_object->IsChildrenListable()) Editor::WorldTree::Remove(child_object.get());
        }
        
        std::shared_ptr<Object> parent_object = nullptr;
        std::shared_ptr<Object> child_object = nullptr;
    };
    
    class ActionRemove : public Action {
    public:
        ActionRemove () {
            for (auto& object : selection->objects) {
                removal_list.push_back({object->parent->GetPointer(), object});
            }
            
            Perform();
        }
        
        void Perform() {
            for (auto& objects : removal_list) {
                objects.first->RemoveChild(objects.second);
                if (objects.first->IsChildrenListable()) Editor::WorldTree::Remove(objects.second.get());
            }
        }
        
        void Unperform() {
            for (auto& objects : removal_list) {
                objects.first->AddChild(objects.second);
                if (objects.first->IsChildrenListable()) Editor::WorldTree::Add(objects.second.get());
            }
        }
        
        std::list<std::pair<std::shared_ptr<Object>, std::shared_ptr<Object>>> removal_list;
    };
    
    
}

#endif // ACTIONS_H