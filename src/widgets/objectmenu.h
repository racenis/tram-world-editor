#ifndef WIDGETS_OBJECTMENU_H
#define WIDGETS_OBJECTMENU_H

#include <wx/wx.h>

#include <editor/editor.h>

class ObjectMenuCtrl : public wxMenu {
public:
    ObjectMenuCtrl ();
    
    void SetSelectionStatus(Editor::Selection* selection);
    
    void OnIsVisibleCheckboxClick(wxCommandEvent& event);
    void OnAddSelection(wxCommandEvent& event);
    void OnEditSelection(wxCommandEvent& event);
    void OnDuplicateSelection(wxCommandEvent& event);
    void OnSignalSelection(wxCommandEvent& event);
    void OnDeleteSelection(wxCommandEvent& event);
    
    wxMenuItem* is_visible_checkbox = nullptr;
    wxMenuItem* add_selection = nullptr;
    wxMenuItem* edit_selection = nullptr;
    wxMenuItem* duplicate_selection = nullptr;
    wxMenuItem* signal_selection = nullptr;
    wxMenuItem* delete_selection = nullptr;
};

extern ObjectMenuCtrl* world_tree_popup;

#endif // WIDGETS_OBJECTMENU_H