#include "MeshPool.h"
#include <cassert>

std::unordered_map<uint32_t, Mesh> MeshPool::meshes;
uint32_t MeshPool::nextID = 1; // 0 = invalid Mesh or Null

uint32_t MeshPool::Upload(const std::vector<Vertex>& vertices,
						 const std::vector<uint32_t>& indices)
{
	Mesh mesh;
	mesh.indexCount = static_cast<uint32_t>(indices.size());
	mesh.vertexCount = static_cast<uint32_t>(vertices.size());

	// VAO
	glGenVertexArrays(1, &mesh.VAO);
	glBindVertexArray(mesh.VAO);

	// VBO
	glGenBuffers(1, &mesh.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_STATIC_DRAW);

	// IBO
	glGenBuffers(1, &mesh.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 indices.size() * sizeof(uint32_t),
				 indices.data(),
				 GL_STATIC_DRAW);

	// position (location = 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	// normal (location = 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// texCoords (location = 2)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	uint32_t id = nextID++;
	meshes[id] = mesh;
	return id;
}

void MeshPool::Draw(uint32_t meshID)
{
	assert(meshes.count(meshID) && "ERROR: MeshID not found!");
	const Mesh& mesh = meshes.at(meshID);

	glBindVertexArray(mesh.VAO);

	if (mesh.indexCount > 0)
		glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
	else
		glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);

	glBindVertexArray(0);
}

void MeshPool::Unload(uint32_t meshID)
{
	assert(meshes.count(meshID) && "ERROR: MeshID not found!");
	Mesh& mesh = meshes.at(meshID);
	glDeleteVertexArrays(1, &mesh.VAO);
	glDeleteBuffers(1, &mesh.VBO);
	glDeleteBuffers(1, &mesh.IBO);
	meshes.erase(meshID);
}

void MeshPool::UnloadAll()
{
	for (auto& [id, mesh] : meshes)
	{
		glDeleteVertexArrays(1, &mesh.VAO);
		glDeleteBuffers(1, &mesh.VBO);
		glDeleteBuffers(1, &mesh.IBO);
	}
	meshes.clear();
}