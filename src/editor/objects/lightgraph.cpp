#include <editor/objects/lightgraph.h>

#include <framework/file.h>
#include <render/render.h>

namespace Editor {

using namespace tram;

void LightGraph::LoadFromDisk() {
    std::string path = "data/worldcells/";
    path += this->parent->GetName();
    path += ".light";
    
    File file (path.c_str(), File::READ /*| MODE_PAUSE_LINE*/);
    
    if (!file.is_open()) {
        std::cout << "Can't find path file: " << path << std::endl; return;
    }
    
    if (file.read_name() != "LIGHTGRAPHv1") {
        Viewport::ShowErrorDialog(std::string("Unrecognized light graph format in ") + path + "!"); return;
    }
    
    //file.skip_linebreak();
    
    std::vector<Node*> nodes;
    
    while (file.is_continue()) {
        name_t record_type = file.read_name();

        if (record_type == "node") {
            auto new_node = std::make_shared<Node>(this);
            
            new_node->SetProperty("index", nodes.size());
        
            new_node->SetProperty("position-x", file.read_float32());
            new_node->SetProperty("position-y", file.read_float32());
            new_node->SetProperty("position-z", file.read_float32());
            
            new_node->SetProperty("probe-light", (bool)file.read_uint32());
            new_node->SetProperty("probe-reflection", (bool)file.read_uint32());
            
            nodes.push_back(new_node.get());
            children.push_back(new_node);
            
        } else if (record_type == "edge") {
            const uint32_t from_node_index = file.read_uint32();
            const uint32_t to_node_index = file.read_uint32();
            
            Node* from_node = nodes[from_node_index];
            Node* to_node = nodes[to_node_index];
            
            if (from_node_index >= nodes.size()) {
                std::cout << "invalid from node index " << from_node_index << std::endl;
            }
            
            if (to_node_index >= nodes.size()) {
                std::cout << "invalid to node index " << to_node_index << std::endl;
            }
            
            Edge* existing = nullptr;
            for (auto& edge : edges) {
                const bool a_to_b = edge.a == from_node && edge.b == to_node;
                const bool b_to_a = edge.a == to_node && edge.b == from_node;
                if (a_to_b || b_to_a) {
                    existing = &edge;
                }
            }
            
            if (existing) {
                std::cout << "Error parsing " << path << ", edge" << from_node_index << " -> " << to_node_index << " duplicate." << std::endl;
            } else {
                Edge edge = {.a = from_node, .b = to_node, .dormant = false};
                edges.push_back(edge);
            }
        } else if (record_type == "light") {
            file.read_uint32();     // layer
            file.read_name();       // channel
            
            file.read_name();
            
            file.read_float32();    // coefficients
            
            file.read_float32();
            file.read_float32();
            file.read_float32();
            
            file.read_float32();
            file.read_float32();
            file.read_float32();
            file.read_float32();
            file.read_float32();
        } else if (record_type == "entity") {
            file.read_uint32();
        } else {
            std::cout << "unknown path record: " << record_type << std::endl;
        }
        
        //file.skip_linebreak();
    }
}

void LightGraph::SaveToDisk() {
    std::string path = "data/worldcells/";
    path += this->parent->GetName();
    path += ".light";
    
    File file (path.c_str(), File::WRITE);
    
    if (!file.is_open()) {
        std::cout << "Can't open path file for writing: " << path << std::endl; return;
    }
    
    ReindexChildren();
    
    file.write_name("LIGHTGRAPHv1");
    
    file.write_newline();
    file.write_newline();
    
    for (auto& child : children) {
        file.write_name("node");
        
        file.write_float32(child->GetProperty("position-x"));
        file.write_float32(child->GetProperty("position-y"));
        file.write_float32(child->GetProperty("position-z"));
        
        file.write_uint32((bool)child->GetProperty("probe-light"));
        file.write_uint32((bool)child->GetProperty("probe-reflection"));
        
        file.write_newline();
    }
    
    file.write_newline();
    
    for (auto& edge : edges) {
        file.write_name("edge");
        
        file.write_uint32(edge.a->GetProperty("index"));
        file.write_uint32(edge.b->GetProperty("index"));
        
        file.write_newline();
    
    }
}

void LightGraph::Draw() {
    for (auto& child : children) {
        vec3 position = {
            child->GetProperty("position-x"),
            child->GetProperty("position-y"),
            child->GetProperty("position-z")
        };
        
        Render::AddLineAABB({-0.1f, -0.1f, -0.1f}, {0.1f, 0.1f, 0.1f}, position, vec3(0.0f, 0.0f, 0.0f), Render::COLOR_GREEN);
    }
    
    for (auto& edge : edges) {
        if (edge.dormant) continue;
        vec3 node_a = {
            edge.a->GetProperty("position-x"),
            edge.a->GetProperty("position-y"),
            edge.a->GetProperty("position-z")
        };
        
        vec3 node_b = {
            edge.b->GetProperty("position-x"),
            edge.b->GetProperty("position-y"),
            edge.b->GetProperty("position-z")
        };
        
        Render::AddLine(node_a, node_b, Render::COLOR_WHITE);
    }
    
}

void LightGraph::Node::Draw() {
    parent->Draw();
    
    vec3 pos = {
        this->GetProperty("position-x"),
        this->GetProperty("position-y"),
        this->GetProperty("position-z")
    };
    
    Render::AddLineAABB({-0.2f, -0.2f, -0.2f}, {0.2f, 0.2f, 0.2f}, pos, vec3(0.0f, 0.0f, 0.0f), Render::COLOR_RED);    
}

std::shared_ptr<Object> LightGraph::Node::Extrude() {
    //auto new_node = std::make_shared<Node>(this);
    auto new_node = std::dynamic_pointer_cast<LightGraph::Node>(parent->AddChild());
    
    new_node->SetProperty("index", parent->GetChildren().size());

    new_node->SetProperty("position-x", this->GetProperty("position-x"));
    new_node->SetProperty("position-y", this->GetProperty("position-y"));
    new_node->SetProperty("position-z", this->GetProperty("position-z"));
    
    //parent->AddChild(new_node);

    Edge edge;
    
    edge.a = this;
    edge.b = new_node.get();
    edge.dormant = false;

    dynamic_cast<LightGraph*>(parent)->edges.push_back(edge);
    
    return new_node;
}

void LightGraph::Node::Connect(std::shared_ptr<Object> object) {
    Node* other = dynamic_cast<Node*>(object.get());
    
    Edge* existing = nullptr;
    for (auto& edge : dynamic_cast<LightGraph*>(parent)->edges) {
        const bool a_to_b = edge.a == this && edge.b == other;
        const bool b_to_a = edge.a == other && edge.b == this;
        if (a_to_b || b_to_a) {
            existing = &edge;
        }
    }
    
    if (existing) {
        existing->dormant = false;
    } else {
        Edge edge = {.a = this, .b = other, .dormant = false};
        dynamic_cast<LightGraph*>(parent)->edges.push_back(edge);
    }
}

void LightGraph::Node::Disconnect(std::shared_ptr<Object> object) {
    Node* other = dynamic_cast<Node*>(object.get());
    
    Edge* existing = nullptr;
    for (auto& edge : dynamic_cast<LightGraph*>(parent)->edges) {
        const bool a_to_b = edge.a == this && edge.b == other;
        const bool b_to_a = edge.a == other && edge.b == this;
        if (a_to_b || b_to_a) {
            existing = &edge;
        }
    }
    
    if (existing) {
        existing->dormant = true;
    } else {
        std::cout << "not connentexc" << std::endl;
    }
}

bool LightGraph::Node::IsConnected(std::shared_ptr<Object> object) {
    Node* other = dynamic_cast<Node*>(object.get());
    
    for (auto& edge : dynamic_cast<LightGraph*>(parent)->edges) {
        const bool a_to_b = edge.a == this && edge.b == other;
        const bool b_to_a = edge.a == other && edge.b == this;
        if (a_to_b || b_to_a) {
            if (!edge.dormant) return true;
        }
    }
    
    return false;
}

}