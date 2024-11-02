
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



static const char* all_signals[15] = {
    "none",
    "spawn",
    "kill",
    "activate",
    "use",
    "locked-use",
    "open",
    "close",
    "end-open",
    "end-close",
    "trigger",
    "enter-trigger",
    "exit-trigger",
    "progress",
    "none"
};




static const char* all_messages[17] = {
    "none",
    "ping",
    "move-pick-up",
    "open",
    "close",
    "lock",
    "unlock",
    "toggle",
    "kill",
    "use",
    "start",
    "stop",
    "activate",
    "activate-once",
    "select",
    "set-progress",
    "set-animation"
};

static const char* all_types[5] = {
"none",    
"int",    
"float",
"name",
"vec3"
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
    
    this->Bind(wxEVT_CLOSE_WINDOW, &SignalEditor::Closng, this);
    
    wxSplitterWindow* splitter = new wxSplitterWindow(this, -1, wxPoint(0, 0), wxSize(400, 400), wxSP_3D);
    
    signal_list = new SignalListCtrl(splitter, this->entity);
    auto signal_box = new wxPanel(splitter);
    
    
    // all of the controls in the menu
    signal_type_value = new wxChoice(signal_box, INPUT_FIELD_SIGNAL_TYPE, wxDefaultPosition, wxSize(169, -1), {15, all_signals});
    signal_type_label = new wxStaticText(signal_box, INPUT_FIELD_SIGNAL_TYPE, "When this entity fires signal  ");
    for (int i = 0; i < 3; i++) {
        signal_type_value->SetString(i, all_signals[i]);
    }
    
    target_value = new wxTextCtrl(signal_box, INPUT_FIELD_TARGET, wxEmptyString, wxDefaultPosition, wxSize(169, -1));
    target_label = new wxStaticText(signal_box, INPUT_FIELD_TARGET, "Target entity named  ");
    delay_value = new wxTextCtrl(signal_box, INPUT_FIELD_DELAY, wxEmptyString, wxDefaultPosition, wxSize(69, -1));
    delay_label = new wxStaticText(signal_box, INPUT_FIELD_DELAY, "After a delay of  ");
    repeats_value = new wxTextCtrl(signal_box, INPUT_FIELD_REPEATS, wxEmptyString, wxDefaultPosition, wxSize(69, -1));
    repeats_label = new wxStaticText(signal_box, INPUT_FIELD_REPEATS, "  seconds and limit fires to  ");
    repeats_label2 = new wxStaticText(signal_box, INPUT_FIELD_REPEATS, "  times");
    
    message_type_value = new wxChoice(signal_box, INPUT_FIELD_MESSAGE_TYPE, wxDefaultPosition, wxSize(169, -1), {17, all_messages});
    message_type_label = new wxStaticText(signal_box, INPUT_FIELD_MESSAGE_TYPE, "By sending a message of type  ");
    for (int i = 0; i < 3; i++) {
        message_type_value->SetString(i, all_messages[i]);
    }
    
    parameter_type_value = new wxChoice(signal_box, INPUT_FIELD_PARAMETER_TYPE, wxDefaultPosition, wxSize(169, -1), {5, all_types});
    parameter_type_label = new wxStaticText(signal_box, INPUT_FIELD_PARAMETER_TYPE, "With a parameter type of  ");
    
    parameter_value = new wxTextCtrl(signal_box, INPUT_FIELD_PARAMETER, wxEmptyString, wxDefaultPosition, wxSize(169, -1));
    parameter_label = new wxStaticText(signal_box, INPUT_FIELD_PARAMETER, "And value of  ");
    
    add_new = new wxButton(signal_box, wxID_ANY, "Add New");
    delete_new = new wxButton(signal_box, wxID_ANY, "Delete Selected");
    
    // layout

    auto grid_sizer = new wxGridSizer(2);
    
    grid_sizer->SetVGap(2);
    
    grid_sizer->Add(signal_type_label, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    grid_sizer->Add(signal_type_value);
    
    grid_sizer->Add(target_label, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    grid_sizer->Add(target_value);
    
    grid_sizer->Add(message_type_label, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    grid_sizer->Add(message_type_value);
    
    grid_sizer->Add(parameter_type_label, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    grid_sizer->Add(parameter_type_value);
    
    grid_sizer->Add(parameter_label, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    grid_sizer->Add(parameter_value);
    
    auto container_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    container_sizer->Add(delay_label, 0, wxALIGN_CENTER_VERTICAL);
    container_sizer->Add(delay_value);
    container_sizer->Add(repeats_label, 0, wxALIGN_CENTER_VERTICAL);
    container_sizer->Add(repeats_value);
    container_sizer->Add(repeats_label2, 0, wxALIGN_CENTER_VERTICAL);
    
    auto button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    button_sizer->Add(add_new);
    button_sizer->Add(delete_new);
    
    auto box_sizer = new wxBoxSizer(wxVERTICAL);
    
    box_sizer->Add(grid_sizer, 0, wxALIGN_CENTER);
    box_sizer->AddSpacer(2);
    box_sizer->Add(container_sizer, 0, wxALIGN_CENTER);
    box_sizer->AddSpacer(2);
    box_sizer->Add(button_sizer, 0, wxALIGN_CENTER);
    
    
    
    
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
    
    target_value->Disable();
    delay_value->Disable();
    repeats_value->Disable();
    message_type_value->Disable();
    signal_type_value->Disable();
    parameter_type_value->Disable();
    parameter_value->Disable();
    delete_new->Disable();
    
    signal_box->SetSizer(box_sizer);
    
    splitter->SplitHorizontally(signal_list, signal_box, 182);
    splitter->SetMinimumPaneSize(165);
}

void SignalEditor::AddNew(wxCommandEvent& event) {
    entity->signals.push_back(Editor::Signal({
        .type = "none",
        .target = "none",
        .delay = 0.0f,
        .limit = -1,
        .message = "none",
        .param_type = "none",
        .param = ""
    }));
    
    signal_list->SetItemCount(entity->signals.size());
}

void SignalEditor::Delete(wxCommandEvent& event) {
    if (selected == -1) return;
    entity->signals.erase(entity->signals.begin()+selected);
    signal_list->SetItemCount(entity->signals.size());
    
    target_value->Disable();
    delay_value->Disable();
    repeats_value->Disable();
    message_type_value->Disable();
    signal_type_value->Disable();
    parameter_type_value->Disable();
    parameter_value->Disable();
    delete_new->Disable();
}

void SignalEditor::Select(wxListEvent& event) {
    selected = event.GetIndex();
    const auto& signal = entity->signals[selected];
    
    target_value->Enable();
    delay_value->Enable();
    repeats_value->Enable();
    message_type_value->Enable();
    signal_type_value->Enable();
    parameter_type_value->Enable();
    parameter_value->Enable();
    delete_new->Enable();
    
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

void SignalEditor::Closng(wxCloseEvent& event) {
    for (auto& signal : entity->signals) {
        if (signal.target == "") {
            wxMessageDialog info_some (nullptr, "There is a signal that is missing its target. Delete it or set the target.", "Signal configuration error!", wxOK | wxOK_DEFAULT | wxICON_INFORMATION);
            info_some.ShowModal(); event.Veto(); return;
        }
        
        if (signal.param_type != "none" && signal.param == "") {
            wxMessageDialog info_some (nullptr, "There is a signal that is missing its parameter. Delete it or set the parameter.", "Signal configuration error!", wxOK | wxOK_DEFAULT | wxICON_INFORMATION);
            info_some.ShowModal(); event.Veto(); return;
        }
        
        if (signal.param_type != "vec3" && signal.param.find_first_of(" \t\r\n") != std::string::npos) {
            wxMessageDialog info_some (nullptr, "There is a signal that has a borked parameter. Delete it or fix the parameter.", "Signal configuration error!", wxOK | wxOK_DEFAULT | wxICON_INFORMATION);
            info_some.ShowModal(); event.Veto(); return;
        }
        
        if (signal.param_type == "vec3") {
            int spaces = 0;
            for (auto& c : signal.param) if (isspace(c)) spaces++;
            if (spaces != 2) {
                wxMessageDialog info_some (nullptr, "There is a signal that has a borked parameter. Delete it or fix the parameter.", "Signal configuration error!", wxOK | wxOK_DEFAULT | wxICON_INFORMATION);
                info_some.ShowModal(); event.Veto(); return;
            }
        }
    }
    
    Destroy();
}

class ActionChangeSignals : public Editor::Action {
public:
    ActionChangeSignals (std::shared_ptr<Editor::Entity> entity) {
        this->signals = entity->signals;
        this->entity = entity;
    }
    
    void Perform() {
        std::swap(entity->signals, signals);
    }
    
    void Unperform() {
        std::swap(entity->signals, signals);
    }
    
    std::vector<Editor::Signal> signals;
    std::shared_ptr<Editor::Entity> entity;
};


void OpenSignalEditorModal() {
    if (Editor::SELECTION->objects.size() != 1) {
        wxMessageDialog info_some (nullptr, "Need to select at least 1 entity and no more.", "Can't edit signals!", wxOK | wxOK_DEFAULT | wxICON_INFORMATION);
        info_some.ShowModal();
    } else if (std::dynamic_pointer_cast<Editor::Entity>(Editor::SELECTION->objects.front()).get() == nullptr) { // cursed
        wxMessageDialog info_some (nullptr, "Selected object is not entity.", "Can't edit signals!", wxOK | wxOK_DEFAULT | wxICON_INFORMATION);
        info_some.ShowModal();
    } else {
        Editor::PerformAction<ActionChangeSignals>(std::dynamic_pointer_cast<Editor::Entity>(Editor::SELECTION->objects.front()));
        SignalEditor editor;
        editor.ShowModal();
    }
}