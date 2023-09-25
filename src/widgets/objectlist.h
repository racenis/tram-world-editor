#ifndef WIDGETS_OBJECTLIST_H
#define WIDGETS_OBJECTLIST_H

#include <wx/listctrl.h>

class ObjectListCtrl : public wxListCtrl {
public:
    ObjectListCtrl (wxWindow* parent);

    wxString OnGetItemText (long item, long column) const override;
    
    void RefreshAllItems ();
    
    void OnMenuOpen(wxListEvent& event);
    void OnSelectionChanged(wxListEvent& event);
    void OnItemActivated(wxListEvent& event);
};

extern ObjectListCtrl* object_list;

#endif // WIDGETS_OBJECTLIST_H