#define ENGINE_EDITOR_MODE

#include <platform/file.cpp>

#include <framework/core.cpp>
#include <framework/async.cpp>
#include <framework/entitycomponent.cpp>
#include <framework/file.cpp>
#include <framework/math.cpp>
#include <framework/stats.cpp>
#include <framework/system.cpp>
#include <framework/settings.cpp>
#include <framework/event.cpp>
#include <framework/uid.cpp>
#include <framework/logging.cpp>
#include <framework/language.cpp>
#include <framework/type.cpp>

#include <render/opengl/renderer.cpp>
#include <render/opengl/shader.cpp>
#include <render/opengl/buffer.cpp>
#include <render/opengl/drawlist.cpp>
#include <render/opengl/light.cpp>
#include <render/opengl/material.cpp>
#include <render/opengl/sprite.cpp>
#include <render/opengl/texture.cpp>

#include <render/scene.cpp>
#include <render/animation.cpp>
#include <render/material.cpp>
#include <render/light.cpp>
#include <render/sprite.cpp>
#include <render/model.cpp>
#include <render/error.cpp>
#include <render/render.cpp>
#include <render/renderer.cpp>

#include <components/render.cpp>
#include <components/animation.cpp>

#include <glad/glad.c>

bool tram::Platform::Window::IsRenderContextThread() {
    return true;
}

bool tram::UI::ShouldExit() {
    return false;
}

#include <platform/terminal.cpp>
#include <platform/other.cpp>