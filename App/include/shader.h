#pragma once
#include <string>
#include <filesystem>
#include <GL/glew.h>


class Shader
{
public:
	Shader(
		const std::filesystem::path& computeShaderPath
	);

	Shader(
		const std::filesystem::path& vertexShaderPath,
		const std::filesystem::path& fragmentShaderPath
	);

	Shader(
		const std::filesystem::path& vertexShaderPath,
		const std::filesystem::path& geometryShaderPath,
		const std::filesystem::path& fragmentShaderPath
	);

	Shader(
		const std::filesystem::path& vertexShaderPath,
		const std::filesystem::path& tessCtrlShaderPath,
		const std::filesystem::path& tessEvalShaderPath,
		const std::filesystem::path& fragmentShaderPath
	);

	Shader(
		const std::filesystem::path& vertexShaderPath,
		const std::filesystem::path& tessCtrlShaderPath,
		const std::filesystem::path& tessEvalShaderPath,
		const std::filesystem::path& geometryShaderPath,
		const std::filesystem::path& fragmentShaderPath
	);

	Shader(const Shader& other) = delete;
	Shader(Shader&& other) noexcept;
	~Shader();

	Shader& operator=(const Shader& other) = delete;
	Shader& operator=(Shader&& other) noexcept;

	void useProgram();

	void setUniform1f(const std::string& uniformName, GLfloat v0);
	void setUniform2f(const std::string& uniformName, GLfloat v0, GLfloat v1);
	void setUniform3f(const std::string& uniformName, GLfloat v0, GLfloat v1, GLfloat v2);
	void setUniform4f(const std::string& uniformName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

	void setUniform1i(const std::string& uniformName, GLint v0);
	void setUniform2i(const std::string& uniformName, GLint v0, GLint v1);
	void setUniform3i(const std::string& uniformName, GLint v0, GLint v1, GLint v2);
	void setUniform4i(const std::string& uniformName, GLint v0, GLint v1, GLint v2, GLint v3);

	void setUniform1ui(const std::string& uniformName, GLuint v0);
	void setUniform2ui(const std::string& uniformName, GLuint v0, GLuint v1);
	void setUniform3ui(const std::string& uniformName, GLuint v0, GLuint v1, GLuint v2);
	void setUniform4ui(const std::string& uniformName, GLuint v0, GLuint v1, GLuint v2, GLuint v3);

	void setUniform1fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1);
	void setUniform2fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1);
	void setUniform3fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1);
	void setUniform4fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1);

	void setUniform1iv(const std::string& uniformName, const GLint* value, GLsizei count = 1);
	void setUniform2iv(const std::string& uniformName, const GLint* value, GLsizei count = 1);
	void setUniform3iv(const std::string& uniformName, const GLint* value, GLsizei count = 1);
	void setUniform4iv(const std::string& uniformName, const GLint* value, GLsizei count = 1);

	void setUniform1uiv(const std::string& uniformName, const GLuint* value, GLsizei count = 1);
	void setUniform2uiv(const std::string& uniformName, const GLuint* value, GLsizei count = 1);
	void setUniform3uiv(const std::string& uniformName, const GLuint* value, GLsizei count = 1);
	void setUniform4uiv(const std::string& uniformName, const GLuint* value, GLsizei count = 1);

	void setUniformMatrix2fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);
	void setUniformMatrix3fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);
	void setUniformMatrix4fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);

	void setUniformMatrix2x3fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);
	void setUniformMatrix3x2fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);
	void setUniformMatrix2x4fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);
	void setUniformMatrix4x2fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);
	void setUniformMatrix3x4fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);
	void setUniformMatrix4x3fv(const std::string& uniformName, const GLfloat* value, GLsizei count = 1, bool transpose = false);

private:
	GLuint program;

	void compileShader(const std::filesystem::path& shaderPath, GLenum shaderType);
	void linkProgram();
	void deleteProgram();
};
