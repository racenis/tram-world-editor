#include <glad.h>

#include <editor.h>
#include <actions.h>
#include <widgets.h>
#include <viewport.h>

using namespace Editor;

#include <core.h>
#include <async.h>
#include <render/render.h>

#include <components/rendercomponent.h>

void Editor::Viewport::Refresh() {
    viewport->Refresh();
}

void Editor::Viewport::SetCurrentSelection() {
    viewport->Refresh();
}

ViewportCtrl::ViewportCtrl(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, 0L, L"GLCanvas", wxNullPalette), key_timer(this) {
    m_context = new wxGLContext(this);
    Bind(wxEVT_PAINT, &ViewportCtrl::OnPaint, this);
    Bind(wxEVT_LEFT_UP, &ViewportCtrl::OnLeftClick, this);
    Bind(wxEVT_RIGHT_UP, &ViewportCtrl::OnRightClick, this);
    Bind(wxEVT_MOTION, &ViewportCtrl::OnMouseMove, this);
    Bind(wxEVT_SIZE, &ViewportCtrl::OnSizeChange, this);
    Bind(wxEVT_KEY_DOWN, &ViewportCtrl::OnKeydown, this);
    Bind(wxEVT_KEY_UP, &ViewportCtrl::OnKeyup, this);
    Bind(wxEVT_TIMER, &ViewportCtrl::OnTimer, this);

    SetCurrent(*m_context);

    gladLoadGL();

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    using namespace Core;
    using namespace Core::Render;
    Core::Init();
    Core::Render::Init();
    Core::Async::Init();
    
    Material::SetErrorMaterial(new Material(UID("defaulttexture"), Material::TEXTURE));
    Model::SetErrorModel(new Model(UID("errorstatic")));

    LoadText("data/lv.lang");
    
    Material::LoadMaterialInfo("data/texture.list");
    
    // create the mongus model
    //monguser = PoolProxy<RenderComponent>::New();
    //monguser->SetModel(UID("mongus"));
    //monguser->SetPose(poseList.begin());
    //monguser->Init();
    //monguser->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    //monguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
    
    CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 5.0f);
    CAMERA_ROTATION = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
}

ViewportCtrl::~ViewportCtrl()
{
    Core::Async::Yeet();
    SetCurrent(*m_context);
}

void ViewportCtrl::CenterMouseCursor() {
    int width, height;
    GetSize(&width, &height);
    WarpPointer(width/2, height/2);
}

void ViewportCtrl::OnLeftClick(wxMouseEvent& event) {
    if (viewport_mode == MODE_NONE) {
        CaptureMouse();
        CenterMouseCursor();
        
        viewport_mode = MODE_MOVE;
        std::cout << "captured and centered the mouse!" << std::endl;
    } else {
        ReleaseMouse();
        Refresh();
        
        viewport_axis = AXIS_NONE;
        viewport_mode = MODE_NONE;
        std::cout << "released the mouse!" << std::endl;
    }
}

void ViewportCtrl::OnRightClick(wxMouseEvent& event) {
    if (viewport_mode == MODE_TRANSLATE || viewport_mode == MODE_ROTATE) {
        // TODO: do an undo in here
        std::cout << "Canceling a transform not implemented!" << std::endl;
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
            Core::Render::CAMERA_ROTATION = glm::quat(glm::vec3(-glm::radians(mouse_y), -glm::radians(mouse_x), 0.0f));
        } else if (viewport_mode == MODE_TRANSLATE) {
            glm::vec3 translation = glm::vec3 (0.0, 0.0f, 0.0f);
            
            if (viewport_axis & AXIS_X) translation += glm::vec3 (0.05f, 0.0f, 0.0f);
            if (viewport_axis & AXIS_Y) translation += glm::vec3 (0.0f, 0.05f, 0.0f);
            if (viewport_axis & AXIS_Z) translation += glm::vec3 (0.0f, 0.0f, 0.05f);
            
            translation *= delta_x + delta_x;
            
            for (auto& object : Editor::selection->objects) {
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
            
            for (auto& object : Editor::selection->objects) {
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
}

void ViewportCtrl::OnTimer(wxTimerEvent& event) {
    using namespace Core::Render;
    using namespace Core;
    
    if (key_forward) CAMERA_POSITION += CAMERA_ROTATION * DIRECTION_FORWARD * 0.1f;
    if (key_backward) CAMERA_POSITION -= CAMERA_ROTATION * DIRECTION_FORWARD * 0.1f;
    if (key_left) CAMERA_POSITION -= CAMERA_ROTATION * DIRECTION_SIDE * 0.1f;
    if (key_right) CAMERA_POSITION += CAMERA_ROTATION * DIRECTION_SIDE * 0.1f;
    
    Refresh();
}

void ViewportCtrl::OnSizeChange(wxSizeEvent& event) {
    int width, height;
    GetSize(&width, &height);
    glViewport(0, 0, width, height);
    Core::Render::ScreenSize(width, height);
}

void ViewportCtrl::OnPaint(wxPaintEvent& event)
{
    SetCurrent(*m_context);

    using namespace Core;
    using namespace Core::Render;
    
    for (auto& object : Editor::selection->objects) {
        glm::quat space;
        // TODO: make this code not ugly
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_WORLD) space = glm::vec3(0.0f);
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) space = glm::quat(glm::vec3 {
                    object->GetProperty("rotation-x").float_value,
                    object->GetProperty("rotation-y").float_value,
                    object->GetProperty("rotation-z").float_value
                });
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITYGROUP) space = glm::vec3(glm::radians(180.0f));
        
        
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
    
    
    /*for (auto cell : Editor::worldCells) {
        if (cell->is_transitions_visible) {
            for (auto trans : cell->transitions) {
                if (trans->is_visible) {
                    for (auto ps : trans->points) {
                        for (auto pe : trans->points) {
                            AddLine(ps->location, pe->location, COLOR_CYAN);
                        }
                    }
                }
            }
        }
        
        if (cell->is_paths_visible) {
            for (auto path : cell->paths) {
                if (path->is_visible) {
                    for (auto curve : path->curves) {
                        for (float t = 0.0f; t < 1.0f; t += 0.1f) {
                            glm::vec3 l1 = glm::mix(curve->location1, curve->location2, t);
                            glm::vec3 l2 = glm::mix(curve->location3, curve->location4, t);
                            glm::vec3 l3 = glm::mix(curve->location1, curve->location2, t + 0.1f);
                            glm::vec3 l4 = glm::mix(curve->location3, curve->location4, t + 0.1f);
                            glm::vec3 p1 = glm::mix(l1, l2, t);
                            glm::vec3 p2 = glm::mix(l3, l4, t + 0.1f);
                            AddLine(p1, p2, COLOR_GREEN);
                        }
                    }
                }
            }
        }
        
        if (cell->is_navmeshes_visible) {
            for (auto navmesh : cell->navmeshes) {
                if (navmesh->is_visible) {
                    for (auto ns : navmesh->nodes) {
                        for (auto ne : navmesh->nodes) {
                            if (ne->id == ns->next_id || ne->id == ns->prev_id || ne->id == ns->left_id || ne->id == ns->right_id)
                            AddLine(ns->location, ne->location, COLOR_GREEN);
                        }
                    }
                }
            }
        }
    }*/
    
    
    
    
    AddLineMarker(glm::vec3(0.0f, 0.0f, 0.0f), COLOR_CYAN);
    
    SetSun(time_of_day);

    
    Async::ResourceLoader2ndStage();
    Async::FinishResource();
    
    //Render::UpdateArmatures();
    Render::Render();

    glFlush();
    SwapBuffers();
}