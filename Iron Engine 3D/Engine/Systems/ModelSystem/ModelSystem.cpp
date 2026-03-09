#include "ModelSystem.h"

#include "../../Components/ModelComponent.h"
#include "../../Components/RenderableComponent.h"
#include "../../Components/TextureComponent.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad.h>
#include <stb_image.h>
#include "../../Rendering/Mesh/MeshPool.h"

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <cassert>

// Texture Cache
static std::unordered_map<std::string, uint32_t> s_texCache;

static uint32_t LoadTexture(const std::string& path)
{
	auto it = s_texCache.find(path);
	if (it != s_texCache.end()) return it->second;

	uint32_t id = 0;
	glGenTextures(1, &id);

	int width, height, nrchannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrchannels, 0);
	if (!data)
	{
		std::cerr << "[MODEL SYSTEM] stbi failed: " << path << "\n"; // new error format. wowwwww
		return 0;
	}

	GLenum fmt = (nrchannels == 4) ? GL_RGBA : (nrchannels == 3) ? GL_RGB : GL_RED; // also different approach. i feel so smart

	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	stbi_image_free(data);
	s_texCache[path] = id;
	return id;
}

static uint32_t ProcessMesh(aiMesh* mesh)
{
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex v{};
        v.position[0] = mesh->mVertices[i].x;
        v.position[1] = mesh->mVertices[i].y;
        v.position[2] = mesh->mVertices[i].z;

        if (mesh->HasNormals()) {
            v.normal[0] = mesh->mNormals[i].x;
            v.normal[1] = mesh->mNormals[i].y;
            v.normal[2] = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0]) {
            v.texCoord[0] = mesh->mTextureCoords[0][i].x;
            v.texCoord[1] = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(v);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
        for (uint32_t j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
            indices.push_back(mesh->mFaces[i].mIndices[j]);

    return MeshPool::Upload(vertices, indices);
}

static uint32_t ProcessMaterial(aiMaterial* mat,
    aiTextureType type,
    const std::string& dir)
{
    if (mat->GetTextureCount(type) == 0) return 0;

    aiString str;
    mat->GetTexture(type, 0, &str);
    std::string fullPath = dir + "/" + str.C_Str();
    return LoadTexture(fullPath);
}

static void ProcessNode(aiNode* node, const aiScene* scene,
    const std::string& dir,
    uint32_t shaderID,
    ModelComponent& model)
{
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
        aiMaterial* aimat = scene->mMaterials[aimesh->mMaterialIndex];

        ModelComponent::SubMesh sub;
        sub.meshID = ProcessMesh(aimesh);
        sub.textureID = ProcessMaterial(aimat, aiTextureType_DIFFUSE, dir);
        sub.texType = GL_TEXTURE_2D;

        model.subMeshes.push_back(sub);
    }

    for (uint32_t i = 0; i < node->mNumChildren; ++i)
        ProcessNode(node->mChildren[i], scene, dir, shaderID, model);
}

namespace ModelSystem
{
    bool Load(entt::registry& registry, entt::entity entity,
        const std::string& filepath, uint32_t shaderID)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath,
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "[ModelSystem] Assimp: " << importer.GetErrorString() << "\n";
            return false;
        }

        std::string dir = std::filesystem::path(filepath).parent_path().string();

        ModelComponent& model = registry.emplace_or_replace<ModelComponent>(entity);
        model.sourcePath = filepath;

        ProcessNode(scene->mRootNode, scene, dir, shaderID, model);

        // Build one RenderableComponent that covers the first sub-mesh.
        // If you need per-submesh entities, split here instead.
        if (!model.subMeshes.empty())
        {
            auto& r = registry.emplace_or_replace<RenderableComponent>(entity);
            r.meshID = model.subMeshes[0].meshID;
            r.shaderID = shaderID;
            r.visible = true;

            // Attach texture from first diffuse map (if any)
            if (model.subMeshes[0].textureID != 0)
            {
                auto& tex = registry.emplace_or_replace<TextureComponent>(entity);
                tex.id = model.subMeshes[0].textureID;
                tex.type = GL_TEXTURE_2D;
            }
        }

        model.loaded = true;
        std::cout << "[ModelSystem] Loaded '" << filepath
            << "' (" << model.subMeshes.size() << " sub-meshes)\n";
        return true;
    }

    void Unload(entt::registry& registry, entt::entity entity)
    {
        if (!registry.all_of<ModelComponent>(entity)) return;

        auto& model = registry.get<ModelComponent>(entity);
        for (auto& sub : model.subMeshes)
            MeshPool::Unload(sub.meshID);

        registry.remove<ModelComponent>(entity);
        registry.remove<RenderableComponent>(entity);
        if (registry.all_of<TextureComponent>(entity))
            registry.remove<TextureComponent>(entity);
    }
}
