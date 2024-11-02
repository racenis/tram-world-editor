#include <editor/editor.h>
#include <editor/language.h>
#include <editor/actions.h>

#include <widgets/mainframe.h>
#include <widgets/objectmenu.h>
#include <widgets/objectlist.h>

using namespace Editor;
std::vector<PropertyDefinition> columns;
std::shared_ptr<Object> selected_object;

ObjectListCtrl* object_list = nullptr;

ObjectListCtrl::ObjectListCtrl(wxWindow* parent) :  wxListCtrl(parent, -1, wxDefaultPosition, wxSize(200,150), wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES) {
    Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &ObjectListCtrl::OnMenuOpen, this);
    Bind(wxEVT_LIST_ITEM_SELECTED, &ObjectListCtrl::OnSelectionChanged, this);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ObjectListCtrl::OnItemActivated, this);
}

// I think that htis finds which object is going to be shown in the object list
auto GetSingleSelectedObjectFromSelection() {
    if (SELECTION->objects.size() == 1) {
        // if there is only one object selected, then just return that
        auto object = SELECTION->objects.front();
        return object->IsChildrenListable() ? object : object->GetParent();
    } else if (SELECTION->objects.size() > 0) {
        // otherwise check if all of the selected objects have the same type
        auto first = SELECTION->objects.front().get();
        auto first_type = typeid(*first).hash_code();
        for (const auto& obj : SELECTION->objects) {
            auto next = obj.get();
            auto next_type = typeid(*next).hash_code();
            if (first_type != next_type) {
                return std::shared_ptr<Object>(nullptr);
            }
        }
        auto object = SELECTION->objects.front();
        return object->IsChildrenListable() ? object : object->GetParent();
    } else {
        return std::shared_ptr<Object>(nullptr);
    }
}

void Editor::ObjectList::SetCurrentSelection() {
    object_list->DeleteAllColumns();
    columns.clear();
    
    selected_object = GetSingleSelectedObjectFromSelection();
    
    if (selected_object) {
        columns = selected_object->GetListPropertyDefinitions();
        
        for (size_t i = 0; i < columns.size(); i++) {
            object_list->InsertColumn(i, Editor::Get(columns[i].display_name));
        }
        
        object_list->SetItemCount(selected_object->GetChildren().size());
    } else {
        object_list->SetItemCount(0);
    }
}

void Editor::ObjectList::Refresh() {
    // I think that it might be a good idea to change the name of selected_object
    // to displayed_object or something
    if (selected_object && selected_object == GetSingleSelectedObjectFromSelection()) {
        auto list_selection = selected_object->GetChildren();
        auto all_selected = SELECTION->objects;
        object_list->SetItemCount(list_selection.size());
        
        object_list->disable_selection = true;
        
        // step through every object in ObjectList
        auto check = list_selection.begin();
        for (size_t i = 0; i < list_selection.size(); i++) {
            
            // check if it is selected
            if (std::find(all_selected.begin(), all_selected.end(), *check) != all_selected.end()) {
                // mark as selected in the widget
                object_list->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            } else {
                // mark as unselected in the widget
                object_list->SetItemState(i, 0, wxLIST_STATE_SELECTED);
            }
            
            check++;
        }
        
        object_list->disable_selection = false;
        
        object_list->Refresh();
    } else {
        SetCurrentSelection();
    }
}

wxString ObjectListCtrl::OnGetItemText (long item, long column) const {
    auto childrens = selected_object->GetChildren();
    auto first_childrens = childrens.begin();
    std::advance(first_childrens, item);
    auto& info = columns[column];
    auto& object = *first_childrens;
    
    PropertyValue value = object->GetProperty(info.name);
    
    switch (value.type) {
        case PROPERTY_STRING: 
            return wxString(Editor::Get(value.str_value));
        case PROPERTY_FLOAT:
            return wxString(std::to_string(value.float_value));
        case PROPERTY_INT:
            return wxString(std::to_string(value.int_value));
        case PROPERTY_UINT:
            return wxString(std::to_string(value.uint_value));
        default:
            return wxString("nil");
        }
}

void ObjectListCtrl::OnMenuOpen(wxListEvent& event) {
    world_tree_popup->SetSelectionStatus(Editor::SELECTION.get());
    main_frame->PopupMenu(world_tree_popup);
}

void ObjectListCtrl::OnSelectionChanged(wxListEvent& event) {
    // this here is a hack.
    // this method is supposed to run only if you click on a thing in the object
    // list. like click with a mouse, to select.
    // sometimes we need to select stuff programmatically, but this method will
    // still be called (wxwidgets acting stupid). so this hack will make this
    // method go away when we do that
    if (disable_selection) return;
    
    auto new_selection = std::make_shared<Editor::Selection>();
    auto childrens = selected_object->GetChildren();
    
    for (long item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); item != -1; item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)){
        auto chi_benig = childrens.begin();
        std::advance(chi_benig, item);
        new_selection->objects.push_back(*chi_benig);
    }
    
    Editor::PerformAction<Editor::ActionChangeSelection>(new_selection);
}

void ObjectListCtrl::OnItemActivated(wxListEvent& event) {
    //Editor::PropertyPanel::SetCurrentSelection();
    //Editor::ObjectList::SetCurrentSelection();
}