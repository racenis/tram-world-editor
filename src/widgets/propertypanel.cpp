#include <editor/editor.h>
#include <editor/language.h>
#include <editor/actions.h>
#include <editor/settings.h>

#include <widgets/propertypanel.h>

#include <sstream>

std::unordered_map<std::string, bool> category_is_collapsed;

PropertyPanelCtrl* property_panel = nullptr;

void Editor::PropertyPanel::SetCurrentSelection() {
    property_panel->Clear();
    if (SELECTION->objects.size()) {
        std::unordered_map<std::string, wxPGProperty*> fields;
        std::vector<wxPGProperty*> fields_list;
        
        // add fields and categories from the selection
        for (auto& object : SELECTION->objects) {
            auto object_fields = object->GetFullPropertyDefinitions();
            
            for (auto& field : object_fields) {
                auto field_ptr = fields[field.name];
                
                if (!field_ptr) {
                    switch (field.type) {
                        case PROPERTY_STRING:
                        case PROPERTY_VECTOR:
                        case PROPERTY_ORIGIN:
                        case PROPERTY_DIRECTION:
                            field_ptr = new wxStringProperty(Editor::Get(field.display_name), field.name);
                            break;
                        case PROPERTY_FLOAT:
                            field_ptr = new wxFloatProperty(Editor::Get(field.display_name), field.name);
                            break;
                        case PROPERTY_INT:
                            field_ptr = new wxIntProperty(Editor::Get(field.display_name), field.name);
                            break;
                        case PROPERTY_UINT:
                        case PROPERTY_FLAG:
                            field_ptr = new wxUIntProperty(Editor::Get(field.display_name), field.name);
                            break;
                        case PROPERTY_ENUM:
                            {
                                wxPGChoices choices;
                                for (auto& enumeration: PROPERTY_ENUMERATIONS[field.name]) choices.Add(Editor::Get(enumeration));
                                field_ptr = new wxEnumProperty(Editor::Get(field.display_name), field.name, choices);
                            }
                            break;
                        case PROPERTY_BOOL:
                            field_ptr = new wxBoolProperty(Editor::Get(field.display_name), field.name);
                            field_ptr->SetAttribute("UseCheckbox", 1);
                            break;
                        case PROPERTY_CATEGORY:
                            field_ptr = new wxPropertyCategory(Editor::Get(field.display_name), field.name);
                            field_ptr->ChangeFlag(wxPG_PROP_COLLAPSED, category_is_collapsed[field.name]);
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
            for (auto& object : SELECTION->objects) {
                PropertyValue next_value = object->GetProperty(field_name);
                
                if (first) {
                    value = next_value;
                    first = false;
                    if (value.type == PROPERTY_NULL) goto not_matching;
                } else if (value != next_value) {
                    goto not_matching;
                }
            }

            switch (value.type) {
                case PROPERTY_STRING:
                    field->SetValue(Editor::Get(value.str_value));
                    break;
                case PROPERTY_FLOAT:
                    // hack
                    if (Settings::ROTATION_UNIT == Settings::ROTATION_DEGREES && (field_name == "rotation-x" || field_name == "rotation-y" || field_name == "rotation-z")) {
                        field->SetValue(glm::degrees(value.float_value));
                    } else {
                        field->SetValue(value.float_value);
                    }
                    break;
                case PROPERTY_INT:
                    field->SetValue(wxLongLong(value.int_value));
                    break;
                case PROPERTY_UINT:
                    field->SetValue(wxULongLong(value.uint_value));
                    break;
                case PROPERTY_ENUM:
                    field->SetValue((long) value.uint_value);
                    break;
                case PROPERTY_BOOL:
                    field->SetValue(value.bool_value);
                    break;
                case PROPERTY_VECTOR:
                case PROPERTY_ORIGIN:
                case PROPERTY_DIRECTION:
                {
                    std::string x_str = std::to_string(value.vector_value.x);
                    std::string y_str = std::to_string(value.vector_value.y);
                    std::string z_str = std::to_string(value.vector_value.z);
                
                    x_str.erase(x_str.find_last_not_of('0') + 1, std::string::npos);
                    y_str.erase(x_str.find_last_not_of('0') + 1, std::string::npos);
                    z_str.erase(x_str.find_last_not_of('0') + 1, std::string::npos);
                
                //std::cout << "READ IN!!!" << value.vector_value.x << "  " << value.vector_value.z << std::endl;
                
                    field->SetValue(x_str + " " + y_str + " " + z_str);
                }
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
        //std::cout << "Selection canceled! " << std::endl;
    }
}
    
void Editor::PropertyPanel::Refresh() {
    SetCurrentSelection();
}


PropertyPanelCtrl::PropertyPanelCtrl (wxWindow* parent) : wxPropertyGrid(parent, -1) {
    Bind(wxEVT_PG_CHANGED, &PropertyPanelCtrl::OnChanged, this);
    Bind(wxEVT_PG_ITEM_COLLAPSED, &PropertyPanelCtrl::OnCollapsed, this);
    Bind(wxEVT_PG_ITEM_EXPANDED, &PropertyPanelCtrl::OnExpanded, this);
}

void PropertyPanelCtrl::OnChanged (wxPropertyGridEvent& event) {
    auto value = event.GetValue();
    auto value_name = event.GetPropertyName().ToStdString();
    
    // make a back-up of the properties of the selected objects 
    Editor::PerformAction<Editor::ActionChangeProperties>(std::vector<std::string> {value_name});
    
    for (auto& object : Editor::SELECTION->objects) {
        if (value.GetType() == "longlong") {
            object->SetProperty(value_name, value.GetLongLong().GetValue());
        } else if (value.GetType() == "ulonglong") {
            object->SetProperty(value_name, value.GetULongLong().GetValue());
        } else if (value.GetType() == "double") {
            float float_value = value.GetDouble();
            
            if (Editor::Settings::ROTATION_UNIT == Editor::Settings::ROTATION_DEGREES && (value_name == "rotation-x" || value_name == "rotation-y" || value_name == "rotation-z")) {
                float_value = glm::radians(float_value);
            } 
            
            object->SetProperty(value_name, float_value);
        } else if (value.GetType() == "string") {
            switch (object->GetProperty(value_name).type) {
                case Editor::PROPERTY_VECTOR:
                case Editor::PROPERTY_ORIGIN:
                case Editor::PROPERTY_DIRECTION:
                {
                    std::string new_value_unparse = value.GetString().ToStdString();
                    std::stringstream new_value_parse(new_value_unparse); 
                    //Editor::PropertyValue::Vector new_value;
                    tram::vec3 new_value;
                    new_value_parse >> new_value.x;
                    new_value_parse >> new_value.y;
                    new_value_parse >> new_value.z;
                    // TODO: add validation
                    object->SetProperty(value_name, new_value);
                    break;
                }
                default:
                    object->SetProperty(value_name, value.GetString().ToStdString());
            }
        } else if (value.GetType() == "long") {
            // long in wxWidgets PropGrid is used for enums
            object->SetProperty(value_name, (int32_t) value.GetLong());
        } else if (value.GetType() == "bool") {
            object->SetProperty(value_name, value.GetBool());
        } else {
            std::cout << "VALUE_TYPE '" << value.GetType().c_str() << "' UNRECOGNIZED" << std::endl;
        }
    }
    
    Editor::ObjectList::Refresh();
    Editor::Viewport::Refresh();
}

void PropertyPanelCtrl::OnCollapsed (wxPropertyGridEvent& event) {
    category_is_collapsed[event.GetPropertyName().ToStdString()] = true;
}

void PropertyPanelCtrl::OnExpanded (wxPropertyGridEvent& event) {
    category_is_collapsed[event.GetPropertyName().ToStdString()] = false;
}