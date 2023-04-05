#ifndef TRAM_SDK_EDITOR_OBJECTS_PATH_H
#define TRAM_SDK_EDITOR_OBJECTS_PATH_H

#include <editor/editor.h>

namespace Editor {

class Path : public Object {
public:
    class Node : public Object {
    public:
        Node (Object* parent) : Object(parent) {
            properties["id"] = 0;
            properties["next-id"] = 0;
            properties["prev-id"] = 0;
            properties["left-id"] = 0;
            properties["right-id"] = 0;
            properties["position-x"] = 0.0f;
            properties["position-y"] = 0.0f;
            properties["position-z"] = 0.0f;
            properties["position-x-2"] = 0.0f;
            properties["position-y-2"] = 0.0f;
            properties["position-z-2"] = 0.0f;
            properties["position-x-3"] = 0.0f;
            properties["position-y-3"] = 0.0f;
            properties["position-z-3"] = 0.0f;
            properties["position-x-4"] = 0.0f;
            properties["position-y-4"] = 0.0f;
            properties["position-z-4"] = 0.0f;
        }
        
        std::string_view GetName() { return "Path Node"; }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return false; }
        bool IsAddable() { return false; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-path-node", "Path Node", "", PROPERTY_CATEGORY},
                {"id", "ID", "group-path-node", PROPERTY_UINT},
                {"next-id", "Next ID", "group-path-node", PROPERTY_UINT},
                {"prev-id", "Prev ID", "group-path-node", PROPERTY_UINT},
                {"left-id", "Left ID", "group-path-node", PROPERTY_UINT},
                {"right-id", "Right ID", "group-path-node", PROPERTY_UINT},
                {"group-path-node-pos-1", "Position 1", "group-path-node", PROPERTY_CATEGORY},
                {"position-x", "X", "group-path-node-pos-1", PROPERTY_FLOAT},
                {"position-y", "Y", "group-path-node-pos-1", PROPERTY_FLOAT},
                {"position-z", "Z", "group-path-node-pos-1", PROPERTY_FLOAT},
                {"group-path-node-pos-2", "Position 2", "group-path-node", PROPERTY_CATEGORY},
                {"position-x-2", "X", "group-path-node-pos-2", PROPERTY_FLOAT},
                {"position-y-2", "Y", "group-path-node-pos-2", PROPERTY_FLOAT},
                {"position-z-2", "Z", "group-path-node-pos-2", PROPERTY_FLOAT},
                {"group-path-node-pos-3", "Position 3", "group-path-node", PROPERTY_CATEGORY},
                {"position-x-3", "X", "group-path-node-pos-3", PROPERTY_FLOAT},
                {"position-y-3", "Y", "group-path-node-pos-3", PROPERTY_FLOAT},
                {"position-z-3", "Z", "group-path-node-pos-3", PROPERTY_FLOAT},
                {"group-path-node-pos-4", "Position 4", "group-path-node", PROPERTY_CATEGORY},
                {"position-x-4", "X", "group-path-node-pos-4", PROPERTY_FLOAT},
                {"position-y-4", "Y", "group-path-node-pos-4", PROPERTY_FLOAT},
                {"position-z-4", "Z", "group-path-node-pos-4", PROPERTY_FLOAT}
            };
        }
    };
    
    Path (Object* parent) : Object(parent) {
        properties["name"] = std::string("New Path");
    }
    
    bool IsChildrenTreeable() { return false; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return true; }
    bool IsRemovable() { return true; }
    bool IsEditable() { return true; }
    bool IsCopyable() { return true; }
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"position-x", "X", "", PROPERTY_FLOAT},
            {"position-y", "Y", "", PROPERTY_FLOAT},
            {"position-z", "Z", "", PROPERTY_FLOAT}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-path", "Path", "", PROPERTY_CATEGORY},
            {"name", "Name", "group-path", PROPERTY_STRING}
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Node>(this); children.push_back(child); return child; }
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_PATH_H