#include <iomanip>
#include <sstream>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "textRenderer.h"


TextRenderer::TextRenderer(
	const std::filesystem::path& fontPath,
	unsigned int width, unsigned int height
)
	: window{ nullptr }
	, defaultSize{ glm::uvec2(width, height) }
	, maxCharSize{ glm::uvec2(0, 0) }
	, maxLetterSize{ glm::uvec2(0, 0) }
	, maxNumberSize{ glm::uvec2(0, 0) }
	, maxBearing{ glm::ivec2(0, 0) }
	, VAO{ 0 }
	, VBO{ 0 }
	, EBO{ 0 }
{
	std::stringstream errorMessage;
	errorMessage << "Error: TextRenderer::TextRenderer(): ";

	// this renderer is bound to the OpenGL context currently active when
	// calling this constructor and can only be used in this context
	GLFWwindow* currentContext = glfwGetCurrentContext();
	if (currentContext == nullptr)
	{
		errorMessage << "Constructor called without active OpenGL context." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(currentContext));
	if (window == nullptr)
	{
		errorMessage << "User pointer of current OpenGL context not set to a valid window handler." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	FT_Library lib;
	if (FT_Init_FreeType(&lib))
	{
		errorMessage << "FT_Init_FreeType() failed." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	FT_Face face;
	if (FT_New_Face(lib, fontPath.string().c_str(), 0, &face))
	{
		FT_Done_FreeType(lib);
		errorMessage
			<< "FT_New_Face() failed for font "
			<< fontPath << "." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	FT_Set_Pixel_Sizes(face, defaultSize.x, defaultSize.y);

	// OpenGL options needed by FreeType
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// FreeType only uses 1 byte per pixel (FT_PIXEL_MODE_GRAY) instead of 3 bytes (GL_RGB)
	// therefore the alignment for pixel data has to be changed to 1
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 characters of ASCII set
	bool success = true;
	for (unsigned int c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			success = false;
			errorMessage
				<< "FT_Load_Char() failed for char 0x"
				<< std::setw(2) << std::setfill('0') << std::hex << c << "." << std::endl;
			continue;
		}

		// texture begin
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		// texture end

		// store character for later use
		Char ch =
		{
			texture,
			glm::uvec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};

		chars.insert({ c, ch });

		// update max dimensions for...
		// all chars
		maxCharSize.x = std::max(maxCharSize.x, ch.size.x);
		maxCharSize.y = std::max(maxCharSize.y, ch.size.y);
		maxBearing.x = std::max(maxBearing.x, ch.bearing.x);
		maxBearing.y = std::max(maxBearing.y, ch.bearing.y);

		// numbers
		if (c >= '0' && c <= '9')
		{
			maxNumberSize.x = std::max(maxNumberSize.x, ch.size.x);
			maxNumberSize.y = std::max(maxNumberSize.y, ch.size.y);
		}
		// capitalized letters
		else if (c >= 'A' && c <= 'Z')
		{
			maxLetterSize.x = std::max(maxLetterSize.x, ch.size.x);
			maxLetterSize.y = std::max(maxLetterSize.y, ch.size.y);
		}
	}

	FT_Done_Face(face);
	FT_Done_FreeType(lib);

	if (!success)
	{
		deleteGLObjects();
		throw std::runtime_error(errorMessage.str());
	}

	// index array
	GLuint indices[] =
	{
		0, 1, 2,
		2, 3, 1
	};

// VAO begin
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO begin
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 4 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(
		0, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), reinterpret_cast<void*>(0)
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(1);
	// VBO end

	// EBO begin
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// EBO end

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// VAO end
}

TextRenderer::TextRenderer(TextRenderer&& other) noexcept
	: window{ std::move(other.window) }
	, defaultSize{ other.defaultSize }
	, maxCharSize{ other.maxCharSize }
	, maxLetterSize{ other.maxLetterSize }
	, maxNumberSize{ other.maxNumberSize }
	, maxBearing{ other.maxBearing }
	, chars{ std::move(other.chars) }
	, VAO{ 0 }
	, VBO{ 0 }
	, EBO{ 0 }
{
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
}

TextRenderer::~TextRenderer()
{
	deleteGLObjects();
}

TextRenderer& TextRenderer::operator=(TextRenderer&& other) noexcept
{
	if (this != &other)
	{
		deleteGLObjects();

		window = std::move(other.window);
		defaultSize = other.defaultSize;
		maxCharSize = other.maxCharSize;
		maxLetterSize = other.maxLetterSize;
		maxNumberSize = other.maxNumberSize;
		maxBearing = other.maxBearing;
		chars = std::move(other.chars);
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;

		other.VAO = 0;
		other.VBO = 0;
		other.EBO = 0;
	}

	return *this;
}

void TextRenderer::renderText(
	Shader& shader,
	const std::string& str,
	float x, float y, Origin origin,
	glm::vec3 color, float scale
)
{
	if (origin == TOP_LEFT)
		y = window->getHeight() - y;

	glViewport(0, 0, window->getWidth(), window->getHeight());

	// set active shader
	shader.useProgram();

	// set projection matrix in current shader
	glm::mat4 projection = glm::ortho<GLfloat>(
		0.0f, static_cast<GLfloat>(window->getWidth()),
		0.0f, static_cast<GLfloat>(window->getHeight())
	);
	shader.setUniformMatrix4fv("projection", glm::value_ptr<GLfloat>(projection));

	// set textColor in current shader
	shader.setUniform3f("texColor", color.x, color.y, color.z);
	
	// bind current vertex array
	glBindVertexArray(VAO);
	
	// iterate over all chars in str
	for (const char& c : str)
	{
		Char ch;

		// skip non-printable chars
		if (c < 32 || c > 126)
			continue;
		// when the current char was not loaded or is not available, '?' is rendered instead
		else if (chars.find(c) == chars.end())
			ch = chars.at('?');
		else
			ch = chars.at(c);

		GLfloat xPos = x + ch.bearing.x * scale;
		GLfloat yPos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;

		// vertex array
		GLfloat vertices[] =
		{
			// positions		// texture coordinates
			xPos,     yPos,		0.0f, 1.0f,	// top left
			xPos + w, yPos,		1.0f, 1.0f,	// top right
			xPos,     yPos + h,	0.0f, 0.0f,	// bottom left
			xPos + w, yPos + h,	1.0f, 0.0f	// bottom right
		};

		// copy vertex array into the OpenGL vertex buffer created previously
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// bind current texture object
		glBindTexture(GL_TEXTURE_2D, ch.texture);
		
		// draw glyph
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// unbind texture object
		glBindTexture(GL_TEXTURE_2D, 0);

		// move cursors to next glyph
		x += (ch.advance >> 6) * scale; // advance is measured in 1/64 pixels, divide by 64 to get advance in pixels
	}

	// unbind vertex array object
	glBindVertexArray(0);
}

unsigned int TextRenderer::getDefaultWidth() const
{
	return defaultSize.x;
}

unsigned int TextRenderer::getDefaultHeight() const
{
	return defaultSize.y;
}

unsigned int TextRenderer::getMaxCharWidth() const
{
	return maxCharSize.x;
}

unsigned int TextRenderer::getMaxCharHeight() const
{
	return maxCharSize.y;
}

unsigned int TextRenderer::getMaxLetterWidth() const
{
	return maxLetterSize.x;
}

unsigned int TextRenderer::getMaxLetterHeight() const
{
	return maxLetterSize.y;
}

unsigned int TextRenderer::getMaxNumberWidth() const
{
	return maxNumberSize.x;
}

unsigned int TextRenderer::getMaxNumberHeight() const
{
	return maxNumberSize.y;
}

int TextRenderer::getMaxBearingX() const
{
	return maxBearing.x;
}

int TextRenderer::getMaxBearingY() const
{
	return maxBearing.y;
}
void TextRenderer::deleteGLObjects()
{
	for (std::pair<const char, Char>& entry : chars)
		glDeleteTextures(1, &entry.second.texture);

	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
