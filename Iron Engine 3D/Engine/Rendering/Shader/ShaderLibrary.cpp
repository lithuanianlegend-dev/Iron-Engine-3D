#include "ShaderLibrary.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <gtc/type_ptr.hpp>
#include <cassert>

std::unordered_map<uint32_t, GLuint>								 ShaderLibrary::shaders;
std::unordered_map<uint32_t, std::unordered_map<std::string, GLint>> ShaderLibrary::uniformCache;

uint32_t ShaderLibrary::nextID = 1; // 0 = invalid

uint32_t ShaderLibrary::Load(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertSrc = ReadFile(vertexPath);
	std::string fragSrc = ReadFile(fragmentPath);

	GLuint vert = Compile(vertSrc, GL_VERTEX_SHADER);
	GLuint frag = Compile(fragSrc, GL_FRAGMENT_SHADER);

	GLuint program = Link(vert, frag);

	// cleanup after linking
	glDeleteShader(vert);
	glDeleteShader(frag);

	uint32_t id = nextID++;
	shaders[id] = program;
	return id;
}

void ShaderLibrary::Bind(uint32_t shaderID)
{
	assert(shaders.count(shaderID) && "ERROR: ShaderID not found!");
	glUseProgram(shaders.at(shaderID));
}

void ShaderLibrary::UnBind()
{
	glUseProgram(0);
}

void ShaderLibrary::Unload(uint32_t shaderID)
{
	assert(shaders.count(shaderID) && "ERROR: ShaderID not found!");
	glDeleteProgram(shaders.at(shaderID));
	shaders.erase(shaderID);
	uniformCache.erase(shaderID);
}

void ShaderLibrary::UnloadAll()
{
	for (auto& [id, program] : shaders)
		glDeleteProgram(program);
	shaders.clear();
	uniformCache.clear();
}

// -----------------------------------------
// Uniforms
// -----------------------------------------

GLint ShaderLibrary::GetUniformLocation(uint32_t shaderID, const std::string& name)
{
	// check cache first
	auto& cache = uniformCache[shaderID];
	if (cache.count(name))
		return cache.at(name);

	Bind(shaderID);
	GLint location = glGetUniformLocation(shaders.at(shaderID), name.c_str());
	if (location == -1)
		std::cout << "WARNING: Uniform '" << name << "' not found in shader " << shaderID << std::endl;
	
	cache[name] = location;
	return location;
}

void ShaderLibrary::SetInt(uint32_t shaderID, const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(shaderID, name), value);
}

void ShaderLibrary::SetFloat(uint32_t shaderID, const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(shaderID, name), value);
}

void ShaderLibrary::SetVec3(uint32_t shaderID, const std::string& name, glm::vec3 value)
{
	glUniform3fv(GetUniformLocation(shaderID, name), 1, glm::value_ptr(value));
}

void ShaderLibrary::SetVec4(uint32_t shaderID, const std::string& name, glm::vec4 value)
{
	glUniform4fv(GetUniformLocation(shaderID, name), 1, glm::value_ptr(value));
}

void ShaderLibrary::SetMat4(uint32_t shaderID, const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(GetUniformLocation(shaderID, name), 1, GL_FALSE, glm::value_ptr(value));
}

// -----------------------------------------
// Private
// -----------------------------------------

std::string ShaderLibrary::ReadFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "ERROR: Could not open file: " << path << std::endl;
		return "";
	}
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

GLuint ShaderLibrary::Compile(const std::string& source, GLenum type)
{
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	// error check
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glad_glGetShaderInfoLog(shader, 512, nullptr, log);
		std::cout << "ERROR: Shader compilation failed:\n" << log << std::endl;
	}

	return shader;
}

GLuint ShaderLibrary::Link(GLuint vertex, GLuint fragment)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	GLint success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char log[512];
		glGetProgramInfoLog(program, 512, nullptr, log);
		std::cout << "ERROR: Shader linking failed:\n" << log << std::endl;
	}

	return program;
}
