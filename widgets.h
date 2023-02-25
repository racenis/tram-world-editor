#ifndef WIDGETS_H
#define WIDGETS_H

#include <wx/wx.h>




#include <wx/aui/aui.h>
#include <wx/aboutdlg.h>
#include <wx/progdlg.h>

class MainFrame;
class ObjectListCtrl;
class ViewportCtrl;
class EditorObjectMenu;
class PropertyPanel;
class WorldTree;

// at any point in time the editor will have one and only one window open
// and only one instance of each widget. that means that we can just declare
// them as globals, as to make programming easier. this app is so small
// that it will not be a problem
extern MainFrame* main_frame;
extern WorldTree* world_tree;
extern ObjectListCtrl* object_list;
extern PropertyPanel* property_panel;
extern EditorObjectMenu* world_tree_popup;
extern ViewportCtrl* viewport;

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
    void OpenViewport();
protected:
    wxAuiManager m_mgr;
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
    void OnAction(wxCommandEvent& event);
    
    void OnLoadCells(wxCommandEvent& event);
    void OnSaveCells(wxCommandEvent& event);
    
    void OnClose(wxCloseEvent& event);

    bool property_panel_degrees = false;
    bool property_panel_radians = true;
    
    wxStreamToTextRedirector* std_cout_redirect;
};



#endif // WIDGETS_H