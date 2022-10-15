#include <editor.h>
#include <widgets.h>

namespace Editor::WorldTree {
    std::unordered_map<worldTreeID_t, Object*> treeId_to_obj;
    std::unordered_map<Object*, worldTreeID_t> obj_to_treeId;
    wxTreeItemId root_node;
    
    // TODO: I think that we should actually recursively add all of the object's children as well
    void Add (Object* object) {
        assert(world_tree);
        assert(object->parent);
        
        auto node_to_parent_to =  wxTreeItemId(obj_to_treeId[object->parent]);
        auto new_node = world_tree->AppendItem(node_to_parent_to, std::string(object->GetName()));
        
        obj_to_treeId[object] = new_node.GetID();
        treeId_to_obj[new_node.GetID()] = object;
    }
    
    void Remove (Object* object) {
        world_tree->Delete(obj_to_treeId[object]);
    }
    
    void Rename (Object* object) {
        world_tree->SetItemText(obj_to_treeId[object], std::string(object->GetName()));
    }
    
    void AddChildren (wxTreeItemId parent_tree_node, Object* object) {
        auto tree_node = world_tree->AppendItem(parent_tree_node, std::string(object->GetName()));
        
        obj_to_treeId[object] = tree_node.GetID();
        treeId_to_obj[tree_node.GetID()] = object;
        
        auto childrens = object->GetChildren();
        if (childrens.size()) {
            for (auto child : childrens) AddChildren(tree_node, child.get());
        }
    }
    
    void Rebuild() {
        world_tree->DeleteAllItems();
        root_node = world_tree->AddRoot(std::string(worldcells->GetName()));
        
        obj_to_treeId[worldcells.get()] = root_node.GetID();
        treeId_to_obj[root_node.GetID()] = worldcells.get();
        
        for (auto cell : worldcells->cells) {
            AddChildren(root_node, cell.get());
        }
    }
    
    std::shared_ptr<Object> GetObject(void* Id) {
        return treeId_to_obj[Id]->GetPointer();
    }
}