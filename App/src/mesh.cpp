#include "mesh.h"


Mesh::Mesh(
	const std::vector<Vertex>& vertices,
	const std::vector<GLuint>& indices,
	const std::vector<std::shared_ptr<Texture>>& textures
)
	: vertices{ vertices }
	, indices{ indices }
	, textures{ textures }
	, VAO{ 0 }
	, VBO{ 0 }
	, EBO{ 0 }
{
	setupGLObjects();
}

Mesh::Mesh(
	std::vector<Vertex>&& vertices,
	std::vector<GLuint>&& indices,
	std::vector<std::shared_ptr<Texture>>&& textures
)
	: vertices{ std::move(vertices) }
	, indices{ std::move(indices) }
	, textures{ std::move(textures) }
	, VAO{ 0 }
	, VBO{ 0 }
	, EBO{ 0 }
{
	setupGLObjects();
}

Mesh::Mesh(Mesh&& other) noexcept
	: vertices{ std::move(other.vertices) }
	, indices{ std::move(other.indices) }
	, textures{ std::move(other.textures) }
	, VAO{ other.VAO }
	, VBO{ other.VBO }
	, EBO{ other.EBO }
{
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
}

Mesh::~Mesh()
{
	deleteGLObjects();
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other)
	{
		deleteGLObjects();

		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
		textures = std::move(other.textures);
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;

		other.VAO = 0;
		other.VBO = 0;
		other.EBO = 0;
	}

	return *this;
}

const std::vector<Vertex>& Mesh::getVertices() const
{
	return vertices;
}

const std::vector<GLuint>& Mesh::getIndices() const
{
	return indices;
}

const std::vector<std::shared_ptr<Texture>>& Mesh::getTextures() const
{
	return textures;
}

void Mesh::draw(Shader& shader)
{
	unsigned int diffuseIdx = 1;
	unsigned int specularIdx = 1;
	unsigned int normalIdx = 1;
	unsigned int heightIdx = 1;

	for (GLint i = 0; i < textures.size(); i++)
	{
		std::string name = textures[i]->getName();
		std::string idx;

		if (name == "texture_diffuse")
		{
			idx = std::to_string(diffuseIdx);
			diffuseIdx++;
		}
		else if (name == "texture_specular")
		{
			idx = std::to_string(specularIdx);
			specularIdx++;
		}
		else if (name == "texture_normal")
		{
			idx = std::to_string(normalIdx);
			normalIdx++;
		}
		else if (name == "texture_height")
		{
			idx = std::to_string(heightIdx++);
			heightIdx++;
		}

		glActiveTexture(GL_TEXTURE0 + i);
		shader.setUniform1i("material" + name + idx, i);
		glBindTexture(GL_TEXTURE_2D, textures[i]->getId());
		glActiveTexture(GL_TEXTURE0);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setupGLObjects()
{
	deleteGLObjects();

// VAO begin
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO begin
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
	// normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	// texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords)));
	// VBO end

	// EBO begin
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	// EBO end

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// VAO end
}

void Mesh::deleteGLObjects()
{
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
