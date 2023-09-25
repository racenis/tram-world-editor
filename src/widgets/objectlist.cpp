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

auto GetSelectedObject() {
    if (SELECTION->objects.size() > 0) {
        auto object = SELECTION->objects.front();
        return object->IsChildrenListable() ? object : object->GetParent();
    } else {
        return std::shared_ptr<Object>(nullptr);
    }
}

void Editor::ObjectList::SetCurrentSelection() {
    object_list->DeleteAllColumns();
    columns.clear();
    
    selected_object = GetSelectedObject();
    
    if (selected_object) {
        columns = selected_object->GetListPropertyDefinitions();
        
        for (size_t i = 0; i < columns.size(); i++) {
            object_list->InsertColumn(i, Editor::PropertyRename(columns[i].display_name));
        }
        
        object_list->SetItemCount(selected_object->GetChildren().size());
    } else {
        object_list->SetItemCount(0);
    }
}

void Editor::ObjectList::Refresh() {
    if (selected_object == GetSelectedObject()) {
        object_list->SetItemCount(selected_object->GetChildren().size());
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
            return wxString(Editor::PropertyRename(value.str_value));
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