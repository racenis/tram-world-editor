#ifndef EDITOR_ACTIONS_H
#define EDITOR_ACTIONS_H

#include <editor/editor.h>

// TODO: yeet
#include <editor/objects/entity.h>

namespace Editor {

class ActionChangeSelection : public Action {
public:
    ActionChangeSelection (std::shared_ptr<Selection> new_selection) {
        prev_selection = SELECTION;
        next_selection = new_selection;
        
        //std::cout << "Arrived selection: ";
        //for (auto& obj : new_selection->objects) std::cout << obj->GetName() << " ";
        //std::cout << std::endl;
        
        Perform();
    }
    
    void Perform() {
        
        //std::cout << "CHANGED selection: ";
        //for (auto& obj : next_selection->objects) std::cout << obj->GetName() << " ";
        //std::cout << std::endl;
        
        SELECTION = next_selection;
        
        //std::cout << "finished chaning! now updating!" << std::endl;
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
        
        //std::cout << "okay done!" << std::endl;
    }
    
    void Unperform() {
        SELECTION = prev_selection;
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    std::shared_ptr<Selection> prev_selection = nullptr;
    std::shared_ptr<Selection> next_selection = nullptr;
};

class ActionNew : public Action {
public:
    ActionNew() {
        // we will allow creating a new object if and only if a single object is selected.
        // otherwise it would be weird. usually you don't do something like that.
        if (SELECTION->objects.size() == 1) {
            parent_object = SELECTION->objects.front();
            bool parent_hidden = parent_object->IsHidden();
            child_object = parent_object->AddChild();
            child_object->SetHidden(parent_hidden);
            if (parent_object->IsChildrenTreeable()) Editor::WorldTree::Add(child_object.get());
            selection = std::make_shared<Editor::Selection>();
            selection->objects.push_back(child_object);
            std::swap(selection, Editor::SELECTION);
            Editor::PropertyPanel::Refresh();
            Editor::ObjectList::Refresh();
            Editor::Viewport::Refresh();
        } else {
            Editor::Viewport::ShowErrorDialog("Adding new objects can only if single object is selected!\n\nDo not DO NOT press undo, or you will crash the program!!!SAVE AND RESTART!!!!\n\n//TODO: fix");
        }
    }
    
    void Perform() {
        parent_object->AddChild(child_object);
        child_object->SetHidden(child_was_hidden);
        std::swap(selection, Editor::SELECTION);
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
        if (parent_object->IsChildrenTreeable()) Editor::WorldTree::Add(child_object.get());
    }
    
    void Unperform() {
        child_was_hidden = child_object->IsHidden();
        parent_object->RemoveChild(child_object);
        child_object->SetHidden(true);
        std::swap(selection, Editor::SELECTION);
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
        if (parent_object->IsChildrenTreeable()) Editor::WorldTree::Remove(child_object.get());
    }
    
    bool child_was_hidden = false;
    std::shared_ptr<Object> parent_object = nullptr;
    std::shared_ptr<Object> child_object = nullptr;
    std::shared_ptr<Editor::Selection> selection;
};

class ActionRemove : public Action {
public:
    ActionRemove () {
        for (auto& object : SELECTION->objects) {
            // TODO: add a check for if the object can even be removed
            removal_list.push_back({object->parent->GetPointer(), object, object->IsHidden()});
        }
        
        selection = std::make_shared<Editor::Selection>();
        
        Perform();
    }
    
    void Perform() {
        for (auto& objects : removal_list) {
            objects.first->RemoveChild(objects.second);
            objects.second->SetHidden(true);
            if (objects.first->IsChildrenTreeable()) Editor::WorldTree::Remove(objects.second.get());
        }
        
        std::swap(Editor::SELECTION, selection);
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
        
        Editor::data_modified = true;
    }
    
    void Unperform() {
        for (auto& objects : removal_list) {
            objects.first->AddChild(objects.second);
            objects.second->SetHidden(objects.was_hidden);
            if (objects.first->IsChildrenTreeable()) Editor::WorldTree::Add(objects.second.get());
        }
        
        std::swap(Editor::SELECTION, selection);
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    struct Removal {
        std::shared_ptr<Object> first;
        std::shared_ptr<Object> second;
        bool was_hidden;
    };
    
    std::list<Removal> removal_list;
    std::shared_ptr<Editor::Selection> selection;
};

class ActionDuplicate : public Action {
public:
    ActionDuplicate() {
        if (SELECTION->objects.size() == 0 ) {
            std::cout << "Need a lest a single objec tto dupe!" << std::endl;
            failed = true;
            return;
        }
        
        auto dupables = SELECTION->objects;
        SELECTION->objects.clear();
        
        for (auto& object : dupables) {
            auto dupe = object->Duplicate();
            
            dupe->SetHidden(object->IsHidden());
            auto parent = dupe->GetParent();
            
            if (parent->IsChildrenTreeable()) {
                 Editor::WorldTree::Add(dupe.get());
            }
            
            SELECTION->objects.push_back(dupe);
            
            duped_objects.push_back({parent, dupe});
            
            /*duped_object = SELECTION->objects.front()->Duplicate();
            parent_object = duped_object->GetParent();
            duped_object->SetHidden(SELECTION->objects.front()->IsHidden());
            if (parent_object->IsChildrenTreeable()) Editor::WorldTree::Add(duped_object.get());
            
            SELECTION->objects.clear();
            SELECTION->objects.push_back(duped_object);
            
            Editor::PropertyPanel::Refresh();
            Editor::Viewport::Refresh();*/
        }// else {
         //   std::cout << "Adding new objects can only if single object is selected!" << std::endl;
        //    failed = true;
        //}
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
    }
    
    void Perform() {
        if (failed) return;
        
        SELECTION->objects.clear();
        
        for (auto& [parent, dupe, was_hidden] : duped_objects) {
            parent->AddChild(dupe);
            dupe->SetHidden(was_hidden);
            
            if (parent->IsChildrenTreeable()) {
                Editor::WorldTree::Add(dupe.get()); 
            }
            
            SELECTION->objects.push_back(dupe);
        }
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
        
        /*
        parent_object->AddChild(duped_object);
        duped_object->SetHidden(was_hidden);
        
        SELECTION->objects.clear();
        SELECTION->objects.push_back(duped_object);
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
        if (parent_object->IsChildrenTreeable()) Editor::WorldTree::Add(duped_object.get());*/
    }
    
    void Unperform() {
        if (failed) return;
        
        SELECTION->objects.clear();
        
        for (auto& [parent, dupe, was_hidden] : duped_objects) {
            was_hidden = dupe->IsHidden();
            parent->RemoveChild(dupe);
            dupe->SetHidden(true);
            
            if (parent->IsChildrenTreeable()) {
                Editor::WorldTree::Remove(dupe.get());
            }
            
            SELECTION->objects.push_back(parent);
        }
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
        
        /*was_hidden = duped_object->IsHidden();
        parent_object->RemoveChild(duped_object);
        duped_object->SetHidden(true);
        
        SELECTION->objects.clear();
        SELECTION->objects.push_back(parent_object);
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
        if (parent_object->IsChildrenTreeable()) Editor::WorldTree::Remove(duped_object.get());*/
    }
    
    bool failed = false;
    
    //bool was_hidden = false;
    //std::shared_ptr<Object> parent_object = nullptr;
    //std::shared_ptr<Object> duped_object = nullptr;
    
    struct ParentDupePair {
        std::shared_ptr<Object> parent;
        std::shared_ptr<Object> dupe;
        bool was_hidden;
    };
    
    std::list<ParentDupePair> duped_objects;
};

/*class ActionChangeProperties : public Action {
public:
    // Makes a backup of the properties of the selected objects.
    ActionChangeProperties() {
        for (auto& object : selection->objects) {
            property_backups.push_back({object, object->GetProperties()});
        }
    }
    
    // Swaps the backed-up properties with the new properties.
    void Perform () {
        for (auto& backup : property_backups) {
            auto new_property = backup.first->GetProperties();
            backup.first->SetProperties(backup.second);
            backup.second = new_property;
        }
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    // Swaps the new properties with the backed-up properties.
    void Unperform() {
        Perform();
    }
    
    std::list<std::pair<std::shared_ptr<Object>, std::unordered_map<std::string, PropertyValue>>> property_backups;
};*/

class ActionChangeProperties : public Action {
public:
    // This will back up the values of the properties of the selection.
    ActionChangeProperties(std::vector<std::string> properties) {
        for (auto& object : SELECTION->objects) {
            for (auto& property : properties) {
                property_backups.push_back({object, {property, object->GetProperty(property)}});
            }
        }
        Editor::data_modified = true;
    }
    
    // Swaps the backed-up properties with the new properties.
    void Perform () {
        for (auto& backup : property_backups) {
            auto new_property = backup.first->GetProperty(backup.second.property_name);
            backup.first->SetProperty(backup.second.property_name, backup.second.property_value);
            backup.second.property_value = new_property;
        }
        
        Editor::data_modified = true;
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    // Swaps the new properties with the backed-up properties.
    void Unperform() {
        Perform();
    }
    
    struct PropertyBackup {
        std::string property_name;
        PropertyValue property_value;
    };
    
    std::list<std::pair<std::shared_ptr<Object>, PropertyBackup>> property_backups;
};

class ActionCenterOrigin : public Action {
public:
    // This will back up the values of the properties of the selection.
    ActionCenterOrigin() {
        for (auto& object : SELECTION->objects) {
            origin_backups.push_back({object, object->GetProperty("origin")});
            
            auto entity = std::dynamic_pointer_cast<Entity>(object);
            if (entity) {
                entity->CenterOrigin();
            }
        }
        Editor::data_modified = true;
    }
    
    // Swaps the backed-up properties with the new properties.
    void Perform () {
        for (auto& backup : origin_backups) {
            auto new_property = backup.first->GetProperty("origin");
            backup.first->SetProperty("origin", backup.second);
            backup.second = new_property;
        }
        
        Editor::data_modified = true;
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    // Swaps the new properties with the backed-up properties.
    void Unperform() {
        Perform();
    }
    
    std::list<std::pair<std::shared_ptr<Object>, PropertyValue>> origin_backups;
};

/*class ActionSwapSelection : public Action {
public:
    ActionSwapSelection (std::shared_ptr<Selection> new_selection) {
        prev_selection = selection;
        next_selection = new_selection;
    }
    
    // TODO: you also have to
    // - update the property panel about the new selection
    // - update the object list about the new selection
    // - reparent all children of the selected objects
    // - reparent the object to its children
    void Perform() {
        for (auto& object : prev_selection->objects) {
            auto parent_object = object->GetParent();
            parent_object->RemoveChild(object);
            
            if (parent_object->IsChildrenTreeable()) {
                Editor::WorldTree::Remove(object.get());
            }
        }

        for (auto& object : next_selection->objects) {
            auto parent_object = object->GetParent();
            parent_object->AddChild(object);
            
            if (parent_object->IsChildrenTreeable()) {
                Editor::WorldTree::Add(object.get());
            }
        }
        
        selection = next_selection;
    }
    
    void Unperform() {
        for (auto& object : next_selection->objects) {
            auto parent_object = object->GetParent();
            parent_object->RemoveChild(object);
            
            if (parent_object->IsChildrenTreeable()) {
                Editor::WorldTree::Remove(object.get());
            }
        }

        for (auto& object : prev_selection->objects) {
            auto parent_object = object->GetParent();
            parent_object->AddChild(object);
            
            if (parent_object->IsChildrenTreeable()) {
                Editor::WorldTree::Add(object.get());
            }
        }
        
        selection = prev_selection;
    }
    
    std::shared_ptr<Selection> prev_selection = nullptr;
    std::shared_ptr<Selection> next_selection = nullptr;
};*/

}

#endif // EDITOR_ACTIONS_H