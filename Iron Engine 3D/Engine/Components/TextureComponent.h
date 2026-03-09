#pragma once

#include <glad.h>
#include <string>

struct TextureComponent
{
	GLuint id = 0;
	std::string path;
	GLenum type = GL_TEXTURE_2D;

	TextureComponent() = default;
	TextureComponent(GLuint texID, const std::string& texPath, GLenum texType = GL_TEXTURE_2D) : id(texID), path(texPath), type(texType) {}
};