#ifndef WIDGETS_SIGNALEDITOR_H
#define WIDGETS_SIGNALEDITOR_H

#include <wx/wx.h>

#include <editor/objects/entity.h>

class wxListEvent;
class SignalListCtrl;

class SignalEditor : public wxDialog {
public:
    SignalEditor();
    ~SignalEditor() {}
    
    void AddNew(wxCommandEvent& event);
    void Delete(wxCommandEvent& event);
    void Select(wxListEvent& event);
    void Change(wxCommandEvent& event);
protected:
    wxTextCtrl* target_value;
    wxStaticText* target_label;
    wxTextCtrl* delay_value;
    wxStaticText* delay_label;
    wxTextCtrl* repeats_value;
    wxStaticText* repeats_label;
    wxChoice* message_type_value;
    wxChoice* signal_type_value;
    wxStaticText* message_type_label;
    wxStaticText* signal_type_label;
    wxChoice* parameter_type_value;
    wxStaticText* parameter_type_label;
    wxTextCtrl* parameter_value;
    wxStaticText* parameter_label;
    SignalListCtrl* signal_list;
    int selected = -1;
    std::shared_ptr<Editor::Entity> entity;
};

#endif // WIDGETS_SIGNALEDITOR_H