#ifndef WIDGETS_MAINFRAME_H
#define WIDGETS_MAINFRAME_H

#include <wx/wx.h>
#include <wx/aui/aui.h>

class MainFrameCtrl : public wxFrame {
public:
    MainFrameCtrl();
    ~MainFrameCtrl() {
        delete std_cout_redirect;
        m_mgr.UnInit();
    }
protected:
    void OnHelp(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
    void OnAction(wxCommandEvent& event);
    
    void OnLoadCells(wxCommandEvent& event);
    void OnSaveCells(wxCommandEvent& event);
    
    void OnClose(wxCloseEvent& event);

    bool property_panel_degrees = false;
    bool property_panel_radians = true;
    
    wxAuiManager m_mgr;
    
    wxStreamToTextRedirector* std_cout_redirect;
};

extern MainFrameCtrl* main_frame;

#endif // WIDGETS_MAINFRAME_H