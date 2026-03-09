#include "RenderSystem.h"

#include "../Engine/Components/TextureComponent.h"
#include "../Engine/Components/ModelComponent.h" 

namespace RenderSystem
{
	void Render(entt::registry& registry)
	{
		std::vector<DrawCall> drawCalls;

		// view<A, B> = only entities that have BOTH components
		auto view = registry.view<TransformComponent, RenderableComponent>();

		view.each([&](entt::entity entity, TransformComponent& t, RenderableComponent r) {
			if (!r.visible) return;
			DrawCall dc;
			dc.sortkey = ((uint64_t)r.shaderID << 32) | r.meshID;
			dc.entity = entity;
			drawCalls.push_back(dc);
			});

		std::sort(drawCalls.begin(), drawCalls.end(),
			[](const DrawCall& a, const DrawCall& b) {
				return a.sortkey < b.sortkey;
			});

		uint32_t boundShader = UINT32_MAX;
		for (auto& dc : drawCalls)
		{
			auto& r = registry.get<RenderableComponent>(dc.entity);
			auto& t = registry.get<TransformComponent>(dc.entity);

			if (r.shaderID != boundShader)
			{
				ShaderLibrary::Bind(r.shaderID);
				boundShader = r.shaderID;
			}

			ShaderLibrary::SetMat4(r.shaderID, "u_Model", t.worldMatrix);


			if (registry.any_of<ModelComponent>(dc.entity))
			{
				auto& model = registry.get<ModelComponent>(dc.entity);
				for (auto& sub : model.subMeshes)
				{
					if (sub.textureID != 0)
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(sub.texType, sub.textureID);
						ShaderLibrary::SetInt(r.shaderID, "u_Texture", 0);
					}

					MeshPool::Draw(sub.meshID);
				}
			}
			else
			{
				if (registry.any_of<TextureComponent>(dc.entity))
				{
					auto& tex = registry.get<TextureComponent>(dc.entity);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(tex.type, tex.id);
					ShaderLibrary::SetInt(r.shaderID, "u_Texture", 0);
				}
			}

			MeshPool::Draw(r.meshID);
		}
	}
}

	