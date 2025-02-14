#include <wx/aboutdlg.h>

#include <editor/editor.h>
#include <editor/language.h>
#include <editor/settings.h>
#include <editor/actions.h>

#include <editor/objects/world.h>

#include <widgets/mainframe.h>
#include <widgets/objectlist.h>
#include <widgets/objectmenu.h>
#include <widgets/propertypanel.h>
#include <widgets/viewport.h>
#include <widgets/worldtree.h>
#include <widgets/dialogs.h>
#include <widgets/signaleditor.h>

enum {
    ID_Hello = 1,
    ID_New_Member = 2,
    ID_Existing_Member = 3,
    ID_Load_Cells = 4,
    ID_Save_Cells = 5,
    ID_Action_Redo = 10,
    ID_Action_Undo = 11,
    ID_Action_Signals = 12,
    ID_Action_Duplicate = 14,
    ID_Action_CenterOrigin = 15,
    ID_Action_WorldspawnOffset = 16,
    ID_Action_StashSelection = 17,
    ID_Action_ReparentStash = 18,
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
    ID_Settings_View_Move_To_Selection = 110,
    ID_Settings_Snap_0_01 = 111,
    ID_Settings_Snap_0_10 = 112,
    ID_Settings_Snap_0_25 = 113,
    ID_Settings_Snap_0_50 = 114,
    ID_Settings_Snap_1_00 = 115,
    ID_Settings_Snap_15 = 116,
    ID_Settings_Snap_30 = 117,
    ID_Settings_Snap_45 = 118,
    ID_Settings_Snap_90 = 119,
    ID_Settings_Language = 200
};

MainFrameCtrl* main_frame = nullptr;

MainFrameCtrl::MainFrameCtrl() : wxFrame(NULL, wxID_ANY, Editor::Get("title_bar"), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE) {    
    using namespace Editor::Settings;
    
    // --- MENUS ---
    wxMenu* file_menu = new wxMenu;
    file_menu->Append(ID_Load_Cells, Editor::Get("file_menu_load"), Editor::Get("file_menu_load_info"));
    file_menu->Append(ID_Save_Cells, Editor::Get("file_menu_save"), Editor::Get("file_menu_save_info"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, Editor::Get("file_menu_quit"), Editor::Get("file_menu_quit_info"));
 
    wxMenu* edit_menu = new wxMenu;
    edit_menu->Append(ID_Action_Undo, Editor::Get("edit_menu_undo"), Editor::Get("edit_menu_undo_info"));
    edit_menu->Append(ID_Action_Redo, Editor::Get("edit_menu_redo"), Editor::Get("edit_menu_undo_info"));
    edit_menu->AppendSeparator();
    edit_menu->Append(ID_Action_Duplicate, Editor::Get("edit_menu_duplicate"), Editor::Get("edit_menu_duplicate_info"));
    edit_menu->Append(ID_Action_Signals, Editor::Get("edit_menu_signals"), Editor::Get("edit_menu_signals_info"));
    edit_menu->AppendSeparator();
    edit_menu->Append(ID_Action_CenterOrigin, Editor::Get("edit_menu_center_aabb"), Editor::Get("edit_menu_center_aabb_info"));
    edit_menu->Append(ID_Action_WorldspawnOffset, Editor::Get("edit_menu_worldspawn_offset"), Editor::Get("edit_menu_worldspawn_offset_info"));
    edit_menu->AppendSeparator();
    edit_menu->Append(ID_Action_StashSelection, Editor::Get("edit_menu_stash_selection"), Editor::Get("edit_menu_stash_selection_info"));
    edit_menu->Append(ID_Action_ReparentStash, Editor::Get("edit_menu_reparent_stash"), Editor::Get("edit_menu_reparent_stash_info"));
 
    wxMenu* view_menu = new wxMenu;
    view_menu->Append(ID_Settings_View_Move_To_Selection, Editor::Get("view_menu_center_selection"), Editor::Get("view_menu_center_selection_info"));
 
    wxMenu* settings_menu = new wxMenu;
    settings_menu->AppendRadioItem(ID_Settings_Space_World, Editor::Get("settings_menu_world_space"), Editor::Get("settings_menu_world_space_info"))->Check(TRANSFORM_SPACE == SPACE_WORLD);
    settings_menu->AppendRadioItem(ID_Settings_Space_Entity, Editor::Get("settings_menu_entity_space"), Editor::Get("settings_menu_entity_space_info"))->Check(TRANSFORM_SPACE == SPACE_ENTITY);
    settings_menu->AppendRadioItem(ID_Settings_Space_Group, Editor::Get("settings_menu_entity_group_space"), Editor::Get("settings_menu_entity_group_space_info"))->Check(TRANSFORM_SPACE == SPACE_ENTITYGROUP);
    settings_menu->AppendSeparator();
    settings_menu->AppendRadioItem(ID_Settings_Angle_Radians, Editor::Get("settings_menu_radians"), Editor::Get("settings_menu_radians_info"))->Check(ROTATION_UNIT == ROTATION_RADIANS);
    settings_menu->AppendRadioItem(ID_Settings_Angle_Degrees, Editor::Get("settings_menu_degrees"), Editor::Get("settings_menu_degrees_info"))->Check(ROTATION_UNIT == ROTATION_DEGREES);
    settings_menu->AppendSeparator();
    settings_menu->AppendRadioItem(ID_Settings_Snap_0_01, Editor::Get("snap_to_0_01"), Editor::Get("snap"))->Check(TRANSLATION_SNAP == SNAP_0_01);
    settings_menu->AppendRadioItem(ID_Settings_Snap_0_10, Editor::Get("snap_to_0_10"), Editor::Get("snap"))->Check(TRANSLATION_SNAP == SNAP_0_10);
    settings_menu->AppendRadioItem(ID_Settings_Snap_0_25, Editor::Get("snap_to_0_25"), Editor::Get("snap"))->Check(TRANSLATION_SNAP == SNAP_0_25);
    settings_menu->AppendRadioItem(ID_Settings_Snap_0_50, Editor::Get("snap_to_0_50"), Editor::Get("snap"))->Check(TRANSLATION_SNAP == SNAP_0_50);
    settings_menu->AppendRadioItem(ID_Settings_Snap_1_00, Editor::Get("snap_to_1_00"), Editor::Get("snap"))->Check(TRANSLATION_SNAP == SNAP_1_00);
    settings_menu->AppendSeparator();
    settings_menu->AppendRadioItem(ID_Settings_Snap_15, Editor::Get("snap_to_15"), Editor::Get("snap"))->Check(ROTATION_SNAP == SNAP_15);
    settings_menu->AppendRadioItem(ID_Settings_Snap_30, Editor::Get("snap_to_30"), Editor::Get("snap"))->Check(ROTATION_SNAP == SNAP_30);
    settings_menu->AppendRadioItem(ID_Settings_Snap_45, Editor::Get("snap_to_45"), Editor::Get("snap"))->Check(ROTATION_SNAP == SNAP_45);
    settings_menu->AppendRadioItem(ID_Settings_Snap_90, Editor::Get("snap_to_90"), Editor::Get("snap"))->Check(ROTATION_SNAP == SNAP_90);
    
    wxMenu* language_menu = new wxMenu;
    language_menu->AppendRadioItem((int) ID_Settings_Language + LANGUAGE_LV, L"Latviešu", L"Latvijas Republikas un Roņu Salas Autonomās Teritorijas valoda.")->Check(INTERFACE_LANGUAGE == LANGUAGE_LV);
    language_menu->AppendRadioItem((int) ID_Settings_Language + LANGUAGE_EN, L"English", L"Language of United Kingdom, Canada, Australia and New Zealand.")->Check(INTERFACE_LANGUAGE == LANGUAGE_EN);
    
    wxMenu* help_menu = new wxMenu;
    help_menu->Append(ID_Hello, Editor::Get("help_menu_help"), Editor::Get("help_menu_help_info"));
    help_menu->Append(wxID_ABOUT, Editor::Get("help_menu_about"), Editor::Get("help_menu_about_info"));
 
    wxMenuBar* menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, Editor::Get("file_menu"));
    menu_bar->Append(edit_menu, Editor::Get("edit_menu"));
    menu_bar->Append(view_menu, Editor::Get("view_menu"));
    menu_bar->Append(settings_menu, Editor::Get("settings_menu"));
    menu_bar->Append(language_menu, Editor::Get("language_menu"));
    menu_bar->Append(help_menu, Editor::Get("help_menu"));

 
    SetMenuBar(menu_bar);
 
    CreateStatusBar();
    SetStatusText(Editor::Get("info_ready"));
    
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this);
    
    Bind(wxEVT_MENU, &MainFrameCtrl::OnHelp, this, ID_Hello);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnExit, this, wxID_EXIT);
    
    Bind(wxEVT_MENU, &MainFrameCtrl::OnLoadCells, this, ID_Load_Cells);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnSaveCells, this, ID_Save_Cells);
    
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_Redo);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_Undo);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_Duplicate);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_Signals);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_CenterOrigin);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_WorldspawnOffset);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_StashSelection);
    Bind(wxEVT_MENU, &MainFrameCtrl::OnAction, this, ID_Action_ReparentStash);
    
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

    m_mgr.AddPane(world_tree, wxLEFT, Editor::Get("world_tree"));
    m_mgr.AddPane(property_panel, wxLEFT, Editor::Get("property_panel"));
    m_mgr.AddPane(object_list, wxBOTTOM, Editor::Get("object_list"));
    m_mgr.AddPane(output_text_ctrl, wxBOTTOM, Editor::Get("output_text"));
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



void MainFrameCtrl::OnSaveCells(wxCommandEvent& event) {
    SaveCells();
    Editor::data_modified = false;
}

void MainFrameCtrl::OnLoadCells(wxCommandEvent& event) {
    LoadCells();
}

void MainFrameCtrl::OnClose(wxCloseEvent& event) {
    if (event.CanVeto() && Editor::data_modified) {
        wxMessageDialog confirmation (this, Editor::Get("dialog_save_data"), Editor::Get("dialog_data_loss"), wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_EXCLAMATION);
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
 


void MainFrameCtrl::OnAbout(wxCommandEvent& event) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(Editor::Get("dialog_about_name"));
    aboutInfo.SetVersion(L"v0.1.0");
    aboutInfo.SetDescription(Editor::Get("dialog_description"));
    aboutInfo.SetCopyright(Editor::Get("dialog_copyright") + L" (C) racenis 2022-2025");
    aboutInfo.SetWebSite(L"https://github.com/racenis/tram-sdk");
    wxAboutBox(aboutInfo);
}
 
void MainFrameCtrl::OnHelp(wxCommandEvent& event) {
    wxLaunchDefaultBrowser("https://racenis.github.io/tram-sdk/documentation.html");
    //wxLogMessage("Hello world from wxWidgets!");
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
        case ID_Action_Signals:
            OpenSignalEditorModal();
            break;
        case ID_Action_Duplicate:
            Editor::PerformAction<Editor::ActionDuplicate>();
            break;
        case ID_Action_CenterOrigin:
            Editor::PerformAction<Editor::ActionCenterOrigin>();
            break;
        case ID_Action_WorldspawnOffset:
            Editor::PerformAction<Editor::ActionWorldspawnOffset>();
            break;
        case ID_Action_StashSelection:
            Editor::PerformAction<Editor::ActionStashSelection>();
            break;
        case ID_Action_ReparentStash:
            Editor::PerformAction<Editor::ActionReparent>();
            break;
        case ID_Settings_Angle_Radians:
            ROTATION_UNIT = ROTATION_RADIANS;
            Editor::PropertyPanel::Refresh();
            break;
        case ID_Settings_Angle_Degrees:
            ROTATION_UNIT = ROTATION_DEGREES;
            Editor::PropertyPanel::Refresh();
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
          
        case ID_Settings_Snap_0_01: TRANSLATION_SNAP = SNAP_0_01;   break;
        case ID_Settings_Snap_0_10: TRANSLATION_SNAP = SNAP_0_10;   break;
        case ID_Settings_Snap_0_25: TRANSLATION_SNAP = SNAP_0_25;   break;
        case ID_Settings_Snap_0_50: TRANSLATION_SNAP = SNAP_0_50;   break;
        case ID_Settings_Snap_1_00: TRANSLATION_SNAP = SNAP_1_00;   break;
        
        case ID_Settings_Snap_15:   ROTATION_SNAP = SNAP_15;        break;
        case ID_Settings_Snap_30:   ROTATION_SNAP = SNAP_30;        break;
        case ID_Settings_Snap_45:   ROTATION_SNAP = SNAP_45;        break;
        case ID_Settings_Snap_90:   ROTATION_SNAP = SNAP_90;        break;
            
        case ID_Settings_View_Move_To_Selection:
            viewport->ViewCenterOnSelection();
            break;
        default:
            int selected = event.GetId() - ID_Settings_Language;
            if (selected >= 0 && selected < 100) {
                INTERFACE_LANGUAGE = (Language) selected;
                //Editor::selected_language = Editor::Languages[selected];
                //Editor::ResetRename();
                Editor::ResetLanguage();
                Editor::Settings::Save();
                wxMessageDialog info_some (this, Editor::Get("dialog_change_language"), Editor::Get("dialog_change_language_title"), wxOK | wxOK_DEFAULT | wxICON_INFORMATION);
                info_some.ShowModal();
            } else {
                std::cout << "SETTINGS_ERROR" << std::endl;
            }
    }
}
