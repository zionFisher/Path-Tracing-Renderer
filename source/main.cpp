#include "Utility.hpp"
#include "CornellBox.hpp"
#include "FrameSaver.hpp"

// using Utility::camera;
// using Utility::screenRay;

using Global::ScreenWidth;
using Global::ScreenHeight;
using Global::spp;
using Global::ScreenHeight;
using Global::RussianRoulette;
using Global::IndirLightContributionRate;

int main()
{
	GLFWwindow *window = Utility::SetupGlfwAndGlad();

	if (window == nullptr)
		return 0;

	Shader pathTracingShader("SimplePathTracing.vs", "SimplePathTracing.fs");

	FrameSaver image;

	Camera &camera = Utility::camera;

	camera.GenerateRay();

	auto tuple = Utility::SetVAOVBO(camera.vertices);
	unsigned int VAO = std::get<0>(tuple);
	unsigned int VBO = std::get<1>(tuple);
	// auto[VAO, VBO] = Utility::SetVAOVBO(screenRay.vertices); // optional

	pathTracingShader.use();
	pathTracingShader.setInt("spp", spp);
	pathTracingShader.setVec2("Screen", ScreenWidth, ScreenHeight);
	pathTracingShader.setArray("Triangles", sizeof(triangleVertices), const_cast<float *>(triangleVertices));
	pathTracingShader.setFloat("RussianRoulette", RussianRoulette);
	pathTracingShader.setFloat("IndirLightContriRate", IndirLightContributionRate);

	srand(time(NULL));

	glm::mat4 rayRotateMatrix = glm::identity<glm::mat4>();

	// render loop=============================================================
	while (!glfwWindowShouldClose(window))
	{
		Utility::ProcessTime();
		Utility::ProcessInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float seed[4] = {(float)rand() / RAND_MAX, (float)rand() / RAND_MAX,
						 (float)rand() / RAND_MAX, (float)rand() / RAND_MAX};

		rayRotateMatrix = camera.GetRotateMatrix();

		pathTracingShader.use();
		pathTracingShader.setArray("rdSeed", 4, seed);
		pathTracingShader.setMat4("RayRotateMatrix", rayRotateMatrix);
		pathTracingShader.setVec3("Eye", camera.Position.x, camera.Position.y, camera.Position.z);
		// randomNumShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, ScreenWidth * ScreenHeight);

		glfwSwapBuffers(window);
		glfwPollEvents();

		image.SaveBuffer();
	}
	//=========================================================================

	image.SaveImage("result.ppm", PPM);

	glfwTerminate();
	return 0;
}