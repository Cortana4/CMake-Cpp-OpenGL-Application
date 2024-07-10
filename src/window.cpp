#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <GL/glew.h>

#include "window.h"


int Window::instanceCount = 0;

Window::Window(
	int width, int height, const std::string& title,
	bool resizable, bool debugContext
)
	: window{ nullptr }
	, width {width }, height{ height }
	, previousTime{ 0.0 }, deltaTime{ 0.0 }
	, fps{ 0 }
	, keys{}
	, cursorPosX{ 0.0 }, cursorPosY{ 0.0 }
	, scrollOffsetX{ 0.0 }, scrollOffsetY{ 0.0 }
{
	std::stringstream errorMessage;
	errorMessage << "Error: Window::Window(): ";

	// initialize GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		errorMessage << "glfwInit() failed." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	glfwSetErrorCallback(errorCallback);

	// set GLFW options
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	// create window
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (window == nullptr)
	{
		if (instanceCount == 0)
			glfwTerminate();

		errorMessage << "glfwCreateWindow() failed." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	glfwSetWindowUserPointer(window, this);
	glfwMakeContextCurrent(window);

	// set GLFW callback functions
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// set GLFW window options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// initialize GLEW
	// glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(window);
		if (instanceCount == 0)
			glfwTerminate();

		errorMessage << "glewInit() failed." << std::endl;
		throw std::runtime_error(errorMessage.str());
	}

	// set OpenGL callback functions
	if (debugContext)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(debugMessageCallback, this);

		// enable all messages
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	instanceCount++;
}

Window::Window(Window&& other) noexcept
	: window{ other.window }
	, width{ other.width }, height{ other.height }
	, previousTime{ other.previousTime }, deltaTime{ other.deltaTime }
	, fps{ other.fps }
	, keys{ std::move(other.keys) }
	, cursorPosX{ other.cursorPosX }, cursorPosY{ other.cursorPosY }
	, scrollOffsetX{ other.scrollOffsetX }, scrollOffsetY{ other.scrollOffsetY }
{
	other.window = nullptr;
}

Window::~Window()
{
	instanceCount--;

	if (instanceCount == 0)
		glfwTerminate();
	else
		glfwDestroyWindow(window);
}

Window& Window::operator=(Window&& other) noexcept
{
	if (this != &other)
	{
		glfwDestroyWindow(window);

		window = other.window;
		width = other.width;
		height = other.height;
		previousTime = other.previousTime;
		deltaTime = other.deltaTime;
		fps = other.fps;
		keys = std::move(other.keys);
		cursorPosX = other.cursorPosX;
		cursorPosY = other.cursorPosY;
		scrollOffsetX = other.scrollOffsetX;
		scrollOffsetY = other.scrollOffsetY;

		other.window = nullptr;
	}

	return *this;
}

Window::operator GLFWwindow* () const
{
	return window;
}

GLFWwindow* Window::get()
{
	return window;
}

int Window::getWidth() const
{
	return width;
}

int Window::getHeight() const
{
	return height;
}

double Window::getDeltaTime() const
{
	return deltaTime;
}

int Window::getFPS() const
{
	return fps;
}

bool Window::getKeyStatus(int key) const
{
	return keys.at(key);
}

double Window::getCursorPosX() const
{
	return cursorPosX;
}

double Window::getCursorPosY() const
{
	return cursorPosY;
}

double Window::getScrollOffsetX() const
{
	return scrollOffsetX;
}

double Window::getScrollOffsetY() const
{
	return scrollOffsetY;
}

void Window::setSize(int width, int height)
{
	glViewport(0, 0, width, height);
	glfwSetWindowSize(window, width, height);
}

void Window::update()
{
	/* glfwGetTime() returns the time since glfwInit() was called in seconds.
	 * After each frame, update() is called, which increments the frame counter
	 * and updates the current time. When currentFrame - firstFrame >= 1.0,
	 * 1 second has passed and the frame counter contains the number of frames
	 * rendered in that second.
	 */

	static double startTime = glfwGetTime();
	static int frameCounter = 0;

	double currentTime = glfwGetTime();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	frameCounter++;

	if (currentTime - startTime >= 1.0)
	{
		startTime = currentTime;
		fps = frameCounter;
		frameCounter = 0;
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Window::addKeyCallback(std::function<void(Window*)> callback)
{
	keyCallbacks.push_back(callback);
}

void Window::addCursorPosCallback(std::function<void(Window*)> callback)
{
	cursorPosCallbacks.push_back(callback);
}

void Window::addScrollCallback(std::function<void(Window*)> callback)
{
	scrollCallbacks.push_back(callback);
}

void Window::debugMessageCallback(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message
) const
{
	//if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::string sourceStr;
	std::string typeStr;
	std::string severityStr;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		sourceStr = "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		sourceStr = "WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		sourceStr = "SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		sourceStr = "THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		sourceStr = "APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		sourceStr = "OTHER";
		break;
	default:
		sourceStr = "UNKNOWN";
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		typeStr = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeStr = "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeStr = "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typeStr = "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typeStr = "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_MARKER:
		typeStr = "MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		typeStr = "PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		typeStr = "POP_GROUP";
		break;
	case GL_DEBUG_TYPE_OTHER:
		typeStr = "OTHER";
		break;
	default:
		typeStr = "UNKNOWN";
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		severityStr = "HIGH";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		severityStr = "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		severityStr = "LOW";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		severityStr = "NOTIFICATION";
		break;
	default:
		severityStr = "UNKNOWN";
	}

	std::cout
		<< "Info: Window::debugMessageCallback("
		<< "id=0x" << std::setfill('0') << std::setw(8) << std::hex << id
		<< ", source=" << sourceStr
		<< ", type=" << typeStr
		<< ", severity=" << severityStr << "): "
		<< message << std::endl << std::endl;
}

void Window::windowSizeCallback(int width, int height)
{
	this->width = width;
	this->height = height;

	glViewport(0, 0, width, height);
}

void Window::keyCallback(int key, int scancode, int action, int mods)
{
	if (key >= 0 && key <= GLFW_KEY_LAST)
	{
		if (action == GLFW_PRESS)
			keys.at(key) = true;

		else if (action == GLFW_RELEASE)
			keys.at(key) = false;
	}

	if (keys.at(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (keys.at(GLFW_KEY_F1))
	{
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	for (std::function<void(Window*)> callback : keyCallbacks)
		callback(this);
}

void Window::cursorPosCallback(double posX, double posY)
{
	cursorPosX = posX;
	cursorPosY = posY;

	for (std::function<void(Window*)> callback : cursorPosCallbacks)
		callback(this);
}

void Window::scrollCallback(double offsetX, double offsetY)
{
	scrollOffsetX = offsetX;
	scrollOffsetY = offsetY;

	for (std::function<void(Window*)> callback : scrollCallbacks)
		callback(this);
}

void APIENTRY Window::debugMessageCallback(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam
)
{
	const Window* windowHandler = reinterpret_cast<const Window*>(userParam);
	windowHandler->debugMessageCallback(source, type, id, severity, length, message);
}

void Window::errorCallback(int error, const char* description)
{
	std::cerr
		<< "Error: Window::errorCallback(): " << description
		<< " (0x" << std::setfill('0') << std::setw(8) << std::hex << error << ")"
		<< std::endl << std::endl;
}

void Window::windowSizeCallback(GLFWwindow* window, int width, int height)
{
	Window* windowHandler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	windowHandler->windowSizeCallback(width, height);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* windowHandler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	windowHandler->keyCallback(key, scancode, action, mods);
}

void Window::cursorPosCallback(GLFWwindow* window, double posX, double posY)
{
	Window* windowHandler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	windowHandler->cursorPosCallback(posX, posY);
}

void Window::scrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	Window* windowHandler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	windowHandler->scrollCallback(offsetX, offsetY);
}
