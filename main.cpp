#ifndef NEKAD
#include <glad.h>

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/propgrid/propgrid.h>
#include <wx/aui/aui.h>
#include <wx/aboutdlg.h>
#include <wx/glcanvas.h>

#include <editor.h>

#include <core.h>
#include <async.h>
#include <render.h>

#include <entitycomponents.h>

class Viewport : public wxGLCanvas 
{
    public:
        Viewport(wxWindow* parent, wxWindowID id = wxID_ANY, 
            const int* attribList = 0,
            const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0L, 
            const wxString& name = L"GLCanvas", 
            const wxPalette& palette = wxNullPalette);
        virtual ~Viewport();
        Viewport(const Viewport& tc) = delete;
        Viewport(Viewport&& tc) = delete;
        Viewport& operator=(const Viewport& tc) = delete; 
        Viewport& operator=(Viewport&& tc) = delete; 
    private:
            void OnPaint(wxPaintEvent& event);
            void OnRightClick(wxMouseEvent& event);
            void OnMouseMove(wxMouseEvent& event);
            void OnSizeChange(wxSizeEvent& event);
            void OnKeydown(wxKeyEvent& event);
            void OnKeyup(wxKeyEvent& event);
            void OnTimer(wxTimerEvent& event);
            
            // stuff for viewport navigation
            bool mouse_captured = false;
            float mouse_x = 0;
            float mouse_y = 0;
            bool key_forward = false;
            bool key_backward = false;
            bool key_left = false;
            bool key_right = false;
            wxTimer key_timer;

            wxGLContext* m_context;
            
            
            Core::RenderComponent* monguser;
            Core::LightComponent* lit;
};

Viewport::Viewport(wxWindow* parent, wxWindowID id, 
        const int* attribList, const wxPoint& pos, const wxSize& size,
        long style, const wxString& name, const wxPalette& palette)
	: wxGLCanvas(parent, id, attribList, pos, size, style, name, palette),
    key_timer(this)
{
	m_context = new wxGLContext(this);
	Bind(wxEVT_PAINT, &Viewport::OnPaint, this);
    Bind(wxEVT_LEFT_UP, &Viewport::OnRightClick, this);
    Bind(wxEVT_MOTION, &Viewport::OnMouseMove, this);
    Bind(wxEVT_SIZE, &Viewport::OnSizeChange, this);
    Bind(wxEVT_KEY_DOWN, &Viewport::OnKeydown, this);
    Bind(wxEVT_KEY_UP, &Viewport::OnKeyup, this);
    Bind(wxEVT_TIMER, &Viewport::OnTimer, this);
	
	SetCurrent(*m_context);
	
    gladLoadGL();

    std::cout << "OPENGL VERSION:" << std::endl << glGetString(GL_VERSION) << std::endl;
    
    using namespace Core;
    using namespace Core::Render;
    Core::Init();
    Core::Render::Init();
    Core::Async::Init();
    
    Material::SetErrorMaterial(new Material(UID("defaulttexture"), Material::TEXTURE));
    Model::SetErrorModel(new Model(UID("errorstatic")));

    LoadText("data/lv.lang");
    
    Material::LoadMaterialInfo("data/texture.list");
    
    // transition for the demo level
    auto demo_trans = PoolProxy<WorldCell::Transition>::New();
    demo_trans->AddPoint(glm::vec3(-5.0f, 0.0f, -5.0f));
    demo_trans->AddPoint(glm::vec3(5.0f, 0.0f, -5.0f));
    demo_trans->AddPoint(glm::vec3(-5.0f, 0.0f, 5.0f));
    demo_trans->AddPoint(glm::vec3(5.0f, 0.0f, 5.0f));
    demo_trans->AddPoint(glm::vec3(-5.0f, 5.0f, -5.0f));
    demo_trans->AddPoint(glm::vec3(5.0f, 5.0f, -5.0f));
    demo_trans->AddPoint(glm::vec3(-5.0f, 5.0f, 5.0f));
    demo_trans->AddPoint(glm::vec3(5.0f, 5.0f, 5.0f));
    
    demo_trans->GeneratePlanes();
    
    // create the mongus model
    monguser = PoolProxy<RenderComponent>::New();
    monguser->SetModel(UID("mongus"));
    monguser->SetPose(poseList.begin());
    monguser->Init();
    monguser->UpdateLocation(glm::vec3(0.0f, 10.0f, 0.0f));
    monguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create a light
    lit = PoolProxy<LightComponent>::New();;
    lit->Init();
    lit->UpdateColor(255.0f, 0.0f, 255.0f);
    lit->UpdateDistance(1000.0f);
    
    CAMERA_POSITION = glm::vec3(0.0f, 2.0f, -5.0f);
    CAMERA_ROTATION = glm::quat(glm::vec3(0.0f, 3.14f, 0.0f));
}

Viewport::~Viewport()
{
    Core::Async::Yeet();
	SetCurrent(*m_context);
}

void Viewport::OnRightClick(wxMouseEvent& event) {
    if (mouse_captured) {
        mouse_captured = false;
        ReleaseMouse();
        std::cout << "released" << std::endl;
    } else {
        mouse_captured = true;
        CaptureMouse();
        
        // put the pointer in the center of viewport
        int width, height;
        GetSize(&width, &height);
        WarpPointer(width/2, height/2);
        std::cout << "captured" << std::endl;
    }
}

void Viewport::OnMouseMove(wxMouseEvent& event) {
    if (mouse_captured) {
        int width, height;
        GetSize(&width, &height);
        int center_x = width/2, center_y = height/2;
        mouse_x += (float)(event.GetX() - center_x) * 0.1f;
        mouse_y += (float)(event.GetY() - center_y) * 0.1f;
        WarpPointer(center_x, center_y);
        
        mouse_y = mouse_y > 90.0f ? 90.0f : mouse_y < -90.0f ? -90.0f : mouse_y;
        Core::Render::CAMERA_ROTATION = glm::quat(glm::vec3(-glm::radians(mouse_y), -glm::radians(mouse_x), 0.0f));
        
        Refresh();
        //std::cout << event.GetX() << " " << event.GetY() << std::endl;
    }
}

void Viewport::OnKeydown(wxKeyEvent& event) {
    bool movement_key_pressed = false;
    bool any_movement = key_forward || key_backward || key_left  || key_right;
    if (event.GetUnicodeKey() == 'W') {
        movement_key_pressed = true;
        key_forward = true;
    }
    
    if (event.GetUnicodeKey() == 'A') {
        movement_key_pressed = true;
        key_left = true;
    }
    
    if (event.GetUnicodeKey() == 'S') {
        movement_key_pressed = true;
        key_backward = true;
    }
    
    if (event.GetUnicodeKey() == 'D') {
        movement_key_pressed = true;
        key_right = true;
    }
    
    //std::cout << "hello this is keydown " << (char)event.GetUnicodeKey() << std::endl;
    
    if (!any_movement && movement_key_pressed) {
        key_timer.Start(30);
    }
}

void Viewport::OnKeyup(wxKeyEvent& event) {
    if (event.GetUnicodeKey() == 'W') {
        key_forward = false;
    }
    
    if (event.GetUnicodeKey() == 'A') {
        key_left = false;
    }
    
    if (event.GetUnicodeKey() == 'S') {
        key_backward = false;
    }
    
    if (event.GetUnicodeKey() == 'D') {
        key_right = false;
    }
    
    if (!(key_forward || key_backward || key_left  || key_right)) {
        key_timer.Stop();
        std::cout << "stoppe" << std::endl;
    }
}

void Viewport::OnTimer(wxTimerEvent& event) {
    std::cout << "movin " << key_forward << std::endl;
    using namespace Core::Render;
    
    if (key_forward) CAMERA_POSITION += CAMERA_ROTATION * CAMERA_FORWARD * 0.1f;
    if (key_backward) CAMERA_POSITION -= CAMERA_ROTATION * CAMERA_FORWARD * 0.1f;
    if (key_left) CAMERA_POSITION -= CAMERA_ROTATION * CAMERA_SIDE * 0.1f;
    if (key_right) CAMERA_POSITION += CAMERA_ROTATION * CAMERA_SIDE * 0.1f;
    
    Refresh();
}

void Viewport::OnSizeChange(wxSizeEvent& event) {
    int width, height;
    GetSize(&width, &height);
    glViewport(0, 0, width, height);
    Core::Render::ScreenSize(width, height);
}

void Viewport::OnPaint(wxPaintEvent& event)
{
	SetCurrent(*m_context);

	// set background to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    
    using namespace Core;
    using namespace Core::Render;
    
    SetSun(time_of_day); // this sets the ambient lighting

    static int tick = 0;
    tick++;
    
    //CAMERA_POSITION = glm::vec3(0.0f, 2.0f, -5.0f);
    //CAMERA_ROTATION = glm::quat(glm::vec3(0.0f, 3.14f, 0.0f));
    
    // this will make the light spin
    lit->UpdateLocation(cos(((float)tick) / 60.0f) * 100.0f, 0.01 ,sin(((float)tick) / 60.0f) * 100.0f);
    
    // this makes the mongus model bob up and down
    monguser->UpdateLocation(glm::vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.1f, 0.0f));
    
    Async::ResourceLoader2ndStage();
    Async::FinishResource();
    
    Render::UpdateArmatures();
    Render::Render();
    
	glFlush();
	SwapBuffers();
}




enum {
    ID_Hello = 1,
    ID_New_Member = 2,
    ID_Existing_Member = 3,
    ID_Load_Cells = 4,
    ID_Save_Cells = 5,
    ID_World_Tree_Show = 40,
    ID_World_Tree_Hide = 41,
    ID_World_Tree_Begonis = 42,
    ID_World_Tree_Delete = 43,
    ID_World_Tree_New = 44,
    ID_World_Tree_Edit = 45,
    ID_Entity_List_New = 50,
    ID_Entity_List_Edit = 51,
    ID_Entity_List_Delete = 52,
};

class EntityList;

class MainFrame : public wxFrame {
public:
    MainFrame();
    ~MainFrame()
    {
        delete std_cout_redirect;
        // deinitialize the frame manager
        m_mgr.UnInit();
    }
protected:
    wxAuiManager m_mgr;
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
    // world/worldcell tree is the tree in the top-left pane
    // that contains a list of all of the world-cells
    wxTreeCtrl* world_tree;
    wxMenu* world_tree_cell_popup;
    wxMenu* world_tree_item_popup;
    wxMenu* world_tree_root_popup;
    wxTreeItemId world_tree_selected_item;
    wxTreeItemId world_tree_root_node;
    std::unordered_map<void*, Editor::WorldCellIndirector> world_tree_map;
    void RebuildWorldCellTree();
    void OnWorldCellTreeSelect(wxTreeEvent& event);
    void OnWorldCellTreeActivate(wxTreeEvent& event);
    void OnWorldCellTreePopupSelect(wxCommandEvent& event);
    
    // property panel stuff
    wxPropertyGrid* property_panel;
    Editor::WorldCell* selected_worldcell;
    Editor::Entity* selected_entity;
    void PropertyPanelClear();
    void PropertyPanelSetWorldCell(Editor::WorldCell* cell);
    void PropertyPanelSetEntity(Editor::Entity* cell);
    void OnPropertyPanelChanged(wxPropertyGridEvent& event);
    
    // entity list stuff
    EntityList* entity_list;
    wxMenu* entity_list_popup;
    long entity_list_selected_item;
    void OnEntityListSelect(wxListEvent& event);
    void OnEntityListActivate(wxListEvent& event);
    void OnEntityListPopupSelect(wxCommandEvent& event);
    
    wxStreamToTextRedirector* std_cout_redirect;
    
    friend class EntityList;
};

class TramEditor : public wxApp {
public:
    bool OnInit() {
        MainFrame *frame = new MainFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(TramEditor);

class EntityList : public wxListCtrl {
public:
    EntityList (wxWindow* parent, wxWindowID id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxLC_ICON) : wxListCtrl(parent, id, pos, size, style) {}
    
    wxString OnGetItemText (long item, long column) const override {
        if (selected_worldcell) {
            auto ent = selected_worldcell->entities[item];
            switch (column) {
                case 0:
                    return wxString(std::to_string(ent->id));
                case 1:
                    return wxString(ent->name);
                case 2:
                    return wxString("(") + std::to_string(ent->location[0]) + ", " + std::to_string(ent->location[1]) + ", " + std::to_string(ent->location[2]) + ")";
                case 3:
                    return wxString("(") + std::to_string(ent->rotation[0]) + ", " + std::to_string(ent->rotation[1]) + ", " + std::to_string(ent->rotation[2]) + ")";
                case 4:
                    return wxString(ent->action);
                default:
                    return wxString("benis");
            }
        } else {
            return wxString("nil");
        }
    }
    
    void SetClear() {
        DeleteAllColumns();
        selected_worldcell = nullptr;
    }
    
    void SetWorldCell(Editor::WorldCell* cell) {
        SetClear();
        selected_worldcell = cell;
        InsertColumn(0, L"ID");
        InsertColumn(1, L"Nosaukums");
        InsertColumn(2, L"Lokācija");
        InsertColumn(3, L"Rotācija");
        InsertColumn(4, L"Darbība");
        SetItemCount(selected_worldcell->entities.size());
    }
    
    void New (MainFrame* frame) {
        if (selected_worldcell) {
            auto ent = selected_worldcell->NewEntity();
            frame->PropertyPanelSetEntity(ent);
            SetItemCount(selected_worldcell->entities.size());
        }
    }
    
    void Edit (MainFrame* frame, long item) {
        if (selected_worldcell) {
            auto ent = selected_worldcell->entities[item];
            frame->PropertyPanelSetEntity(ent);
        }
    }
    
    void Delete (long item) {
        if (selected_worldcell) {
            selected_worldcell->DeleteEntity(item);
            SetItemCount(selected_worldcell->entities.size());
            RefreshItems(item, GetItemCount());
        }
    }
    
private:
    Editor::WorldCell* selected_worldcell = nullptr;
    
};
 
MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, L"Dzimumlocekļu palielienāšanas programma", wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE) {
    Editor::Init();
    
    // --- MENUS ---
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Load_Cells, L"Ielādēt šūnas", L"Ielādē šūnas kas atrodas konfigurācijas failā.");
    menuFile->Append(ID_Save_Cells, L"Saglabāt šūnas", L"Saglabā atvērtās šūnas.");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, L"Beidzēt\tCtrl-Q", L"Izbeigt programmu.");
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(ID_Hello, L"Labrīt", L"Labrīt.");
    menuHelp->Append(wxID_ABOUT, L"Par", L"Par programmu.");
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, L"&Fails");
    menuBar->Append(menuHelp, L"&Palīdzība");

 
    SetMenuBar( menuBar );
 
    CreateStatusBar();
    SetStatusText(L"Man ir milzīgs pimpis!");
 
    Bind(wxEVT_MENU, &MainFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    
    // --- POP-UP MENUS ---
    world_tree_cell_popup = new wxMenu;
    world_tree_cell_popup->Append(ID_World_Tree_Edit, L"Rediģēt", L"Rediģēt šūnu.");
    world_tree_cell_popup->Append(ID_World_Tree_Show, L"Rādīt", L"Parāda šūnas saturu 3D skatā.");
    world_tree_cell_popup->Append(ID_World_Tree_Hide, L"Slēpt", L"Paslēpj šūnas saturu 3D skatā.");
    world_tree_cell_popup->Append(ID_World_Tree_Begonis, L"Begonizēt", L"Begonizē šūnas saturu.");
    world_tree_cell_popup->Append(ID_World_Tree_Delete, L"Dzēst", L"Dzēš šūnu.");
    world_tree_item_popup = new wxMenu;
    world_tree_item_popup->Append(ID_World_Tree_Show, L"Rādīt", L"Parāda šūnas apakšsadaļas saturu 3D skatā.");
    world_tree_item_popup->Append(ID_World_Tree_Hide, L"Slēpt", L"Paslēpj šūnas apakšsadaļas saturu 3D skatā.");
    world_tree_root_popup = new wxMenu;
    world_tree_root_popup->Append(ID_World_Tree_New, L"Pievienot jaunu", L"Pievieno jaunu šūnu");
    Bind(wxEVT_MENU, &MainFrame::OnWorldCellTreePopupSelect, this, ID_World_Tree_New);
    Bind(wxEVT_MENU, &MainFrame::OnWorldCellTreePopupSelect, this, ID_World_Tree_Show);
    Bind(wxEVT_MENU, &MainFrame::OnWorldCellTreePopupSelect, this, ID_World_Tree_Edit);
    Bind(wxEVT_MENU, &MainFrame::OnWorldCellTreePopupSelect, this, ID_World_Tree_Hide);
    Bind(wxEVT_MENU, &MainFrame::OnWorldCellTreePopupSelect, this, ID_World_Tree_Begonis);
    Bind(wxEVT_MENU, &MainFrame::OnWorldCellTreePopupSelect, this, ID_World_Tree_Delete);
    
    entity_list_popup = new wxMenu;
    entity_list_popup->Append(ID_Entity_List_New, L"Jauns", L"Dzēš izvēlēto ierakstu.");
    entity_list_popup->Append(ID_Entity_List_Delete, L"Dzēst", L"Dzēš izvēlēto ierakstu.");
    entity_list_popup->Append(ID_Entity_List_Edit, L"Rediģēt", L"Atver rediģēšanai izvēlēto ierakstu.");
    Bind(wxEVT_MENU, &MainFrame::OnEntityListPopupSelect, this, ID_Entity_List_New);
    Bind(wxEVT_MENU, &MainFrame::OnEntityListPopupSelect, this, ID_Entity_List_Edit);
    Bind(wxEVT_MENU, &MainFrame::OnEntityListPopupSelect, this, ID_Entity_List_Delete);
    
     // notify wxAUI which frame to use
    m_mgr.SetManagedWindow(this);

    
    /*wxTextCtrl* text1 = new wxTextCtrl(this, -1, _("Pane 1 - sample text"),
    wxDefaultPosition, wxSize(200,150),
    wxNO_BORDER | wxTE_MULTILINE);*/
    
    world_tree = new wxTreeCtrl(this, -1, wxDefaultPosition, wxSize(200, 150));
    
    property_panel = new wxPropertyGrid(this, -1);

    entity_list = new EntityList(this, -1, wxDefaultPosition, wxSize(200,150), wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES);
    
    wxTextCtrl* output_text_ctrl = new wxTextCtrl(this, -1, "",
    wxDefaultPosition, wxSize(200,150),
    wxNO_BORDER | wxTE_READONLY | wxTE_MULTILINE);
    
    std_cout_redirect = new wxStreamToTextRedirector(output_text_ctrl);
    
    std::cout << "Loceklis!" << std::endl;
    std::cout << "Loceklis!" << std::endl;
    std::cout << "Loceklis!" << std::endl;

    Viewport* canvas = new Viewport(this, wxID_ANY, nullptr, { 0, 0 }, { 800, 800 });
    m_mgr.AddPane(world_tree, wxLEFT, L"Dzimumceļu navigators");
    m_mgr.AddPane(property_panel, wxLEFT, L"Dzimumlocekļa īpašības");
    m_mgr.AddPane(entity_list, wxBOTTOM, L"Dzimumlocekļu uzskaite");
    m_mgr.AddPane(output_text_ctrl, wxBOTTOM, L"Konsole");
    //m_mgr.AddPane(text4, wxCENTER, L"GIANT PENIS");
    m_mgr.AddPane(canvas, wxCENTER, L"GIANT PENIS");
    
    //m_mgr.GetPane(text1).CloseButton(false);
    
    // --- WORLD TREE ---
    world_tree_root_node = world_tree->AddRoot(L"Pasaule");
    RebuildWorldCellTree();
    world_tree->Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &MainFrame::OnWorldCellTreeSelect, this);
    world_tree->Bind(wxEVT_TREE_ITEM_ACTIVATED, &MainFrame::OnWorldCellTreeActivate, this);
    
    // --- PROPERTY PANEL ---
    //entity_data_panel->Append(new wxPropertyCategory(L"Īpašnieks"));
    //entity_data_panel->Append(new wxStringProperty(L"Vārds", wxPG_LABEL, L"Regejs"));
    //entity_data_panel->Append(new wxPropertyCategory(L"Dzimumloceklis"));
    //entity_data_panel->Append(new wxFloatProperty(L"Izmērs", wxPG_LABEL, 12345.678));
    //entity_data_panel->Append(new wxBoolProperty(L"Iespaidīgs?", wxPG_LABEL, true));
    property_panel->Bind(wxEVT_PG_CHANGED, &MainFrame::OnPropertyPanelChanged, this);
    
    // --- ENTITY LIST ---

    entity_list->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &MainFrame::OnEntityListSelect, this);
    entity_list->Bind(wxEVT_LIST_ITEM_ACTIVATED, &MainFrame::OnEntityListActivate, this);
    //long benis1 = entity_list->InsertItem(0, L"420");
    //entity_list->SetItem(benis1, 1, L"Lielais Čunguss");
    //entity_list->SetItem(benis1, 2, L"Lielais Priekšnieks");    
    
    //long benis2 = entity_list->InsertItem(0, L"69");
    //entity_list->SetItem(benis2, 1, L"Benis");
    //entity_list->SetItem(benis2, 2, L"Spurdo O. Fuggs");

    // --- AUI FLAGS ---
    unsigned int flags = m_mgr.GetFlags();
    flags = flags ^ wxAUI_MGR_TRANSPARENT_HINT;
    flags = flags | wxAUI_MGR_RECTANGLE_HINT;
    m_mgr.SetFlags(flags);
    
    //std::cout << "MY ASS" << std::endl;


    // tell the manager to "commit" all the changes just made
    m_mgr.Update();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
 
void MainFrame::OnAbout(wxCommandEvent& event)
{
    //wxMessageBox("This is a wxWidgets Hello World example",
    //             "About Hello World", wxOK | wxICON_INFORMATION);
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(L"Dzimumlocekļu palielinātājs");
    aboutInfo.SetVersion(L"versija virs 9000");
    aboutInfo.SetDescription(L"Vispiemērotākā dzimumlocekļu palielināšanas funkcionalitāte vienā rīkkopā.");
    aboutInfo.SetCopyright(L"Autortiesības (C) Lielais Jānis Dambergs 2022");
    aboutInfo.SetWebSite(L"http://dzimumlocekļuserviss.lv");
    wxAboutBox(aboutInfo);
}
 
void MainFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

void MainFrame::RebuildWorldCellTree() {
    world_tree->DeleteChildren(world_tree_root_node);
    world_tree_map.clear();
    
    for (auto it : Editor::worldCells) {
        auto cell_node = world_tree->AppendItem(world_tree_root_node, it->name);
        auto ent_node = world_tree->AppendItem(cell_node, L"Entītijas");
        auto trans_node = world_tree->AppendItem(cell_node, L"Pārejas");
        auto path_node = world_tree->AppendItem(cell_node, L"Ceļi");
        auto nav_node = world_tree->AppendItem(cell_node, L"Navigācija");
        
        using namespace Editor; using Ind = WorldCellIndirector;
        world_tree_map[cell_node.GetID()] = WorldCellIndirector { Ind::CELL_ITSELF, it };
        world_tree_map[ent_node.GetID()] = WorldCellIndirector { Ind::CELL_ENTITIES, it };
        world_tree_map[trans_node.GetID()] = WorldCellIndirector { Ind::CELL_TRANSITIONS, it };
        world_tree_map[path_node.GetID()] = WorldCellIndirector { Ind::CELL_PATHS, it };
        world_tree_map[nav_node.GetID()] = WorldCellIndirector { Ind::CELL_NAVMESH, it };
    }
}

void MainFrame::OnWorldCellTreeSelect(wxTreeEvent& event) {
    if (event.GetItem() == world_tree_root_node) {
        world_tree_selected_item = world_tree_root_node;
        PopupMenu(world_tree_root_popup);
    } else {
        auto& indirect = world_tree_map[event.GetItem().GetID()];
        world_tree_selected_item = event.GetItem();
        
        if (indirect.indirection_type == Editor::WorldCellIndirector::CELL_ITSELF) {
            PopupMenu(world_tree_cell_popup);
        } else {
            PopupMenu(world_tree_item_popup);
        }
    }
    
    std::cout << "open menu for: " << event.GetItem() << std::endl;
}

void MainFrame::OnWorldCellTreePopupSelect(wxCommandEvent& event) {
    if (world_tree_selected_item == world_tree_root_node) {
        switch (event.GetId()) {
            case ID_World_Tree_New:
            auto cell = Editor::NewWorldCell();
            RebuildWorldCellTree();
            PropertyPanelSetWorldCell(cell);
            break;
        }
    } else {
        auto& indirect = world_tree_map[world_tree_selected_item];
        switch (event.GetId()) {
            case ID_World_Tree_Show:
            std::cout << "event is load, selected: " << world_tree_selected_item << std::endl;
            indirect.Show();
            break;
            case ID_World_Tree_Hide:
            std::cout << "event is yeet, selected: " << world_tree_selected_item << std::endl;
            indirect.Hide();
            break;
            case ID_World_Tree_Edit:
            std::cout << "event is yeet, selected: " << world_tree_selected_item << std::endl;
            PropertyPanelSetWorldCell(indirect.into);
            break;
            case ID_World_Tree_Begonis:
            std::cout << "event is begonis, selected: " << world_tree_selected_item << std::endl;
            indirect.Begonis();
            break;
            case ID_World_Tree_Delete:
            wxMessageDialog delete_confirmation(this, L"Uzmanību! Jums ir iespaidīgs loceklis!", L"Iespaidīgs loceklis!", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_EXCLAMATION);
            std::cout << "output: " << delete_confirmation.ShowModal() << std::endl;
            // TODO: return on no or cancel
            indirect.Delete();
            RebuildWorldCellTree();
            break;
        }
    }
}

void MainFrame::PropertyPanelClear() {
    property_panel->Clear();
    selected_worldcell = nullptr;
}
void MainFrame::PropertyPanelSetWorldCell(Editor::WorldCell* cell) {
    PropertyPanelClear();
    selected_worldcell = cell;
    property_panel->Append(new wxPropertyCategory(L"Pasaules šūna"));
    property_panel->Append(new wxStringProperty(L"Nosaukums", "worldcell-name", cell->name.data()));
    auto chkprop1 = new wxBoolProperty(L"Iekštelpa", "worldcell-int", cell->is_interior);
    auto chkprop2 = new wxBoolProperty(L"Iekštlp. apg.", "worldcell-int-light", cell->is_interior_lighting);
    chkprop1->SetAttribute("UseCheckbox", 1); // <-- bad library design
    chkprop2->SetAttribute("UseCheckbox", 1); // <-- DO NOT design your libraries like that
    property_panel->Append(chkprop1);
    property_panel->Append(chkprop2);
}

void MainFrame::PropertyPanelSetEntity(Editor::Entity* entity) {
    PropertyPanelClear();
    selected_entity = entity;
    auto gen_props = property_panel->Append(new wxPropertyCategory(L"Vispārīgās īpašības"));
    auto spec_props = property_panel->Append(new wxPropertyCategory(L"Īpašās īpašības"));
    std::cout << "spec props: " << spec_props << std::endl;
    
    property_panel->AppendIn(gen_props, new wxUIntProperty(L"ID", "entity-id", entity->id));
    property_panel->AppendIn(gen_props, new wxStringProperty(L"Nosaukums", "entity-name", entity->name.data()));
    auto loc_props = property_panel->AppendIn(gen_props, new wxPropertyCategory(L"Lokācija"));
    property_panel->AppendIn(loc_props, new wxFloatProperty(L"X", "entity-location-x", entity->location[0]));
    property_panel->AppendIn(loc_props, new wxFloatProperty(L"Y", "entity-location-y", entity->location[1]));
    property_panel->AppendIn(loc_props, new wxFloatProperty(L"Z", "entity-location-z", entity->location[2]));
    auto rot_props = property_panel->AppendIn(gen_props, new wxPropertyCategory(L"Rotācija"));
    property_panel->AppendIn(rot_props, new wxFloatProperty(L"X", "entity-rotation-x", entity->rotation[0]));
    property_panel->AppendIn(rot_props, new wxFloatProperty(L"Y", "entity-rotation-y", entity->rotation[1]));
    property_panel->AppendIn(rot_props, new wxFloatProperty(L"Z", "entity-rotation-z", entity->rotation[2]));
    property_panel->AppendIn(gen_props, new wxStringProperty(L"Darbība", "entity-action", entity->action.data()));
}

void MainFrame::OnPropertyPanelChanged(wxPropertyGridEvent& event) {
    std::unordered_map<std::string, void (*)(wxPropertyGridEvent&, MainFrame*)> updates = {
        {"worldcell-name", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_worldcell->name = event.GetPropertyValue().GetString(); frame->RebuildWorldCellTree(); }},
        {"worldcell-int", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_worldcell->is_interior = event.GetPropertyValue().GetBool(); }},
        {"worldcell-int-light", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_worldcell->is_interior_lighting = event.GetPropertyValue().GetBool(); }},
        
        {"entity-id", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->id = event.GetPropertyValue().GetULongLong().GetValue(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        {"entity-name", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->name = event.GetPropertyValue().GetString(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        {"entity-location-x", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->location[0] = event.GetPropertyValue().GetDouble(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        {"entity-location-y", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->location[1] = event.GetPropertyValue().GetDouble(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        {"entity-location-z", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->location[2] = event.GetPropertyValue().GetDouble(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        {"entity-rotation-x", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->rotation[0] = event.GetPropertyValue().GetDouble(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        {"entity-rotation-y", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->rotation[1] = event.GetPropertyValue().GetDouble(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        {"entity-rotation-z", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->rotation[2] = event.GetPropertyValue().GetDouble(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        
        {"entity-action", [](wxPropertyGridEvent& event, MainFrame* frame){ frame->selected_entity->action = event.GetPropertyValue().GetString(); frame->entity_list->RefreshItem(frame->entity_list_selected_item); }},
        };
    
    updates[std::string(event.GetPropertyName())](event, this);
    
    std::cout << "PROPERTY PANEL CHANGED!!!" << event.GetPropertyName() << std::endl;
}

void MainFrame::OnWorldCellTreeActivate(wxTreeEvent& event) {
    if (event.GetItem() == world_tree_root_node) {
        auto alert = wxMessageDialog(this, L"Tev biksēs skudras!", L"Skudru uzbrukums!", wxOK | wxICON_EXCLAMATION);
        alert.SetExtendedMessage(L"Ir noticis skudru uzbrukums jūsu biksēm. Programma pēc šī uzbrukuma nespēs atkopties. Skatīt lietošanas instrukcijas nodaļu \"7.6.3 Negaidīts skudru uzbrukums\" (431. lpp).");
        alert.ShowModal();
    } else {
        auto& indirect = world_tree_map[event.GetItem().GetID()];
        if (indirect.indirection_type == Editor::WorldCellIndirector::CELL_ITSELF) {
            PropertyPanelSetWorldCell(indirect.into);
            entity_list->SetWorldCell(indirect.into);
        }
    }
}

void MainFrame::OnEntityListSelect(wxListEvent& event) {
    entity_list_selected_item = event.GetIndex();
    PopupMenu(entity_list_popup);
    std::cout << "selected" << std::endl;
}

void MainFrame::OnEntityListActivate(wxListEvent& event) {
    entity_list_selected_item = event.GetIndex();
    entity_list->Edit(this, entity_list_selected_item);
    std::cout << "activated" << std::endl;
}

void MainFrame::OnEntityListPopupSelect(wxCommandEvent& event) {
    switch (event.GetId()) {
        case ID_Entity_List_New:
            entity_list->New(this);
            break;
        case ID_Entity_List_Delete:
            entity_list->Delete(entity_list_selected_item);
            break;
        case ID_Entity_List_Edit:
            entity_list->Edit(this, entity_list_selected_item);
            break;
    }
}

#endif
#ifdef NEKAD

///////////////////////////////////////////////////////////////////////////////
// Name:        auidemo.cpp
// Purpose:     wxaui: wx advanced user interface - sample/test program
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-10-03
// Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#include "wx/app.h"
#include "wx/grid.h"
#include "wx/treectrl.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/clipbrd.h"
#include "wx/image.h"
#include "wx/colordlg.h"
#include "wx/wxhtml.h"
#include "wx/imaglist.h"
#include "wx/dataobj.h"
#include "wx/dcclient.h"
#include "wx/bmpbuttn.h"
#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"

#include "wx/aui/aui.h"


/* XPM */
static const char *const sample_xpm[] = {
"32 32 10 1",
" 	c None",
".	c #2020FF",
"+	c #8080FF",
"@	c #C0C0FF",
"#	c #FFFF00",
"$	c #FFFF80",
"%	c #FF2020",
"&	c #FF4848",
"*	c #FFFFDF",
"=	c #FFC0C0",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"        ..............          ",
"        .++++++++++++.          ",
"        .++++++++++++.          ",
"        .+@@+++++++++.          ",
"        .+@@+++++++++.          ",
"        .+@@+++++++++.          ",
"        .+@@++++++##############",
"        .+@@++++++#$$$$$$$$$$$$#",
"%%%%%%%%.+@@++++++#$$$$$$$$$$$$#",
"%&&&&&&&.+@@++++++#$**$$$$$$$$$#",
"%&&&&&&&.+@@++++++#$**$$$$$$$$$#",
"%&==&&&&.+++++++++#$**$$$$$$$$$#",
"%&==&&&&.+++++++++#$**$$$$$$$$$#",
"%&==&&&&..........#$**$$$$$$$$$#",
"%&==&&&&&&&&&%    #$**$$$$$$$$$#",
"%&==&&&&&&&&&%    #$**$$$$$$$$$#",
"%&==&&&&&&&&&%    #$**$$$$$$$$$#",
"%&==&&&&&&&&&%    #$$$$$$$$$$$$#",
"%&==&&&&&&&&&%    #$$$$$$$$$$$$#",
"%&&&&&&&&&&&&%    ##############",
"%&&&&&&&&&&&&%                  ",
"%%%%%%%%%%%%%%                  ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                "
};


// -- application --

class MyApp : public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
};

wxDECLARE_APP(MyApp);
wxIMPLEMENT_APP(MyApp);


class wxSizeReportCtrl;

// -- frame --

class MyFrame : public wxFrame
{
    enum
    {
        ID_CreateTree = wxID_HIGHEST+1,
        ID_CreateGrid,
        ID_CreateText,
        ID_CreateHTML,
        ID_CreateNotebook,
        ID_CreateSizeReport,
        ID_GridContent,
        ID_TextContent,
        ID_TreeContent,
        ID_HTMLContent,
        ID_NotebookContent,
        ID_SizeReportContent,
        ID_CreatePerspective,
        ID_CopyPerspectiveCode,
        ID_AllowFloating,
        ID_AllowActivePane,
        ID_TransparentHint,
        ID_VenetianBlindsHint,
        ID_RectangleHint,
        ID_NoHint,
        ID_HintFade,
        ID_NoVenetianFade,
        ID_TransparentDrag,
        ID_NoGradient,
        ID_VerticalGradient,
        ID_HorizontalGradient,
        ID_LiveUpdate,
        ID_AllowToolbarResizing,
        ID_Settings,
        ID_CustomizeToolbar,
        ID_DropDownToolbarItem,
        ID_NotebookNoCloseButton,
        ID_NotebookCloseButton,
        ID_NotebookCloseButtonAll,
        ID_NotebookCloseButtonActive,
        ID_NotebookAllowTabMove,
        ID_NotebookAllowTabExternalMove,
        ID_NotebookAllowTabSplit,
        ID_NotebookWindowList,
        ID_NotebookScrollButtons,
        ID_NotebookTabFixedWidth,
        ID_NotebookArtGloss,
        ID_NotebookArtSimple,
        ID_NotebookAlignTop,
        ID_NotebookAlignBottom,

        ID_SampleItem,

        ID_FirstPerspective = ID_CreatePerspective+1000
    };

public:
    MyFrame(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

    wxAuiDockArt* GetDockArt();
    void DoUpdate();

private:
    wxTextCtrl* CreateTextCtrl(const wxString& text = wxEmptyString);
    wxGrid* CreateGrid();
    wxTreeCtrl* CreateTreeCtrl();
    wxSizeReportCtrl* CreateSizeReportCtrl(const wxSize &size = wxWindow::FromDIP(wxSize(80, 80), NULL));
    wxPoint GetStartPosition();
    wxHtmlWindow* CreateHTMLCtrl(wxWindow* parent = NULL);
    wxAuiNotebook* CreateNotebook();

    wxString GetIntroText();

private:

    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);

    void OnCreateTree(wxCommandEvent& evt);
    void OnCreateGrid(wxCommandEvent& evt);
    void OnCreateHTML(wxCommandEvent& evt);
    void OnCreateNotebook(wxCommandEvent& evt);
    void OnCreateText(wxCommandEvent& evt);
    void OnCreateSizeReport(wxCommandEvent& evt);
    void OnChangeContentPane(wxCommandEvent& evt);
    void OnDropDownToolbarItem(wxAuiToolBarEvent& evt);
    void OnCreatePerspective(wxCommandEvent& evt);
    void OnCopyPerspectiveCode(wxCommandEvent& evt);
    void OnRestorePerspective(wxCommandEvent& evt);
    void OnSettings(wxCommandEvent& evt);
    void OnCustomizeToolbar(wxCommandEvent& evt);
    void OnAllowNotebookDnD(wxAuiNotebookEvent& evt);
    void OnNotebookPageClose(wxAuiNotebookEvent& evt);
    void OnNotebookPageClosed(wxAuiNotebookEvent& evt);
    void OnNotebookPageChanging(wxAuiNotebookEvent &evt);
    void OnExit(wxCommandEvent& evt);
    void OnAbout(wxCommandEvent& evt);
    void OnTabAlignment(wxCommandEvent &evt);

    void OnGradient(wxCommandEvent& evt);
    void OnToolbarResizing(wxCommandEvent& evt);
    void OnManagerFlag(wxCommandEvent& evt);
    void OnNotebookFlag(wxCommandEvent& evt);
    void OnUpdateUI(wxUpdateUIEvent& evt);

    void OnPaneClose(wxAuiManagerEvent& evt);

private:

    wxAuiManager m_mgr;
    wxArrayString m_perspectives;
    wxMenu* m_perspectives_menu;
    long m_notebook_style;
    long m_notebook_theme;

    wxDECLARE_EVENT_TABLE();
};


// -- wxSizeReportCtrl --
// (a utility control that always reports it's client size)

class wxSizeReportCtrl : public wxControl
{
public:

    wxSizeReportCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     wxAuiManager* mgr = NULL)
                     : wxControl(parent, id, pos, size, wxNO_BORDER)
    {
        m_mgr = mgr;
    }

private:

    void OnPaint(wxPaintEvent& WXUNUSED(evt))
    {
        wxPaintDC dc(this);
        wxSize size = GetClientSize();
        wxString s;
        int h, w, height;

        s.Printf("Size: %d x %d", size.x, size.y);

        dc.SetFont(*wxNORMAL_FONT);
        dc.GetTextExtent(s, &w, &height);
        height += FromDIP(3);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(0, 0, size.x, size.y);
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawLine(0, 0, size.x, size.y);
        dc.DrawLine(0, size.y, size.x, 0);
        dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2));

        if (m_mgr)
        {
            wxAuiPaneInfo pi = m_mgr->GetPane(this);

            s.Printf("Layer: %d", pi.dock_layer);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*1));

            s.Printf("Dock: %d Row: %d", pi.dock_direction, pi.dock_row);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*2));

            s.Printf("Position: %d", pi.dock_pos);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*3));

            s.Printf("Proportion: %d", pi.dock_proportion);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*4));
        }
    }

    void OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
    {
        // intentionally empty
    }

    void OnSize(wxSizeEvent& WXUNUSED(evt))
    {
        Refresh();
    }
private:

    wxAuiManager* m_mgr;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxSizeReportCtrl, wxControl)
    EVT_PAINT(wxSizeReportCtrl::OnPaint)
    EVT_SIZE(wxSizeReportCtrl::OnSize)
    EVT_ERASE_BACKGROUND(wxSizeReportCtrl::OnEraseBackground)
wxEND_EVENT_TABLE()


class SettingsPanel : public wxPanel
{
    enum
    {
        ID_PaneBorderSize = wxID_HIGHEST+1,
        ID_SashSize,
        ID_CaptionSize,
        ID_BackgroundColor,
        ID_SashColor,
        ID_InactiveCaptionColor,
        ID_InactiveCaptionGradientColor,
        ID_InactiveCaptionTextColor,
        ID_ActiveCaptionColor,
        ID_ActiveCaptionGradientColor,
        ID_ActiveCaptionTextColor,
        ID_BorderColor,
        ID_GripperColor
    };

public:

    SettingsPanel(wxWindow* parent, MyFrame* frame)
            : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
              m_frame(frame)
    {
        //wxBoxSizer* vert = new wxBoxSizer(wxVERTICAL);

        //vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);

        wxBoxSizer* s1 = new wxBoxSizer(wxHORIZONTAL);
        m_border_size = new wxSpinCtrl(this, ID_PaneBorderSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
        s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s1->Add(new wxStaticText(this, wxID_ANY, "Pane Border Size:"));
        s1->Add(m_border_size);
        s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s1->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
        //vert->Add(s1, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        wxBoxSizer* s2 = new wxBoxSizer(wxHORIZONTAL);
        m_sash_size = new wxSpinCtrl(this, ID_SashSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
        s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s2->Add(new wxStaticText(this, wxID_ANY, "Sash Size:"));
        s2->Add(m_sash_size);
        s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s2->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
        //vert->Add(s2, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        wxBoxSizer* s3 = new wxBoxSizer(wxHORIZONTAL);
        m_caption_size = new wxSpinCtrl(this, ID_CaptionSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));
        s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s3->Add(new wxStaticText(this, wxID_ANY, "Caption Size:"));
        s3->Add(m_caption_size);
        s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s3->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
        //vert->Add(s3, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        //vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);


        wxBitmap b = CreateColorBitmap(*wxBLACK);

        wxBoxSizer* s4 = new wxBoxSizer(wxHORIZONTAL);
        m_background_color = new wxBitmapButton(this, ID_BackgroundColor, b, wxDefaultPosition, FromDIP(wxSize(50,25)));
        s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s4->Add(new wxStaticText(this, wxID_ANY, "Background Color:"));
        s4->Add(m_background_color);
        s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s4->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s5 = new wxBoxSizer(wxHORIZONTAL);
        m_sash_color = new wxBitmapButton(this, ID_SashColor, b, wxDefaultPosition, wxSize(50,25));
        s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s5->Add(new wxStaticText(this, wxID_ANY, "Sash Color:"));
        s5->Add(m_sash_color);
        s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s5->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s6 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_color = new wxBitmapButton(this, ID_InactiveCaptionColor, b, wxDefaultPosition, wxSize(50,25));
        s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s6->Add(new wxStaticText(this, wxID_ANY, "Normal Caption:"));
        s6->Add(m_inactive_caption_color);
        s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s6->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s7 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_gradient_color = new wxBitmapButton(this, ID_InactiveCaptionGradientColor, b, wxDefaultPosition, wxSize(50,25));
        s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s7->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Gradient:"));
        s7->Add(m_inactive_caption_gradient_color);
        s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s7->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s8 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_text_color = new wxBitmapButton(this, ID_InactiveCaptionTextColor, b, wxDefaultPosition, wxSize(50,25));
        s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s8->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Text:"));
        s8->Add(m_inactive_caption_text_color);
        s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s8->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s9 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_color = new wxBitmapButton(this, ID_ActiveCaptionColor, b, wxDefaultPosition, wxSize(50,25));
        s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s9->Add(new wxStaticText(this, wxID_ANY, "Active Caption:"));
        s9->Add(m_active_caption_color);
        s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s9->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s10 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_gradient_color = new wxBitmapButton(this, ID_ActiveCaptionGradientColor, b, wxDefaultPosition, wxSize(50,25));
        s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s10->Add(new wxStaticText(this, wxID_ANY, "Active Caption Gradient:"));
        s10->Add(m_active_caption_gradient_color);
        s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s10->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s11 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_text_color = new wxBitmapButton(this, ID_ActiveCaptionTextColor, b, wxDefaultPosition, wxSize(50,25));
        s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s11->Add(new wxStaticText(this, wxID_ANY, "Active Caption Text:"));
        s11->Add(m_active_caption_text_color);
        s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s11->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s12 = new wxBoxSizer(wxHORIZONTAL);
        m_border_color = new wxBitmapButton(this, ID_BorderColor, b, wxDefaultPosition, wxSize(50,25));
        s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s12->Add(new wxStaticText(this, wxID_ANY, "Border Color:"));
        s12->Add(m_border_color);
        s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s12->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s13 = new wxBoxSizer(wxHORIZONTAL);
        m_gripper_color = new wxBitmapButton(this, ID_GripperColor, b, wxDefaultPosition, wxSize(50,25));
        s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s13->Add(new wxStaticText(this, wxID_ANY, "Gripper Color:"));
        s13->Add(m_gripper_color);
        s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s13->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxGridSizer* grid_sizer = new wxGridSizer(2);
        grid_sizer->SetHGap(FromDIP(5));
        grid_sizer->Add(s1);  grid_sizer->Add(s4);
        grid_sizer->Add(s2);  grid_sizer->Add(s5);
        grid_sizer->Add(s3);  grid_sizer->Add(s13);
        grid_sizer->Add(FromDIP(1),FromDIP(1)); grid_sizer->Add(s12);
        grid_sizer->Add(s6);  grid_sizer->Add(s9);
        grid_sizer->Add(s7);  grid_sizer->Add(s10);
        grid_sizer->Add(s8);  grid_sizer->Add(s11);

        wxBoxSizer* cont_sizer = new wxBoxSizer(wxVERTICAL);
        cont_sizer->Add(grid_sizer, 1, wxEXPAND | wxALL, FromDIP(5));
        SetSizer(cont_sizer);
        GetSizer()->SetSizeHints(this);

        m_border_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
        m_sash_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
        m_caption_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));

        UpdateColors();
    }

private:

    wxBitmap CreateColorBitmap(const wxColour& c)
    {
        wxImage image;
        wxSize size = FromDIP(wxSize(25, 14));
        image.Create(size);
        for (int x = 0; x < size.x; ++x)
            for (int y = 0; y < size.y; ++y)
            {
                wxColour pixcol = c;
                if (x == 0 || x == size.x || y == 0 || y == size.y)
                    pixcol = *wxBLACK;
                image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue());
            }
        return wxBitmap(image);
    }

    void UpdateColors()
    {
        wxColour bk = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BACKGROUND_COLOUR);
        m_background_color->SetBitmapLabel(CreateColorBitmap(bk));

        wxColour cap = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
        m_inactive_caption_color->SetBitmapLabel(CreateColorBitmap(cap));

        wxColour capgrad = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR);
        m_inactive_caption_gradient_color->SetBitmapLabel(CreateColorBitmap(capgrad));

        wxColour captxt = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR);
        m_inactive_caption_text_color->SetBitmapLabel(CreateColorBitmap(captxt));

        wxColour acap = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR);
        m_active_caption_color->SetBitmapLabel(CreateColorBitmap(acap));

        wxColour acapgrad = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR);
        m_active_caption_gradient_color->SetBitmapLabel(CreateColorBitmap(acapgrad));

        wxColour acaptxt = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR);
        m_active_caption_text_color->SetBitmapLabel(CreateColorBitmap(acaptxt));

        wxColour sash = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_SASH_COLOUR);
        m_sash_color->SetBitmapLabel(CreateColorBitmap(sash));

        wxColour border = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BORDER_COLOUR);
        m_border_color->SetBitmapLabel(CreateColorBitmap(border));

        wxColour gripper = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_GRIPPER_COLOUR);
        m_gripper_color->SetBitmapLabel(CreateColorBitmap(gripper));
    }

    void OnPaneBorderSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnSashSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_SASH_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnCaptionSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnSetColor(wxCommandEvent& event)
    {
        wxColourDialog dlg(m_frame);
        dlg.SetTitle(_("Color Picker"));
        if (dlg.ShowModal() != wxID_OK)
            return;

        int var = 0;
        switch (event.GetId())
        {
            case ID_BackgroundColor:              var = wxAUI_DOCKART_BACKGROUND_COLOUR; break;
            case ID_SashColor:                    var = wxAUI_DOCKART_SASH_COLOUR; break;
            case ID_InactiveCaptionColor:         var = wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR; break;
            case ID_InactiveCaptionGradientColor: var = wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR; break;
            case ID_InactiveCaptionTextColor:     var = wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR; break;
            case ID_ActiveCaptionColor:           var = wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR; break;
            case ID_ActiveCaptionGradientColor:   var = wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR; break;
            case ID_ActiveCaptionTextColor:       var = wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR; break;
            case ID_BorderColor:                  var = wxAUI_DOCKART_BORDER_COLOUR; break;
            case ID_GripperColor:                 var = wxAUI_DOCKART_GRIPPER_COLOUR; break;
            default: return;
        }

        m_frame->GetDockArt()->SetColor(var, dlg.GetColourData().GetColour());
        m_frame->DoUpdate();
        UpdateColors();
    }

private:

    MyFrame* m_frame;
    wxSpinCtrl* m_border_size;
    wxSpinCtrl* m_sash_size;
    wxSpinCtrl* m_caption_size;
    wxBitmapButton* m_inactive_caption_text_color;
    wxBitmapButton* m_inactive_caption_gradient_color;
    wxBitmapButton* m_inactive_caption_color;
    wxBitmapButton* m_active_caption_text_color;
    wxBitmapButton* m_active_caption_gradient_color;
    wxBitmapButton* m_active_caption_color;
    wxBitmapButton* m_sash_color;
    wxBitmapButton* m_background_color;
    wxBitmapButton* m_border_color;
    wxBitmapButton* m_gripper_color;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(SettingsPanel, wxPanel)
    EVT_SPINCTRL(ID_PaneBorderSize, SettingsPanel::OnPaneBorderSize)
    EVT_SPINCTRL(ID_SashSize, SettingsPanel::OnSashSize)
    EVT_SPINCTRL(ID_CaptionSize, SettingsPanel::OnCaptionSize)
    EVT_BUTTON(ID_BackgroundColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_SashColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionGradientColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionTextColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionGradientColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionTextColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_BorderColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_GripperColor, SettingsPanel::OnSetColor)
wxEND_EVENT_TABLE()


bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxFrame* frame = new MyFrame(NULL,
                                 wxID_ANY,
                                 "wxAUI Sample Application",
                                 wxDefaultPosition,
                                 wxWindow::FromDIP(wxSize(800, 600), NULL));
    frame->Show();

    return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MENU(MyFrame::ID_CreateTree, MyFrame::OnCreateTree)
    EVT_MENU(MyFrame::ID_CreateGrid, MyFrame::OnCreateGrid)
    EVT_MENU(MyFrame::ID_CreateText, MyFrame::OnCreateText)
    EVT_MENU(MyFrame::ID_CreateHTML, MyFrame::OnCreateHTML)
    EVT_MENU(MyFrame::ID_CreateSizeReport, MyFrame::OnCreateSizeReport)
    EVT_MENU(MyFrame::ID_CreateNotebook, MyFrame::OnCreateNotebook)
    EVT_MENU(MyFrame::ID_CreatePerspective, MyFrame::OnCreatePerspective)
    EVT_MENU(MyFrame::ID_CopyPerspectiveCode, MyFrame::OnCopyPerspectiveCode)
    EVT_MENU(ID_AllowFloating, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_VenetianBlindsHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_RectangleHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NoHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_HintFade, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NoVenetianFade, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentDrag, MyFrame::OnManagerFlag)
    EVT_MENU(ID_LiveUpdate, MyFrame::OnManagerFlag)
    EVT_MENU(ID_AllowActivePane, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NotebookTabFixedWidth, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookNoCloseButton, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButton, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButtonAll, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButtonActive, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabMove, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabExternalMove, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabSplit, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookScrollButtons, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookWindowList, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtGloss, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtSimple, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAlignTop,     MyFrame::OnTabAlignment)
    EVT_MENU(ID_NotebookAlignBottom,  MyFrame::OnTabAlignment)
    EVT_MENU(ID_NoGradient, MyFrame::OnGradient)
    EVT_MENU(ID_VerticalGradient, MyFrame::OnGradient)
    EVT_MENU(ID_HorizontalGradient, MyFrame::OnGradient)
    EVT_MENU(ID_AllowToolbarResizing, MyFrame::OnToolbarResizing)
    EVT_MENU(ID_Settings, MyFrame::OnSettings)
    EVT_MENU(ID_CustomizeToolbar, MyFrame::OnCustomizeToolbar)
    EVT_MENU(ID_GridContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_TreeContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_TextContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_SizeReportContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_HTMLContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_NotebookContent, MyFrame::OnChangeContentPane)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_UPDATE_UI(ID_NotebookTabFixedWidth, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookNoCloseButton, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButton, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButtonAll, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButtonActive, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabMove, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabExternalMove, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabSplit, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookScrollButtons, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookWindowList, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_AllowFloating, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VenetianBlindsHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_RectangleHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HintFade, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoVenetianFade, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentDrag, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_LiveUpdate, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VerticalGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HorizontalGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_AllowToolbarResizing, MyFrame::OnUpdateUI)
    EVT_MENU_RANGE(MyFrame::ID_FirstPerspective, MyFrame::ID_FirstPerspective+1000,
                   MyFrame::OnRestorePerspective)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_DropDownToolbarItem, MyFrame::OnDropDownToolbarItem)
    EVT_AUI_PANE_CLOSE(MyFrame::OnPaneClose)
    EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, MyFrame::OnAllowNotebookDnD)
    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, MyFrame::OnNotebookPageClose)
    EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, MyFrame::OnNotebookPageClosed)
    EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, MyFrame::OnNotebookPageChanging)
wxEND_EVENT_TABLE()


MyFrame::MyFrame(wxWindow* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
        : wxFrame(parent, id, title, pos, size, style)
{
    // tell wxAuiManager to manage this frame
    m_mgr.SetManagedWindow(this);

    // set frame icon
    SetIcon(wxIcon(sample_xpm));

    // set up default notebook style
    m_notebook_style = wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
    m_notebook_theme = 0;

    // create menu
    wxMenuBar* mb = new wxMenuBar;

    wxMenu* file_menu = new wxMenu;
    file_menu->Append(wxID_EXIT);

    wxMenu* view_menu = new wxMenu;
    view_menu->Append(ID_CreateText, _("Create Text Control"));
    view_menu->Append(ID_CreateHTML, _("Create HTML Control"));
    view_menu->Append(ID_CreateTree, _("Create Tree"));
    view_menu->Append(ID_CreateGrid, _("Create Grid"));
    view_menu->Append(ID_CreateNotebook, _("Create Notebook"));
    view_menu->Append(ID_CreateSizeReport, _("Create Size Reporter"));
    view_menu->AppendSeparator();
    view_menu->Append(ID_GridContent, _("Use a Grid for the Content Pane"));
    view_menu->Append(ID_TextContent, _("Use a Text Control for the Content Pane"));
    view_menu->Append(ID_HTMLContent, _("Use an HTML Control for the Content Pane"));
    view_menu->Append(ID_TreeContent, _("Use a Tree Control for the Content Pane"));
    view_menu->Append(ID_NotebookContent, _("Use a wxAuiNotebook control for the Content Pane"));
    view_menu->Append(ID_SizeReportContent, _("Use a Size Reporter for the Content Pane"));

    wxMenu* options_menu = new wxMenu;
    options_menu->AppendRadioItem(ID_TransparentHint, _("Transparent Hint"));
    options_menu->AppendRadioItem(ID_VenetianBlindsHint, _("Venetian Blinds Hint"));
    options_menu->AppendRadioItem(ID_RectangleHint, _("Rectangle Hint"));
    options_menu->AppendRadioItem(ID_NoHint, _("No Hint"));
    options_menu->AppendSeparator();
    options_menu->AppendCheckItem(ID_HintFade, _("Hint Fade-in"));
    options_menu->AppendCheckItem(ID_AllowFloating, _("Allow Floating"));
    options_menu->AppendCheckItem(ID_NoVenetianFade, _("Disable Venetian Blinds Hint Fade-in"));
    options_menu->AppendCheckItem(ID_TransparentDrag, _("Transparent Drag"));
    options_menu->AppendCheckItem(ID_AllowActivePane, _("Allow Active Pane"));
    // Only show "live resize" toggle if it's actually functional.
    if ( !wxAuiManager::AlwaysUsesLiveResize() )
        options_menu->AppendCheckItem(ID_LiveUpdate, _("Live Resize Update"));
    options_menu->AppendSeparator();
    options_menu->AppendRadioItem(ID_NoGradient, _("No Caption Gradient"));
    options_menu->AppendRadioItem(ID_VerticalGradient, _("Vertical Caption Gradient"));
    options_menu->AppendRadioItem(ID_HorizontalGradient, _("Horizontal Caption Gradient"));
    options_menu->AppendSeparator();
    options_menu->AppendCheckItem(ID_AllowToolbarResizing, _("Allow Toolbar Resizing"));
    options_menu->AppendSeparator();
    options_menu->Append(ID_Settings, _("Settings Pane"));

    wxMenu* notebook_menu = new wxMenu;
    notebook_menu->AppendRadioItem(ID_NotebookArtGloss, _("Glossy Theme (Default)"));
    notebook_menu->AppendRadioItem(ID_NotebookArtSimple, _("Simple Theme"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendRadioItem(ID_NotebookNoCloseButton, _("No Close Button"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButton, _("Close Button at Right"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButtonAll, _("Close Button on All Tabs"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButtonActive, _("Close Button on Active Tab"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendRadioItem(ID_NotebookAlignTop, _("Tab Top Alignment"));
    notebook_menu->AppendRadioItem(ID_NotebookAlignBottom, _("Tab Bottom Alignment"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabMove, _("Allow Tab Move"));
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabExternalMove, _("Allow External Tab Move"));
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabSplit, _("Allow Notebook Split"));
    notebook_menu->AppendCheckItem(ID_NotebookScrollButtons, _("Scroll Buttons Visible"));
    notebook_menu->AppendCheckItem(ID_NotebookWindowList, _("Window List Button Visible"));
    notebook_menu->AppendCheckItem(ID_NotebookTabFixedWidth, _("Fixed-width Tabs"));

    m_perspectives_menu = new wxMenu;
    m_perspectives_menu->Append(ID_CreatePerspective, _("Create Perspective"));
    m_perspectives_menu->Append(ID_CopyPerspectiveCode, _("Copy Perspective Data To Clipboard"));
    m_perspectives_menu->AppendSeparator();
    m_perspectives_menu->Append(ID_FirstPerspective+0, _("Default Startup"));
    m_perspectives_menu->Append(ID_FirstPerspective+1, _("All Panes"));

    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT);

    mb->Append(file_menu, _("&File"));
    mb->Append(view_menu, _("&View"));
    mb->Append(m_perspectives_menu, _("&Perspectives"));
    mb->Append(options_menu, _("&Options"));
    mb->Append(notebook_menu, _("&Notebook"));
    mb->Append(help_menu, _("&Help"));

    SetMenuBar(mb);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(_("Ready"));


    // min size for the frame itself isn't completely done.
    // see the end up wxAuiManager::Update() for the test
    // code. For now, just hard code a frame minimum size
    SetMinSize(FromDIP(wxSize(400,300)));



    // prepare a few custom overflow elements for the toolbars' overflow buttons

    wxAuiToolBarItemArray prepend_items;
    wxAuiToolBarItemArray append_items;
    wxAuiToolBarItem item;
    item.SetKind(wxITEM_SEPARATOR);
    append_items.Add(item);
    item.SetKind(wxITEM_NORMAL);
    item.SetId(ID_CustomizeToolbar);
    item.SetLabel(_("Customize..."));
    append_items.Add(item);


    // create some toolbars
    wxAuiToolBar* tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    tb1->AddTool(ID_SampleItem+1, "Test", wxArtProvider::GetBitmapBundle(wxART_ERROR));
    tb1->AddSeparator();
    tb1->AddTool(ID_SampleItem+2, "Test", wxArtProvider::GetBitmapBundle(wxART_QUESTION));
    tb1->AddTool(ID_SampleItem+3, "Test", wxArtProvider::GetBitmapBundle(wxART_INFORMATION));
    tb1->AddTool(ID_SampleItem+4, "Test", wxArtProvider::GetBitmapBundle(wxART_WARNING));
    tb1->AddTool(ID_SampleItem+5, "Test", wxArtProvider::GetBitmapBundle(wxART_MISSING_IMAGE));
    tb1->SetCustomOverflowItems(prepend_items, append_items);
    tb1->Realize();


    wxAuiToolBar* tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);

    wxBitmapBundle tb2_bmp1 = wxArtProvider::GetBitmapBundle(wxART_QUESTION, wxART_OTHER, wxSize(16,16));
    tb2->AddTool(ID_SampleItem+6, "Disabled", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+7, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+8, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+9, "Test", tb2_bmp1);
    tb2->AddSeparator();
    tb2->AddTool(ID_SampleItem+10, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+11, "Test", tb2_bmp1);
    tb2->AddSeparator();
    tb2->AddTool(ID_SampleItem+12, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+13, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+14, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+15, "Test", tb2_bmp1);
    tb2->SetCustomOverflowItems(prepend_items, append_items);
    tb2->EnableTool(ID_SampleItem+6, false);
    tb2->Realize();


    wxAuiToolBar* tb3 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    wxBitmapBundle tb3_bmp1 = wxArtProvider::GetBitmapBundle(wxART_FOLDER, wxART_OTHER, wxSize(16,16));
    tb3->AddTool(ID_SampleItem+16, "Check 1", tb3_bmp1, "Check 1", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+17, "Check 2", tb3_bmp1, "Check 2", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+18, "Check 3", tb3_bmp1, "Check 3", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+19, "Check 4", tb3_bmp1, "Check 4", wxITEM_CHECK);
    tb3->AddSeparator();
    tb3->AddTool(ID_SampleItem+20, "Radio 1", tb3_bmp1, "Radio 1", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+21, "Radio 2", tb3_bmp1, "Radio 2", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+22, "Radio 3", tb3_bmp1, "Radio 3", wxITEM_RADIO);
    tb3->AddSeparator();
    tb3->AddTool(ID_SampleItem+23, "Radio 1 (Group 2)", tb3_bmp1, "Radio 1 (Group 2)", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+24, "Radio 2 (Group 2)", tb3_bmp1, "Radio 2 (Group 2)", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+25, "Radio 3 (Group 2)", tb3_bmp1, "Radio 3 (Group 2)", wxITEM_RADIO);
    tb3->SetCustomOverflowItems(prepend_items, append_items);
    tb3->Realize();


    wxAuiToolBar* tb4 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_OVERFLOW |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
    wxBitmapBundle tb4_bmp1 = wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
    tb4->AddTool(ID_DropDownToolbarItem, "Item 1", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+23, "Item 2", tb4_bmp1);
    tb4->SetToolSticky(ID_SampleItem+23, true);
    tb4->AddTool(ID_SampleItem+24, "Disabled", tb4_bmp1);
    tb4->EnableTool(ID_SampleItem+24, false); // Just to show disabled items look
    tb4->AddTool(ID_SampleItem+25, "Item 4", tb4_bmp1);
    tb4->AddSeparator();
    tb4->AddTool(ID_SampleItem+26, "Item 5", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+27, "Item 6", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+28, "Item 7", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+29, "Item 8", tb4_bmp1);
    tb4->SetToolDropDown(ID_DropDownToolbarItem, true);
    tb4->SetCustomOverflowItems(prepend_items, append_items);
    wxChoice* choice = new wxChoice(tb4, ID_SampleItem+35);
    choice->AppendString("One choice");
    choice->AppendString("Another choice");
    tb4->AddControl(choice);
    tb4->Realize();


    wxAuiToolBar* tb5 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_VERTICAL);
    tb5->AddTool(ID_SampleItem+30, "Test", wxArtProvider::GetBitmapBundle(wxART_ERROR));
    tb5->AddSeparator();
    tb5->AddTool(ID_SampleItem+31, "Test", wxArtProvider::GetBitmapBundle(wxART_QUESTION));
    tb5->AddTool(ID_SampleItem+32, "Test", wxArtProvider::GetBitmapBundle(wxART_INFORMATION));
    tb5->AddTool(ID_SampleItem+33, "Test", wxArtProvider::GetBitmapBundle(wxART_WARNING));
    tb5->AddTool(ID_SampleItem+34, "Test", wxArtProvider::GetBitmapBundle(wxART_MISSING_IMAGE));
    tb5->SetCustomOverflowItems(prepend_items, append_items);
    tb5->Realize();

    // add a bunch of panes
    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test1").Caption("Pane Caption").
                  Top());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test2").Caption("Client Size Reporter").
                  Bottom().Position(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test3").Caption("Client Size Reporter").
                  Bottom().
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test4").Caption("Pane Caption").
                  Left());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test5").Caption("No Close Button").
                  Right().CloseButton(false));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test6").Caption("Client Size Reporter").
                  Right().Row(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test7").Caption("Client Size Reporter").
                  Left().Layer(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
                  Name("test8").Caption("Tree Pane").
                  Left().Layer(1).Position(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test9").Caption("Min Size 200x100").
                  BestSize(FromDIP(wxSize(200,100))).MinSize(FromDIP(wxSize(200,100))).
                  Bottom().Layer(1).
                  CloseButton(true).MaximizeButton(true));

    wxWindow* wnd10 = CreateTextCtrl("This pane will prompt the user before hiding.");

    // Give this pane an icon, too, just for testing.
    int iconSize = m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);

    // Make it even to use 16 pixel icons with default 17 caption height.
    iconSize &= ~1;

    m_mgr.AddPane(wnd10, wxAuiPaneInfo().
                  Name("test10").Caption("Text Pane with Hide Prompt").
                  Bottom().Layer(1).Position(1).
                  Icon(wxArtProvider::GetBitmapBundle(wxART_WARNING,
                                                      wxART_OTHER,
                                                      wxSize(iconSize, iconSize))));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test11").Caption("Fixed Pane").
                  Bottom().Layer(1).Position(2).Fixed());


    m_mgr.AddPane(new SettingsPanel(this,this), wxAuiPaneInfo().
                  Name("settings").Caption("Dock Manager Settings").
                  Dockable(false).Float().Hide());

    // create some center panes

    m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().Name("grid_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().Name("tree_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().Name("sizereport_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().Name("text_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().Name("html_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().Name("notebook_content").
                  CenterPane().PaneBorder(false));

    // add the toolbars to the manager
    m_mgr.AddPane(tb1, wxAuiPaneInfo().
                  Name("tb1").Caption("Big Toolbar").
                  ToolbarPane().Top());

    m_mgr.AddPane(tb2, wxAuiPaneInfo().
                  Name("tb2").Caption("Toolbar 2 (Horizontal)").
                  ToolbarPane().Top().Row(1));

    m_mgr.AddPane(tb3, wxAuiPaneInfo().
                  Name("tb3").Caption("Toolbar 3").
                  ToolbarPane().Top().Row(1).Position(1));

    m_mgr.AddPane(tb4, wxAuiPaneInfo().
                  Name("tb4").Caption("Sample Bookmark Toolbar").
                  ToolbarPane().Top().Row(2));

    m_mgr.AddPane(tb5, wxAuiPaneInfo().
                  Name("tb5").Caption("Sample Vertical Toolbar").
                  ToolbarPane().Left().
                  GripperTop());

    m_mgr.AddPane(new wxButton(this, wxID_ANY, _("Test Button")),
                  wxAuiPaneInfo().Name("tb6").
                  ToolbarPane().Top().Row(2).Position(1).
                  LeftDockable(false).RightDockable(false));

    // make some default perspectives

    wxString perspective_all = m_mgr.SavePerspective();

    int i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
        if (!all_panes.Item(i).IsToolbar())
            all_panes.Item(i).Hide();
    m_mgr.GetPane("tb1").Hide();
    m_mgr.GetPane("tb6").Hide();
    m_mgr.GetPane("test8").Show().Left().Layer(0).Row(0).Position(0);
    m_mgr.GetPane("test10").Show().Bottom().Layer(0).Row(0).Position(0);
    m_mgr.GetPane("notebook_content").Show();
    wxString perspective_default = m_mgr.SavePerspective();

    m_perspectives.Add(perspective_default);
    m_perspectives.Add(perspective_all);

    // "commit" all changes made to wxAuiManager
    m_mgr.Update();
}

wxAuiDockArt* MyFrame::GetDockArt()
{
    return m_mgr.GetArtProvider();
}

void MyFrame::DoUpdate()
{
    m_mgr.Update();
}

void MyFrame::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void MyFrame::OnSettings(wxCommandEvent& WXUNUSED(evt))
{
    // show the settings pane, and float it
    wxAuiPaneInfo& floating_pane = m_mgr.GetPane("settings").Float().Show();

    if (floating_pane.floating_pos == wxDefaultPosition)
        floating_pane.FloatingPosition(GetStartPosition());

    m_mgr.Update();
}

void MyFrame::OnCustomizeToolbar(wxCommandEvent& WXUNUSED(evt))
{
    wxMessageBox(_("Customize Toolbar clicked"));
}

void MyFrame::OnGradient(wxCommandEvent& event)
{
    int gradient = 0;

    switch (event.GetId())
    {
        case ID_NoGradient:         gradient = wxAUI_GRADIENT_NONE; break;
        case ID_VerticalGradient:   gradient = wxAUI_GRADIENT_VERTICAL; break;
        case ID_HorizontalGradient: gradient = wxAUI_GRADIENT_HORIZONTAL; break;
    }

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, gradient);
    m_mgr.Update();
}

void MyFrame::OnToolbarResizing(wxCommandEvent& WXUNUSED(evt))
{
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t count = all_panes.GetCount();
    for (size_t i = 0; i < count; ++i)
    {
        wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
        if (toolbar)
        {
            all_panes[i].Resizable(!all_panes[i].IsResizable());
        }
    }

    m_mgr.Update();
}

void MyFrame::OnManagerFlag(wxCommandEvent& event)
{
    unsigned int flag = 0;

#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)
    if (event.GetId() == ID_TransparentDrag ||
        event.GetId() == ID_TransparentHint ||
        event.GetId() == ID_HintFade)
    {
        wxMessageBox("This option is presently only available on wxGTK, wxMSW and wxMac");
        return;
    }
#endif

    int id = event.GetId();

    if (id == ID_TransparentHint ||
        id == ID_VenetianBlindsHint ||
        id == ID_RectangleHint ||
        id == ID_NoHint)
    {
        unsigned int flags = m_mgr.GetFlags();
        flags &= ~wxAUI_MGR_TRANSPARENT_HINT;
        flags &= ~wxAUI_MGR_VENETIAN_BLINDS_HINT;
        flags &= ~wxAUI_MGR_RECTANGLE_HINT;
        m_mgr.SetFlags(flags);
    }

    switch (id)
    {
        case ID_AllowFloating: flag = wxAUI_MGR_ALLOW_FLOATING; break;
        case ID_TransparentDrag: flag = wxAUI_MGR_TRANSPARENT_DRAG; break;
        case ID_HintFade: flag = wxAUI_MGR_HINT_FADE; break;
        case ID_NoVenetianFade: flag = wxAUI_MGR_NO_VENETIAN_BLINDS_FADE; break;
        case ID_AllowActivePane: flag = wxAUI_MGR_ALLOW_ACTIVE_PANE; break;
        case ID_TransparentHint: flag = wxAUI_MGR_TRANSPARENT_HINT; break;
        case ID_VenetianBlindsHint: flag = wxAUI_MGR_VENETIAN_BLINDS_HINT; break;
        case ID_RectangleHint: flag = wxAUI_MGR_RECTANGLE_HINT; break;
        case ID_LiveUpdate: flag = wxAUI_MGR_LIVE_RESIZE; break;
    }

    if (flag)
    {
        m_mgr.SetFlags(m_mgr.GetFlags() ^ flag);
    }

    m_mgr.Update();
}


void MyFrame::OnNotebookFlag(wxCommandEvent& event)
{
    int id = event.GetId();

    if (id == ID_NotebookNoCloseButton ||
        id == ID_NotebookCloseButton ||
        id == ID_NotebookCloseButtonAll ||
        id == ID_NotebookCloseButtonActive)
    {
        m_notebook_style &= ~(wxAUI_NB_CLOSE_BUTTON |
                              wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                              wxAUI_NB_CLOSE_ON_ALL_TABS);

        switch (id)
        {
            case ID_NotebookNoCloseButton: break;
            case ID_NotebookCloseButton: m_notebook_style |= wxAUI_NB_CLOSE_BUTTON; break;
            case ID_NotebookCloseButtonAll: m_notebook_style |= wxAUI_NB_CLOSE_ON_ALL_TABS; break;
            case ID_NotebookCloseButtonActive: m_notebook_style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB; break;
        }
    }

    if (id == ID_NotebookAllowTabMove)
    {
        m_notebook_style ^= wxAUI_NB_TAB_MOVE;
    }
    if (id == ID_NotebookAllowTabExternalMove)
    {
        m_notebook_style ^= wxAUI_NB_TAB_EXTERNAL_MOVE;
    }
     else if (id == ID_NotebookAllowTabSplit)
    {
        m_notebook_style ^= wxAUI_NB_TAB_SPLIT;
    }
     else if (id == ID_NotebookWindowList)
    {
        m_notebook_style ^= wxAUI_NB_WINDOWLIST_BUTTON;
    }
     else if (id == ID_NotebookScrollButtons)
    {
        m_notebook_style ^= wxAUI_NB_SCROLL_BUTTONS;
    }
     else if (id == ID_NotebookTabFixedWidth)
    {
        m_notebook_style ^= wxAUI_NB_TAB_FIXED_WIDTH;
    }


    size_t i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.window->IsKindOf(CLASSINFO(wxAuiNotebook)))
        {
            wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;

            if (id == ID_NotebookArtGloss)
            {
                nb->SetArtProvider(new wxAuiDefaultTabArt);
                m_notebook_theme = 0;
            }
             else if (id == ID_NotebookArtSimple)
            {
                nb->SetArtProvider(new wxAuiSimpleTabArt);
                m_notebook_theme = 1;
            }


            nb->SetWindowStyleFlag(m_notebook_style);
            nb->Refresh();
        }
    }


}


void MyFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
    unsigned int flags = m_mgr.GetFlags();

    switch (event.GetId())
    {
        case ID_NoGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE);
            break;
        case ID_VerticalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL);
            break;
        case ID_HorizontalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL);
            break;
        case ID_AllowToolbarResizing:
        {
            wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
            const size_t count = all_panes.GetCount();
            for (size_t i = 0; i < count; ++i)
            {
                wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
                if (toolbar)
                {
                    event.Check(all_panes[i].IsResizable());
                    break;
                }
            }
            break;
        }
        case ID_AllowFloating:
            event.Check((flags & wxAUI_MGR_ALLOW_FLOATING) != 0);
            break;
        case ID_TransparentDrag:
            event.Check((flags & wxAUI_MGR_TRANSPARENT_DRAG) != 0);
            break;
        case ID_TransparentHint:
            event.Check((flags & wxAUI_MGR_TRANSPARENT_HINT) != 0);
            break;
        case ID_LiveUpdate:
            event.Check((flags & wxAUI_MGR_LIVE_RESIZE) != 0);
            break;
        case ID_VenetianBlindsHint:
            event.Check((flags & wxAUI_MGR_VENETIAN_BLINDS_HINT) != 0);
            break;
        case ID_RectangleHint:
            event.Check((flags & wxAUI_MGR_RECTANGLE_HINT) != 0);
            break;
        case ID_NoHint:
            event.Check(((wxAUI_MGR_TRANSPARENT_HINT |
                          wxAUI_MGR_VENETIAN_BLINDS_HINT |
                          wxAUI_MGR_RECTANGLE_HINT) & flags) == 0);
            break;
        case ID_HintFade:
            event.Check((flags & wxAUI_MGR_HINT_FADE) != 0);
            break;
        case ID_NoVenetianFade:
            event.Check((flags & wxAUI_MGR_NO_VENETIAN_BLINDS_FADE) != 0);
            break;

        case ID_NotebookNoCloseButton:
            event.Check((m_notebook_style & (wxAUI_NB_CLOSE_BUTTON|wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_CLOSE_ON_ACTIVE_TAB)) != 0);
            break;
        case ID_NotebookCloseButton:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_BUTTON) != 0);
            break;
        case ID_NotebookCloseButtonAll:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_ON_ALL_TABS) != 0);
            break;
        case ID_NotebookCloseButtonActive:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_ON_ACTIVE_TAB) != 0);
            break;
        case ID_NotebookAllowTabSplit:
            event.Check((m_notebook_style & wxAUI_NB_TAB_SPLIT) != 0);
            break;
        case ID_NotebookAllowTabMove:
            event.Check((m_notebook_style & wxAUI_NB_TAB_MOVE) != 0);
            break;
        case ID_NotebookAllowTabExternalMove:
            event.Check((m_notebook_style & wxAUI_NB_TAB_EXTERNAL_MOVE) != 0);
            break;
        case ID_NotebookScrollButtons:
            event.Check((m_notebook_style & wxAUI_NB_SCROLL_BUTTONS) != 0);
            break;
        case ID_NotebookWindowList:
            event.Check((m_notebook_style & wxAUI_NB_WINDOWLIST_BUTTON) != 0);
            break;
        case ID_NotebookTabFixedWidth:
            event.Check((m_notebook_style & wxAUI_NB_TAB_FIXED_WIDTH) != 0);
            break;
        case ID_NotebookArtGloss:
            event.Check(m_notebook_style == 0);
            break;
        case ID_NotebookArtSimple:
            event.Check(m_notebook_style == 1);
            break;

    }
}

void MyFrame::OnPaneClose(wxAuiManagerEvent& evt)
{
    if (evt.pane->name == "test10")
    {
        int res = wxMessageBox("Are you sure you want to close/hide this pane?",
                               "wxAUI",
                               wxYES_NO,
                               this);
        if (res != wxYES)
            evt.Veto();
    }
}

void MyFrame::OnCreatePerspective(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dlg(this, "Enter a name for the new perspective:",
                          "wxAUI Test");

    dlg.SetValue(wxString::Format("Perspective %u", unsigned(m_perspectives.GetCount() + 1)));
    if (dlg.ShowModal() != wxID_OK)
        return;

    if (m_perspectives.GetCount() == 0)
    {
        m_perspectives_menu->AppendSeparator();
    }

    m_perspectives_menu->Append(ID_FirstPerspective + m_perspectives.GetCount(), dlg.GetValue());
    m_perspectives.Add(m_mgr.SavePerspective());
}

void MyFrame::OnCopyPerspectiveCode(wxCommandEvent& WXUNUSED(evt))
{
    wxString s = m_mgr.SavePerspective();

#if wxUSE_CLIPBOARD
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(s));
        wxTheClipboard->Close();
    }
#endif
}

void MyFrame::OnRestorePerspective(wxCommandEvent& evt)
{
    m_mgr.LoadPerspective(m_perspectives.Item(evt.GetId() - ID_FirstPerspective));
}

void MyFrame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(wxHtmlWindow)))
    {
        int res = wxMessageBox("Are you sure you want to close/hide this notebook page?",
                       "wxAUI",
                       wxYES_NO,
                       this);
        if (res != wxYES)
            evt.Veto();
    }
}

void MyFrame::OnNotebookPageClosed(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    wxUnusedVar(ctrl);

    // selection should always be a valid index
    wxASSERT_MSG( ctrl->GetSelection() < (int)ctrl->GetPageCount(),
                  wxString::Format("Invalid selection %d, only %d pages left",
                                   ctrl->GetSelection(),
                                   (int)ctrl->GetPageCount()) );

    evt.Skip();
}

void MyFrame::OnNotebookPageChanging(wxAuiNotebookEvent& evt)
{
    if ( evt.GetOldSelection() == 3 )
    {
        if ( wxMessageBox( "Are you sure you want to leave this page?\n"
                           "(This demonstrates veto-ing)",
                           "wxAUI",
                           wxICON_QUESTION | wxYES_NO,
                           this ) != wxYES )
        {
            evt.Veto();
        }
    }
}

void MyFrame::OnAllowNotebookDnD(wxAuiNotebookEvent& evt)
{
    // for the purpose of this test application, explicitly
    // allow all notebook drag and drop events
    evt.Allow();
}

wxPoint MyFrame::GetStartPosition()
{
    static int x = 0;
    x += FromDIP(20);
    wxPoint pt = ClientToScreen(wxPoint(0,0));
    return wxPoint(pt.x + x, pt.y + x);
}

void MyFrame::OnCreateTree(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
                  Caption("Tree Control").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(150,300))));
    m_mgr.Update();
}

void MyFrame::OnCreateGrid(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().
                  Caption("Grid").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(300,200))));
    m_mgr.Update();
}

void MyFrame::OnCreateHTML(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().
                  Caption("HTML Control").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(300,200))));
    m_mgr.Update();
}

void MyFrame::OnCreateNotebook(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().
                  Caption("Notebook").
                  Float().FloatingPosition(GetStartPosition()).
                  //FloatingSize(FromDIP(wxSize(300,200))).
                  CloseButton(true).MaximizeButton(true));
    m_mgr.Update();
}

void MyFrame::OnCreateText(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().
                  Caption("Text Control").
                  Float().FloatingPosition(GetStartPosition()));
    m_mgr.Update();
}

void MyFrame::OnCreateSizeReport(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Caption("Client Size Reporter").
                  Float().FloatingPosition(GetStartPosition()).
                  CloseButton(true).MaximizeButton(true));
    m_mgr.Update();
}

void MyFrame::OnChangeContentPane(wxCommandEvent& evt)
{
    m_mgr.GetPane("grid_content").Show(evt.GetId() == ID_GridContent);
    m_mgr.GetPane("text_content").Show(evt.GetId() == ID_TextContent);
    m_mgr.GetPane("tree_content").Show(evt.GetId() == ID_TreeContent);
    m_mgr.GetPane("sizereport_content").Show(evt.GetId() == ID_SizeReportContent);
    m_mgr.GetPane("html_content").Show(evt.GetId() == ID_HTMLContent);
    m_mgr.GetPane("notebook_content").Show(evt.GetId() == ID_NotebookContent);
    m_mgr.Update();
}

void MyFrame::OnDropDownToolbarItem(wxAuiToolBarEvent& evt)
{
    if (evt.IsDropDownClicked())
    {
        wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());

        tb->SetToolSticky(evt.GetId(), true);

        // create the popup menu
        wxMenu menuPopup;

        // TODO: Use GetBitmapBundle() when wxMenuItem is updated to use it too.
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, FromDIP(wxSize(16,16)));

        wxMenuItem* m1 =  new wxMenuItem(&menuPopup, 10001, _("Drop Down Item 1"));
        m1->SetBitmap(bmp);
        menuPopup.Append(m1);

        wxMenuItem* m2 =  new wxMenuItem(&menuPopup, 10002, _("Drop Down Item 2"));
        m2->SetBitmap(bmp);
        menuPopup.Append(m2);

        wxMenuItem* m3 =  new wxMenuItem(&menuPopup, 10003, _("Drop Down Item 3"));
        m3->SetBitmap(bmp);
        menuPopup.Append(m3);

        wxMenuItem* m4 =  new wxMenuItem(&menuPopup, 10004, _("Drop Down Item 4"));
        m4->SetBitmap(bmp);
        menuPopup.Append(m4);

        // line up our menu with the button
        wxRect rect = tb->GetToolRect(evt.GetId());
        wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);


        PopupMenu(&menuPopup, pt);


        // make sure the button is "un-stuck"
        tb->SetToolSticky(evt.GetId(), false);
    }
}


void MyFrame::OnTabAlignment(wxCommandEvent &evt)
{
    size_t i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.window->IsKindOf(CLASSINFO(wxAuiNotebook)))
        {
            wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;

            long style = nb->GetWindowStyleFlag();
            style &= ~(wxAUI_NB_TOP | wxAUI_NB_BOTTOM);
            if (evt.GetId() == ID_NotebookAlignTop)
                style |= wxAUI_NB_TOP;
            else if (evt.GetId() == ID_NotebookAlignBottom)
                style |= wxAUI_NB_BOTTOM;
            nb->SetWindowStyleFlag(style);

            nb->Refresh();
        }
    }
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_("wxAUI Demo\nAn advanced window management library for wxWidgets\n(c) Copyright 2005-2006, Kirix Corporation"), _("About wxAUI Demo"), wxOK, this);
}

wxTextCtrl* MyFrame::CreateTextCtrl(const wxString& ctrl_text)
{
    static int n = 0;

    wxString text;
    if ( !ctrl_text.empty() )
        text = ctrl_text;
    else
        text.Printf("This is text box %d", ++n);

    return new wxTextCtrl(this,wxID_ANY, text,
                          wxPoint(0,0), FromDIP(wxSize(150,90)),
                          wxNO_BORDER | wxTE_MULTILINE);
}


wxGrid* MyFrame::CreateGrid()
{
    wxGrid* grid = new wxGrid(this, wxID_ANY,
                              wxPoint(0,0),
                              FromDIP(wxSize(150,250)),
                              wxNO_BORDER | wxWANTS_CHARS);
    grid->CreateGrid(50, 20);
    return grid;
}

wxTreeCtrl* MyFrame::CreateTreeCtrl()
{
    wxTreeCtrl* tree = new wxTreeCtrl(this, wxID_ANY,
                                      wxPoint(0,0),
                                      FromDIP(wxSize(160,250)),
                                      wxTR_DEFAULT_STYLE | wxNO_BORDER);

    wxSize size(16, 16);
    wxVector<wxBitmapBundle> images;
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_FOLDER, wxART_OTHER, size));
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, size));
    tree->SetImages(images);

    wxTreeItemId root = tree->AddRoot("wxAUI Project", 0);
    wxArrayTreeItemIds items;



    items.Add(tree->AppendItem(root, "Item 1", 0));
    items.Add(tree->AppendItem(root, "Item 2", 0));
    items.Add(tree->AppendItem(root, "Item 3", 0));
    items.Add(tree->AppendItem(root, "Item 4", 0));
    items.Add(tree->AppendItem(root, "Item 5", 0));


    int i, count;
    for (i = 0, count = items.Count(); i < count; ++i)
    {
        wxTreeItemId id = items.Item(i);
        tree->AppendItem(id, "Subitem 1", 1);
        tree->AppendItem(id, "Subitem 2", 1);
        tree->AppendItem(id, "Subitem 3", 1);
        tree->AppendItem(id, "Subitem 4", 1);
        tree->AppendItem(id, "Subitem 5", 1);
    }


    tree->Expand(root);

    return tree;
}

wxSizeReportCtrl* MyFrame::CreateSizeReportCtrl(const wxSize& size)
{
    wxSizeReportCtrl* ctrl = new wxSizeReportCtrl(this, wxID_ANY,
                                   wxDefaultPosition,
                                   size, &m_mgr);
    return ctrl;
}

wxHtmlWindow* MyFrame::CreateHTMLCtrl(wxWindow* parent)
{
    if (!parent)
        parent = this;

    wxHtmlWindow* ctrl = new wxHtmlWindow(parent, wxID_ANY,
                                   wxDefaultPosition,
                                   FromDIP(wxSize(400,300)));
    ctrl->SetPage(GetIntroText());
    return ctrl;
}

wxAuiNotebook* MyFrame::CreateNotebook()
{
   // create the notebook off-window to avoid flicker
   wxSize client_size = GetClientSize();

   wxAuiNotebook* ctrl = new wxAuiNotebook(this, wxID_ANY,
                                    wxPoint(client_size.x, client_size.y),
                                    FromDIP(wxSize(430,200)),
                                    m_notebook_style);
   ctrl->Freeze();

   wxBitmapBundle page_bmp = wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));

   ctrl->AddPage(CreateHTMLCtrl(ctrl), "Welcome to wxAUI" , false, page_bmp);
   ctrl->SetPageToolTip(0, "Welcome to wxAUI (this is a page tooltip)");

   wxPanel *panel = new wxPanel( ctrl, wxID_ANY );
   wxFlexGridSizer *flex = new wxFlexGridSizer( 4, 2, 0, 0 );
   flex->AddGrowableRow( 0 );
   flex->AddGrowableRow( 3 );
   flex->AddGrowableCol( 1 );
   flex->Add( FromDIP(5), FromDIP(5) );   flex->Add( FromDIP(5), FromDIP(5) );
   flex->Add( new wxStaticText( panel, -1, "wxTextCtrl:" ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxTextCtrl( panel, -1, "", wxDefaultPosition, FromDIP(wxSize(100,-1))),
                1, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxStaticText( panel, -1, "wxSpinCtrl:" ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxSpinCtrl( panel, -1, "5", wxDefaultPosition, wxDefaultSize,
                wxSP_ARROW_KEYS, 5, 50, 5 ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( FromDIP(5), FromDIP(5) );   flex->Add( FromDIP(5), FromDIP(5) );
   panel->SetSizer( flex );
   ctrl->AddPage( panel, "wxPanel", false, page_bmp );


   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 1", false, page_bmp );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 2" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 3" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 4" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 5" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 6" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 7 (longer title)" );
   ctrl->SetPageToolTip(ctrl->GetPageCount()-1,
                        "wxTextCtrl 7: and the tooltip message can be even longer!");

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 8" );

   ctrl->Thaw();
   return ctrl;
}

wxString MyFrame::GetIntroText()
{
    const char* text =
        "<html><body>"
        "<h3>Welcome to wxAUI</h3>"
        "<br/><b>Overview</b><br/>"
        "<p>wxAUI is an Advanced User Interface library for the wxWidgets toolkit "
        "that allows developers to create high-quality, cross-platform user "
        "interfaces quickly and easily.</p>"
        "<p><b>Features</b></p>"
        "<p>With wxAUI, developers can create application frameworks with:</p>"
        "<ul>"
        "<li>Native, dockable floating frames</li>"
        "<li>Perspective saving and loading</li>"
        "<li>Native toolbars incorporating real-time, &quot;spring-loaded&quot; dragging</li>"
        "<li>Customizable floating/docking behaviour</li>"
        "<li>Completely customizable look-and-feel</li>"
        "<li>Optional transparent window effects (while dragging or docking)</li>"
        "<li>Splittable notebook control</li>"
        "</ul>"
        "<p><b>What's new in 0.9.4?</b></p>"
        "<p>wxAUI 0.9.4, which is bundled with wxWidgets, adds the following features:"
        "<ul>"
        "<li>New wxAuiToolBar class, a toolbar control which integrates more "
        "cleanly with wxAuiFrameManager.</li>"
        "<li>Lots of bug fixes</li>"
        "</ul>"
        "<p><b>What's new in 0.9.3?</b></p>"
        "<p>wxAUI 0.9.3, which is now bundled with wxWidgets, adds the following features:"
        "<ul>"
        "<li>New wxAuiNotebook class, a dynamic splittable notebook control</li>"
        "<li>New wxAuiMDI* classes, a tab-based MDI and drop-in replacement for classic MDI</li>"
        "<li>Maximize/Restore buttons implemented</li>"
        "<li>Better hinting with wxGTK</li>"
        "<li>Class rename.  'wxAui' is now the standard class prefix for all wxAUI classes</li>"
        "<li>Lots of bug fixes</li>"
        "</ul>"
        "<p><b>What's new in 0.9.2?</b></p>"
        "<p>The following features/fixes have been added since the last version of wxAUI:</p>"
        "<ul>"
        "<li>Support for wxMac</li>"
        "<li>Updates for wxWidgets 2.6.3</li>"
        "<li>Fix to pass more unused events through</li>"
        "<li>Fix to allow floating windows to receive idle events</li>"
        "<li>Fix for minimizing/maximizing problem with transparent hint pane</li>"
        "<li>Fix to not paint empty hint rectangles</li>"
        "<li>Fix for 64-bit compilation</li>"
        "</ul>"
        "<p><b>What changed in 0.9.1?</b></p>"
        "<p>The following features/fixes were added in wxAUI 0.9.1:</p>"
        "<ul>"
        "<li>Support for MDI frames</li>"
        "<li>Gradient captions option</li>"
        "<li>Active/Inactive panes option</li>"
        "<li>Fix for screen artifacts/paint problems</li>"
        "<li>Fix for hiding/showing floated window problem</li>"
        "<li>Fix for floating pane sizing problem</li>"
        "<li>Fix for drop position problem when dragging around center pane margins</li>"
        "<li>LF-only text file formatting for source code</li>"
        "</ul>"
        "<p>See README.txt for more information.</p>"
        "</body></html>";

    return wxString::FromAscii(text);
}
#endif