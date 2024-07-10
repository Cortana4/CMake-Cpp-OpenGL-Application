#include <sstream>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.h"


Model::Model(const std::filesystem::path& path)
{
	baseDir = path.parent_path();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::stringstream errorMessage;
		errorMessage
			<< "Error: Model::readFile(): "
			<< importer.GetErrorString()
			<< std::endl;

		throw std::runtime_error(errorMessage.str());
	}

	processNode(scene->mRootNode, scene);
}

void Model::draw(Shader& shader, glm::vec3 pos, GLfloat scale,
	glm::vec3 axis, GLfloat angle)
{
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, pos);
	model = glm::scale(model, scale * glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::rotate(model, glm::radians(angle), axis);

	shader.useProgram();
	shader.setUniformMatrix4fv("model", glm::value_ptr(model));

	for (Mesh& mesh : meshes)
		mesh.draw(shader);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;

	getVertices(vertices, mesh);
	getIndices(indices, mesh);
	getTextures(textures, mesh, scene);

	meshes.emplace_back(std::move(vertices), std::move(indices), std::move(textures));
}

void Model::getVertices(std::vector<Vertex>& vertices, aiMesh* mesh)
{
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		if (mesh->HasPositions())
		{
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;
		}

		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTextureCoords(0))
		{
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}
}

void Model::getIndices(std::vector<GLuint>& indices, aiMesh* mesh)
{
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
}

void Model::getTextures(std::vector<std::shared_ptr<Texture>>& textures,
	aiMesh* mesh, const aiScene* scene
)
{
	if (mesh->mMaterialIndex == 0)
		return;

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<std::pair<aiTextureType, std::string>> textureTypes = {
		{ aiTextureType_DIFFUSE, "texture_diffuse" },
		{ aiTextureType_SPECULAR, "texture_specular" },
		{ aiTextureType_HEIGHT, "texture_normal" },
		{ aiTextureType_AMBIENT, "texture_height" }
	};

	for (const std::pair<aiTextureType, std::string>& textureType : textureTypes)
	{
		for (unsigned int i = 0; i < material->GetTextureCount(textureType.first); i++)
		{
			aiString str;
			material->GetTexture(textureType.first, i, &str);
			std::filesystem::path path = baseDir / str.C_Str();

			auto it = loadedTextures.find(path);
			if (it != loadedTextures.end())
				textures.push_back(it->second);
			else
			{
				std::shared_ptr<Texture> texture = std::make_shared<Texture>(path, textureType.second);
				textures.push_back(texture);
				loadedTextures[path] = texture;
			}
		}
	}
}
