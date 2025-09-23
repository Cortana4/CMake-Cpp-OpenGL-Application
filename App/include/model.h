#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "mesh.h"


class Model
{
public:
	Model(const std::filesystem::path& path);

	void draw(
		Shader& shader,
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		GLfloat scale = 1.0f,
		glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f),
		GLfloat angle = 0.0f
	);

private:
	std::vector<Mesh> meshes;
	std::unordered_map<
		std::filesystem::path,
		std::shared_ptr<Texture>
	> loadedTextures;

	std::filesystem::path baseDir;

	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);

	void getVertices(std::vector<Vertex>& vertices, aiMesh* mesh);
	void getIndices(std::vector<GLuint>& indices, aiMesh* mesh);
	void getTextures(std::vector<std::shared_ptr<Texture>>& textures,
		aiMesh* mesh, const aiScene* scene
	);

};
