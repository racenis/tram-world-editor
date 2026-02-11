#ifndef TRAM_SDK_EDITOR_OBJECTS_LIGHT_GRAPH_H
#define TRAM_SDK_EDITOR_OBJECTS_LIGHT_GRAPH_H

#include <editor/editor.h>

namespace Editor {

class LightGraph : public Object {
public:
    class Node;

    struct Edge {
        Node* a;
        Node* b;
        bool dormant;
    };
    
    class Node : public Object {
    public:
        Node (Object* parent) : Object(parent) {
            properties["index"] = PropertyValue::UInt(parent->GetChildren().size());
            properties["position-x"] = Viewport::CURSOR_X;
            properties["position-y"] = Viewport::CURSOR_Y;
            properties["position-z"] = Viewport::CURSOR_Z;
            properties["probe-light"] = true;
            properties["probe-reflection"] = true;
        }
        
        std::string_view GetName() { return "light_node"; }
        
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
                {"group-light-graph-node", "Path Node", "", PROPERTY_CATEGORY},
                {"index", "Index", "group-light-graph-node", PROPERTY_UINT},
                {"probe-light", "Probe Light", "group-light-graph-node", PROPERTY_BOOL},
                {"probe-reflection", "Probe Reflection", "group-light-graph-node", PROPERTY_BOOL},
                {"group-light-graph-node-pos", "Position", "group-light-graph-node", PROPERTY_CATEGORY},
                {"position-x", "X", "group-light-graph-node-pos", PROPERTY_FLOAT},
                {"position-y", "Y", "group-light-graph-node-pos", PROPERTY_FLOAT},
                {"position-z", "Z", "group-light-graph-node-pos", PROPERTY_FLOAT},
            };
        }
    };
    
    LightGraph(Object* parent) : LightGraph (parent, "Light Graph") {}
    LightGraph(Object* parent, std::string name) : Object(parent) {
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
            {"position-x", "x", "", PROPERTY_FLOAT},
            {"position-y", "y", "", PROPERTY_FLOAT},
            {"position-z", "z", "", PROPERTY_FLOAT}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-light-graph", "light_graph", "", PROPERTY_CATEGORY}
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

#endif // TRAM_SDK_EDITOR_OBJECTS_LIGHT_GRAPH_H