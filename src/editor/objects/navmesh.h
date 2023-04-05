#ifndef TRAM_SDK_EDITOR_OBJECTS_NAVMESH_H
#define TRAM_SDK_EDITOR_OBJECTS_NAVMESH_H

#include <editor/editor.h>

namespace Editor {

class Navmesh : public Object {
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
        }
        
        std::string_view GetName() { return "Navmesh Node"; }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return false; }
        bool IsAddable() { return false; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-navmesh-node", "Navmesh Node", "", PROPERTY_CATEGORY},
                {"id", "ID", "group-navmesh-node", PROPERTY_UINT},
                {"next-id", "Next ID", "group-navmesh-node", PROPERTY_UINT},
                {"prev-id", "Prev ID", "group-navmesh-node", PROPERTY_UINT},
                {"left-id", "Left ID", "group-navmesh-node", PROPERTY_UINT},
                {"right-id", "Right ID", "group-navmesh-node", PROPERTY_UINT},
                {"group-navmesh-node-position", "Position", "group-navmesh-node", PROPERTY_CATEGORY},
                {"position-x", "X", "group-navmesh-node-position", PROPERTY_FLOAT},
                {"position-y", "Y", "group-navmesh-node-position", PROPERTY_FLOAT},
                {"position-z", "Z", "group-navmesh-node-position", PROPERTY_FLOAT}
            };
        }
    };
    
    Navmesh (Object* parent) : Object(parent) {
        properties["name"] = std::string("New Navmesh");
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
            {"group-path", "Navmesh", "", PROPERTY_CATEGORY},
            {"name", "Name", "group-path", PROPERTY_STRING}
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Node>(this); children.push_back(child); return child; }
};


}

#endif // TRAM_SDK_EDITOR_OBJECTS_NAVMESH_H