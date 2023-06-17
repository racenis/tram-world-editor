#include <editor/editor.h>
#include <editor/language.h>
#include <editor/actions.h>

#include <editor/objects/world.h>

#include <widgets/mainframe.h>
#include <widgets/objectmenu.h>
#include <widgets/worldtree.h>

WorldTreeCtrl* world_tree = nullptr;

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
        auto new_node = world_tree->AppendItem(node_to_parent_to, Editor::PropertyRename(std::string(object->GetName())));
        
        obj_to_treeId[object] = new_node.GetID();
        treeId_to_obj[new_node.GetID()] = object;
        
        // check if children can be added to tree
        if (!object->IsChildrenTreeable()) {
            return;
        }
        
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
        if (!obj_to_treeId[object]) return;
        world_tree->SetItemText(obj_to_treeId[object], Editor::PropertyRename(std::string(object->GetName())));
    }
    
    void Rebuild () {
        world_tree->DeleteAllItems();
        root_node = world_tree->AddRoot(Editor::PropertyRename(std::string(WORLD->GetName())));
        obj_to_treeId.clear();
        treeId_to_obj.clear();
        
        obj_to_treeId[WORLD.get()] = root_node.GetID();
        treeId_to_obj[root_node.GetID()] = WORLD.get();
        
        for (auto cell : WORLD->children) {
            Add (cell.get());
        }
        
        world_tree->Refresh();
    }
    
    std::shared_ptr<Object> GetObject(void* Id) {
        return treeId_to_obj[Id]->GetPointer();
    }
}

WorldTreeCtrl::WorldTreeCtrl (wxWindow* parent) : wxTreeCtrl(parent, -1, wxDefaultPosition, wxSize(200, 150), wxTR_DEFAULT_STYLE | wxTR_MULTIPLE) {
    Bind(wxEVT_TREE_ITEM_MENU, &WorldTreeCtrl::OnMenuOpen, this);
    Bind(wxEVT_TREE_SEL_CHANGED, &WorldTreeCtrl::OnSelectionChanged, this);
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &WorldTreeCtrl::OnItemActivated, this);
}

void WorldTreeCtrl::OnMenuOpen (wxTreeEvent& event) {
    world_tree_popup->SetSelectionStatus(Editor::SELECTION.get());
    main_frame->PopupMenu(world_tree_popup);
}

void WorldTreeCtrl::OnSelectionChanged (wxTreeEvent& event) {
    wxArrayTreeItemIds selected_ids;
    size_t selected_count = GetSelections(selected_ids);
    
    auto new_selection = std::make_shared<Editor::Selection>();
    for (size_t i = 0; i < selected_count; i++) {
        auto ob = Editor::WorldTree::GetObject(selected_ids[i].GetID());
        new_selection->objects.push_back(ob);
    }
    
    Editor::PerformAction<Editor::ActionChangeSelection>(new_selection);
}

// this method gets called when you double-click an item
// idk what to do with it..
void WorldTreeCtrl::OnItemActivated (wxTreeEvent& event) {
    //Editor::PropertyPanel::SetCurrentSelection();
    //Editor::ObjectList::SetCurrentSelection();
}