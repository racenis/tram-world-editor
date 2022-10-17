#include <editor.h>
#include <widgets.h>

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
                            case PROPERTY_NULL:
                                abort();
                        }
                        
                        fields[field.display_name] = field_ptr;
                        fields_list.push_back(field_ptr);
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
            
            for (auto& field : fields_list) {
                if (field->IsCategory()) continue;
                std::string field_name = field->GetName().ToStdString();
                wxVariant field_value;
                bool first = true;
                
                // will crash if selection.size() < 1
                for (auto& object : selection->objects) {
                    wxVariant field_value_next;
                    PropertyValue field_value_retrieved = object->GetProperty(field_name);
                    
                    switch (field_value_retrieved.type) {
                        case PROPERTY_STRING:
                            field_value_next = wxVariant(field_value_retrieved.str_value);
                            break;
                        case PROPERTY_FLOAT:
                            field_value_next = wxVariant(field_value_retrieved.float_value);
                            break;
                        case PROPERTY_INT:
                            field_value_next = wxVariant(wxLongLong(field_value_retrieved.int_value));
                            break;
                        case PROPERTY_UINT:
                            field_value_next = wxVariant(wxULongLong(field_value_retrieved.uint_value));
                            break;
                        case PROPERTY_CATEGORY:
                            goto field_is_category;
                        case PROPERTY_NULL:
                            std::cout << "[PROPS] prop null for " << field_name << std::endl;
                            goto not_matching;
                    }
                    
                    if (first) {
                        field_value = field_value_next;
                        field->SetValue(field_value);
                        first = false;
                    } else if (field_value != field_value_next) {
                        std::cout << "[PROPS] not matching: " << field_value.GetString().c_str() << " and " << field_value_next.GetString().c_str() << std::endl;
                        goto not_matching;
                    }
                }
                
                not_matching:
                field->SetBackgroundColour(*wxYELLOW);
                continue;
                
                field_is_category:
                continue;
            }
            
        } else {
            std::cout << "Selection canceled! " << std::endl;
        }
    }
}