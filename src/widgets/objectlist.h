#ifndef WIDGETS_OBJECTLIST_H
#define WIDGETS_OBJECTLIST_H

#include <wx/listctrl.h>

class ObjectListCtrl : public wxListCtrl {
public:
    ObjectListCtrl (wxWindow* parent) :  wxListCtrl(parent, -1, wxDefaultPosition, wxSize(200,150), wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES) {
            Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &ObjectListCtrl::OnMenuOpen, this);
            Bind(wxEVT_LIST_ITEM_SELECTED, &ObjectListCtrl::OnSelectionChanged, this);
            Bind(wxEVT_LIST_ITEM_ACTIVATED, &ObjectListCtrl::OnItemActivated, this);
        }

    wxString OnGetItemText (long item, long column) const override;
    
    void RefreshAllItems ();
    
    void OnMenuOpen(wxListEvent& event);
    void OnSelectionChanged(wxListEvent& event);
    void OnItemActivated(wxListEvent& event);
};

extern ObjectListCtrl* object_list;

#endif // WIDGETS_OBJECTLIST_H