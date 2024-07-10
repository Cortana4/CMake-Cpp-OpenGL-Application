#pragma once
#include <string>
#include <array>
#include <vector>
#include <utility>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Window
{
public:
	Window(
		int width, int height,
		const std::string& title = "",
		bool resizable = false,
		bool debugContext = true
	);

	Window(const Window& other) = delete;
	Window(Window&& other) noexcept;
	~Window();

	Window& operator=(const Window& other) = delete;
	Window& operator=(Window&& other) noexcept;

	operator GLFWwindow* () const;

	GLFWwindow* get();
	int getWidth() const;
	int getHeight() const;
	double getDeltaTime() const;
	int getFPS() const;
	bool getKeyStatus(int key) const;
	double getCursorPosX() const;
	double getCursorPosY() const;
	double getScrollOffsetX() const;
	double getScrollOffsetY() const;

	void setSize(int width, int height);
	void update();

	void addKeyCallback(std::function<void(Window*)> callback);
	void addCursorPosCallback(std::function<void(Window*)> callback);
	void addScrollCallback(std::function<void(Window*)> callback);

private:
	GLFWwindow* window;

	int width;
	int height;

	double previousTime;
	double deltaTime;
	int fps;

	std::array<bool, GLFW_KEY_LAST+1> keys;
	double cursorPosX;
	double cursorPosY;
	double scrollOffsetX;
	double scrollOffsetY;

	std::vector<std::function<void(Window*)>> keyCallbacks;
	std::vector<std::function<void(Window*)>> cursorPosCallbacks;
	std::vector<std::function<void(Window*)>> scrollCallbacks;

	static int instanceCount;

	// OpenGL callback functions:
	void debugMessageCallback(
		GLenum source, GLenum type,
		GLuint id, GLenum severity,
		GLsizei length, const GLchar* message) const;

	// GLFW callback functions:
	void windowSizeCallback(int width, int height);
	void keyCallback(int key, int scancode, int action, int mods);
	void cursorPosCallback(double posX, double posY);
	void scrollCallback(double offsetX, double offsetY);

	// OpenGL static callback functions:
	static void APIENTRY debugMessageCallback(
		GLenum source, GLenum type,
		GLuint id, GLenum severity,
		GLsizei length, const GLchar* message,
		const void* userParam
	);
	
	// GLFW static callback functions:
	static void errorCallback(int error, const char* description);
	static void windowSizeCallback(GLFWwindow* window, int width, int height);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double posX, double posY);
	static void scrollCallback(GLFWwindow* window, double offsetX, double offsetY);
};
