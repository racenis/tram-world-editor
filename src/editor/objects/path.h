#ifndef TRAM_SDK_EDITOR_OBJECTS_PATH_H
#define TRAM_SDK_EDITOR_OBJECTS_PATH_H

#include <editor/editor.h>

namespace Editor {

class Path : public Object {
public:
    class Node;
    
    enum EdgeType {
        AB,
        BA,
        BI
    };
    
    struct Edge {
        Node* a;
        Node* b;
        EdgeType type;
        bool dormant;
    };
    
    class Node : public Object {
    public:
        Node (Object* parent) : Object(parent) {
            properties["id"] = PropertyValue::UInt(parent->GetChildren().size());
            properties["position-x"] = 0.0f;
            properties["position-y"] = 0.0f;
            properties["position-z"] = 0.0f;
        }
        
        std::string_view GetName() { return "path_node"; }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return false; }
        bool IsAddable() { return false; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        
        std::shared_ptr<Object> Extrude();
        void Connect(std::shared_ptr<Object> other);
        void Disconnect(std::shared_ptr<Object> other);
        bool IsConnected(std::shared_ptr<Object> other);
        
        float SelectSize() { return 5.0f; }
        
        void Draw();
        
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
    
    std::list<Edge> GetEdges() { return edges; }
    std::list<Edge> edges;
    
    void Draw();
    
    void LoadFromDisk();
    void SaveToDisk();
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"position-x", "x", "", PROPERTY_FLOAT},
            {"position-y", "y", "", PROPERTY_FLOAT},
            {"position-z", "z", "", PROPERTY_FLOAT}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-path", "path", "", PROPERTY_CATEGORY},
            {"name", "Name", "group-path", PROPERTY_STRING}
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Node>(this); children.push_back(child); return child; }
    
    void ReindexChildren() {
        uint32_t id = 0;
        for (auto& child : children) {
            child->SetProperty("id", PropertyValue::UInt(id++));
        }
    }
    
    void AddChild(std::shared_ptr<Object> child) { children.push_back(child); ReindexChildren(); }
    void RemoveChild(std::shared_ptr<Object> child) { children.remove(child); ReindexChildren(); }
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_PATH_H