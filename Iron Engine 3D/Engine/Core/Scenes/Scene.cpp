#include "Scene.h"
#include <gtc/matrix_transform.hpp>

void Scene::Init(entt::registry& registry)
{
	// load the shading shaders!
	m_shaderID = ShaderLibrary::Load("ShaderFiles/basic.vs", "ShaderFiles/basic.frag");
	IronEngine::Core.SetCurrentShader(m_shaderID);


	std::vector<Vertex> verts = {
		//  position              normal        texcoord
		{{ 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}},
		{{-1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
		{{ 1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	};

	std::vector<uint32_t> inds = {
		0, 1, 2
	};
	m_meshID = MeshPool::Upload(verts, inds);

	// create entities
	entt::entity e = registry.create();
	registry.emplace<TransformComponent>(e);
	registry.emplace<RenderableComponent>(e, m_meshID, m_shaderID, 0u);
}

void Scene::Update(entt::registry& registry)
{


	// Move camera back so the triangle is visible
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 2.0f),  // camera position - 3 units back
		glm::vec3(0.0f, 0.0f, 0.0f),  // looking at origin
		glm::vec3(0.0f, 1.0f, 0.0f)   // up vector
	);

	ShaderLibrary::Bind(m_shaderID);
	ShaderLibrary::SetMat4(m_shaderID, "u_View", view);
	IronEngine::Core.UpdateProjection();

	IronEngine::Core.UpdateProjection();
}

void Scene::Destroy(entt::registry& registry)
{
	registry.clear();
	ShaderLibrary::UnloadAll();
	MeshPool::UnloadAll();
}