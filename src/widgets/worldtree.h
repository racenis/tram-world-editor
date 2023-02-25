#ifndef WIDGETS_WORLDTREE_H
#define WIDGETS_WORLDTREE_H

#include <wx/treectrl.h>

class WorldTreeCtrl : public wxTreeCtrl {
public:
    WorldTreeCtrl (wxWindow* parent);
    void OnMenuOpen (wxTreeEvent& event);
    void OnSelectionChanged (wxTreeEvent& event);
    void OnItemActivated (wxTreeEvent& event);
};

extern WorldTreeCtrl* world_tree;

#endif // WIDGETS_WORLDTREE_H