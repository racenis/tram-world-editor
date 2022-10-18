#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <wx/listctrl.h>

class ObjectList : public wxListCtrl {
public:
    ObjectList (wxWindow* parent) :  wxListCtrl(parent, -1, wxDefaultPosition, wxSize(200,150), wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES) {
            Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &ObjectList::OnMenuOpen, this);
            Bind(wxEVT_LIST_ITEM_SELECTED, &ObjectList::OnSelectionChanged, this);
            Bind(wxEVT_LIST_ITEM_ACTIVATED, &ObjectList::OnItemActivated, this);
        }

    wxString OnGetItemText (long item, long column) const override;
    
    void RefreshAllItems ();
    
    void OnMenuOpen(wxListEvent& event);
    void OnSelectionChanged(wxListEvent& event);
    void OnItemActivated(wxListEvent& event);
};

#endif // OBJECTLIST_H