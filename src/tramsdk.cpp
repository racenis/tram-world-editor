#define ENGINE_EDITOR_MODE
#include <framework/core.cpp>
#include <framework/async.cpp>
#include <framework/entitycomponent.cpp>
#include <framework/file.cpp>
#include <framework/math.cpp>
#include <framework/stats.cpp>
#include <framework/system.cpp>
#include <framework/event.cpp>
#include <framework/uid.cpp>
#include <framework/logging.cpp>
#include <framework/language.cpp>

#include <platform/file.cpp>
#include <platform/time.cpp>

#include <render/opengl/renderer.cpp>
#include <render/opengl/shader.cpp>

#include <render/aabb.cpp>
#include <render/animation.cpp>
#include <render/material.cpp>
#include <render/model.cpp>
#include <render/error.cpp>
#include <render/render.cpp>
#include <render/renderer.cpp>

#include <components/render.cpp>
#include <components/animation.cpp>

#include <glad/glad.c>