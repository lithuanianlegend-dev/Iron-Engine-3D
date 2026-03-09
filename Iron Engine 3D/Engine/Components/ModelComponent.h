#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include <glad.h>

struct ModelComponent
{
	
	struct SubMesh
	{
		uint32_t meshID    = 0;
		uint32_t textureID = 0;
		GLenum   texType   = GL_TEXTURE_2D;
	};

	std::vector<SubMesh> subMeshes;
	std::string		     sourcePath;
	bool				 loaded = false;
};