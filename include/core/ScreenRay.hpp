#ifndef SCREENRAY_HPP
#define SCREENRAY_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Global.hpp"

class ScreenRay
{
public:
    float *vertices;

    ScreenRay();
    ~ScreenRay();

    void GenerateRay();
};

ScreenRay::ScreenRay()
{
    vertices = new float[Global::PixelCount * 5];
}

ScreenRay::~ScreenRay()
{
    delete[] vertices;
}

void ScreenRay::GenerateRay()
{
	int counter = 0;

	for (int j = 0; j < Global::ScreenHeight; ++j)
	{
		for (int i = 0; i < Global::ScreenWidth; ++i)
		{
			float worldSpaceCoordX = 2 * ((float)i + 0.5) / (float)Global::ScreenWidth - 1;
			float worldSpaceCoordY = 2 * ((float)j + 0.5) / (float)Global::ScreenHeight - 1; // OpenGL 屏幕空间坐标原点在左下角

			float x = worldSpaceCoordX * Global::ImageAspectRatio * Global::Scale;
			float y = worldSpaceCoordY * Global::Scale;
			glm::vec3 dir = normalize(glm::vec3(x, y, 1));

			for (int temp = 0; temp < 3; temp++)
				vertices[counter++] = dir[temp];
			vertices[counter++] = worldSpaceCoordX;
			vertices[counter++] = worldSpaceCoordY;
		}
	}
}

#endif