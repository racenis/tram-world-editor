#include <glad.h>

#include <editor/editor.h>
#include <editor/actions.h>
#include <editor/settings.h>

#include <editor/objects/entity.h>

#include <widgets/viewport.h>
#include <widgets/objectmenu.h>
#include <widgets/mainframe.h>

using namespace Editor;

#include <framework/core.h>
#include <framework/async.h>
#include <framework/language.h>
#include <framework/system.h>
#include <render/render.h>
#include <render/renderer.h>
#include <render/api.h>
#include <render/aabb.h>

#include <components/render.h>

ViewportCtrl* viewport = nullptr;

float Editor::Viewport::CURSOR_X = 0.0f;
float Editor::Viewport::CURSOR_Y = 0.0f;
float Editor::Viewport::CURSOR_Z = 0.0f;

void Editor::Viewport::Refresh() {
    viewport->Refresh();
}

void Editor::Viewport::SetCurrentSelection() {
    viewport->Refresh();
}

void Editor::Viewport::ShowErrorDialog(std::string msg) {
    wxMessageBox(msg, "An error occured!", wxOK | wxICON_ERROR | wxSTAY_ON_TOP);
}

ViewportCtrl::ViewportCtrl(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, 0L, L"GLCanvas", wxNullPalette), key_timer(this) {
    //assert(GetParent()->IsShown());
    
    
    // performing regular tram-sdk initialization

    tram::Core::Init();
    
    // we're not using framework's UI system, so we'll do it ourselves
    
    m_context = new wxGLContext(this);
    SetCurrent(*m_context);
    gladLoadGL();

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    tram::System::SetInitialized(tram::System::SYSTEM_UI, true);
    
    tram::Render::Init();
    tram::Async::Init(0);
    
    //Material::SetErrorMaterial(new Material(UID("defaulttexture"), Material::TEXTURE));
    //Model::SetErrorModel(new Model(UID("errorstatic")));

    //LoadText("data/lv.lang");
    
    
    tram::Language::Load("lv");
    
    tram::Render::Material::LoadMaterialInfo("material");
    
    // create the mongus model
    //monguser = PoolProxy<RenderComponent>::New();
    //monguser->SetModel(UID("mongus"));
    //monguser->SetPose(poseList.begin());
    //monguser->Init();
    //monguser->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    //monguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
    
    //tram::Render::CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 5.0f);
    //tram::Render::CAMERA_ROTATION = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    tram::Render::SetCameraPosition({0.0f, 5.0f, 0.0f});
    
    Bind(wxEVT_PAINT, &ViewportCtrl::OnPaint, this);
    Bind(wxEVT_LEFT_UP, &ViewportCtrl::OnLeftClick, this);
    Bind(wxEVT_RIGHT_UP, &ViewportCtrl::OnRightClick, this);
    Bind(wxEVT_MOTION, &ViewportCtrl::OnMouseMove, this);
    Bind(wxEVT_SIZE, &ViewportCtrl::OnSizeChange, this);
    Bind(wxEVT_KEY_DOWN, &ViewportCtrl::OnKeydown, this);
    Bind(wxEVT_KEY_UP, &ViewportCtrl::OnKeyup, this);
    Bind(wxEVT_TIMER, &ViewportCtrl::OnTimer, this);
}

ViewportCtrl::~ViewportCtrl()
{
    tram::Async::Yeet();
    SetCurrent(*m_context);
}

void ViewportCtrl::CenterMouseCursor() {
    int width, height;
    GetSize(&width, &height);
    WarpPointer(width/2, height/2);
}

void ViewportCtrl::CancelViewportOperation() {
    ReleaseMouse();
    Refresh();
    
    viewport_axis = AXIS_NONE;
    viewport_mode = MODE_NONE;
}

void ViewportCtrl::OnLeftClick(wxMouseEvent& event) {
    
    // if viewport is in move mode or translate mode, cancel them
    if (viewport_mode != MODE_NONE) {
        CancelViewportOperation();
        return;
    }
    
    // if control key is pressed down, select object in viewport
    if (wxGetKeyState(WXK_CONTROL)) {
        tram::vec3 far_point = tram::Render::ProjectInverse({event.GetX(), event.GetY(), 0.0f});
        tram::vec3 near_point = tram::Render::ProjectInverse({event.GetX(), event.GetY(), 1000.0f});
        
        tram::vec3 look_direction = glm::normalize(far_point - near_point);
        tram::vec3 look_position = near_point;
        
        auto res = tram::Render::AABB::FindNearestFromRay(look_position, look_direction, -1);
        
        if (res.data) {
            std::cout << GetEntityFromViewmodel((tram::RenderComponent*) res.data)->GetName() << std::endl;
            
            // create a new selection object
            auto new_selection = std::make_shared<Editor::Selection>();
            
            // if shift is pressed down, then multiple selection mode
            if (wxGetKeyState(WXK_SHIFT)) {
                new_selection->objects = Editor::SELECTION->objects;
            }
            
            // lookup selected object and try to find it in new selection
            auto selected_object = GetEntityFromViewmodel((tram::RenderComponent*) res.data)->GetPointer();
            auto already_selected = std::find(new_selection->objects.begin(), new_selection->objects.end(), selected_object);
            
            // if already selected, deselect. otherwise append to selection
            if (already_selected == new_selection->objects.end()) {
                new_selection->objects.push_back(selected_object);
            } else {
                new_selection->objects.erase(already_selected);
            }
            
            // single selection override
            if (Editor::SELECTION->objects.size() == 1 && Editor::SELECTION->objects.front() == selected_object) {
                new_selection->objects.clear();
            }

            // set new selection as editor selection
            Editor::PerformAction<Editor::ActionChangeSelection>(new_selection);
        } else {
            if (wxGetKeyState(WXK_SHIFT)) return;
            
            // clear selection
            Editor::PerformAction<Editor::ActionChangeSelection>(std::make_shared<Editor::Selection>());
        }
        
        return;
    }
    
    // otherwise
    
    CaptureMouse();
    CenterMouseCursor();
    
    viewport_mode = MODE_MOVE;
}

void ViewportCtrl::OnRightClick(wxMouseEvent& event) {
    if (viewport_mode == MODE_TRANSLATE || viewport_mode == MODE_ROTATE) {
        Editor::Undo();
        
        CancelViewportOperation();
        
        return;
    }
    
    if (viewport_mode == MODE_NONE) {
        world_tree_popup->SetSelectionStatus(Editor::SELECTION.get());
        main_frame->PopupMenu(world_tree_popup);
    }
}

void ViewportCtrl::OnMouseMove(wxMouseEvent& event) {
    if (viewport_mode != MODE_NONE) {
        int width, height;
        GetSize(&width, &height);
        int center_x = width/2, center_y = height/2;
        
        float delta_x = (float)(event.GetX() - center_x);
        float delta_y = (float)(event.GetY() - center_y);
        WarpPointer(center_x, center_y);
        
        
        if (viewport_mode == MODE_MOVE) {
            mouse_x += delta_x * 0.1f;
            mouse_y += delta_y * 0.1f;
            
            mouse_y = mouse_y > 90.0f ? 90.0f : mouse_y < -90.0f ? -90.0f : mouse_y;
            tram::Render::SetCameraRotation(glm::quat(glm::vec3(-glm::radians(mouse_y), -glm::radians(mouse_x), 0.0f)));
            
            using namespace tram;
            using namespace tram::Render;

            vec3 camera_pos = GetCameraPosition();
            quat camera_rot = GetCameraRotation();
            
            vec3 cursor_pos = camera_pos + camera_rot * DIRECTION_FORWARD * 2.0f;
            
            Viewport::CURSOR_X = cursor_pos.x;
            Viewport::CURSOR_Y = cursor_pos.y;
            Viewport::CURSOR_Z = cursor_pos.z;
        } else if (viewport_mode == MODE_TRANSLATE) {
            glm::vec3 translation = glm::vec3 (0.0, 0.0f, 0.0f);
            
            if (viewport_axis & AXIS_X) translation += glm::vec3 (0.05f, 0.0f, 0.0f);
            if (viewport_axis & AXIS_Y) translation += glm::vec3 (0.0f, 0.05f, 0.0f);
            if (viewport_axis & AXIS_Z) translation += glm::vec3 (0.0f, 0.0f, 0.05f);
            
            translation *= delta_x + delta_x;
            
            for (auto& object : Editor::SELECTION->objects) {
                glm::vec3 object_position = glm::vec3 {
                    object->GetProperty("position-x").float_value,
                    object->GetProperty("position-y").float_value,
                    object->GetProperty("position-z").float_value
                };
                
                auto translation_adjusted = translation;
                
                if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) {
                    glm::vec3 object_rotation = glm::vec3 {
                        object->GetProperty("rotation-x").float_value,
                        object->GetProperty("rotation-y").float_value,
                        object->GetProperty("rotation-z").float_value
                    };
                    
                    translation_adjusted = glm::quat(object_rotation) * translation;
                }
                
                if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITYGROUP) {
                    // TODO: implement entity group transforms
                    Editor::Viewport::ShowErrorDialog("EntityGroup transform not implemented yet!");
                }
                
                object_position += translation_adjusted;
                                
                object->SetProperty("position-x", object_position.x);
                object->SetProperty("position-y", object_position.y);
                object->SetProperty("position-z", object_position.z);
            }
        } else if (viewport_mode == MODE_ROTATE) {
            glm::vec3 rotation = glm::vec3 (0.0, 0.0f, 0.0f);
            
            if (viewport_axis & AXIS_X) rotation += glm::vec3 (0.05f, 0.0f, 0.0f);
            if (viewport_axis & AXIS_Y) rotation += glm::vec3 (0.0f, 0.05f, 0.0f);
            if (viewport_axis & AXIS_Z) rotation += glm::vec3 (0.0f, 0.0f, 0.05f);
            
            rotation *= delta_x + delta_x;
            
            for (auto& object : Editor::SELECTION->objects) {
                glm::quat object_rotation = glm::vec3 {
                    object->GetProperty("rotation-x").float_value,
                    object->GetProperty("rotation-y").float_value,
                    object->GetProperty("rotation-z").float_value
                };
                
                if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_WORLD) {
                    object_rotation = glm::quat(rotation) * object_rotation;
                }
                
                if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) {
                    object_rotation = object_rotation * glm::quat(rotation);
                }
                
                if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITYGROUP) {
                    // TODO: implement entity group transforms
                }
                
                auto rotation_euler = glm::eulerAngles(object_rotation);
                
                object->SetProperty("rotation-x", rotation_euler.x);
                object->SetProperty("rotation-y", rotation_euler.y);
                object->SetProperty("rotation-z", rotation_euler.z);
            }
        }
        

        
        Refresh();
    }
}

void ViewportCtrl::OnKeydown(wxKeyEvent& event) {
    auto keycode = event.GetUnicodeKey();
    bool movement_key_pressed = false;
    bool any_movement = key_forward || key_backward || key_left  || key_right;
    
    if (keycode == 'W') {
        movement_key_pressed = true;
        key_forward = true;
    }
    
    if (keycode == 'A') {
        movement_key_pressed = true;
        key_left = true;
    }
    
    if (keycode == 'S') {
        movement_key_pressed = true;
        key_backward = true;
    }
    
    if (keycode == 'D') {
        movement_key_pressed = true;
        key_right = true;
    }
    
    if (!any_movement && movement_key_pressed) {
        key_timer.Start(30);
    }
    
    if (keycode == 'X') {
        viewport_axis ^= AXIS_X;
    }
    
    if (keycode == 'Y') {
        viewport_axis ^= AXIS_Y;
    }
    
    if (keycode == 'Z') {
        viewport_axis ^= AXIS_Z;
    }
    
    if (keycode == 'G') {
        if (viewport_mode == MODE_NONE) {
            CaptureMouse();
            CenterMouseCursor();
        }
        viewport_mode = MODE_TRANSLATE;
        
        Editor::PerformAction<Editor::ActionChangeProperties>(std::vector<std::string> {"position-x", "position-y", "position-z"});
    }
    
    if (keycode == 'R') {
        if (viewport_mode == MODE_NONE) {
            CaptureMouse();
            CenterMouseCursor();
        }
        viewport_mode = MODE_ROTATE;
        
        Editor::PerformAction<Editor::ActionChangeProperties>(std::vector<std::string> {"rotation-x", "rotation-y", "rotation-z"});
    }
    
    if (event.GetKeyCode() == WXK_SHIFT) {
        key_shift = true;
    }
    
    if (event.GetKeyCode() == WXK_CONTROL && viewport_mode == MODE_TRANSLATE) {
        for (auto& object : Editor::SELECTION->objects) {
            float x = object->GetProperty("position-x");
            float y = object->GetProperty("position-y");
            float z = object->GetProperty("position-z");
            
            float div = 1.0f;
            switch (Editor::Settings::TRANSLATION_SNAP) {
                case Editor::Settings::SNAP_0_01: div = 0.01f; break;
                case Editor::Settings::SNAP_0_10: div = 0.1f; break;
                case Editor::Settings::SNAP_0_25: div = 0.25f; break;
                case Editor::Settings::SNAP_0_50: div = 0.5f; break;
                default:
                case Editor::Settings::SNAP_1_00: div = 1.0f; break;
            }
            
            object->SetProperty("position-x", round(x/div)*div);
            object->SetProperty("position-y", round(y/div)*div);
            object->SetProperty("position-z", round(z/div)*div);
        }
        
        Editor::Viewport::Refresh();
    }
    
     if (event.GetKeyCode() == WXK_CONTROL && viewport_mode == MODE_ROTATE) {
        for (auto& object : Editor::SELECTION->objects) {
            float x = object->GetProperty("rotation-x");
            float y = object->GetProperty("rotation-y");
            float z = object->GetProperty("rotation-z");
            
            float div = 1.0f;
            switch (Editor::Settings::ROTATION_SNAP) {
                case Editor::Settings::SNAP_15: div = 0.261799f; break;
                case Editor::Settings::SNAP_30: div = 0.523599f; break;
                case Editor::Settings::SNAP_45: div = 0.785398f; break;
                default:
                case Editor::Settings::SNAP_90: div = 1.5708f; break;
            }
            
            object->SetProperty("rotation-x", round(x/div)*div);
            object->SetProperty("rotation-y", round(y/div)*div);
            object->SetProperty("rotation-z", round(z/div)*div);
        }
        
        Editor::Viewport::Refresh();
    }
}

void ViewportCtrl::OnKeyup(wxKeyEvent& event) {
    auto keycode = event.GetUnicodeKey();
    
    if (keycode == 'W') {
        key_forward = false;
    }
    
    if (keycode == 'A') {
        key_left = false;
    }
    
    if (keycode == 'S') {
        key_backward = false;
    }
    
    if (keycode == 'D') {
        key_right = false;
    }
    
    if (!(key_forward || key_backward || key_left  || key_right)) {
        key_timer.Stop();
    }
    
    if (event.GetKeyCode() == WXK_SHIFT) {
        key_shift = false;
    }
}

void ViewportCtrl::OnTimer(wxTimerEvent& event) {
    using namespace tram::Render;
    using namespace tram;
    
    //if (key_forward) CAMERA_POSITION += CAMERA_ROTATION * DIRECTION_FORWARD * 0.1f;
    //if (key_backward) CAMERA_POSITION -= CAMERA_ROTATION * DIRECTION_FORWARD * 0.1f;
    //if (key_left) CAMERA_POSITION -= CAMERA_ROTATION * DIRECTION_SIDE * 0.1f;
    //if (key_right) CAMERA_POSITION += CAMERA_ROTATION * DIRECTION_SIDE * 0.1f;
    
    vec3 camera_position = GetCameraPosition();
    quat camera_rotation = GetCameraRotation();
    
    float move_speed = key_shift ? 0.3f : 0.1f; 
    
    if (key_forward) camera_position += camera_rotation * DIRECTION_FORWARD * move_speed;
    if (key_backward) camera_position -= camera_rotation * DIRECTION_FORWARD * move_speed;
    if (key_left) camera_position -= camera_rotation * DIRECTION_SIDE * move_speed;
    if (key_right) camera_position += camera_rotation * DIRECTION_SIDE * move_speed;
    
    SetCameraPosition(camera_position);
    
    Refresh();
}

void ViewportCtrl::OnSizeChange(wxSizeEvent& event) {
    //assert(GetParent()->IsShown());
    
    int width, height;
    GetSize(&width, &height);
    //glViewport(0, 0, width, height);
    //return;
    
    tram::Render::SetScreenSize(width, height);
}

void ViewportCtrl::ViewCenterOnSelection() {
    using namespace tram;
    using namespace tram::Render;
    
    if (!Editor::SELECTION->objects.size()) return;
    
    Object* target = Editor::SELECTION->objects.begin()->get();
    
    vec3 camera_pos = GetCameraPosition();
    quat camera_rot = GetCameraRotation();
    
    vec3 target_pos = {target->GetProperty("position-x"), target->GetProperty("position-y"), target->GetProperty("position-z")};
    
    vec3 to_target = glm::normalize(target_pos - camera_pos);
    
    camera_pos = target_pos + (to_target * -10.0f);
    camera_rot = glm::quatLookAt(to_target, DIRECTION_UP);
    
    SetCameraPosition(camera_pos);
    SetCameraRotation(camera_rot);
    
    Refresh();
}

void ViewportCtrl::OnPaint(wxPaintEvent& event)
{
    //assert(GetParent()->IsShown());
    //if (!GetParent()->IsShown()) return;
        
    //SetCurrent(*m_context);

    using namespace tram;
    using namespace tram::Render;

    for (auto& object : Editor::SELECTION->objects) {
        glm::quat space;
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_WORLD) space = glm::vec3(0.0f);
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) space = glm::quat(glm::vec3 {
                    object->GetProperty("rotation-x").float_value,
                    object->GetProperty("rotation-y").float_value,
                    object->GetProperty("rotation-z").float_value
                });
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITYGROUP) space = glm::vec3(glm::radians(180.0f));
        
        object->Draw();
        
        if (viewport_mode == MODE_TRANSLATE || viewport_mode == MODE_ROTATE) {
            glm::vec3 gizmo_location = glm::vec3 {
                object->GetProperty("position-x").float_value,
                object->GetProperty("position-y").float_value,
                object->GetProperty("position-z").float_value
            };
            AddLine(gizmo_location, gizmo_location + space * glm::vec3(1.0f, 0.0f, 0.0f), COLOR_RED);
            AddLine(gizmo_location, gizmo_location + space * glm::vec3(0.0f, 1.0f, 0.0f), COLOR_GREEN);
            AddLine(gizmo_location, gizmo_location + space * glm::vec3(0.0f, 0.0f, 1.0f), COLOR_BLUE);
            
            if (viewport_axis & AXIS_X) AddLine(gizmo_location - space * glm::vec3(100.0f, 0.0f, 0.0f), gizmo_location + space * glm::vec3(100.0f, 0.0f, 0.0f), COLOR_RED);
            if (viewport_axis & AXIS_Y) AddLine(gizmo_location - space * glm::vec3(0.0f, 100.0f, 0.0f), gizmo_location + space * glm::vec3(0.0f, 100.0f, 0.0f), COLOR_GREEN);
            if (viewport_axis & AXIS_Z) AddLine(gizmo_location - space * glm::vec3(0.0f, 0.0f, 100.0f), gizmo_location + space * glm::vec3(0.0f, 0.0f, 100.0f), COLOR_BLUE);
        }
        
        auto gizmos = object->GetWidgetDefinitions();
        
        for (const auto& gizmo : gizmos) {
            glm::vec3 color;
            switch (gizmo.color) {
                case Editor::WidgetDefinition::WIDGET_CYAN:
                default:
                    color = {0.0f, 1.0f, 1.0f};
            }
            switch (gizmo.type) {
                case Editor::WidgetDefinition::WIDGET_NORMAL: {
                    PropertyValue::Vector dir_vec = object->GetProperty(gizmo.property);
                    glm::vec3 dir = {dir_vec.x, dir_vec.y, dir_vec.z};
                    glm::vec3 pos = glm::vec3 {
                        object->GetProperty("position-x").float_value,
                        object->GetProperty("position-y").float_value,
                        object->GetProperty("position-z").float_value
                    };
                    AddLine(pos, pos+dir, color);
                    } break;
                case Editor::WidgetDefinition::WIDGET_POINT:{
                    PropertyValue::Vector pos_vec = object->GetProperty(gizmo.property);
                    glm::vec3 pos = {pos_vec.x, pos_vec.y, pos_vec.z};
                    AddLineMarker(pos, color);
                    } break;
                    
                default:
                    color = {0.0f, 1.0f, 1.0f};
            }
        }
    }
    

    AddLineMarker(glm::vec3(0.0f, 0.0f, 0.0f), COLOR_CYAN);
    
    //SetSun(time_of_day);
    //SetSun(0.8f);

    Async::ResourceLoader1stStage();
    Async::ResourceLoader2ndStage();
    Async::FinishResource();
    
    //Render::UpdateArmatures();
    Render::Render();

    glFlush();
    SwapBuffers();
}