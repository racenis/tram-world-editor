#ifndef TRAM_SDK_EDITOR_OBJECTS_TRANSITION_H
#define TRAM_SDK_EDITOR_OBJECTS_TRANSITION_H

#include <editor/editor.h>

namespace Editor {

class Transition : public Object {
public:
    class Node : public Object {
    public:
        Node (Object* parent) : Object(parent) {
            properties["position-x"] = Viewport::CURSOR_X;
            properties["position-y"] = Viewport::CURSOR_Y;
            properties["position-z"] = Viewport::CURSOR_Z;
        }
        
        std::string_view GetName() { return "transition_node"; }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return false; }
        bool IsAddable() { return false; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        void SetHidden(bool hidden) { this->is_hidden = hidden; }
        bool IsHidden() { return this->is_hidden; }
        
        std::shared_ptr<Object> Duplicate() {
            auto dupe = parent->AddChild();
            dupe->SetProperty("position-x", this->GetProperty("position-x"));
            dupe->SetProperty("position-y", this->GetProperty("position-y"));
            dupe->SetProperty("position-z", this->GetProperty("position-z"));
            return dupe;
        }
        
        std::shared_ptr<Object> Extrude() {
            return Duplicate();
        }
        
        std::vector<WidgetDefinition> GetWidgetDefinitions();
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-transition-node", "transition_node", "", PROPERTY_CATEGORY},
                {"position-x", "X", "group-transition-node", PROPERTY_FLOAT},
                {"position-y", "Y", "group-transition-node", PROPERTY_FLOAT},
                {"position-z", "Z", "group-transition-node", PROPERTY_FLOAT}
            };
        }
    };

    Transition(Object* parent) : Object(parent) {
        properties["name"] = std::string("new-transition");
        properties["cell-into"] = std::string("none");
    }
    
    bool IsChildrenTreeable() { return false; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return true; }
    bool IsRemovable() { return true; }
    bool IsEditable() { return true; }
    bool IsCopyable() { return true; }
    
    //bool IsHidden() { return is_hidden; }
    //void SetHidden(bool is_hidden) { this->is_hidden = is_hidden; }
    
    std::shared_ptr<Object> Duplicate() {
        auto new_transition = parent->AddChild();
        for (auto& child: children) {
            auto new_node = new_transition->AddChild();
            new_node->SetProperty("position-x", child->GetProperty("position-x"));
            new_node->SetProperty("position-y", child->GetProperty("position-y"));
            new_node->SetProperty("position-z", child->GetProperty("position-z"));
        }
        return new_transition;
    }
    
    bool IsWidgetedWithChildren() { return true; }
    std::vector<WidgetDefinition> GetWidgetDefinitions();
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"position-x", "x", "", PROPERTY_FLOAT},
            {"position-y", "y", "", PROPERTY_FLOAT},
            {"position-z", "z", "", PROPERTY_FLOAT}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-transition", "transition", "", PROPERTY_CATEGORY},
            {"name", "Name", "group-transition", PROPERTY_STRING},
            {"cell-into", "Into", "group-transition", PROPERTY_STRING},
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Node>(this); children.push_back(child); return child; }
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_TRANSITION_H