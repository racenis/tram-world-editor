#include <editor.h>
#include <widgets.h>

namespace Editor::PropertyPanel {
    void SetCurrentSelection() {
        property_panel->Clear();
        if (selection->objects.size()) {
            std::unordered_map<std::string, wxPGProperty*> fields;
            
            // add fields and categories from the selection
            for (auto& object : selection->objects) {
                auto object_fields = object->GetFullPropertyDefinitions();
                
                for (auto& field : object_fields) {
                    auto field_ptr = fields[field.display_name];
                    
                    if (!field_ptr) {
                        switch (field.type) {
                            case PROPERTY_STRING:
                                field_ptr = new wxStringProperty(field.display_name, field.name);
                                break;
                            case PROPERTY_FLOAT:
                                field_ptr = new wxFloatProperty(field.display_name, field.name);
                                break;
                            case PROPERTY_INT:
                                field_ptr = new wxIntProperty(field.display_name, field.name);
                                break;
                            case PROPERTY_UINT:
                                field_ptr = new wxUIntProperty(field.display_name, field.name);
                                break;
                            case PROPERTY_CATEGORY:
                                field_ptr = new wxPropertyCategory(field.display_name, field.name);
                                break;
                        }
                    }
                    
                    auto category_ptr = fields[field.category_name];
                    if (category_ptr) {
                        property_panel->AppendIn(category_ptr, field_ptr);
                    } else {
                        property_panel->Append(field_ptr);
                    }
                }
            }
            
            // add in values from the selection
            
            // TODO: implement
            
        } else {
            std::cout << "Selection canceled! " << std::endl;
        }
    }
}