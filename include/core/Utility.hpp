#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Global.hpp"
#include "Camera.hpp"
#include "ScreenRay.hpp"
#include "shader.hpp"
#include "model.hpp"

namespace Utility
{
	// Variables-------------------------------------------------------------------

	// camera and screen ray generater
	Camera camera(glm::vec3(Global::OriginX, Global::OriginY, Global::OriginZ));

	ScreenRay screenRay;

	// coords and time
	float lastX = Global::ScreenWidth / 2.0f;
	float lastY = Global::ScreenHeight / 2.0f;
	bool firstMouse = true;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Declaration-----------------------------------------------------------------

	// Set Up functions
	GLFWwindow *SetupGlfwAndGlad();

	GLFWwindow *InitGlfwAndCreateWindow();

	bool SetCallback(GLFWwindow *window);

	bool InitGlad();

	std::tuple<unsigned int, unsigned int> SetVAOVBO(float *vertices);

	// Process and Callbacks
	void ProcessInput(GLFWwindow *window);

	void ProcessTime();

	void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

	void MouseCallback(GLFWwindow *window, double xpos, double ypos);

	void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

	// Other Utility
	unsigned int LoadTexture(char const *path);

	// Implementations-------------------------------------------------------------

	// Set Up functions
	GLFWwindow *SetupGlfwAndGlad()
	{
		GLFWwindow *window = InitGlfwAndCreateWindow();

		if (window == nullptr)
			return nullptr;

		bool flag = SetCallback(window);

		if (!flag)
			return nullptr;

		flag = InitGlad();

		if (!flag)
			return nullptr;

		return window;
	}

	GLFWwindow *InitGlfwAndCreateWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GLFWwindow *window = glfwCreateWindow(Global::ScreenWidth, Global::ScreenHeight, Global::WindowName, NULL, NULL);

		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return nullptr;
		}

		glfwMakeContextCurrent(window);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		return window;
	}

	bool SetCallback(GLFWwindow *window)
	{
		glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
		glfwSetCursorPosCallback(window, MouseCallback);
		// glfwSetScrollCallback(window, ScrollCallback);

		return true;
	}

	bool InitGlad()
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}
		else
			return true;
	}

	std::tuple<unsigned int, unsigned int> SetVAOVBO(float *vertices)
	{
		unsigned int VAO;
		unsigned int VBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, Global::ScreenHeight * Global::ScreenWidth * sizeof(float) * 5, vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		return std::tuple<unsigned int, unsigned int>{VAO, VBO};
	}

	// Process and Callbacks
	void ProcessInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	void ProcessTime()
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void MouseCallback(GLFWwindow *window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	// void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
	// {
	// 	   camera.ProcessMouseScroll(yoffset);
	// }

	// Other Utility
	unsigned int LoadTexture(char const *path)
	{
		// stbi_set_flip_vertically_on_load(true);

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
}

#endif