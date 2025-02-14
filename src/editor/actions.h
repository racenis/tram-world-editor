#ifndef EDITOR_ACTIONS_H
#define EDITOR_ACTIONS_H

#include <editor/editor.h>

// TODO: yeet
#include <editor/objects/entity.h>

namespace Editor {

class ActionChangeSelection : public Action {
public:
    ActionChangeSelection(std::shared_ptr<Selection> new_selection) {
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

class ActionStashSelection : public Action {
public:
    ActionStashSelection() {
        Perform();
    }
    
    void Perform() {
        prev_stash = STASH;
        STASH = SELECTION;
        SELECTION = std::make_shared<Editor::Selection>();
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    void Unperform() {
        SELECTION = STASH;
        STASH = prev_stash;
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    std::shared_ptr<Selection> prev_stash = nullptr;
};

class ActionReparent : public Action {
public:
    ActionReparent() {
        if (STASH->objects.size() == 0) {
            Editor::Viewport::ShowErrorDialog("Reparenting can occur only if an object or objects are stashed!");
            failed = true;
            return;
        }
        
        if (SELECTION->objects.size() != 1) {
            Editor::Viewport::ShowErrorDialog("Reparenting can occur only if a single object is selected!");
            failed = true;
            return;
        }
        
        new_parent = SELECTION->objects.front();
        
        for (auto& object : STASH->objects) {
            reparent_info.push_back({.prev_parent = object->GetParent(), .object = object});
            
            if (!new_parent->IsChildAddable(object)) {
                Object* obj_parent = new_parent.get();
                Object* obj_child = object.get();
                
                Editor::Viewport::ShowErrorDialog(std::string("Cannot parent a ") + typeid(*obj_child).name() + " to a " + typeid(*obj_parent).name() + "!");
                
                failed = true;
                return;
            }
        }
        
        Perform();
    }
    
    void Perform() {
        if (failed) return;
        
        for (auto& reparent : reparent_info) {
            if (reparent.prev_parent->IsChildrenTreeable()) Editor::WorldTree::Remove(reparent.object.get());
            reparent.prev_parent->RemoveChild(reparent.object);
            
            reparent.object->SetHidden(new_parent->IsHidden());
            
            new_parent->AddChild(reparent.object);
            if (new_parent->IsChildrenTreeable()) Editor::WorldTree::Add(reparent.object.get());
        }
        
        Editor::SELECTION = std::make_shared<Editor::Selection>();
        
        std::swap(Editor::SELECTION, Editor::STASH);
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    void Unperform() {
        if (failed) return;
        
        for (auto& reparent : reparent_info) {
            if (new_parent->IsChildrenTreeable()) Editor::WorldTree::Remove(reparent.object.get());
            new_parent->RemoveChild(reparent.object);

            reparent.object->SetHidden(reparent.prev_parent->IsHidden());

            reparent.prev_parent->AddChild(reparent.object);
            if (reparent.prev_parent->IsChildrenTreeable()) Editor::WorldTree::Remove(reparent.object.get());
        }
        
        std::swap(Editor::SELECTION, Editor::STASH);
        
        Editor::SELECTION->objects.push_back(new_parent);
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
    }
    
    bool failed = false;
    
    struct ReparentInfo {
        std::shared_ptr<Object> prev_parent = nullptr;
        std::shared_ptr<Object> object = nullptr;
    };
    
    std::list<ReparentInfo> reparent_info;
    
    std::shared_ptr<Object> new_parent = nullptr;
    
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
            
            child_object = parent_object->AddChild();
            
            // AddChild() can fail and in that case it will return a nullptr, so
            // we have to handle that
            if (child_object) {
                
                // set parent visibility properties
                child_object->SetHidden(parent_object->IsHidden());
                if (parent_object->IsChildrenTreeable()) Editor::WorldTree::Add(child_object.get());
                
                // create a new selection containing our new object and swap it
                selection = std::make_shared<Editor::Selection>();
                selection->objects.push_back(child_object);
                
                std::swap(selection, Editor::SELECTION);
                
                Editor::PropertyPanel::Refresh();
                Editor::ObjectList::Refresh();
                Editor::Viewport::Refresh();
            }
        } else {
            Editor::Viewport::ShowErrorDialog("Adding new objects can only if single object is selected!\n\nDo not DO NOT press undo, or you will crash the program!!!SAVE AND RESTART!!!!\n\n//TODO: fix");
        }
    }
    
    void Perform() {
        if (!child_object) return;
        
        parent_object->AddChild(child_object);
        child_object->SetHidden(child_was_hidden);
        
        std::swap(selection, Editor::SELECTION);
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
        if (parent_object->IsChildrenTreeable()) Editor::WorldTree::Add(child_object.get());
    }
    
    void Unperform() {
        if (!child_object) return;
        
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

class ActionWorldspawnOffset : public Action {
public:
    // This will back up the values of the properties of the selection.
    ActionWorldspawnOffset() {
        if (STASH->objects.size() == 0) {
            Editor::Viewport::ShowErrorDialog("No stashed objects to worldspawn offset to!");
            return;
        }
        
        if (SELECTION->objects.size() != 1) {
            Editor::Viewport::ShowErrorDialog("Need to select a single worldspawn object!");
            return;
        }
        
        Entity* worldspawn = dynamic_cast<Entity*>(SELECTION->objects.front().get());
        
        if (!worldspawn) {
            Editor::Viewport::ShowErrorDialog("Selected worldspawn is not an entity!");
            return;
        }
        
        for (auto& object : STASH->objects) {
            origin_backups.push_back({object, {object->GetProperty("position-x"),
                                               object->GetProperty("position-y"),
                                               object->GetProperty("position-z"),
                                               
                                               object->GetProperty("rotation-x"),
                                               object->GetProperty("rotation-y"),
                                               object->GetProperty("rotation-z")}});
                                               
            auto entity = std::dynamic_pointer_cast<Entity>(object);
            if (entity) {
                entity->WorldspawnOffset(worldspawn);
            } else {
                std::cout << "Couldn't set worldspawn offset, object not entity!" << std::endl;
            }
        }
        
        Editor::PropertyPanel::Refresh();
        Editor::ObjectList::Refresh();
        Editor::Viewport::Refresh();
        
        Editor::data_modified = true;
    }
    
    // Swaps the backed-up properties with the new properties.
    void Perform () {
        for (auto& backup : origin_backups) {
            Transform new_transform = {backup.first->GetProperty("position-x"),
                                       backup.first->GetProperty("position-y"),
                                       backup.first->GetProperty("position-z"),
                                       
                                       backup.first->GetProperty("rotation-x"),
                                       backup.first->GetProperty("rotation-y"),
                                       backup.first->GetProperty("rotation-z")};
            
            backup.first->SetProperty("position-x", backup.second.pos_x);
            backup.first->SetProperty("position-y", backup.second.pos_y);
            backup.first->SetProperty("position-z", backup.second.pos_z);
            
            backup.first->SetProperty("rotation-x", backup.second.rot_x);
            backup.first->SetProperty("rotation-y", backup.second.rot_y);
            backup.first->SetProperty("rotation-z", backup.second.rot_z);
            
            backup.second = new_transform;
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
    
    struct Transform {
        float pos_x, pos_y, pos_z;
        float rot_x, rot_y, rot_z;
    };
    
    std::list<std::pair<std::shared_ptr<Object>, Transform>> origin_backups;
};

class ActionDuplicate : public Action {
public:
    ActionDuplicate() {
        selection = std::make_shared<Editor::Selection>();
        
        for (auto& object : SELECTION->objects) {
            auto dupe = object->Duplicate();

            if (!dupe) continue;
            
            dupe->SetHidden(object->IsHidden());
            auto parent = dupe->GetParent();
            
            if (parent->IsChildrenTreeable()) {
                 Editor::WorldTree::Add(dupe.get());
            }
            
            selection->objects.push_back(dupe);
            
            duped_objects.push_back({parent, dupe});
        }
        
        std::swap(selection, Editor::SELECTION);
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
    }
    
    void Perform() {
        for (auto& [parent, dupe, was_hidden] : duped_objects) {
            parent->AddChild(dupe);
            dupe->SetHidden(was_hidden);
            
            if (parent->IsChildrenTreeable()) {
                Editor::WorldTree::Add(dupe.get()); 
            }
        }
        
        std::swap(selection, Editor::SELECTION);
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
    }
    
    void Unperform() {
        for (auto& [parent, dupe, was_hidden] : duped_objects) {
            was_hidden = dupe->IsHidden();
            parent->RemoveChild(dupe);
            dupe->SetHidden(true);
            
            if (parent->IsChildrenTreeable()) {
                Editor::WorldTree::Remove(dupe.get());
            }
        }
        
        std::swap(selection, Editor::SELECTION);
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
    }
    
    struct ParentDupePair {
        std::shared_ptr<Object> parent;
        std::shared_ptr<Object> dupe;
        bool was_hidden;
    };
    
    std::shared_ptr<Editor::Selection> selection;
    std::list<ParentDupePair> duped_objects;
};

class ActionExtrude : public Action {
public:
    ActionExtrude() {
        selection = std::make_shared<Editor::Selection>();
        
        for (auto& object : SELECTION->objects) {
            auto dupe = object->Extrude();
            
            if (!dupe) continue;
            
            dupe->SetHidden(object->IsHidden());
            auto parent = dupe->GetParent();
            
            if (parent->IsChildrenTreeable()) {
                 Editor::WorldTree::Add(dupe.get());
            }
            
            duped_objects.push_back({parent, dupe});
        }
        
        std::swap(selection, Editor::SELECTION);
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
    }
    
    void Perform() {
        for (auto& [parent, dupe, was_hidden] : duped_objects) {
            parent->AddChild(dupe);
            dupe->SetHidden(was_hidden);
            
            if (parent->IsChildrenTreeable()) {
                Editor::WorldTree::Add(dupe.get()); 
            }
        }
        
        std::swap(selection, Editor::SELECTION);
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
    }
    
    void Unperform() {
        for (auto& [parent, dupe, was_hidden] : duped_objects) {
            was_hidden = dupe->IsHidden();
            parent->RemoveChild(dupe);
            dupe->SetHidden(true);
            
            if (parent->IsChildrenTreeable()) {
                Editor::WorldTree::Remove(dupe.get());
            }
        }
        
        std::swap(selection, Editor::SELECTION);
        
        Editor::PropertyPanel::Refresh();
        Editor::Viewport::Refresh();
    }
    
    struct ParentDupePair {
        std::shared_ptr<Object> parent;
        std::shared_ptr<Object> dupe;
        bool was_hidden;
    };
    
    std::shared_ptr<Editor::Selection> selection;
    std::list<ParentDupePair> duped_objects;
};

class ActionConnect : public Action {
public:

    ActionConnect() {
        if (SELECTION->objects.size() != 2) {
            std::cout << "we can  only connect 2 objects" << std::endl;
            failed = true;
            return;
        }
        
        object_a = *SELECTION->objects.begin();
        object_b = *++SELECTION->objects.begin();
        
        std::cout << "object_a "<< object_a.get() << " object b " << object_b.get() << std::endl;
        
        connect = !object_a->IsConnected(object_b);
        
        if (connect) {
            object_a->Connect(object_b);
        } else {
            object_a->Disconnect(object_b);
        }
        
        failed = false;
        Editor::data_modified = true;
        
        Editor::Viewport::Refresh();
    }
    
    void Perform () {
        if (failed) return;
        
        if (connect) {
            object_a->Connect(object_b);
        } else {
            object_a->Disconnect(object_b);
        }
        
        Editor::Viewport::Refresh();
    }
    
    void Unperform() {
        if (failed) return;
        
        if (connect) {
            object_a->Disconnect(object_b);
        } else {
            object_a->Connect(object_b);
        }
        
        Editor::Viewport::Refresh();
    }
    
    std::shared_ptr<Object> object_a;
    std::shared_ptr<Object> object_b;
    
    bool failed = false;
    bool connect = false;
};

}

#endif // EDITOR_ACTIONS_H