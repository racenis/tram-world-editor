#ifndef TRAM_SDK_EDITOR_OBJECTS_NAVMESH_H
#define TRAM_SDK_EDITOR_OBJECTS_NAVMESH_H

#include <editor/editor.h>

namespace Editor {

class Navmesh : public Object {
public:
    class Node : public Object {
    public:
        Node (Object* parent) : Object(parent) {
            properties["id"] = GetNewID();
            properties["next-id"] = 0;
            properties["prev-id"] = 0;
            properties["left-id"] = 0;
            properties["right-id"] = 0;
            properties["position-x"] = 0.0f;
            properties["position-y"] = 0.0f;
            properties["position-z"] = 0.0f;
        }
        
        std::string_view GetName() { return "navmesh-node"; }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return false; }
        bool IsAddable() { return false; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"id",          "group-navmesh-node",           PROPERTY_UINT},
                {"next-id",     "group-navmesh-node",           PROPERTY_UINT},
                {"prev-id",     "group-navmesh-node",           PROPERTY_UINT},
                {"left-id",     "group-navmesh-node",           PROPERTY_UINT},
                {"right-id",    "group-navmesh-node",           PROPERTY_UINT},
                {"position-x",  "group-navmesh-node-position",  PROPERTY_FLOAT},
                {"position-y",  "group-navmesh-node-position",  PROPERTY_FLOAT},
                {"position-z",  "group-navmesh-node-position",  PROPERTY_FLOAT}
            };
        }
    };
    
    Navmesh (Object* parent) : Navmesh(parent, "new-navmesh") {}
    Navmesh (Object* parent, std::string name) : Object(parent) {
        properties["name"] = name;
    }
    
    bool IsChildrenTreeable() { return false; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return true; }
    bool IsRemovable() { return true; }
    bool IsEditable() { return true; }
    bool IsCopyable() { return true; }
    
    void LoadFromDisk();
    void SaveToDisk();
    
    static uint32_t GetNewID();
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"position-x", "", PROPERTY_FLOAT},
            {"position-y", "", PROPERTY_FLOAT},
            {"position-z", "", PROPERTY_FLOAT}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "group-path", PROPERTY_STRING}
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Node>(this); children.push_back(child); return child; }
};


}

#endif // TRAM_SDK_EDITOR_OBJECTS_NAVMESH_H