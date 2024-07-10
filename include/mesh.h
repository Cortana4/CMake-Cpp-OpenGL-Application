#pragma once
#include <string>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

class Mesh
{
public:
	Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<GLuint>& indices,
		const std::vector<std::shared_ptr<Texture>>& textures
	);
	Mesh(
		std::vector<Vertex>&& vertices,
		std::vector<GLuint>&& indices,
		std::vector<std::shared_ptr<Texture>>&& textures
	);
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) noexcept;
	~Mesh();

	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) noexcept;

	const std::vector<Vertex>& getVertices() const;
	const std::vector<GLuint>& getIndices() const;
	const std::vector<std::shared_ptr<Texture>>& getTextures() const;

	
	void draw(Shader& shader);

private:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;

	GLuint VAO, VBO, EBO;

	void setupGLObjects();
	void deleteGLObjects();
};
