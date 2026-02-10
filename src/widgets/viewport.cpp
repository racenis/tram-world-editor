#include <glad.h>

#include <editor/editor.h>
#include <editor/actions.h>
#include <editor/settings.h>

#include <editor/objects/entity.h>
#include <editor/objects/world.h>

#include <widgets/viewport.h>
#include <widgets/objectmenu.h>
#include <widgets/mainframe.h>

#include <widgets/signaleditor.h>

using namespace Editor;

#include <framework/core.h>
#include <framework/async.h>
#include <framework/language.h>
#include <framework/system.h>
#include <render/render.h>
#include <render/renderer.h>
#include <render/api.h>
#include <render/scene.h>

#include <components/render.h>

#include <numeric>
#include <map>
#include <set>

ViewportCtrl* viewport = nullptr;

float Editor::Viewport::CURSOR_X = 0.0f;
float Editor::Viewport::CURSOR_Y = 0.0f;
float Editor::Viewport::CURSOR_Z = 0.0f;

using namespace tram;
using namespace tram::Render;

class ViewportTool {
public:
    virtual ~ViewportTool() = default;
    virtual void MouseMove(float x, float y, float delta_x, float delta_y) = 0;
    virtual void LeftClick() = 0;
    virtual void RightClick() = 0;
    virtual void Keydown(wchar_t keycode, int code) = 0;
    virtual void Keyup(wchar_t keycode, int code) = 0;
    
    virtual void Update() = 0;
    virtual void Display() = 0;
    
    virtual void Finish() = 0;
    virtual void Cancel() = 0;
};

class MoveTool : public ViewportTool {
public:
    MoveTool() {
        viewport->CaptureMouse();
        viewport->CenterMouseCursor();
    }
    
    ~MoveTool() {
        viewport->StopTimer();
        viewport->ReleaseMouse();
    }

    void MouseMove(float x, float y, float delta_x_e, float delta_y_e) {
        static float mouse_x = 0.0f;
        static float mouse_y = 0.0f;
        
        int width, height;
        viewport->GetSize(&width, &height);
        int center_x = width/2, center_y = height/2;
        
        float delta_x = (float)(x - center_x);
        float delta_y = (float)(y - center_y);
        viewport->WarpPointer(center_x, center_y);
        
        mouse_x += delta_x * 0.1f;
        mouse_y += delta_y * 0.1f;
        
        mouse_y = mouse_y > 90.0f ? 90.0f : mouse_y < -90.0f ? -90.0f : mouse_y;
        tram::Render::SetViewRotation(glm::quat(glm::vec3(-glm::radians(mouse_y), -glm::radians(mouse_x), 0.0f)), 0);
        tram::Render::SetViewRotation(glm::quat(glm::vec3(-glm::radians(mouse_y), -glm::radians(mouse_x), 0.0f)), 1);
        
        using namespace tram;
        using namespace tram::Render;

        vec3 camera_pos = GetViewPosition();
        quat camera_rot = GetViewRotation();
        
        vec3 cursor_pos = camera_pos + camera_rot * DIRECTION_FORWARD * 2.0f;
        
        Viewport::CURSOR_X = cursor_pos.x;
        Viewport::CURSOR_Y = cursor_pos.y;
        Viewport::CURSOR_Z = cursor_pos.z;
    }
    
    void LeftClick() {
        
    }
    
    void RightClick() {
        
    }
    
    void Keydown(wchar_t keycode, int code) {
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
            viewport->StartTimer();
            Update();
        }
        
        if (code == WXK_SHIFT) {
            key_shift = true;
        }
    }
    
    void Keyup(wchar_t keycode, int code) {
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
            viewport->StopTimer();
        }
        
        if (code == WXK_SHIFT) {
            key_shift = false;
        }
    }
    
    void Update() {
        vec3 camera_position = GetViewPosition();
        quat camera_rotation = GetViewRotation();
        
        float move_speed = key_shift ? 0.3f : 0.1f; 
        
        if (key_forward) camera_position += camera_rotation * DIRECTION_FORWARD * move_speed;
        if (key_backward) camera_position -= camera_rotation * DIRECTION_FORWARD * move_speed;
        if (key_left) camera_position -= camera_rotation * DIRECTION_SIDE * move_speed;
        if (key_right) camera_position += camera_rotation * DIRECTION_SIDE * move_speed;
        
        SetViewPosition(camera_position, 0);
        SetViewPosition(camera_position, 1);
        
        viewport->Refresh();
    }
    
    void Display() {
        
    }
    
    void Finish() {
        
    }
    
    void Cancel() {
        
    }
private:
    bool key_forward = false;
    bool key_backward = false;
    bool key_left = false;
    bool key_right = false;
    bool key_shift = false;
};

class TranslateTool : public ViewportTool {
public:
    TranslateTool() {
        viewport->CaptureMouse();
        //viewport->CenterMouseCursor();
        
        Editor::PerformAction<Editor::ActionChangeProperties>(std::vector<std::string> {"position-x", "position-y", "position-z"});
    }
    
    ~TranslateTool() {
        viewport->ReleaseMouse();
    }
    
    void MouseMove(float x, float y, float delta_x, float delta_y) {
        std::vector<vec3> positions(Editor::SELECTION->objects.size());
        std::transform(Editor::SELECTION->objects.begin(),
                       Editor::SELECTION->objects.end(),
                       positions.begin(),
                       [](auto& object) -> vec3 {
                           return {
                                object->GetProperty("position-x").float_value,
                                object->GetProperty("position-y").float_value,
                                object->GetProperty("position-z").float_value
                            };
                       });
        vec3 middle_point = std::accumulate(positions.begin(), positions.end(), vec3(0, 0, 0)) / (float)positions.size();
        
        if (delta_x == 0.0f && delta_y == 0.0f) return;
        
        vec3 translation = {0.0f, 0.0f, 0.0f};
        
        const bool screen_translate = !translate_x && !translate_y && !translate_z;
        const bool no_translate = translate_x && translate_y && translate_z;
        
        if (no_translate) {
            return;
        }
        

        // TODO: fix project function!!!
        vec3 screen_origin = middle_point;
        
        Project(screen_origin, screen_origin);
        
        vec3 screen_axis_x = screen_origin + vec3(delta_x, 0.0f, 0.0f);
        vec3 screen_axis_y = screen_origin + vec3(0.0f, delta_y, 0.0f);
        
        vec3 point_origin = ProjectInverse(screen_origin);
        vec3 point_axis_y = ProjectInverse(screen_axis_y);
        vec3 point_axis_x = ProjectInverse(screen_axis_x);
        
        vec3 axis_x = point_axis_x - point_origin;
        vec3 axis_y = point_axis_y - point_origin;

        translation += axis_x;
        translation += axis_y;
        
        if (!screen_translate) {
            vec3 axis = {1, 0, 0};
            
            bool single_axis = false;
            
            if (translate_x && !translate_y && !translate_z) axis = {1, 0, 0}, single_axis = true;
            if (!translate_x && translate_y && !translate_z) axis = {0, 1, 0}, single_axis = true;
            if (!translate_x && !translate_y && translate_z) axis = {0, 0, 1}, single_axis = true;
            
            vec3 object_rotation = {0.0f, 0.0f, 0.0f};
            
            if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) {
                object_rotation = vec3 {
                    Editor::SELECTION->objects.front()->GetProperty("rotation-x").float_value,
                    Editor::SELECTION->objects.front()->GetProperty("rotation-y").float_value,
                    Editor::SELECTION->objects.front()->GetProperty("rotation-z").float_value
                };
                
                axis = quat(object_rotation) * axis;
            }
            
            if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITYGROUP) {
                // TODO: implement entity group transforms
                Editor::Viewport::ShowErrorDialog("EntityGroup transform not implemented yet!");
            }
            
            if (single_axis) {
                vec3 translated = middle_point + translation;
                
                vec3 to_pt = middle_point + axis;
                
                ProjectLine(translated, middle_point, to_pt);
                
                translation = translated - middle_point;
            } else {
                mat4 matrix = PositionRotationToMatrix(middle_point, object_rotation);
                
                translation += middle_point;
                translation = glm::inverse(matrix) * vec4(translation, 1.0f);

                if (translate_x && translate_y) {
                    translation.z = 0.0f;
                }
                
                if (translate_x && translate_z) {
                    translation.y = 0.0f;
                }
                
                if (translate_y && translate_z) {
                    translation.x = 0.0f;
                }
                
                translation = matrix * vec4(translation, 1.0f);
                translation -= middle_point;
            }
        }
        
        

        for (auto& object : Editor::SELECTION->objects) {
            glm::vec3 object_position = glm::vec3 {
                object->GetProperty("position-x").float_value,
                object->GetProperty("position-y").float_value,
                object->GetProperty("position-z").float_value
            };
            
            auto translation_adjusted = translation;
            
            object_position += translation_adjusted;
                            
            object->SetProperty("position-x", object_position.x);
            object->SetProperty("position-y", object_position.y);
            object->SetProperty("position-z", object_position.z);
        }
        
        Editor::Viewport::Refresh();
    }
    
    void LeftClick() {
        
    }
    
    void RightClick() {
        
    }
    
    void Keydown(wchar_t keycode, int code) {
        if (keycode == 'X') translate_x = !translate_x;
        if (keycode == 'Y') translate_y = !translate_y;
        if (keycode == 'Z') translate_z = !translate_z;
        
        if (code == WXK_SHIFT) shifting = true;
        
        if (keycode == 'X' && shifting) {
            if (!translate_y || !translate_z) {
                translate_x = false;
                translate_y = true;
                translate_z = true;
            } else {
                translate_x = false;
                translate_y = false;
                translate_z = false;
            }
        }
        
        if (keycode == 'Y' && shifting) {
            if (!translate_x || !translate_z) {
                translate_x = true;
                translate_y = false;
                translate_z = true;
            } else {
                translate_x = false;
                translate_y = false;
                translate_z = false;
            }
        }
        
        if (keycode == 'Z' && shifting) {
            if (!translate_x || !translate_y) {
                translate_x = true;
                translate_y = true;
                translate_z = false;
            } else {
                translate_x = false;
                translate_y = false;
                translate_z = false;
            }
        }
        
        if (code == WXK_CONTROL) {
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
    }
    
    void Keyup(wchar_t keycode, int code) {
        if (code == WXK_SHIFT) shifting = false;
    }
    
    void Update() {
        
    }
    
    void Display() {
        for (auto& object : Editor::SELECTION->objects) {
            vec3 position =  {
                object->GetProperty("position-x").float_value,
                object->GetProperty("position-y").float_value,
                object->GetProperty("position-z").float_value
            };
            
            quat space = {1.0f, 0.0f, 0.0f, 0.0f};
            
            if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) {
                space = vec3 {
                    Editor::SELECTION->objects.front()->GetProperty("rotation-x").float_value,
                    Editor::SELECTION->objects.front()->GetProperty("rotation-y").float_value,
                    Editor::SELECTION->objects.front()->GetProperty("rotation-z").float_value
                };
            }
            
            if (translate_x) AddLine(position - space * glm::vec3(100.0f, 0.0f, 0.0f), position + space * glm::vec3(100.0f, 0.0f, 0.0f), COLOR_RED);
            if (translate_y) AddLine(position - space * glm::vec3(0.0f, 100.0f, 0.0f), position + space * glm::vec3(0.0f, 100.0f, 0.0f), COLOR_GREEN);
            if (translate_z) AddLine(position - space * glm::vec3(0.0f, 0.0f, 100.0f), position + space * glm::vec3(0.0f, 0.0f, 100.0f), COLOR_BLUE);
        }
    }
    
    void Finish() {
        
    }
    
    void Cancel() {
        Editor::Undo();
    }
protected:
    bool translate_x = false;
    bool translate_y = false;
    bool translate_z = false;
    
    bool shifting = false;
};

class RotateTool : public ViewportTool {
public:
    RotateTool() {
        viewport->CaptureMouse();
        //viewport->CenterMouseCursor();
        
        for (auto& object : Editor::SELECTION->objects) {
            vec3 position = {
                object->GetProperty("position-x").float_value,
                object->GetProperty("position-y").float_value,
                object->GetProperty("position-z").float_value
            };
            
            vec3 rotation = {
                object->GetProperty("rotation-x").float_value,
                object->GetProperty("rotation-y").float_value,
                object->GetProperty("rotation-z").float_value
            };
            
            backups[object.get()] = {position, rotation, rotation};
        }
        
        vec3 position_sum = {0.0f, 0.0f, 0.0f};
        vec3 aabb_min = {0.0f, 0.0f, 0.0f};
        vec3 aabb_max = {0.0f, 0.0f, 0.0f};
        bool aabb_set = false;
        
        for (auto& object : Editor::SELECTION->objects) {
            vec3 position = {
                object->GetProperty("position-x").float_value,
                object->GetProperty("position-y").float_value,
                object->GetProperty("position-z").float_value
            };
            
            position_sum += position;
            
            if (!aabb_set) {
                aabb_min = position;
                aabb_max = position;
                aabb_set = true;
            } else {
                aabb_min = MergeAABBMin(aabb_min, position);
                aabb_max = MergeAABBMax(aabb_max, position);
            }
        }
        
        middle_point = glm::mix(aabb_min, aabb_max, 0.5f);
        
        Editor::PerformAction<Editor::ActionChangeProperties>(std::vector<std::string> {"position-x", "position-y", "position-z", "rotation-x", "rotation-y", "rotation-z"});
    }

    ~RotateTool() {
        viewport->ReleaseMouse();
    }

    void Apply() {
        for (auto& object : Editor::SELECTION->objects) {
            auto [object_position, object_rotation, object_rotation_euler] = backups[object.get()];
            
            object_position -= middle_point;
            object_position = rotation * object_position;
            object_position += middle_point;
            
            if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_WORLD) {
                object_rotation = glm::quat(rotation) * object_rotation;
            }
            
            if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) {
                object_rotation = object_rotation * glm::quat(rotation);
            }
            
            if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITYGROUP) {
                // TODO: implement entity group transforms
            }
            
            auto rotation_euler = EulerFromQuat(object_rotation, object_rotation_euler);
            
            object->SetProperty("rotation-x", rotation_euler.x);
            object->SetProperty("rotation-y", rotation_euler.y);
            object->SetProperty("rotation-z", rotation_euler.z);
            
            object->SetProperty("position-x", object_position.x);
            object->SetProperty("position-y", object_position.y);
            object->SetProperty("position-z", object_position.z);
        }
    }

    void MouseMove(float x, float y, float delta_x, float delta_y) {
        if (delta_x == 0.0f && delta_y == 0.0f) return;
        
        int selected_axes = (int)rotate_x + (int)rotate_y + (int)rotate_z;
        if (selected_axes > 1) return;
        
        if (!rotate_x && !rotate_y && !rotate_z) {
            vec3 axis_x = {1.0f, 0.0f, 0.0f};
            vec3 axis_y = {0.0f, 1.0f, 0.0f};
            
            axis_x = GetViewRotation() * axis_x;
            axis_y = GetViewRotation() * axis_y;
            
            rotation = glm::rotate(rotation, delta_y * 0.01f, axis_x);
            rotation = glm::rotate(rotation, delta_x * 0.01f, axis_y);
            
        } else {
            vec3 axis = {1.0f, 0.0f, 0.0f};
            
            if (rotate_x) axis = {1.0f, 0.0f, 0.0f};
            if (rotate_y) axis = {0.0f, 1.0f, 0.0f};
            if (rotate_z) axis = {0.0f, 0.0f, 1.0f};
            
            if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) {
                vec3 object_rotation = vec3 {
                    Editor::SELECTION->objects.front()->GetProperty("rotation-x").float_value,
                    Editor::SELECTION->objects.front()->GetProperty("rotation-y").float_value,
                    Editor::SELECTION->objects.front()->GetProperty("rotation-z").float_value
                };
                
                axis = quat(object_rotation) * axis;
            }
            
            vec3 world_axis = middle_point + axis;
            vec3 world_point = middle_point;
            
            Project(world_axis, world_axis);
            Project(world_point, world_point);
            
            vec2 screen_axis = vec2(world_axis.y, world_axis.y) - vec2(world_point.x, world_point.y);
            
            vec2 screen_direction = (vec2(delta_x, delta_y));
            
            float amount = glm::dot(glm::normalize(vec2(screen_axis.y,  -screen_axis.x)), glm::normalize(screen_direction));
            
            
            rotation = glm::rotate(rotation, glm::length(screen_direction) * amount * 0.01f, axis);
        }
        
        Apply();
        
        
        Editor::Viewport::Refresh();
    }
    
    void LeftClick() {
        
    }
    
    void RightClick() {
        
    }
    
    void Keydown(wchar_t keycode, int code) {
        if (keycode == 'X') rotate_x = !rotate_x;
        if (keycode == 'Y') rotate_y = !rotate_y;
        if (keycode == 'Z') rotate_z = !rotate_z;
        
        if (code == WXK_CONTROL && !rotate_x && !rotate_y && !rotate_z) {
            if (!control) {
                control = true;
            } else {
                for (auto& backup : backups) {
                    backup.second.rotation = {1.0f, 0.0f, 0.0f, 0.0f};
                }
                rotation = {1.0f, 0.0f, 0.0f, 0.0f};
            }
        } else {
            control = false;
        }
            
            
        
        if (code == WXK_CONTROL) {
            vec3 euler_rotation = EulerFromQuat(rotation);
            
            float div = 1.0f;
            switch (Editor::Settings::ROTATION_SNAP) {
                case Editor::Settings::SNAP_15: div = 0.261799f; break;
                case Editor::Settings::SNAP_30: div = 0.523599f; break;
                case Editor::Settings::SNAP_45: div = 0.785398f; break;
                default:
                case Editor::Settings::SNAP_90: div = 1.5708f; break;
            }
            
            euler_rotation = glm::round(euler_rotation/div) * div;

            rotation = euler_rotation;

            Apply();
            
            // this helps with floating point imprecision, e.g. instead of the
            // interface displaying that an object snapped to 45.02 degrees, it
            // will show that it snapped to 45.0 degrees
            for (auto& object : Editor::SELECTION->objects) {
                float x = object->GetProperty("rotation-x");
                float y = object->GetProperty("rotation-y");
                float z = object->GetProperty("rotation-z");
                
                const float tupi = 2.0f * glm::pi<float>();
                const float bias = 0.02f;

                x = round(x/div)*div;
                y = round(y/div)*div;
                z = round(z/div)*div;

                if (x > tupi - bias) x = 0.0f;
                if (y > tupi - bias) y = 0.0f;
                if (z > tupi - bias) z = 0.0f;
                
                object->SetProperty("rotation-x", x);
                object->SetProperty("rotation-y", y);
                object->SetProperty("rotation-z", z);
            }
            
            Editor::Viewport::Refresh();
        }
        
    }
    
    void Keyup(wchar_t keycode, int code) {
        
    }
    
    void Update() {
        
    }
    
    void Display() {
        AddLineMarker(middle_point, COLOR_YELLOW);
        
        quat space = {1.0f, 0.0f, 0.0f, 0.0f};
        
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) {
            space = vec3 {
                Editor::SELECTION->objects.front()->GetProperty("rotation-x").float_value,
                Editor::SELECTION->objects.front()->GetProperty("rotation-y").float_value,
                Editor::SELECTION->objects.front()->GetProperty("rotation-z").float_value
            };
        }
        
        if (rotate_x) AddLine(middle_point - space * glm::vec3(100.0f, 0.0f, 0.0f), middle_point + space * glm::vec3(100.0f, 0.0f, 0.0f), COLOR_RED);
        if (rotate_y) AddLine(middle_point - space * glm::vec3(0.0f, 100.0f, 0.0f), middle_point + space * glm::vec3(0.0f, 100.0f, 0.0f), COLOR_GREEN);
        if (rotate_z) AddLine(middle_point - space * glm::vec3(0.0f, 0.0f, 100.0f), middle_point + space * glm::vec3(0.0f, 0.0f, 100.0f), COLOR_BLUE);
    }
    
    void Finish() {
        
    }
    
    void Cancel() {
        Editor::Undo();
    }
protected:
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 middle_point = {0.0f, 0.0f, 0.0f};

    struct TransformBackup {
        vec3 position;
        quat rotation;
        vec3 rotation_euler;
    };

    std::map<Object*, TransformBackup> backups;

    bool rotate_x = false;
    bool rotate_y = false;
    bool rotate_z = false;
    
    bool control = false;
};

class Tool : public ViewportTool {
public:
    void MouseMove(float x, float y, float delta_x, float delta_y) {
        
    }
    
    void LeftClick() {
        
    }
    
    void RightClick() {
        
    }
    
    void Keydown(wchar_t keycode, int code) {
        
    }
    
    void Keyup(wchar_t keycode, int code) {
        
    }
    
    void Update() {
        
    }
    
    void Display() {
        
    }
    
    void Finish() {
        
    }
    
    void Cancel() {
        
    }
};















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
    
    tram::System::SetInitialized(tram::System::UI, true);
    
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
    tram::Render::SetViewPosition({0.0f, 5.0f, 0.0f}, 0);
    tram::Render::SetViewPosition({0.0f, 5.0f, 0.0f}, 1);
    
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

void ViewportCtrl::StartTimer() {
    key_timer.Start(15);
}

void ViewportCtrl::StopTimer() {
    key_timer.Stop();
}

void ViewportCtrl::CenterMouseCursor() {
    int width, height;
    GetSize(&width, &height);
    WarpPointer(width/2, height/2);
}

void ViewportCtrl::CancelViewportOperation() {
   // ReleaseMouse();
    Refresh();
    
    //viewport_axis = AXIS_NONE;
    //viewport_mode = MODE_NONE;
}

std::set<Object*> FindWidgetables() {
    std::set<Object*> widgetables;
    
    for (auto& object : Editor::SELECTION->objects) {
        widgetables.emplace(object.get());
        
        if (object->IsWidgetedWithChildren()) {
            for (auto& object : object->GetChildren()) {
                widgetables.emplace(object.get());
            }
        }
        
        auto parent = object->GetParent().get();
        
        if (!parent) continue;
        
        if (!parent->IsWidgetedWithChildren()) continue;
        
        if (widgetables.contains(parent)) continue;
        
        for (auto& object : parent->GetChildren()) {
            widgetables.emplace(object.get());
        }
        
        widgetables.emplace(parent);
    }
    
    return widgetables;
}

Object* FindCursorObject(std::set<Object*>& objects, float cursor_x, float cursor_y) {
    tram::vec3 far_point = tram::Render::ProjectInverse({cursor_x, cursor_y, 0.0f});
    tram::vec3 near_point = tram::Render::ProjectInverse({cursor_x, cursor_y, 1000.0f});
    
    tram::vec3 look_direction = glm::normalize(far_point - near_point);
    tram::vec3 look_position = near_point;
    
    auto res = tram::Render::AABB::FindNearestFromRay(look_position, look_direction, -1);
    
    float distance = INFINITY;
    Object* nearest = nullptr;
    
    //std::cout << "trying.." << std::endl;
    for (auto object : objects) {
        //std::cout << "looking at: " << typeid(*object).name() << " and its " << object->IsHidden() << std::endl;
        auto gizmos = object->GetWidgetDefinitions();
        
        vec3 position = {
            object->GetProperty("position-x").float_value,
            object->GetProperty("position-y").float_value,
            object->GetProperty("position-z").float_value
        };
        
        vec3 edge = position;
        
        bool has_widget = false;
        
        for (const auto& gizmo : gizmos) {
            if (gizmo.type != Editor::WidgetDefinition::WIDGET_SELECTION_BOX) continue;
            
            edge += (tram::vec3)gizmo.value1;
            
            has_widget = true;
            break;
        }
        
        if (!has_widget) continue;
        
        vec3 projected_position = position;
        vec3 projected_edge = edge;
        
        Project(projected_position, projected_position);
        Project(projected_edge, projected_edge);
        
        vec2 screen_pos = {projected_position.x, projected_position.y};
        vec2 screen_edge = {projected_edge.x, projected_edge.y};
        vec2 pointer_pos = {cursor_x, cursor_y};
        
        float radius = glm::distance(screen_pos, screen_edge);
        float screen_distance = glm::distance(pointer_pos, screen_pos);
    
        if (screen_distance > radius) continue;
        
        float object_distance = glm::distance(GetViewPosition(), position);
        
        if (object_distance > distance) continue;
        
        distance = object_distance;
        nearest = object;
    };
    
    Object* selected = nullptr;
    
    if (res.data && distance > glm::distance(GetViewPosition(), res.intersection)) {
        selected = GetEntityFromViewmodel((tram::RenderComponent*) res.data);
    } else if (nearest) {
        selected = nearest;
    }
    
    return selected;
}

void ViewportCtrl::OnLeftClick(wxMouseEvent& event) {
    
    if (viewport_tool) {
        delete viewport_tool;
        viewport_tool = nullptr;
        CancelViewportOperation();
        return;
    }
    
    // if viewport is in move mode or translate mode, cancel them
    //if (viewport_mode != MODE_NONE) {
    //    CancelViewportOperation();
   //     return;
   // }
    
    // if control key is pressed down, select object in viewport
    if (wxGetKeyState(WXK_CONTROL)) {
        std::set<Object*> widgetables = FindWidgetables();
    
        Object* selected = nullptr;
        selected = FindCursorObject(widgetables, cursor_x, cursor_y);
        
        if (selected) {
            //std::cout << GetEntityFromViewmodel((tram::RenderComponent*) res.data)->GetName() << std::endl;
            
            //std::cout << "distance to intersection is " <<  glm::distance(GetViewPosition(), res.intersection) << std::endl;
            
            // create a new selection object
            auto new_selection = std::make_shared<Editor::Selection>();
            
            // if shift is pressed down, then multiple selection mode
            if (wxGetKeyState(WXK_SHIFT)) {
                new_selection->objects = Editor::SELECTION->objects;
            }
            
            // lookup selected object and try to find it in new selection
            auto selected_object = selected->GetPointer();
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
    
    viewport_tool = new MoveTool;
    
    // otherwise
    
    //CaptureMouse();
    //CenterMouseCursor();
    
    //viewport_mode = MODE_MOVE;
}

void ViewportCtrl::OnRightClick(wxMouseEvent& event) {
    if (viewport_tool) {
        viewport_tool->Cancel();
        delete viewport_tool;
        viewport_tool = nullptr;
        return;
    }
    
    /*if (viewport_mode == MODE_TRANSLATE || viewport_mode == MODE_ROTATE) {
        Editor::Undo();
        
        CancelViewportOperation();
        
        return;
    }*/
    
    if (!viewport_tool) {
        world_tree_popup->SetSelectionStatus(Editor::SELECTION.get());
        main_frame->PopupMenu(world_tree_popup);
    }
}

void ViewportCtrl::OnMouseMove(wxMouseEvent& event) {
    static float prev_x = event.GetX();
    static float prev_y = event.GetY();
        
    this->cursor_x = event.GetX();
    this->cursor_y = event.GetY();

    // if using a tool, wrap mouse pointer around
    if (viewport_tool) {
        int width, height;
        GetSize(&width, &height);
        
        int pointer_x = event.GetX();
        int pointer_y = event.GetY();
        
        bool warped = false;
        
        if (pointer_x < 0) pointer_x = width, warped = true;
        if (pointer_y < 0) pointer_y = height, warped = true;
        if (pointer_x > width) pointer_x = 0, warped = true;
        if (pointer_y > height) pointer_y = 0, warped = true;
        
        if (warped) {
            prev_x = pointer_x;
            prev_y = pointer_y;
            
            WarpPointer(pointer_x, pointer_y);
            
            return;
        }
    }
    
    const float delta_x = event.GetX() - prev_x;
    const float delta_y = event.GetY() - prev_y;
    
    prev_x = event.GetX();
    prev_y = event.GetY();
    
    if (viewport_tool) {
        
        
        viewport_tool->MouseMove(event.GetX(), event.GetY(), delta_x, delta_y);
        
        //std::cout << event.GetX() << ":" << event.GetY() << std::endl;
        
    }
    
    if (viewport_tool || wxGetKeyState(WXK_CONTROL)) {
        /*int width, height;
        GetSize(&width, &height);
        int center_x = width/2, center_y = height/2;
        
        float delta_x = (float)(event.GetX() - center_x);
        float delta_y = (float)(event.GetY() - center_y);
        WarpPointer(center_x, center_y);*/
        
        Refresh();
    }
}

void ViewportCtrl::OnKeydown(wxKeyEvent& event) {
    auto keycode = event.GetUnicodeKey();
    
    if (event.IsAutoRepeat()) return;
    
    if (viewport_tool) viewport_tool->Keydown(event.GetUnicodeKey(), event.GetKeyCode());
    
    if (keycode == 'G') {
        if (viewport_tool) {
            viewport_tool->Cancel();
            delete viewport_tool;
        }
        
        viewport_tool = new TranslateTool;
    }
    
    if (keycode == 'R') {
        if (viewport_tool) {
            viewport_tool->Cancel();
            delete viewport_tool;
        }
        
        viewport_tool = new RotateTool;
    }
    
    
    if (keycode == 'D' /*&& event.ShiftDown()*/ && (!viewport_tool || !dynamic_cast<MoveTool*>(viewport_tool))) {
        if (viewport_tool) {
            delete viewport_tool;
        }
        
        Editor::PerformAction<Editor::ActionDuplicate>();
        
        viewport_tool = new TranslateTool;
    }
    
    if (keycode == 'S' /*&& event.ShiftDown()*/ && (!viewport_tool || !dynamic_cast<MoveTool*>(viewport_tool))) {
        if (viewport_tool) {
            delete viewport_tool;
            viewport_tool = nullptr;
        }
        
        OpenSignalEditorModal();
    }


    if (keycode == 'E') {
        if (viewport_tool) {
            //viewport_tool->Cancel();
            delete viewport_tool;
            //viewport_tool = nullptr;
        }
        
        Editor::PerformAction<Editor::ActionExtrude>();
        
        viewport_tool = new TranslateTool;
    }

    if (keycode == 'F') {
        if (viewport_tool) {
            viewport_tool->Cancel();
            delete viewport_tool;
            viewport_tool = nullptr;
        }
        
        Editor::PerformAction<Editor::ActionConnect>();
    }
    
    
    
     
}

void ViewportCtrl::OnKeyup(wxKeyEvent& event) {
    //auto keycode = event.GetUnicodeKey();
    
    if (viewport_tool) viewport_tool->Keyup(event.GetUnicodeKey(), event.GetKeyCode());
    
    
}

void ViewportCtrl::OnTimer(wxTimerEvent& event) {
    using namespace tram::Render;
    using namespace tram;
    
    if (viewport_tool) viewport_tool->Update();
    
    //if (key_forward) CAMERA_POSITION += CAMERA_ROTATION * DIRECTION_FORWARD * 0.1f;
    //if (key_backward) CAMERA_POSITION -= CAMERA_ROTATION * DIRECTION_FORWARD * 0.1f;
    //if (key_left) CAMERA_POSITION -= CAMERA_ROTATION * DIRECTION_SIDE * 0.1f;
    //if (key_right) CAMERA_POSITION += CAMERA_ROTATION * DIRECTION_SIDE * 0.1f;
    
    
}

void ViewportCtrl::OnSizeChange(wxSizeEvent& event) {
    //assert(GetParent()->IsShown());
    
    int width, height;
    GetSize(&width, &height);
    //glViewport(0, 0, width, height);
    //return;
    
    //tram::Render::API::SetScreenSize(width, height);
    tram::Render::SetScreenSize(width, height);
}

void ViewportCtrl::ViewCenterOnSelection() {
    using namespace tram;
    using namespace tram::Render;
    
    if (!Editor::SELECTION->objects.size()) return;
    
    Object* target = Editor::SELECTION->objects.begin()->get();
    
    vec3 camera_pos = GetViewPosition();
    quat camera_rot = GetViewRotation();
    
    vec3 target_pos = {target->GetProperty("position-x"), target->GetProperty("position-y"), target->GetProperty("position-z")};
    
    vec3 to_target = glm::normalize(target_pos - camera_pos);
    
    camera_pos = target_pos + (to_target * -10.0f);
    camera_rot = glm::quatLookAt(to_target, DIRECTION_UP);
    
    SetViewPosition(camera_pos);
    SetViewRotation(camera_rot);
    
    Refresh();
}

void ViewportCtrl::OnPaint(wxPaintEvent& event) {
    //assert(GetParent()->IsShown());
    //if (!GetParent()->IsShown()) return;
        
    //SetCurrent(*m_context);

    using namespace tram;
    using namespace tram::Render;

    if (viewport_tool) viewport_tool->Display();

    /*auto recursive_draw = [](Object* object, auto draw_func) {
        if (object->IsHidden()) return;
        object->Draw();
        for (auto& obj : object->GetChildren()) draw_func(obj.get(), draw_func);
    };
    recursive_draw(Editor::WORLD.get(), recursive_draw);*/

    std::set<Object*> selected;
    for (auto& object : Editor::SELECTION->objects) {
        selected.emplace(object.get());
    }

    // 1. create an object set
    // 2. go through all selection objects and then add it or parent [ add drawn by parent method ]
    // 3. do same thing with widgets?
    std::set<Object*> widgetables = FindWidgetables();
    
    Object* selectable = nullptr;
    if (!viewport_tool) {
        selectable = FindCursorObject(widgetables, cursor_x, cursor_y);
    }
    
    for (auto& object : Editor::SELECTION->objects) {
        widgetables.emplace(object.get());
        
        if (object->IsWidgetedWithChildren()) {
            for (auto& object : object->GetChildren()) {
                widgetables.emplace(object.get());
            }
        }
        
        auto parent = object->GetParent().get();
        
        if (!parent) continue;
        
        if (!parent->IsWidgetedWithChildren()) continue;
        
        if (widgetables.contains(parent)) continue;
        
        for (auto& object : parent->GetChildren()) {
            widgetables.emplace(object.get());
        }
        
        widgetables.emplace(parent);
    }

    for (auto object : widgetables) {
        glm::quat space;
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_WORLD) space = glm::vec3(0.0f);
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITY) space = glm::quat(glm::vec3 {
                    object->GetProperty("rotation-x").float_value,
                    object->GetProperty("rotation-y").float_value,
                    object->GetProperty("rotation-z").float_value
                });
        if (Editor::Settings::TRANSFORM_SPACE == Editor::Settings::SPACE_ENTITYGROUP) space = glm::vec3(glm::radians(180.0f));
        
        object->Draw();
        
        //if (viewport_mode == MODE_TRANSLATE || viewport_mode == MODE_ROTATE) {
            glm::vec3 gizmo_location = glm::vec3 {
                object->GetProperty("position-x").float_value,
                object->GetProperty("position-y").float_value,
                object->GetProperty("position-z").float_value
            };
            AddLine(gizmo_location, gizmo_location + space * glm::vec3(1.0f, 0.0f, 0.0f), COLOR_RED);
            AddLine(gizmo_location, gizmo_location + space * glm::vec3(0.0f, 1.0f, 0.0f), COLOR_GREEN);
            AddLine(gizmo_location, gizmo_location + space * glm::vec3(0.0f, 0.0f, 1.0f), COLOR_BLUE);
            
            
        //}
        
        auto gizmos = object->GetWidgetDefinitions();
        
    
        
        
        
        for (const auto& gizmo : gizmos) {
            
            // important
            switch (gizmo.type) {
                case Editor::WidgetDefinition::WIDGET_SELECTION_BOX:
                case Editor::WidgetDefinition::WIDGET_LINE:
                    break;
                default:
                    if (!object->HasProperty(gizmo.property)) continue;
            }
            
            
            //std::cout << "drawing gizmo" << std::endl;
            
            glm::vec3 color;
            switch (gizmo.color) {
                case Editor::WidgetDefinition::WIDGET_GREEN:
                    color = {0.0f, 1.0f, 0.0f}; break;
                case Editor::WidgetDefinition::WIDGET_CYAN:
                default:
                    color = {0.0f, 1.0f, 1.0f};
            }
            
            glm::vec3 inv_color = vec3(1.0f, 1.0f, 1.0f) - color;
            
            if (object == selectable) color *= vec3{0.5f, 0.5f, 0.5f};
            
            //std::cout << "got color" << std::endl;
            
            switch (gizmo.type) {
                case Editor::WidgetDefinition::WIDGET_NORMAL: {
                    //std::cout << "normawl widegt" << std::endl;
                    PropertyValue dir_vec = object->GetProperty(gizmo.property);
                    //std::cout << "got prop" << std::endl;
                    glm::vec3 dir = {dir_vec.vector_value.x, dir_vec.vector_value.y, dir_vec.vector_value.z};
                    glm::vec3 pos = glm::vec3 {
                        object->GetProperty("position-x").float_value,
                        object->GetProperty("position-y").float_value,
                        object->GetProperty("position-z").float_value
                    };
                    //std::cout << "got pos" << std::endl;
                    AddLine(pos, pos+dir, color);
                    //std::cout << "done roonomal gizmo" << std::endl;
                    } break;
                case Editor::WidgetDefinition::WIDGET_POINT: {
                    //std::cout << "widget point" << std::endl;
                    PropertyValue pos_vec = object->GetProperty(gizmo.property);
                    //std::cout << "doing point gizmo" << std::endl;
                    glm::vec3 pos = {pos_vec.vector_value.x, pos_vec.vector_value.y, pos_vec.vector_value.z};
                    AddLineMarker(pos, color);
                    //std::cout << "done point gizmo" << std::endl;
                    } break;
                    
                case Editor::WidgetDefinition::WIDGET_SELECTION_BOX: {
                    
                    AddLineAABB(-(glm::vec3)gizmo.value1, gizmo.value1, gizmo_location, vec3(0.0f, 0.0f, 0.0f), color);
                    //std::cout << "done point gizmo" << std::endl;
                    
                    if (selected.contains(object)) {
                        AddLineAABB(-2.0f * (glm::vec3)gizmo.value1, 2.0f * (glm::vec3)gizmo.value1, gizmo_location, vec3(0.0f, 0.0f, 0.0f), inv_color);
                    }
                    
                    } break;
                    
                case Editor::WidgetDefinition::WIDGET_LINE: {
                    AddLine(gizmo.value1, gizmo.value2, color);
                    } break;
                    
                default:
                    color = {0.0f, 1.0f, 1.0f};
            }
            
            //std::cout << "done!" << std::endl;
        }
    }
    

    AddLineMarker(glm::vec3(0.0f, 0.0f, 0.0f), COLOR_CYAN);
    
    //SetSun(time_of_day);
    //SetSun(0.8f);

    //Async::ResourceLoader1stStage();
    //Async::ResourceLoader2ndStage();
    //Async::FinishResource();
    Async::LoadResourcesFromDisk();
    Async::LoadResourcesFromMemory();
    Async::FinishResources();
    
    //Render::UpdateArmatures();
    Render::Render();

    glFlush();
    SwapBuffers();
}