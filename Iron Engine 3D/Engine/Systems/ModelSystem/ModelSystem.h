#pragma once
#include <string>
#include <entt/entt.hpp> 

namespace ModelSystem
{

	bool Load(entt::registry& registry, entt::entity entity,
		const std::string& filepath,
		uint32_t shaderID);

	void Unload(entt::registry& registry, entt::entity entity);
}