#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H

#include <wx/propgrid/propgrid.h>

class PropertyPanel : public wxPropertyGrid {
public:
    PropertyPanel (wxWindow* parent);
    void OnChanged (wxPropertyGridEvent& event);
    void OnCollapsed (wxPropertyGridEvent& event);
    void OnExpanded (wxPropertyGridEvent& event);
};

#endif // PROPERTYPANEL_H