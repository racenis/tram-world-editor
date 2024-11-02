#ifndef TRAM_SDK_EDITOR_OBJECTS_TRANSITION_H
#define TRAM_SDK_EDITOR_OBJECTS_TRANSITION_H

#include <editor/editor.h>

namespace Editor {

class Transition : public Object {
public:
    class Node : public Object {
    public:
        Node (Object* parent) : Object(parent) {
            properties["position-x"] = 0.0f;
            properties["position-y"] = 0.0f;
            properties["position-z"] = 0.0f;
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
        
        float SelectSize() { return 5.0f; }
        
        void Draw();
        
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
    
    void Draw();
    
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