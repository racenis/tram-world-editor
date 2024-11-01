#include <editor/objects/navmesh.h>

#include <framework/file.h>

namespace Editor {

using namespace tram;

static uint64_t navmesh_id_counter = 0;

uint64_t Navmesh::GetNewID() {
    return ++navmesh_id_counter;
}

void Navmesh::LoadFromDisk() {
    std::string path = "data/navmeshes/";
    path += this->GetName();
    path += ".navmesh";
    
    File file (path.c_str(), MODE_READ);
    
    if (!file.is_open()) {
        std::cout << "Can't find path file: " << path << std::endl; return;
    }
    
    if (file.read_name() != "NAVMESHv1") {
        std::cout << "Unrecognized navmesh format in " << path << std::endl; return;
    }
    
    file.read_name(); // skip navmesh name
    
    while (file.is_open()) {
        uint64_t node_id = file.read_uint32();
        
        if (node_id > navmesh_id_counter) {
            navmesh_id_counter = node_id;
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

void Navmesh::SaveToDisk() {
    std::string path = "data/navmeshes/";
    path += this->GetName();
    path += ".navmesh";
    
    File file (path.c_str(), MODE_WRITE);
    
    if (!file.is_open()) {
        std::cout << "Can't open path file for writing: " << path << std::endl; return;
    }
    
    file.write_name("NAVMESHv1");
    
    file.write_name(std::string(this->GetName()).c_str());
    
    /// TODO: add newline writes
    
    for (auto& child : children) {
        file.write_uint32(child->GetProperty("id"));
        
        file.write_uint32(child->GetProperty("next-id"));
        file.write_uint32(child->GetProperty("prev-id"));
        file.write_uint32(child->GetProperty("left-id"));
        file.write_uint32(child->GetProperty("right-id"));
        
        file.write_float32(child->GetProperty("position-x"));
        file.write_float32(child->GetProperty("position-y"));
        file.write_float32(child->GetProperty("position-z"));
    }
}

}