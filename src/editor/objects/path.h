#ifndef TRAM_SDK_EDITOR_OBJECTS_PATH_H
#define TRAM_SDK_EDITOR_OBJECTS_PATH_H

#include <editor/editor.h>

namespace Editor {

class Path : public Object {
public:
    class Node;
    
    struct Edge {
        Node* from;
        Node* to;
    };
    
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
        
        std::string_view GetName() { return "Path Node"; }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return false; }
        bool IsAddable() { return false; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        
        std::list<Edge> GetOutgoing() { return outgoing; }
        
        std::list<Edge> outgoing;
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-path-node", "Path Node", "", PROPERTY_CATEGORY},
                {"id", "ID", "group-path-node", PROPERTY_UINT},
                {"group-path-node-pos", "Position", "group-path-node", PROPERTY_CATEGORY},
                {"position-x", "X", "group-path-node-pos", PROPERTY_FLOAT},
                {"position-y", "Y", "group-path-node-pos", PROPERTY_FLOAT},
                {"position-z", "Z", "group-path-node-pos", PROPERTY_FLOAT},
            };
        }
    };
    
    Path (Object* parent) : Path (parent, "New Path") {}
    Path (Object* parent, std::string name) : Object(parent) {
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
    
    static uint64_t GetNewID();
    
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