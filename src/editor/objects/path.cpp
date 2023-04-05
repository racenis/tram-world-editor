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
    
    File file (path.c_str(), MODE_READ);
    
    if (!file.is_open()) {
        std::cout << "Can't find path file: " << path << std::endl; return;
    }
    
    if (file.read_name() != "PATHv1") {
        std::cout << "Unrecognized path format in " << path << std::endl; return;
    }
    
    file.read_name(); // skip path name
    
    while (file.is_open()) {
        uint64_t node_id = file.read_uint32();
        
        if (node_id > path_id_counter) {
            path_id_counter = node_id;
        }
        
        auto new_node = std::make_shared<Node>(this);
        
        new_node->SetProperty("id", node_id);
        
        new_node->SetProperty("next-id", file.read_uint64());
        new_node->SetProperty("prev-id", file.read_uint64());
        new_node->SetProperty("left-id", file.read_uint64());
        new_node->SetProperty("right-id", file.read_uint64());
        
        new_node->SetProperty("position-x", file.read_float32());
        new_node->SetProperty("position-y", file.read_float32());
        new_node->SetProperty("position-z", file.read_float32());

        children.push_back(new_node);
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
    
    file.write_name("PATHv1");
    
    file.write_name(std::string(this->GetName()).c_str());
    
    /// TODO: add newline writes
    
    for (auto& child : children) {
        file.write_uint64(child->GetProperty("id"));
        
        file.write_uint64(child->GetProperty("next-id"));
        file.write_uint64(child->GetProperty("prev-id"));
        file.write_uint64(child->GetProperty("left-id"));
        file.write_uint64(child->GetProperty("right-id"));
        
        file.write_float32(child->GetProperty("position-x"));
        file.write_float32(child->GetProperty("position-y"));
        file.write_float32(child->GetProperty("position-z"));
    }
}

}