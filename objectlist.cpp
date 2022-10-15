#include <editor.h>
#include <actions.h>
#include <widgets.h>

using namespace Editor;
std::vector<PropertyDefinition> columns;
std::shared_ptr<Object> selected_object;

namespace Editor::ObjectList {
    void SetCurrentSelection() {
        entity_list->DeleteAllColumns();
        columns.clear();
        
        if (selection->objects.size() == 1) {
            selected_object = selection->objects.front();
            columns = selected_object->GetListPropertyDefinitions();
            
            for (size_t i = 0; i < columns.size(); i++) {
                entity_list->InsertColumn(i, columns[i].display_name);
            }
            
            entity_list->SetItemCount(selected_object->GetChildren().size());
        } else {
            entity_list->SetItemCount(0);
        }
    }
}

    wxString EntityList::OnGetItemText (long item, long column) const {
        auto childrens = selected_object->GetChildren();
        auto first_childrens = childrens.begin();
        std::advance(first_childrens, item);
        auto& info = columns[column];
        auto& object = *first_childrens;
        
        switch (info.type) {
            case PROPERTY_STRING: 
                char* str;
                if (object->GetProperty(info.name, &str)) return wxString(str);
                break;
            case PROPERTY_FLOAT:
                float floot;
                if (object->GetProperty(info.name, &floot)) return wxString(std::to_string(floot));
                break;
            case PROPERTY_INT:
                int64_t integ;
                if (object->GetProperty(info.name, &integ)) return wxString(std::to_string(integ));
                break;
            case PROPERTY_UINT:
                uint64_t uinteg;
                if (object->GetProperty(info.name, &uinteg)) return wxString(std::to_string(uinteg));
                break;
            default:
                break;
            }
        
        return wxString("nil");
    }
    
    void EntityList::OnMenuOpen(wxListEvent& event) {
        std::cout << "TODO: open menu" << std::endl;
        main_frame->PopupMenu(world_tree_popup);
    }
    
    void EntityList::OnSelectionChanged(wxListEvent& event) {
        std::cout << "Selection changed!" << std::endl;
        
        auto new_selection = std::make_shared<Editor::Selection>();
        auto childrens = selected_object->GetChildren();
        
        for (long item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); item != -1; item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)){
            auto chi_benig = childrens.begin();
            std::advance(chi_benig, item);
            new_selection->objects.push_back(*chi_benig);
        }
        
        Editor::PerformAction<Editor::ActionChangeSelection>(new_selection);
    }
    
    void EntityList::OnItemActivated(wxListEvent& event) {
        Editor::PropertyPanel::SetCurrentSelection();
        Editor::ObjectList::SetCurrentSelection();
    }
    
    
    void EntityList::RefreshAllItems () {
        DeleteAllColumns();
        /*if (selection.front().indirection_type == Editor::Selector::CELL_ITSELF ||
            selection.front().indirection_type == Editor::Selector::CELL_ENTITIES) {
            InsertColumn(0, L"ID");
            InsertColumn(1, L"Nosaukums");
            InsertColumn(2, L"Lokācija");
            InsertColumn(3, L"Rotācija");
            InsertColumn(4, L"Darbība");
            SetItemCount(selection.front().into->entities.size());
        } else if (selection.front().indirection_type == Editor::Selector::TRANSITION) {
            InsertColumn(0, L"X");
            InsertColumn(1, L"Y");
            InsertColumn(2, L"Z");
            SetItemCount(selection.front().trans->points.size());
        } else if (selection.front().indirection_type == Editor::Selector::TRANSITION_POINT) {
            InsertColumn(0, L"X");
            InsertColumn(1, L"Y");
            InsertColumn(2, L"Z");
            SetItemCount(selection.front().point->parent->points.size());
        } else if (selection.front().indirection_type == Editor::Selector::ENTITY_GROUP) {
            InsertColumn(0, L"ID");
            InsertColumn(1, L"Nosaukums");
            InsertColumn(2, L"Lokācija");
            InsertColumn(3, L"Rotācija");
            InsertColumn(4, L"Darbība");
            SetItemCount(selection.front().group->entities.size());
        } else if (selection.front().indirection_type == Editor::Selector::PATH) {
            InsertColumn(0, L"ID");
            InsertColumn(1, L"⇧ ");
            InsertColumn(2, L"⇩");
            InsertColumn(3, L"⇦");
            InsertColumn(4, L"⇨");
            SetItemCount(selection.front().path->curves.size());
        } else if (selection.front().indirection_type == Editor::Selector::PATH_CURVE) {
            InsertColumn(0, L"ID");
            InsertColumn(1, L"⇧ ");
            InsertColumn(2, L"⇩");
            InsertColumn(3, L"⇦");
            InsertColumn(4, L"⇨");
            SetItemCount(selection.front().curve->parent->curves.size());
        } else if (selection.front().indirection_type == Editor::Selector::NAVMESH) {
            InsertColumn(0, L"ID");
            InsertColumn(1, L"⇧ ");
            InsertColumn(2, L"⇩");
            InsertColumn(3, L"⇦");
            InsertColumn(4, L"⇨");
            SetItemCount(selection.front().navmesh->nodes.size());
        } else if (selection.front().indirection_type == Editor::Selector::NAVMESH_NODE) {
            InsertColumn(0, L"ID");
            InsertColumn(1, L"⇧");
            InsertColumn(2, L"⇩");
            InsertColumn(3, L"⇦");
            InsertColumn(4, L"⇨");
            SetItemCount(selection.front().node->parent->nodes.size());
        } else {
             SetItemCount(0);
        }*/
    }