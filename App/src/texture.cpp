#include "texture.h"
#include "image.h"


Texture::Texture(
	const std::filesystem::path& path,
	const std::string& name
)
	: id{ 0 }
	, name{ name }
{
	Image image;
	if (!image.readFile(path))
	{
		std::string error = image.getErrorStr();
	}

	image.flip();

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		image.getWidth(),
		image.getHeight(),
		0,
		image.getFormat(),
		image.getType(),
		image.getData()
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(Texture&& other) noexcept
	: id{ other.id }
	, name{ std::move(other.name) }
{
	other.id = 0;
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (this != &other)
	{
		glDeleteTextures(1, &id);

		id = other.id;
		name = std::move(other.name);

		other.id = 0;
	}

	return *this;
}

GLuint Texture::getId() const
{
	return id;
}

const std::string& Texture::getName() const
{
	return name;
}
