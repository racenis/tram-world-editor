#ifndef WIDGETS_PROPERTYPANEL_H
#define WIDGETS_PROPERTYPANEL_H

#include <wx/propgrid/propgrid.h>

class PropertyPanelCtrl : public wxPropertyGrid {
public:
    PropertyPanelCtrl (wxWindow* parent);
    void OnChanged (wxPropertyGridEvent& event);
    void OnCollapsed (wxPropertyGridEvent& event);
    void OnExpanded (wxPropertyGridEvent& event);
};

extern PropertyPanelCtrl* property_panel;

#endif // WIDGETS_PROPERTYPANEL_H