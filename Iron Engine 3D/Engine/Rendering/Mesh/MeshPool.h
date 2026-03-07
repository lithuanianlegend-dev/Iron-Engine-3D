#pragma once
#include <glad.h>
#include <vector>
#include <unordered_map>
#include <cstdint>

struct Mesh
{
	GLuint VAO			 = 0;
	GLuint VBO			 = 0;
	GLuint IBO			 = 0;
	uint32_t indexCount  = 0;
	uint32_t vertexCount = 0;
};

struct Vertex
{
	float position[3];
	float normal[3];
	float texCoord[2];
};

class MeshPool {
public:
	static uint32_t Upload(const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indices);
	static void Draw(uint32_t meshID);
	static void Unload(uint32_t meshID);
	static void UnloadAll();

private:
	static std::unordered_map<uint32_t, Mesh> meshes;
	static uint32_t nextID;
};