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
            properties["index"] = PropertyValue::UInt(parent->GetChildren().size());
            properties["position-x"] = Viewport::CURSOR_X;
            properties["position-y"] = Viewport::CURSOR_Y;
            properties["position-z"] = Viewport::CURSOR_Z;
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
        
        std::vector<WidgetDefinition> GetWidgetDefinitions();
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"index",       "group-path-node",      PROPERTY_UINT},
                {"position-x",  "group-path-node-pos",  PROPERTY_FLOAT},
                {"position-y",  "group-path-node-pos",  PROPERTY_FLOAT},
                {"position-z",  "group-path-node-pos",  PROPERTY_FLOAT},
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
    
    bool IsWidgetedWithChildren() { return true; }
    std::vector<WidgetDefinition> GetWidgetDefinitions();
    
    void LoadFromDisk();
    void SaveToDisk();
    
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
    
    void ReindexChildren() {
        uint32_t index = 0;
        for (auto& child : children) {
            child->SetProperty("index", PropertyValue::UInt(index++));
        }
    }
    
    void AddChild(std::shared_ptr<Object> child) { children.push_back(child); ReindexChildren(); for (auto& edge : edges) { if (edge.a == child.get() || edge.b == child.get()) edge.dormant = false;} }
    void RemoveChild(std::shared_ptr<Object> child) { children.remove(child); ReindexChildren(); for (auto& edge : edges) { if (edge.a == child.get() || edge.b == child.get()) edge.dormant = true;} }
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_PATH_H