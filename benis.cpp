#include <core.cpp>
#include <async.cpp>
#include <worldcell.cpp>
#include <entity.cpp>

#include <render/armature.cpp>
#include <render/armaturecomponent.cpp>
#include <render/material.cpp>
#include <render/model.cpp>
#include <render/opengl_loader.cpp>
#include <render/opengl_renderer.cpp>
#include <render/opengl_shader.cpp>
#include <render/render.cpp>
#include <render/render_misc.cpp>

#include <components/rendercomponent.h>
template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool("render component pool", 250, false);