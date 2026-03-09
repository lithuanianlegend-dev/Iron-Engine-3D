#pragma once

#include "../../Components/RenderableComponent.h"
#include "../../Components/TransformComponent.h"

#include "../../Rendering/Mesh/MeshPool.h"
#include "../../Rendering/Shader/ShaderLibrary.h"

#include <entt/entt.hpp>

namespace RenderSystem
{
	struct DrawCall {
		uint64_t sortkey;
		entt::entity entity;
	};

	void Render(entt::registry& registry);
}