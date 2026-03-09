#pragma once

#include <entt/entt.hpp>
#include <gtc/matrix_transform.hpp>

#include "../../Components/TransformComponent.h"

namespace TransformSystem {
	void Update(entt::registry& registry);
}