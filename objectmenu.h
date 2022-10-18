#ifndef OBJECTMENU_H
#define OBJECTMENU_H

#include <editor.h>
#include <wx/wx.h>

class EditorObjectMenu : public wxMenu {
public:
    EditorObjectMenu ();
    
    void SetSelectionStatus(Editor::Selection* selection);
    
    void OnIsVisibleCheckboxClick(wxCommandEvent& event);
    
    void OnAddSelection(wxCommandEvent& event);
    
    void OnEditSelection(wxCommandEvent& event);
    
    void OnDeleteSelection(wxCommandEvent& event);
    
    wxMenuItem* is_visible_checkbox = nullptr;
    wxMenuItem* add_selection = nullptr;
    wxMenuItem* edit_selection = nullptr;
    wxMenuItem* delete_selection = nullptr;
    
};

#endif // OBJECTMENU_H