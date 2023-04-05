#include <editor/editor.h>
#include <editor/language.h>
#include <editor/actions.h>

#include <widgets/propertypanel.h>

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
                            field_ptr = new wxStringProperty(Editor::PropertyRename(field.display_name), field.name);
                            break;
                        case PROPERTY_FLOAT:
                            field_ptr = new wxFloatProperty(Editor::PropertyRename(field.display_name), field.name);
                            break;
                        case PROPERTY_INT:
                            field_ptr = new wxIntProperty(Editor::PropertyRename(field.display_name), field.name);
                            break;
                        case PROPERTY_UINT:
                            field_ptr = new wxUIntProperty(Editor::PropertyRename(field.display_name), field.name);
                            break;
                        case PROPERTY_ENUM:
                            {
                                wxPGChoices choices;
                                for (auto& enumeration: PROPERTY_ENUMERATIONS[field.name]) choices.Add(Editor::PropertyRename(enumeration));
                                field_ptr = new wxEnumProperty(Editor::PropertyRename(field.display_name), field.name, choices);
                            }
                            break;
                        case PROPERTY_BOOL:
                            field_ptr = new wxBoolProperty(Editor::PropertyRename(field.display_name), field.name);
                            field_ptr->SetAttribute("UseCheckbox", 1);
                            break;
                        case PROPERTY_CATEGORY:
                            field_ptr = new wxPropertyCategory(Editor::PropertyRename(field.display_name), field.name);
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
                    field->SetValue(Editor::PropertyRename(value.str_value));
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
                case PROPERTY_ENUM:
                    field->SetValue((long) value.uint_value);
                    break;
                case PROPERTY_BOOL:
                    field->SetValue(value.bool_value);
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
    
    // TODO: cache the result of value.GetType() == "type"
    for (auto& object : Editor::SELECTION->objects) {
        if (value.GetType() == "longlong") {
            object->SetProperty(value_name, value.GetLongLong().GetValue());
        } else if (value.GetType() == "ulonglong") {
            object->SetProperty(value_name, value.GetULongLong().GetValue());
        } else if (value.GetType() == "double") {
            object->SetProperty(value_name, (float) value.GetDouble());
        } else if (value.GetType() == "string") {
            object->SetProperty(value_name, value.GetString().ToStdString());
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