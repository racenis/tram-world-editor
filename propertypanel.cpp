#include <editor.h>
#include <actions.h>
#include <widgets.h>
#include <propertypanel.h>

namespace Editor::PropertyPanel {
    void SetCurrentSelection() {
        property_panel->Clear();
        if (selection->objects.size()) {
            std::unordered_map<std::string, wxPGProperty*> fields;
            std::vector<wxPGProperty*> fields_list;
            
            // add fields and categories from the selection
            for (auto& object : selection->objects) {
                auto object_fields = object->GetFullPropertyDefinitions();
                
                for (auto& field : object_fields) {
                    auto field_ptr = fields[field.name];
                    
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
                            case PROPERTY_NULL:
                                abort();
                        }
                        
                        fields[field.name] = field_ptr;
                        fields_list.push_back(field_ptr);
                        
                        auto category_ptr = fields[field.category_name];
                        if (category_ptr) {
                            property_panel->AppendIn(category_ptr, field_ptr);
                        } else {
                            property_panel->Append(field_ptr);
                        }
                    }
                }
            }
            
            // add in values from the selection
            
            for (auto& field : fields_list) {
                if (field->IsCategory()) continue;
                std::string field_name = field->GetName().ToStdString();
                PropertyValue value;
                bool first = true;
                
                // will crash if selection.size() < 1
                for (auto& object : selection->objects) {
                    PropertyValue next_value = object->GetProperty(field_name);
                    
                    if (first) {
                        value = next_value;
                        first = false;
                        if (value.type == PROPERTY_NULL) goto not_matching;
                    } else if (value != next_value) {
                        std::cout << "[PROPS] VALUE not matching !! " << std::endl;
                        goto not_matching;
                    }
                }

                switch (value.type) {
                    case PROPERTY_STRING:
                        field->SetValue(value.str_value);
                        break;
                    case PROPERTY_FLOAT:
                        field->SetValue(value.float_value);
                        break;
                    case PROPERTY_INT:
                        field->SetValue(wxLongLong(value.int_value));
                        break;
                    case PROPERTY_UINT:
                        field->SetValue(wxULongLong(value.uint_value));
                        break;
                    default:
                        break;
                }
                
                continue;
                
                not_matching:
                field->SetBackgroundColour(*wxYELLOW);
                field->SetValue("---");
            }
            
            property_panel->Refresh();
        } else {
            std::cout << "Selection canceled! " << std::endl;
        }
    }
}

    PropertyPanel::PropertyPanel (wxWindow* parent) : wxPropertyGrid(parent, -1) {
        Bind(wxEVT_PG_CHANGED, &PropertyPanel::OnChanged, this);
        Bind(wxEVT_PG_ITEM_COLLAPSED, &PropertyPanel::OnCollapsed, this);
        Bind(wxEVT_PG_ITEM_EXPANDED, &PropertyPanel::OnExpanded, this);
    }
    
    void PropertyPanel::OnChanged (wxPropertyGridEvent& event) {
        std::cout << "Something changed!" << std::endl;
        auto value = event.GetValue();
        auto value_name = event.GetPropertyName().ToStdString();
        
        // make a back-up of the properties of the selected objects 
        Editor::PerformAction<Editor::ActionChangeProperties>();
        
        // TODO: cache the result of value.GetType() == "type"
        for (auto& object : Editor::selection->objects) {
            if (value.GetType() == "longlong") {
                object->SetProperty(value_name, value.GetLongLong().GetValue());
            } else if (value.GetType() == "ulonglong") {
                object->SetProperty(value_name, value.GetULongLong().GetValue());
            } else if (value.GetType() == "double") {
                object->SetProperty(value_name, (float)value.GetDouble());
            } else if (value.GetType() == "string") {
                object->SetProperty(value_name, value.GetString().ToStdString());
            } else {
                std::cout << "value type '" << value.GetType().c_str() << "' unrecognized!" << std::endl;
            }
        }
        
        
    }
    
    void PropertyPanel::OnCollapsed (wxPropertyGridEvent& event) {
        std::cout << "Something collapsed!" << std::endl;
    }
    
    void PropertyPanel::OnExpanded (wxPropertyGridEvent& event) {
        std::cout << "Something expanded!" << std::endl;
    }