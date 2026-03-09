#include "Scene.h"
#include <gtc/matrix_transform.hpp>
#include <../../Engine/Components/CameraComponent.h>
#include <../../Engine/Core/Engine/Engine.h>
#include <../../Engine/Core/Time/Time.h>
#include <../../Engine/Components/TextureComponent.h>
#include <../../Engine/Components/RigidBodyComponent.h>

#include "../../Engine/Rendering/Textures/TextureLoader.h"
#include "../../Engine/Systems/ModelSystem/ModelSystem.h"
#include "../../Engine/Systems/PhysicsSystem/IronPhysicsSystem.h"

void Scene::Init(entt::registry& registry)
{
    // Load shaders
    m_shaderID = ShaderLibrary::Load("ShaderFiles/basic.vs", "ShaderFiles/basic.frag");
    m_shaderIDcube = ShaderLibrary::Load("ShaderFiles/lightCube.vs", "ShaderFiles/lightCube.frag");

    IronEngine::Core.SetCurrentShader(m_shaderID);

    // Create entities
    m_sponza = registry.create();
    m_floor = registry.create();
    m_camera = registry.create();
    m_lightCube = registry.create();

    // Create transforms
    auto& sponzaTransform = registry.emplace<TransformComponent>(m_sponza);
    auto& lightTransform = registry.emplace<TransformComponent>(m_lightCube);
    auto& floorTransform = registry.emplace<TransformComponent>(m_floor);

    // Set floor size and position BEFORE physics
    floorTransform.position = glm::vec3(0.0f, -10.0f, 0.0f);
    floorTransform.scale = glm::vec3(10.0f, 0.1f, 10.0f);

    // Load models
    ModelSystem::Load(registry, m_sponza, "assets/models/backpack/backpack.obj", m_shaderID);
    ModelSystem::Load(registry, m_lightCube, "assets/models/primitives/sphere/sphere.obj", m_shaderIDcube);
    ModelSystem::Load(registry, m_floor, "assets/models/primitives/Cube/Cube.obj", m_shaderIDcube);

    // Initialize physics
    PhysicsSystem::Init();

    // Create RigidBodies
    auto& sponzaRB = registry.emplace<RigidBodyComponent>(m_sponza);
    auto& floorRB = registry.emplace<RigidBodyComponent>(m_floor);

    PhysicsSystem::RegisterBody(registry, m_sponza, false); // Dynamic
    PhysicsSystem::RegisterBody(registry, m_floor, true);    // Static floor

    // Camera
    registry.emplace<CameraComponent>(m_camera);
    registry.emplace<TransformComponent>(m_camera);
}

void Scene::Update(entt::registry& registry)
{
    auto& camComp = registry.get<CameraComponent>(m_camera);
    auto& camTransform = registry.get<TransformComponent>(m_camera);

    // View matrix
    glm::mat4 view = glm::lookAt(
        camComp.engineCamera.Position,
        camComp.engineCamera.Position + camComp.engineCamera.Front,
        camComp.engineCamera.Up
    );

    // Step physics first
    PhysicsSystem::Step(registry, IronEngine::Time.GetDeltaTime());

    // Retrieve updated transform for dynamic objects
    auto& sponzaTransform = registry.get<TransformComponent>(m_sponza);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), sponzaTransform.position);
    model = glm::rotate(model, glm::radians(sponzaTransform.rotation.z), glm::vec3(-3, 0, 0));
    model = glm::scale(model, sponzaTransform.scale);
    sponzaTransform.worldMatrix = model;

    // Projection
    int width, height;
    glfwGetFramebufferSize(&IronEngine::Core.GetWindow(), &width, &height);
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    glm::mat4 projection = glm::perspective(glm::radians(camComp.engineCamera.Zoom), aspectRatio, 0.1f, 100.0f);

    // Set shader uniforms for dynamic model
    ShaderLibrary::Bind(m_shaderID);
    ShaderLibrary::SetMat4(m_shaderID, "u_View", view);
    ShaderLibrary::SetMat4(m_shaderID, "u_Model", model);
    ShaderLibrary::SetMat4(m_shaderID, "u_Projection", projection);

    // Light cube
    auto& lightTransform = registry.get<TransformComponent>(m_lightCube);
    lightTransform.position = glm::vec3(0, 90, 90);
    lightTransform.scale = glm::vec3(0.1f);

    glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightTransform.position);
    lightModel = glm::scale(lightModel, lightTransform.scale);
    lightTransform.worldMatrix = lightModel;

    ShaderLibrary::Bind(m_shaderIDcube);
    ShaderLibrary::SetMat4(m_shaderIDcube, "u_View", view);
    ShaderLibrary::SetMat4(m_shaderIDcube, "u_Model", lightModel);
    ShaderLibrary::SetMat4(m_shaderIDcube, "u_Projection", projection);

    // Floor model (static, no physics update)
    auto& floorTransform = registry.get<TransformComponent>(m_floor);
    glm::mat4 floorModel = glm::translate(glm::mat4(1.0f), floorTransform.position);
    floorModel = glm::scale(floorModel, floorTransform.scale);
    floorTransform.worldMatrix = floorModel;

    ShaderLibrary::Bind(m_shaderIDcube);
    ShaderLibrary::SetMat4(m_shaderIDcube, "u_Model", floorModel);

    // Lighting uniforms
    ShaderLibrary::SetFloat(m_shaderID, "u_AmbientStrength", 0.1f);
    ShaderLibrary::SetFloat(m_shaderID, "u_SpecularStrength", 1.0f);
    ShaderLibrary::SetVec3(m_shaderID, "u_LightColor", glm::vec3(1.0f));
    ShaderLibrary::SetVec3(m_shaderID, "u_ObjectColor", glm::vec3(1.0f));
    ShaderLibrary::SetVec3(m_shaderID, "u_ViewPos", camTransform.position);
    ShaderLibrary::SetVec3(m_shaderID, "u_LightPos", lightTransform.position);

    IronEngine::Core.UpdateProjection();
}

void Scene::Destroy(entt::registry& registry)
{
    registry.clear();
    ShaderLibrary::UnloadAll();
    MeshPool::UnloadAll();
}