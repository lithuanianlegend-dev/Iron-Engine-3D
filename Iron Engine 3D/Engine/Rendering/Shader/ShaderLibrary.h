#pragma once
#include <glad.h>
#include <string>
#include <unordered_map>
#include <glm.hpp>

class ShaderLibrary {
public:
	static uint32_t  Load(const std::string& vertexPath, const std::string& fragmentPath);
	static void      Bind(uint32_t shaderID);
	static void		 UnBind();
	static void		 Unload(uint32_t shaderID);
	static void		 UnloadAll();

	// Uniforms
	static void		 SetInt (uint32_t shaderID, const std::string& name, int value);
	static void	     SetFloat (uint32_t shaderID, const std::string& name, float value);
	static void		 SetVec3 (uint32_t shaderID, const std::string& name, glm::vec3 value);
	static void	     SetVec4 (uint32_t shaderID, const std::string& name, glm::vec4 value);
	static void		 SetMat4 (uint32_t shaderID, const std::string& name, glm::mat4 value);
private:
	static std::string ReadFile(const std::string& path);
	static GLuint Compile(const std::string& source, GLenum type);
	static GLuint Link(GLuint vertex, GLuint fragment);
	static GLint GetUniformLocation(uint32_t shaderID, const std::string& name);

	static std::unordered_map<uint32_t, GLuint> shaders;		// shaderID -> GL program
	static std::unordered_map<uint32_t,
		std::unordered_map<std::string, GLint>>uniformCache;    // cache uniform locations
	static uint32_t nextID;
};