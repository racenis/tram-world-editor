#include <wx/aboutdlg.h>
#include <wx/progdlg.h>

#include <editor/editor.h>
#include <editor/language.h>

#include <widgets/mainframe.h>
#include <widgets/objectlist.h>
#include <widgets/objectmenu.h>
#include <widgets/propertypanel.h>
#include <widgets/viewport.h>
#include <widgets/worldtree.h>

enum {
    ID_Hello = 1,
    ID_New_Member = 2,
    ID_Existing_Member = 3,
    ID_Load_Cells = 4,
    ID_Save_Cells = 5,
    ID_Action_Redo = 10,
    ID_Action_Undo = 11,
    ID_World_Tree_Show = 40,
    ID_World_Tree_Hide = 41,
    ID_World_Tree_Begonis = 42,
    ID_World_Tree_Delete = 43,
    ID_World_Tree_New = 44,
    ID_World_Tree_Edit = 45,
    ID_Entity_List_New = 50,
    ID_Entity_List_Edit = 51,
    ID_Entity_List_Delete = 52,
    ID_Entity_Change_Type = 100,
    ID_Settings_Angle_Radians = 105,
    ID_Settings_Angle_Degrees = 106,
    ID_Settings_Space_World = 107,
    ID_Settings_Space_Entity = 108,
    ID_Settings_Space_Group = 109,
    ID_Settings_Language = 200
};

MainFrameCtrl* main_frame = nullptr;

auto& lang = Editor::selected_language;

MainFrameCtrl::MainFrameCtrl() : wxFrame(NULL, wxID_ANY, lang->title_bar, wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE) {    
    using namespace Editor::Settings;
    
    // --- MENUS ---
    wxMenu* file_menu = new wxMenu;
    file_menu->Append(ID_Load_Cells, lang->file_menu_load, lang->file_menu_load_info);
    file_menu->Append(ID_Save_Cells, lang->file_menu_save, lang->file_menu_save_info);
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, lang->file_menu_quit, lang->file_menu_quit_info);
 
    wxMenu* edit_menu = new wxMenu;
    edit_menu->Append(ID_Action_Undo, lang->edit_menu_undo, lang->edit_menu_undo_info);
    edit_menu->Append(ID_Action_Redo, lang->edit_menu_redo, lang->edit_menu_undo_info);
 
    wxMenu* settings_menu = new wxMenu;
    settings_menu->AppendRadioItem(ID_Settings_Space_World, lang->settings_menu_world_space, lang->settings_menu_world_space_info)->Check(TRANSFORM_SPACE == SPACE_WORLD);
    settings_menu->AppendRadioItem(ID_Settings_Space_Entity, lang->settings_menu_entity_space, lang->settings_menu_entity_space_info)->Check(TRANSFORM_SPACE == SPACE_ENTITY);
    settings_menu->AppendRadioItem(ID_Settings_Space_World, lang->settings_menu_entity_group_space, lang->settings_menu_entity_group_space_info)->Check(TRANSFORM_SPACE == SPACE_ENTITYGROUP);
    settings_menu->AppendSeparator();
    settings_menu->AppendRadioItem(ID_Settings_Angle_Radians, lang->settings_menu_radians, lang->settings_menu_radians_info)->Check(ROTATION_UNIT == ROTATION_RADIANS);
    settings_menu->AppendRadioItem(ID_Settings_Angle_Degrees, lang->settings_menu_degrees, lang->settings_menu_degrees_info)->Check(ROTATION_UNIT == ROTATION_DEGREES);
    
    wxMenu* language_menu = new wxMenu;
    language_menu->AppendRadioItem((int) ID_Settings_Language + LANGUAGE_LV, L"Latviešu", L"Latvijas Republikas un Roņu Salas Autonomās Teritorijas valoda.")->Check(INTERFACE_LANGUAGE == LANGUAGE_LV);
    language_menu->AppendRadioItem((int) ID_Settings_Language + LANGUAGE_EN, L"English", L"Language of United Kingdom, Canada, Australia and New Zealand.")->Check(INTERFACE_LANGUAGE == LANGUAGE_EN);
    
    wxMenu* help_menu = new wxMenu;
    help_menu->Append(ID_Hello, lang->help_menu_hello, lang->help_menu_hello_info);
    help_menu->Append(wxID_ABOUT, lang->help_menu_about, lang->help_menu_about_info);
 
    wxMenuBar* menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, lang->file_menu);
    menu_bar->Append(edit_menu, lang->edit_menu);
    menu_bar->Append(settings_menu, lang->settings_menu);
    menu_bar->Append(language_menu, lang->language_menu);
    menu_bar->Append(help_menu, lang->help_menu);

 
    SetMenuBar(menu_bar);
 
    CreateStatusBar();
    SetStatusText(lang->info_ready);
    
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this);
    
    Bind(wxEVT_MENU, &MainFrameCtrl::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnExit, this, wxID_EXIT);
    
    Bind(wxEVT_MENU, &MainFrameCtrl::OnLoadCells, this, ID_Load_Cells);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnSaveCells, this, ID_Save_Cells);
    
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_Redo);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_Undo);
    
    Bind(wxEVT_CLOSE_WINDOW, &MainFrameCtrl::OnClose, this);
    
    // --- POP-UP MENUS ---
    world_tree_popup = new ObjectMenuCtrl;
    
    wxTextCtrl* output_text_ctrl = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(200,150), wxNO_BORDER | wxTE_READONLY | wxTE_MULTILINE);
    std_cout_redirect = new wxStreamToTextRedirector(output_text_ctrl);
    object_list = new ObjectListCtrl(this);
    property_panel = new PropertyPanelCtrl(this);
    world_tree = new WorldTreeCtrl(this);
    viewport = new ViewportCtrl(this);
    
    m_mgr.SetManagedWindow(this);

    m_mgr.AddPane(world_tree, wxLEFT, lang->world_tree);
    m_mgr.AddPane(property_panel, wxLEFT, lang->property_panel);
    m_mgr.AddPane(object_list, wxBOTTOM, lang->object_list);
    m_mgr.AddPane(output_text_ctrl, wxBOTTOM, lang->output_text);
    m_mgr.AddPane(viewport, wxCENTER, L"HUMONGOUS");
    
    m_mgr.GetPane(world_tree).CloseButton(false);
    m_mgr.GetPane(property_panel).CloseButton(false);
    m_mgr.GetPane(object_list).CloseButton(false);
    m_mgr.GetPane(output_text_ctrl).CloseButton(false);

    // --- AUI FLAGS ---
    unsigned int flags = m_mgr.GetFlags();
    flags = flags ^ wxAUI_MGR_TRANSPARENT_HINT;
    flags = flags | wxAUI_MGR_RECTANGLE_HINT;
    m_mgr.SetFlags(flags);
    m_mgr.Update();
    
    // --- PANEL INITS ---
    //Editor::ProduceTestData();
    Editor::WorldTree::Rebuild();
}

void SaveCells() {
    wxProgressDialog progress_dialog (lang->dialog_saving_title, lang->dialog_saving_info, 100, main_frame);
    
    auto progress = 0;
    auto cells = Editor::worldcells->GetChildren();
    auto progress_increment = (100 / cells.size()) - 1;
    for (auto& wcell : cells) {
        progress_dialog.Update(progress, lang->dialog_saving_cell + std::string(wcell->GetName()));
        auto cell = std::dynamic_pointer_cast<Editor::WorldCell>(wcell);
        SaveCell(cell.get());
        progress += progress_increment;
    }
    
    Editor::Settings::Save();
    
    progress_dialog.Update(100, lang->dialog_finished);
    
    std::cout << lang->dialog_finished << std::endl;
}



void MainFrameCtrl::OnClose(wxCloseEvent& event) {
    if (event.CanVeto() && Editor::data_modified) {
        wxMessageDialog confirmation (this, lang->dialog_save_data, lang->dialog_data_loss, wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_EXCLAMATION);
        auto result = confirmation.ShowModal();
        
        if(result == wxID_YES) {
            SaveCells();
        }
        
        if (result == wxID_CANCEL) {
            event.Veto();
            return;
        }
    }
    
    Destroy();
}

void MainFrameCtrl::OnExit(wxCommandEvent& event) {
    Close();
}
 


void MainFrameCtrl::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(lang->dialog_about_name);
    aboutInfo.SetVersion(L"v0.0.3");
    aboutInfo.SetDescription(lang->dialog_description);
    aboutInfo.SetCopyright(lang->dialog_copyright + L" (C) Lielais Jānis Dambergs 2022");
    aboutInfo.SetWebSite(L"https://github.com/racenis/tram-sdk");
    wxAboutBox(aboutInfo);
}
 
void MainFrameCtrl::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

void MainFrameCtrl::OnAction(wxCommandEvent& event) {
    using namespace Editor::Settings;
    switch (event.GetId()) {
        case ID_Action_Redo:
            Editor::Redo();
            break;
        case ID_Action_Undo:
            Editor::Undo();
            break;
        case ID_Settings_Angle_Radians:
            ROTATION_UNIT = ROTATION_RADIANS;
            break;
        case ID_Settings_Angle_Degrees:
            ROTATION_UNIT = ROTATION_DEGREES;
            break;
        case ID_Settings_Space_World:
            TRANSFORM_SPACE = SPACE_WORLD;
            break;
        case ID_Settings_Space_Entity:
            TRANSFORM_SPACE = SPACE_ENTITY;
            break;
        case ID_Settings_Space_Group:
            TRANSFORM_SPACE = SPACE_ENTITYGROUP;
            break;
        default:
            int selected = event.GetId() - ID_Settings_Language;
            if (selected >= 0 && selected < 100) {
                INTERFACE_LANGUAGE = (Language) selected;
                Editor::selected_language = Editor::Languages[selected];
                Editor::ResetRename();
                Editor::Settings::Save();
                wxMessageDialog info_some (this, lang->dialog_change_language, lang->dialog_change_language_title, wxOK | wxOK_DEFAULT | wxICON_INFORMATION);
                info_some.ShowModal();
            } else {
                std::cout << "SETTINGS_ERROR" << std::endl;
            }
    }
}

void MainFrameCtrl::OnLoadCells(wxCommandEvent& event) {
    if (Editor::data_modified) {
        wxMessageDialog confirmation (this, lang->dialog_load_data, lang->dialog_data_loss, wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_EXCLAMATION);
        if(confirmation.ShowModal() != wxID_YES) {
            return;
        }
    }
    
    Editor::Reset();
        
    wxProgressDialog progress_dialog (lang->dialog_loading_title, lang->dialog_loading_info, 100, this);
    
    auto progress = 0;
    auto cells = Editor::worldcells->GetChildren();
    auto progress_increment = (100 / cells.size()) - 1;
    for (auto& wcell : cells) {
        progress_dialog.Update(progress, lang->dialog_loading_cell + std::string(wcell->GetName()));
        auto cell = std::dynamic_pointer_cast<Editor::WorldCell>(wcell);
        LoadCell(cell.get());
        progress += progress_increment;
    }
    
    Editor::WorldTree::Rebuild();
    
    progress_dialog.Update(100, lang->dialog_finished);
    
    std::cout << lang->dialog_finished << std::endl;
}

void MainFrameCtrl::OnSaveCells(wxCommandEvent& event) {
    SaveCells();
    Editor::data_modified = false;
}