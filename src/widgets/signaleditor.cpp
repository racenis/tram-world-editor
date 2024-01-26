
#include <widgets/signaleditor.h>

#include <wx/splitter.h>


#include <wx/listctrl.h>

class SignalListCtrl : public wxListCtrl {
public:
    SignalListCtrl(wxWindow* parent, std::shared_ptr<Editor::Entity> entity) : wxListCtrl(parent, -1, wxDefaultPosition, wxSize(200,150), wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES), entity(entity) {
        Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &SignalListCtrl::OnMenuOpen, this);
        //Bind(wxEVT_LIST_ITEM_SELECTED, &SignalListCtrl::OnSelectionChanged, this);
        Bind(wxEVT_LIST_ITEM_ACTIVATED, &SignalListCtrl::OnItemActivated, this);

        this->InsertColumn(0, "Type");
        this->InsertColumn(1, "Target");
        this->InsertColumn(2, "Delay");
        this->InsertColumn(3, "Limit");
        this->InsertColumn(4, "Message");
        this->InsertColumn(5, "Parameter");
        this->InsertColumn(6, "Value");
        
        this->SetItemCount(entity->signals.size());
    }

    wxString OnGetItemText (long item, long column) const override {
        switch(column) {
            case 0: return entity->signals[item].type;
            case 1: return entity->signals[item].target;
            case 2: return std::to_string(entity->signals[item].delay);
            case 3: return std::to_string(entity->signals[item].limit);
            case 4: return entity->signals[item].message;
            case 5: return entity->signals[item].param_type;
            case 6: return entity->signals[item].param;
            default: return "what";
        }
        
    }
    
    void RefreshAllItems() {
        
    }
    
    void OnMenuOpen(wxListEvent& event) {
        
    }
    
    void OnSelectionChanged(wxListEvent& event) {
        std::cout << "CHANGED" << std::endl;
    }
    
    void OnItemActivated(wxListEvent& event) {
        std::cout << "ACTIVATEd" << std::endl;
    }
    
    std::shared_ptr<Editor::Entity> entity;
};

static const char* all_signals[3] = {
"open",    
"close",    
"poop"    
};

static const char* all_messages[3] = {
"open",    
"close",    
"poop"    
};

static const char* all_types[3] = {
"none",    
"int",    
"float"    
};

enum {
    INPUT_FIELD_SIGNAL_TYPE=200,
    INPUT_FIELD_TARGET,
    INPUT_FIELD_DELAY,
    INPUT_FIELD_REPEATS,
    INPUT_FIELD_MESSAGE_TYPE,
    INPUT_FIELD_PARAMETER_TYPE,
    INPUT_FIELD_PARAMETER,
};

SignalEditor::SignalEditor() : wxDialog(NULL, wxID_ANY, "Signal Editor", wxDefaultPosition, wxSize(600, 400), wxDEFAULT_FRAME_STYLE) {
    this->entity = std::dynamic_pointer_cast<Editor::Entity>(Editor::SELECTION->objects.front());
    
    
    wxSplitterWindow* splitter = new wxSplitterWindow(this, -1, wxPoint(0, 0), wxSize(400, 400), wxSP_3D);
    
    signal_list = new SignalListCtrl(splitter, this->entity);
    auto signal_box = new wxPanel(splitter);
    
    auto box_sizer = new wxBoxSizer(wxVERTICAL);
    
    signal_type_value = new wxChoice(signal_box, INPUT_FIELD_SIGNAL_TYPE, wxDefaultPosition, wxDefaultSize, {3, all_signals});
    signal_type_label = new wxStaticText(signal_box, INPUT_FIELD_SIGNAL_TYPE, "Signal");
    for (int i = 0; i < 3; i++) {
        signal_type_value->SetString(i, all_signals[i]);
    }
    
    target_value = new wxTextCtrl(signal_box, INPUT_FIELD_TARGET);
    target_label = new wxStaticText(signal_box, INPUT_FIELD_TARGET, "Target");
    delay_value = new wxTextCtrl(signal_box, INPUT_FIELD_DELAY);
    delay_label = new wxStaticText(signal_box, INPUT_FIELD_DELAY, "Delay");
    repeats_value = new wxTextCtrl(signal_box, INPUT_FIELD_REPEATS);
    repeats_label = new wxStaticText(signal_box, INPUT_FIELD_REPEATS, "Repeats");
    
    message_type_value = new wxChoice(signal_box, INPUT_FIELD_MESSAGE_TYPE, wxDefaultPosition, wxDefaultSize, {3, all_signals});
    message_type_label = new wxStaticText(signal_box, INPUT_FIELD_MESSAGE_TYPE, "Message");
    for (int i = 0; i < 3; i++) {
        message_type_value->SetString(i, all_messages[i]);
    }
    
    parameter_type_value = new wxChoice(signal_box, INPUT_FIELD_PARAMETER_TYPE, wxDefaultPosition, wxDefaultSize, {3, all_types});
    parameter_type_label = new wxStaticText(signal_box, INPUT_FIELD_PARAMETER_TYPE, "Value");
    
    parameter_value = new wxTextCtrl(signal_box, INPUT_FIELD_PARAMETER);
    parameter_label = new wxStaticText(signal_box, INPUT_FIELD_PARAMETER, "Value");
    
    auto add_new = new wxButton(signal_box, wxID_ANY, "Add New");
    auto delete_new = new wxButton(signal_box, wxID_ANY, "Delete Selected");
    
    
    
    box_sizer->Add(signal_type_label);
    box_sizer->Add(signal_type_value);
    
    box_sizer->Add(target_label);
    box_sizer->Add(target_value);
    box_sizer->Add(delay_label);
    box_sizer->Add(delay_value);
    box_sizer->Add(repeats_label);
    box_sizer->Add(repeats_value);
    
    box_sizer->Add(message_type_label);
    box_sizer->Add(message_type_value);
    
    box_sizer->Add(parameter_type_label);
    box_sizer->Add(parameter_type_value);
    
    box_sizer->Add(parameter_label);
    box_sizer->Add(parameter_value);
    
    box_sizer->Add(add_new);
    box_sizer->Add(delete_new);
    
    add_new->Bind(wxEVT_BUTTON, &SignalEditor::AddNew, this);
    delete_new->Bind(wxEVT_BUTTON, &SignalEditor::Delete, this);
    signal_list->Bind(wxEVT_LIST_ITEM_SELECTED, &SignalEditor::Select, this);
    
    target_value->Bind(wxEVT_TEXT, &SignalEditor::Change, this);
    delay_value->Bind(wxEVT_TEXT, &SignalEditor::Change, this);
    repeats_value->Bind(wxEVT_TEXT, &SignalEditor::Change, this);
    message_type_value->Bind(wxEVT_CHOICE, &SignalEditor::Change, this);
    signal_type_value->Bind(wxEVT_CHOICE, &SignalEditor::Change, this);
    parameter_type_value->Bind(wxEVT_CHOICE, &SignalEditor::Change, this);
    parameter_value->Bind(wxEVT_TEXT, &SignalEditor::Change, this);
    
    signal_box->SetSizer(box_sizer);
    
    splitter->SplitHorizontally(signal_list, signal_box);
    
}

void SignalEditor::AddNew(wxCommandEvent& event) {
    std::cout << "aDDED NEW" << std::endl;
}

void SignalEditor::Delete(wxCommandEvent& event) {
    std::cout << "DOLOT" << std::endl;
}

void SignalEditor::Select(wxListEvent& event) {
    selected = event.GetIndex();
    const auto& signal = entity->signals[selected];
    
    target_value->SetValue(signal.target);
    delay_value->SetValue(std::to_string(signal.delay));
    repeats_value->SetValue(std::to_string(signal.limit));
    message_type_value->SetSelection(message_type_value->FindString(signal.message));
    signal_type_value->SetSelection(signal_type_value->FindString(signal.type));
    parameter_type_value->SetSelection(parameter_type_value->FindString(signal.param_type));
    parameter_value->SetValue(signal.param);
}

void SignalEditor::Change(wxCommandEvent& event) {
    if (selected == -1) return;
    
    std::cout << "change!! " << event.GetId() << " " << event.GetSelection() << std::endl;
    switch (event.GetId()) {
        case INPUT_FIELD_SIGNAL_TYPE:
            entity->signals[selected].type = signal_type_value->GetString(event.GetSelection());
            break;
        case INPUT_FIELD_TARGET:
            entity->signals[selected].target = target_value->GetValue();
            break;
        case INPUT_FIELD_DELAY:
            {double num; bool succ = delay_value->GetValue().ToDouble(&num);
            if (succ) entity->signals[selected].delay = num; }
            break;
        case INPUT_FIELD_REPEATS:
            {int num; bool succ = repeats_value->GetValue().ToInt(&num);
            if (succ) entity->signals[selected].limit = num; }
            break;
        case INPUT_FIELD_MESSAGE_TYPE:
            entity->signals[selected].message = message_type_value->GetString(event.GetSelection());
            break;
        case INPUT_FIELD_PARAMETER_TYPE:
            entity->signals[selected].param_type = parameter_type_value->GetString(event.GetSelection());
            break;
        case INPUT_FIELD_PARAMETER:
            entity->signals[selected].param = parameter_value->GetValue();
            break;
    }
    
    signal_list->RefreshItem(event.GetSelection());
}