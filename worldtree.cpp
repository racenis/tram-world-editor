#include <editor.h>
#include <actions.h>
#include <widgets.h>
#include <objectmenu.h>
#include <worldtree.h>

namespace Editor::WorldTree {
    std::unordered_map<worldTreeID_t, Object*> treeId_to_obj;
    std::unordered_map<Object*, worldTreeID_t> obj_to_treeId;
    wxTreeItemId root_node;
    
    void Add (Object* object) {
        assert(world_tree);
        assert(object->parent);
        
        // check if object hasn't been already added
        if (obj_to_treeId[object]) return;
        
        // check if parent isn't in the tree
        if (!obj_to_treeId[object->parent]) {
            Add(object->parent);
            return;
        }
        
        // if parent is in tree and object isn't in the tree
        auto node_to_parent_to =  wxTreeItemId(obj_to_treeId[object->parent]);
        auto new_node = world_tree->AppendItem(node_to_parent_to, std::string(object->GetName()));
        
        obj_to_treeId[object] = new_node.GetID();
        treeId_to_obj[new_node.GetID()] = object;
        
        // recursively add all of the object's children too
        auto childrens = object->GetChildren();
        if (childrens.size()) {
            for (auto child : childrens) Add(child.get());
        }
    }
    
    void Remove (Object* object) {
        // if object isn't in the tree, do nothing
        if (!obj_to_treeId[object]) return;
        
        // first remove all of the children
        auto childrens = object->GetChildren();
        if (childrens.size()) {
            for (auto child : childrens) Remove(child.get());
        }
        
        // erase object from the tree
        auto tree_node = obj_to_treeId[object];
        world_tree->Delete(tree_node);
        
        obj_to_treeId[object] = nullptr;
        treeId_to_obj[tree_node] = nullptr;
    }
    
    void Rename (Object* object) {
        assert(obj_to_treeId[object]);
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
        
        for (auto cell : worldcells->children) {
            //AddChildren(root_node, cell.get());
            Add(cell.get());
        }
        
        world_tree->Refresh();
    }
    
    std::shared_ptr<Object> GetObject(void* Id) {
        return treeId_to_obj[Id]->GetPointer();
    }
}

WorldTree::WorldTree (wxWindow* parent) : wxTreeCtrl(parent, -1, wxDefaultPosition, wxSize(200, 150), wxTR_DEFAULT_STYLE | wxTR_MULTIPLE) {
    Bind(wxEVT_TREE_ITEM_MENU, &WorldTree::OnMenuOpen, this);
    Bind(wxEVT_TREE_SEL_CHANGED, &WorldTree::OnSelectionChanged, this);
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &WorldTree::OnItemActivated, this);
}

void WorldTree::OnMenuOpen (wxTreeEvent& event) {
    std::cout << "Opened tree menu!" << std::endl;
    world_tree_popup->SetSelectionStatus(Editor::selection.get());
    main_frame->PopupMenu(world_tree_popup);
}

void WorldTree::OnSelectionChanged (wxTreeEvent& event) {
    std::cout << "Selection changed!" << std::endl;
    wxArrayTreeItemIds selected_ids;
    size_t selected_count = GetSelections(selected_ids);
    
    auto new_selection = std::make_shared<Editor::Selection>();
    for (size_t i = 0; i < selected_count; i++) {
        auto ob = Editor::WorldTree::GetObject(selected_ids[i].GetID());
        new_selection->objects.push_back(ob);
    }
    
    Editor::PerformAction<Editor::ActionChangeSelection>(new_selection);
}

void WorldTree::OnItemActivated (wxTreeEvent& event) {
    Editor::PropertyPanel::SetCurrentSelection();
    Editor::ObjectList::SetCurrentSelection();
}