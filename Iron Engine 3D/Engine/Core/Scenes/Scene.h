#pragma once


// RENDERING
#include "../Engine/Rendering/Mesh/MeshPool.h"
#include "../Engine/Rendering/Shader/ShaderLibrary.h"

// COMPONENTS
#include "../Engine/Components/TransformComponent.h"
#include "../Engine/Components/RenderableComponent.h"

#include "../Engine/Core/Engine/Engine.h"

#include <entt/entt.hpp>
#include <glm.hpp>

class Scene {
public:
	void Init(entt::registry& registry);
	void Update(entt::registry& registry);
	void Destroy(entt::registry& registry);
private:
	uint32_t m_shaderID = 0;
	uint32_t m_meshID = 0;
};