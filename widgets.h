#ifndef WIDGETS_H
#define WIDGETS_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>

#include <wx/propgrid/propgrid.h>
#include <wx/aui/aui.h>
#include <wx/aboutdlg.h>
#include <wx/progdlg.h>

class MainFrame;
class EntityList;
class Viewport;
class EditorObjectMenu;

// at any point in time the editor will have one and only one window open
// and only one instance of each widget. that means that we can just declare
// them as globals, as to make programming easier. this app is so small
// that it will not be a problem
extern MainFrame* main_frame;
extern wxTreeCtrl* world_tree;
extern EntityList* entity_list;
extern wxPropertyGrid* property_panel;
extern EditorObjectMenu* world_tree_popup;

namespace Editor { class Selection; }

// this is the main window class of the app
class MainFrame : public wxFrame {
public:
    MainFrame();
    ~MainFrame()
    {
        delete std_cout_redirect;
        // deinitialize the frame manager
        m_mgr.UnInit();
    }
protected:
    wxAuiManager m_mgr;
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSettingsChange(wxCommandEvent& event);
    
    void OnLoadCells(wxCommandEvent& event);
    void OnSaveCells(wxCommandEvent& event);
    
    // world/worldcell tree is the tree in the top-left pane
    // that contains a list of all of the world-cells
    //wxMenu* world_tree_cell_popup;
    //wxMenu* world_tree_item_popup;
    //wxMenu* world_tree_root_popup;
    //wxMenuItem* world_tree_cell_is_visible_checkbox;
    //wxMenuItem* world_tree_item_is_visible_checkbox;
    //wxMenuItem* world_tree_item_add_selection;
    //wxMenuItem* world_tree_item_edit_selection;
    //wxMenuItem* world_tree_item_delete_selection;
    //wxTreeItemId world_tree_selected_item;
    //wxTreeItemId world_tree_root_node;
    //std::unordered_map<void*, Editor::WorldCellIndirector> world_tree_map;
    //void BuildWorldCellTree();
    //void RebuildWorldCellTree();
    //void OnWorldCellTreeRightClick(wxTreeEvent& event);
    //void OnWorldCellTreeDoubleClick(wxTreeEvent& event);
    //void OnWorldCellTreePopupClick(wxCommandEvent& event);
    
    // property panel stuff
    
    bool property_panel_degrees = false;
    bool property_panel_radians = true;
    //std::vector<Editor::Selector> selection;
    //void SetSingleSelection(Editor::Selector select);
    void PropertyPanelClear(); // TODO: yeet this
    void PropertyPanelRebuild();
    void OnPropertyPanelChanged(wxPropertyGridEvent& event);
    std::unordered_map<std::string, bool> property_collapsed;
    
    // entity list stuff
    
    wxMenu* entity_list_popup;
    wxMenu* entity_list_change_type_popup;
    wxMenuItem* entity_list_change_type_popup_item;
    long entity_list_selected_item;
    void OnEntityListRightClick(wxListEvent& event);
    void OnEntityListClick(wxListEvent& event);
    void OnEntityListDoubleClick(wxListEvent& event);
    void OnEntityListPopupSelect(wxCommandEvent& event);
    void OnEntityTypeChange(wxCommandEvent& event);
    
    // for viewport
    Viewport* viewport;
    
    wxStreamToTextRedirector* std_cout_redirect;
    
    friend class EntityList;
    friend class Viewport;
};


class EntityList : public wxListCtrl {
public:
    EntityList (wxWindow* parent) :  wxListCtrl(parent, -1, wxDefaultPosition, wxSize(200,150), wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES) {
            Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &EntityList::OnMenuOpen, this);
            Bind(wxEVT_LIST_ITEM_SELECTED, &EntityList::OnSelectionChanged, this);
            Bind(wxEVT_LIST_ITEM_ACTIVATED, &EntityList::OnItemActivated, this);
        }

    wxString OnGetItemText (long item, long column) const override;
    
    void RefreshAllItems ();
    
    void OnMenuOpen(wxListEvent& event);
    void OnSelectionChanged(wxListEvent& event);
    void OnItemActivated(wxListEvent& event);
};

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

#endif // WIDGETS_H