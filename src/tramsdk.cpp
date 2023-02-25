#define ENGINE_EDITOR_MODE
#include <framework/core.cpp>
#include <framework/async.cpp>
#include <framework/stats.cpp>
#include <framework/system.cpp>
#include <framework/event.cpp>
#include <framework/uid.cpp>
#include <framework/logging.cpp>
#include <framework/language.cpp>

#include <render/opengl/renderer.cpp>
#include <render/opengl/shader.cpp>

#include <render/armature.cpp>
#include <render/material.cpp>
#include <render/model.cpp>
#include <render/render.cpp>
#include <render/render_misc.cpp>
#include <render/renderer.cpp>

#include <components/rendercomponent.cpp>
#include <components/armaturecomponent.cpp>
template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool("render component pool", 250, false);
