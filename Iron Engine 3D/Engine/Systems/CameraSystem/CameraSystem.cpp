#include "CameraSystem.h"

#include "../../Engine/Components/TransformComponent.h"

#include "../../Engine/Core/Engine/Engine.h"

namespace CameraSystem
{
    void Update(entt::registry& registry)
    {
        auto view = registry.view<TransformComponent, CameraComponent>();

        for (auto entity : view)
        {
            auto& transform = view.get<TransformComponent>(entity);
            auto& camera = view.get<CameraComponent>(entity);

            if (!camera.primary)
                continue;

            float aspectRatio = (float)IronEngine::Core.WindowWidth() / (float)IronEngine::Core.WindowHeight();

            camera.view = camera.engineCamera.GetViewMatrix();
            camera.projection = glm::perspective(glm::radians(camera.engineCamera.Zoom),                    
                                                aspectRatio,
                                                camera.nearPlane,
                                                camera.farPlane);

            transform.position = camera.engineCamera.Position;
        }
    }
}