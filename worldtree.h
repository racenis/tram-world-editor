#ifndef WORLDTREE_H
#define WORLDTREE_H

#include <wx/treectrl.h>

class WorldTree : public wxTreeCtrl {
public:
    WorldTree (wxWindow* parent);
    void OnMenuOpen (wxTreeEvent& event);
    void OnSelectionChanged (wxTreeEvent& event);
    void OnItemActivated (wxTreeEvent& event);
};

#endif // WORLDTREE_H