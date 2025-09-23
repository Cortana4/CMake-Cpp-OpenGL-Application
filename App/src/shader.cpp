#include <fstream>
#include <sstream>
#include <memory>

#include "shader.h"


static_assert(
	sizeof(GLchar) == sizeof(char),
	"Assertion failed: Size of GLchar and char is not compatible."
);

Shader::Shader(
	const std::filesystem::path& computeShaderPath
)
	: program{ 0 }
{
	program = glCreateProgram();
	try
	{
		compileShader(computeShaderPath, GL_COMPUTE_SHADER);
		linkProgram();
	}
	catch (const std::ifstream::failure&)
	{
		deleteProgram();
		throw;
	}
	catch (const std::runtime_error&)
	{
		deleteProgram();
		throw;
	}
}

Shader::Shader(
	const std::filesystem::path& vertexShaderPath,
	const std::filesystem::path& fragmentShaderPath
)
	: program{ 0 }
{
	program = glCreateProgram();
	try
	{
		compileShader(vertexShaderPath, GL_VERTEX_SHADER);
		compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
		linkProgram();
	}
	catch (const std::ifstream::failure&)
	{
		deleteProgram();
		throw;
	}
	catch (const std::runtime_error&)
	{
		deleteProgram();
		throw;
	}
}

Shader::Shader(
	const std::filesystem::path& vertexShaderPath,
	const std::filesystem::path& geometryShaderPath,
	const std::filesystem::path& fragmentShaderPath
)
	: program{ 0 }
{
	program = glCreateProgram();
	try
	{
		compileShader(vertexShaderPath, GL_VERTEX_SHADER);
		compileShader(geometryShaderPath, GL_GEOMETRY_SHADER);
		compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
		linkProgram();
	}
	catch (const std::ifstream::failure&)
	{
		deleteProgram();
		throw;
	}
	catch (const std::runtime_error&)
	{
		deleteProgram();
		throw;
	}
}

Shader::Shader(
	const std::filesystem::path& vertexShaderPath,
	const std::filesystem::path& tessCtrlShaderPath,
	const std::filesystem::path& tessEvalShaderPath,
	const std::filesystem::path& fragmentShaderPath
)
	: program{ 0 }
{
	program = glCreateProgram();
	try
	{
		compileShader(vertexShaderPath, GL_VERTEX_SHADER);
		compileShader(tessCtrlShaderPath, GL_TESS_CONTROL_SHADER);
		compileShader(tessEvalShaderPath, GL_TESS_EVALUATION_SHADER);
		compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
		linkProgram();
	}
	catch (const std::ifstream::failure&)
	{
		deleteProgram();
		throw;
	}
	catch (const std::runtime_error&)
	{
		deleteProgram();
		throw;
	}
}

Shader::Shader(
	const std::filesystem::path& vertexShaderPath,
	const std::filesystem::path& tessCtrlShaderPath,
	const std::filesystem::path& tessEvalShaderPath,
	const std::filesystem::path& geometryShaderPath,
	const std::filesystem::path& fragmentShaderPath
)
	: program{ 0 }
{
	program = glCreateProgram();
	try
	{
		compileShader(vertexShaderPath, GL_VERTEX_SHADER);
		compileShader(tessCtrlShaderPath, GL_TESS_CONTROL_SHADER);
		compileShader(tessEvalShaderPath, GL_TESS_EVALUATION_SHADER);
		compileShader(geometryShaderPath, GL_GEOMETRY_SHADER);
		compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
		linkProgram();
	}
	catch (const std::ifstream::failure&)
	{
		deleteProgram();
		throw;
	}
	catch (const std::runtime_error&)
	{
		deleteProgram();
		throw;
	}
}

Shader::Shader(Shader&& other) noexcept
	: program{ other.program }
{
	other.program = 0;
}

Shader::~Shader()
{
	deleteProgram();
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this != &other)
	{
		deleteProgram();
		program = other.program;
		other.program = 0;
	}

	return *this;
}

void Shader::useProgram()
{
	glUseProgram(program);
}

void Shader::setUniform1f(const std::string& uniformName, GLfloat v0)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform1f(uniformLocation, v0);
}

void Shader::setUniform2f(const std::string& uniformName, GLfloat v0, GLfloat v1)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform2f(uniformLocation, v0, v1);
}

void Shader::setUniform3f(const std::string& uniformName, GLfloat v0, GLfloat v1, GLfloat v2)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform3f(uniformLocation, v0, v1, v2);
}

void Shader::setUniform4f(const std::string& uniformName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());	
	glUniform4f(uniformLocation, v0, v1, v2, v3);
}

void Shader::setUniform1i(const std::string& uniformName, GLint v0)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform1i(uniformLocation, v0);
}

void Shader::setUniform2i(const std::string& uniformName, GLint v0, GLint v1)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform2i(uniformLocation, v0, v1);
}

void Shader::setUniform3i(const std::string& uniformName, GLint v0, GLint v1, GLint v2)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform3i(uniformLocation, v0, v1, v2);
}

void Shader::setUniform4i(const std::string& uniformName, GLint v0, GLint v1, GLint v2, GLint v3)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform4i(uniformLocation, v0, v1, v2, v3);
}

void Shader::setUniform1ui(const std::string& uniformName, GLuint v0)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform1ui(uniformLocation, v0);
}

void Shader::setUniform2ui(const std::string& uniformName, GLuint v0, GLuint v1)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform2ui(uniformLocation, v0, v1);
}

void Shader::setUniform3ui(const std::string& uniformName, GLuint v0, GLuint v1, GLuint v2)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform3ui(uniformLocation, v0, v1, v2);
}

void Shader::setUniform4ui(const std::string& uniformName, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform4ui(uniformLocation, v0, v1, v2, v3);
}

void Shader::setUniform1fv(const std::string& uniformName, const GLfloat* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform1fv(uniformLocation, count, value);
}

void Shader::setUniform2fv(const std::string& uniformName, const GLfloat* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform2fv(uniformLocation, count, value);
}

void Shader::setUniform3fv(const std::string& uniformName, const GLfloat* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform3fv(uniformLocation, count, value);
}

void Shader::setUniform4fv(const std::string& uniformName, const GLfloat* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform4fv(uniformLocation, count, value);
}

void Shader::setUniform1iv(const std::string& uniformName, const GLint* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform1iv(uniformLocation, count, value);
}

void Shader::setUniform2iv(const std::string& uniformName, const GLint* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform2iv(uniformLocation, count, value);
}

void Shader::setUniform3iv(const std::string& uniformName, const GLint* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform3iv(uniformLocation, count, value);
}

void Shader::setUniform4iv(const std::string& uniformName, const GLint* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform4iv(uniformLocation, count, value);
}

void Shader::setUniform1uiv(const std::string& uniformName, const GLuint* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform1uiv(uniformLocation, count, value);
}

void Shader::setUniform2uiv(const std::string& uniformName, const GLuint* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform2uiv(uniformLocation, count, value);
}

void Shader::setUniform3uiv(const std::string& uniformName, const GLuint* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform3uiv(uniformLocation, count, value);
}

void Shader::setUniform4uiv(const std::string& uniformName, const GLuint* value, GLsizei count)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniform4uiv(uniformLocation, count, value);
}

void Shader::setUniformMatrix2fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix2fv(uniformLocation, count, transpose, value);
}

void Shader::setUniformMatrix3fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix3fv(uniformLocation, count, transpose, value);
}

void Shader::setUniformMatrix4fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix4fv(uniformLocation, count, transpose, value);
}

void Shader::setUniformMatrix2x3fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix2x3fv(uniformLocation, count, transpose, value);
}

void Shader::setUniformMatrix3x2fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix3x2fv(uniformLocation, count, transpose, value);
}

void Shader::setUniformMatrix2x4fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix2x4fv(uniformLocation, count, transpose, value);
}

void Shader::setUniformMatrix4x2fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix4x2fv(uniformLocation, count, transpose, value);
}

void Shader::setUniformMatrix3x4fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix3x4fv(uniformLocation, count, transpose, value);
}

void Shader::setUniformMatrix4x3fv(const std::string& uniformName, const GLfloat* value, GLsizei count, bool transpose)
{
	GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
	glUniformMatrix4x3fv(uniformLocation, count, transpose, value);
}

void Shader::compileShader(const std::filesystem::path& shaderPath, GLenum shaderType)
{
	std::stringstream errorMessage;
	std::string shaderTypeStr;

	errorMessage << "Error: Shader::compileShader(): ";

	// order corresponds to the OpenGL rendering pipeline
	switch (shaderType)
	{
	case GL_VERTEX_SHADER:			// 1. vertex shader (needed)
		shaderTypeStr = "vertex";
		break;
	case GL_TESS_CONTROL_SHADER:	// 2. tessellation control shader (optional)
		shaderTypeStr = "tessellation control";
		break;
	case GL_TESS_EVALUATION_SHADER:	// 3. tessellation evaluation shader (optional)
		shaderTypeStr = "tessellation evaluation";
		break;
	case GL_GEOMETRY_SHADER:		// 4. geometry shader (optional)
		shaderTypeStr = "geometry";
		break;
	case GL_FRAGMENT_SHADER:		// 5. fragment shader (needed)
		shaderTypeStr = "fragment";
		break;
	case GL_COMPUTE_SHADER:			// independent compute shader (optional)
		shaderTypeStr = "compute";
		break;
	default:
		errorMessage << "Unknown shader type." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	// open shader file and read its source
	std::ifstream shaderFile;
	std::stringstream shaderSourceSS;
	std::string shaderSourceStr;
	const GLchar* shaderSource;

	shaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

	try
	{
		shaderFile.open(shaderPath);
		shaderSourceSS << shaderFile.rdbuf();
		shaderFile.close();
		shaderSourceStr = shaderSourceSS.str();
		shaderSource = shaderSourceStr.c_str();
	}
	catch (const std::ifstream::failure&)
	{
		errorMessage << "Reading file " << shaderPath << " failed." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	// compile shader
	GLuint shader;
	GLint success;
	GLint infoLogLen;
	std::unique_ptr<GLchar[]> infoLogBuf;

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
		infoLogBuf = std::make_unique<GLchar[]>(infoLogLen);
		glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLogBuf.get());

		glDeleteShader(shader);
		errorMessage
			<< "Compilation of " << shaderTypeStr
			<< " shader " << shaderPath << " failed with:" << std::endl
			<< infoLogBuf << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	glAttachShader(program, shader);
	glDeleteShader(shader);
}

void Shader::linkProgram()
{
	// link shader program
	GLint success;
	GLint infoLogLen;
	std::unique_ptr<GLchar[]> infoLogBuf;
	std::stringstream errorMessage;
	errorMessage << "Error: Shader::linkProgram(): ";

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
		infoLogBuf = std::make_unique<GLchar[]>(infoLogLen);
		glGetProgramInfoLog(program, infoLogLen, nullptr, infoLogBuf.get());

		errorMessage
			<< "Linking failed with: " << std::endl
			<< infoLogBuf << std::endl;
		throw std::runtime_error(errorMessage.str());
	}
}

void Shader::deleteProgram()
{
	glDeleteProgram(program);
}
