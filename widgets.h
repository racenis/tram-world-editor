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

// at any point in time the editor will have one and only one window open
// and only one instance of each widget. that means that we can just declare
// them as globals, as to make programming easier. this app is so small
// that it will not be a problem
MainFrame* main_frame = nullptr;
wxTreeCtrl* world_tree = nullptr;
EntityList* entity_list = nullptr;
wxPropertyGrid* property_panel = nullptr;

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
    wxMenu* world_tree_cell_popup;
    wxMenu* world_tree_item_popup;
    wxMenu* world_tree_root_popup;
    wxMenuItem* world_tree_cell_is_visible_checkbox;
    wxMenuItem* world_tree_item_is_visible_checkbox;
    wxMenuItem* world_tree_item_add_selection;
    wxMenuItem* world_tree_item_edit_selection;
    wxMenuItem* world_tree_item_delete_selection;
    wxTreeItemId world_tree_selected_item;
    wxTreeItemId world_tree_root_node;
    //std::unordered_map<void*, Editor::WorldCellIndirector> world_tree_map;
    void BuildWorldCellTree();
    void RebuildWorldCellTree();
    void OnWorldCellTreeRightClick(wxTreeEvent& event);
    void OnWorldCellTreeDoubleClick(wxTreeEvent& event);
    void OnWorldCellTreePopupClick(wxCommandEvent& event);
    
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
    EntityList (wxWindow* parent, wxWindowID id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxLC_ICON) : wxListCtrl(parent, id, pos, size, style), parent_frame((MainFrame*)parent) {}

    wxString OnGetItemText (long item, long column) const override {
        return wxString("nil");
    }
    
    
    void RefreshAllItems () {
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
    
    MainFrame* parent_frame;
};

#endif // WIDGETS_H