#include <editor/objects/path.h>

#include <framework/file.h>

namespace Editor {

using namespace tram;

static uint64_t path_id_counter = 0;

uint64_t Path::GetNewID() {
    return ++path_id_counter;
}

void Path::LoadFromDisk() {
    std::string path = "data/paths/";
    path += this->GetName();
    path += ".path";
    
    File file (path.c_str(), MODE_READ /*| MODE_PAUSE_LINE*/);
    
    if (!file.is_open()) {
        std::cout << "Can't find path file: " << path << std::endl; return;
    }
    
    if (file.read_name() != "PATHv2") {
        Viewport::ShowErrorDialog(std::string("Unrecognized path format in ") + path + "!"); return;
    }
    
    //file.skip_linebreak();
    
    std::vector<Node*> nodes;
    
    while (file.is_continue()) {
        name_t record_type = file.read_name();
        
        if (record_type == "node") {
            auto new_node = std::make_shared<Node>(this);
            
            new_node->SetProperty("id", nodes.size());
        
            new_node->SetProperty("position-x", file.read_float32());
            new_node->SetProperty("position-y", file.read_float32());
            new_node->SetProperty("position-z", file.read_float32());
            
            nodes.push_back(new_node.get());
            children.push_back(new_node);
            
        } else if (record_type == "edge") {
            const uint32_t from_node_index = file.read_uint32();
            const uint32_t to_node_index = file.read_uint32();
            
            Node* from_node = nodes[from_node_index];
            Node* to_node = nodes[to_node_index];
            
            Edge edge = {.from = from_node, .to = to_node};
            
            from_node->outgoing.push_back(edge);
            
        } else {
            std::cout << "unknown path record: " << record_type << std::endl;
        }
        
        //file.skip_linebreak();
    }
}

void Path::SaveToDisk() {
    std::string path = "data/paths/";
    path += this->GetName();
    path += ".path";
    
    File file (path.c_str(), MODE_WRITE);
    
    if (!file.is_open()) {
        std::cout << "Can't open path file for writing: " << path << std::endl; return;
    }
    
    file.write_name("PATHv2");
    
    file.write_newline();
    file.write_newline();
    
    for (auto& child : children) {
        file.write_name("node");
        
        file.write_float32(child->GetProperty("position-x"));
        file.write_float32(child->GetProperty("position-y"));
        file.write_float32(child->GetProperty("position-z"));
        
        file.write_newline();
    }
    
    file.write_newline();
    
    for (auto& child : children) {
        for (auto& edge : dynamic_cast<Node*>(child.get())->GetOutgoing()) {
            file.write_name("edge");
            
            file.write_uint32(edge.from->GetProperty("id"));
            file.write_uint32(edge.to->GetProperty("id"));
            
            file.write_newline();
        }
    }
}

}