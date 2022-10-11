#include <editor.h>
#include <widgets.h>

namespace Editor::WorldTree {
    std::unordered_map<worldTreeID_t, std::shared_ptr<Object>> obj_to_treeId;
    std::unordered_map<std::shared_ptr<Object>, worldTreeID_t> treeId_to_obj;
    
    
    worldTreeID_t Add() {
        
    }
    
    void Remove(worldTreeID_t id) {
        
    }
    
    void Rename (worldTreeID_t id, std::string new_name) {
        
    }
    
    void Rebuild() {
        world_tree->DeleteAllItems();
        auto root = world_tree->AddRoot(wxString("Pasaule"));
        
        for (auto cell : worldcells) {
            
        }
    }
}