#include "TransformSystem.h"

#include <entt/entt.hpp>
#include <gtc/matrix_transform.hpp>
#include "../../Components/TransformComponent.h"

namespace TransformSystem {
	void Update(entt::registry& registry)
	{
		// view<T> gives you only entities that have TransformComponent
		auto view = registry.view<TransformComponent>();

		view.each([](TransformComponent& t) {
			if (!t.dirty) return;

			glm::mat4 m = glm::mat4(1.0f);
			m = glm::translate(m, t.position);
			m = glm::rotate(m, glm::radians(t.rotation.x), { 1,0,0 });
			m = glm::rotate(m, glm::radians(t.rotation.y), { 0,1,0 });
			m = glm::rotate(m, glm::radians(t.rotation.z), { 0,0,1 });
			m = glm::scale(m, t.scale);
			t.worldMatrix = m;
			t.dirty = false;
		});
	}
}