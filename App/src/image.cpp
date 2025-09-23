#include <sstream>

#include "image.h"


int Image::instanceCount = 0;
bool Image::exceptionsEnabled = false;

Image::Image()
	: image { 0 }
	, error{ IL_NO_ERROR }
{
	initIf();
	instanceCount++;
}

Image::Image(const std::filesystem::path& path)
	: image{ 0 }
	, error{ IL_NO_ERROR }
{
	initIf();

	try { readFile(path); }
	catch (const std::runtime_error&)
	{
		shutDownIf();
		throw;
	}

	instanceCount++;
}

Image::Image(
	ILuint width, ILuint height, ILuint channels,
	ILenum format, ILenum type, void* data
)
	: image{ ilGenImage() }
	, error{ IL_NO_ERROR }
{
	initIf();

	ilBindImage(image);
	ilGetError();
	ilTexImage(width, height, 1, channels, format, type, data);
	error = ilGetError();
	ilBindImage(0);

	try { evaluateError("Error: Image::Image(): "); }
	catch (const std::runtime_error&)
	{
		ilDeleteImage(image);
		shutDownIf();
		throw;
	}

	instanceCount++;
}

Image::Image(const Image& other)
	: image{ ilGenImage() }
	, error{ other.error }
{
	initIf();

	ilBindImage(image);
	ilGetError();
	ilCopyImage(other.image);
	error = ilGetError();
	ilBindImage(0);

	try { evaluateError("Error: Image::Image(): "); }
	catch (const std::runtime_error&)
	{
		ilDeleteImage(image);
		shutDownIf();
		throw;
	}

	instanceCount++;
}

Image::Image(Image&& other) noexcept
	: image{ other.image }
	, error{ other.error }
{
	initIf();
	other.image = 0;
	instanceCount++;
}

Image::~Image()
{
	ilDeleteImage(image);
	instanceCount--;
	shutDownIf();
}

Image& Image::operator=(const Image& other)
{
	if (this != &other)
	{
		ilDeleteImage(image);

		image = ilGenImage();
		ilBindImage(image);
		ilGetError();
		ilCopyImage(other.image);
		error = ilGetError();
		ilBindImage(0);

		try { evaluateError("Error: Image::Image(): "); }
		catch (const std::runtime_error&)
		{
			ilDeleteImage(image);
			throw;
		}
	}

	return *this;
}

Image& Image::operator=(Image&& other) noexcept
{
	if (this != &other)
	{
		ilDeleteImage(image);

		image = other.image;
		error = other.error;

		other.image = 0;
	}

	return *this;
}

void Image::exceptions(bool enabled)
{
	Image::exceptionsEnabled = enabled;
}

bool Image::readFile(const std::filesystem::path& path)
{
	ilDeleteImage(image);

	image = ilGenImage();
	ilBindImage(image);
	ilGetError();
	ilLoadImage(path.string().c_str());
	error = ilGetError();
	ilBindImage(0);

	return evaluateError("Error: Image::readFile(): ");
}

bool Image::writeFile(const std::filesystem::path& path)
{
	try
	{
		std::filesystem::create_directories(path.parent_path());

		ilBindImage(image);
		ilGetError();
		ilSaveImage(path.string().c_str());
		error = ilGetError();
		ilBindImage(0);
	}
	catch (const std::filesystem::filesystem_error&)
	{
		error = IL_COULD_NOT_OPEN_FILE;
	}

	return evaluateError("Error: Image::writeFile(): ");
}

bool Image::convert(ILenum format, ILenum type)
{
	ilBindImage(image);
	ilGetError();
	ilConvertImage(format, type);
	error = ilGetError();
	ilBindImage(0);

	return evaluateError("Error: Image::convert(): ");
}

bool Image::flip()
{
	ilBindImage(image);
	ilGetError();
	iluFlipImage();
	error = ilGetError();
	ilBindImage(0);

	return evaluateError("Error: Image::flip(): ");
}

bool Image::setData(void* data)
{
	ilBindImage(image);
	ilGetError();
	ilTexImage(getWidth(), getHeight(), 1, getChannels(), getFormat(), getType(), data);
	error = ilGetError();
	ilBindImage(0);

	return evaluateError("Error: Image::setData(): ");
}

ILubyte* Image::getData() const
{
	ilBindImage(image);
	ILubyte* data = ilGetData();
	ilBindImage(0);
	return data;
}

ILuint Image::getSize() const
{
	ilBindImage(image);
	ILuint size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
	ilBindImage(0);
	return size;
}

ILenum Image::getFormat() const
{
	ilBindImage(image);
	ILenum format = ilGetInteger(IL_IMAGE_FORMAT);
	ilBindImage(0);
	return format;
}

ILenum Image::getType() const
{
	ilBindImage(image);
	ILenum type = ilGetInteger(IL_IMAGE_TYPE);
	ilBindImage(0);
	return type;
}

ILuint Image::getWidth() const
{
	ilBindImage(image);
	ILuint width = ilGetInteger(IL_IMAGE_WIDTH);
	ilBindImage(0);
	return width;
}

ILuint Image::getHeight() const
{
	ilBindImage(image);
	ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);
	ilBindImage(0);
	return height;
}

ILuint Image::getChannels() const
{
	ilBindImage(image);
	ILuint channels = ilGetInteger(IL_IMAGE_CHANNELS);
	ilBindImage(0);
	return channels;
}

ILenum Image::getError() const
{
	return error;
}

std::string Image::getErrorStr() const
{
	return iluErrorString(error);
}

void Image::initIf()
{
	if (instanceCount == 0) {
		ilInit();
		iluInit();
		ilutInit();

		// overwrite existing files
		ilEnable(IL_FILE_OVERWRITE);

		// make sure images are flipped vertically when needed
		// .bmp for example has origin at lower left,
		// while most other formats have origin at upper left
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

		// set language of error strings
		iluSetLanguage(ILU_ENGLISH);
	}
}

void Image::shutDownIf()
{
	if (instanceCount == 0)
		ilShutDown();
}

bool Image::evaluateError(std::string message)
{
	if (error == IL_NO_ERROR)
		return true;

	if (exceptionsEnabled)
	{
		std::stringstream errorMessage;
		errorMessage << message << getErrorStr() << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	return false;
}
