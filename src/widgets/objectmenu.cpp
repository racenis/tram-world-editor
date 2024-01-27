#include <editor/editor.h>
#include <editor/language.h>
#include <editor/actions.h>

#include <widgets/signaleditor.h>

#include <widgets/objectmenu.h>


ObjectMenuCtrl* world_tree_popup = nullptr;

ObjectMenuCtrl::ObjectMenuCtrl () : wxMenu() {
    is_visible_checkbox = AppendCheckItem(1, Editor::selected_language->dialog_show); 
    add_selection = Append(2, Editor::selected_language->dialog_add_new);
    edit_selection = Append(3, Editor::selected_language->dialog_edit);
    duplicate_selection = Append(4, L"Duplicate");
    duplicate_selection = Append(5, L"Signals");
    delete_selection = Append(6, Editor::selected_language->dialog_delete);
    
    this->Bind(wxEVT_MENU, &ObjectMenuCtrl::OnIsVisibleCheckboxClick, this, 1);
    this->Bind(wxEVT_MENU, &ObjectMenuCtrl::OnAddSelection, this, 2);
    this->Bind(wxEVT_MENU, &ObjectMenuCtrl::OnEditSelection, this, 3);
    this->Bind(wxEVT_MENU, &ObjectMenuCtrl::OnDuplicateSelection, this, 4);
    this->Bind(wxEVT_MENU, &ObjectMenuCtrl::OnSignalSelection, this, 5);
    this->Bind(wxEVT_MENU, &ObjectMenuCtrl::OnDeleteSelection, this, 6);
}

void ObjectMenuCtrl::SetSelectionStatus(Editor::Selection* selection) {
    bool is_visible, is_addable, is_editable, is_duplicatable, is_deletable;
    is_visible = is_addable = is_editable = is_duplicatable = is_deletable = true;
    
    for (auto& object : selection->objects) { if (object->IsHidden()) { is_visible = false; break; } }
    for (auto& object : selection->objects) { if (!object->IsAddable()) { is_addable = false; break; } }
    for (auto& object : selection->objects) { if (!object->IsEditable()) { is_editable = false; break; } }
    for (auto& object : selection->objects) { if (!object->IsCopyable()) { is_duplicatable = false; break; } }
    for (auto& object : selection->objects) { if (!object->IsRemovable()) { is_deletable = false; break; } }
    
    is_visible_checkbox->Check(is_visible);
    add_selection->Enable(is_addable);
    edit_selection->Enable(is_editable);
    duplicate_selection->Enable(is_duplicatable);
    delete_selection->Enable(is_deletable);
}

void ObjectMenuCtrl::OnIsVisibleCheckboxClick(wxCommandEvent& event) {
    std::cout << "Clicked on is visible!" << std::endl;
    // TODO: move this code out into editor.h logic code
    bool is_hidden = false;
    for (auto& object : Editor::SELECTION->objects) { if (object->IsHidden()) { is_hidden = true; break; } }
    for (auto& object : Editor::SELECTION->objects) { object->SetHidden(!is_hidden); }
}

void ObjectMenuCtrl::OnAddSelection(wxCommandEvent& event) {
    Editor::PerformAction<Editor::ActionNew>();
}

void ObjectMenuCtrl::OnEditSelection(wxCommandEvent& event) {
    Editor::PropertyPanel::SetCurrentSelection();
}

void ObjectMenuCtrl::OnDuplicateSelection(wxCommandEvent& event) {
    Editor::PerformAction<Editor::ActionDuplicate>();
}

void ObjectMenuCtrl::OnSignalSelection(wxCommandEvent& event) {
    OpenSignalEditorModal();
}

void ObjectMenuCtrl::OnDeleteSelection(wxCommandEvent& event) {
    Editor::PerformAction<Editor::ActionRemove>();
}