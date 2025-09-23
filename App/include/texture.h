#pragma once
#include <string>
#include <filesystem>
#include <GL/glew.h>


class Texture
{
public:
	Texture(
		const std::filesystem::path& path,
		const std::string& name
	);
	Texture(const Texture& other) = delete;
	Texture(Texture&& other) noexcept;
	~Texture();

	Texture& operator=(const Texture& other) = delete;
	Texture& operator=(Texture&& other) noexcept;

	GLuint getId() const;
	const std::string& getName() const;

private:
	GLuint id;
	std::string name;
};
