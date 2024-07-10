#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "textRenderer.h"


int main(int argC, char* argV[])
{
	Window window(800, 800, "OpenGL", false, true);
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Shader mainShader("src/shader/main.vert", "src/shader/main.frag");
	Shader textShader("src/shader/text.vert", "src/shader/text.frag");
	Shader lampShader("src/shader/lamp.vert", "src/shader/lamp.frag");
	TextRenderer textRenderer("resources/font/consola.ttf", 0, 30);
	
	window.addKeyCallback(camera.getKeyCallback());
	window.addCursorPosCallback(camera.getCursorPosCallback());
	window.addScrollCallback(camera.getScrollCallback());

	Model backpack("resources\\objects\\backpack\\backpack.obj");
	Model container("resources\\objects\\container\\container.obj");
	Model lamp("resources\\objects\\lamp\\lamp.obj");

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::vec3 lightPos(0.0f, 0.0f, 3.0f);

	// game loop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
		lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

		// manually call key callback every frame for smother movement
		camera.getKeyCallback()(&window);
		camera.applyTransformation(mainShader, window.getWidth(), window.getHeight());
		camera.applyTransformation(lampShader, window.getWidth(), window.getHeight());


		mainShader.useProgram();
		mainShader.setUniform3f("viewPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		mainShader.setUniform3f("light.position", lightPos.x, lightPos.y, lightPos.z);
		
		// light properties
		mainShader.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
		mainShader.setUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f);
		mainShader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		mainShader.setUniform1f("material.shininess", 64.0f);


		backpack.draw(mainShader);
		lamp.draw(lampShader, lightPos, 0.25f);
		
		textRenderer.renderText(
			textShader,
			std::to_string(window.getFPS()),
			0.0f, static_cast<float>(textRenderer.getMaxNumberHeight() + 1),
			TextRenderer::TOP_LEFT
		);

		window.update();
	}

	return 0;
}
