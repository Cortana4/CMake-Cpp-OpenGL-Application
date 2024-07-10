#pragma once
#include <string>
#include <filesystem>
#include <memory>
#include <map>
#include <glm/glm.hpp>

#include "window.h"
#include "shader.h"


class TextRenderer
{
public:
	enum Origin
	{
		BOTTOM_LEFT,
		TOP_LEFT
	};

	// width is automatically calculated based on height when set to 0
	TextRenderer(
		const std::filesystem::path& fontPath,
		unsigned int width, unsigned int height
	);

	TextRenderer(const TextRenderer& other) = delete;
	TextRenderer(TextRenderer&& other) noexcept;
	~TextRenderer();

	TextRenderer& operator=(const TextRenderer& other) = delete;
	TextRenderer& operator=(TextRenderer&& other) noexcept;

	// (x, y) is the position of the bottom left pixel from the first
	// char in str measured from the BOTTOM_LEFT/TOP_LEFT window corner
	void renderText(
		Shader& shader,
		const std::string& str,
		float x, float y, Origin origin = BOTTOM_LEFT,
		glm::vec3 color = glm::vec3(1.0f, 0.804f, 0.133f),
		float scale = 1.0f
	);
	
	unsigned int getDefaultWidth() const;
	unsigned int getDefaultHeight() const;
	unsigned int getMaxCharWidth() const;
	unsigned int getMaxCharHeight() const;
	unsigned int getMaxLetterWidth() const;
	unsigned int getMaxLetterHeight() const;
	unsigned int getMaxNumberWidth() const;
	unsigned int getMaxNumberHeight() const;
	int getMaxBearingX() const;
	int getMaxBearingY() const;

private:
	// size.x:		width in pixels of the bitmap
	// size.y:		height in pixels of the bitmap
	// bearing.x:	horizontal position in pixels of the bitmap relative to the origin
	// bearing.y:	vertical position in pixels of the bitmap relative to the baseline
	// advance:		horizontal distance in 1/64 pixels from the origin to the origin of the next glyph

	struct Char
	{
		GLuint texture;
		glm::uvec2 size;
		glm::ivec2 bearing;
		int advance;
	};

	Window* window;

	glm::uvec2 defaultSize;
	glm::uvec2 maxCharSize;
	glm::uvec2 maxLetterSize;
	glm::uvec2 maxNumberSize;
	glm::ivec2 maxBearing;

	std::map<char, Char> chars;

	GLuint VAO, VBO, EBO;

	void deleteGLObjects();
};
