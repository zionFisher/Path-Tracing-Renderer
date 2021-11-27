#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "global.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "CornellBox.hpp"
#include "FrameSaver.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>

Camera camera(glm::vec3(278, 273, -800));

float x = camera.Position.x;
float y = camera.Position.y;
float z = camera.Position.z;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

float lastX = ScreenWidth / 2.0f;
float lastY = ScreenHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glfwSetCursorPosCallback(window, mouse_callback);
	// glfwSetScrollCallback(window, scroll_callback);

	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	Shader pathTracingShader("SimplePathTracing.vs", "SimplePathTracing.fs");

	Shader randomNumShader("randomNum.vs", "randomNum.fs"); // generate random number test.

	float *vertices = new float[PixelCount * 5];

	int counter = 0;

	for (int j = 0; j < ScreenHeight; ++j)
	{
		for (int i = 0; i < ScreenWidth; ++i)
		{
			float worldSpaceCoordX = 2 * ((float)i + 0.5) / (float)ScreenWidth - 1;
			float worldSpaceCoordY = 2 * ((float)j + 0.5) / (float)ScreenHeight - 1; // OpenGL 屏幕空间坐标原点在左下角
			float x = worldSpaceCoordX * imageAspectRatio * scale;
			float y = worldSpaceCoordY * scale;
			glm::vec3 dir = normalize(glm::vec3(x, y, 1));

			for (int temp = 0; temp < 3; temp++)
				vertices[counter++] = dir[temp];
			vertices[counter++] = worldSpaceCoordX;
			vertices[counter++] = worldSpaceCoordY;
		}
	}

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, ScreenHeight * ScreenWidth * sizeof(float) * 5, vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	pathTracingShader.use();
	pathTracingShader.setInt("spp", spp);
	pathTracingShader.setVec2("Screen", ScreenWidth, ScreenHeight);
	pathTracingShader.setVec3("Eye", camera.Position.x, camera.Position.y, camera.Position.z);
	pathTracingShader.setArray("Triangles", sizeof(triangleVertices), const_cast<float *>(triangleVertices));
	pathTracingShader.setFloat("RussianRoulette",RussianRoulette);
	pathTracingShader.setFloat("IndirLightContributionRate", IndirLightContributionRate);

	randomNumShader.use();
	randomNumShader.setFloat("time", time(0));

	int frameCounter = 0;

	FrameSaver image;

	srand(time(NULL));

	while (!glfwWindowShouldClose(window))
	{
		std::cout << "Frame counts: " << frameCounter << std::endl;
		frameCounter++;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float seed[4] = { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,
						  (float)rand() / RAND_MAX, (float)rand() / RAND_MAX };

		// float x = 273 + 100 * sin(glfwGetTime());
		// float z = -800 + 200 * abs(sin(glfwGetTime()));

		pathTracingShader.use();
		pathTracingShader.setArray("rdSeed", 4, seed);
		pathTracingShader.setVec3("Eye", x, y, z);
		// randomNumShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, ScreenWidth * ScreenHeight);

		glfwSwapBuffers(window);
		glfwPollEvents();

		image.SaveBuffer();
	}

	image.SaveImage("result.ppm", ImageType::PPM);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		z += 10;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		z -= 10;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		x -= 10;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		x += 10;
}

//callbacks----------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const *path)
{
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