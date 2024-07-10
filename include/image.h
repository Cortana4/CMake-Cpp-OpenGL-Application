#pragma once
#include <filesystem>
#include <string>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


class Image
{
public:
	Image();
	Image(const std::filesystem::path& path);
	Image(
		ILuint width, ILuint height, ILuint ch,
		ILenum format, ILenum type, void* data = nullptr
	);

	Image(const Image& other);
	Image(Image&& other) noexcept;
	~Image();

	Image& operator=(const Image& other);
	Image& operator=(Image&& other) noexcept;

	static void exceptions(bool enabled);

	bool readFile(const std::filesystem::path& path);
	bool writeFile(const std::filesystem::path& path);
	bool convert(ILenum format, ILenum type);
	bool flip();

	bool setData(void* data);

	ILubyte* getData() const;
	ILuint getSize() const;

	ILenum getFormat() const;
	ILenum getType() const;

	ILuint getWidth() const;
	ILuint getHeight() const;
	ILuint getChannels() const;

	ILenum getError() const;
	std::string getErrorStr() const;

private:
	static int instanceCount;
	static bool exceptionsEnabled;

	ILuint image;
	ILenum error;

	void initIf();
	void shutDownIf();
	bool evaluateError(std::string message);
	
};
