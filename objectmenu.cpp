#include <objectmenu.h>
#include <editor.h>
#include <language.h>
#include <actions.h>

EditorObjectMenu::EditorObjectMenu () : wxMenu() {
    is_visible_checkbox = AppendCheckItem(1, Editor::selected_language->dialog_show); 
    add_selection = Append(2, Editor::selected_language->dialog_add_new);
    edit_selection = Append(3, Editor::selected_language->dialog_edit);
    delete_selection = Append(4, Editor::selected_language->dialog_delete);
    
    this->Bind(wxEVT_MENU, &EditorObjectMenu::OnIsVisibleCheckboxClick, this, 1);
    this->Bind(wxEVT_MENU, &EditorObjectMenu::OnAddSelection, this, 2);
    this->Bind(wxEVT_MENU, &EditorObjectMenu::OnEditSelection, this, 3);
    this->Bind(wxEVT_MENU, &EditorObjectMenu::OnDeleteSelection, this, 4);
}

void EditorObjectMenu::SetSelectionStatus(Editor::Selection* selection) {
    bool is_visible, is_addable, is_editable, is_deletable;
    is_visible = is_addable = is_editable = is_deletable = true;
    
    for (auto& object : selection->objects) { if (object->IsHidden()) { is_visible = false; break; } }
    for (auto& object : selection->objects) { if (!object->IsAddable()) { is_addable = false; break; } }
    for (auto& object : selection->objects) { if (!object->IsEditable()) { is_editable = false; break; } }
    for (auto& object : selection->objects) { if (!object->IsRemovable()) { is_deletable = false; break; } }
    
    is_visible_checkbox->Check(is_visible);
    add_selection->Enable(is_addable);
    edit_selection->Enable(is_editable);
    delete_selection->Enable(is_deletable);
}

void EditorObjectMenu::OnIsVisibleCheckboxClick(wxCommandEvent& event) {
    std::cout << "Clicked on is visible!" << std::endl;
    // TODO: move this code out into editor.h logic code
    bool is_hidden = false;
    for (auto& object : Editor::selection->objects) { if (object->IsHidden()) { is_hidden = true; break; } }
    for (auto& object : Editor::selection->objects) { object->SetHidden(!is_hidden); }
}

void EditorObjectMenu::OnAddSelection(wxCommandEvent& event) {
    Editor::PerformAction<Editor::ActionNew>();
}

void EditorObjectMenu::OnEditSelection(wxCommandEvent& event) {
    Editor::PropertyPanel::SetCurrentSelection();
}

void EditorObjectMenu::OnDeleteSelection(wxCommandEvent& event) {
    Editor::PerformAction<Editor::ActionRemove>();
}